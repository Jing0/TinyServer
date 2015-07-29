#include <cstdio>
#include <cstdlib>
#include <map>
#include <cstring>
#include <string>
#include <cstddef>
#include <sys/socket.h>

using namespace std;

void extra_fields(char *str, map<string, string> &mp)
{
    char *pch = strtok(p, "&");
    while (pch) {
        p = strchr(pch, '=');
        *p = '\0';
        mp[pch] = p + 1;
        pch = strtok(NULL, "&");
    }
}

/*
 * buf stores HTTP request message, length is the message's length
 * query is the field wanted to look up
 * rst is a buffer to store the look up result
 * */

extern "C" void http_analysis(const char *buf, size_t length, const char *query, char *rst);
void http_analysis(const char *buf, size_t length, const char *query, char *rst)
{
    size_t i = 0, j = 0, skip_n;
    char *str = (char *) malloc(length + 1);
    static map<string, string> mp; 
    string key;
    static const char *request_line[] = {
        "Method",
        "Url",
        "Version"
    };

    /*
     * Extract the request line
     * */
    for (int cnt = 0; cnt < 3; ++cnt) {
        sscanf(buf + i, "%s%n", str, &skip_n);
        i += skip_n;
        if (1 == cnt) {
            char *p = strchr(str, '?');
            *p = '\0';
            mp["Query"] = ++p;
        }
        mp[request_line[cnt]] = str;

        if (1 == cnt) { // Url decompose
            char *p = strchr(str, '?');
            if (NULL == p) continue;
            ++p;
            extra_fields(str, mp);
        }
    }
    i += 2; // Skip "\r\n"

    /*
     * Handle header lines
     * */
    while (buf[i] != '\r') {
        j = 0;
        while (buf[i] != '\r') {
            if (buf[i] == ':') {
                str[j] = '\0';
                key = string(str);
                j = 0;
                i += 2; // Skip ": "
            } else {
                str[j++] = buf[i++];
            }
        }
        str[j] = '\0';
        i += 2;     // Skip "\r\n"
        mp[key] = str;
    }

    if (!strcmp("POST", mp["Method"].c_str())) {
        strcpy(str, buf + i + 2);
        mp["post_info"] = str;
    }

    free(str);

    /************************************/

    strcpy(rst, mp[query].c_str());

    return;
}

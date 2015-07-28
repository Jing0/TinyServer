#include <cstdio>
#include <cstdlib>
#include <map>
#include <cstring>
#include <string>
#include <cstddef>
#include <sys/socket.h>

using namespace std;

void http_analysis(map<string, string> &mp, int fd, size_t length)
{
    size_t i = 0, j = 0, skip_n;
    char *str = (char *) malloc(length + 1);
    char *buf = (char *) malloc(length + 1);
    recv(fd, buf, length, 0);
    string key;
    static const char *request_line[] = {
        "Method",
        "URL",
        "Version"
    };

    /*
     * Extract the request line
     * */
    for (int cnt = 0; cnt < 3; ++cnt) {
        sscanf(buf + i, "%s%n", str, &skip_n);
        i += skip_n;
        mp[request_line[cnt]] = str;

        if (1 == cnt) { // Url decompose
            char *p = strchr(str, '?');
            if (NULL == p) continue;
            ++p;
            char *pch = strtok(p, "&");
            while (pch) {
                p = strchr(pch, '=');
                *p = '\0';
                mp[pch] = p + 1;
                pch = strtok(NULL, "&");
            }
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

    free(str);
    free(buf);

    return;
}

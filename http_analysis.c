#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Link_Node {
    char *key, *value;
    struct Link_Node *next;
} Link_Node;

/*
void traverse_link(Link_Node *node)
{
    for ( ; node; node = node->next) {
        printf("%s: %s\n", node->key, node->value);
    }
}
*/

/*
 * The first node in Link_Node is a head node which is not for storing data.
 * */
Link_Node *node_create()
{
    Link_Node *node = (Link_Node *) malloc(sizeof(Link_Node));

    node->next = NULL;
    node->key = NULL;
    node->value = NULL;

    return node;
}

void node_insert(Link_Node *root, const char *key, const char *value)
{
    Link_Node *node = (Link_Node *) malloc(sizeof(Link_Node));

    node->key = (char *) malloc(strlen(key) + 1);
    strcpy(node->key, key);

    node->value = (char *) malloc(strlen(value) + 1);
    strcpy(node->value, value);

    node->next = root->next;
    root->next = node;
}

void node_find(Link_Node *node, const char *key, char *buf)
{
    Link_Node *p_node = node;

    for ( ; p_node != NULL; p_node = p_node->next) {
        if (p_node->key && !strcmp(p_node->key, key)) {
            strcpy(buf, p_node->value);
            return;
        }
    }

    buf[0] = '\0';
}

void link_destory(Link_Node *node)
{
    if (node == NULL) return;

    link_destory(node->next);
    free(node->key);
    free(node->value);
    free(node);
}

void extra_fields(Link_Node *root, char *str)
{
    char *p = str;
    char *pch = strtok(p, "&");
    while (pch) {
        p = strchr(pch, '=');
        *p = '\0';
        node_insert(root, pch, p + 1);
        pch = strtok(NULL, "&");
    }
}

/*
 * buf stores HTTP request message, length is the message's length
 * query is the field wanted to look up
 * rst is a buffer to store the look up result
 * */
int http_analysis(const char *buf, size_t length, const char *query, char *rst)
{
    if (0 == length) return -1;
    if (strlen(buf) != length) return -1;
    if (query == NULL || rst == NULL) return -1;

    size_t i = 0, j = 0;
    int skip_n;
    char *str = (char *) malloc(length + 1);
    Link_Node *root = node_create();
    static const char *request_line[] = {
        "Method",
        "Url",
        "Version"
    };

    /*
     * Extract the request line
     * */
    int cnt;
    for (cnt = 0; i < length && cnt < 3; ++cnt) {
        sscanf(buf + i, "%s%n", str, &skip_n);
        i += skip_n;
        if (i >= length) return -1;
        if (1 == cnt) {
            char *p = strchr(str, '?');
            if (p) {
                *p = '\0';
                node_insert(root, "Query", ++p);
            }
        }
        node_insert(root, request_line[cnt], str);

        if (1 == cnt) { // Url decompose
            char *p = strchr(str, '?');
            if (NULL == p) continue;
            ++p;
            extra_fields(root, str);
        }
    }
    i += 2; // Skip "\r\n"
    if (i >= length) return -1;
    /*
     * Handle header lines
     * */
    char key[1024];
    int maohao = 0;
    while (i < length && buf[i] != '\r') {
        j = 0;
        maohao = 0;
        while (buf[i] != '\r') {
            if (buf[i] == ':' && !maohao++) {
                str[j] = '\0';
                strcpy(key, str);
                j = 0;
                i += 2; // Skip ": "
                if (i >= length) return -1;
            } else {
                str[j++] = buf[i++];
            }
        }
        str[j] = '\0';
        i += 2;     // Skip "\r\n"
        if (i >= length) return -1;
        node_insert(root, key, str);
    }
    node_find(root, "Method", rst);
    if (!strcmp("POST", rst)) {
        strcpy(str, buf + i + 2);
        node_insert(root, "post_info", str);
    }
    // traverse_link(root);

    node_find(root, query, rst);

    link_destory(root);
    free(str);

    return 0;
}

/*
int main()
{
    const char buf[] = 
        "POST /../../../etc/passwd HTTP/1.1\r\n"
        "Host: acm.hdu.edu.cn\r\n"
        "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:39.0) Gecko/20100101 Firefox/39.0\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.5\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "Referer: http://acm.hdu.edu.cn/userloginex.php?action=login\r\n"
        "Cookie: CNZZDATA1254072405=1655252587-1437546803-7C1437959767; PHPSESSID=9tr1jmqbcrc3etacct1lqpvju7\r\n"
        "Connection: keep-alive\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 45\r\n\r\n"
        "username=changmu&userpass=chang&login=Sign+In";

    char rst[1024] = "", buff[100];
    while (~scanf("%s", buff)) {
        http_analysis(buf, 0, buff, rst);
        puts(rst);
    }
    return 0;
}
*/

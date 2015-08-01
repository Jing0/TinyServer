#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "config.h"

#define strEqual(str1, str2) (strcmp((str1), (str2)) == 0)
#define lastChar(str) str[strlen(str)-1]

#define BUFSIZE         8192
#define MAX_URI         280
#define MAX_PATH        257
#define MAX_LINE        300
#define CONFIG_PATH     "./config.ini"

typedef struct req_t {
    char method[10];
    char uri[MAX_URI];
    char version[10];
    char host[100];
} req_t;

typedef struct serverConfig {
    int port;
    int backlog;
    char indexPage[MAX_URI];
    char errPath[MAX_URI];
} serverConfig;

serverConfig gConfig; /* global server configuration */

void saferfree(void **pp) {
    if (pp != NULL && *pp != NULL) {
        free(*pp);
        *pp = NULL;
    }
}

int getSubstr(const char *str, int pos, char end, char *substr) {
    int i, len = strlen(str);
    for (i = pos; str[i] != end && i < len; ++i) {
        *(substr + i - pos) = *(str + i);
    }
    *(substr + i - pos) = '\0';
    return i;
}

/* get extension name of a file */
int getExtName(const char *fileName, char *extName) {
    char *start = strrchr(fileName, '.');
    if (start == NULL) {
        return -1;
    }
    strcpy(extName, start + 1);
    return 0;
}

void clientError(int socket, int status, const char *msg) {
    char buf[BUFSIZE], path[MAX_PATH];

    sprintf(buf, "HTTP/1.1 %d %s\r\n", status, msg);
    sprintf(buf + strlen(buf), "Content-Type: text/html\r\n\r\n");

    sprintf(path, "%s%d.html", gConfig.errPath, status);
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        perror("File open error");
    }
    fgets(buf + strlen(buf), sizeof(buf), fp);
    write(socket, buf, strlen(buf));
    fclose(fp);
}

void success(int socket, int status, const char *msg, FILE *fp, char *extName) {
    char buf[BUFSIZE], line[MAX_LINE];

    sprintf(buf, "HTTP/1.1 %d %s\r\n", status, msg);
    sprintf(buf + strlen(buf), "Content-Type: ");
    if (strEqual(extName, "html")) {
        sprintf(buf + strlen(buf), "text/html");
    }
    sprintf(buf + strlen(buf), "\r\n\r\n");

    while (fgets(line, MAX_LINE - 1, fp) != NULL) {
        sprintf(buf + strlen(buf), "%s", line);
    }
    write(socket, buf, strlen(buf));
}

int dealWithRequest(int socket, const req_t req) {
    int status = 200;
    char path[MAX_PATH], query[MAX_URI], msg[50];
    strcpy(msg, "OK");
    {
        int i = getSubstr(req.uri, 1, '?', path);
        printf("path: %s\n", path);
        ++i;
        i = getSubstr(req.uri, i, '\0', query);
    }
    if (strEqual(req.method, "GET")) {
        FILE *fp = fopen(path, "r");
        if (lastChar(path) == '/' || (strEqual(path, "") && fp == NULL)) {
            sprintf(path + strlen(path), "%s", gConfig.indexPage);
            fp = fopen(path, "r");
        }
        if (fp == NULL) {
            status = 404;
            strcpy(msg, "Not Found");
        }
        if (status == 200) {
            char extName[MAX_PATH];
            getExtName(path, extName);
            success(socket, status, msg, fp, extName);
        } else {
            clientError(socket, status, msg);
        }
        fclose(fp);
    } else if (strEqual(req.method, "POST")) {
        ;
    }
    return status;
}

int parseRequest(const char *request_buf, req_t *req) {
    char tmp[MAX_URI * 2];
    int i;
    i = getSubstr(request_buf, 0, ' ', tmp);
    strcpy(req->method, tmp);
    ++i;
    i = getSubstr(request_buf, i, ' ', tmp);
    if (strlen(tmp) >= MAX_URI) {
        return -1;
    }
    strcpy(req->uri, tmp);
    ++i;
    i = getSubstr(request_buf, i, '\r', tmp);
    strcpy(req->version, tmp);
    i += 8;
    i = getSubstr(request_buf, i, '\r', tmp);
    strcpy(req->host, tmp);
    return 0;
}

int processSocket(
    int create_socket, struct sockaddr *addr, socklen_t *addr_len, char *request_buf) {
    int pid = fork();
    if (pid < 0) {
        perror("Failed to create process");
        return 1;
    }

    int new_socket = accept(create_socket, addr, addr_len);
    if (new_socket < 0) {
        perror("Server: accept");
        return 1;
    } else {
        printf("%s : %d connected\n",
               inet_ntoa(((struct sockaddr_in *)addr)->sin_addr),
               ntohs(((struct sockaddr_in *)addr)->sin_port));
    }

    memset(request_buf, 0, BUFSIZE * sizeof(char));
    if (recvfrom(new_socket, request_buf, BUFSIZE, 0, NULL, NULL) < 0) {
        perror("Server: receive");
    }

    req_t req;
    int parseRes = parseRequest(request_buf, &req);
    if (parseRes == 0) {
        dealWithRequest(new_socket, req);
    } else if (parseRes == -1) {
        clientError(new_socket, 414, "Request-URI Too Long");
    }

    close(new_socket);
    printf("%s : %d disconnected\n",
               inet_ntoa(((struct sockaddr_in *)addr)->sin_addr),
               ntohs(((struct sockaddr_in *)addr)->sin_port));
    return 0;
}

int initialize() {
    config_t *config = config_new(CONFIG_PATH);
    if (config == NULL) {
        fprintf(stderr, "%s\n",
                "Failed to read initialization file.\n"
                "Default configuration will be used");
        return -1;
    }
    config_getInt(config, "DATA", "port", &gConfig.port);
    config_getInt(config, "DATA", "backlog", &gConfig.backlog);
    config_getString(config, "PATH", "index", gConfig.indexPage);
    config_getString(config, "PATH", "errPath", gConfig.errPath);
    config_free(config);
    return 0;
}

int main(int argc, char const *argv[]) {
    socklen_t addr_len;
    struct sockaddr_in addr;
    int create_socket, reuse = 0;

    initialize();
    printf("port: %d\n", gConfig.port);
    printf("backlog: %d\n", gConfig.backlog);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(gConfig.port);

    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        printf("The socket was created : %d\n", create_socket);
    }
    if (setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Setsockopt error");
    }
    if (bind(create_socket, (struct sockaddr *) &addr, sizeof(addr)) == 0) {
        printf("Binding Socket...\n");
    } else {
        perror("Failed to bind");
    }
    if (listen(create_socket, gConfig.backlog) < 0) {
        perror("Server: listen");
    }

    char *request_buf = malloc(BUFSIZE);
    while (1) {
        processSocket(create_socket, (struct sockaddr *) &addr, &addr_len, request_buf);
    }
    saferfree((void **) &request_buf);
    close(create_socket);
    return 0;
}
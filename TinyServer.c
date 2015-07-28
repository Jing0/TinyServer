#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "config.h"

#define BUFSIZE 8192
/* BACKLOG defines the maximum length for the queue of pending connections */
#define BACKLOG 10
#define DEFAULT_PORT 8080

void saferfree(void **pp) {
    if (pp != NULL && *pp != NULL) {
        free(*pp);
        *pp = NULL;
    }
}

const char response[] = "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<!DOCTYPE html><html>"
    "<head><meta content=\"text/html; charset=utf-8\"><title>Hello</title></head>"
    "<body><h2>Hello, world!</h2></body></html>\r\n";

int processSocket(int create_socket, struct sockaddr *addr, socklen_t *addr_len, char *request_buf) {
    int new_socket = accept(create_socket, addr, addr_len);
    if (new_socket < 0) {
        perror("server: accept");
        return 1;
    } else {
        printf("The Client is connected...\n");
    }

    memset(request_buf, 0, BUFSIZE * sizeof(char));
    /* recv() is identical to recvfrom() with a null pointer passed as its address argument.
    As it is redundant, it may not be sup-ported supported in future releases. */
    if (recvfrom(new_socket, request_buf, BUFSIZE, 0, NULL, NULL) < 0) {
        perror("server: receive");
    }

    write(new_socket, response, sizeof(response) - 1);
    close(new_socket);
    return 0;
}

int initialize(int *port) {
    config_t *config = config_new("./config.ini");
    if (config == NULL) {
        fprintf(stderr, "%s\n", "Failed to read initialization file");
        return -1;
    }
    *port = config->get_port(config);
    config->delete(config);
    return 0;
}

int main() {
    int create_socket;
    int reuse = 0;    
    socklen_t addr_len;
    struct sockaddr_in addr;
    char *request_buf = malloc(BUFSIZE);
    int port = DEFAULT_PORT;
    
    initialize(&port);
    printf("port: %d\n", port);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        printf("The socket was created : %d\n", create_socket);
    }
    if (setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Setsockopt error\n");
    }
    if (bind(create_socket, (struct sockaddr *) &addr, sizeof(addr)) == 0) {
        printf("Binding Socket\n");
    } else {
        perror("Failed to bind\n");
    }
    if (listen(create_socket, BACKLOG) < 0) {
        perror("server: listen");
    }

    while (1) {
        processSocket(create_socket, (struct sockaddr *)&addr, &addr_len, request_buf);
        printf("%s\n", request_buf);
        int len = strlen(request_buf);
        printf("%d\n", len); 
    }
    saferfree((void **) &request_buf);
    close(create_socket);
    return 0;
}

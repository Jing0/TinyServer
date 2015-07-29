#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "config.h"
#include "request.h"

#define BUFSIZE 1024
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

int processSocket(int create_socket, struct sockaddr *address, socklen_t *address_len, char **request_buffer) {
    int new_socket = accept(create_socket, address, address_len);
    if (new_socket < 0) {
        perror("server: accept");
        return 1;
    } else if (new_socket > 0) {
        printf("The Client is connected...\n");
    }

    char *buff = malloc(BUFSIZE);
    /* recv() is identical to recvfrom() with a null pointer passed as its address argument.
    As it is redundant, it may not be sup-ported supported in future releases. */
    if (recvfrom(new_socket, buff, BUFSIZE, 0, NULL, NULL) < 0) {
        perror("server: receive");
    }
    *request_buffer = buff;
    
    //show message
    request(new_socket, *request_buffer);
    
    saferfree((void **) &buff);

    //write(new_socket, response, sizeof(response) - 1);
    close(new_socket);
    return 0;
}

int initialize(int *port) {
    config_t *config = config_new("./config.ini");
    if (config == NULL) {
        fprintf(stderr, "%s\n", "Failed to read initialization file");
        config->delete(config);
        return -1;
    }
    *port = config->get_port(config);
    config->delete(config);
    return 0;
}

int main() {
    int create_socket;
    int reuse = 0;    
    socklen_t address_len;
    struct sockaddr_in address;
    char *request_buffer;
    int port = DEFAULT_PORT;
    
    initialize(&port);
    printf("port: %d\n", port);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        printf("The socket was created : %d\n", create_socket);
    }
    if (setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Setsockopt error\n");
    }
    if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0) {
        printf("Binding Socket\n");
    } else {
        perror("Failed to bind\n");
    }
    if (listen(create_socket, BACKLOG) < 0) {
        perror("server: listen");
    }

    while (1) {
        processSocket(create_socket, (struct sockaddr *)&address, &address_len, &request_buffer);
        
        //printf("%s\n", request_buffer);
        
        int len = strlen(request_buffer);
        printf("%d\n", len);
    }
    saferfree((void **) &request_buffer);
    close(create_socket);
    return 0;
}

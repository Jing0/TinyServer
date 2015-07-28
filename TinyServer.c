#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFSIZE 1024
/* BACKLOG defines the maximum length for the queue of pending connections */
#define BACKLOG 10

const char response[] = "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n\r\n"
    "<!DOCTYPE html><html>"
    "<head><meta content=\"text/html; charset=utf-8\"><title>Hello</title></head>"
    "<body><h2>Hello, world!</h2></body></html>\r\n";

int processSocket(int create_socket, struct sockaddr *address, socklen_t *address_len) {
    if (listen(create_socket, BACKLOG) < 0) {
        perror("server: listen");
        return 1;
    }
    
    int new_socket = accept(create_socket, address, address_len);

    if (new_socket < 0) {
        perror("server: accept");
        return 1;
    } else if (new_socket > 0){
        printf("The Client is connected...\n");
    }

    /* recv() is identical to recvfrom() with a null pointer passed as its address argument.
    As it is redundant, it may not be sup-ported supported in future releases. */
    recvfrom(new_socket, NULL, BUFSIZE, 0, NULL, NULL); 
    write(new_socket, response, sizeof(response) - 1);
    close(new_socket);
    return 0;
}

int main() {
    int create_socket;
    int default_port = 8080;
    socklen_t address_len;
    struct sockaddr_in address;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(default_port);

    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        printf("The socket was created : %d\n", create_socket);
    }

    if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0) {
        printf("Binding Socket\n");
    } else {
        perror("Failed to bind\n");
    }

    while (1) {
        processSocket(create_socket, (struct sockaddr *)&address, &address_len);
    }

    close(create_socket);
    return 0;
}
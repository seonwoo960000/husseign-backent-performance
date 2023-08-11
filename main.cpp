#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// max application buffer size
#define APP_MAX_BUFFER 1024
#define PORT 8080


int main() {
    // server_fd: listening socket fd, client_fd: connection from client
    int server_fd, client_fd;

    struct sockaddr_in address;
    int address_len = sizeof(address);

    char buffer[APP_MAX_BUFFER] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; // ipv4
    address.sin_addr.s_addr = INADDR_ANY; // listen 0.0.0.0 interfaces
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Creates the queues
    // 10 is the backlog: number of connections that can be stored in the queue
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while (true) {
        printf("\nWaiting for connection...\n");

        // if the accept queue is empty, we are stuck here..
        if ((client_fd == accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &address_len)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // reads from OS receive buffer to the application
        read(client_fd, buffer, APP_MAX_BUFFER);
        printf("%s\n", buffer);

        char *http_response = "HTTP/1.1 200 OK\n"
                              "Content-Type: text/plain\n"
                              "Content-Length: 13\n\n"
                              "Hello World!\n";

        // writing to send queue in OS
        write(client_fd, http_response, strlen(http_response));

        // terminate client socket
        close(client_fd);
    }
    return 0;
}

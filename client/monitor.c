#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MESSAGE_SIZE 200

int main() {
    int monitor_socket;
    struct sockaddr_in server;

    // Create socket
    monitor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (monitor_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Use the correct IP address of the server
    server.sin_port = htons(8888);

    // Connect to the server
    if (connect(monitor_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server on socket %d\n", monitor_socket);

    char message[MESSAGE_SIZE];

    while (recv(monitor_socket, message, MESSAGE_SIZE, 0) > 0) {
        printf("Received message: %s", message);
    }

    close(monitor_socket);

    return 0;
}
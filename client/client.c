#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server\n");

    // Handle communication with the server
    while (1) {
        printf("Enter message (type 'exit' to quit): ");
        fgets(buffer, sizeof(buffer), stdin);
        send(clientSocket, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Exiting...\n");
            break;
        }
    }

    close(clientSocket);

    return 0;
}

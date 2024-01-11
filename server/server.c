#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 5

void *handleClient(void *socketDesc) {
    int clientSocket = *(int *)socketDesc;
    char buffer[1024];
    struct sockaddr_in clientAddr;
    socklen_t addrSize = sizeof(clientAddr);

    getpeername(clientSocket, (struct sockaddr *)&clientAddr, &addrSize);
    printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        if (recv(clientSocket, buffer, sizeof(buffer), 0) <= 0) {
            printf("Client %s:%d disconnected\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            close(clientSocket);
            pthread_exit(NULL);
        }

        if (strcmp(buffer, "exit") == 0) {
            printf("Client %s:%d disconnected\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
            close(clientSocket);
            pthread_exit(NULL);
        }

        printf("Client %s:%d says: %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buffer);
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(clientAddr);
    pthread_t thread;

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, MAX_CLIENTS) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept connections from clients and create a new thread for each
    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Create a new thread to handle the client
        if (pthread_create(&thread, NULL, handleClient, (void *)&clientSocket) != 0) {
            perror("Error creating thread");
            close(clientSocket);
            continue;
        }

        // Detach the thread to allow it to run independently
        pthread_detach(thread);
    }

    close(serverSocket);

    return 0;
}


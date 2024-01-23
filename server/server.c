#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define USERNAME_SIZE 20
#define TIMESTAMP_SIZE 10
#define MESSAGE_SIZE 200
#define FORMATTED_MESSAGE_SIZE (USERNAME_SIZE + TIMESTAMP_SIZE + MESSAGE_SIZE + 3)

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888

int client_sockets[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(char *message, int sender_socket) {
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0 && client_sockets[i] != sender_socket) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }

    pthread_mutex_unlock(&mutex);
}

void *client_handler(void *socket_ptr) {
    int client_socket = *(int *)socket_ptr;
    char* message = (char*) calloc(FORMATTED_MESSAGE_SIZE, sizeof(char));

    printf("Client connected on socket %d\n", client_socket);

    while (recv(client_socket, message, FORMATTED_MESSAGE_SIZE, 0) > 0) {
        printf("Received message from socket %d: %s", client_socket, message);
        broadcast(message, client_socket);
        memset(message, 0, FORMATTED_MESSAGE_SIZE);
    }

    // Client disconnected
    printf("Client on socket %d disconnected\n", client_socket);

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_socket) {
            client_sockets[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket, client_socket, c;
    struct sockaddr_in server, client;

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(SERVER_PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    listen(server_socket, 3);

    printf("Server listening on port 8888...\n");

    c = sizeof(struct sockaddr_in);

    pthread_t thread;
    int new_socket;

    // Initialize client sockets
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    while ((new_socket = accept(server_socket, (struct sockaddr *)&client, (socklen_t *)&c))) {
        pthread_create(&thread, NULL, client_handler, (void *)&new_socket);

        // Add new client to the array
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = new_socket;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    if (new_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    close(server_socket);

    return 0;
}

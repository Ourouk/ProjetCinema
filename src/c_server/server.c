#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_PAYLOAD_SIZE 1024

struct payload {
    uint16_t payload_type;
    uint16_t payload_length;
    char payload[MAX_PAYLOAD_SIZE];
};

void serialize_payload(const struct payload *msg, char *buffer)
{

}
void deserialize_payload(const char *buffer, struct payload *msg)
{
    
}


// Warning do not strtok but strtok_r because static variable would cause problems
void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[1024]; // Assuming a maximum message size of 1024 bytes

    while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            // Connection closed or error occurred
            if (bytes_received == 0) {
                printf("Client disconnected\n");
            } else {
                perror("Error receiving data");
            }
            break;
        }
        strtok_r(buffer, "\n", &buffer);
        // Process the received data (in 'buffer')
        // For example, you can print it
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        printf("Received: %s", buffer);
    }

    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // Choose a port (in this example, 8080)
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on any available interface

    // Bind the socket to the specified address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 10) == -1) { // The second argument is the backlog queue size
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Accept incoming connections
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    while(1)
    {
        int client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sockfd == -1) {
            perror("accept");
            close(sockfd);
            exit(EXIT_FAILURE);
        }else
        {
            //At demand Handling
            printf("Client connected\n");
            pthread_t handle_client_thread;
            int result = pthread_create(&handle_client_thread, NULL, handle_client, &client_sockfd);
            if (result != 0) {
                perror("Thread creation failed");
                return 1;
            }
        }
    }
    return 0;
}

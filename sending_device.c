#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

void sendCommand(int socket, const char* command) {
    if (send(socket, command, strlen(command), 0) < 0) {
        perror("Error sending command");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int socketDescriptor;
    struct sockaddr_in serverAddress;
    char command[MAX_BUFFER_SIZE];

    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // Replace with the server's port number
    serverAddress.sin_addr.s_addr = inet_addr("SERVER_IP_ADDRESS"); // Replace with the server's IP address

    if (connect(socketDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    printf("Enter the command to send: ");
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0'; // Remove trailing newline character

    sendCommand(socketDescriptor, command);

    close(socketDescriptor);

    return 0;
}

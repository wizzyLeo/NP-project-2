#include "Server.h"
#include <unistd.h>
#include <cstring>  // For memset
#include <fcntl.h>  // For O_CLOEXEC

Server::Server(int port) : serverPort(port) {
    createSocket();
    setServerAddrAndPort();
    bindSocket();
    startListening();
}

void Server::createSocket() {
    serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }
}

void Server::setServerAddrAndPort() {
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);  // Accept connections on any IP
    serverAddr.sin_port = htons(serverPort);
}

void Server::bindSocket() {
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }
}

void Server::startListening() {
    if (listen(serverSocket, 5) < 0) {  // Up to 5 pending connections allowed
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

void Server::start() {
    FD_ZERO(&master_fds);
    FD_SET(serverSocket, &master_fds);
    int max_fd = serverSocket;

    while (true) {
        memcpy(&working_fds, &master_fds, sizeof(master_fds));
        if (select(max_fd + 1, &working_fds, NULL, NULL, NULL) < 0) {
            perror("Select error");
            continue;
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &working_fds)) {
                if (i == serverSocket) {
                    handleNewConnection();
                } else {
                    if (processRequest(i)) {  // If true, client wants to close connection
                        close(i);
                        FD_CLR(i, &master_fds);
                    }
                }
            }
        }
    }
}

void Server::handleNewConnection() {
    struct sockaddr_in clientAddr;
    socklen_t addrSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);

    if (clientSocket < 0) {
        perror("Accept failed");
        return;
    }

    FD_SET(clientSocket, &master_fds);
    broadcastMessage("New user connected");
}

void Server::broadcastMessage(const std::string& message) {
    for (int i = 0; i <= FD_SETSIZE; i++) {
        if (FD_ISSET(i, &master_fds) && i != serverSocket) {
            dprintf(i, "%s\n", message.c_str());
        }
    }
}

bool Server::processRequest(int clientSocket) {
    char buffer[1024];
    int nbytes = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (nbytes <= 0) {
        if (nbytes == 0) {
            // Connection closed
            printf("Client %d disconnected.\n", clientSocket);
        } else {
            perror("recv - failed");
        }
        return true;
    }

    buffer[nbytes] = '\0';
    Shell shell(clientSocket);
    shell.runCommand(std::string(buffer));  // Assuming Shell has a runCommand method
    return false;
}

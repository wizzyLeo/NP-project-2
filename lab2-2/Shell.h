#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unordered_map>
#include "Shell.h"

#define PORT 7001

class Server {
private:
    int serverSocket;
    struct sockaddr_in serverAddr;
    int serverPort = PORT;
    fd_set master_fds, working_fds; // fd_set for select()

    void createSocket();
    void setServerAddrAndPort();
    void bindSocket();
    void startListening();
    void handleNewConnection();
    void processRequest(int clientSocket);
    void broadcastMessage(const std::string& message);

public:
    Server(int port = PORT);
    void start();
    ~Server();
};

#endif // SERVER_H

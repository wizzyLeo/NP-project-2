#ifndef SERVER_H
#define SERVER_H

#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 7001


class Server{
    private:
        int serverSocket;
        struct sockaddr_in serverAddr;
        int serverPort;

        void createSocket();
        void setServerAddrAndPort();
        void bindSocket();
        void startListening();
        void startAccepting();
        void processRequest(int clientSocket);
    public:
        Server();
        void start();
};

#endif
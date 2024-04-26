#ifndef SERVER_H
#define SERVER_H

#include<iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sstream>
#include <set>
#include <map>
#include <sys/socket.h>
#include <sys/select.h>
#include <thread>

#define MAX_USER_ID 30
#define DEFAULT_USER_NAME "(no name)"
#define PORT 7001


class Server{
    private:
        int serverSocket;
        struct sockaddr_in serverAddr;
        int serverPort;

        std::set<int> ids_available;
        std::map<int, int> id_fd; // Map of user ID to socket FD
        std::unordered_map<int, int> fd_id; // Reverse of id_fd
        std::unordered_map<int, std::string> id_name; // User ID to name mapping
        std::set<std::string> names; // Set of names to check for uniqueness
        std::unordered_map<int, std::unordered_map<std::string, std::string>> id_env; // Environment variables per user

        fd_set readfds;
        int max_fd;

        void broadcastMessage(const std::string& message);
        bool processRequest(int clientSocket);
        void acceptNewConnection();
        std::string tcp2Address(int);
    public:
        Server();
        ~Server();
        void start();
};

#endif
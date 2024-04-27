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
#include "UserManager.h"

#define MAX_USER_ID 30
#define DEFAULT_USER_NAME "(no name)"
#define PORT 7001


class Server{
    private:
        int serverSocket;
        struct sockaddr_in serverAddr;
        int serverPort;

        UserManager& userManager;

        std::set<int> ids_available;
        std::map<int, int>& id_fd;
        std::unordered_map<int, int>& fd_id;
        std::unordered_map<int, std::string>& id_name;
        std::set<std::string>& names;
        std::unordered_map<int, std::unordered_map<std::string, std::string>>& id_env;
        std::map<std::pair<int, int>, int>& user_pipe;

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
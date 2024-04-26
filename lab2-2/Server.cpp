#include "Server.h"
#include "Shell.h"

Server::Server() : serverPort(PORT) {
    for (int i = 1; i <= MAX_USER_ID; i++) {
        ids_available.insert(i);
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "[Server] Hosting on " << inet_ntoa(serverAddr.sin_addr) << ", port " << ntohs(serverAddr.sin_port) << std::endl;


    FD_ZERO(&readfds);
    FD_SET(serverSocket, &readfds);
    max_fd = serverSocket;
}

Server::~Server() {
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
    std::cout << "[Server] Shutdown\n";
}

void set_nonblock(int socket) {
    int flags;
    flags = fcntl(socket,F_GETFL,0);
    assert(flags != -1);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}



void Server::acceptNewConnection(){
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if(clientSocket < 0){
        perror("Socket accept failed");
        return;
    }

    FD_SET(clientSocket, &readfds);
    max_fd = std::max(max_fd, clientSocket);

    int user_id = *ids_available.begin();
    ids_available.erase(user_id);
    id_fd[user_id] = clientSocket;
    fd_id[clientSocket] = user_id;
    id_name[user_id] = DEFAULT_USER_NAME;
    id_env[user_id]["PATH"] = "bin:.";

    std::cout << "New Client Socket FD: " << clientSocket << std::endl;

    std::string welcomeMessage = "****************************************\n"
                                 "** Welcome to the information server. **\n"
                                 "****************************************\n" + tcp2Address(clientSocket) + " connected.\n";
    dprintf(clientSocket, "%s", welcomeMessage.c_str());
    dprintf(clientSocket, "%s", "% ");

    broadcastMessage("Someone has entered the chat room.");
    set_nonblock(clientSocket);
}

void Server::broadcastMessage(const std::string& message) {
    for (auto& pair : id_fd) {
        if (pair.second != serverSocket) { // Do not send to the server's own socket
            dprintf(pair.second, "%s\n", message.c_str());
            printf("Broadcasting to fd %d\n", pair.second);
        }
    }
}

bool Server::processRequest(int clientSocket) {
    Shell shell(clientSocket);
    return shell.run();
}

void Server::start(){
    listen(serverSocket, SOMAXCONN);

    while(true){
        fd_set current_fds = readfds;
        int activity = select(max_fd + 1, &current_fds, NULL, NULL, NULL);
        if(activity < 0){
            perror("Select failed");
            return;
        }

        for(int fd = 0; fd <= max_fd; fd++){
            if(FD_ISSET(fd, &current_fds)){
                std::cout << "Set fd: " << fd << std::endl;
                if(fd == serverSocket){
                    acceptNewConnection();
                }else{
                    if(processRequest(fd)){
                        std::cout << "[Server] Exiting service for client " << tcp2Address(fd) << '\n';
                        shutdown(fd, SHUT_RDWR);
                        close(fd);
                        FD_CLR(fd, &readfds);
                    }
                }
            }
        }
    }
}

std::string Server::tcp2Address(int clientSocket){
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(clientSocket, (struct sockaddr *)&addr, &addr_size);
    std::stringstream ss;
    ss << inet_ntoa(addr.sin_addr) << ":" << ntohs(addr.sin_port);
    return ss.str();
}
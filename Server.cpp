#include "Server.h"
#include "Shell.h"

Server::Server():serverPort(PORT){;}

void Server::createSocket(){
    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
}

void Server::setServerAddrAndPort(){
    serverAddr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serverAddr.sin_addr);
    serverAddr.sin_port = htons(serverPort);
}

void Server::bindSocket(){
    // assign specific address and port to the socket
    if((bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) < 0){
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }
}

void Server::startListening(){
    if((listen(serverSocket, 5)) < 0){
        perror("Socket listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << serverPort << std::endl;
}

void Server::startAccepting(){
    int clientSocket;
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    while(true){
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, (socklen_t*)&clientAddrLen);
        if(clientSocket < 0){
            perror("Socket accept failed");
            continue; // continue accepting other connections
        }

        // spawn a new thread to process the request
        pid_t pid;
        if((pid = fork()) < 0){
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }else if(pid == 0){
            // child process
            // handle the request
            close(serverSocket); // Close the server socket in the child process
            processRequest(clientSocket);
        }
        close(clientSocket); // Close the client socket after processing
    }
}

void Server::processRequest(int clientSocket){
    Shell shell(clientSocket);
    shell.run();
}

void Server::start(){
    createSocket();
    setServerAddrAndPort();
    bindSocket();
    startListening();
    startAccepting();
    close(serverSocket);
}
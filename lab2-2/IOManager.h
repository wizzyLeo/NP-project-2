#ifndef IOMANAGER_H
#define IOMANAGER_H
#include<iostream>
#include<string>
#include<unistd.h>
#include<fcntl.h>
#include <sys/socket.h>

class IOManager{
public:
    static std::string getInput(int input_fd);
};

#endif
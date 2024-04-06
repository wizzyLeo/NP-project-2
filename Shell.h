#ifndef SHELL_H
#define SHELL_H

#include<iostream>
#include<sstream>
#include<vector>
#include<array>
#include<stdio.h>
#include<unistd.h>
#include<unordered_map>
#include<fcntl.h>
#include<string>

#include"IOManager.h"
#include"Parser.h"
#include"CommandExecutor.h"



class Shell{
    int input_fd;
    CommandExecutor cmdExec;
    std::unordered_map<int, std::array<int, 2> > pipeCounter;
    std::unordered_map<int, std::string> testPipeCounter;

    bool isPipedIn();
    void registerNumberPipe(int);
    void setCommandPipe(command_t& cmd);
    void setRedirectFile(command_t& cmd, std::string fileName);
    void setupEnviroment();
    
    void agePipeCounter();
    void setCommandIO(command_t& , int, std::string);
public:
    Shell(int);
    void run();
};


#endif 

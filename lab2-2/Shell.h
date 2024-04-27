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
#include "UserManager.h"



class Shell{
    int input_fd;
    int user_id;
    CommandExecutor cmdExec;
    std::unordered_map<int, std::array<int, 2> > pipeCounter;
    std::unordered_map<int, std::string> testPipeCounter;

    UserManager& userManager;

    std::set<int> ids_available;
    std::map<int, int>& id_fd;
    std::unordered_map<int, int>& fd_id;
    std::unordered_map<int, std::string>& id_name;
    std::set<std::string>& names;
    std::unordered_map<int, std::unordered_map<std::string, std::string>>& id_env;
    std::map<std::pair<int, int>, int>& user_pipe;


    bool isPipedIn();
    void registerNumberPipe(int);
    void setCommandPipe(command_t& cmd);
    void setRedirectFile(command_t& cmd, std::string fileName);
    void setupEnviroment();
    void agePipeCounter();
    void setCommandIO(command_t& , int, std::string);
    void processCommand(std::vector<std::string>);
public:
    Shell(int);
    bool run();
};


#endif

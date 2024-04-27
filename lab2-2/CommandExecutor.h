#ifndef COMMANDEXETUER_H
#define COMMANDEXETUER_H

#include<vector>
#include<unistd.h>
#include<array>
#include<sys/stat.h>
#include<iostream>
#include<string>
#include<memory>
#include <functional> 
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include "UserManager.h"

typedef struct command_t {
    int sender_id;
    std::vector<std::string> args;
    std::array<int, 2> pipe = {STDIN_FILENO, STDOUT_FILENO};
    int fd_out = STDOUT_FILENO;
    int fd_err = STDERR_FILENO;
} command_t;

#include <array>



class CommandExecutor{
    int input_fd;
    UserManager& userManager;

    std::set<int> ids_available;
    std::map<int, int>& id_fd;
    std::unordered_map<int, int>& fd_id;
    std::unordered_map<int, std::string>& id_name;
    std::set<std::string>& names;
    std::unordered_map<int, std::unordered_map<std::string, std::string>>& id_env;
    std::map<std::pair<int, int>, int>& user_pipe;


    bool isRedirectingToFile(command_t cmd);
    bool isEndOfCommand(command_t cmd);
    bool isUsingPipe(command_t cmd);
    void setupChildIO(command_t& cmd);
    char** getExecCmdArgs(const command_t& cmd);
    void executeCommand(const command_t& cmd);
    void cleanArgsMemory(char** args, const command_t& cmd);
public:
    CommandExecutor(int);
    void execute(command_t& cmd);
};


#endif
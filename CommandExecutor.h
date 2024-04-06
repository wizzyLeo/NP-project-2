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

typedef struct command_t {
    std::vector<std::string> args;
    std::array<int, 2> pipe = {STDIN_FILENO, STDOUT_FILENO};
    int fd_out = STDOUT_FILENO;
    int fd_err = STDERR_FILENO;
} command_t;

#include <array>



class CommandExecutor{
    bool isRedirectingToFile(command_t cmd);
    bool isEndOfCommand(command_t cmd);
    bool isUsingPipe(command_t cmd);
    void setupChildIO(command_t& cmd);
    char** getExecCmdArgs(const command_t& cmd);
    void executeCommand(const command_t& cmd);
    void cleanArgsMemory(char** args, const command_t& cmd);
public:
    void execute(command_t& cmd);
};


#endif
#include "IOManager.h"

std::string IOManager::getInput(int input_fd){
    std::string input;
    char buffer[1024];

    const char* prompt = "% ";
    write(input_fd, prompt, strlen(prompt));

    ssize_t n = read(input_fd, buffer, sizeof(buffer) - 1);
    if(n > 0){
        buffer[n] = '\0';
        input = std::string(buffer);
    }

    return input;
}
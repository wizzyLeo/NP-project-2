#include "IOManager.h"

std::string IOManager::getInput(){
    std::string input;
    while(true){
        std::cout << "% ";
        getline(std::cin, input);
        if(!input.empty()){
            return input;
        }
    }
}
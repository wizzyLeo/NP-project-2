#include"Parser.h"

std::vector< std::string> Parser::getTokenizedLine(std::string line){
    std::stringstream ss;
    ss << line;
    // tokenize line by splitting with white spaces
    std::string token;
    std::vector<std::string> tokens;
    while(ss >> token){
        tokens.push_back(token);
    }

    return tokens;
}


bool Parser::isPipeOrRedirect(std::string arg){
    return (arg[0]=='|' || arg[0]=='!' || arg[0]=='>');
}
bool Parser::isRedirect(std::string arg){
    return arg[0] == '>';
}
bool Parser::isPipe(std::string arg){
    return arg[0] == '|';
}
bool Parser::isErrorPipe(std::string arg){
    return arg[0] == '!';
}
bool Parser::isPipeUpdatingCounter(std::string arg){
    return arg != "|";
}
bool Parser::isNumberPipe(std::string argPipe){
    return argPipe.size() > 1;
}
int Parser::getUserPipeReceicer(std::string argPipe){
    return stoi(argPipe.substr(1));
}

int Parser::getPipeNumber(std::string arg){
    return std::stoi(arg.substr(1));
}

int Parser::getPipeTo(std::string arg){
    if(isNumberPipe(arg)){
        return getPipeNumber(arg);
    }
    return 0;
}
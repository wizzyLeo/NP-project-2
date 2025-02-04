#ifndef PARSER_H
#define PARSER_H

#include<sstream>
#include<iostream>
#include<vector>
#include<string>

class Parser{
public:
    static std::vector<std::string > getTokenizedLine(std::string);
    static bool isRedirect(std::string);
    static bool isErrorPipe(std::string);
    static bool isPipeOrRedirect(std::string);
    static bool isNumberPipe(std::string);
    static bool isPipe(std::string);
    static bool isPipeUpdatingCounter(std::string);
    static int getPipeNumber(std::string);
    static int getPipeTo(std::string);
};

#endif


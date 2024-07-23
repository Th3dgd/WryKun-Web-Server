// Copyright (c) 2024 WryKun
#include "php.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>

php::php(){}
php::~php(){}

bool php::getout(std::string route){
    std::string command = "php " + route;
    FILE* pipe = popen(command.c_str(), "r");
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr){
        output += buffer;
    }
    int result = pclose(pipe);
    if(result != 0){
        return false;
    }
    return true;
}

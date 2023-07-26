#include "config.h"
#include <iostream>
#include <regex>

config::config(){}
config::~config(){}

bool config::interpretateConfig(std::string conf){
    if(this->dir_public_sintaxis(conf)){
        return this->dir_public(conf);
    }
    return false;
}

bool config::dir_public_sintaxis(std::string line){
    std::regex pattern(R"(dir_public\s*=\s*("[^"]*"|'[^']*'))");
    return std::regex_match(line, pattern);
}

bool config::dir_public(std::string line){
    std::regex pattern(R"(dir_public\s*=\s*("[^"]*"|'[^']*'))");
    std::smatch matches;
    if(std::regex_search(line, matches, pattern)){
        getPublic = matches[1];
        getPublic = getPublic.substr(1);
        getPublic.pop_back();
        return true;
    }else{
        return false;
    }
}
// Copyright (c) 2024 WryKun

#include "config.h"
#include <iostream>
#include <regex>
#include <fstream>
#include <string>
#include <cstdlib>

config::config(){}
config::~config(){}

bool config::interpretateConfig(std::string conf){
    if (this->wws_port_sintaxis(conf)) {
        return this->wws_port(conf);
    }else if(this->dir_public_sintaxis(conf)){
        return this->dir_public(conf);
    }else if (this->wtp_workers_sintaxis(conf)){
        return this->wtp_workers(conf);
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

bool config::wtp_workers_sintaxis(std::string line){
    std::regex pattern(R"(wtp_workers\s*=\s*(\d+))");
    return std::regex_match(line, pattern);
}

bool config::wtp_workers(std::string line){
    std::regex pattern(R"(wtp_workers\s*=\s*(\d+))");
    std::smatch matches;
    if(std::regex_search(line, matches, pattern)){
        getWorkersWTP = std::stoull(matches[1].str());
        return true;
    }else{
        return false;
    }
}

bool config::wws_port_sintaxis(std::string line){
    std::regex pattern(R"(wws_port\s*=\s*(\d+))");
    return std::regex_match(line, pattern);
}

bool config::wws_port(std::string line){
    std::regex pattern(R"(wws_port\s*=\s*(\d+))");
    std::smatch matches;
    if (std::regex_search(line, matches, pattern)){
        port = std::stoull(matches[1].str());
        return true;
    }else{
        return false;
    }
    
}

bool config::run(){
    std::string fileConfig = "conf/server.conf";
    std::ifstream configFile(fileConfig.c_str());
    std::string lineConfig;
    while (getline(configFile, lineConfig)){
        this->interpretateConfig(lineConfig);
    }
    return 0;
}

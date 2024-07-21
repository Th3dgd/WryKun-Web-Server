// Copyright (c) 2024 WryKun

#ifndef HTTP_H
#define HTTP_H
#include <string>

class httpWWS{
public:
    httpWWS();
    ~httpWWS();
    std::string header(std::string code);
    bool httpHeader(std::string code);
};

#endif

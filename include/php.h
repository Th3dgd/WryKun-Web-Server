// Copyright (c) 2024 WryKun

#ifndef PHP_H
#define PHP_H
#include <string>

class php{
public:
    php();
    ~php();
    bool getout(std::string route);
    std::string output;
};

#endif

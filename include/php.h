// Copyright (c) 2024 WryKun

#ifndef PHP_H
#define PHP_H
#include <string>

class php{
public:
    php();
    ~php();
    bool getout(const std::string& route);
    std::string output;
private:
    bool is_safe_route(const std::string& route);
};

#endif

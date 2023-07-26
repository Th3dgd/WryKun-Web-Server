#ifndef CONFIG_H
#define CONFIG_H
#include <string>

class config   {
public:
    config();
    ~config();
    bool interpretateConfig(std::string conf);
    std::string getPublic;
private:
    bool dir_public_sintaxis(std::string line);
    bool dir_public(std::string line);
};

#endif
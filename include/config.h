#ifndef CONFIG_H
#define CONFIG_H
#include <string>

class config   {
public:
    config();
    ~config();
    bool run();
    bool interpretateConfig(std::string conf);
    std::string getPublic;
    size_t getWorkersWTP = 16;
    int port = 80;
private:
    bool dir_public_sintaxis(std::string line);
    bool dir_public(std::string line);
    bool wtp_workers_sintaxis(std::string line);
    bool wtp_workers(std::string line);
    bool wws_port_sintaxis(std::string line);
    bool wws_port(std::string line);
};

#endif
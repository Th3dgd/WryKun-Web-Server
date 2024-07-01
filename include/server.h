#ifndef SERVER_H
#define SERVER_H
#include "wtp.h"
#include "http.h"
#include "config.h"
#include "php.h"
#include "wwl.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <cstring>
#include <regex>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <chrono>

class ServerWWS {
public:
    ServerWWS(int sysExec);
    ~ServerWWS();

    bool startingSocket();
    void acceptConnection();
    void handleClient(int cientSocket);
    
private:
    int socketServer;
    int port;
    int sysExec;
    int numWorkers;
    static const int BACKLOG = 125;
    std::vector<int> workerPIDs;
};

#endif

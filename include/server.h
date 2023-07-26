#ifndef SERVER_H
#define SERVER_H
#define BACKLOG 0
#include <vector>

class ServerWWS {
public:
    ServerWWS(int puerto);
    ~ServerWWS();

    bool startingSocket();
    void acceptConnection();
private:
    int socketServer;
    int port;
    std::vector<int> clientes;
};

#endif

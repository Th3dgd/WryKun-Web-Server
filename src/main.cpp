#include <iostream>
#include "server.h"

int main() {
    ServerWWS ServerWWS(80);
    if (ServerWWS.startingSocket()) {
        ServerWWS.acceptConnection();
    } else {
        std::cerr << "Error to start server" << std::endl;
    }
    return 0;
}

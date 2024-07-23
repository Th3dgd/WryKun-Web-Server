// Copyright (c) 2024 WryKun
#include <iostream>
#include "server.h"

int main() {
    ServerWWS ServerWWS(1);
    if (ServerWWS.startingSocket()) {
        ServerWWS.acceptConnection();
    } else {
        std::cerr << "Error to start server" << std::endl;
    }
    return 0;
}

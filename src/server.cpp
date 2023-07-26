#include "server.h"
#include "http.h"
#include "config.h"
#include "php.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include <regex>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

ServerWWS::ServerWWS(int port) : port(port), socketServer(socket(AF_INET, SOCK_STREAM, 0)) {}

ServerWWS::~ServerWWS() {}

std::vector<std::string> explode(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}
std::string getExtencion(const std::string& namefiles) {
    size_t posPoint = namefiles.rfind('.');
    if (posPoint != std::string::npos) {
        return namefiles.substr(posPoint + 1);
    }
    return "";
}
std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        return buffer;
    }

    return std::vector<char>();
}
bool ServerWWS::startingSocket() {
    if (this->socketServer == -1) {
        return false;
    }
    int reuseAddr = 1;
    if (setsockopt(this->socketServer, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int)) == -1) {
        close(this->socketServer);
        return -1;
    }
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(this->socketServer, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        close(this->socketServer);
        return false;
    }
    return true;
}
void ServerWWS::acceptConnection() {
    if (listen(this->socketServer, BACKLOG) == -1) {
        close(this->socketServer);
        return;
    }
    while (true)
    {   
        config config;
        php php;
        std::string fileConfig = "conf/server.conf";
        std::ifstream configFile(fileConfig.c_str());
        std::string lineConfig;
        while (getline(configFile, lineConfig)){
            config.interpretateConfig(lineConfig);
        }
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(this->socketServer, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            close(this->socketServer);
            continue;
        }
        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead == -1) {
            close(clientSocket);
            close(this->socketServer);
            return;
        }
        buffer[bytesRead] = '\0';
        char determiner = '\n';
        std::vector<std::string> data = explode(buffer, determiner);
        std::string dataHeader = data[0];
        httpWWS httpWWS;
        dataHeader = std::regex_replace(dataHeader, std::regex(R"(^\s+|\s+$)"), "");
        if (!dataHeader.empty() && dataHeader.back() == '\r') {
            dataHeader.pop_back();
        }
        httpWWS.header(dataHeader);
        std::cout << "[WWS] - " << data[0] << std::endl;
        std::vector<std::string> separedData = explode(data[0], '/');
        std::vector<std::string> URI = explode(separedData[1], ' ');
        std::string nameFile;
        std::vector<std::string> URI_EXPLODE = explode(URI[0], '?');
        if(URI_EXPLODE[0] == ""){
            nameFile = config.getPublic+"/index.html";
        }else{
            nameFile = config.getPublic+"/"+URI_EXPLODE[0];
        }
        std::string ext = getExtencion(URI_EXPLODE[0]);
        if(ext=="png"){
            std::vector<char> imageData = readFile(nameFile);
            std::ostringstream response;
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: image/png\r\n";
            response << "Server: wws/0.0.1\r\n";
            response << "Content-Length: " << imageData.size() << "\r\n";
            response << "\r\n";
            send(clientSocket, response.str().c_str(), response.str().length(), 0);
            send(clientSocket, imageData.data(), imageData.size(), 0);
        }else if(ext=="php"){
            php.getout(config.getPublic+"/"+URI_EXPLODE[0]);
            std::stringstream ss;
            ss << "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=UTF-8\r\nServer: wws/0.0.1\r\n\r\n" << php.output;
            const std::string message = ss.str();
            const char* cstrMessage = message.c_str();
            int bytesSent = send(clientSocket, cstrMessage, strlen(cstrMessage), 0);
            if(bytesSent == -1){
                close(clientSocket);
                close(this->socketServer);
                return;
            }
        }else{
            std::ifstream file(nameFile.c_str());
            std::string dataFile;
            std::string linesDataFile;
            while(getline(file, dataFile)){
                linesDataFile = linesDataFile + dataFile + "\n";
            }
            std::stringstream ss;
            ss << "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=UTF-8\r\nServer: wws/0.0.1\r\n\r\n" << linesDataFile;
            const std::string message = ss.str();
            const char* cstrMessage = message.c_str();
            int bytesSent = send(clientSocket, cstrMessage, strlen(cstrMessage), 0);
            if (bytesSent == -1) {
                close(clientSocket);
                close(this->socketServer);
                return;
            }
        }
        close(clientSocket);
    }
}

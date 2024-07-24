#include "server.h"

ServerWWS::ServerWWS(int sysExec) : socketServer(socket(AF_INET, SOCK_STREAM, 0)), sysExec(sysExec) {
    config config;
    this->port = config.port;

    std::cout << "[WWS] - Running server in port " << this->port << std::endl;
}

ServerWWS::~ServerWWS() {}

std::vector<std::string> explode(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    try {
        while (std::getline(ss, token, delimiter)) {
            if (tokens.size() > 1000000) {
                throw std::length_error("[WWS] - Too many tokens");
            }
            tokens.push_back(token);
        }
    } catch (const std::length_error& le) {
        std::cerr << "[WWS] - Length error: " << le.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[WWS] - Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[WWS] - Unknown exception caught" << std::endl;
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
    std::vector<char> buffer;
    try {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("[WWS] - Cannot open file");
        }

        std::streamsize size = file.tellg();
        if (size > 1000000000) {
            throw std::length_error("[WWS] - File size is too large");
        }

        file.seekg(0, std::ios::beg);

        buffer.resize(size);
        if (!file.read(buffer.data(), size)) {
            throw std::runtime_error("[WWS] - Error reading file");
        }
    } catch (const std::length_error& le) {
        std::cerr << "[WWS] - Length error: " << le.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[WWS] - Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[WWS] - Unknown exception caught" << std::endl;
    }

    return buffer;
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

void ServerWWS::handleClient(int clientSocket){
    config config;
    php php;
    wwl wwl;
    config.run();

    char buffer[4096];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
        perror("Receive failed");
        close(clientSocket);
        return;
    }

    buffer[bytesRead] = '\0';

    std::vector<std::string> data = explode(buffer, '\n');
    if (data.empty()) {
        close(clientSocket);
        return;
    }

    std::string dataHeader = data[0];
    httpWWS httpWWS;
    dataHeader = std::regex_replace(dataHeader, std::regex(R"(^\s+|\s+$)"), "");
    if (!dataHeader.empty() && dataHeader.back() == '\r') {
        dataHeader.pop_back();
    }
    httpWWS.header(dataHeader);
    std::cout << "[WWS] - " << data[0] << std::endl;

    std::vector<std::string> separedData = explode(data[0], '/');
    if (separedData.size() < 2) {
        std::cerr << "[WWS] - Invalid request format" << std::endl;
        close(clientSocket);
        return;
    }

    std::vector<std::string> URI = explode(separedData[1], ' ');
    if (URI.empty()) {
        std::cerr << "[WWS] - Invalid URI format" << std::endl;
        close(clientSocket);
        return;
    }

    std::string ext;

    std::string nameFile;
    std::vector<std::string> URI_EXPLODE = explode(URI[0], '?');
    if (URI_EXPLODE.empty() || URI_EXPLODE[0].empty()) {
        nameFile = config.getPublic + "/index.wwl";
        ext = "wwl";
    } else {
        nameFile = config.getPublic + "/" + URI_EXPLODE[0];
        ext = getExtencion(URI_EXPLODE[0]);
    }

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
    }else if(ext == "wwl"){
        wwl.processLanguageValueGlobal = "";
        auto start = std::chrono::high_resolution_clock::now();
        std::string linesDataFile = wwl.processLanguage(nameFile);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "[WWL] - Processing time " << duration.count() << "ms" << std::endl;

        std::stringstream ss;
        ss << "HTTP/1.1 200 OK\r\nContent-Type: text/html;charset=UTF-8\nConnection: keep-alive\r\nServer: wws/0.0.1\r\n\r\n" << linesDataFile;
        const std::string message = ss.str();
        const char* cstrMessage = message.c_str();
        int bytesSent = send(clientSocket, cstrMessage, strlen(cstrMessage), 0);
        if (bytesSent == -1) {
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
        if (!file.is_open()) {
            linesDataFile = "<!DOCTYPE html><html lang=\"es\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>WWS - Pagina no encontrada</title><link href=\"https://fonts.googleapis.com/css2?family=Roboto:ital,wght@0,100;0,300;0,400;0,500;0,700;0,900;1,100;1,300;1,400;1,500;1,700;1,900&amp;display=swap\" rel=\"stylesheet\"><style>*{margin: 0;padding: 0;font-family: roboto;}body{display: flex;justify-content: center;align-items: center;background: linear-gradient(to right, rgb(62, 169, 240), rgb(24, 255, 112));height: 100vh;}.main-content{background: #202020;display: flex;padding: 15px;gap: 30px;border-radius: 15px;box-shadow: 0px 0px 5px 2px rgba(0, 0, 0, 0.3);}.image{width: 250px;}.second-content{color: white;display: flex;align-items: center;gap: 15px;justify-content: center;flex-direction: column;}.btn{background: rgba(255, 255, 255, 0.137);padding: 10px;border-radius: 15px;box-shadow: 0px 0px 5px 2px rgba(0, 0, 0, 0.3);transition: 250ms;cursor: pointer;}.btn:hover{transform: scale(1.1);}</style></head><body><div class=\"main-content\"><div class=\"image\"><img src=\"https://wrykun.com/wrykun-image-uniforme.svg\" width=\"100%\" alt=\"WryKun Image\"></div><div class=\"second-content\"><div class=\"text\">Pagina no encontrada</div><div class=\"btns\"><div class=\"btn\" onclick=\"toHome()\">Regresar al inicio</div></div></div></div><script>function toHome() {window.location.href = \"../\";}</script></body></html>\n";
        }
        std::stringstream ss;
        ss << "HTTP/1.1 404 Not Found\r\nContent-Type: text/html;charset=UTF-8\nConnection: keep-alive\r\nServer: wws/0.0.1\r\n\r\n" << linesDataFile;
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

void ServerWWS::acceptConnection() {
    if (listen(this->socketServer, BACKLOG) == -1) {
        perror("Listen failed");
        close(this->socketServer);
        return;
    }

    config config;

    std::cout << "[WSC] - Number of configured Workers " << config.getWorkersWTP << std::endl;

    wtp wtp(config.getWorkersWTP);

    while (true) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket = accept(this->socketServer, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket == -1) {
            perror("Accept failed");
            continue;
        }
        wtp.enqueue([this, clientSocket] {
            this->handleClient(clientSocket);
        });
    }
}

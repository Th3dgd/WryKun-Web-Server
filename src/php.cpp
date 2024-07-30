// Copyright (c) 2024 WryKun
#include "php.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <cctype>
#include <stdexcept>
#include <regex>
#include <array>
#include <filesystem>

php::php(){}
php::~php(){}

bool php::is_safe_route(const std::string& route) {
    std::regex safe_pattern("^[a-zA-Z0-9_/]+\\.php$");
    return std::regex_match(route, safe_pattern);
}

bool php::getout(const std::string& route) {
    if (!this->is_safe_route(route)) {
        std::cerr << "[PHP] - Invalid route provided." << std::endl;
        output = "<!DOCTYPE html><html lang=\"es\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>WWS - Nice try</title><link href=\"https://fonts.googleapis.com/css2?family=Roboto:ital,wght@0,100;0,300;0,400;0,500;0,700;0,900;1,100;1,300;1,400;1,500;1,700;1,900&amp;display=swap\" rel=\"stylesheet\"><style>*{margin: 0;padding: 0;font-family: roboto;}body{display: flex;justify-content: center;align-items: center;background: linear-gradient(to right, rgb(62, 169, 240), rgb(24, 255, 112));height: 100vh;}.main-content{background: #202020;display: flex;padding: 15px;gap: 30px;border-radius: 15px;box-shadow: 0px 0px 5px 2px rgba(0, 0, 0, 0.3);}.image{width: 250px;}.second-content{color: white;display: flex;align-items: center;gap: 15px;justify-content: center;flex-direction: column;}.btn{background: rgba(255, 255, 255, 0.137);padding: 10px;border-radius: 15px;box-shadow: 0px 0px 5px 2px rgba(0, 0, 0, 0.3);transition: 250ms;cursor: pointer;}.btn:hover{transform: scale(1.1);}</style></head><body><div class=\"main-content\"><div class=\"image\"><img src=\"https://wrykun.com/wrykun-image-uniforme.svg\" width=\"100%\" alt=\"WryKun Image\"></div><div class=\"second-content\"><div class=\"text\">Nice try</div><div class=\"btns\"><div class=\"btn\" onclick=\"toHome()\">Regresar al inicio</div></div></div></div><script>function toHome() {window.location.href = \"../\";}</script></body></html>\n";
        return false;
    }
    
    std::filesystem::path script_path = std::filesystem::absolute(route);
    std::filesystem::path allowed_dir = std::filesystem::current_path() / "public";
    
    if (!std::filesystem::exists(script_path) || 
        !script_path.string().starts_with(allowed_dir.string())) {
        std::cerr << "[PHP] - Public not found or not in allowed directory." << std::endl;
        output = "<!DOCTYPE html><html lang=\"es\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>WWS - Nice try</title><link href=\"https://fonts.googleapis.com/css2?family=Roboto:ital,wght@0,100;0,300;0,400;0,500;0,700;0,900;1,100;1,300;1,400;1,500;1,700;1,900&amp;display=swap\" rel=\"stylesheet\"><style>*{margin: 0;padding: 0;font-family: roboto;}body{display: flex;justify-content: center;align-items: center;background: linear-gradient(to right, rgb(62, 169, 240), rgb(24, 255, 112));height: 100vh;}.main-content{background: #202020;display: flex;padding: 15px;gap: 30px;border-radius: 15px;box-shadow: 0px 0px 5px 2px rgba(0, 0, 0, 0.3);}.image{width: 250px;}.second-content{color: white;display: flex;align-items: center;gap: 15px;justify-content: center;flex-direction: column;}.btn{background: rgba(255, 255, 255, 0.137);padding: 10px;border-radius: 15px;box-shadow: 0px 0px 5px 2px rgba(0, 0, 0, 0.3);transition: 250ms;cursor: pointer;}.btn:hover{transform: scale(1.1);}</style></head><body><div class=\"main-content\"><div class=\"image\"><img src=\"https://wrykun.com/wrykun-image-uniforme.svg\" width=\"100%\" alt=\"WryKun Image\"></div><div class=\"second-content\"><div class=\"text\">Nice try</div><div class=\"btns\"><div class=\"btn\" onclick=\"toHome()\">Regresar al inicio</div></div></div></div><script>function toHome() {window.location.href = \"../\";}</script></body></html>\n";
        return false;
    }

    std::array<char, 128> buffer;
    std::string result;
    std::string command = "php " + script_path.string();
    
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "[PHP] - Failed to open pipe." << std::endl;
        return false;
    }

    output.clear();
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        output += buffer.data();
    }

    int status = pclose(pipe);
    return (status == 0);
}
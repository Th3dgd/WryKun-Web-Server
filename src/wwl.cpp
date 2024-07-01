#include "wwl.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <map>

wwl::wwl() : statusOpen(false) {}

wwl::~wwl() {}

const std::regex openInterpretePattern("^(\\s*\\[\\[\\s*)$");
const std::regex closeInterpretePattern("^(\\s*\\]\\]\\s*)$");
const std::regex writeInterpretePattern(R"(wwl::write\s+(\"([^\"]*)\"|([^\s;]+));)");
const std::regex debugInterpretePattern(R"(wwl::debug\s+\"([^\"]*)\")");
const std::regex importInterpretePattern(R"(wwl::import\s+\"([^\"]*)\")");
const std::regex intInterpretePattern(R"(int\s+(\w+)\s*=\s*(\d+);)");
const std::regex ifInterpretePattern(R"(if\s*\(([^)]+)\)\s*\{)");
const std::regex WWLCommentPattern(R"(^\s*-/.*)");


bool WWLisComment(const std::string& line) {
    return std::regex_match(line, WWLCommentPattern);
}

std::string wwl::WWLWrite(const std::string& line) {
    std::smatch matches;
    if (std::regex_search(line, matches, writeInterpretePattern)) {
        bool hasQuotes = matches[1].length() > 1;

        if (hasQuotes){
            return matches[2].str();
        }else{
            return std::to_string(this->valuesInt[matches[1].str()]);
        }
    }
    return "";
}

bool WWLDebug(const std::string& line) {
    std::smatch matches;

    if (std::regex_search(line, matches, debugInterpretePattern)) {
        std::cout << "\033[1;31m[DEBUG]\033[0m" << "[WWS] - " << matches[1].str() << std::endl;
        return true;
    } else {
        return false;
    }
    return false;
}

/* TODO Add conditions with variables */

/*bool WWLConditions (std::string line) {
    std::smatch matches;

    if (std::regex_search(line, matches, ifInterpretePattern)) {
        std::string condition = matches[1].str();

        std::regex operatorPattern(R"((==|!=|<=|>=|<|>))");
        std::smatch operatorMatches;

        if (std::regex_search(condition, operatorMatches, operatorPattern)){
            //std::cout << "testing operator: " << operatorMatches[1].str() << std::endl;

            bool toReturnIf = false;
            
            

            return true;
        }

    }
    return false;
}*/

std::string wwl::WWLImport(const std::string& line) {
    std::smatch matches;

    config config;
    config.run();

    if (std::regex_search(line, matches, importInterpretePattern)) {
        std::string RouteToProcess = config.getPublic + "/" + matches[1].str();

        std::ifstream file(RouteToProcess.c_str());
        if (!file.is_open()) {
            return "";
        }
        std::string dataFile;
        while (getline(file, dataFile)) {
            this->processLanguageValueGlobal += this->languageProcess(dataFile);
        }
        return "";
        file.close();
    } else {
        return "";
    }
    return "";
}

bool WWLSearch(const std::string& line, const std::regex& regexPattern) {
    std::smatch matches;
    return std::regex_search(line, matches, regexPattern);
}

std::string wwl::processLanguage(std::string RouteToProcess) {
    this->logicToProcess(RouteToProcess);
    return this->processLanguageValueGlobal;
}

std::string wwl::logicToProcess(std::string RouteToProcess) {
    std::ifstream file(RouteToProcess.c_str());
    if (!file.is_open()) {
        this->processLanguageValueGlobal += "<!DOCTYPE html><html lang=\"es\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>WWS - Pagina no encontrada</title><link href=\"https://fonts.googleapis.com/css2?family=Roboto:ital,wght@0,100;0,300;0,400;0,500;0,700;0,900;1,100;1,300;1,400;1,500;1,700;1,900&amp;display=swap\" rel=\"stylesheet\"><style>*{margin: 0;padding: 0;font-family: roboto;}body{display: flex;justify-content: center;align-items: center;background: linear-gradient(to right, rgb(62, 169, 240), rgb(24, 255, 112));height: 100vh;}.main-content{background: #202020;display: flex;padding: 15px;gap: 30px;border-radius: 15px;box-shadow: 0px 0px 5px 2px rgba(0, 0, 0, 0.3);}.image{width: 250px;}.second-content{color: white;display: flex;align-items: center;gap: 15px;justify-content: center;flex-direction: column;}.btn{background: rgba(255, 255, 255, 0.137);padding: 10px;border-radius: 15px;box-shadow: 0px 0px 5px 2px rgba(0, 0, 0, 0.3);transition: 250ms;cursor: pointer;}.btn:hover{transform: scale(1.1);}</style></head><body><div class=\"main-content\"><div class=\"image\"><img src=\"https://wrykun.com/wrykun-image-uniforme.svg\" width=\"100%\" alt=\"WryKun Image\"></div><div class=\"second-content\"><div class=\"text\">Pagina no encontrada</div><div class=\"btns\"><div class=\"btn\" onclick=\"toHome()\">Regresar al inicio</div></div></div></div><script>function toHome() {window.location.href = \"../\";}</script></body></html>\n";
    }
    std::string dataFile;
    while (getline(file, dataFile)) {
        this->processLanguageValueGlobal += this->languageProcess(dataFile);
    }
    file.close();
    return this->processLanguageValueGlobal;
}


bool wwl::WWLSaveInt(std::string line) {
    std::smatch matches;

    if (std::regex_search(line, matches, intInterpretePattern)) {
        std::string name = matches[1].str();
        int value = std::stoi(matches[2].str());

        this->valuesInt[name] = value;
    }

    return true;
}

std::string wwl::languageProcess(const std::string& line) {
    std::string processToReturn = "";

    if (WWLSearch(line, openInterpretePattern) && !this->statusOpen) {
        this->statusOpen = true;
    }

    int isCommnet = WWLisComment(line);

    if (WWLSearch(line, closeInterpretePattern) && this->statusOpen) {
        this->statusOpen = false;
    }

    if (WWLSearch(line, writeInterpretePattern) && statusOpen && !isCommnet) {
        processToReturn += this->WWLWrite(line) + "\n";
    }else if (WWLSearch(line, debugInterpretePattern) && statusOpen && !isCommnet){
        WWLDebug(line);
    } else if (WWLSearch(line, importInterpretePattern) && statusOpen && !isCommnet) {
        this->statusOpen = false;
        processToReturn += WWLImport(line);
        this->statusOpen = true;
    }else if (WWLSearch(line, intInterpretePattern) && statusOpen && !isCommnet) {
        WWLSaveInt(line);
    }else if (WWLSearch(line, ifInterpretePattern) && statusOpen && !isCommnet){
        /*WWLConditions(line);*/
    }else if (!statusOpen) {
        if (!WWLSearch(line, closeInterpretePattern) && !isCommnet) {
            processToReturn += line + "\n";
        }
    }

    return processToReturn;
}

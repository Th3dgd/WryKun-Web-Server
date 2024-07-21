// Copyright (c) 2024 WryKun

#ifndef WWL_H // WryKun Web Language
#define WWL_H // WryKun Web Language
#include <map>
#include <regex>

#include <string>

class wwl {
public:
    wwl();
    ~wwl();
    std::string WWLImport(const std::string& line);
    std::string processLanguage(std::string RouteToProcess);
    std::string importsProcessLanguage;
    std::string processLanguageValueGlobal;
    std::map<std::string, int> valuesInt;
    std::string WWLWrite(const std::string& line);

private:
    bool statusOpen;
    bool WWLSaveInt(std::string line);
    std::string logicToProcess(std::string RouteToProcess);
    std::string languageProcess(const std::string& line);
};

bool WWLisComment(const std::string& line);
bool WWLSearch(const std::string& line, const std::regex& regexPattern);

#endif // WWL_H

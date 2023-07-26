#include "http.h"
#include <iostream>
#include <string>
#include <regex>

httpWWS::httpWWS(){}

httpWWS::~httpWWS(){}

std::string httpWWS::header(std::string code){
    if(this->httpHeader(code)){
        return "false";
    }
    return "false";
}
bool httpWWS::httpHeader(std::string code){
    std::regex pattern(R"(^(GET|POST|HEAD|PUT|DELETE|CONNECT|OPTIONS|TRACE|PATCH)\s/.*\sHTTP/\d\.\d$)");
    return std::regex_match(code, pattern);
}
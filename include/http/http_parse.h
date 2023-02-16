#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#include <iostream>
#include <string>
#include <map>
#include <sstream>

class HttpParser{
private:
    std::map<std::string, std::string> http;
    std::string format_key(std::string &str);
public:
    HttpParser(char *buf);
    ~HttpParser();
    void show();
    std::string operator[](std::string str);
};

#endif

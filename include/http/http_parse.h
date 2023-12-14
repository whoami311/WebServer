#pragma once

#include <map>
#include <string>

class HttpParser {
public:
    HttpParser(char* buf);
    ~HttpParser();
    void Show();
    std::string operator[](const std::string& str);

private:
    std::string FormatKey(const std::string& str);

    std::map<std::string, std::string> http;
};

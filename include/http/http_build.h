#pragma once

#include <string>
#include <unordered_map>

class HttpBuild {
public:
    HttpBuild();
    ~HttpBuild();
    void Show();
    std::string operator[](const std::string& str);
    void Set(const std::string& key, const std::string& val);
    std::string GetContent();

private:
    // string stateLine();
    std::string GetStateLine();
    std::string GetHeader();

    std::string endOfLine;
    std::unordered_map<int, std::string> stateTitle;
    std::unordered_map<int, std::string> stateBody;
    std::string version;
    int state;
    std::unordered_map<std::string, std::string> header;
    std::string body;
};

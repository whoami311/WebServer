#ifndef HTTP_BUILD_H
#define HTTP_BUILD_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

using namespace std;

class HttpBuild {
public:
    HttpBuild();
    ~HttpBuild();
    void show();
    std::string operator[](std::string str);
    void set(string key, string val);
    string getContent();
private:
    string stateLine();
    string getStateLine();
    string getHeader();

    string endOfLine;
    unordered_map<int, string> stateTitle;
    unordered_map<int, string> stateBody;
    string version;
    int state;
    unordered_map<string, string> header;
    string body;
};

#endif

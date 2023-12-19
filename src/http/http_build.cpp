#include "http/http_build.h"

#include <iostream>

HttpBuild::HttpBuild() {
    endOfLine = "\r\n";

    stateTitle[200] = "OK";
    stateTitle[400] = "Bad Request";
    stateTitle[403] = "Forbidden";
    stateTitle[404] = "Not Found";
    stateTitle[500] = "Internal Error";

    stateBody[400] = "Your request has bad syntax or is inherently impossible to staisfy.";
    stateBody[403] = "You do not have permission to get file form this server.";
    stateBody[404] = "The requested file was not found on this server.";
    stateBody[500] = "There was an unusual problem serving the request file.";
}

HttpBuild::~HttpBuild() {}

void HttpBuild::Show() {
    std::cout << GetContent();
}

std::string HttpBuild::operator[](const std::string& str) {
    if (str == "State")
        return std::to_string(state);
    else if (str == "Version")
        return version;
    else if (str == "Body")
        return body;
    else
        return header[str];
}

void HttpBuild::Set(const std::string& key, const std::string& val) {
    if (key == "State") {
        state = stoi(val);
        if (state != 200) {
            body = stateBody[state];
            header["Content-length"] = std::to_string(stateBody[state].size());
        }
    } else if (key == "Version")
        version = val;
    else if (key == "Body")
        body = val;
    else
        header[key] = val;
}

std::string HttpBuild::GetStateLine() {
    return version + " " + std::to_string(state) + " " + stateTitle[state] + endOfLine;
}

std::string HttpBuild::GetHeader() {
    std::string s;
    for (auto it = header.cbegin(); it != header.cend(); ++it) {
        s += it->first + ": " + it->second + endOfLine;
    }
    return s;
}

std::string HttpBuild::GetContent() {
    std::string s = GetStateLine() + GetHeader() + body + endOfLine;
    return s;
}

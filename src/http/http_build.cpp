#include "http/http_build.h"

using namespace std;

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

void HttpBuild::show() {
    std::cout << getContent();
}

string HttpBuild::operator[](string str) {
    if (str == "State")
        return to_string(state);
    else if (str == "Version")
        return version;
    else if (str == "Body")
        return body;
    else
        return header[str];
}

void HttpBuild::set(string key, string val) {
    if (key == "State") {
        state = stoi(val);
        if (state != 200) {
            body = stateBody[state];
            header["Content-length"] = to_string(stateBody[state].size());
        }
    } else if (key == "Version")
        version = val;
    else if (key == "Body")
        body = val;
    else
        header[key] = val;
}

string HttpBuild::getStateLine() {
    return version + " " + to_string(state) + " " + stateTitle[state] + endOfLine;
}

string HttpBuild::getHeader() {
    string s;
    for(auto it = header.cbegin(); it != header.cend(); ++it){
        s += it->first + ": " + it->second + endOfLine;
    }
    return s;
}

string HttpBuild::getContent() {
    string s = getStateLine() + getHeader() + body + endOfLine;
    return s;
}

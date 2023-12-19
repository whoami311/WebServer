#include "http/http_parse.h"

#include <iostream>
#include <sstream>

HttpParser::HttpParser(char* msg) {
    std::string buf(msg);
    std::istringstream buf_stream(buf);
    enum class Parts {
        START_LINE,
        HEADERS,
        BODY
    };
    Parts part = Parts::START_LINE;
    std::string line;
    std::string body_string;
    while (getline(buf_stream, line)) {
        switch (part) {
            case Parts::START_LINE: {
                std::istringstream line_stream(line);
                std::string tmp;
                line_stream >> tmp;
                if (tmp.find("HTTP") == std::string::npos) {
                    http.insert(std::make_pair("Method", tmp));
                    line_stream >> tmp;
                    http.insert(std::make_pair("Path", tmp));
                    line_stream >> tmp;
                    http.insert(std::make_pair("Version", tmp));
                } else {
                    http.insert(std::make_pair("Version", tmp));
                    line_stream >> tmp;
                    http.insert(std::make_pair("Status", tmp));
                    line_stream >> tmp;
                    http.insert(std::make_pair("Status_text", tmp));
                }
                part = Parts::HEADERS;
                break;
            }
            case Parts::HEADERS: {
                if (line.size() == 1) {
                    part = Parts::BODY;
                    break;
                }
                auto pos = line.find(":");
                if (pos == std::string::npos)
                    continue;
                std::string tmp1(line, 0, pos);
                std::string tmp2(line, pos + 2);
                http.insert(std::make_pair(FormatKey(tmp1), tmp2));
                break;
            }
            case Parts::BODY: {
                body_string.append(line);
                body_string.push_back('\n');
                break;
            }
            default:
                break;
        }
    }
    http.insert(std::make_pair("body", body_string));
}

HttpParser::~HttpParser() {}

void HttpParser::Show() {
    for (auto it = http.cbegin(); it != http.cend(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
}

std::string HttpParser::operator[](const std::string& str) {
    auto it = http.find(FormatKey(str));
    return it != http.end() ? it->second : "";
}

std::string HttpParser::FormatKey(const std::string& str) {
    std::string des(str);
    if (des[0] >= 'a' && des[0] <= 'z') {
        des[0] = des[0] + 'A' - 'a';
    }
    int position = 0;
    while ((position = des.find("-", position)) != std::string::npos) {
        if (des[position + 1] >= 'a' && des[position + 1] <= 'z') {
            des[position + 1] = des[position + 1] + 'A' - 'a';
        }
        position++;
    }
    return des;
}

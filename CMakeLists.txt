cmake_minimum_required(VERSION 3.5)

project(WebServer VERSION 1.0)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 14)

set(CXX_FLAGS
    -pthread
)

include_directories(
    include
)

set(SRCS
    src/utils.cpp
    src/main.cpp
    src/server.cpp
    src/log.cpp
    src/thread_pool.cpp
    src/timer.cpp
    src/client.cpp
    src/network_conn.cpp
    src/http/http_data.cpp
    src/http/http_req.cpp
    src/http/http_rsp.cpp
    src/http/http_parse.cpp
    src/http/http_build.cpp
)

add_executable(WebServer ${SRCS})
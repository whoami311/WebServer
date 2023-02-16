# WebServer

## Introduction

Linux下的轻量级Web服务器，支持Get请求获取静态资源。使用C++实现线程池 + 非阻塞socket + epoll(ET)的并发模型。

## Reference

项目：

- [TinyWebServer](https://github.com/qinguoyi/TinyWebServer)
- [WebServer](https://github.com/imarvinle/WebServer)
- [WebServer](https://github.com/linyacool/WebServer)
- [WebServer](https://github.com/markparticle/WebServer)
- [httpParser](https://github.com/yuesong-feng/httpParser)

书籍：

- 《UNIX网络编程 卷1：套接字联网API》
- 《Linux高性能服务器编程》游双

## Technical points

- 单例模式的线程池：保证并发前提下减少创建线程的开销。
- 单例模式的异步日志：使用阻塞队列和条件变量实现，避免写日志的IO操作降低程序运行效率。
- 非阻塞socket + epoll(ET)：提高并发请求处理效率，使用EPOLLONESHOT保证一个socket只被一个线程处理。
- 定时器：使用c++11的优先队列实现，固定时间后处理回调函数，节点可删除。
- http_build：构建http消息体，提高易用性。

## Envoirment

- 系统：Kali2022.3
- 浏览器：Edge、Chrome
- C++标准：C++14
- 编译器版本：g++ (Debian 12.2.0-3) 12.2.0
- 代码编辑器：vscode

## Todo List

- 并发性能测试。
- 实现时间轮方式的定时器。
- 丰富网页功能。
- 学习[muduo](https://github.com/chenshuo/muduo)中的异步日志处理，完善日志模块。

## Build & Usage

构建:

```shell
mkdir build
cd build
cmake ..
make
```

运行：

```shell
./WebServer
```

启动参数：

- p: 端口，默认80
- t: 线程池的线程数量，默认4
- l: 日志路径，默认webserver.log

## 致谢

[@qinguoyi](https://github.com/qinguoyi/TinyWebServer)
[@imarvinle](https://github.com/imarvinle/WebServer)
[@linyacool](https://github.com/linyacool/WebServer)
[@markparticle](https://github.com/markparticle/WebServer)
[@yuesong-feng](https://github.com/yuesong-feng/httpParser)

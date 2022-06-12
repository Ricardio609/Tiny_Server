/*
 * endoue: utf-8
 * @author Ricardo609
 * @date 2022-06-12
 */

#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <arpa/inet.h>          //sockaddr_in
#include <sys/uio.h>            //readv / writev
#include <iostream>
#include <sys/types.h>
#include <assert.h>

#include "buffer.h"
#include "HTTPrequest.h"
#include "HTTPresponse.h"

class HTTPConnection {
public:
    HTTPConnection();
    ~HTTPConnection();

    /* 初始化HTTP连接 */
    void initHTTPConn(int socketFd, const sockaddr_in& addr);

    /* 每个HTTP连接中定义的对缓冲区的读写接口 */
    ssize_t readBuffer(int* saveErrno);
    ssize_t writeBuffer(int* saveErrno);

    /* 关闭HTTP连接的接口 */
    void closeHTTPConn();
    /* 定义处理HTTP连接的接口，主要分为reequest的解析和respones的生成 */
    bool handleHTTPConn();

    /* 一些获取HTTP连接信息的函数 */
    const char* getIP() const;
    int getPort() const;
    int getFd() const;              //获取HTTP连接的描述符，也是唯一标志          
    sockaddr_in getAddr() const;

    /* 获取已经写入的数据长度 */
    int writeBytes() {
        return iov_[1].iov_len + iov_[0].iov_len;
    }

    /* 获取HTTP连接的KeppAlive状态 */
    bool isKeepAlive() const {
        return request_.isKeepAlive();
    }

    static bool                 isET;
    static const char*          srcDir;              //解析请求时，当前目录的路径
    static std::atmoic<int>     userCount;           //HTTP连接的个数，即用户的个数。（在HTTP连接初始化和关闭时修改该数据，在上层初始化它）

private:
    int                         fd_;                //HTTP连接对应的描述符
    struct sockaddr_in          addr_;
    bool                        isClose_;           //标记是否关闭

    int                         iovCnt_;
    struct iovec                iov_[2];

    Buffer                      readBuffer_;        //读缓冲区
    Buffer                      writeBuffer_;       //写缓冲区

    HTTPrequest                 request_;           //解析请求
    HTTPresponse                response_;          //响应请求
};

#endif /* HTTPCONNECTION_H */
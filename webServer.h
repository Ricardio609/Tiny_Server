/*
 * encode: utf-8
 * @author Ricardo609
 * @date 2022-06-12
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>          //fcntl()
#include <unistd.h>         //close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "threadpool.h"
#include "epoller.h"
#include "timer.h"
#include "HTTPConnection.h"

class WebServer {
public:
    WebServer(int port, int trigMode, int timeoutMs, bool optLinger, int threadNum);
    ~WebServer();

    void Start();

private:
    /* 对服务端的socket进行设置，最后可以得到listenFd */
    bool initSocket_();
    /* 初始化事件模式 */
    void initEventMode_(int trigMode);
    /* 添加HTTP连接 */
    void addClientConnection(int fd, sockaddr_in addr);
    /* 关闭HTTP连接 */
    void closeConn_(HTTPConnection* client);

    /* 收到新的HTTP请求，即fd==listenFd_ */
    void handleListen_();       //该函数中会得到新的描述符，然后需要将新的描述符和新的描述符对应的连接记录下来，调用addClientConnection（）
    
    /* 已经建立连接的HTTP发来IO请求，此时需要提供读写处理。  为了提高性能，采用线程池。下面两个函数将读写的底层实现函加入到线程池中*/
    void handleWrite_(HTTPConnection* client);
    void handleRead_(HTTPConnection* client);

    /* 读写的底层函数*/
    void onRead_(HTTPConnection* client);
    void onWrite_(HTTPConnection* client);

    /*epoll采用的是边缘触发(ET)，在读结束时，根据需要改变epoll的事件*/
    void onProcess_(HTTPConnection* client);

    void sendError_(int fd, const char* info);
    void extentTime_(HTTPConnection* client);

    static const int MAX_FD = 65536;
    static int setFdNonblock(int fd);

    int     prot_;
    int     timeoutMs_;         //毫秒，定时器的默认过期时间
    bool    isClose_;
    int     listenFd_;
    bool    openLinger_;
    char*   srcDir_;            //需要获取的路径

    uint32_t listenEvent_;
    uint32_t connectionEvent_;

    std::unique_ptr<TimerManager>               timer_;
    std::unique_ptr<ThreadPool>                 threadpool_;
    std::unique_ptr<Epoller>                    epoller_;
    std::unordered_map<int, HTTPConnection>     users_;
};

#endif /* WEBSERVER_H */

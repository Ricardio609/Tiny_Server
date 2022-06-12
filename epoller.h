/*
 * encode: utf-8
 * @author Ricardo609
 * @date 2022-06-08
 */

#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h>  //epoll_ctl()
#include <fcntl.h>      //fcntl()
#include <unistd.h>     //close()
#include <assert.h>
#include <vector>
#include <errno.h>

class Epoller {
public:
    explicit Epoller(int maxEvent = 1024);
    ~Epoller();

    bool addFd(int fd, uint32_t events);    //将描述符fd加入epoll监控
    bool modFd(int fd, uint32_t events);    //修改描述符fd对应的事件
    bool delFd(int fd);                     //将描述符fd移处epoll的监控

    int wait(int timewait = -1);            //用于返回监控的结果，成功时返回就绪文件描述符的个数

    int getEventFd(size_t i) const;         //获取fd的函数
    
    uint32_t getEvents(size_t i) const;     //获取events的函数

private:
    int epollerFd_;                                   //标记epoll的标识符
    std::vector<struct epoll_event> events_;          //存储需要返回准备**就绪的事件**

};

#endif /* EPOLLER_H */
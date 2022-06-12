/*
 * encode: utf-8
 * @author Ricardo609
 * @date 2022-06-08
 */

#ifndef TIMER_H
#define TIMER_H

#include <ctime>
#include <chrono>

#include <vector>
#include <queue>
#include <deque>
#include <unordered_map>

#include<memory>

#include <functional>

#include "HTTPConnection.h"

typedef std::function<void()>               TimeoutCallBack;
typedef std::chrono::high_resolution_clock  Clock;
typedef std::chrono::milliseconds           Ms;
typedef Clock::time_point                   TimeStamp;
// typedef std::unique_ptr<HTTPConnection>     HTTPConnection_Ptr;

class TimerNode {
public:
    int                 id;         //标记定时器
    TimeStamp           expire;     //设置过期时间
    TimeoutCallBack     cb;         //设置一个回调函数，以方便删除定时器时关闭http连接

    bool operator<(const TimerNode& t) {
        return expire < t.expire;
    }
};

class TimerManager {
    typedef std::shared_ptr<TimerNode> SP_TimerNode;
public:
    TimerManager() {heap_.reserve(64);}
    ~TimerManager() {clear();}
    
    /* 设置定时器 */
    void addTimer(int id, int timeout, const TimeoutCallBack& cb);
    /* 处理过期的定时器 */
    void handle_expired_event();
    /* 下一次处理过期定时器的时间 */
    int getNextHandle();

    void update(int id, int timeout);
    /* 删除指定id节点，并用指针触发处理函数 */
    void work(int id);

    void pop();
    void clear();

private:
    void del_(size_t i);
    void shiftup_(size_t i);
    void shiftdown_(size_t index, size_t n);
    void swapNode_(size_t i, size_t j);

    std::vector<TimerNode> heap_;
    std::unordered_map<int, size_t> ref_;       //映射一个fd对应的定时器在heap_中的位置
};


#endif /* TIMER_H */
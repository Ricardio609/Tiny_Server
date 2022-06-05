/*
 * endoue: utf-8
 * @author Ricardo609
 * @date 2022-06-05
 */
#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <atomic>
#include <string>

#include <unistd.h>     //read()    /   write()
#include <sys/uio.h>    //readv()   /   writev()
#include <assert.h>

class Buffer {
public:
    Buffer(int initBufferSize = 1024);
    ~Buffer() = default;

    /* 获取缓冲区中的信息 */
    size_t writeableBytes() const;              //缓冲区中还可以读取的字节数
    size_t readableBytes() const;               //缓冲区中还可以写入的字节数
    size_t readBytes() const;                   //缓冲区中已经读取的字符数

    /* 以指针的方式获取当前读写指针 */
    const char* curReadPtr() const;             //获取当前读指针
    const char* curWriterPtrConst() const;      //获取当前写指针
    char* curWritePtr();

    /* 读指定长度后读指针的的更新 */
    void updateReadPtr(size_t len);
    /* 将读指针移到指定位置 */
    void updateReadPtrUntilEnd(const char* end);
    /* 写入指定长度后写指针的更新 */
    void updateWritePtr(size_t len);
    /* 读写指针初始化 */
    void initPtr();

    /* 在写入时，考虑缓冲区容量，保证将数据写入缓冲区 */
    void ensureWriteable(size_t len);
    //用STL提供的对vector的方法和对string的支持就可以实现这些功能。所以这部分主要需要实现向buffer缓冲区中添加数据的方法
    void append(const char* str, size_t len);
    void append(const std::string& str);
    void append(const void* data, size_t len);
    void append(const Buffer& buffer);

    /* 与客户端直接I/O的读写接口 */
    ssize_t readFd(int fd, int* Errno);
    ssize_t writeFd(int fd, int* Errno);

    /* 将缓冲区的数据转换为字符串 */
    std::string AlltoStr();

private:
    /* 返回指向缓冲区初始位置的指针 */
    char* BeginPtr_();
    const char* BeginPtr_() const;

    /* 用于缓冲区空间不够时的扩容 */
    void allocateSpace(size_t len);

    /* 存储实体 */
    std::vector<char> buffer_;

    /* 当前读写位置的下标 */
    std::atomic<std::size_t> readPos_;      //读指针
    std::atomic<std::size_t> writePos_;     //写指针
};

#endif /* BUFFER_H */
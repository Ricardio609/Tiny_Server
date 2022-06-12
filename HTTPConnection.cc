/*
 * encode: utf-8
 * @author Ricardo609
 * @date 2022-06-12
 */

#include "HTTPConnection.h"

const char* HTTPConnection::srcDir;
std::atomic<int> HTTPConnection::userCount;
bool HTTPConnection::isET;

HTTPConnection::HTTPConnection() {
    fd_      = -1;
    addr_    = {0};
    isClose_ = true;
};

HTTPConnection::~HTTPConnection() {
    closeHTTPConn();
};

void HTTPConnection::initHTTPConn(int fd, const sockaddr_in& addr) {
    assert(fd > 0);
    userCount++;
    addr_ = addr;
    fd_ = fd;
    writeBuffer_.initPtr();
    readBuffer_.initPtr();
    isClose_ = false;
}

void HTTPConnection::closeHTTPConn() {
    response_.unmapFile_();
    if(isClose_ == false) {
        isClose_ = true;
        userCount++;
        close(fd_);
    }
}

int HTTPConnection::getFd() const {
    return fd_;
}

struct sockaddr_in HTTPConnection::getAddr() const {
    return addr_;
}

const char* HTTPConnection::getIP() const {
    return inet_ntoa(addr_.sin_addr);
}

int HTTPConnection::getPort() const {
    return addr_.sin_port;
}

ssize_t HTTPConnection::readBuffer(int* saveErrno) {
    ssize_t len = -1;
    do {
        len = readBuffer_.readFd(fd_, saveErrno);
        //std::cout << fd_ << " read bytes: " << len << std::endl;
        if(len <= 0) break;
    } while(isET);
    return len;
}

ssize_t HTTPConnection::writeBuffer(int* saveErrno) {
    ssize_t len = -1;
    do {
        len = write(fd_, iov_, iovCnt_);
        if(len <= 0) {
            *saveErrno = errno;
            break;
        }
        if(iov_[0].iov_len + iov_[1].iov_len == 0)  break;  //传输结束
        else if(static_cast<size_t>(len) > iov_[0].iov_len) {
            iov_[1].iov_base = (uint8_t*)iov_[1].iov_base + (len - iov_[0].iov_len);
            iov_[1].iov_len -= (len - iov_[0].iov_len);
            if(iov_[0].iov_len) {
                writeBuffer_.initPtr();
                iov_[0].iov_len = 0;
            }
        } else {
            iov_[0].iov_base = (uint8_t*)iov_[0].iov_base + len;
            iov_[0].iov_len -= len;
            writeBuffer_.updateReadPtr(len);
        }
    } while(isET || writeBytes() > 10240);
    return len;
}

bool HTTPConnection::handleHTTPConn() {
    request_.init();
    if(readBuffer_.readableBytes() <= 0) {
        //std::cout << "readBufferis empty!" << std::endl;
        return false;
    } else if(request_.parse(readBuffer_)) {
        response_.init(srcDir, request_.path(), request_.isKeepAlive(), 200);
    } else {
        std::cout << "400!" << std::endl;
        //readBuffer_.printContent();
        response_.init(srcDir, request_.path(), false, 400);
    }

    response_.makeResponse(writeBuffer_);

    /* 响应头 */
    iov_[0].iov_base = const_cast<char*>(writeBuffer_.curReadPtr());
    iov_[0].iov_len = writeBuffer_.readableBytes();
    iovCnt_ = 1;

    /* 文件 */
    if(response_.fileLen() > 0 && response_.file()) {
        iov_[1].iov_base = response_.file();
        iov_[1].iov_len = response_.fileLen();
        iovCnt_ = 2;
    }
    return true;
}
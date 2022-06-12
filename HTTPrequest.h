/*
 * encode: utf-8
 * @author Ricardo609
 * @date 2022-06-12
 */

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>

#include "buffer.h"

class HTTPrequest {
public:
    enum PARSE_STATE {
        REQUEST_LINE,
        HEADERS,
        BODY,
        FINISH,
    };

    HTTPrequest() {init();};
    ~HTTPrequest() = default;

    /* 初始化变量 */
    void init();
    /* 解析HTTP请求 */
    bool parse(Buffer& buff);       //这个函数在实现过程中，根据HTTP请求报文的格式，用"\r\n"作为分隔符将报文分割成行，然后运用自动机来进行解析。

    /* 获取HTTP信息 */
    std::string path() const;
    std::string path();
    std::string method() const;
    std::string version() const;
    std::string getPost(const std::string& key) const;
    std::string getPost(const char* key) const;

    /* 获取HTTP连接是否KeepAlive */
    bool isKeepAlive() const;

private:
    /* 在parse()中，需要分别实现解析请求行、请求头和数据体的函数 */
    bool parseRequestLine_(const std::string& line);        //解析请求行
    void parseRequestHeader_(const std::string& line);      //解析请求头
    void parseDataBody_(const std::string& line);           //解析数据体

    /* 在解析请求行的时候，会解析出路径信息，之后还需要对路径信息做一个处理 */
    void parsePath_();
    /* 在处理数据体的时候，如果格式是post，那么还需要解析post报文 */
    void parsePost_();
    /* 转换Hex格式 */
    static int convertHex(char ch);

    PARSE_STATE                                     state_;
    std::string                                     method_, path_, version_, body_;
    std::unordered_map<std::string, std::string>    header_;                //存储请求头解析出来的信息
    std::unordered_map<std::string, std::string>    post_;                  //存储post解析出来的信息

    static const std::unordered_set<std::string>    DEFAULT_HTML;           //默认的网页名称
};

#endif /* HTTPREQUEST_H */
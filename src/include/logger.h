#include "lockqueue.h"
#include <string>

// 定义宏 LOG_INFO("xxx %d %s", 20, "xxxx")
#define LOG_INFO(logmsgformat, ...) \
    do { \
        Logger &logger = Logger::GetInstance(); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(LogLevel::INFO, buf); \
    } while (0) \
    
#define LOG_ERR(logmsgformat, ...) \
    do { \
        Logger &logger = Logger::GetInstance(); \
        char buf[1024] = {}; \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(LogLevel::ERROR, buf); \
    } while(0) \

// 定义日志级别
enum LogLevel {
    INFO, // 普通信息
    ERROR, // 错误信息
};

// mprpc框架提供的日志系统
class Logger {
public:
    // 获取日志的单例
    static Logger& GetInstance();
    // 写日志
    void Log(LogLevel level, std::string msg);

private:
    LockQueue<std::pair<LogLevel, std::string>> m_lckQue; // 日志缓冲队列

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};
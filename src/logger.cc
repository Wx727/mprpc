#include "logger.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

// 获取日志的单例
Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    // 启动专门的写日志线程
    std::thread writeLogTask([&](){
        for (;;) {
            // 获取当前的日期，然后取日志信息，写入相应的日志文件当中
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);

            FILE *pf = fopen(file_name, "a+");
            if (pf == nullptr) {
                std::cout << "logger file: " << file_name << " open error" << std::endl;
                exit(EXIT_FAILURE);
            }

            auto logpair = m_lckQue.Pop();
            LogLevel level = logpair.first;
            std::string msg = logpair.second;

            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d => [%s] ",
                    nowtm->tm_hour,
                    nowtm->tm_min,
                    nowtm->tm_sec,
                    (level == LogLevel::INFO ? "info" : "error"));
            msg.insert(0, time_buf);
            msg.append("\n");

            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });

    // 设置分离线程，守护线程
    writeLogTask.detach();
}

// 写日志，把日志信息写入lockqueue缓冲区当中
void Logger::Log(LogLevel level, std::string msg) {
    m_lckQue.Push({level, msg});
}
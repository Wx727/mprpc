#include "mprpcconfig.h"
#include <iostream>

void Trim(std::string& src_buf) {
    // 去掉字符串前面多余的空格
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1) {
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    // 去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1) {
        src_buf = src_buf.substr(0);
    }
}

// 负责解析加载配置文件
void MprpcConfig::LoadConfigFile(const char* config_file) {
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr) {
        std::cout << config_file << " is not exist" << std::endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        std::string read_buf(buf);
        Trim(read_buf);

        // 判断#的注释
        if (read_buf.empty() || read_buf[0] == '#') {
            continue;
        }

        // 解析配置项
        int idx = read_buf.find('=');
        if (idx == -1)
        {
            // 配置项不合法
            continue;
        }

        std::string key, value;
        key = read_buf.substr(0, idx);
        Trim(key);

        // 每次读一行，去除行尾的换行符，rpcserverip=127.0.0.1    \n
        int endidx = read_buf.find('\n', idx);
        value = read_buf.substr(idx+1, endidx-idx-1);
        Trim(value);

        m_configMap.insert({key, value});
    }
}

// 查询配置项信息
std::string MprpcConfig::Load(const std::string& key) {
    auto it = m_configMap.find(key);
    if (it == m_configMap.end()) {
        return "";
    }
    return it->second;
}
#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <muduo/net/InetAddress.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <errno.h>
#include "mprpcapplication.h"

/*
header_size + service_name method_name args_size + args
*/
// 所有通过stub代理对象调用的rpc方法，都是走到这里统一做rpc方法调用的数据序列化和网络发送
void MprpChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, 
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response, 
                          google::protobuf::Closure* done) {
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name(); // service_name
    std::string method_name = method->name(); // method_name

    // 获取参数的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        // std::cout << "serialize request error" << std::endl;
        controller->SetFailed("serialize request error");
        return;
    }

    // 定义rpc的请求header
    mprpc::RpcHeader rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcheader.SerializeToString(&rpc_header_str)) {
        header_size = rpc_header_str.size();
    } else {
        // std::cout << "serialize rpc header error" << std::endl;
        controller->SetFailed("serialize rpc header error");
        return;
    }

    // 组织待发送的rpc请求的字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4)); // header_size
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    // 打印调试信息
    std::cout << "================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "================================" << std::endl;

    // 使用tcp编程，完成rpc方法的远程调用
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        // std::cout << "create socket error, errno: " << errno << std::endl;
        // exit(EXIT_FAILURE);

        char errText[512] = {0};
        sprintf(errText, "create socket error, errno: %d", errno);
        controller->SetFailed(errText);
        return;
    }

    // 读取配置文件rpcserver的信息
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port );
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        // std::cout << "connet error, errno: " << errno << std::endl;
        close(clientfd);
        char errText[512] = {0};
        sprintf(errText, "connet error, errno: %d", errno);
        controller->SetFailed(errText);
        return;
    }

    // 发送rpc请求
    if (send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
        // std::cout << "send error, errno: " << errno << std::endl;
        close(clientfd);
        char errText[512] = {0};
        sprintf(errText, "send error, errno: %d", errno);
        controller->SetFailed(errText);
        return;
    }

    // 接收rpc请求的响应值
    char recv_buf[1024] = {};
    int recv_size = 0;
    if ((recv_size = recv(clientfd, recv_buf, 1024, 0)) == -1) {
        // std::cout << "recv error, errno: " << errno << std::endl;
        close(clientfd);
        char errText[512] = {0};
        sprintf(errText, "recv error, errno: %d", errno);
        controller->SetFailed(errText);
        return;
    }

    // 反序列化rpc调用的响应数据
    std::string response_str(recv_buf, recv_size); // 精确拷贝 recv_size 个字节，不会被 \0 截断
    if (!response->ParseFromString(response_str)) {
        // std::cout << "response error, reponse_str: " << response_str << std::endl;
        close(clientfd);
        controller->SetFailed("response error, reponse_str: " + response_str);
        return;
    }

    close(clientfd);
}
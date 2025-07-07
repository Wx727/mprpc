#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

int main(int argc, char** argv) {
    // 要使用mprpc框架，要先调用框架初始化函数
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    rpc::UserServiceRpc_Stub stub(new MprpChannel());
    
    // rpc方法的请求参数
    rpc::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    // rpc方法的响应
    rpc::LoginResponse response;

    // 发起rpc方法的调用，同步的调用过程，MprpChannel::CallMethod
    stub.Login(nullptr, &request, &response, nullptr); //RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用参数的序列化和网络发送

    // 一次rpc调用完成，读调用的结果
    if (response.result().errcode() == 0) {
        std::cout << "rpc login response success: " << response.success() << std::endl;
    } else {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl; 
    }

    // 演示调用远程发布的rpc方法Register
    rpc::RegisterRequest req;
    req.set_id(2000);
    req.set_name("mprpc");
    req.set_pwd("666666");

    rpc::RegisterResponse rsp;

    // 以同步的方式发起rpc调用请求，等待返回结果
    stub.Register(nullptr, &req, &rsp, nullptr);

    // 一次rpc调用完成，读调用的结果
    if (rsp.result().errcode() == 0) {
        std::cout << "rpc register response success: " << rsp.success() << std::endl;
    } else {
        std::cout << "rpc register response error: " << rsp.result().errmsg() << std::endl; 
    }

    return 0;
}

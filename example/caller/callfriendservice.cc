#include <iostream>
#include "mprpcapplication.h"
#include "friend.pb.h"

int main(int argc, char** argv) {
    // 要使用mprpc框架，要先调用框架初始化函数
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法GetFriendList
    rpc::FriendServiceRpc_Stub stub(new MprpChannel());
    
    // rpc方法的请求参数
    rpc::GetFriendListRequest request;
    request.set_userid(777);
    // rpc方法的响应
    rpc::GetFriendListResponse response;

    // 发起rpc方法的调用，同步的调用过程，MprpChannel::CallMethod
    MprpcController controller;
    stub.GetFriendList(&controller, &request, &response, nullptr); //RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用参数的序列化和网络发送

    if (controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    } else {
        // 一次rpc调用完成，读调用的结果
        if (response.result().errcode() == 0) {
            std::cout << "rpc login response success" << std::endl;
            int size = response.friends_size();
            for (int i = 0; i < size; i++) {
                std::cout << "index: " << i + 1 << " name: " << response.friends(i) << std::endl;
            }
        } else {
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl; 
        }
    }

    return 0;
}
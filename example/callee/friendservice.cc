#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

class FriendService : public rpc::FriendServiceRpc {
public:
    // 本地方法
    std::vector<std::string> GetFriendList(uint32_t userid) {
        std::cout << "do GetFriendList service, userid: " << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("abc");
        vec.push_back("def");
        vec.push_back("ghi");
        return vec;
    }

    // 重写基类方法
    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::rpc::GetFriendListRequest* request,
                       ::rpc::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done) {
        uint32_t userid = request->userid();

        std::vector<std::string> friendList = GetFriendList(userid);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (std::string &name : friendList) {
            response->add_friends(name);
        }

        done->Run();
    }
};

int main(int argc, char** argv) {
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象，把FriendService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // 启动一个rpc服务发布节点，Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}
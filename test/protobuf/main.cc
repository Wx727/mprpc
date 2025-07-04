#include <iostream>
#include <string>
#include "test.pb.h"

using namespace mprpc;

int main() {
    GetFriendListResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    User *user1 = rsp.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(17);
    user1->set_sex(User::MAN);

    User *user2 = rsp.add_friend_list();
    user2->set_name("li si");
    user2->set_age(17);
    user2->set_sex(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;
    for (auto v : rsp.friend_list()) {
        std::cout << v.name() << " " << v.age() << " " << v.sex() << std::endl;
    }
    return 0;
}
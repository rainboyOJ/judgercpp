/**
 * 没有发送的数据的dump 与 load
 */

#include "../src/Send.hpp"
#include <cstdio>

int main(){
    MessageSendJudge msg(
            "key"
            ,"this is code"
            ,"cpp"
            ,"1000"
            ,1000
            ,128
            );
    std::cout << msg << std::endl; 

    auto dumps = msg.dumps();
    std::cout << "dumps.size() "<< dumps.size() << std::endl;
    
    MessageSendJudge msg2;
    msg2.loads(dumps);
    for (const auto& e : dumps) {
        //std::cout << e << " ";
        printf("%x ",e & 0xff);

    }
    
    std::cout  << "\n\n";
    std::cout << msg2 << std::endl;
    return 0;
}

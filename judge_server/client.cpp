#include "./src/Client.hpp"

std::string codeAplusB = R"(/* author: Rainboy email: rainboylvx@qq.com  time: 2022年 03月 21日 星期一 08:56:19 CST */
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int maxn = 1e6+5,maxe = 1e6+5; //点与边的数量

int n,m;
/* 定义全局变量 */

int main(int argc,char * argv[]){
    int a,b;
    std::cin >> a >> b;
    std::cout << a+b ;
    return 0;
}
)";

int main(){
    
    Client myclient(4); //与服务器建立4个连接

    //返回信息的处理函数
    myclient.set_result_handle([](MessageResultJudge & res){
                std::cout << res << std::endl;
            });
    //发送评测数据
    myclient.send("test key"
            ,codeAplusB
            ,"cpp"
            ,"1000"
            ,1000
            ,128
            );
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    return 0;
}

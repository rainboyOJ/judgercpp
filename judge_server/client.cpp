#include "./src/Client.hpp"


int main(){
    
    Client myclient(4);

    myclient.set_result_handle([](MessageResultJudge & res){
                std::cout << res << std::endl;
            });
    myclient.send("test key"
            ,"this is code"
            ,"cpp"
            ,"1000"
            ,1000
            ,128
            );
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    return 0;
}

#include "./src/Client.hpp"


int main(){
    
    Client myclient;
    myclient.Connect();
    myclient.send("key"
            ,"this is code"
            ,"cpp"
            ,"1000"
            ,1000
            ,128
            );
    return 0;
}

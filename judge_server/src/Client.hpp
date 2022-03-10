/**
 * @brief 对client 进行封装
 */

#pragma once

#include <iostream>
#include <string_view>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "socketBase.hpp"

class Client :public socketBase{
public:
    explicit Client(int port = 9000);
    ~Client(){
        close(sockfd);
    }
    void Connect();
    void send(std::string_view msg); //发送评测数据
    void clear(); // 清空

private:
    int    sockfd;
    struct sockaddr_in servaddr;
    bool   connected{false};
    int port;

};

Client::Client(int port)
    :port(port)
{
    std::cout << port << std::endl;
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    std::cout << sockfd << std::endl;
    if (sockfd < 0) 
    { 
        std::cout <<"socket() failed.\n";
        return;
    }

    memset(&servaddr,0,sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(port);
    servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
}

void Client::Connect() {
    int result= connect(sockfd, (struct sockaddr *)&servaddr,sizeof(servaddr));
    std::cout << result << std::endl;
    if ( result == -1)
    {
        char msg[2048] = {0};
        sprintf(msg,"connect(%s:%d) failed.\n","127.0.0.1",port); close(sockfd);  
        std::cout << msg << std::endl;
        return;
    }
    std::cout << "connect ok" << std::endl;
    connected = true;
}

void Client::send(std::string_view msg){
}

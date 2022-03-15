/**
 * @desc 
 * 目的 : 将多个socket加入队列中,用于多线程的访问
 * 保证数据的发送顺序
 */

#pragma once
#include "concurrentqueue.h"
#include <chrono>
#include <thread>

//RAII
class socketPicker {
public:
    socketPicker( moodycamel::ConcurrentQueue<int>& q)
        :q{q}
    {
        do {
            q.try_dequeue(sock);
            std::cout << "pick sock "<< sock << std::endl;
            if(sock == -1 )
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            else break;
        } while(1);
    }
    ~socketPicker(){
        q.enqueue(sock);
    };
    int get(){ return sock; }
private:
    int sock{-1};
    moodycamel::ConcurrentQueue<int>& q; //多线程队列
};



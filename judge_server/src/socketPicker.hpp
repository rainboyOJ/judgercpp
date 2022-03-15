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
template<std::size_t size=4>
class socketPicker {
public:
    socketPicker(){
        do {
            q.try_dequeue(sock);
            if(sock == -1 )
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } while(1);
    }
    ~socketPicker(){
        q.enqueue(sock);
    };
    int get(){ return sock; }
private:
    int sock{-1};
    static moodycamel::ConcurrentQueue<int> q; //多线程队列
};

template<std::size_t size>
moodycamel::ConcurrentQueue<int> socketPicker<size>::q{size};


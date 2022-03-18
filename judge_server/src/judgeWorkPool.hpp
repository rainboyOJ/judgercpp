/**
 * 工作线程池,一个特化的线程池,只能执行judgeWork
 * 基于 judge_Queue
 *
 * 当 加入数据时,会换醒工作线程
 *
 * 工作线程,会一会工作,直接队列为空时,挂起工作,等待下一次唤醒
 */

#pragma once

#include <vector>
#include "judgeQueue.hpp"



class judgeWorkPool {
public:
    //默认4个线程
    explicit judgeWorkPool(unsigned int size = 4);
    judgeWorkPool(const judgeWorkPool &) = delete;
    judgeWorkPool& operator=(const judgeWorkPool &) = delete;

    ~judgeWorkPool() {
        _task_cv.notify_all();
        for(auto &th : _workPool){
            if( th.joinable()) th.join();
        }
    }
    void notify_all(); //唤醒所有

    //加入数据进队列
    bool enqueue(judge_Queue_node &jn){
        bool ret = judge_Queue::get().enque(jn);
        if( ret ) _task_cv.notify_one(); //唤醒一个
        return ret;
    }


private:
    std::mutex mtx; //锁
    std::condition_variable _task_cv; //条件
    std::vector<std::thread> _workPool; //工作线程池
    std::atomic<int> ThrNum; //空闲线程数量
};

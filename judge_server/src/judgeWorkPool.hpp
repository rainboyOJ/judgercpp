/**
 * 工作线程池,一个特化的线程池,只能执行judgeWork
 * 基于 judge_Queue
 *
 * 当 加入数据时,会换醒工作线程
 *
 * 工作线程,会一会工作,直接队列为空时,挂起工作,等待下一次唤醒
 */

#pragma once

#include <mutex>
#include <condition_variable>
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
    void judgeWork();
    std::mutex mtx; //锁
    std::condition_variable _task_cv; //条件
    std::vector<std::thread> _workPool; //工作线程池
    std::atomic<int> ThrNum; //空闲线程数量
};
//默认4个线程
judgeWorkPool::judgeWorkPool(unsigned int size){
    for(int i = 0 ;i< size ;++i){
        _workPool.emplace_back(&judgeWorkPool::judgeWork,this);
        ThrNum.fetch_add(1); //空闲线程+1
    }
}

/**
 * @desc 执行评测的流程
 *
 * 整个评测流程分和两个部分:
 *
 * 一: 准备阶段
 *      是否是支持的评语
 *      查找题目的位置,判断题目是否存在,并返回题目的相关信息
 *      创建评测的文件夹
 *      写入代码
 *      编译
 *      写入评测队列,进入评测阶段
 * 二: 评测阶段
 */

//阶段一,二都应该从一个队列里取放数据,所以要设计一个队列
void judgeWorkPool::judgeWork(){

    auto &jq = judge_Queue::get(); //得到judge queue
    judge_Queue_node jn;
    bool succ = jq.try_deque(jn); //加入
    if( succ == false) return ;  //失败

    std::cout << "de judge queue succ" << std::endl;

}



/**
 * @desc
 * 评测队列
 */
#pragma once
#include "judgeArgs.hpp"
#include "concurrentqueue.h"

//评测队列里的一个点
struct judge_Queue_node {

    
    JUDGE_STAGE stage;      //评测的阶段
    int fd;                 //请求过的sock
    
    std::string key;        // 返回结果时携带的key值
    std::string code;       // 代码
    std::string language;   // 语言
    std::string pid;        // 评测的题目的id,也有可能是题目的路径
    std::string problem_path; //评测题目的目录

    int timeLimit;          // ms
    int memoryLimit;        // mb
    judge_args JA; //参数
};


//评测队列,单例模式
class judge_Queue {
public:
    static judge_Queue& get(){
        static judge_Queue jq;
        return  jq;
    }

    bool enque(judge_Queue_node &jn){
        return q.enqueue(jn);
    }

    bool try_deque(judge_Queue_node &jn){
        return q.try_enqueue(jn);
    }

private:
    judge_Queue() = default;
    judge_Queue(judge_Queue&)  = delete;
    judge_Queue(judge_Queue&&) = delete;

    void enque(); //插入
    moodycamel::ConcurrentQueue<judge_Queue_node> q;     // 存接入的socket队列
};

/**
 * @desc
 * 评测队列
 */
#pragma once
#include "judgeArgs.hpp"

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

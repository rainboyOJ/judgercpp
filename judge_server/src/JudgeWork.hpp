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

#pragma once
#include <condition_variable>
#include <vector>

#include "judgeQueue.hpp"


void judgeWork(){

    auto &jq = judge_Queue::get(); //得到judge queue
    judge_Queue_node jn;
    bool succ = jq.try_deque(jn); //加入
    if( succ == false) return ;  //失败

    std::cout << "de judge queue succ" << std::endl;

}


/**
 * @desc server 评测后的 结果 发送的 评测数据
 */
#pragma once

#include <string_view>
#include <sstream>
#include <cstring>
#include <iostream>
#include <vector>

#include "MessageBuffer.hpp"
#include "define.hpp"

/**
 *
 * 发送结果的可能性:
 * std::
 */

class MessageResultJudge {

public:
    MessageResultJudge() = default;
    MessageResultJudge(judgeResult_id code,std::string_view msg)
        :code{code},msg{msg}
    {}

    void loads(std::string_view str);
    MessageBuffer dumps();
    void push_back(
    int cpu_time, int real_time, long memory, int signal,
    int exit_code, int error, int result) {
        Results.push_back({cpu_time,real_time,memory,signal,exit_code,error,result});
    }
    friend std::ostream & operator<<(std::ostream & out,const MessageResultJudge & msgBuf);
private:
    judgeResult_id code;//执行的结果
    std::string msg; //
    std::vector<result> Results; //结果信
};

std::ostream & operator<<(std::ostream & out,const MessageResultJudge & msgBuf){
    out << "code : " << msgBuf.code << "\n";
    out << "msg: " << msgBuf.msg<< "\n";
    out << "Results size : " << msgBuf.Results.size() << "\n";
    for (int i = 0 ;i < msgBuf.Results.size() ;++i) {
        out << "Result : " << i << "\n";
        out << "    cpu_time  :" << msgBuf.Results[i].cpu_time  << "\n";
        out << "    real_time :" << msgBuf.Results[i].real_time<< "\n";
        out << "    memory    :" << msgBuf.Results[i].memory<< "\n";
        out << "    signal    :" << msgBuf.Results[i].signal<< "\n";
        out << "    exit_code :" << msgBuf.Results[i].exit_code<< "\n";
        out << "    error     :" << msgBuf.Results[i].error<< "\n";
        out << "    result    :" << msgBuf.Results[i].result<< "\n\n";
    }
    return out;
}

MessageBuffer MessageResultJudge::dumps(){
    MessageBuffer ret;
    ret.push_back(code);
    ret.appendMessge(msg);
    ret.push_back(Results.size());
    for (const auto& e : Results) {
        ret.push_back(e.cpu_time);
        ret.push_back(e.real_time);

        long long val = e.memory;
        auto p = (int *)&val;
        int v1 = *(p);//前半部分
        int v2 = *(p+1);//前半部分
        ret.push_back(v1);
        ret.push_back(v2);

        ret.push_back(e.signal);
        ret.push_back(e.exit_code);
        ret.push_back(e.error);
        ret.push_back(e.result);
    }
    return ret;
}

void MessageResultJudge::loads(std::string_view str){
    int cur = 0;
    code = (judgeResult_id)MessageBuffer::dumpsMessageFromStr<int>(str.data(),cur);
    msg = MessageBuffer::dumpsMessageFromStr<std::string>(str.data(),cur);
    int size =MessageBuffer::dumpsMessageFromStr<int>(str.data(),cur);
    for(int i=1;i<=size;++i){
        result r;
        r.cpu_time = MessageBuffer::dumpsMessageFromStr<int>(str.data(),cur);
        r.real_time = MessageBuffer::dumpsMessageFromStr<int>(str.data(),cur);
        int v1 = MessageBuffer::dumpsMessageFromStr<int>(str.data(),cur);
        int v2 = MessageBuffer::dumpsMessageFromStr<int>(str.data(),cur);
        auto p = (int *)&r.memory;
        *p     = v1;
        *(p+1) = v2;
        r.exit_code= MessageBuffer::dumpsMessageFromStr<int>(str.data(),cur);
        r.error= MessageBuffer::dumpsMessageFromStr<int>(str.data(),cur);
        r.result= MessageBuffer::dumpsMessageFromStr<int>(str.data(),cur);
        Results.push_back(r);
    }
}

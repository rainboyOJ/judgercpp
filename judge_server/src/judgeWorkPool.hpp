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
#include <deque>        //双端队列

#include "socketBase.hpp"
#include "socketManager.hpp"
#include "judgeQueue.hpp"
#include "Result.hpp"
#include "utils.hpp"
#include "Problem.hpp"


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
        //bool ret = judge_Queue::get().enque(jn);
        //if( ret ) _task_cv.notify_one(); //唤醒一个
        //return ret;
    }


    // 评测
    result __judger(judge_args&& args){
        std::stringstream ss;
        //log("参数",(judge_bin + static_cast<std::string>(args)).c_str() );
        //std::cout << std::endl ;
        exec( ( std::string(__CONFIG::judger_bin) + static_cast<std::string>(args)).c_str() ,ss);
        result RESULT;
        ss >> RESULT.cpu_time;
        ss >> RESULT.real_time;
        ss >> RESULT.memory;
        ss >> RESULT.signal;
        ss >> RESULT.exit_code;
        ss >> RESULT.error;
        ss >> RESULT.result;
        return RESULT;
    }

    /**
     * 写message
     */
    void write_message(int fd,MessageResultJudge& msg);

    bool enque(MessageSendJudge&& msj,int fd){
        std::unique_lock<std::mutex> lck(mtx);
        judge_Queue_node jn;
        jn.stage       = JUDGE_STAGE::PREPARE;
        jn.fd          = fd;
        jn.key         = std::move(msj.key);
        jn.code        = std::move(msj.code);
        jn.language    = std::move(msj.language);
        jn.pid         = std::move(msj.pid);
        jn.timeLimit   = msj.timeLimit;
        jn.memoryLimit = msj.memoryLimit;
        q.push_back(std::move(jn));
        _task_cv.notify_one(); //唤醒一个
        return true;
    }

private:
    void judgeWork();
    void work_stage1(judge_Queue_node & jn); //第一个阶段工作
    void work_stage2(judge_Queue_node & jn); //第二个阶段工作
    std::mutex mtx; //锁
    std::condition_variable _task_cv; //条件
    std::vector<std::thread> _workPool; //工作线程池
    std::atomic<int> ThrNum; //空闲线程数量
    std::deque<judge_Queue_node> q; //队列
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

    while ( 1 ) {
        judge_Queue_node jn;
        {
            std::unique_lock<std::mutex> lck(mtx);
            while( q.empty() )//队列为空,就挂起
            {
                std::cout << "go wait" << std::endl;
                _task_cv.wait(lck); 
            }
            jn = q.front();
            q.pop_front();
        }
        std::cout << "de judge queue succ" << std::endl;
        std::cout << jn.fd << std::endl;
        std::cout << jn.key << std::endl;
        std::cout << jn.code << std::endl;
        std::cout << jn.language << std::endl;
        std::cout << jn.pid << std::endl;
        if( jn.stage == JUDGE_STAGE::PREPARE){
            std::cout << "stage 1" << std::endl;
            work_stage1(jn);
        }
        else if (jn.stage == JUDGE_STAGE::JUDGING ) {
            std::cout << "stage 2" << std::endl; work_stage2(jn);
        }
        //TODO 输出数据的内容
    }
}

void judgeWorkPool::write_message(int fd,MessageResultJudge& msg){
    socketManagerRAII smra(fd);
    auto str = msg.dumps().to_string();
    std::cout << msg << std::endl;
    show_hex_code(msg.dumps());
    std::cout << str << std::endl;
    show_hex_code(str);
    std::cout << "fd : "<< fd << std::endl;
    socketBase::TcpWrite(fd,str.c_str(),str.length());
}
/*
 * 一: 准备阶段
 *      是否是支持的语言
 *      查找题目的位置,判断题目是否存在,并返回题目的相关信息
 *      创建评测的文件夹
 *      写入代码
 *      编译
 *      写入评测队列,进入评测阶段
 */
void judgeWorkPool::work_stage1(judge_Queue_node &jn){
    try {
        //1 是否是支持的语言
        if( is_sport_language(jn.language) == false){
            MessageResultJudge res(jn.key,judgeResult_id::INVALID_CONFIG,std::string("unsupport language : ") + jn.language);
            write_message(jn.fd, res);
            return;
        }
        //2 查找题目的位置,判断题目是否存在,并返回题目的相关信息
        if( jn.problem_path.length() == 0){
            Problem p(__CONFIG::BASE_PROBLEM_PATH,jn.pid);
            //for (const auto& e : p.input_data) {
                //std::cout << e.first<< " " << e.second << std::endl;
            //}
            //for (const auto& e : p.output_data) {
                //std::cout << e.first<< " " << e.second << std::endl;
            //}
        }
        else
            Problem p(jn.problem_path);
        //3 创建评测的文件夹 写入代码
        std::string uuid = UUID(); //生成uuid
        std::cout << "uuid " << uuid << std::endl;
        auto work_path = fs::path(__CONFIG::BASE_WORK_PATH) / uuid;
        std::filesystem::create_directories(work_path);

        const std::string code_name  = "main.code"; // 代码名

        auto code_path = work_path / code_name ;
        writeFile(code_path.c_str(), jn.code);
        //std::cout << "uuid " << uuid << std::endl;
        // 4 编译
        result res = __judger(compile_CPP_args(work_path, code_name));
    }
    catch(std::exception & e){
        std::cerr << " Exception : " << e.what() << "\n";
    }
}


//TODO
void judgeWorkPool::work_stage2(judge_Queue_node &jn){
}


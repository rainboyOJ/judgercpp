#include <iostream>
#include <thread>
#include <chrono>
#include <utility>
#include <string>
#include <fstream>
#include <unistd.h> 
#include <variant>
#include <vector>
#include <algorithm>
#include <iomanip>

#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <grp.h>
#include <signal.h>

#include "commandline.hpp"
#include "judger.h"



#define UNLIMITED 0

// 全局变量
//// == for arguments
CommandLine cmd("judger for noier and acmer.");

bool     help                    = false;
bool     show_version            = false;

uint32_t version = 1;

//// 程序比较运行需要的变量

struct config {
    uint32_t    max_cpu_time{UNLIMITED};
    uint32_t    max_real_time{UNLIMITED};
    uint32_t    max_process_number{UNLIMITED};
    bool        memory_limit_check_only{false};
    uint64_t    max_output_size{UNLIMITED};
    uint64_t    max_memory{UNLIMITED};
    uint64_t    max_stack{UNLIMITED};
    std::string cwd;
    std::string exe_path{"1"};
    std::string input_path{"/dev/stdin"};
    std::string output_path{"/dev/stdout"};
    std::string error_path{"/dev/stderr"};
    std::vector<std::string> args{};
    std::vector<std::string> env{};
    std::string log_path{"judger_log.txt"};
    std::string seccomp_rule_name;
    uint32_t     uid{65534};
    uint32_t     gid{65534};
} CONFIG;


result RESULT;

// ==================== utils
template<char Delimiter = ' ',typename... Args>
void debug_out(std::ostream &os, Args&&... args){
    ( (os << args << Delimiter),... ) <<std::endl;
}




// ==================== utils end
// ==================== 子进程处理
//
template <class C, typename T>
T getPointerType(T C::*v);

template<typename CONFIG,typename Member,
    typename LIMIT_Type,
    typename pointTotype = decltype(getPointerType(std::declval<Member>()))
>
bool __do_serlimit(CONFIG& c,Member m,
        pointTotype v,LIMIT_Type limit){
    if(c.*m != UNLIMITED){
        struct rlimit LIMIT;
        LIMIT.rlim_cur = LIMIT.rlim_max = static_cast<rlim_t>(v);
        return setrlimit(limit, &LIMIT) == 0;
    }
    return true;
}

#define do_serlimit(Member,LIMIT_Type)\
    if( not __do_serlimit(_config,&config::Member,_config.Member,LIMIT_Type) )\
            CHILD_ERROR_EXIT(SETRLIMIT_FAILED);

#define do_serlimit_new_value(Member,LIMIT_Type,value)\
    if( not __do_serlimit(_config,&config::Member,value,LIMIT_Type) )\
            CHILD_ERROR_EXIT(SETRLIMIT_FAILED);



void child_process(config & _config){
    FILE *input_file = NULL, *output_file = NULL, *error_file = NULL;

    if( _config.cwd.length() != 0){
        chdir(_config.cwd.c_str());
    }
    // 栈空间
    do_serlimit(max_stack,RLIMIT_STACK)

    // 内存
    if( not _config.memory_limit_check_only ){
        do_serlimit_new_value(max_memory,RLIMIT_AS,_config.max_memory*2)
    }

    //cpu time
    do_serlimit_new_value(max_cpu_time, RLIMIT_CPU,(_config.max_cpu_time+1000)/1000 )

    //
    do_serlimit(max_process_number, RLIMIT_NPROC)

    do_serlimit(max_output_size, RLIMIT_FSIZE)

    if(_config.input_path.length() != 0){
        input_file = fopen(_config.input_path.c_str(), "r");
        if (input_file == NULL) {
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }

        if( dup2(fileno(input_file),fileno(stdin)) == -1){
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }
    }

    if(_config.output_path.length() != 0){
        output_file = fopen(_config.output_path.c_str(), "w");
        if (output_file == NULL) {
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }

        if( dup2(fileno(output_file),fileno(stdout)) == -1){
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }
    }


    if (_config.error_path.length() != 0 ) {
        // if outfile and error_file is the same path, we use the same file pointer
        if (_config.output_path.length() != 0 &&_config.output_path == _config.error_path) {
            error_file = output_file;
        }
        else {
            error_file = fopen(_config.error_path.c_str(), "w");
            if (error_file == NULL) {
                CHILD_ERROR_EXIT(DUP2_FAILED);
            }
        }
        // redirect stderr -> file
        if (dup2(fileno(error_file), fileno(stderr)) == -1) {
            CHILD_ERROR_EXIT(DUP2_FAILED);
        }
    }

#ifndef LOCAL
    //set gid
    gid_t group_list[] = {_config.gid};
    if (_config.gid != -1 && (setgid(_config.gid) == -1 || setgroups(sizeof(group_list) / sizeof(gid_t), group_list) == -1)) {
        CHILD_ERROR_EXIT(SETUID_FAILED);
    }
#endif

#ifndef LOCAL
    //set uid
    if (_config.uid != -1 && setuid(_config.uid) == -1) {
        CHILD_ERROR_EXIT(SETUID_FAILED);
    }
#endif

    char * args[256]{NULL},* env[256]{NULL};
    args[0] = _config.exe_path.data();

    for(int i = 0 ;i < 256 && i < _config.args.size(); ++i)
        args[i+1] = _config.args[i].data();
    for(int i = 0 ;i < 256 && i < _config.env.size(); ++i)
        env[i] = _config.env[i].data();

    #ifdef DEBUG
    //std::cout << "==args=="  << std::endl;
        //for(int i=0;i<256;++i){
            //if( args[i] != NULL)
                //std::cout << args[i] << std::endl;
        //}
    #endif

    execve(_config.exe_path.c_str(), args,env);
    CHILD_ERROR_EXIT(EXECVE_FAILED);

}

// ==================== Function
void run(config & _config, result &_result) {

#ifndef LOCAL
    // check whether current user is root
    uid_t uid = getuid();
    if (uid != 0) {
        ERROR_EXIT(ROOT_REQUIRED);
    }
#endif
        
    // TODO check arguments



  //std::chrono::time_point< std::chrono::system_clock > 
      auto time_start = std::chrono::system_clock::now();

    pid_t child_pid = fork();
    if( child_pid < 0 ){
        ERROR_EXIT(FORK_FAILED);
    }
    else if (child_pid == 0){
        // child_process
        child_process(_config);
    }
    else if ( child_pid > 0){
        int status;
        struct rusage resource_usage;
        //wait4 等到pid进程结束,并得到resource https://man7.org/linux/man-pages/man2/wait4.2.html
        // WSTOPPED 直到stop https://man7.org/linux/man-pages/man2/wait.2.html
        if( wait4(child_pid, &status, WSTOPPED, &resource_usage) == -1) {
            kill(child_pid,SIGKILL);
            ERROR_EXIT(WAIT_FAILED);
        }



        //std::chrono::time_point< std::chrono::system_clock > 
        auto time_end = std::chrono::system_clock::now();

        std::chrono::duration<double, std::milli>(time_start
                - time_end).count();
 

//WIFSIGNALED(wstatus) returns true if the child process was terminated by a signal.
              // WTERMSIG(wstatus)
              //    returns the number of the signal that caused the child
              //    process to terminate.  This macro should be employed only
              //    if WIFSIGNALED returned true.
        if (WIFSIGNALED(status) != 0) {
            _result.signal = WTERMSIG(status); //被哪个信号关闭的
        }

        if(_result.signal == SIGUSR1) {
            _result.result = SYSTEM_ERROR; // 为什么SIGUSR1 表示系统错误
        }
        else {
            _result.exit_code = WEXITSTATUS(status);
            _result.cpu_time = (int) (resource_usage.ru_utime.tv_sec * 1000 +
                                       resource_usage.ru_utime.tv_usec / 1000);
            _result.memory = resource_usage.ru_maxrss * 1024; // kb to bytes

            if (_result.exit_code != 0) {
                _result.result = RUNTIME_ERROR;
            }

            if (_result.signal == SIGSEGV) { // 段错误
                if (_config.max_memory != UNLIMITED && _result.memory > _config.max_memory) {
                    _result.result = MEMORY_LIMIT_EXCEEDED; //超内存
                }
                else {
                    _result.result = RUNTIME_ERROR; // 运行错误
                }
            }
            else {
                if (_result.signal != 0) {
                    _result.result = RUNTIME_ERROR;
                }
                if (_config.max_memory != UNLIMITED && _result.memory > _config.max_memory) {
                    _result.result = MEMORY_LIMIT_EXCEEDED;
                }
                if (_config.max_real_time != UNLIMITED && _result.real_time > _config.max_real_time) {
                    _result.result = REAL_TIME_LIMIT_EXCEEDED;
                }
                if (_config.max_cpu_time != UNLIMITED && _result.cpu_time > _config.max_cpu_time) {
                    _result.result = CPU_TIME_LIMIT_EXCEEDED;
                }
            }
        }
    }
}
// ==================== Function end



int main(int argc,char *argv[]){


    // ================= 命令行解析 功能
    // example :
    //  judger -h
    //  judger -t=1000 -e 1         设置时间为1000ms 执行程序 1
    //  judger --max_cpu_time=1000 -e 1
    cmd.addArgument({"-h", "--help"},    &help,    "输出帮助,然后退出。");
    cmd.addArgument({"-v", "--version"}, &show_version, "输出版本号,然后退出。");
    cmd.addArgument({"-t","--max_cpu_time"}, &CONFIG.max_cpu_time,  "程序的最大CPU运行时间 (ms),0 表示没有限制,默认为0。");
    cmd.addArgument({"-r","--max_real_time"}, &CONFIG.max_real_time,  "程序的最大实际时间(ms),0表示没有限制,默认为0。");
    cmd.addArgument({"-m","--max_memory"}, &CONFIG.max_memory,  "程序运行的内存限制(byte),默认512M。");
    cmd.addArgument({"-only","--memory_limit_check_only"}, &CONFIG.memory_limit_check_only,  "只检查内存使用，不加以限制，默认为False");
    cmd.addArgument({"-s","--max_stack"}, &CONFIG.max_stack,  "栈空间限制,默认没有限制");
    cmd.addArgument({"-p","--max_process_number"}, &CONFIG.max_process_number,  "最大进程数量，默认没有限制");
    cmd.addArgument({"-os","--max_output_size"}, &CONFIG.max_output_size,  "最大输出大小(byte)");

    cmd.addArgument({"--cwd"}, &CONFIG.cwd, "运行的评测程序的work_path");
    cmd.addArgument({"-e","--exe_path"}, &CONFIG.exe_path, "要判断的程序的路径");
    cmd.addArgument({"-i","--input_path"}, &CONFIG.input_path,  "输入文件路径");
    cmd.addArgument({"-o","--output_path"}, &CONFIG.output_path,  "输出文件路径");
    cmd.addArgument({"-ep","--error_path"}, &CONFIG.error_path,  "错误输出路径");

    cmd.addArgument({"--args"}, &CONFIG.args,  "程序运行的参数表"); // TODO
    cmd.addArgument({"--env"}, &CONFIG.env,  "环境表");

    cmd.addArgument({"--log_path"}, &CONFIG.log_path,  "日志路径,默认 judge_log.txt");
    cmd.addArgument({"-rule","--seccomp_rule_name"}, &CONFIG.seccomp_rule_name,  "Seccomp Rule Name");

//#ifndef LOCAL //local也接收这个参数 但不会产生任何效果
    cmd.addArgument({"-u","--uid"}, &CONFIG.uid,  "UID (default 65534)");
    cmd.addArgument({"-g","--gid"}, &CONFIG.gid,  "GID (default 65534)");
//#endif
    cmd.parse(argc, argv);

    if( help ){
        cmd.printHelp();
        return 0;
    }
    else if( show_version ){
        std::cout << "version: "  << version << std::endl;
        return 0;
    }

    LOG_INIT(CONFIG.log_path.c_str());

    
    #ifdef DEBUG
#define print_config(node) std::cout << std::setw(25) << #node ":" << '\t'<< CONFIG.node << '\n'
        print_config(max_cpu_time);
        print_config(max_real_time);
        print_config(max_process_number);
        print_config(memory_limit_check_only);
        print_config(max_output_size);
        print_config(max_memory);
        print_config(max_stack);
        print_config(exe_path);
        print_config(input_path);
        print_config(output_path);
        print_config(error_path);
        print_config(log_path);
        print_config(seccomp_rule_name);
        print_config(seccomp_rule_name);
        print_config(uid);
        print_config(gid);

        std::cout << "--args count " << CONFIG.args.size() << std::endl ;
        for (const auto& e : CONFIG.args) {
            std::cout << "     " ;
            std::cout << e << std::endl;
        }
        std::cout << "\n\n" ;

        std::cout << "--env count " << CONFIG.env.size() << std::endl ;
        for (const auto& e : CONFIG.env) {
            std::cout << "     " ;
            std::cout << e << std::endl;
        }
        std::cout << "\n\n" ;
    #endif
    // ================= 命令行解析 功能 结束

    run(CONFIG, RESULT);
    print_result();
    return 0;
}

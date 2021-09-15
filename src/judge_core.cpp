#include <thread>
#include <string>

#include "common.h"

#define UNLIMITED 0
// 全局变量
//// == for arguments
CommandLine cmd("judger for noier and acmer.");

bool     help                    = false;
bool     show_version            = false;

uint32_t version = 1;

//// 程序比较运行需要的变量
enum {
    SUCCESS             = 0,
    INVALID_CONFIG      = -1,
    FORK_FAILED         = -2,
    PTHREAD_FAILED      = -3,
    WAIT_FAILED         = -4,
    ROOT_REQUIRED       = -5,
    LOAD_SECCOMP_FAILED = -6,
    SETRLIMIT_FAILED    = -7,
    DUP2_FAILED         = -8,
    SETUID_FAILED       = -9,
    EXECVE_FAILED       = -10,
    SPJ_ERROR           = -11
};

struct config {
    uint32_t    max_cpu_time{UNLIMITED};
    uint32_t    max_real_time{UNLIMITED};
    uint32_t    max_process_number{UNLIMITED};
    bool        memory_limit_check_only{false};
    uint64_t    max_output_size{UNLIMITED};
    uint64_t    max_memory{UNLIMITED};
    uint64_t    max_stack{UNLIMITED};
    std::string exe_path{"1"};
    std::string input_path{"/dev/stdin"};
    std::string output_path{"/dev/stdout"};
    std::string error_path{"/dev/stderr"};
    std::vector<std::string> args{};
    std::vector<std::string> env{};
    std::string log_path{"judger_log.txt"};
    std::string seccomp_rule_name;
    int32_t     uid{65534};
    int32_t     gid{65534};
} CONFIG;


enum {
    WRONG_ANSWER             = -1,
    CPU_TIME_LIMIT_EXCEEDED  = 1,
    REAL_TIME_LIMIT_EXCEEDED = 2,
    MEMORY_LIMIT_EXCEEDED    = 3,
    RUNTIME_ERROR            = 4,
    SYSTEM_ERROR             = 5
};

// 存结果 POD
struct result {
    int cpu_time;
    int real_time;
    long memory;
    int signal;
    int exit_code;
    int error;
    int result;
} RESULT;

// ==================== utils
template<typename... Args>
void debug_out(Args&&... args){
    (std::cout << ... << args) <<std::endl;
}
// ==================== utils end

// ==================== Function
void run(config const & _config, result &_result) {
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

    cmd.addArgument({"-e","exe_path"}, &CONFIG.exe_path, "要判断的程序的路径");
    cmd.addArgument({"-i","input_path"}, &CONFIG.input_path,  "输入文件路径");
    cmd.addArgument({"-o","output_path"}, &CONFIG.output_path,  "输出文件路径");
    cmd.addArgument({"-ep","error_path"}, &CONFIG.error_path,  "错误输出路径");

    cmd.addArgument({"--args"}, &CONFIG.args,  "程序运行的参数表"); // TODO
    cmd.addArgument({"--env"}, &CONFIG.env,  "环境表");

    cmd.addArgument({"--log_path"}, &CONFIG.log_path,  "日志路径,默认 judge_log.txt");
    cmd.addArgument({"-rule","--seccomp_rule_name"}, &CONFIG.seccomp_rule_name,  "Seccomp Rule Name");

    cmd.addArgument({"-u","--uid"}, &CONFIG.uid,  "UID (default 65534)");
    cmd.addArgument({"-g","--gid"}, &CONFIG.gid,  "GID (default 65534)");
    cmd.parse(argc, argv);

    if( help ){
        cmd.printHelp();
        return 0;
    }
    else if( show_version ){
        std::cout << "version: "  << version << std::endl;
        return 0;
    }

    
    #ifdef DEBUG
        std::cout << "max_cpu_time "<<" "<< CONFIG.max_cpu_time << std::endl;
        std::cout << "log_path"<<" "<< CONFIG.log_path << std::endl;

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
    return 0;
}

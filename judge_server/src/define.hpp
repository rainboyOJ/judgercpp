#pragma once
#include <filesystem>
#include <string_view>

using namespace std::literals;
namespace fs = std::filesystem;

using ull = unsigned long long;
constexpr ull operator ""_MB(ull a){
    return a*1024*1024*1024;
}

constexpr std::size_t operator ""_SEC(ull a){
    return a*1000;
}

const int unlimit = 0;

/* 执行judge出现的结果 */
enum judgeResult_id {
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
    SPJ_ERROR           = -11,
    COMPILE_FAIL        = -12 // TODO
};

enum RESULT_MEAN {
    WRONG_ANSWER             = -1,
    CPU_TIME_LIMIT_EXCEEDED  = 1,
    REAL_TIME_LIMIT_EXCEEDED = 2,
    MEMORY_LIMIT_EXCEEDED    = 3,
    RUNTIME_ERROR            = 4,
    SYSTEM_ERROR             = 5
};

enum class STATUS : int {
    WAITING,
    PROBLEM_ERROR,
    PROBLEM_DATA_NOT_EXISTS,
    JUDGING,
    COMPILE_ERROR,
    END
};

enum class SUPORT_LANG {
    CPP,
    PY3,
    UNSUPORT
};

enum class JUDGE_STAGE: int {
    PREPARE = 1,    //准备
    JUDGING = 2     //评测
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
};

#define __print_result(node,RESULT) std::cout << std::setw(12) << #node ": " << RESULT.node <<'\n';
#define print_result(RESULT)\
    __print_result(cpu_time,RESULT);\
    __print_result(real_time,RESULT);\
    __print_result(memory,RESULT);\
    __print_result(signal,RESULT);\
    __print_result(exit_code,RESULT);\
    __print_result(error,RESULT);\
    __print_result(result,RESULT);


std::string_view result_to_string(RESULT_MEAN mean) {
    using namespace std::literals;
    switch(mean){
        case WRONG_ANSWER:              return "WRONG_ANSWER"sv;
        case CPU_TIME_LIMIT_EXCEEDED:   return "CPU_TIME_LIMIT_EXCEEDED"sv;
        case REAL_TIME_LIMIT_EXCEEDED:  return "REAL_TIME_LIMIT_EXCEEDED"sv;
        case MEMORY_LIMIT_EXCEEDED:     return "MEMORY_LIMIT_EXCEEDED"sv;
        case RUNTIME_ERROR:             return "RUNTIME_ERROR"sv;
        case SYSTEM_ERROR:              return "SYSTEM_ERROR"sv;
        default:    return "UNKOWN"sv;
    }
}

std::string_view lang_to_string(SUPORT_LANG lang) {
    using namespace std::literals;
    switch(lang){
        case SUPORT_LANG::CPP:                   return "cpp"sv;
        case SUPORT_LANG::PY3:                   return "python3"sv;
        default:    return "UNSUPORT"sv;
    }
}




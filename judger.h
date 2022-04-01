#pragma once

#include <string>

#define UNLIMITED 0

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

enum RESULT_MEAN {
    WAIT                     = -2,
    WRONG_ANSWER             = -1,
    ACCEPT                   = 0,
    CPU_TIME_LIMIT_EXCEEDED  = 1,
    REAL_TIME_LIMIT_EXCEEDED = 2,
    MEMORY_LIMIT_EXCEEDED    = 3,
    RUNTIME_ERROR            = 4,
    SYSTEM_ERROR             = 5
};

const std::string __mean__[5]{
    "CPU_TIME_LIMIT_EXCEEDED",
    "REAL_TIME_LIMIT_EXCEEDED",
    "MEMORY_LIMIT_EXCEEDED",
    "RUNTIME_ERROR",
    "SYSTEM_ERROR"
};

std::string result_to_string(RESULT_MEAN mean) {
    if( mean >= RESULT_MEAN::CPU_TIME_LIMIT_EXCEEDED and mean <= RESULT_MEAN::SYSTEM_ERROR)
        return __mean__[mean-1];
    return "WRONG_ANSWER";
}

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

//// 程序比较运行需要的变量

struct config {
    uint32_t    max_cpu_time{UNLIMITED};
    uint32_t    max_real_time{UNLIMITED};
    uint32_t    max_process_number{UNLIMITED};
    bool        memory_limit_check_only{false}; //只检查内存使用，不加以限制，默认为False
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

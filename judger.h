#pragma once

#include <string>

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
    WRONG_ANSWER             = -1,
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

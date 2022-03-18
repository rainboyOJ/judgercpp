/**
 * @desc
 * 评测参数
 */
#pragma once
#include <iostream>
#include "define.hpp"

//基类
struct judge_args {
    judge_args() = default;
    explicit judge_args(
            std::size_t max_cpu_time, std::size_t max_real_time, std::size_t max_process_number, std::size_t max_memory, std::size_t max_stack, std::size_t max_output_size,
            std::string seccomp_rule_name,
            const fs::path& cwd, const fs::path& input_path, const fs::path& output_path, const fs::path& error_path, const fs::path& log_path, const fs::path& exe_path,
            int gid, int uid);
std::size_t max_cpu_time;
    std::size_t max_real_time;
    std::size_t max_process_number;
    std::size_t max_memory;//  512mb
    std::size_t max_stack;
    std::size_t max_output_size;
    std::string seccomp_rule_name;
    fs::path cwd;
    fs::path input_path;
    fs::path output_path;
    fs::path error_path;
    fs::path log_path;
    fs::path exe_path;

    std::vector<std::string> args;
    std::vector<std::string> env;
    int gid;
    int uid;

    operator std::string() const; //转成字符串,命令行参数
};

    //参数 编译的目录 评测的代码名字
#define create_compile_args(name) struct compile_##name##_args : public judge_args {\
        compile_##name##_args() = delete;\
        explicit compile_##name##_args(const fs::path& cwd,std::string_view code_name);\
    };\

    create_compile_args(PY3)
    create_compile_args(CPP)

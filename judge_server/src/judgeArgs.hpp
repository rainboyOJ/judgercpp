/**
 * @desc
 * 评测参数
 */
#pragma once
#include <iostream>
#include <vector>

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

//python3的编译
create_compile_args(PY3)

//cpp的编译
create_compile_args(CPP)


//========================== 实现

judge_args::judge_args(
                std::size_t max_cpu_time, std::size_t max_real_time, std::size_t max_process_number, std::size_t max_memory, std::size_t max_stack, std::size_t max_output_size,
                std::string seccomp_rule_name,
                const fs::path& cwd, const fs::path& input_path, const fs::path& output_path, const fs::path& error_path, const fs::path& log_path, const fs::path& exe_path,
                //std::vector<std::string> args,
                //std::vector<std::string> env,
                int gid, int uid)
            : max_cpu_time{max_cpu_time},
            max_real_time{max_real_time},
            max_process_number{max_process_number},
            max_memory{max_memory},
            max_stack{max_stack},
            max_output_size{max_output_size},
            seccomp_rule_name{std::move(seccomp_rule_name)},
            cwd{cwd},
            exe_path{exe_path},
            input_path{input_path},
            output_path{output_path},
            error_path{error_path},
            log_path{log_path},
            gid{gid},
            uid{uid} {}

judge_args::operator std::string() const { //转成string
            std::stringstream args_str;
            args_str << ' ';

            args_str <<"--max_cpu_time="<<max_cpu_time << ' ';
            args_str <<"--max_real_time="<<max_real_time<< ' ';
            args_str <<"--max_process_number="<<max_process_number<< ' ';
            args_str <<"--max_memory="<<max_memory<< ' ';
            args_str <<"--max_stack="<<max_stack<< ' ';
            args_str <<"--max_output_size="<<max_output_size<< ' ';
            args_str <<"--seccomp_rule_name="<<seccomp_rule_name<< ' ';

            args_str << "--cwd=" << cwd << ' ';
            args_str << "--input_path=" << input_path<< ' ';
            args_str << "--output_path=" << output_path<< ' ';
            args_str << "--error_path=" << error_path<< ' ';
            args_str << "--log_path=" << log_path<< ' ';
            args_str << "--exe_path=" << exe_path<< ' ';

            for (const auto& e : args) {
                args_str << "--args=" <<'"'  << e << '"' << ' ';
            }
            for (const auto& e : env) {
                args_str << "--env=" <<'"'  << e << '"' << ' ';
            }
            args_str << "--gid=" << gid << ' ';
            args_str << "--uid=" << uid << ' ';
            return args_str.str();
}

compile_PY3_args::compile_PY3_args (const fs::path& cwd,std::string_view code_name)
            :judge_args(
                    10*1000,90*1000,unlimit,unlimit,unlimit,128ull*(1ull<<30),
                    "null", //不使用sec
                    cwd,"/dev/null",
                    cwd/"compile_output",
                    cwd/"compile_error",
                    cwd/"compile_log",
                    "/usr/bin/python3",
                    0,0)
        {
            args = { "-m", "py_compile",cwd/code_name};
            env = {"PATH=/usr/bin"};
        }

compile_CPP_args::compile_CPP_args (const fs::path& cwd,std::string_view code_name)
            :judge_args(
                    10*1000,90*1000,unlimit,unlimit,unlimit,512ull*(1ull<<30),
                    "null", //不使用sec
                    cwd,"/dev/null",
                    cwd/"compile_output",
                    cwd/"compile_error",
                    cwd/"compile_log",
                    "/usr/bin/g++",
                    0,0)
        {
            args = {"-x","c++" ,"-o",fs::path(code_name).stem(),std::string(code_name)};
            env = {"PATH=/usr/bin"};
        }

/**
 * 用户定义
 */
#pragma once
#include <filesystem>
#include <string_view>
#include <string>

namespace fs = std::filesystem;

struct __CONFIG {
    //基础题目路径
    static constexpr std::string_view BASE_PROBLEM_PATH = "/home/rainboy/mycode/RainboyOJ/problems/problems";
};

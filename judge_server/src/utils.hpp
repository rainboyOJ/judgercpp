#pragma once

/**
 * @desc 向str的末尾添加val数字的字节
 */

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <cassert>

#include "define.hpp"



template<typename T>
void show_hex_code(T&& obj){
    for (const auto& e : obj) {
        std::cout << std::hex << (e & 0xff)  << " ";
    }
    std::cout << "\n";
}

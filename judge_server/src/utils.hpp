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
    std::ios_base::fmtflags save = std::cout.flags();
    for (const auto& e : obj) {
        std::cout << std::hex << (e & 0xff)  << " ";
    }
    
    std::cout  << "\n";
    std::cout.flags(save);

}

inline bool iequal(std::string_view s1,std::string_view s2) {
    if(s1.length() != s2.length() ) return  false;
    for (size_t i = 0; i < s1.length(); i++) {
        if (std::tolower(s1[i]) != std::tolower(s2[i]))
            return false;
    }
    return true;
}

//fold express
template<typename... T>
inline bool more_iequal(std::string_view s1 , T... args2 ){
    return ( iequal(s1, args2) ||...);
}

//字符串转 SUPORT_LANG
SUPORT_LANG string_to_lang(std::string_view lang){
    if( iequal(lang, "c++") || iequal(lang, "cpp") )
        return SUPORT_LANG::CPP;
    if( more_iequal(lang, "py","py3","python3","python"))
        return SUPORT_LANG::CPP;
    return SUPORT_LANG::UNSUPORT;
}

//是否是支持的语言
inline bool is_sport_language(std::string_view lang){
    return  string_to_lang(lang) != SUPORT_LANG::UNSUPORT;
}


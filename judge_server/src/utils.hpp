#pragma once

/**
 * @desc 向str的末尾添加val数字的字节
 */

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <cassert>
#include <fstream>

#include "sole.hpp"

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


//
std::string UUID(){
    return sole::uuid0().str();
}


//namespace fs = std::filesystem;
//struct Make_code_path {
    //explicit Make_code_path(std::string_view _path)
        //: base_path{_path}
    //{}

    //std::filesystem::path operator()(int id,std::string_view lang) const{
        //return base_path / std::to_string(id) 
            /// (std::string("main") + std::get<1>(string_to_lang(lang)));
    //}

    //std::filesystem::path base_path;
//};

//读取文件
std::string readFile(const char * path){
    std::stringstream ss;
    std::ifstream f(path);
    ss << f.rdbuf();
    return ss.str();
}

//写入文件
bool writeFile(const char * path,std::string_view code){
    std::ofstream file(path) ;
    file << code;
}

////把代码写入到对应的文件里
//void write_code(fs::path& path,std::string_view lang,std::string_view code ){
    //writeFile((path / ).c_str(), code);
//}


//执行程序
void exec(const char* cmd,std::ostream& __out) {
    char buffer[128];
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error(std::string("popen() failed!") + __FILE__ + " line: " +  std::to_string(__LINE__).c_str());
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            __out << buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    //return result;
}



// 文件夹RAII

struct directoryRAII {
    directoryRAII(fs::path &_p) 
        : __p{_p}
    { //创建dir
        std::filesystem::create_directories(_p);
    }
    ~directoryRAII() { // dele dir
        std::filesystem::remove_all(__p); // Deletes one or more files recursively.
    }

    fs::path __p;
};

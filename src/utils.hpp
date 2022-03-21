#pragma once

#ifdef DEBUG
//#define __print_result(node) debug_out<'\0'>(std::cout,#node ":","\t\t",RESULT.node)
#define __print_result(node) std::cout << std::setw(12) << #node ": " << RESULT.node <<'\n';

#else
#define __print_result(node) debug_out(std::cout,RESULT.node)
#endif

#define print_result()\
    __print_result(cpu_time);\
    __print_result(real_time);\
    __print_result(memory);\
    __print_result(signal);\
    __print_result(exit_code);\
    __print_result(error);\
    __print_result(result);

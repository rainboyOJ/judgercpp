/**
 * 没有发送的数据的dump 与 load
 */

#include <cstdio>
#include <iostream>
#include "../src/Result.hpp"

int main(){
    MessageResultJudge res(COMPILE_FAIL,"compile error,hello world");
    res.push_back(1,2,3,4,5,6,7);
    res.push_back(1,2,3,4,5,6,7);
    res.push_back(1,2,3,4,5,6,7);
    res.push_back(1,2,3,4,5,6,7);
    res.push_back(1,2,3,4,5,6,7);
    std::cout << res;
    std::cout  << std::endl;

    return 0;
}

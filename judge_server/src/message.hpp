
/**
 * @desc 基础的类 序列与反序化
 */
#pragma once

#include <map>
#include <vector>
#include "base64.hpp"

class baseMessage {
public:
    std::string loads(); //反序列化
    std::string dumps(); //序列化
protected:
    std::vector<std::pair<std::string, std::string>> key_value;
};

std::string baseMessage::dumps() {
    std::ostringstream oss;
    for (const auto& e : key_value) {
        oss << e.first << " " << e.second << '\n';

    }
}

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <sstream>

// 工具类：处理时间解析等辅助功能
class Utils {
public:
    // 解析时间戳 [HH:MM:SS] -> 秒数 (从当天0点开始)
    static long long parseTimestamp(const std::string& timeStr) {
        // 格式示例: [00:01:00]
        // TODO: 解析字符串，转换为秒数返回
        // 简单实现提示: sscanf 或 string::substr + stoi
        return 0; 
    }
};

#endif // UTILS_HPP

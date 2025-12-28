#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <sstream>

// 工具类：帮我们处理时间转换
class Utils {
public:
    // 把 [00:01:00] 这种格式的时间变成秒数
    static long long parseTimestamp(const std::string& timeStr) {
        long long hours, minutes, seconds;
        // 用 sscanf 快速把数字抠出来
        if(sscanf(timeStr.c_str(), "[%lld:%lld:%lld]", &hours, &minutes, &seconds) == 3) {
            return hours * 3600 + minutes * 60 + seconds;
        }
        return 0; 
    }

    // 把秒数变回 [00:01:00] 这种格式
    static std::string formatTimestamp(long long totalSeconds) {
        if (totalSeconds < 0) totalSeconds = 0;
        long long hours = totalSeconds / 3600;
        long long minutes = (totalSeconds % 3600) / 60;
        long long seconds = totalSeconds % 60;
        char buf[32];
        // 格式化成两位数，不够补0
        sprintf(buf, "[%02lld:%02lld:%02lld]", hours, minutes, seconds);
        return std::string(buf);
    }
};

#endif // UTILS_HPP

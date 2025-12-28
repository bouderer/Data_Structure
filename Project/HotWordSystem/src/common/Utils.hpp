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
        long long hours, minutes, seconds;
        if(sscanf(timeStr.c_str(), "[%lld:%lld:%lld]", &hours, &minutes, &seconds) == 3) {
            return hours * 3600 + minutes * 60 + seconds;
        }
        return 0; 
    }

    // 将秒数转换回 [HH:MM:SS] 格式
    static std::string formatTimestamp(long long totalSeconds) {
        if (totalSeconds < 0) totalSeconds = 0; // 防止出现负数时间
        long long hours = totalSeconds / 3600;
        long long minutes = (totalSeconds % 3600) / 60;
        long long seconds = totalSeconds % 60;
        char buf[32];
        sprintf(buf, "[%02lld:%02lld:%02lld]", hours, minutes, seconds);
        return std::string(buf);
    }
};

#endif // UTILS_HPP

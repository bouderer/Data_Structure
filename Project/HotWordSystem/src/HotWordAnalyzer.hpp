#ifndef HOT_WORD_ANALYZER_HPP
#define HOT_WORD_ANALYZER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <deque>
#include <queue>
#include <algorithm>

// 核心分析类：负责滑动窗口管理和热词统计
class HotWordAnalyzer {
public:
    // 构造函数：设置窗口大小（秒）
    HotWordAnalyzer(int windowSizeSeconds) : windowSize(windowSizeSeconds), currentTime(0) {}

    // 接收新数据：传入时间戳和分词后的词列表
    void ingest(long long timestamp, const std::vector<std::string>& words) {
        // TODO: 1. 更新当前系统时间 currentTime
        // TODO: 2. 淘汰过期数据 (检查 eventQueue)
        // TODO: 3. 录入新数据 (更新 wordCounts 和 eventQueue)
    }

    // 获取 Top-K 热词
    std::vector<std::pair<std::string, int>> getTopK(int k) {
        std::vector<std::pair<std::string, int>> result;
        // TODO: 使用优先队列 (Min-Heap) 遍历 wordCounts 找出前 K 个
        return result;
    }

private:
    int windowSize; // 窗口大小
    long long currentTime; // 当前流逝到的最大时间

    // 词频统计表: word -> count
    std::unordered_map<std::string, int> wordCounts;

    // 事件队列: 用于记录每个词到达的时间，以便过期淘汰
    // pair<timestamp, word>
    std::deque<std::pair<long long, std::string>> eventQueue;
};

#endif // HOT_WORD_ANALYZER_HPP

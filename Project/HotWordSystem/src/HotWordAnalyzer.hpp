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
private:
    // 定义一个结构体来表示过期事件，方便在优先队列中排序
    struct ExpirationEvent {
        long long timestamp;
        std::string word;

        // 小顶堆需要定义 > 运算符 (时间越早，优先级越高)
        bool operator>(const ExpirationEvent& other) const {
            return timestamp > other.timestamp;
        }
    };

    int windowSize; // 窗口大小
    long long currentTime; // 当前流逝到的最大时间

    // 词频统计表: word -> count
    std::unordered_map<std::string, int> wordCounts;

    // 优先队列（小顶堆）：确保时间最早的词总是在堆顶，无论它什么时候进入系统
    std::priority_queue<ExpirationEvent, std::vector<ExpirationEvent>, std::greater<ExpirationEvent>> eventQueue;

public:
    // 构造函数：设置窗口大小（秒）
    HotWordAnalyzer(int windowSizeSeconds) : windowSize(windowSizeSeconds), currentTime(0) {}

    // 接收新数据：传入时间戳和分词后的词列表
    void ingest(long long timestamp, const std::vector<std::string>& words) {
        // 1. 更新当前系统时间
        currentTime = std::max(currentTime, timestamp);

        // 2. 录入新数据
        for (const auto& w : words) {
            wordCounts[w]++;
            eventQueue.push({timestamp, w});
        }

        // 3. 淘汰过期数据
        // 只要堆顶的时间戳早于 (当前时间 - 窗口大小)，就说明过期了
        while (!eventQueue.empty() && eventQueue.top().timestamp <= currentTime - windowSize) {
            std::string expiredWord = eventQueue.top().word;
            
            // 更新词频表
            if (wordCounts.count(expiredWord)) {
                wordCounts[expiredWord]--;
                if (wordCounts[expiredWord] <= 0) {
                    wordCounts.erase(expiredWord);
                }
            }
            eventQueue.pop();
        }
    }

    // 获取 Top-K 热词
    std::vector<std::pair<std::string, int>> getTopK(int k) {
        // 使用一个小顶堆来筛选出现次数最多的 K 个词
        //使用小顶堆，每次如果要加入新元素就和堆顶比较，堆顶小就弹出堆顶，加入新元素
        // 堆里存的是 pair<次数, 词>
        typedef std::pair<int, std::string> ScorePair;
        std::priority_queue<ScorePair, std::vector<ScorePair>, std::greater<ScorePair>> minHeap;

        for (auto const& it : wordCounts) {
            minHeap.push({it.second, it.first});
            if (minHeap.size() > (size_t)k) {
                minHeap.pop();
            }
        }

        // 将结果从堆中取出并反转
        std::vector<std::pair<std::string, int>> result;
        while (!minHeap.empty()) {
            result.push_back({minHeap.top().second, minHeap.top().first});
            minHeap.pop();
        }
        std::reverse(result.begin(), result.end());
        
        return result;
    }

    // 获取当前系统时间（窗口结束时间）
    long long getCurrentTime() const {
        return currentTime;
    }
};

#endif // HOT_WORD_ANALYZER_HPP

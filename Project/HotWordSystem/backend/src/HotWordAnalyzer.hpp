#ifndef HOT_WORD_ANALYZER_HPP
#define HOT_WORD_ANALYZER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <queue>
#include <algorithm>

// 核心分析类：负责统计热词和管理滑动窗口
class HotWordAnalyzer {
private:
    // 记录词语
    struct ExpirationEvent {
        long long timestamp;
        std::string word;

        // 比较函数，方便小顶堆排序
        bool operator>(const ExpirationEvent& other) const {
            return timestamp > other.timestamp;
        }
    };

    int windowSize; // 窗口大小（秒）
    long long currentTime; // 系统当前跑到了什么时间

    // 词频表：记录每个词出现了几次
    std::unordered_map<std::string, int> wordCounts;

    // 优先队列：帮我们快速找到过期的词
    std::priority_queue<ExpirationEvent, 
                        std::vector<ExpirationEvent>, 
                        std::greater<ExpirationEvent>> eventQueue;

    // 历史记录：存下所有的词，方便以后查询
    struct HistoryRecord {
        long long timestamp;
        std::string word;
    };
    std::vector<HistoryRecord> fullHistory;

    // 统一的淘汰逻辑
    void expireData() {
        while (!eventQueue.empty() && eventQueue.top().timestamp <= currentTime - windowSize) {
            std::string expiredWord = eventQueue.top().word;
            if (wordCounts.count(expiredWord)) {
                wordCounts[expiredWord]--;
                if (wordCounts[expiredWord] <= 0) {
                    wordCounts.erase(expiredWord);
                }
            }
            eventQueue.pop();
        }
    }

public:
    // 构造函数
    HotWordAnalyzer(int windowSizeSeconds) : windowSize(windowSizeSeconds), currentTime(0) {}

    // 存入新数据
    void ingest(
        long long timestamp, 
        const std::vector<std::string>& words) {
        // 更新当前时间
        currentTime = std::max(currentTime, timestamp);

        // 把新词加进去
        for (const auto& w : words) {
            wordCounts[w]++;
            eventQueue.push({timestamp, w});
            fullHistory.push_back({timestamp, w});
        }

        // 删掉过期的词
        expireData();
    }

    // 找出出现次数最多的前 K 个词
    std::vector<std::pair<std::string, int>> getTopK(int k) {
        // 用一个小顶堆来挑出最大的 K 个
        typedef std::pair<int, std::string> ScorePair;
        std::priority_queue<ScorePair, 
                            std::vector<ScorePair>, 
                            std::greater<ScorePair>> minHeap;

        for (auto const& it : wordCounts) {
            minHeap.push({it.second, it.first});
            if (minHeap.size() > (size_t)k) {
                minHeap.pop();
            }
        }

        // 把结果倒序排好（从大到小）
        std::vector<std::pair<std::string, int>> result;
        while (!minHeap.empty()) {
            result.push_back({minHeap.top().second, minHeap.top().first});
            minHeap.pop();
        }
        std::reverse(result.begin(), result.end());
        
        return result;
    }

    // 查询某个时间段内的热词
    std::vector<std::pair<std::string, int>> getHistoryTopK(
        long long startTime,
        long long endTime, 
        int k) {
        std::unordered_map<std::string, int> historyCounts;
        
        // 遍历所有历史记录，看看哪些在时间范围内
        for (const auto& record : fullHistory) {
            if (record.timestamp >= startTime && record.timestamp <= endTime) {
                historyCounts[record.word]++;
            }
        }

        // 同样的 Top-K 逻辑
        typedef std::pair<int, std::string> ScorePair;
        std::priority_queue<ScorePair, 
                            std::vector<ScorePair>, 
                            std::greater<ScorePair>> minHeap;

        for (auto const& it : historyCounts) {
            minHeap.push({it.second, it.first});
            if (minHeap.size() > (size_t)k) {
                minHeap.pop();
            }
        }

        std::vector<std::pair<std::string, int>> result;
        while (!minHeap.empty()) {
            result.push_back({minHeap.top().second, minHeap.top().first});
            minHeap.pop();
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    // 趋势分析：获取某个词在整个历史中的频率变化
    // 返回 pair<时间戳列表, 频率列表>
    std::pair<std::vector<long long>, std::vector<int>> getTrendData(
        const std::string& word, 
        int intervalSeconds) {
        if (fullHistory.empty()) return {};
        
        long long startTime = 0; // 从 0 点开始对齐
        long long endTime = currentTime;
        
        if (intervalSeconds <= 0) intervalSeconds = 60;
        
        int numBuckets = (endTime - startTime) / intervalSeconds + 1;
        std::vector<long long> times(numBuckets);
        std::vector<int> counts(numBuckets, 0);
        
        for (int i = 0; i < numBuckets; ++i) {
            // 横坐标显示时间段的结束点，比如 0-10 分钟显示 10 分钟
            times[i] = startTime + (i + 1) * intervalSeconds;
        }
        
        for (const auto& record : fullHistory) {
            if (record.word == word) {
                int bucket = (record.timestamp - startTime) / intervalSeconds;
                if (bucket >= 0 && bucket < numBuckets) {
                    counts[bucket]++;
                }
            }
        }
        return {times, counts};
    }

    // 获取当前系统时间（窗口结束时间）
    long long getCurrentTime() const {
        return currentTime;
    }

    // 获取“阶段性”Top-K 趋势：找出每个时间段内的 Top-K，并返回这些词的整体趋势
    std::unordered_map<std::string, std::pair<std::vector<long long>, std::vector<int>>> getGlobalTopKTrend(
        int k, 
        int intervalSeconds) {
        if (fullHistory.empty()) return {};
        
        long long startTime = 0;
        long long endTime = currentTime;
        if (intervalSeconds <= 0) intervalSeconds = 60;
        int numBuckets = (endTime - startTime) / intervalSeconds + 1;

        // 1. 统计每个桶内所有词的频率
        std::vector<std::unordered_map<std::string, int>> bucketStats(numBuckets);
        for (const auto& record : fullHistory) {
            int bucket = (record.timestamp - startTime) / intervalSeconds;
            if (bucket >= 0 && bucket < numBuckets) {
                bucketStats[bucket][record.word]++;
            }
        }

        // 2. 找出每个桶的 Top-K，并取并集
        std::unordered_set<std::string> topWordsUnion;
        for (int i = 0; i < numBuckets; ++i) {
            typedef std::pair<int, std::string> ScorePair;
            std::priority_queue<ScorePair, std::vector<ScorePair>, std::greater<ScorePair>> minHeap;
            for (auto const& it : bucketStats[i]) {
                minHeap.push({it.second, it.first});
                if (minHeap.size() > (size_t)k) minHeap.pop();
            }
            while (!minHeap.empty()) {
                topWordsUnion.insert(minHeap.top().second);
                minHeap.pop();
            }
        }

        // 3. 为并集中的每个词生成完整的趋势数据
        std::unordered_map<std::string, std::pair<std::vector<long long>, std::vector<int>>> result;
        for (const auto& word : topWordsUnion) {
            std::vector<long long> times(numBuckets);
            std::vector<int> counts(numBuckets, 0);
            for (int i = 0; i < numBuckets; ++i) {
                // 横坐标显示时间段的结束点
                times[i] = startTime + (i + 1) * intervalSeconds;
                if (bucketStats[i].count(word)) {
                    counts[i] = bucketStats[i][word];
                }
            }
            result[word] = {times, counts};
        }
        return result;
    }
    /* "爆发性"趋势分析
     */
    /*    // 获取“爆发性”趋势分析：基于 75 分位数准入和斜率计算
    // 返回：{时间轴, 阈值线, 单词数据(词->频率列表)}
    struct BurstTrendData {
        std::vector<long long> times;
        std::vector<int> thresholds;
        std::unordered_map<std::string, std::vector<int>> wordCounts;
    };

    BurstTrendData getBurstTrends(int intervalSeconds) {
        BurstTrendData result;
        if (fullHistory.empty()) return result;

        long long startTime = 0;
        long long endTime = currentTime;
        if (intervalSeconds <= 0) intervalSeconds = 60;
        int numBuckets = (endTime - startTime) / intervalSeconds + 1;

        // 1. 分桶统计
        std::vector<std::unordered_map<std::string, int>> bucketStats(numBuckets);
        for (const auto& record : fullHistory) {
            int bucket = (record.timestamp - startTime) / intervalSeconds;
            if (bucket >= 0 && bucket < numBuckets) {
                bucketStats[bucket][record.word]++;
            }
        }

        // 2. 计算每个桶的 75 分位数作为阈值
        result.times.resize(numBuckets);
        result.thresholds.resize(numBuckets);
        std::unordered_set<std::string> activeWords;

        for (int i = 0; i < numBuckets; ++i) {
            // 横坐标显示时间段的结束点
            result.times[i] = startTime + (i + 1) * intervalSeconds;
            
            if (bucketStats[i].empty()) {
                result.thresholds[i] = 0;
                continue;
            }

            std::vector<int> freqs;
            for (auto const& it : bucketStats[i]) freqs.push_back(it.second);
            
            // 计算 75 分位数 (Top 25%)
            size_t n = freqs.size();
            size_t targetIdx = static_cast<size_t>(n * 0.75);
            if (targetIdx >= n) targetIdx = n - 1;
            std::nth_element(freqs.begin(), freqs.begin() + targetIdx, freqs.end());
            int p75 = freqs[targetIdx];
            result.thresholds[i] = p75;

            // 找出超过阈值的词
            for (auto const& it : bucketStats[i]) {
                if (it.second >= p75 && p75 > 0) {
                    activeWords.insert(it.first);
                }
            }
        }

        // 3. 提取这些活跃词在所有阶段的频率
        for (const auto& word : activeWords) {
            std::vector<int> counts(numBuckets, 0);
            for (int i = 0; i < numBuckets; ++i) {
                if (bucketStats[i].count(word)) {
                    counts[i] = bucketStats[i][word];
                }
            }
            result.wordCounts[word] = counts;
        }

        return result;
    } */

    // 获取当前滑动窗口的起止时间
    std::pair<long long, long long> getWindowBounds(long long time) const {
        return {time - windowSize, time};
    }

    // 动态设置窗口大小
    void setWindowSize(int newSizeSeconds) {
        windowSize = newSizeSeconds;
        // 缩小窗口后，可能需要立即淘汰一部分数据
        expireData();
    }
};

#endif // HOT_WORD_ANALYZER_HPP

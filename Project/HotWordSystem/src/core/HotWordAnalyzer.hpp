#ifndef HOT_WORD_ANALYZER_HPP
#define HOT_WORD_ANALYZER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
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

    // 全量历史数据：存储所有出现过的 (时间戳, 词)，用于历史范围查询
    struct HistoryRecord {
        long long timestamp;
        std::string word;
    };
    std::vector<HistoryRecord> fullHistory;

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
            fullHistory.push_back({timestamp, w}); // 存入全量历史
        }

        // 3. 淘汰过期数据
        expireData();
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

    // 历史范围查询：计算 [startTime, endTime] 之间的 Top-K
    std::vector<std::pair<std::string, int>> getHistoryTopK(long long startTime, long long endTime, int k) {
        std::unordered_map<std::string, int> historyCounts;
        
        // 遍历历史记录 (如果数据量极大，这里可以用二分查找优化起点)
        for (const auto& record : fullHistory) {
            if (record.timestamp >= startTime && record.timestamp <= endTime) {
                historyCounts[record.word]++;
            }
        }

        // 同样的 Top-K 逻辑
        typedef std::pair<int, std::string> ScorePair;
        std::priority_queue<ScorePair, std::vector<ScorePair>, std::greater<ScorePair>> minHeap;

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
    std::pair<std::vector<long long>, std::vector<int>> getTrendData(const std::string& word, int intervalSeconds) {
        if (fullHistory.empty()) return {};
        
        long long startTime = 0; // 从 0 点开始对齐
        long long endTime = currentTime;
        
        if (intervalSeconds <= 0) intervalSeconds = 60;
        
        int numBuckets = (endTime - startTime) / intervalSeconds + 1;
        std::vector<long long> times(numBuckets);
        std::vector<int> counts(numBuckets, 0);
        
        for (int i = 0; i < numBuckets; ++i) {
            times[i] = startTime + i * intervalSeconds;
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
    // 这能解决用户提到的“前十分钟最高的是abc，第二个十分钟是bcf”这种动态变化
    std::unordered_map<std::string, std::pair<std::vector<long long>, std::vector<int>>> getGlobalTopKTrend(int k, int intervalSeconds) {
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
                times[i] = startTime + i * intervalSeconds;
                if (bucketStats[i].count(word)) {
                    counts[i] = bucketStats[i][word];
                }
            }
            result[word] = {times, counts};
        }
        return result;
    }

    // 获取“爆发性”趋势分析：基于 75 分位数准入和斜率计算
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
            result.times[i] = startTime + i * intervalSeconds;
            
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
    }

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

private:
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
};

#endif // HOT_WORD_ANALYZER_HPP

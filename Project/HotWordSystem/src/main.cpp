#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib> // 用于 system()
#include "nlp/TextProcessor.hpp"
#include "core/HotWordAnalyzer.hpp"
#include "common/Utils.hpp"

using namespace std;

// 词典路径常量
const string DICT_PATH = "../dict/jieba.dict.utf8";
const string HMM_PATH = "../dict/hmm_model.utf8";
const string USER_DICT_PATH = "../dict/user.dict.utf8";
const string IDF_PATH = "../dict/idf.utf8";
const string STOP_WORD_PATH = "../dict/stop_words.utf8";
const string SENSITIVE_WORD_PATH = "../dict/sensitive_words.utf8";

int main(int argc, char** argv) {
    // 在 Windows 终端中强制使用 UTF-8 编码，解决中文乱码问题
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    // 1. 初始化模块
    cout << "Initializing Jieba..." << endl;
    TextProcessor processor(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH, SENSITIVE_WORD_PATH);
    
    // 默认窗口大小 600秒 (10分钟)
    HotWordAnalyzer analyzer(600); 

    // 决定输入源：如果有命令行参数则读文件，否则读标准输入
    istream* inputSource = &cin;
    ifstream fileStream;
    
    if (argc > 1) {
        fileStream.open(argv[1]);
        if (!fileStream.is_open()) {
            cerr << "Error: Cannot open input file: " << argv[1] << endl;
            return 1;
        }
        inputSource = &fileStream;
        cout << "Reading from file: " << argv[1] << endl;
    } else {
        cout << "System started. Reading from console (Press Ctrl+Z/D to stop)..." << endl;
    }

    string line;
    while (getline(*inputSource, line)) {
        if (line.empty()) continue;

        // 检查是否是指令行
        if (line.find("[ACTION]") != string::npos) {
            // 1. 处理查询指令: [ACTION] QUERY K=3
            if (line.find("[ACTION] QUERY") != string::npos) {
                size_t kPos = line.find("K=");
                if (kPos != string::npos) {
                    int k = stoi(line.substr(kPos + 2));
                    auto topK = analyzer.getTopK(k);
                    auto bounds = analyzer.getWindowBounds(analyzer.getCurrentTime());
                    
                    cout << "\n[QUERY RESULT] Window: " << Utils::formatTimestamp(bounds.first) 
                         << " ~ " << Utils::formatTimestamp(bounds.second) 
                         << " | Top-" << k << " Hot Words:" << endl;
                    if (topK.empty()) {
                        cout << "(No words in current window)" << endl;
                    } else {
                        for (size_t i = 0; i < topK.size(); ++i) {
                            cout << i + 1 << ". " << topK[i].first << " (" << topK[i].second << " times)" << endl;
                        }
                    }

                    // 打印敏感词统计
                    auto sensitiveCountsMap = processor.getAndClearSensitiveCounts();
                    if (!sensitiveCountsMap.empty()) {
                        // 将 map 转换为 vector 以便排序
                        vector<pair<string, int>> sortedSensitive(sensitiveCountsMap.begin(), sensitiveCountsMap.end());
                        sort(sortedSensitive.begin(), sortedSensitive.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
                            return a.second > b.second; // 按次数降序排列
                        });

                        cout << "\n[SENSITIVE_SUMMARY] ";
                        bool first = true;
                        for (const auto& pair : sortedSensitive) {
                            if (!first) cout << ", ";
                            cout << pair.first << " (" << pair.second << " times)";
                            first = false;
                        }
                        cout << endl;
                    }
                    cout << endl;
                }
            }
            // 4. 处理历史查询指令: [ACTION] HISTORY_QUERY START=[00:00:00] END=[00:10:00] K=5
            else if (line.find("[ACTION] HISTORY_QUERY") != string::npos) {
                auto getParam = [&](const string& key) -> string {
                    size_t pos = line.find(key);
                    if (pos == string::npos) return "";
                    size_t start = line.find('[', pos);
                    size_t end = line.find(']', pos);
                    if (start != string::npos && end != string::npos && end > start) {
                        return line.substr(start, end - start + 1);
                    }
                    return "";
                };

                string startStr = getParam("START=");
                string endStr = getParam("END=");
                
                int k = 5; // 默认值
                size_t kPos = line.find("K=");
                if (kPos != string::npos) {
                    string kVal = line.substr(kPos + 2);
                    size_t space = kVal.find(' ');
                    if (space != string::npos) kVal = kVal.substr(0, space);
                    try { k = stoi(kVal); } catch(...) {}
                }
                
                if (!startStr.empty() && !endStr.empty()) {
                    long long startTs = Utils::parseTimestamp(startStr);
                    long long endTs = Utils::parseTimestamp(endStr);
                    long long maxTs = analyzer.getCurrentTime();

                    // 如果输入结束时间大于系统最大时间，默认为最大时间
                    if (endTs > maxTs) {
                        endTs = maxTs;
                        endStr = Utils::formatTimestamp(maxTs);
                    }
                    
                    auto historyTopK = analyzer.getHistoryTopK(startTs, endTs, k);
                    
                    cout << "\n[HISTORY RESULT] Range: " << startStr << " to " << endStr << " | Top-" << k << " Hot Words:" << endl;
                    if (historyTopK.empty()) {
                        cout << "(No words found in this range)" << endl;
                    } else {
                        for (size_t i = 0; i < historyTopK.size(); ++i) {
                            cout << i + 1 << ". " << historyTopK[i].first << " (" << historyTopK[i].second << " times)" << endl;
                        }
                    }
                    cout << endl;
                }
            }
            // 5. 处理退出指令
            else if (line.find("EXIT") != string::npos) {
                break;
            }
            // 2. 处理设置窗口指令: [ACTION] SET_WINDOW SIZE=3600
            else if (line.find("SET_WINDOW") != string::npos) {
                size_t sPos = line.find("SIZE=");
                if (sPos != string::npos) {
                    int newSize = stoi(line.substr(sPos + 5));
                    analyzer.setWindowSize(newSize);
                    cout << ">>> Window size updated to: " << newSize << " seconds." << endl;
                }
            }
            // 3. 处理添加敏感词指令: [ACTION] ADD_SENSITIVE WORD=xxx
            else if (line.find("ADD_SENSITIVE") != string::npos) {
                size_t wPos = line.find("WORD=");
                if (wPos != string::npos) {
                    string word = line.substr(wPos + 5);
                    processor.addSensitiveWord(word);
                    cout << ">>> Added sensitive word: " << word << endl;
                }
            }
            // 8. 处理添加保留词指令: [ACTION] ADD_RESERVED WORD=xxx
            else if (line.find("ADD_RESERVED") != string::npos) {
                size_t wPos = line.find("WORD=");
                if (wPos != string::npos) {
                    string word = line.substr(wPos + 5);
                    processor.addReservedWord(word);
                    cout << ">>> Added reserved word: " << word << endl;
                }
            }
            // 6. 处理趋势分析指令: [ACTION] GET_TREND WORD=xxx INTERVAL=60
            else if (line.find("GET_TREND") != string::npos) {
                size_t wPos = line.find("WORD=");
                size_t iPos = line.find("INTERVAL=");
                if (wPos != string::npos) {
                    string wordPart = line.substr(wPos + 5);
                    size_t space = wordPart.find(' ');
                    string word = (space == string::npos) ? wordPart : wordPart.substr(0, space);
                    
                    int interval = 60;
                    if (iPos != string::npos) {
                        try { interval = stoi(line.substr(iPos + 9)); } catch(...) {}
                    }
                    
                    auto trend = analyzer.getTrendData(word, interval);
                    cout << "[TREND_DATA] WORD=" << word << " DATA=";
                    for (size_t i = 0; i < trend.first.size(); ++i) {
                        cout << trend.first[i] << ":" << trend.second[i] << (i == trend.first.size() - 1 ? "" : ",");
                    }
                    cout << endl;
                }
            }
            // 7. 处理 Top-K 趋势分析指令: [ACTION] GET_TOPK_TREND K=3 INTERVAL=60
            else if (line.find("GET_TOPK_TREND") != string::npos) {
                size_t kPos = line.find("K=");
                size_t iPos = line.find("INTERVAL=");
                int k = 3;
                int interval = 60;
                if (kPos != string::npos) {
                    string kVal = line.substr(kPos + 2);
                    size_t space = kVal.find(' ');
                    if (space != string::npos) kVal = kVal.substr(0, space);
                    try { k = stoi(kVal); } catch(...) {}
                }
                if (iPos != string::npos) {
                    try { interval = stoi(line.substr(iPos + 9)); } catch(...) {}
                }
                
                // 使用新的 getGlobalTopKTrend 逻辑，找出每个阶段的热词并汇总
                auto multiTrend = analyzer.getGlobalTopKTrend(k, interval);
                cout << "[MULTI_TREND_START] K=" << multiTrend.size() << endl;
                for (const auto& it : multiTrend) {
                    const string& word = it.first;
                    const auto& trend = it.second;
                    cout << "[TREND_DATA] WORD=" << word << " DATA=";
                    for (size_t i = 0; i < trend.first.size(); ++i) {
                        cout << trend.first[i] << ":" << trend.second[i] << (i == trend.first.size() - 1 ? "" : ",");
                    }
                    cout << endl;
                }
                cout << "[MULTI_TREND_END]" << endl;
            }
        } 
        // 检查是否是数据行
        else if (line[0] == '[') {
            // 格式: [00:00:00] 文本内容...
            size_t endBracket = line.find(']');
            if (endBracket != string::npos) {
                string timeStr = line.substr(0, endBracket + 1);
                string content = line.substr(endBracket + 1);
                
                // 鲁棒性处理：如果内容部分开头还有 ']' (可能是双重时间戳)，继续向后找
                // 或者如果开头是空格，去掉空格
                while (!content.empty() && (content[0] == ' ' || content[0] == '\t')) {
                    content.erase(0, 1);
                }
                
                // 1. 解析时间
                long long timestamp = Utils::parseTimestamp(timeStr);
                
                // 2. 分词与清洗
                vector<string> words = processor.process(content);
                
                // 3. 喂给分析器
                analyzer.ingest(timestamp, words);
            }
        }
    }

    cout << "System exited." << endl;
    return 0;
}

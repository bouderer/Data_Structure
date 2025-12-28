#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib> // 用于 system()
#include "TextProcessor.hpp"
#include "HotWordAnalyzer.hpp"
#include "Utils.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// 获取可执行文件所在目录
string getExeDir() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    string path(buffer);
    size_t pos = path.find_last_of("\\/");
    return (pos != string::npos) ? path.substr(0, pos) : ".";
#else
    return ".";
#endif
}

// 词典路径（运行时根据exe位置确定）
string DICT_PATH;
string HMM_PATH;
string USER_DICT_PATH;
string IDF_PATH;
string STOP_WORD_PATH;
string SENSITIVE_WORD_PATH;

void initDictPaths() {
    string base = getExeDir() + "/../../../dict/";
    DICT_PATH = base + "jieba.dict.utf8";
    HMM_PATH = base + "hmm_model.utf8";
    USER_DICT_PATH = base + "user.dict.utf8";
    IDF_PATH = base + "idf.utf8";
    STOP_WORD_PATH = base + "stop_words.utf8";
    SENSITIVE_WORD_PATH = base + "sensitive_words.utf8";
}

int main(int argc, char** argv) {
    // 在 Windows 上，让控制台支持中文显示
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    // 初始化词典路径（根据exe位置自动确定）
    initDictPaths();

    // 1. 初始化各个模块
    cout << "正在初始化分词引擎..." << endl;
    TextProcessor processor(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH, SENSITIVE_WORD_PATH);
    
    // 默认窗口大小设为 600 秒
    HotWordAnalyzer analyzer(600); 

    // 决定从哪里读数据：是读文件还是读控制台输入
    istream* inputSource = &cin;
    ifstream fileStream;
    
    if (argc > 1) {
        fileStream.open(argv[1]);
        if (!fileStream.is_open()) {
            cerr << "打不开文件: " << argv[1] << endl;
            return 1;
        }
        inputSource = &fileStream;
        cout << "正在读取文件: " << argv[1] << endl;
    } else {
        cout << "系统已启动，等待输入... (按 Ctrl+Z 结束)" << endl;
    }

    string line;
    while (getline(*inputSource, line)) {
        if (line.empty()) continue;

        // 看看是不是指令（以 [ACTION] 开头）
        if (line.find("[ACTION]") != string::npos) {
            // 处理查询指令
            if (line.find("[ACTION] QUERY") != string::npos) {
                size_t kPos = line.find("K=");
                if (kPos != string::npos) {
                    int k = stoi(line.substr(kPos + 2));
                    auto topK = analyzer.getTopK(k);
                    auto bounds = analyzer.getWindowBounds(analyzer.getCurrentTime());
                    
                    cout << "\n[QUERY RESULT] 窗口范围: " << Utils::formatTimestamp(bounds.first) 
                         << " ~ " << Utils::formatTimestamp(bounds.second) 
                         << " | Top " << k << " 热词:" << endl;
                    if (topK.empty()) {
                        cout << "(当前窗口没词)" << endl;
                    } else {
                        for (size_t i = 0; i < topK.size(); ++i) {
                            cout << i + 1 << ". " << topK[i].first << " (" << topK[i].second << " 次)" << endl;
                        }
                    }

                    // 打印敏感词统计
                    auto sensitiveCountsMap = processor.getAndClearSensitiveCounts();
                    if (!sensitiveCountsMap.empty()) {
                        vector<pair<string, int>> sortedSensitive(sensitiveCountsMap.begin(), sensitiveCountsMap.end());
                        sort(sortedSensitive.begin(), sortedSensitive.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
                            return a.second > b.second;
                        });

                        cout << "\n[SENSITIVE_SUMMARY] ";
                        bool first = true;
                        for (const auto& pair : sortedSensitive) {
                            if (!first) cout << ", ";
                            cout << pair.first << " (" << pair.second << " 次)";
                            first = false;
                        }
                        cout << endl;
                    }
                    cout << endl;
                }
            }
            // 处理历史查询
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
                
                int k = 5;
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

                    if (endTs > maxTs) {
                        endTs = maxTs;
                        endStr = Utils::formatTimestamp(maxTs);
                    }
                    
                    auto historyTopK = analyzer.getHistoryTopK(startTs, endTs, k);
                    
                    cout << "\n[HISTORY RESULT] 范围: " << startStr << " 到 " << endStr << " | 前 " << k << " 个热词:" << endl;
                    if (historyTopK.empty()) {
                        cout << "(没找到词)" << endl;
                    } else {
                        for (size_t i = 0; i < historyTopK.size(); ++i) {
                            cout << i + 1 << ". " << historyTopK[i].first << " (" << historyTopK[i].second << " 次)" << endl;
                        }
                    }
                    cout << endl;
                }
            }
            // 退出指令
            else if (line.find("EXIT") != string::npos) {
                break;
            }
            // 设置窗口大小
            else if (line.find("SET_WINDOW") != string::npos) {
                size_t sPos = line.find("SIZE=");
                if (sPos != string::npos) {
                    int newSize = stoi(line.substr(sPos + 5));
                    analyzer.setWindowSize(newSize);
                    cout << ">>> 窗口大小已改为: " << newSize << " 秒" << endl;
                }
            }
            // 加敏感词
            else if (line.find("ADD_SENSITIVE") != string::npos) {
                size_t wPos = line.find("WORD=");
                if (wPos != string::npos) {
                    string word = line.substr(wPos + 5);
                    processor.addSensitiveWord(word);
                    cout << ">>> 已添加敏感词: " << word << endl;
                }
            }
            // 加保留词
            else if (line.find("ADD_RESERVED") != string::npos) {
                size_t wPos = line.find("WORD=");
                if (wPos != string::npos) {
                    string word = line.substr(wPos + 5);
                    processor.addReservedWord(word);
                    cout << ">>> 已添加保留词: " << word << endl;
                }
            }
            // 查趋势
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
            // 查 Top-K 趋势
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
        // 处理数据行
        else if (line[0] == '[') {
            // 格式: [00:00:00] 文本内容...
            size_t endBracket = line.find(']');
            if (endBracket != string::npos) {
                string timeStr = line.substr(0, endBracket + 1);
                string content = line.substr(endBracket + 1);
                
                while (!content.empty() && (content[0] == ' ' || content[0] == '\t')) {
                    content.erase(0, 1);
                }
                
                // 1. 解析时间
                long long timestamp = Utils::parseTimestamp(timeStr);
                
                // 2. 分词
                vector<string> words = processor.process(content);
                
                // 3. 统计
                analyzer.ingest(timestamp, words);
            }
        }
    }

    cout << "系统已退出。" << endl;
    return 0;
}

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib> // 用于 system()
#include "TextProcessor.hpp"
#include "HotWordAnalyzer.hpp"
#include "Utils.hpp"

using namespace std;

// 词典路径常量
const string DICT_PATH = "../dict/jieba.dict.utf8";
const string HMM_PATH = "../dict/hmm_model.utf8";
const string USER_DICT_PATH = "../dict/user.dict.utf8";
const string IDF_PATH = "../dict/idf.utf8";
const string STOP_WORD_PATH = "../dict/stop_words.utf8";

int main(int argc, char** argv) {
    // 在 Windows 终端中强制使用 UTF-8 编码，解决中文乱码问题
#ifdef _WIN32
    system("chcp 65001 > nul");
#endif

    // 1. 初始化模块
    cout << "Initializing Jieba..." << endl;
    TextProcessor processor(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
    
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
            // 格式: [ACTION] QUERY K=3
            size_t kPos = line.find("K=");
            if (kPos != string::npos) {
                int k = stoi(line.substr(kPos + 2));
                auto topK = analyzer.getTopK(k);
                string currentTimeStr = Utils::formatTimestamp(analyzer.getCurrentTime());
                
                cout << "\n[QUERY RESULT] Time: " << currentTimeStr << " | Top-" << k << " Hot Words:" << endl;
                if (topK.empty()) {
                    cout << "(No words in current window)" << endl;
                } else {
                    for (size_t i = 0; i < topK.size(); ++i) {
                        cout << i + 1 << ". " << topK[i].first << " (" << topK[i].second << " times)" << endl;
                    }
                }
                cout << endl;
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

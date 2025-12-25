#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
    // 1. 初始化模块
    cout << "Initializing Jieba..." << endl;
    TextProcessor processor(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
    
    // 默认窗口大小 600秒 (10分钟)
    HotWordAnalyzer analyzer(600); 

    cout << "System started. Reading input..." << endl;

    // 2. 模拟读取数据流 (这里简化为读取标准输入或文件)
    // 实际作业中可能需要循环读取 input.txt
    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;

        // TODO: 解析行
        // 格式1: [00:00:00] 文本内容...
        // 格式2: [ACTION] QUERY K=3
        
        // 伪代码示例:
        // if (isAction(line)) {
        //     analyzer.getTopK(k);
        //     print(result);
        // } else {
        //     long long time = Utils::parseTimestamp(line);
        //     string content = extractContent(line);
        //     vector<string> words = processor.process(content);
        //     analyzer.ingest(time, words);
        // }
    }

    return 0;
}

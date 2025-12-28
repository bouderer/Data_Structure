#ifndef TEXT_PROCESSOR_HPP
#define TEXT_PROCESSOR_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include "cppjieba/Jieba.hpp" // 引用项目根目录下的 cppjieba

// 文本处理类：负责把句子切开，并过滤掉没用的词
class TextProcessor {
private:
    cppjieba::Jieba jieba; // 结巴分词器
    std::unordered_set<std::string> stopWords; // 停用词（比如“的”、“了”）
    std::unordered_set<std::string> sensitiveWords; // 敏感词
    std::unordered_map<std::string, int> sensitiveCounts; // 记录敏感词出现了几次

public:
    // 构造函数：初始化分词器，加载词典
    TextProcessor(const std::string& dictPath, 
                  const std::string& hmmPath, 
                  const std::string& userDictPath, 
                  const std::string& idfPath, 
                  const std::string& stopWordPath,
                  const std::string& sensitiveWordPath) 
        : jieba(dictPath, hmmPath, userDictPath, idfPath, stopWordPath) {
        
        // 把停用词和敏感词读进内存
        loadWordSet(stopWordPath, stopWords);
        loadWordSet(sensitiveWordPath, sensitiveWords);
    }

private:
    // 帮我们读文件的函数
    void loadWordSet(
        const std::string& path, 
        std::unordered_set<std::string>& targetSet) {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            std::cerr << "打不开文件: " << path << std::endl;
            return;
        }
        std::string line;
        while (std::getline(ifs, line)) {
            // 去掉行尾的回车符
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (!line.empty()) {
                targetSet.insert(line);
            }
        }
    }

public:
    // 处理一句话：分词 -> 过滤 -> 返回有用的词
    std::vector<std::string> process(const std::string& sentence) {
        std::vector<std::string> words;
        std::vector<std::string> temp_words;
        
        // 1. 先分词
        jieba.Cut(sentence, temp_words, true);

        // 2. 看看哪些词要留下
        for (const auto& w : temp_words) {
            // 如果是停用词，不要
            if (stopWords.count(w)) {
                continue;
            }

            // 如果是敏感词，记下来，但不要
            if (sensitiveWords.count(w)) {
                sensitiveCounts[w]++;
                continue;
            }

            // 过滤掉空格和换行
            if (w == " " || w == "\t" || w == "\n" || w == "\r") {
                continue;
            }

            // 过滤掉单字节的符号（比如英文逗号、句号）
            if (w.size() == 1 && (unsigned char)w[0] < 128) {
                continue;
            }

            // 过滤掉常见的全角符号
            if (w == "～" || w == "。" || w == "，" || w == "！" || w == "？" || 
                w == "、" || w == "；" || w == "：" || w == "“" || w == "”" ||
                w == "‘" || w == "’" || w == "【" || w == "】" || w == "《" || 
                w == "》" || w == "—" || w == "…" || w == "（" || w == "）") {
                continue;
            }

            words.push_back(w);
        }
        return words;
    }

    // 动态加一个敏感词
    void addSensitiveWord(const std::string& word) {
        if (!word.empty()) {
            sensitiveWords.insert(word);
        }
    }

    // 动态添加保留词（自定义词典），防止被切分
    void addReservedWord(const std::string& word) {
        if (!word.empty()) {
            // 插入用户词典，赋予较高的词频以确保不被切分
            jieba.InsertUserWord(word);
        }
    }

    // 获取并清空敏感词统计
    std::unordered_map<std::string, int> getAndClearSensitiveCounts() {
        std::unordered_map<std::string, int> result = sensitiveCounts;
        sensitiveCounts.clear();
        return result;
    }

    // "清空敏感词"
    // void clearSensitiveWords() {
    //     sensitiveWords.clear();
    // }
    // 
};

#endif // TEXT_PROCESSOR_HPP

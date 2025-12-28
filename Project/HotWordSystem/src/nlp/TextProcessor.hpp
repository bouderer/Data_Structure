#ifndef TEXT_PROCESSOR_HPP
#define TEXT_PROCESSOR_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include "cppjieba/Jieba.hpp" // 引用项目根目录下的 cppjieba

// 文本预处理类：负责分词和清洗
class TextProcessor {
private:
    cppjieba::Jieba jieba;
    std::unordered_set<std::string> stopWords;
    std::unordered_set<std::string> sensitiveWords;
    std::unordered_map<std::string, int> sensitiveCounts; // 记录敏感词出现次数

public:
    // 构造函数：初始化 Jieba、停用词表和敏感词表
    TextProcessor(const std::string& dictPath, 
                  const std::string& hmmPath, 
                  const std::string& userDictPath, 
                  const std::string& idfPath, 
                  const std::string& stopWordPath,
                  const std::string& sensitiveWordPath) 
        : jieba(dictPath, hmmPath, userDictPath, idfPath, stopWordPath) {
        
        // 加载停用词表
        loadWordSet(stopWordPath, stopWords);
        // 加载敏感词表
        loadWordSet(sensitiveWordPath, sensitiveWords);
    }

private:
    // 通用的词表加载函数
    void loadWordSet(const std::string& path, std::unordered_set<std::string>& targetSet) {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            std::cerr << "Error: Cannot open file: " << path << std::endl;
            return;
        }
        std::string line;
        while (std::getline(ifs, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            if (!line.empty()) {
                targetSet.insert(line);
            }
        }
    }

public:
    // 处理单条句子：分词 -> 去停用词 -> 过滤敏感词 -> 返回结果
    std::vector<std::string> process(const std::string& sentence) {
        std::vector<std::string> words;
        std::vector<std::string> temp_words;
        
        // 1. 调用 jieba.Cut 进行分词
        jieba.Cut(sentence, temp_words, true);

        // 2. 遍历分词结果进行过滤
        for (const auto& w : temp_words) {
            // 过滤停用词
            if (stopWords.count(w)) {
                continue;
            }

            // 过滤敏感词
            if (sensitiveWords.count(w)) {
                // 记录敏感词出现次数，不再实时打印
                sensitiveCounts[w]++;
                continue;
            }

            // 过滤空白字符
            if (w == " " || w == "\t" || w == "\n" || w == "\r") {
                continue;
            }

            // 过滤标点符号和单字节字符 (通常我们只关心中文热词)
            // 简单的策略：如果长度为1且是ASCII字符，认为是英文标点或字母，过滤掉
            // UTF-8 中文通常是 3 个字节
            if (w.size() == 1 && (unsigned char)w[0] < 128) {
                continue;
            }

            words.push_back(w);
        }
        return words;
    }

    // 动态添加敏感词
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

    // 清空敏感词
    void clearSensitiveWords() {
        sensitiveWords.clear();
    }
};

#endif // TEXT_PROCESSOR_HPP

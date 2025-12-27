#ifndef TEXT_PROCESSOR_HPP
#define TEXT_PROCESSOR_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include "cppjieba/Jieba.hpp" // 引用项目根目录下的 cppjieba

// 文本预处理类：负责分词和清洗
class TextProcessor {
private:
    cppjieba::Jieba jieba;
    std::unordered_set<std::string> stopWords;

public:
    // 构造函数：初始化 Jieba 和停用词表
    TextProcessor(const std::string& dictPath, 
                  const std::string& hmmPath, 
                  const std::string& userDictPath, 
                  const std::string& idfPath, 
                  const std::string& stopWordPath) 
        : jieba(dictPath, hmmPath, userDictPath, idfPath, stopWordPath) {
        
        // 加载停用词表到内存 set 中，方便快速查询
        std::ifstream ifs(stopWordPath);
        if (!ifs.is_open()) {
            std::cerr << "Error: Cannot open stop words file: " << stopWordPath << std::endl;
        } else {
            std::string line;
            while (std::getline(ifs, line)) {
                // 去除行尾可能的 \r (Windows换行符遗留)
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                if (!line.empty()) {
                    stopWords.insert(line);
                }
            }
            // std::cout << "Loaded " << stopWords.size() << " stop words." << std::endl;
        }
    }

    // 处理单条句子：分词 -> 去停用词 -> 返回结果
    std::vector<std::string> process(const std::string& sentence) {
        std::vector<std::string> words;
        std::vector<std::string> temp_words;
        
        // 1. 调用 jieba.Cut 进行分词 (开启 HMM 新词发现)
        jieba.Cut(sentence, temp_words, true);

        // 2. 遍历分词结果，过滤掉在 stopWords 中的词和标点符号
        for (const auto& w : temp_words) {
            // 过滤停用词
            if (stopWords.count(w)) {
                continue;
            }

            // 过滤空白字符 (空格、制表符等)
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
};

#endif // TEXT_PROCESSOR_HPP

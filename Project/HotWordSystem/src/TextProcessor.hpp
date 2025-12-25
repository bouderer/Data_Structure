#ifndef TEXT_PROCESSOR_HPP
#define TEXT_PROCESSOR_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include "cppjieba/Jieba.hpp" // 引用项目根目录下的 cppjieba

// 文本预处理类：负责分词和清洗
class TextProcessor {
public:
    // 构造函数：初始化 Jieba 和停用词表
    TextProcessor(const std::string& dictPath, 
                  const std::string& hmmPath, 
                  const std::string& userDictPath, 
                  const std::string& idfPath, 
                  const std::string& stopWordPath) 
        : jieba(dictPath, hmmPath, userDictPath, idfPath, stopWordPath) {
        
        // 加载停用词表到内存 set 中，方便快速查询
        // TODO: 读取 stopWordPath 文件，将词填入 stopWords 集合
    }

    // 处理单条句子：分词 -> 去停用词 -> 返回结果
    std::vector<std::string> process(const std::string& sentence) {
        std::vector<std::string> words;
        // TODO: 1. 调用 jieba.CutForSearch 或 jieba.Cut 进行分词
        // TODO: 2. 遍历分词结果，过滤掉在 stopWords 中的词和标点符号
        return words;
    }

private:
    cppjieba::Jieba jieba;
    std::unordered_set<std::string> stopWords;
};

#endif // TEXT_PROCESSOR_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "../src/nlp/TextProcessor.hpp"

void testProcessor() {
    std::cout << "Initializing TextProcessor for testing..." << std::endl;
    
    // 路径相对于执行目录 (假设在 HotWordSystem 目录下运行)
    TextProcessor processor(
        "../dict/jieba.dict.utf8",
        "../dict/hmm_model.utf8",
        "../dict/user.dict.utf8",
        "../dict/idf.utf8",
        "../dict/stop_words.utf8",
        "../dict/sensitive_words.utf8"
    );

    std::cout << "Testing basic tokenization..." << std::endl;
    std::vector<std::string> words = processor.process("我爱北京天安门");
    assert(!words.empty());

    std::cout << "Testing sensitive word filtering..." << std::endl;
    // 确保 "敏感词" 作为一个整体被切分，否则过滤会失效
    processor.addReservedWord("敏感词"); 
    processor.addSensitiveWord("敏感词");
    std::vector<std::string> words2 = processor.process("这是一个敏感词的内容");
    for(const auto& w : words2) {
        assert(w != "敏感词");
    }
    
    auto sensitiveSummary = processor.getAndClearSensitiveCounts();
    assert(sensitiveSummary.count("敏感词") > 0);

    std::cout << "Testing reserved word (custom dict)..." << std::endl;
    // 默认情况下 "黑神话" 可能会被切开，我们强制保留
    processor.addReservedWord("黑神话");
    std::vector<std::string> words3 = processor.process("黑神话悟空很好玩");
    bool found = false;
    for(const auto& w : words3) {
        if(w == "黑神话") found = true;
    }
    assert(found);

    std::cout << "TextProcessor tests passed!" << std::endl;
}

int main() {
    testProcessor();
    return 0;
}

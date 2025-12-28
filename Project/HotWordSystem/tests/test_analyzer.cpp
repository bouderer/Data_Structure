#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "../src/core/HotWordAnalyzer.hpp"

void testAnalyzer() {
    std::cout << "Testing HotWordAnalyzer..." << std::endl;
    
    HotWordAnalyzer analyzer(10); // 10秒窗口

    // T=0: 放入词汇
    analyzer.ingest(0, {"apple", "banana"});
    auto topk = analyzer.getTopK(5);
    assert(topk.size() == 2);

    // T=5: 放入更多词汇
    analyzer.ingest(5, {"apple", "cherry"});
    topk = analyzer.getTopK(5);
    assert(topk[0].first == "apple");
    assert(topk[0].second == 2);

    // T=11: apple(T=0) 应该过期
    analyzer.ingest(11, {"banana"});
    topk = analyzer.getTopK(5);
    // 此时窗口是 [1, 11]
    // apple(T=0) 过期，apple(T=5) 还在
    // banana(T=0) 过期，banana(T=11) 还在
    // cherry(T=5) 还在
    bool foundApple = false;
    for(auto& p : topk) {
        if(p.first == "apple") {
            assert(p.second == 1);
            foundApple = true;
        }
    }
    assert(foundApple);

    std::cout << "Testing trend data..." << std::endl;
    auto trend = analyzer.getTrendData("apple", 5);
    // 应该有数据点
    assert(!trend.first.empty());

    std::cout << "HotWordAnalyzer tests passed!" << std::endl;
}

int main() {
    testAnalyzer();
    return 0;
}

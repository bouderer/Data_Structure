#include <iostream>
#include <cassert>
#include "../src/common/Utils.hpp"

void testTimestamp() {
    std::cout << "Testing Utils::parseTimestamp..." << std::endl;
    assert(Utils::parseTimestamp("[00:00:00]") == 0);
    assert(Utils::parseTimestamp("[00:01:00]") == 60);
    assert(Utils::parseTimestamp("[01:00:00]") == 3600);
    assert(Utils::parseTimestamp("[01:01:01]") == 3661);
    
    std::cout << "Testing Utils::formatTimestamp..." << std::endl;
    assert(Utils::formatTimestamp(0) == "[00:00:00]");
    assert(Utils::formatTimestamp(60) == "[00:01:00]");
    assert(Utils::formatTimestamp(3661) == "[01:01:01]");
    assert(Utils::formatTimestamp(-10) == "[00:00:00]"); // 检查负数修复
    
    std::cout << "Utils tests passed!" << std::endl;
}

int main() {
    testTimestamp();
    return 0;
}

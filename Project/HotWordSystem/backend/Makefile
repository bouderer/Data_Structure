# 编译器设置
CXX = g++
# 修正包含路径：
# . 是 backend/
# src 是 backend/src/
# ../../ 是 Project/ (包含 cppjieba)
CXXFLAGS = -std=c++11 -O2 -I. -Isrc -I../../ -I../../cppjieba -finput-charset=UTF-8 -fexec-charset=UTF-8

# 目录设置
SRC_DIR = src
BIN_DIR = bin

# 目标文件
TARGET = $(BIN_DIR)/hotwords
TEST_DIR = tests
TEST_BIN_DIR = $(BIN_DIR)/tests

# 源文件
SRCS = $(SRC_DIR)/main.cpp

# 默认目标
all: $(TARGET)

# 链接主程序
$(TARGET): $(SRCS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@.exe $^

# 单元测试目标
test: all $(TEST_BIN_DIR)/test_utils.exe $(TEST_BIN_DIR)/test_processor.exe $(TEST_BIN_DIR)/test_analyzer.exe
	@echo Running tests from project root...
	cd .. && .\backend\bin\tests\test_utils.exe
	cd .. && .\backend\bin\tests\test_analyzer.exe
	cd .. && .\backend\bin\tests\test_processor.exe

$(TEST_BIN_DIR)/%.exe: $(TEST_DIR)/%.cpp
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@if not exist $(BIN_DIR)\tests mkdir $(BIN_DIR)\tests
	$(CXX) $(CXXFLAGS) -o $@ $^

# 运行 (默认使用 input3.txt)
run: all
	$(TARGET).exe ../../data/input3.txt

# 清理
clean:
	if exist $(BIN_DIR) rmdir /S /Q $(BIN_DIR)

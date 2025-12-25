# 编译器设置
CXX = g++
CXXFLAGS = -std=c++11 -O2 -I. -I.. -I../cppjieba

# 目录设置
SRC_DIR = src
BIN_DIR = bin

# 目标文件
TARGET = $(BIN_DIR)/hotwords

# 源文件
SRCS = $(SRC_DIR)/main.cpp

# 默认目标
all: $(TARGET)

# 链接
$(TARGET): $(SRCS)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 清理
clean:
	del /Q $(BIN_DIR)\hotwords.exe

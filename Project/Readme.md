# 热词统计与分析系统

## 📋 项目简介

本系统是一个**基于滑动窗口的实时热词统计与分析系统**，能够对持续输入的文本数据流进行中文分词、词频统计，并提供可配置的 Top-K 热词查询、趋势分析、敏感词过滤等功能。

系统采用 **C++ 后端 + Python 前端** 的混合架构：
- **C++ 后端**：负责高性能的文本处理、滑动窗口管理和 Top-K 计算。
- **Python 前端**：提供美观的图形用户界面（GUI）、数据可视化和系统资源监控。

---

## ✨ 功能特性

### 核心功能
| 功能 | 描述 | 状态 |
|------|------|------|
| 实时数据流处理 | 支持持续接收带时间戳的文本数据 | ✅ 已实现 |
| 中文分词 | 集成 cppjieba 高性能分词引擎 | ✅ 已实现 |
| 停用词过滤 | 自动过滤"的"、"了"等无意义词汇 | ✅ 已实现 |
| 保留词/自定义词典 | 支持添加专业术语，确保不被错误切分 | ✅ 已实现 |
| 滑动窗口统计 | 基于时间的滑动窗口，支持动态调整大小 | ✅ 已实现 |
| Top-K 查询 | 实时获取当前窗口内频率最高的 K 个词 | ✅ 已实现 |

### 高级功能
| 功能 | 描述 | 状态 |
|------|------|------|
| 敏感词过滤 | 支持自定义敏感词列表，实时屏蔽并统计 | ✅ 已实现 |
| 保留词管理 | 支持在界面中动态添加/删除保留词 | ✅ 已实现 |
| 趋势分析 | 绘制词频随时间变化的趋势曲线 | ✅ 已实现 |
| 历史查询 | 支持查询任意时间段内的热词排名 | ✅ 已实现 |
| 动态窗口大小 | 运行时可调整滑动窗口的时间范围 | ✅ 已实现 |
| 交互式可视化 | 提供 GUI 界面展示热词列表和趋势曲线 | ✅ 已实现 |
| 资源监控 | 实时显示后端引擎的内存和 CPU 占用 | ✅ 已实现 |

---

## 🏗️ 项目结构

```
Project/
├── StartSystem.bat          # 一键启动脚本
├── README.md                 # 本文件
├── cppjieba/                 # 中文分词库（第三方）
├── dict/                     # 词典文件
│   ├── jieba.dict.utf8       # 主词典
│   ├── hmm_model.utf8        # HMM 模型
│   ├── user.dict.utf8        # 用户自定义词典（保留词）
│   ├── stop_words.utf8       # 停用词表
│   ├── sensitive_words.utf8  # 敏感词表
│   └── idf.utf8              # IDF 词频文件
├── data/                     # 测试数据
│   ├── input1.txt            # 测试数据集 1（6,378 条）
│   ├── input2.txt            # 测试数据集 2（5,783 条）
│   ├── input3.txt            # 测试数据集 3（7,316 条）
│   ├── test_simple.txt       # 简单功能测试
│   └── test_wave.txt         # 特殊符号过滤测试
├── docs/                     # 文档目录
│   ├── 系统设计文档.md
│   └── 性能测试报告.md
└── HotWordSystem/            # 系统主目录
    ├── Makefile              # 总控编译脚本
    ├── backend/              # C++ 后端
    │   ├── Makefile
    │   ├── src/
    │   │   ├── main.cpp              # 主程序入口
    │   │   ├── HotWordAnalyzer.hpp   # 核心统计引擎
    │   │   ├── TextProcessor.hpp     # 分词与过滤
    │   │   └── Utils.hpp             # 工具函数
    │   ├── bin/                      # 编译输出
    │   └── tests/                    # 单元测试
    │       ├── test_analyzer.cpp
    │       ├── test_processor.cpp
    │       └── test_utils.cpp
    └── frontend/             # Python 前端
        ├── main.py           # GUI 主程序
        ├── engine.py         # 后端通信模块
        ├── monitor.py        # 资源监控模块
        ├── charts.py         # 图表绘制模块
        ├── trend_manager.py  # 趋势分析管理
        ├── query_manager.py  # 查询管理
        ├── word_manager.py   # 词典管理
        ├── file_handler.py   # 文件操作
        └── widgets.py        # 通用 UI 组件
```

---

## 🚀 快速开始

### 环境要求
- **操作系统**: Windows 10/11
- **编译器**: MinGW g++ (支持 C++11)
- **Python**: 3.8+ (推荐使用 Anaconda)
- **Python 依赖库**: 
  - `tkinter` (GUI 界面，Python 自带)
  - `matplotlib` (数据可视化)
  - `psutil` (资源监控)

### 安装步骤

1. **安装 Python 依赖**（如尚未安装）：
   ```bash
   pip install matplotlib psutil
   ```

2. **编译后端引擎**：
   ```bash
   cd HotWordSystem/backend
   mingw32-make build
   ```

3. **启动系统**：
   - 方式一：双击项目根目录下的 `StartSystem.bat`
   - 方式二：在命令行中运行：
     ```bash
     cd HotWordSystem
     python frontend/main.py
     ```

---

## 📖 使用说明

### 基本操作流程
1. **选择输入文件**：点击"选择文件"按钮，加载 `data/` 目录下的测试文本。
2. **配置参数**：设置滑动窗口大小（秒）和 Top-K 值。
3. **开始分析**：点击"开始分析"按钮，系统将实时显示热词统计结果。
4. **查看趋势**：点击"趋势分析"按钮，输入要分析的词语，生成趋势图。

### 输入数据格式
```
[HH:MM:SS] 文本内容
```
示例：
```
[00:00:00] 今天人工智能技术发展迅速
[00:01:00] 人工智能将改变未来生活方式
[00:02:00] 技术创新推动人工智能进步
```

### 交互指令（高级用户）
| 指令 | 功能 | 示例 |
|------|------|------|
| `[ACTION] QUERY K=n` | 查询当前 Top-K 热词 | `[ACTION] QUERY K=10` |
| `[ACTION] GET_TREND WORD=词语` | 获取词语趋势数据 | `[ACTION] GET_TREND WORD=人工智能` |
| `[ACTION] SET_WINDOW SIZE=n` | 动态修改窗口大小 | `[ACTION] SET_WINDOW SIZE=300` |
| `[ACTION] HISTORY_QUERY START=[t1] END=[t2] K=n` | 历史区间查询 | `[ACTION] HISTORY_QUERY START=[0:00:00] END=[0:10:00] K=5` |

---

## 🔧 配置说明

### Makefile 使用说明

项目包含两个 Makefile，采用分层设计：

| Makefile 位置 | 用途 |
|---------------|------|
| `HotWordSystem/Makefile` | 总控 Makefile，提供便捷的一键命令 |
| `HotWordSystem/backend/Makefile` | 后端编译 Makefile，处理具体编译细节 |

**在 `HotWordSystem/` 目录下可用的命令：**

| 命令 | 功能 | 说明 |
|------|------|------|
| `mingw32-make` 或 `mingw32-make all` | 编译后端 | 等同于 `mingw32-make build` |
| `mingw32-make build` | 编译后端 | 生成 `backend/bin/hotwords.exe` |
| `mingw32-make run` | 编译并运行 | 先编译后端，再启动 Python 前端 |
| `mingw32-make gui` | 同 run | 编译并启动 GUI 界面 |
| `mingw32-make test` | 运行单元测试 | 执行所有测试用例 |
| `mingw32-make clean` | 清理编译产物 | 删除 `backend/bin/` 目录 |

**在 `HotWordSystem/backend/` 目录下可用的命令：**

| 命令 | 功能 | 说明 |
|------|------|------|
| `mingw32-make` | 编译主程序 | 生成 `bin/hotwords.exe` |
| `mingw32-make test` | 编译并运行测试 | 生成测试程序并执行 |
| `mingw32-make run` | 编译并运行 | 使用 `data/input3.txt` 测试 |
| `mingw32-make clean` | 清理 | 删除 `bin/` 目录 |

**常用操作示例：**

```bash
# 首次编译
cd HotWordSystem
mingw32-make build

# 运行单元测试
mingw32-make test

# 启动系统（编译 + 运行 GUI）
mingw32-make run

# 清理后重新编译
mingw32-make clean
mingw32-make build
```

### 词典文件
| 文件 | 用途 | 格式 |
|------|------|------|
| `stop_words.utf8` | 停用词（会被过滤） | 每行一个词 |
| `sensitive_words.utf8` | 敏感词（会被屏蔽并统计） | 每行一个词 |
| `user.dict.utf8` | 保留词（确保不被切分） | `词语 词频 词性` |

### 修改默认参数
在 `backend/src/main.cpp` 中可修改：
- 默认窗口大小：`HotWordAnalyzer analyzer(600);` （单位：秒）

---

## 📚 第三方库引用

| 库名 | 版本 | 用途 | 许可证 |
|------|------|------|--------|
| cppjieba | 5.0.3 | 中文分词 | MIT |
| matplotlib | 3.x | 数据可视化 | PSF |
| psutil | 5.x | 系统资源监控 | BSD-3 |

---

## 👨‍💻 开发者信息

- **课程**: 数据结构与算法
- **提交日期**: 2025年12月28日

---

## 📄 相关文档

- [系统设计文档](docs/系统设计文档.md) - 详细的架构设计与算法分析
- [性能测试报告](docs/性能测试报告.md) - 性能指标与优化建议

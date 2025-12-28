@echo off
:: 强制使用 UTF-8 编码
chcp 65001 > nul
title 热词统计与分析系统启动器
echo ==========================================
echo   正在启动热词统计与分析系统...
echo ==========================================
echo.

:: 切换到脚本所在目录
cd /d "%~dp0"

:: 检查是否存在 bin 目录，不存在则创建
if not exist bin mkdir bin

:: 执行编译并启动 GUI
:: 使用 mingw32-make gui 命令
mingw32-make gui

:: 如果运行出错，暂停显示错误信息
if %errorlevel% neq 0 (
    echo.
    echo [错误] 系统启动失败！
    echo 请确保您已安装：
    echo 1. MinGW (g++) - 用于编译 C++ 引擎
    echo 2. Python 3 - 用于运行图形界面
    echo.
    pause
)

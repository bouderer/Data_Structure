@echo off
:: 设置编码为 UTF-8，防止中文乱码
chcp 65001 > nul
title 热词统计与分析系统

echo ==========================================
echo   正在启动热词统计与分析系统...
echo ==========================================
echo.

:: 切换到当前脚本所在的文件夹
cd /d "%~dp0"

:: 1. 检查 Python 是否可用
python --version >nul 2>nul
if %errorlevel% neq 0 (
    echo [错误] 找不到 python 命令。
    echo 请确保已安装 Python 3 并将其添加到系统环境变量 PATH 中。
    echo 如果你使用的是 Anaconda，请通过 Anaconda Prompt 运行此脚本。
    echo.
    pause
    exit /b
)

:: 2. 检查后端引擎是否存在，不存在则尝试编译
if not exist "HotWordSystem\backend\bin\hotwords.exe" (
    echo [提示] 找不到后端引擎，正在尝试编译...
    where mingw32-make >nul 2>nul
    if errorlevel 1 (
        echo [错误] 找不到编译工具 mingw32-make，无法自动编译后端。
        echo 请确保已安装 MinGW 并配置了环境变量。
        echo.
        pause
        exit /b
    ) else (
        cd HotWordSystem
        mingw32-make build
        cd ..
    )
)

:: 3. 启动 GUI 界面
echo [提示] 正在启动图形界面...

:: 尝试使用你电脑上工作的 Python 路径 (Anaconda)
set "PYTHON_EXE=D:\anaconda\python.exe"

if exist "%PYTHON_EXE%" (
    echo [调试] 检测到 Anaconda 环境，正在启动...
    "%PYTHON_EXE%" HotWordSystem/frontend/main.py
) else (
    echo [调试] 未检测到指定路径，尝试使用系统默认 python...
    python HotWordSystem/frontend/main.py
)

:: 无论成功还是失败，都停一下，让用户看清楚输出
echo.
echo ==========================================
echo   程序已运行结束。
echo ==========================================
pause

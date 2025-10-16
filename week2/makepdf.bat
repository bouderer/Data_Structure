@echo off
:: 解决中文乱码（将注释单独成行，避免紧跟命令）
chcp 65001 > nul
:: 启用延迟扩展（确保变量正确解析）
setlocal enabledelayedexpansion

:: 检查必要文件
if not exist "*.md" (
    echo 错误：当前目录没有.md文件！
    pause
    exit /b 1
)
if not exist "default-template.latex" (
    echo 错误：上一级目录找不到default-template.latex！
    pause
    exit /b 1
)

:: 转换当前目录下所有.md文件
for %%f in (*.md) do (
    echo 开始转换：%%f
    set "filename=%%~nf"
    :: 移除 -V geometry（解决选项冲突，后面解释）
    pandoc "%%f" --template=../default-template.latex  --pdf-engine=xelatex --resource-path=. -o "!filename!.pdf"
    pandoc "%%f" --template=../default-template.latex  --pdf-engine=xelatex --resource-path=. -o "24325157-梁玮麟-!filename!实验报告.pdf"
    
    if %errorlevel% equ 0 (
        echo 转换成功：!filename!.pdf
    ) else (
        echo 转换失败：%%f
    )
    echo ----------------------
)

endlocal
echo 所有文件处理完毕！
pause
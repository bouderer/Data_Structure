import os
import tkinter as tk
from tkinter import filedialog, messagebox

class FileHandler:
    """
    这个类用来处理文件的读写
    """
    def __init__(self, app):
        self.app = app
        # 获取当前文件所在的目录 (frontend 文件夹)
        current_dir = os.path.dirname(os.path.abspath(__file__))
        # 系统的根目录 (HotWordSystem 文件夹)
        system_root = os.path.dirname(current_dir)
        # 整个项目的根目录 (Data_Structure/Project 文件夹)
        self.project_root = os.path.dirname(system_root)

    def select_file(self):
        # 弹出对话框让用户选一个文件
        data_dir = os.path.join(self.project_root, "data")
        file_path = filedialog.askopenfilename(initialdir=data_dir, title="选择文件", filetypes=(("文本文件", "*.txt"), ("所有文件", "*.*")))
        if file_path:
            self.app.file_label.config(text=os.path.basename(file_path), foreground="black")
            try:
                # 读取文件内容并显示在输入框里
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()
                    self.app.input_area.delete(1.0, tk.END)
                    self.app.input_area.insert(tk.END, content)
                    self.app.status_var.set(f"已加载: {os.path.basename(file_path)}")
            except Exception as e:
                messagebox.showerror("错误", f"读文件出错了: {e}")

    def save_to_file(self, content):
        # 把分析结果保存到 output.txt
        try:
            if not content.strip():
                return
            output_path = os.path.join(self.project_root, "data", "output.txt")
            with open(output_path, "w", encoding="utf-8") as f:
                f.write(content)
            self.app.append_colored_text(f"\n[系统] 结果已保存到: {output_path}\n", "info")
        except Exception as e:
            print(f"保存失败: {e}")

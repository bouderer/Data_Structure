import tkinter as tk
from tkinter import messagebox, scrolledtext, ttk
import os

def manage_words_dialog(
    parent, 
    title, 
    label_text, 
    file_path, 
    on_save_callback=None
):
    # 弹出一个窗口来编辑词库文件
    win = tk.Toplevel(parent)
    win.title(title)
    win.geometry("400x500")
    win.transient(parent)
    win.grab_set()
    
    label = ttk.Label(win, text=label_text, padding=10)
    label.pack(anchor=tk.W)
    
    text_area = scrolledtext.ScrolledText(win, font=("Microsoft YaHei", 10))
    text_area.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
    
    initial_content = ""
    if os.path.exists(file_path):
        try:
            # 读取现有的词库
            with open(file_path, 'r', encoding='utf-8') as f:
                initial_content = f.read().strip()
                text_area.insert(tk.END, initial_content)
        except Exception as e:
            messagebox.showerror("错误", f"读文件失败: {e}")
    
    def save_words(quiet=False):
        # 保存修改到文件
        content = text_area.get(1.0, tk.END).strip()
        try:
            os.makedirs(os.path.dirname(file_path), exist_ok=True)
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            
            if on_save_callback:
                on_save_callback(content, initial_content)
                
            if not quiet:
                messagebox.showinfo("成功", "保存成功！")
            return True
        except Exception as e:
            messagebox.showerror("错误", f"保存失败: {e}")
            return False
            
    def on_closing():
        # 关闭窗口时的检查
        current_content = text_area.get(1.0, tk.END).strip()
        if current_content != initial_content:
            answer = messagebox.askyesnocancel("保存", "内容改过了，要保存吗？")
            if answer is True:
                if save_words(quiet=True): win.destroy()
            elif answer is False: win.destroy()
        else:
            win.destroy()

    win.protocol("WM_DELETE_WINDOW", on_closing)
    btn_frame = ttk.Frame(win)
    btn_frame.pack(pady=10)
    ttk.Button(btn_frame, text="💾 保存", command=lambda: save_words()).pack(side=tk.LEFT, padx=5)
    ttk.Button(btn_frame, text="❌ 关闭", command=on_closing).pack(side=tk.LEFT, padx=5)

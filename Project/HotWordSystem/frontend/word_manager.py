import os
import tkinter as tk
from tkinter import messagebox
import widgets

class WordManager:
    """
    这个类用来管理敏感词和保留词
    """
    def __init__(self, root, engine):
        self.root = root
        self.engine = engine
        # 获取当前文件所在的目录 (frontend 文件夹)
        current_dir = os.path.dirname(os.path.abspath(__file__))
        # 系统的根目录 (HotWordSystem 文件夹)
        system_root = os.path.dirname(current_dir)
        # 整个项目的根目录 (Data_Structure/Project 文件夹)
        self.project_root = os.path.dirname(system_root)

    def manage_sensitive_words(self):
        # 打开敏感词管理窗口
        sensitive_file = os.path.join(self.project_root, "dict", "sensitive_words.utf8")
        widgets.manage_words_dialog(
            self.root, 
            "敏感词管理", 
            "每行输入一个敏感词：", 
            sensitive_file
        )

    def manage_reserved_words(self):
        # 打开保留词管理窗口
        reserved_file = os.path.join(self.project_root, "dict", "user.dict.utf8")
        
        def on_save(content, initial_content):
            # 如果后台正在运行，把新加的词告诉后台
            if self.engine.is_running:
                new_words = content.split('\n')
                old_words = initial_content.split('\n')
                added_words = [w.strip() for w in new_words if w.strip() and w.strip() not in old_words]
                for w in added_words:
                    self.engine.send_command(f"\n[ACTION] ADD_RESERVED WORD={w}\n")
        
        widgets.manage_words_dialog(
            self.root, 
            "保留词管理", 
            "每行输入一个保留词：", 
            reserved_file,
            on_save_callback=on_save
        )

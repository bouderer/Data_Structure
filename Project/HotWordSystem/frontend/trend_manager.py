import tkinter as tk
from tkinter import simpledialog, messagebox
import charts

class TrendManager:
    """
    这个类用来处理词语的趋势图
    """
    def __init__(self, app):
        self.app = app
        self.multi_trend_data = {}
        self.is_collecting_multi = False

    def show_trend(self):
        # 显示单个词的趋势
        if not self.app.engine.is_running:
            messagebox.showwarning("警告", "请先点击 '开始分析'。")
            return
            
        word = simpledialog.askstring("趋势分析", "请输入要分析的词：")
        if word:
            self.is_collecting_multi = False
            interval = self.app.window_size.get()
            cmd = f"\n[ACTION] GET_TREND WORD={word} INTERVAL={interval}\n"
            self.app.engine.send_command(cmd)

    def show_topk_trend(self):
        # 显示前 K 个热词的对比趋势
        if not self.app.engine.is_running:
            messagebox.showwarning("警告", "请先点击 '开始分析'。")
            return
            
        k = simpledialog.askinteger("Top-K 趋势", "请输入 K 值：", initialvalue=3, minvalue=1, maxvalue=10)
        if k:
            interval = self.app.window_size.get()
            cmd = f"\n[ACTION] GET_TOPK_TREND K={k} INTERVAL={interval}\n"
            self.app.engine.send_command(cmd)

    def handle_trend_data(self, line):
        # 处理后台传回来的趋势数据
        try:
            content = line.replace("[TREND_DATA]", "").strip()
            parts = content.split(" DATA=")
            word = parts[0].replace("WORD=", "")
            data_str = parts[1]
            
            x_data, y_data = [], []
            for item in data_str.split(","):
                if ":" in item:
                    t, c = item.split(":")
                    x_data.append(int(t))
                    y_data.append(int(c))
            
            if self.is_collecting_multi:
                self.multi_trend_data[word] = {'x': x_data, 'y': y_data}
            else:
                if x_data:
                    self.app.root.after(0, lambda: charts.create_trend_window(
                        self.app.root, f"'{word}' 的趋势", {word: {'x': x_data, 'y': y_data}}, is_multi=False
                    ))
                else:
                    self.app.append_colored_text(f">>> 没找到 '{word}' 的历史数据。\n", "warning")
        except Exception as e:
            print(f"解析数据失败: {e}")

    def start_multi_collection(self):
        # 开始收集多个词的数据
        self.is_collecting_multi = True
        self.multi_trend_data = {}

    def end_multi_collection(self):
        # 收集完了，画图
        self.is_collecting_multi = False
        self.app.root.after(0, self.draw_multi_trend_window)

    def draw_multi_trend_window(self):
        # 弹出对比趋势窗口
        if not self.multi_trend_data:
            self.app.append_colored_text(">>> 没拿到趋势数据。\n", "warning")
            return
        
        self.app.root.after(0, lambda: charts.create_trend_window(
            self.app.root, "热词对比趋势", self.multi_trend_data, is_multi=True
        ))

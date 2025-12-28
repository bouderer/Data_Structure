import os
import time
import tkinter as tk
from tkinter import messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.font_manager import FontProperties
import matplotlib.pyplot as plt

try:
    import psutil
    HAS_PSUTIL = True
except ImportError:
    HAS_PSUTIL = False

class ResourceMonitor:
    """
    这个类用来监控电脑的内存占用
    """
    def __init__(self, root):
        self.root = root
        self.res_history = {"times": [], "mem_gui": [], "mem_engine": []}
        self.start_time = time.time()
        self.monitor_win = None
        self.monitor_fig = None
        self.monitor_ax = None
        self.monitor_canvas = None

    def get_usage_text(self, process_pid=None):
        # 获取当前的内存使用情况，返回一段文字
        if not HAS_PSUTIL:
            return "未安装 psutil\n请运行: pip install psutil"
        
        try:
            py_proc = psutil.Process(os.getpid())
            py_mem = py_proc.memory_info().rss / 1024 / 1024 # 换算成 MB
            
            cpp_mem = 0
            if process_pid:
                try:
                    cpp_proc = psutil.Process(process_pid)
                    cpp_mem = cpp_proc.memory_info().rss / 1024 / 1024
                except:
                    pass
            
            total_mem = py_mem + cpp_mem
            
            # 记录一下历史数据，方便画图
            self.res_history["times"].append(time.time() - self.start_time)
            self.res_history["mem_gui"].append(py_mem)
            self.res_history["mem_engine"].append(cpp_mem)
            
            # 只保留最近 100 条数据
            if len(self.res_history["times"]) > 100:
                self.res_history["times"].pop(0)
                self.res_history["mem_gui"].pop(0)
                self.res_history["mem_engine"].pop(0)
                
            return f"界面内存: {py_mem:.1f} MB\n引擎内存: {cpp_mem:.1f} MB\n总计: {total_mem:.1f} MB"
        except Exception as e:
            return f"监控出错了: {e}"

    def open_window(self):
        # 打开一个新窗口来画内存曲线图
        if self.monitor_win and self.monitor_win.winfo_exists():
            self.monitor_win.lift()
            return

        self.monitor_win = tk.Toplevel(self.root)
        self.monitor_win.title("内存监控图")
        self.monitor_win.geometry("800x500")
        
        self.monitor_fig, self.monitor_ax = plt.subplots(figsize=(8, 4), dpi=100)
        self.monitor_canvas = FigureCanvasTkAgg(self.monitor_fig, master=self.monitor_win)
        self.monitor_canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        self.refresh_plot()

    def refresh_plot(self):
        # 刷新曲线图
        if not self.monitor_win or not self.monitor_win.winfo_exists():
            return
            
        self.monitor_ax.clear()
        times = self.res_history["times"]
        gui_mem = self.res_history["mem_gui"]
        eng_mem = self.res_history["mem_engine"]
        
        font = FontProperties(family='Microsoft YaHei', size=9)
        
        self.monitor_ax.plot(times, gui_mem, label="界面 (Python)", color='#569cd6')
        self.monitor_ax.plot(times, eng_mem, label="引擎 (C++)", color='#d62728')
        
        self.monitor_ax.set_title("内存占用趋势", fontproperties=FontProperties(family='Microsoft YaHei', size=12, weight='bold'))
        self.monitor_ax.set_xlabel("时间 (秒)", fontproperties=font)
        self.monitor_ax.set_ylabel("内存 (MB)", fontproperties=font)
        self.monitor_ax.legend(prop=font)
        self.monitor_ax.grid(True, linestyle='--', alpha=0.6)
        
        self.monitor_fig.tight_layout()
        self.monitor_canvas.draw()

import os
import sys
import tkinter as tk
from tkinter import filedialog, messagebox, simpledialog, ttk, scrolledtext
import threading

# 把当前目录加到搜索路径里，方便导入其他文件
current_dir = os.path.dirname(os.path.abspath(__file__))
if current_dir not in sys.path:
    sys.path.append(current_dir)

import monitor, engine, word_manager, query_manager, trend_manager, file_handler

# --- 样式常量 ---
COLOR_BG_DARK = "#1e1e1e"
COLOR_FG_LIGHT = "#d4d4d4"
COLOR_INFO = "#569cd6"
COLOR_SUCCESS = "#6a9955"
COLOR_WARNING = "#ce9178"
COLOR_ERROR = "#f44747"
COLOR_RESULT_HEADER = "#b5cea8"
COLOR_SENSITIVE = "#ff00ff"

FONT_TITLE = ("Microsoft YaHei", 18, "bold")
FONT_CONSOLE = ("Consolas", 10)

# 尝试开启 Windows 高 DPI 意识
try:
    from ctypes import windll
    windll.shcore.SetProcessDpiAwareness(1)
except Exception:
    pass

class HotWordGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("热词统计与分析系统 v1.0")
        
        # 获取当前文件所在的目录 (frontend 文件夹)
        current_dir = os.path.dirname(os.path.abspath(__file__))
        # 系统的根目录 (HotWordSystem 文件夹)
        self.system_root = os.path.dirname(current_dir)
        # 整个项目的根目录 (Data_Structure/Project 文件夹)
        self.project_root = os.path.dirname(self.system_root)
        
        # 初始化各个功能模块
        self.monitor = monitor.ResourceMonitor(self.root)
        # 后端程序的路径 (在 HotWordSystem/backend/bin 下)
        exe_path = os.path.join(self.system_root, "backend", "bin", "hotwords.exe")
        self.engine = engine.EngineClient(exe_path, self.handle_engine_output)
        
        self.word_mgr = word_manager.WordManager(self.root, self.engine)
        self.query_mgr = query_manager.QueryManager(self)
        self.trend_mgr = trend_manager.TrendManager(self)
        self.file_hdl = file_handler.FileHandler(self)
        
        # 设置窗口大小和位置
        self._init_window_geometry()
        
        # 设置界面样式
        self.style = ttk.Style()
        self._configure_styles()
        
        # 画出界面
        self.setup_ui()
        
        # 开始刷新内存显示
        self.update_resource_usage()
        
        # 窗口关闭时要做的事
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

    def _configure_styles(self):
        self.style.theme_use('clam')
        self.style.configure("Vertical.TScrollbar", 
                             gripcount=0, background="#7f8c8d", darkcolor="#2c3e50",
                             lightcolor="#95a5a6", troughcolor="#ecf0f1", bordercolor="#bdc3c7", 
                             arrowcolor="#2c3e50")
        self.style.map("Vertical.TScrollbar", background=[('active', '#34495e'), ('pressed', '#2c3e50')])
        self.style.configure("Big.TButton", font=("Microsoft YaHei", 12, "bold"), padding=10)

    def _init_window_geometry(self):
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()
        width = int(screen_width * 0.8)
        height = int(screen_height * 0.8)
        x = (screen_width - width) // 2
        y = (screen_height - height) // 2
        self.root.geometry(f"{width}x{height}+{x}+{y}")

    def setup_ui(self):
        # 顶部大标题
        title_label = tk.Label(self.root, text="热词统计与分析系统", font=FONT_TITLE, pady=10)
        title_label.pack()

        # 左右分栏
        paned = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        paned.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        # --- 左边：控制按钮区 (带滚动条) ---
        left_container = tk.Frame(paned, highlightthickness=1, highlightbackground="#bdc3c7")
        paned.add(left_container, weight=1)

        bg_color = self.style.lookup("TFrame", "background")
        left_container.configure(bg=bg_color)
        left_container.grid_rowconfigure(0, weight=1)
        left_container.grid_columnconfigure(0, weight=1)

        canvas = tk.Canvas(left_container, highlightthickness=0, borderwidth=0, bg=bg_color)
        canvas.grid(row=0, column=0, sticky="nsew")

        scrollbar_frame = tk.Frame(left_container, bg=bg_color, highlightthickness=1, highlightbackground="#dcdde1")
        scrollbar_frame.grid(row=0, column=1, sticky="ns")

        scrollbar = ttk.Scrollbar(scrollbar_frame, orient="vertical", command=canvas.yview)
        scrollbar.pack(fill=tk.Y, expand=True)
        
        control_frame = ttk.Frame(canvas, padding="10")
        def update_scroll_region(event):
            canvas.configure(scrollregion=canvas.bbox("all"))
        control_frame.bind("<Configure>", update_scroll_region)

        canvas_window = canvas.create_window((0, 0), window=control_frame, anchor="nw")
        def on_canvas_configure(event):
            canvas.itemconfig(canvas_window, width=event.width)
        canvas.bind("<Configure>", on_canvas_configure)
        canvas.configure(yscrollcommand=scrollbar.set)

        # 鼠标滚轮支持
        def _on_mousewheel(event):
            canvas.yview_scroll(int(-1*(event.delta/120)), "units")
        left_container.bind("<Enter>", lambda _: canvas.bind_all("<MouseWheel>", _on_mousewheel))
        left_container.bind("<Leave>", lambda _: canvas.unbind_all("<MouseWheel>"))

        # 1. 文件选择
        file_group = ttk.LabelFrame(control_frame, text="数据源控制", padding="10")
        file_group.pack(fill=tk.X, pady=5)
        ttk.Button(file_group, text="📁 选择输入文件", command=self.file_hdl.select_file).pack(fill=tk.X, pady=2)
        self.file_label = ttk.Label(file_group, text="未选择文件", foreground="gray")
        self.file_label.pack(fill=tk.X, pady=2)

        # 2. 参数设置
        param_group = ttk.LabelFrame(control_frame, text="分析参数", padding="10")
        param_group.pack(fill=tk.X, pady=5)
        ttk.Label(param_group, text="滑动窗口大小 (秒):").pack(anchor=tk.W)
        self.window_size = tk.IntVar(value=600)
        ttk.Entry(param_group, textvariable=self.window_size).pack(fill=tk.X, pady=2)
        ttk.Button(param_group, text="🛡️ 敏感词管理", command=self.word_mgr.manage_sensitive_words).pack(fill=tk.X, pady=5)
        ttk.Button(param_group, text="🔖 保留词管理", command=self.word_mgr.manage_reserved_words).pack(fill=tk.X, pady=5)

        # 3. 开始/停止按钮
        run_group = ttk.LabelFrame(control_frame, text="执行控制", padding="10")
        run_group.pack(fill=tk.X, pady=5)
        self.btn_run = ttk.Button(run_group, text="🚀 开始分析", command=self.run_analysis, style="Big.TButton")
        self.btn_run.pack(fill=tk.X, pady=5)
        self.btn_stop = ttk.Button(run_group, text="⏹ 停止分析", command=self.stop_analysis, state=tk.DISABLED, style="Big.TButton")
        self.btn_stop.pack(fill=tk.X, pady=5)

        trend_btn_frame = ttk.Frame(run_group)
        trend_btn_frame.pack(fill=tk.X, pady=2)
        ttk.Button(trend_btn_frame, text="📊 单词趋势", command=self.trend_mgr.show_trend).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0,2))
        ttk.Button(trend_btn_frame, text="📈 Top-K 趋势", command=self.trend_mgr.show_topk_trend).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(2,0))
        ttk.Button(run_group, text="🧹 清空日志", command=self.clear_output).pack(fill=tk.X, pady=2)

        # 4. 历史查询
        history_group = ttk.LabelFrame(control_frame, text="历史范围查询", padding="10")
        history_group.pack(fill=tk.X, pady=5)
        ttk.Label(history_group, text="Top-K 数量:").pack(anchor=tk.W)
        self.k_value = tk.IntVar(value=5)
        ttk.Spinbox(history_group, from_=1, to=100, textvariable=self.k_value).pack(fill=tk.X, pady=2)
        ttk.Label(history_group, text="开始时间:").pack(anchor=tk.W)
        start_time_frame = ttk.Frame(history_group)
        start_time_frame.pack(fill=tk.X)
        self.start_h, self.start_m, self.start_s = tk.IntVar(value=0), tk.IntVar(value=0), tk.IntVar(value=0)
        ttk.Spinbox(start_time_frame, from_=0, to=23, width=5, textvariable=self.start_h).pack(side=tk.LEFT, padx=1)
        ttk.Label(start_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Spinbox(start_time_frame, from_=0, to=59, width=5, textvariable=self.start_m).pack(side=tk.LEFT, padx=1)
        ttk.Label(start_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Spinbox(start_time_frame, from_=0, to=59, width=5, textvariable=self.start_s).pack(side=tk.LEFT, padx=1)

        ttk.Label(history_group, text="结束时间:").pack(anchor=tk.W, pady=(5,0))
        end_time_frame = ttk.Frame(history_group)
        end_time_frame.pack(fill=tk.X)
        self.end_h, self.end_m, self.end_s = tk.IntVar(value=0), tk.IntVar(value=3), tk.IntVar(value=0)
        ttk.Spinbox(end_time_frame, from_=0, to=23, width=5, textvariable=self.end_h).pack(side=tk.LEFT, padx=1)
        ttk.Label(end_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Spinbox(end_time_frame, from_=0, to=59, width=5, textvariable=self.end_m).pack(side=tk.LEFT, padx=1)
        ttk.Label(end_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Spinbox(end_time_frame, from_=0, to=59, width=5, textvariable=self.end_s).pack(side=tk.LEFT, padx=1)
        ttk.Button(history_group, text="🔍 执行历史查询", command=self.query_mgr.run_history_query).pack(fill=tk.X, pady=10)

        # 5. 内存监控
        monitor_group = ttk.LabelFrame(control_frame, text="系统资源监控", padding="10")
        monitor_group.pack(fill=tk.X, pady=5)
        self.mem_var = tk.StringVar(value="正在初始化监控...")
        ttk.Label(monitor_group, textvariable=self.mem_var, font=("Consolas", 9), justify=tk.LEFT).pack(anchor=tk.W)
        ttk.Button(monitor_group, text="📈 打开图形化监控", command=self.open_resource_monitor).pack(fill=tk.X, pady=5)

        # --- 右边：显示文字区 ---
        display_frame = ttk.Frame(paned)
        paned.add(display_frame, weight=4)
        input_labelframe = ttk.LabelFrame(display_frame, text="输入内容", padding="5")
        input_labelframe.pack(fill=tk.BOTH, expand=True, pady=5)
        self.input_area = scrolledtext.ScrolledText(input_labelframe, font=FONT_CONSOLE, height=10)
        self.input_area.pack(fill=tk.BOTH, expand=True)

        output_labelframe = ttk.LabelFrame(display_frame, text="分析结果控制台", padding="5")
        output_labelframe.pack(fill=tk.BOTH, expand=True, pady=5)
        self.output_area = scrolledtext.ScrolledText(output_labelframe, font=FONT_CONSOLE, bg=COLOR_BG_DARK, fg=COLOR_FG_LIGHT, insertbackground="white")
        self.output_area.pack(fill=tk.BOTH, expand=True)
        
        # 设置文字颜色
        self.output_area.tag_config("info", foreground=COLOR_INFO)
        self.output_area.tag_config("success", foreground=COLOR_SUCCESS)
        self.output_area.tag_config("warning", foreground=COLOR_WARNING)
        self.output_area.tag_config("error", foreground=COLOR_ERROR, font=("Consolas", 10, "bold"))
        self.output_area.tag_config("result_header", foreground=COLOR_RESULT_HEADER, font=("Consolas", 11, "bold"))
        self.output_area.tag_config("sensitive", foreground=COLOR_SENSITIVE, font=("Consolas", 10, "italic"))

        # 底部状态栏
        self.status_var = tk.StringVar(value="系统就绪")
        status_bar = ttk.Label(self.root, textvariable=self.status_var, relief=tk.SUNKEN, anchor=tk.W)
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)

    def run_analysis(self):
        # 点击“开始分析”后的逻辑
        input_text = self.input_area.get(1.0, tk.END).strip()
        if not input_text:
            messagebox.showwarning("警告", "请输入内容或选择文件！")
            return

        k = self.k_value.get()
        w_size = self.window_size.get()
        init_commands = f"[ACTION] SET_WINDOW SIZE={w_size}\n"
        
        # 确保 input_text 后面补上换行符，防止 .strip() 删掉末尾回车导致后端 getline 阻塞
        full_input = init_commands + input_text + "\n"
        
        if "[ACTION] QUERY" not in input_text:
            full_input += f"[ACTION] QUERY K={k}\n"

        self.btn_run.config(state=tk.DISABLED)
        self.btn_stop.config(state=tk.NORMAL)
        self.output_area.delete(1.0, tk.END)
        self.append_colored_text(f">>> 启动分析引擎...\n", "info")
        self.engine.start(full_input)

    def stop_analysis(self):
        # 点击“停止分析”后的逻辑
        self.engine.stop()
        self.append_colored_text("\n>>> 分析已停止。\n", "warning")
        self.btn_run.config(state=tk.NORMAL)
        self.btn_stop.config(state=tk.DISABLED)
        self.status_var.set("分析停止")
        self.file_hdl.save_to_file(self.output_area.get(1.0, tk.END))

    def update_resource_usage(self):
        # 每隔2秒更新一次内存显示
        usage_text = self.monitor.get_usage_text(self.engine.get_pid())
        self.mem_var.set(usage_text)
        if self.monitor.monitor_win and self.monitor.monitor_win.winfo_exists():
            self.root.after(0, self.monitor.refresh_plot)
        if self.root.winfo_exists():
            self.root.after(2000, self.update_resource_usage)

    def open_resource_monitor(self):
        # 打开那个画内存曲线的窗口
        self.monitor.open_window()

    def on_closing(self):
        # 关掉窗口时，先把后台进程杀掉
        if self.engine.is_running:
            self.engine.stop()
        self.file_hdl.save_to_file(self.output_area.get(1.0, tk.END))
        self.root.destroy()

    def handle_engine_output(self, line):
        # 处理 C++ 传回来的每一行字
        if not line: return
        if "[QUERY RESULT]" in line:
            self.append_colored_text(line, "result_header")
        elif "[HISTORY RESULT]" in line:
            self.append_colored_text(line, "success")
        elif "[SENSITIVE_SUMMARY]" in line:
            summary = line.replace("[SENSITIVE_SUMMARY]", "").strip()
            self.append_colored_text(f"⚠️ 敏感词统计: {summary}\n", "sensitive")
        elif "[SENSITIVE_DETECTED]" in line:
            word = line.replace("[SENSITIVE_DETECTED]", "").strip()
            self.append_colored_text(f"⚠️ 发现敏感词: {word}\n", "sensitive")
        elif "[TREND_DATA]" in line:
            self.trend_mgr.handle_trend_data(line)
        elif "[MULTI_TREND_START]" in line:
            self.trend_mgr.start_multi_collection()
        elif "[MULTI_TREND_END]" in line:
            self.trend_mgr.end_multi_collection()
        elif "Error" in line or "错误" in line:
            self.append_colored_text(line, "error")
        elif "System exited" in line:
            self.root.after(0, lambda: self.btn_run.config(state=tk.NORMAL))
            self.root.after(0, lambda: self.btn_stop.config(state=tk.DISABLED))
            self.status_var.set("分析完成")
        else:
            self.append_colored_text(line, "")

    def append_colored_text(self, text, tag):
        self.root.after(0, lambda: self._do_append(text, tag))

    def _do_append(self, text, tag):
        self.output_area.insert(tk.END, text, tag)
        self.output_area.see(tk.END)

    def clear_output(self):
        # 清空控制台
        self.output_area.delete(1.0, tk.END)

if __name__ == "__main__":
    print("正在启动 Tkinter...")
    try:
        root = tk.Tk()
        print("Tkinter 窗口已创建")
        # 设置全局字体
        default_font = ("Microsoft YaHei", 9)
        root.option_add("*Font", default_font)
        
        print("正在初始化 GUI 类...")
        app = HotWordGUI(root)
        print("GUI 初始化完成，进入主循环")
        root.mainloop()
    except Exception as e:
        print(f"程序崩溃了: {e}")
        import traceback
        traceback.print_exc()
        input("按回车键退出...")

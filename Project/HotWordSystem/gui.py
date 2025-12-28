import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext, ttk, simpledialog
import subprocess
import threading
import os
import time

# 尝试导入 psutil 用于内存监控
try:
    import psutil
    HAS_PSUTIL = True
except ImportError:
    HAS_PSUTIL = False

# 尝试导入 matplotlib
try:
    import matplotlib.pyplot as plt
    from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
    from matplotlib.font_manager import FontProperties
    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False

# 尝试开启 Windows 高 DPI 意识，解决界面模糊问题
try:
    from ctypes import windll
    windll.shcore.SetProcessDpiAwareness(1)
except Exception:
    pass

class HotWordGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("热词统计与分析系统 v1.0")
        
        # 获取屏幕宽高，设置一个更合适的初始比例（例如屏幕的 80%）
        screen_width = self.root.winfo_screenwidth()
        screen_height = self.root.winfo_screenheight()
        width = int(screen_width * 0.8)
        height = int(screen_height * 0.8)
        
        # 居中显示
        x = (screen_width - width) // 2
        y = (screen_height - height) // 2
        self.root.geometry(f"{width}x{height}+{x}+{y}")
        
        # 设置主题样式
        self.style = ttk.Style()
        self.style.theme_use('clam') # 使用更现代的界面风格
        
        # 自定义滑块颜色 (深色滑块)
        self.style.configure("Vertical.TScrollbar", 
                             gripcount=0,
                             background="#7f8c8d",    # 滑块颜色 (深灰色)
                             darkcolor="#2c3e50",      # 边框深色
                             lightcolor="#95a5a6",     # 边框浅色
                             troughcolor="#ecf0f1",    # 轨道颜色
                             bordercolor="#bdc3c7", 
                             arrowcolor="#2c3e50")     # 箭头颜色
        
        # 鼠标悬停时的颜色
        self.style.map("Vertical.TScrollbar",
                       background=[('active', '#34495e'), ('pressed', '#2c3e50')])
        
        self.setup_ui()
        self.process = None
        self.is_running = False
        self.multi_trend_data = {}
        self.is_collecting_multi = False
        
        # 资源监控数据
        self.res_history = {"times": [], "mem_gui": [], "mem_engine": []}
        self.start_time = time.time()
        self.monitor_win = None
        
        # 启动资源监控
        self.update_resource_usage()
        
        # 绑定窗口关闭事件
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)

    def setup_ui(self):
        # --- 顶部标题 ---
        title_label = tk.Label(self.root, text="热词统计与分析系统", font=("Microsoft YaHei", 18, "bold"), pady=10)
        title_label.pack()

        # --- 主容器 (左右布局) ---
        paned = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        paned.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)

        # --- 左侧：控制面板 (带滚动条) ---
        # 为左侧容器添加一个明显的边框
        left_container = tk.Frame(paned, highlightthickness=1, highlightbackground="#bdc3c7")
        paned.add(left_container, weight=1)

        # 获取背景色以匹配主题
        bg_color = self.style.lookup("TFrame", "background")
        left_container.configure(bg=bg_color)

        # 使用 grid 布局来精确控制画布和滚动条
        left_container.grid_rowconfigure(0, weight=1)
        left_container.grid_columnconfigure(0, weight=1)

        canvas = tk.Canvas(left_container, highlightthickness=0, borderwidth=0, bg=bg_color)
        canvas.grid(row=0, column=0, sticky="nsew")

        # 为滚动条区域也加一个微弱的左边框，使其与内容区分
        scrollbar_frame = tk.Frame(left_container, bg=bg_color, highlightthickness=1, highlightbackground="#dcdde1")
        scrollbar_frame.grid(row=0, column=1, sticky="ns")

        scrollbar = ttk.Scrollbar(scrollbar_frame, orient="vertical", command=canvas.yview)
        scrollbar.pack(fill=tk.Y, expand=True)
        
        # 创建真正放置内容的 Frame
        control_frame = ttk.Frame(canvas, padding="10")
        
        # 绑定滚动区域
        def update_scroll_region(event):
            canvas.configure(scrollregion=canvas.bbox("all"))
        control_frame.bind("<Configure>", update_scroll_region)

        # 在画布上创建窗口
        canvas_window = canvas.create_window((0, 0), window=control_frame, anchor="nw")
        
        # 让 control_frame 的宽度始终跟随 canvas 宽度
        def on_canvas_configure(event):
            canvas.itemconfig(canvas_window, width=event.width)
        canvas.bind("<Configure>", on_canvas_configure)

        canvas.configure(yscrollcommand=scrollbar.set)

        # 鼠标滚轮支持
        def _on_mousewheel(event):
            canvas.yview_scroll(int(-1*(event.delta/120)), "units")
        
        left_container.bind("<Enter>", lambda _: canvas.bind_all("<MouseWheel>", _on_mousewheel))
        left_container.bind("<Leave>", lambda _: canvas.unbind_all("<MouseWheel>"))

        # 文件操作区
        file_group = ttk.LabelFrame(control_frame, text="数据源控制", padding="10")
        file_group.pack(fill=tk.X, pady=5)

        ttk.Button(file_group, text="📁 选择输入文件", command=self.select_file).pack(fill=tk.X, pady=2)
        self.file_label = ttk.Label(file_group, text="未选择文件", foreground="gray")
        self.file_label.pack(fill=tk.X, pady=2)

        # 参数设置区
        param_group = ttk.LabelFrame(control_frame, text="分析参数", padding="10")
        param_group.pack(fill=tk.X, pady=5)

        ttk.Label(param_group, text="滑动窗口大小 (秒):").pack(anchor=tk.W)
        self.window_size = tk.IntVar(value=600)
        ttk.Entry(param_group, textvariable=self.window_size).pack(fill=tk.X, pady=2)

        ttk.Button(param_group, text="🛡️ 敏感词管理", command=self.manage_sensitive_words).pack(fill=tk.X, pady=5)
        ttk.Button(param_group, text="🔖 保留词管理", command=self.manage_reserved_words).pack(fill=tk.X, pady=5)

        # 执行控制区
        run_group = ttk.LabelFrame(control_frame, text="执行控制", padding="10")
        run_group.pack(fill=tk.X, pady=5)

        # 增大按钮样式
        self.style.configure("Big.TButton", font=("Microsoft YaHei", 12, "bold"), padding=10)

        self.btn_run = ttk.Button(run_group, text="🚀 开始分析", command=self.run_analysis, style="Big.TButton")
        self.btn_run.pack(fill=tk.X, pady=5)

        self.btn_stop = ttk.Button(run_group, text="⏹ 停止分析", command=self.stop_analysis, state=tk.DISABLED, style="Big.TButton")
        self.btn_stop.pack(fill=tk.X, pady=5)

        trend_btn_frame = ttk.Frame(run_group)
        trend_btn_frame.pack(fill=tk.X, pady=2)
        ttk.Button(trend_btn_frame, text="📊 单词趋势", command=self.show_trend).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(0,2))
        ttk.Button(trend_btn_frame, text="📈 Top-K 趋势", command=self.show_topk_trend).pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(2,0))
        
        ttk.Button(run_group, text="🧹 清空日志", command=self.clear_output).pack(fill=tk.X, pady=2)

        # 历史查询区
        history_group = ttk.LabelFrame(control_frame, text="历史范围查询", padding="10")
        history_group.pack(fill=tk.X, pady=5)

        ttk.Label(history_group, text="Top-K 数量:").pack(anchor=tk.W)
        self.k_value = tk.IntVar(value=5)
        ttk.Spinbox(history_group, from_=1, to=100, textvariable=self.k_value).pack(fill=tk.X, pady=2)

        ttk.Label(history_group, text="开始时间:").pack(anchor=tk.W)
        start_time_frame = ttk.Frame(history_group)
        start_time_frame.pack(fill=tk.X)
        self.start_h = tk.IntVar(value=0)
        self.start_m = tk.IntVar(value=0)
        self.start_s = tk.IntVar(value=0)
        ttk.Spinbox(start_time_frame, from_=0, to=23, width=5, textvariable=self.start_h).pack(side=tk.LEFT, padx=1)
        ttk.Label(start_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Spinbox(start_time_frame, from_=0, to=59, width=5, textvariable=self.start_m).pack(side=tk.LEFT, padx=1)
        ttk.Label(start_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Spinbox(start_time_frame, from_=0, to=59, width=5, textvariable=self.start_s).pack(side=tk.LEFT, padx=1)

        ttk.Label(history_group, text="结束时间:").pack(anchor=tk.W, pady=(5,0))
        end_time_frame = ttk.Frame(history_group)
        end_time_frame.pack(fill=tk.X)
        self.end_h = tk.IntVar(value=0)
        self.end_m = tk.IntVar(value=3)
        self.end_s = tk.IntVar(value=0)
        ttk.Spinbox(end_time_frame, from_=0, to=23, width=5, textvariable=self.end_h).pack(side=tk.LEFT, padx=1)
        ttk.Label(end_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Spinbox(end_time_frame, from_=0, to=59, width=5, textvariable=self.end_m).pack(side=tk.LEFT, padx=1)
        ttk.Label(end_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Spinbox(end_time_frame, from_=0, to=59, width=5, textvariable=self.end_s).pack(side=tk.LEFT, padx=1)

        ttk.Button(history_group, text="🔍 执行历史查询", command=self.run_history_query).pack(fill=tk.X, pady=10)

        # 系统资源监控区
        monitor_group = ttk.LabelFrame(control_frame, text="系统资源监控", padding="10")
        monitor_group.pack(fill=tk.X, pady=5)
        
        self.mem_var = tk.StringVar(value="正在初始化监控...")
        ttk.Label(monitor_group, textvariable=self.mem_var, font=("Consolas", 9), justify=tk.LEFT).pack(anchor=tk.W)
        ttk.Button(monitor_group, text="📈 打开图形化监控", command=self.open_resource_monitor).pack(fill=tk.X, pady=5)

        # --- 右侧：显示区域 (上下布局) ---
        display_frame = ttk.Frame(paned)
        paned.add(display_frame, weight=4)

        # 输入预览
        input_labelframe = ttk.LabelFrame(display_frame, text="输入", padding="5")
        input_labelframe.pack(fill=tk.BOTH, expand=True, pady=5)
        self.input_area = scrolledtext.ScrolledText(input_labelframe, font=("Consolas", 10), height=10)
        self.input_area.pack(fill=tk.BOTH, expand=True)

        # 输出结果
        output_labelframe = ttk.LabelFrame(display_frame, text="分析控制台 (Analysis Console)", padding="5")
        output_labelframe.pack(fill=tk.BOTH, expand=True, pady=5)
        self.output_area = scrolledtext.ScrolledText(output_labelframe, font=("Consolas", 10), bg="#1e1e1e", fg="#d4d4d4", insertbackground="white")
        self.output_area.pack(fill=tk.BOTH, expand=True)
        
        # 设置输出文字颜色标签
        self.output_area.tag_config("info", foreground="#569cd6")
        self.output_area.tag_config("success", foreground="#6a9955")
        self.output_area.tag_config("warning", foreground="#ce9178")
        self.output_area.tag_config("error", foreground="#f44747", font=("Consolas", 10, "bold"))
        self.output_area.tag_config("result_header", foreground="#b5cea8", font=("Consolas", 11, "bold"))
        self.output_area.tag_config("sensitive", foreground="#ff00ff", font=("Consolas", 10, "italic"))

        # --- 底部状态栏 ---
        self.status_var = tk.StringVar(value="系统就绪")
        status_bar = ttk.Label(self.root, textvariable=self.status_var, relief=tk.SUNKEN, anchor=tk.W)
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)

    def select_file(self):
        file_path = filedialog.askopenfilename(initialdir="..", title="选择测试文件", filetypes=(("Text files", "*.txt"), ("all files", "*.*")))
        if file_path:
            self.current_file = file_path
            self.file_label.config(text=os.path.basename(file_path), foreground="black")
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read() # 读取全部内容
                    self.input_area.delete(1.0, tk.END)
                    self.input_area.insert(tk.END, content)
                    self.status_var.set(f"已加载文件: {os.path.basename(file_path)}")
            except Exception as e:
                messagebox.showerror("错误", f"无法读取文件: {e}")

    def run_analysis(self):
        # 获取输入框中的所有文本
        input_text = self.input_area.get(1.0, tk.END).strip()
        if not input_text:
            messagebox.showwarning("警告", "输入窗口为空，请输入内容或选择文件！")
            return

        # 获取 UI 上的参数并构造初始指令
        k = self.k_value.get()
        w_size = self.window_size.get()
        
        # 构造初始指令序列
        # 1. 设置窗口大小
        # 2. 原始文本内容
        # 3. 如果文本里没有 QUERY 指令，我们自动在最后加一个
        init_commands = f"[ACTION] SET_WINDOW SIZE={w_size}\n"
        
        full_input = init_commands + input_text
        
        # 如果用户没写 QUERY，自动补一个
        if "[ACTION] QUERY" not in input_text:
            full_input += f"\n[ACTION] QUERY K={k}\n"

        self.is_running = True
        self.btn_run.config(state=tk.DISABLED)
        self.btn_stop.config(state=tk.NORMAL)
        self.output_area.delete(1.0, tk.END)
        self.append_colored_text(f">>> 启动分析引擎 (窗口大小: {w_size}s, Top-{k})...\n", "info")
        self.append_colored_text(">>> 提示：分析完成后将自动输出【当前窗口】的热词统计。\n", "info")
        
        # 在后台线程运行，并将文本传给 C++ 的 stdin
        thread = threading.Thread(target=self.execute_cpp, args=(full_input,), daemon=True)
        thread.start()

    def run_history_query(self):
        # 检查进程是否真的在运行
        if not self.process or self.process.poll() is not None:
            messagebox.showwarning("警告", "分析引擎未运行。请先点击 '开始分析'。")
            return
        
        def safe_get(var, default=0):
            try:
                return var.get()
            except:
                return default

        # 获取数值进行校验
        sh, sm, ss = safe_get(self.start_h), safe_get(self.start_m), safe_get(self.start_s)
        eh, em, es = safe_get(self.end_h), safe_get(self.end_m), safe_get(self.end_s)

        # 校验逻辑：小于0，或者分/秒大于等于60
        invalid_range = False
        if any(v < 0 for v in [sh, sm, ss, eh, em, es]):
            invalid_range = True
        if any(v >= 60 for v in [sm, ss, em, es]):
            invalid_range = True
            
        if invalid_range:
            messagebox.showerror("时间格式错误", "请输入有效的时间：\n1. 数值不能小于 0\n2. 分钟和秒钟必须在 0-59 之间")
            return

        # 格式化时间字符串 [HH:MM:SS]
        start = f"{sh:02d}:{sm:02d}:{ss:02d}"
        end = f"{eh:02d}:{em:02d}:{es:02d}"
        k = safe_get(self.k_value, 5)
        
        # 发送指令给 C++
        cmd = f"\n[ACTION] HISTORY_QUERY START=[{start}] END=[{end}] K={k}\n"
        try:
            self.process.stdin.write(cmd)
            self.process.stdin.flush()
        except Exception as e:
            self.append_colored_text(f"发送历史查询指令失败: {e}\n", "error")

    def stop_analysis(self):
        if self.process:
            try:
                self.process.stdin.write("\n[ACTION] EXIT\n") # 尝试优雅退出
                self.process.stdin.flush()
            except:
                pass
            self.process.terminate()
            self.is_running = False
            self.append_colored_text("\n>>> 分析已手动停止。\n", "warning")
            self.btn_run.config(state=tk.NORMAL)
            self.btn_stop.config(state=tk.DISABLED)
            self.status_var.set("分析停止")
            self.save_to_file()

    def update_resource_usage(self):
        """定期更新内存占用情况"""
        try:
            if HAS_PSUTIL:
                # 获取当前 Python 进程
                py_proc = psutil.Process(os.getpid())
                py_mem = py_proc.memory_info().rss / 1024 / 1024 # MB
                
                # 获取 C++ 引擎进程
                cpp_mem = 0
                if self.process and self.process.poll() is None:
                    try:
                        cpp_proc = psutil.Process(self.process.pid)
                        cpp_mem = cpp_proc.memory_info().rss / 1024 / 1024 # MB
                    except:
                        pass
                
                total_mem = py_mem + cpp_mem
                self.mem_var.set(f"GUI内存: {py_mem:.1f} MB\n引擎内存: {cpp_mem:.1f} MB\n总计: {total_mem:.1f} MB")
                
                # 记录历史
                self.res_history["times"].append(time.time() - self.start_time)
                self.res_history["mem_gui"].append(py_mem)
                self.res_history["mem_engine"].append(cpp_mem)
                
                # 限制历史长度 (保留最近 100 个点)
                if len(self.res_history["times"]) > 100:
                    self.res_history["times"].pop(0)
                    self.res_history["mem_gui"].pop(0)
                    self.res_history["mem_engine"].pop(0)
                
                # 如果监控窗口打开，通知其更新
                if self.monitor_win and self.monitor_win.winfo_exists():
                    self.root.after(0, self.refresh_monitor_plot)
            else:
                self.mem_var.set("未安装 psutil\n请运行: pip install psutil")
        except Exception as e:
            self.mem_var.set(f"监控异常: {e}")
        
        # 每 2 秒更新一次
        if self.root.winfo_exists():
            self.root.after(2000, self.update_resource_usage)

    def open_resource_monitor(self):
        """打开独立的图形化资源监控窗口"""
        if self.monitor_win and self.monitor_win.winfo_exists():
            self.monitor_win.lift()
            return

        if not HAS_MATPLOTLIB:
            messagebox.showwarning("警告", "未安装 matplotlib，无法显示图形化监控。")
            return

        self.monitor_win = tk.Toplevel(self.root)
        self.monitor_win.title("系统资源实时监控")
        self.monitor_win.geometry("800x500")
        
        # 创建图表
        self.monitor_fig, self.monitor_ax = plt.subplots(figsize=(8, 4), dpi=100)
        self.monitor_canvas = FigureCanvasTkAgg(self.monitor_fig, master=self.monitor_win)
        self.monitor_canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True)
        
        self.refresh_monitor_plot()

    def refresh_monitor_plot(self):
        """刷新监控窗口中的图表"""
        if not self.monitor_win or not self.monitor_win.winfo_exists():
            return
            
        self.monitor_ax.clear()
        times = self.res_history["times"]
        gui_mem = self.res_history["mem_gui"]
        eng_mem = self.res_history["mem_engine"]
        
        font = FontProperties(family='Microsoft YaHei', size=9)
        
        self.monitor_ax.plot(times, gui_mem, label="GUI (Python)", color='#569cd6')
        self.monitor_ax.plot(times, eng_mem, label="引擎 (C++)", color='#d62728')
        
        self.monitor_ax.set_title("系统内存占用实时趋势", fontproperties=FontProperties(family='Microsoft YaHei', size=12, weight='bold'))
        self.monitor_ax.set_xlabel("运行时间 (秒)", fontproperties=font)
        self.monitor_ax.set_ylabel("内存占用 (MB)", fontproperties=font)
        self.monitor_ax.legend(prop=font)
        self.monitor_ax.grid(True, linestyle='--', alpha=0.6)
        
        self.monitor_fig.tight_layout()
        self.monitor_canvas.draw()

    def save_to_file(self):
        """将控制台输出保存到 project 目录下的 output.txt"""
        try:
            content = self.output_area.get(1.0, tk.END).strip()
            if not content:
                return
            
            # 获取 gui.py 所在的目录，然后找到其父目录 (project)
            current_dir = os.path.dirname(os.path.abspath(__file__))
            project_dir = os.path.dirname(current_dir)
            output_path = os.path.join(project_dir, "data", "output.txt")
            
            with open(output_path, "w", encoding="utf-8") as f:
                f.write(content)
            self.append_colored_text(f"\n[系统] 输出已自动保存至: {output_path}\n", "info")
        except Exception as e:
            print(f"保存文件失败: {e}")

    def on_closing(self):
        """关闭窗口时的处理"""
        if self.is_running:
            self.stop_analysis()
        else:
            self.save_to_file()
        self.root.destroy()

    def execute_cpp(self, input_text):
        try:
            exe_path = os.path.join("bin", "hotwords.exe")
            if not os.path.exists(exe_path):
                self.append_colored_text("错误: 找不到 bin/hotwords.exe，请先编译 C++ 代码！\n", "error")
                return

            # 启动进程
            env = os.environ.copy()
            env["PYTHONIOENCODING"] = "utf-8"
            
            self.process = subprocess.Popen(
                [exe_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                encoding='utf-8',
                errors='replace',
                bufsize=1,
                env=env
            )

            self.status_var.set("正在分析中...")

            # --- 核心修复：开启专门的线程喂数据，不再立即关闭 stdin 以保持交互 ---
            def feed_input():
                try:
                    self.process.stdin.write(input_text)
                    self.process.stdin.flush()
                    # self.process.stdin.close() # 注释掉这一行，保持连接
                except Exception as e:
                    print(f"Stdin Error: {e}")

            feeder = threading.Thread(target=feed_input, daemon=True)
            feeder.start()
            
            # 主线程立即开始读取输出
            for line in self.process.stdout:
                if not self.is_running: break
                
                # 根据内容着色
                if "[QUERY RESULT]" in line:
                    self.append_colored_text(line, "result_header")
                elif "[HISTORY RESULT]" in line:
                    self.append_colored_text(line, "success")
                elif "[SENSITIVE_SUMMARY]" in line:
                    summary = line.replace("[SENSITIVE_SUMMARY]", "").strip()
                    self.append_colored_text(f"⚠️ 过滤敏感词统计: {summary}\n", "sensitive")
                elif "[SENSITIVE_DETECTED]" in line:
                    word = line.replace("[SENSITIVE_DETECTED]", "").strip()
                    self.append_colored_text(f"⚠️ 过滤敏感词: {word}\n", "sensitive")
                elif "[TREND_DATA]" in line:
                    self.handle_trend_data(line)
                elif "[MULTI_TREND_START]" in line:
                    self.is_collecting_multi = True
                    self.multi_trend_data = {}
                elif "[MULTI_TREND_END]" in line:
                    self.is_collecting_multi = False
                    self.root.after(0, self.draw_multi_trend_window)
                elif "Error" in line or "错误" in line:
                    self.append_colored_text(line, "error")
                else:
                    self.append_colored_text(line, "")
            
            self.process.wait()
            if self.is_running:
                self.append_colored_text("\n>>> 分析任务完成。\n", "success")
                self.status_var.set("分析完成")
        except Exception as e:
            self.append_colored_text(f"\n发生异常: {str(e)}\n", "error")
        finally:
            self.is_running = False
            self.root.after(0, lambda: self.btn_run.config(state=tk.NORMAL))
            self.root.after(0, lambda: self.btn_stop.config(state=tk.DISABLED))

    def append_colored_text(self, text, tag):
        self.root.after(0, lambda: self._do_append(text, tag))

    def _do_append(self, text, tag):
        self.output_area.insert(tk.END, text, tag)
        self.output_area.see(tk.END)

    def clear_output(self):
        self.output_area.delete(1.0, tk.END)

    def manage_sensitive_words(self):
        # 创建一个子窗口
        win = tk.Toplevel(self.root)
        win.title("敏感词管理")
        win.geometry("400x500")
        win.transient(self.root) # 设置为父窗口的子窗口
        win.grab_set() # 模态窗口，防止操作主窗口
        
        label = ttk.Label(win, text="每行输入一个敏感词：", padding=10)
        label.pack(anchor=tk.W)
        
        text_area = scrolledtext.ScrolledText(win, font=("Microsoft YaHei", 10))
        text_area.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        # 加载现有敏感词
        sensitive_file = os.path.join("..", "dict", "sensitive_words.utf8")
        initial_content = ""
        if os.path.exists(sensitive_file):
            try:
                with open(sensitive_file, 'r', encoding='utf-8') as f:
                    initial_content = f.read().strip()
                    text_area.insert(tk.END, initial_content)
            except Exception as e:
                messagebox.showerror("错误", f"无法读取敏感词文件: {e}")
        
        def save_words(quiet=False):
            content = text_area.get(1.0, tk.END).strip()
            try:
                # 确保目录存在
                os.makedirs(os.path.dirname(sensitive_file), exist_ok=True)
                with open(sensitive_file, 'w', encoding='utf-8') as f:
                    f.write(content)
                if not quiet:
                    messagebox.showinfo("成功", "敏感词库已更新！下次分析时生效。")
                return True
            except Exception as e:
                messagebox.showerror("错误", f"保存失败: {e}")
                return False
            
        def on_closing():
            current_content = text_area.get(1.0, tk.END).strip()
            if current_content != initial_content:
                answer = messagebox.askyesnocancel("保存修改", "敏感词列表已修改，是否保存？")
                if answer is True: # 是
                    if save_words(quiet=True):
                        win.destroy()
                elif answer is False: # 否
                    win.destroy()
                else: # 取消
                    pass
            else:
                win.destroy()

        win.protocol("WM_DELETE_WINDOW", on_closing)
            
        btn_frame = ttk.Frame(win)
        btn_frame.pack(pady=10)
        
        btn_save = ttk.Button(btn_frame, text="保存修改", command=lambda: save_words())
        btn_save.pack(side=tk.LEFT, padx=5)
        
        btn_close = ttk.Button(btn_frame, text="关闭", command=on_closing)
        btn_close.pack(side=tk.LEFT, padx=5)

    def manage_reserved_words(self):
        # 创建一个子窗口
        win = tk.Toplevel(self.root)
        win.title("保留词管理 (自定义词典)")
        win.geometry("400x500")
        win.transient(self.root)
        win.grab_set()
        
        label = ttk.Label(win, text="每行输入一个保留词（防止被切分）：", padding=10)
        label.pack(anchor=tk.W)
        
        text_area = scrolledtext.ScrolledText(win, font=("Microsoft YaHei", 10))
        text_area.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        # 加载现有保留词 (使用 user.dict.utf8)
        reserved_file = os.path.join("..", "dict", "user.dict.utf8")
        initial_content = ""
        if os.path.exists(reserved_file):
            try:
                with open(reserved_file, 'r', encoding='utf-8') as f:
                    initial_content = f.read().strip()
                    text_area.insert(tk.END, initial_content)
            except Exception as e:
                messagebox.showerror("错误", f"无法读取保留词文件: {e}")
        
        def save_words(quiet=False):
            content = text_area.get(1.0, tk.END).strip()
            try:
                os.makedirs(os.path.dirname(reserved_file), exist_ok=True)
                with open(reserved_file, 'w', encoding='utf-8') as f:
                    f.write(content)
                
                # 如果引擎正在运行，尝试动态发送新词
                if self.process and self.process.poll() is None:
                    new_words = content.split('\n')
                    old_words = initial_content.split('\n')
                    added_words = [w.strip() for w in new_words if w.strip() and w.strip() not in old_words]
                    for w in added_words:
                        try:
                            self.process.stdin.write(f"\n[ACTION] ADD_RESERVED WORD={w}\n")
                            self.process.stdin.flush()
                        except:
                            pass
                
                if not quiet:
                    messagebox.showinfo("成功", "保留词库已更新！" + ("已同步到当前运行的引擎。" if self.process and self.process.poll() is None else "下次分析时生效。"))
                return True
            except Exception as e:
                messagebox.showerror("错误", f"保存失败: {e}")
                return False
            
        def on_closing():
            current_content = text_area.get(1.0, tk.END).strip()
            if current_content != initial_content:
                answer = messagebox.askyesnocancel("保存修改", "保留词列表已修改，是否保存？")
                if answer is True:
                    if save_words(quiet=True):
                        win.destroy()
                elif answer is False:
                    win.destroy()
                else:
                    pass
            else:
                win.destroy()

        win.protocol("WM_DELETE_WINDOW", on_closing)
            
        btn_frame = ttk.Frame(win)
        btn_frame.pack(pady=10)
        
        btn_save = ttk.Button(btn_frame, text="保存修改", command=lambda: save_words())
        btn_save.pack(side=tk.LEFT, padx=5)
        
        btn_close = ttk.Button(btn_frame, text="关闭", command=on_closing)
        btn_close.pack(side=tk.LEFT, padx=5)

    def show_trend(self):
        if not self.process or self.process.poll() is not None:
            messagebox.showwarning("警告", "分析引擎未运行。请先点击 '开始分析'。")
            return
            
        word = simpledialog.askstring("趋势分析", "请输入要分析的关键词：")
        if word:
            self.is_collecting_multi = False
            # 使用用户设置的滑动窗口大小作为统计间隔
            interval = self.window_size.get()
            cmd = f"\n[ACTION] GET_TREND WORD={word} INTERVAL={interval}\n"
            try:
                self.process.stdin.write(cmd)
                self.process.stdin.flush()
            except Exception as e:
                self.append_colored_text(f"发送趋势查询指令失败: {e}\n", "error")

    def show_topk_trend(self):
        if not self.process or self.process.poll() is not None:
            messagebox.showwarning("警告", "分析引擎未运行。请先点击 '开始分析'。")
            return
            
        k = simpledialog.askinteger("阶段性 Top-K 趋势分析", "请输入 K 值 (找出每个阶段的热词并对比)：", initialvalue=3, minvalue=1, maxvalue=10)
        if k:
            # 使用用户设置的滑动窗口大小作为统计间隔
            interval = self.window_size.get()
            cmd = f"\n[ACTION] GET_TOPK_TREND K={k} INTERVAL={interval}\n"
            try:
                self.process.stdin.write(cmd)
                self.process.stdin.flush()
            except Exception as e:
                self.append_colored_text(f"发送 Top-K 趋势查询指令失败: {e}\n", "error")

    def handle_trend_data(self, line):
        try:
            # [TREND_DATA] WORD=xxx DATA=t1:c1,t2:c2...
            content = line.replace("[TREND_DATA]", "").strip()
            parts = content.split(" DATA=")
            word = parts[0].replace("WORD=", "")
            data_str = parts[1]
            
            points = []
            for item in data_str.split(","):
                if ":" in item:
                    t, c = item.split(":")
                    points.append((int(t), int(c)))
            
            if self.is_collecting_multi:
                self.multi_trend_data[word] = points
            else:
                if points:
                    self.root.after(0, lambda: self.draw_trend_window(word, points))
                else:
                    self.append_colored_text(f">>> 词语 '{word}' 在历史记录中未出现。\n", "warning")
        except Exception as e:
            print(f"解析趋势数据失败: {e}")

    def draw_trend_window(self, word, points):
        if HAS_MATPLOTLIB:
            self.draw_matplotlib_trend(f"关键词 '{word}'", {word: points})
        else:
            self.draw_canvas_trend(word, points)

    def draw_multi_trend_window(self):
        if not self.multi_trend_data:
            self.append_colored_text(">>> 未获取到任何热词趋势数据。\n", "warning")
            return
        
        if HAS_MATPLOTLIB:
            self.draw_matplotlib_trend("Top-K 热词对比", self.multi_trend_data)
        else:
            # Canvas 模式暂不支持多曲线，取第一个
            first_word = list(self.multi_trend_data.keys())[0]
            self.draw_canvas_trend(first_word, self.multi_trend_data[first_word])

    def draw_matplotlib_trend(self, title, word_data_map):
        win = tk.Toplevel(self.root)
        win.title(title)
        win.geometry("1100x750")
        
        # 核心修复：强制刷新布局并等待窗口映射
        win.update_idletasks()
        
        def do_draw():
            # 创建 Matplotlib 图表
            fig, ax = plt.subplots(figsize=(11, 7), dpi=100)
            
            # 设置中文字体
            font = FontProperties(family='Microsoft YaHei', size=10)
            title_font = FontProperties(family='Microsoft YaHei', size=14, weight='bold')

            # 格式化时间显示
            def format_time(s):
                hh = s // 3600
                mm = (s % 3600) // 60
                ss = s % 60
                return f"{hh:02d}:{mm:02d}:{ss:02d}"

            all_times = set()
            # 使用 tab20 调色盘确保更多不同颜色
            colormap = plt.cm.get_cmap('tab20')
            for i, (word, points) in enumerate(word_data_map.items()):
                times = [p[0] for p in points]
                counts = [p[1] for p in points]
                color = colormap(i % 20)
                ax.plot(times, counts, marker='o', linestyle='-', label=word, linewidth=2, markersize=4, color=color)
                all_times.update(times)
            
            sorted_times = sorted(list(all_times))
            time_labels = [format_time(t) for t in sorted_times]

            interval = self.window_size.get()
            ax.set_title(f"{title} 频率变化趋势 (统计间隔: {interval}秒)", fontproperties=title_font)
            ax.set_xlabel("时间 (HH:MM:SS)", fontproperties=font)
            ax.set_ylabel("出现次数", fontproperties=font)
            
            if sorted_times:
                if len(sorted_times) > 10:
                    step = len(sorted_times) // 10
                    ax.set_xticks(sorted_times[::step])
                    ax.set_xticklabels(time_labels[::step], rotation=45)
                else:
                    ax.set_xticks(sorted_times)
                    ax.set_xticklabels(time_labels, rotation=45)
                
            ax.grid(True, linestyle='--', alpha=0.6)
            ax.legend(prop=font, loc='upper right', bbox_to_anchor=(1.15, 1))
            
            plt.tight_layout()

            # 将图表嵌入 Tkinter
            canvas = FigureCanvasTkAgg(fig, master=win)
            canvas_widget = canvas.get_tk_widget()
            canvas_widget.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
            
            canvas.draw()

            # 窗口关闭时清理内存
            def on_win_close():
                plt.close(fig)
                win.destroy()
            win.protocol("WM_DELETE_WINDOW", on_win_close)

        # 延迟 200ms 绘图，确保窗口 geometry 已经生效
        win.after(200, do_draw)

    def draw_canvas_trend(self, word, points):
        win = tk.Toplevel(self.root)
        win.title(f"趋势分析 - {word} (Canvas模式)")
        win.geometry("1000x600")
        
        # 标题
        interval = self.window_size.get()
        lbl = ttk.Label(win, text=f"关键词 '{word}' 的频率变化趋势 (每{interval}秒统计)", font=("Microsoft YaHei", 12, "bold"))
        lbl.pack(pady=10)

        canvas = tk.Canvas(win, bg="white", highlightthickness=0)
        canvas.pack(fill=tk.BOTH, expand=True, padx=40, pady=20)
        
        def draw_chart(event=None):
            canvas.delete("all")
            w = canvas.winfo_width()
            h = canvas.winfo_height()
            if w < 100 or h < 100: return

            padding = 40
            chart_w = w - 2 * padding
            chart_h = h - 2 * padding
            
            max_c = max(p[1] for p in points) if points else 1
            if max_c == 0: max_c = 1
            
            min_t = points[0][0]
            max_t = points[-1][0]
            dt = max_t - min_t if max_t > min_t else 1
            
            # 画坐标轴
            canvas.create_line(padding, padding, padding, h - padding, width=2) # Y
            canvas.create_line(padding, h - padding, w - padding, h - padding, width=2) # X
            
            # 画点和线
            prev_x, prev_y = None, None
            for t, c in points:
                x = padding + (t - min_t) / dt * chart_w
                y = h - padding - (c / max_c) * chart_h
                
                # 画小圆点
                canvas.create_oval(x-3, y-3, x+3, y+3, fill="#569cd6", outline="#569cd6")
                
                if prev_x is not None:
                    canvas.create_line(prev_x, prev_y, x, y, fill="#569cd6", width=2)
                
                prev_x, prev_y = x, y

            # 标注最大值
            canvas.create_text(padding - 5, padding, text=str(max_c), anchor=tk.E)
            canvas.create_text(padding - 5, h - padding, text="0", anchor=tk.E)
            
            # 标注起止时间
            def format_time(s):
                hh = s // 3600
                mm = (s % 3600) // 60
                ss = s % 60
                return f"{hh:02d}:{mm:02d}:{ss:02d}"
                
            canvas.create_text(padding, h - padding + 15, text=format_time(min_t), anchor=tk.N)
            canvas.create_text(w - padding, h - padding + 15, text=format_time(max_t), anchor=tk.N)

        win.bind("<Configure>", draw_chart)
        win.after(100, draw_chart)

if __name__ == "__main__":
    root = tk.Tk()
    # 设置全局字体
    default_font = ("Microsoft YaHei", 9)
    root.option_add("*Font", default_font)
    app = HotWordGUI(root)
    root.mainloop()

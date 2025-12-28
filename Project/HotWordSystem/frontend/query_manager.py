import tkinter as tk
from tkinter import messagebox

class QueryManager:
    """
    这个类用来处理历史查询
    """
    def __init__(self, app):
        self.app = app

    def run_history_query(self):
        # 执行历史范围查询
        if not self.app.engine.is_running:
            messagebox.showwarning("警告", "请先点击 '开始分析'。")
            return
        
        def safe_get(var, default=0):
            try: return var.get()
            except: return default

        # 获取界面上的时间设置
        sh, sm, ss = safe_get(self.app.start_h), safe_get(self.app.start_m), safe_get(self.app.start_s)
        eh, em, es = safe_get(self.app.end_h), safe_get(self.app.end_m), safe_get(self.app.end_s)

        # 检查时间对不对
        if any(v < 0 for v in [sh, sm, ss, eh, em, es]) or any(v >= 60 for v in [sm, ss, em, es]):
            messagebox.showerror("错误", "时间格式不对。")
            return

        start = f"{sh:02d}:{sm:02d}:{ss:02d}"
        end = f"{eh:02d}:{em:02d}:{es:02d}"
        k = safe_get(self.app.k_value, 5)
        
        # 发送指令给后台
        cmd = f"\n[ACTION] HISTORY_QUERY START=[{start}] END=[{end}] K={k}\n"
        if not self.app.engine.send_command(cmd):
            self.app.append_colored_text("发送查询指令失败\n", "error")

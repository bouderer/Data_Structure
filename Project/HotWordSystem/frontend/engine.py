import subprocess
import threading
import os

class EngineClient:
    """
    这个类用来和后台的 C++ 程序说话
    """
    def __init__(self, exe_path, on_output_callback):
        self.exe_path = exe_path
        self.on_output_callback = on_output_callback
        self.process = None
        self.is_running = False

    def start(self, input_text):
        # 启动后台程序
        if self.is_running:
            return
        
        self.is_running = True
        thread = threading.Thread(target=self._run, args=(input_text,), daemon=True)
        thread.start()

    def _run(self, input_text):
        # 运行后台程序的具体逻辑
        try:
            env = os.environ.copy()
            env["PYTHONIOENCODING"] = "utf-8"
            
            # 设置工作目录为 HotWordSystem，以便后端能找到 ../dict/ 目录
            work_dir = os.path.dirname(os.path.dirname(self.exe_path))
            
            self.process = subprocess.Popen(
                [self.exe_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                encoding='utf-8',
                errors='replace',
                bufsize=1,
                env=env,
                cwd=work_dir
            )

            # 把输入的内容传给后台
            def feed_input():
                try:
                    self.process.stdin.write(input_text)
                    self.process.stdin.flush()
                except:
                    pass
            
            feeder = threading.Thread(target=feed_input, daemon=True)
            feeder.start()

            # 循环读取后台传回来的每一行字
            for line in self.process.stdout:
                if not self.is_running: break
                self.on_output_callback(line)
            
            self.process.wait()
        except Exception as e:
            self.on_output_callback(f"后台程序出错了: {e}\n")
        finally:
            self.is_running = False

    def send_command(self, cmd):
        # 给后台发一个指令
        if self.process and self.process.poll() is None:
            try:
                self.process.stdin.write(cmd)
                self.process.stdin.flush()
                return True
            except:
                return False
        return False

    def stop(self):
        # 停止后台程序
        self.is_running = False
        if self.process:
            try:
                self.process.stdin.write("\n[ACTION] EXIT\n")
                self.process.stdin.flush()
            except:
                pass
            self.process.terminate()
            self.process = None

    def get_pid(self):
        # 获取后台程序的进程 ID
        return self.process.pid if self.process else None

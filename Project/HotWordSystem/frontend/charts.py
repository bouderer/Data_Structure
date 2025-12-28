import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.font_manager import FontProperties
import tkinter as tk

def format_time(s):
    # 把秒数变成 00:00:00 这种格式
    hh = s // 3600
    mm = (s % 3600) // 60
    ss = s % 60
    return f"{hh:02d}:{mm:02d}:{ss:02d}"

def create_trend_window(
    parent, 
    title, 
    data_dict, 
    is_multi=False
):
    # 创建一个画趋势图的窗口
    win = tk.Toplevel(parent)
    win.title(title)
    win.geometry("1000x700")
    
    # 设置字体，防止中文乱码
    font = FontProperties(family='Microsoft YaHei', size=10)
    title_font = FontProperties(family='Microsoft YaHei', size=12, weight='bold')
    
    fig, ax = plt.subplots(figsize=(10, 6), dpi=100)
    
    all_x = []
    if is_multi:
        # 画多个词的对比图
        try:
            colormap = plt.colormaps.get_cmap('tab20')
        except:
            colormap = plt.cm.get_cmap('tab20')
            
        for i, (word, trend) in enumerate(data_dict.items()):
            color = colormap(i % 20)
            ax.plot(trend['x'], trend['y'], marker='o', label=word, color=color, linewidth=2)
            all_x.extend(trend['x'])
        ax.legend(prop=font, loc='upper left', bbox_to_anchor=(1, 1))
    else:
        # 画单个词的趋势图
        word = list(data_dict.keys())[0]
        trend = data_dict[word]
        ax.plot(trend['x'], trend['y'], marker='o', color='#3498db', linewidth=2, label=word)
        all_x.extend(trend['x'])
        ax.set_title(f"词语趋势: {word}", fontproperties=title_font)

    # 设置横坐标的时间显示
    if all_x:
        unique_x = sorted(list(set(all_x)))
        if len(unique_x) > 10:
            step = len(unique_x) // 10
            display_x = unique_x[::step]
        else:
            display_x = unique_x
        
        ax.set_xticks(display_x)
        ax.set_xticklabels([format_time(x) for x in display_x], rotation=45, fontproperties=font)

    ax.set_xlabel("时间", fontproperties=font)
    ax.set_ylabel("次数", fontproperties=font)
    ax.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()

    canvas = FigureCanvasTkAgg(fig, master=win)
    canvas.draw()
    canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
    
    def on_close():
        plt.close(fig)
        win.destroy()
    win.protocol("WM_DELETE_WINDOW", on_close)
    win.update()

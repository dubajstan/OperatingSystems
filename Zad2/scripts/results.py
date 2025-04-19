import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import itertools
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as tk

def configure_style():
    sns.set_style("whitegrid")
    sns.set_palette("pastel")
    plt.rcParams['font.family'] = 'DejaVu Sans'
    plt.rcParams['font.size'] = 10
    plt.rcParams['axes.titlesize'] = 12
    plt.rcParams['axes.labelsize'] = 10
    plt.rcParams['grid.alpha'] = 0.3
    plt.rcParams['grid.linestyle'] = '--'

def createMainWindow(title, size="1400x1080"):
    root = tk.Tk()
    root.title(title)
    root.geometry(size)

    def on_closing():
        plt.close('all')
        root.destroy()    
    root.protocol("WM_DELETE_WINDOW", on_closing)

    root.grid_rowconfigure(0, weight=1)
    root.grid_columnconfigure(0, weight=1)

    main_frame = tk.Frame(root)
    main_frame.grid(row=0, column=0, sticky="nsew")

    canvas = tk.Canvas(main_frame)
    canvas.grid(row=0, column=0, sticky="nsew")

    scrollbar = tk.Scrollbar(main_frame, orient=tk.VERTICAL, command=canvas.yview)
    scrollbar.grid(row=0, column=1, sticky='ns')

    main_frame.grid_rowconfigure(0, weight=1)
    main_frame.grid_columnconfigure(0, weight=1)

    canvas.configure(yscrollcommand=scrollbar.set)
    canvas.bind('<Configure>', lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

    content_frame = tk.Frame(canvas)
    canvas.create_window((0, 0), window=content_frame, anchor="nw")

    return root, content_frame

algorithms = [
    'fcfs',
    'sstf',
    'scan',
    'cscan',
    'sstf_edf',
    'sstf_fd_scan'
]

def loadExampleData():
    test_data = []
    for alg in algorithms:
        try:
            df = pd.read_csv(f'data/testExample_{alg}.csv', header=0, sep=',', names=[
                'ID',
                'ArrivalTime',
                'Position',
                'IsRealTime',
                'Deadline',
                'WaitTime',
                'IsStarved',
                'ServiceTime'
            ])
            
            df['ID'] = df['ID'].astype(int)
            df['ArrivalTime'] = df['ArrivalTime'].astype(int)
            df['Position'] = df['Position'].astype(int)
            df['Deadline'] = df['Deadline'].astype(int)
            df['WaitTime'] = df['WaitTime'].astype(int)
            df['ServiceTime'] = df['ServiceTime'].astype(int)

            df['IsRealTime'] = df['IsRealTime'].apply(lambda x: True if str(x).lower() == 'true' else False)
            df['IsStarved'] = df['IsStarved'].apply(lambda x: True if str(x).lower() == 'true' else False)
            
            test_data.append({
                'algorithm': alg,
                'data': df
            })
        except FileNotFoundError:
            continue
    return test_data

def loadTestData():
    test_data = []
    modes = list(itertools.product(['uniform', 'normal'], repeat=2))
    
    for group in ['test', 'testSCAN']:
        for alg in algorithms:
            for mode in modes:
                try:
                    df = pd.read_csv(f'data/{group}_{mode[0]}_{mode[1]}_{alg}.csv', header=0, sep=',', names=[
                        'ID',
                        'ArrivalTime',
                        'Position',
                        'IsRealTime',
                        'Deadline',
                        'WaitTime',
                        'IsStarved',
                        'ServiceTime'
                    ])
                
                    df['ID'] = df['ID'].astype(int)
                    df['ArrivalTime'] = df['ArrivalTime'].astype(int)
                    df['Position'] = df['Position'].astype(int)
                    df['Deadline'] = df['Deadline'].astype(int)
                    df['WaitTime'] = df['WaitTime'].astype(int)
                    df['ServiceTime'] = df['ServiceTime'].astype(int)

                    df['IsRealTime'] = df['IsRealTime'].apply(lambda x: True if str(x).lower() == 'true' else False)
                    df['IsStarved'] = df['IsStarved'].apply(lambda x: True if str(x).lower() == 'true' else False)
                
                    test_data.append({
                        'position_mode': mode[0],
                        'time_mode': mode[1],
                        'algorithm': alg,
                        'data': df,
                        'avg_wait_time': sum(df['WaitTime']) / len(df['WaitTime']),
                        'starvation_count': ((df['IsRealTime']) & (df['IsStarved'])).sum(),
                        'is_SCAN_case': group == 'testSCAN'
                    })
                except FileNotFoundError:
                    continue
    
    return test_data

def showExampleData(data, parent_frame):
    fig, axes = plt.subplots(3, 2, figsize=(16, 14))
    axes = axes.flatten()
    
    for i in range(len(data)):
        d = data[i]
        if d is not None:
            axes[i].set_title(d['algorithm'])
            df = d['data']
            colors = []
            for _, row in df.iterrows():
                if not row['IsRealTime']:
                    colors.append('black')
                elif row['IsStarved']:
                    colors.append('red')
                else:
                    colors.append('green')
            
            axes[i].plot(df['Position'], df['ArrivalTime'], color='black', alpha=0.5, linestyle='-', label='Head Movement')
            axes[i].scatter(df['Position'], df['ArrivalTime'], c=colors, s=70)
            axes[i].invert_yaxis()
            axes[i].set_xlabel('Position')
            axes[i].set_ylabel('Arrival Time')
            axes[i].grid(True)          

    fig.tight_layout()
    canvas = FigureCanvasTkAgg(fig, master=parent_frame)
    canvas.draw()
    canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
def showStarvationComparison(data, parent_frame):
    fig, ax = plt.subplots(figsize=(6, 4))

    algs = ['sstf_edf', 'sstf_fd_scan']
    counts = []
    for alg in algs:
        total = 0
        for d in data:
            if d['algorithm'] == alg:
                total += d['starvation_count']
        counts.append(total)

    sns.barplot(x=algs, y=counts, ax=ax, palette=['green', 'blue'])
    ax.set_title("Liczba zagłodzonych zadań")
    ax.set_ylabel("Zagłodzone zadania")
    ax.set_xlabel("Algorytm")

    fig.tight_layout()
    canvas = FigureCanvasTkAgg(fig, master=parent_frame)
    canvas.draw()
    canvas.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
def showResults(data, parent_frame, is_SCAN_case=False):
    grouped = {}
    for d in data:
        if d['is_SCAN_case'] != is_SCAN_case:
            continue
        key = (d['position_mode'], d['time_mode'])
        if key not in grouped:
            grouped[key] = []
        grouped[key].append(d)
        
    if not grouped:
        return
        
    title = "Pesymistyczny przypadek dla SCAN" if is_SCAN_case else "WYNIKI ALGORYTMÓW"
    tk.Label(parent_frame, text=title, font=('Helvetica', 14, 'bold')).pack(pady=10)
    
    for (pos_mode, time_mode), group_data in grouped.items():
        frame = tk.LabelFrame(parent_frame, text=f'Tryb: {pos_mode} \ {time_mode}', font=("Helvetica", 12, "bold"))
        frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        fig1, axes1 = plt.subplots(3, 2, figsize=(16, 30))
        axes1 = axes1.flatten()
        
        for i, d in enumerate(group_data):
            ax = axes1[i]
            df = d['data']
            ax.set_title(d['algorithm'])
            ax.plot(df['Position'], df['ServiceTime'], color='black', alpha=0.5)
            
            colors = []
            for _, row in df.iterrows():
                if not row['IsRealTime']:
                    colors.append('blue')
                elif row['IsStarved']:
                    colors.append('red')
                else:
                    colors.append('green')
            
            ax.scatter(df['Position'], df['ServiceTime'], c=colors, s=70, alpha=0.5)
            ax.invert_yaxis()
            ax.set_xlabel('Position')
            ax.set_ylabel('Service Time')
            ax.grid(True)
        
        fig1.tight_layout()
        canvas1 = FigureCanvasTkAgg(fig1, master=frame)
        canvas1.draw()
        canvas1.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        fig2, axes2 = plt.subplots(3, 2, figsize=(16, 14))
        axes2 = axes2.flatten()
        
        for i, d in enumerate(group_data):
            ax = axes2[i]
            df = d['data']
            sns.kdeplot(df['WaitTime'], ax=ax, fill=True)
            ax.set_title(f"{d['algorithm']} - Wait Time Density")
            ax.set_xlabel("Wait Time")
            ax.set_ylabel("Density")
            
        fig2.tight_layout()
        canvas2 = FigureCanvasTkAgg(fig2, master=frame)
        canvas2.draw()
        canvas2.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        fig3, ax3 = plt.subplots(figsize=(8, 4))
        algs = [d['algorithm'] for d in group_data]
        avgs = [d['avg_wait_time'] for d in group_data]
        sns.barplot(x=algs, y=avgs, ax=ax3, palette='pastel')
        ax3.set_title("Średni czas oczekiwania")
        ax3.set_yscale('log')
        ax3.set_ylabel("Avg Wait Time")
        ax3.set_xlabel("Algorytm")

        fig3.tight_layout()
        canvas3 = FigureCanvasTkAgg(fig3, master=frame)
        canvas3.draw()
        canvas3.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        showStarvationComparison(group_data, frame)

def main():
    configure_style()
    root, content_frame = createMainWindow("Analiza algorytmów kolejkowania dysku HDD")
    
    example_data = loadExampleData()
    test_data = loadTestData()
    
    if not example_data and not test_data:
        tk.Label(content_frame, text="Brak danych do wyświetlenia").pack()
    else:
        if example_data:
            tk.Label(content_frame, text="DANE POKAZOWE - ruch głowicy", font=('Helvetica', 14, 'bold')).pack(pady=10)
            showExampleData(example_data, content_frame)
        
        if test_data:
            showResults(test_data, content_frame, is_SCAN_case=False)
            showResults(test_data, content_frame, is_SCAN_case=True)
    
    root.mainloop()

if __name__ == '__main__':
    main()
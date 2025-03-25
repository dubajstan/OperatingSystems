import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as tk
from tkinter import ttk

# Konfiguracja stylu
sns.set_style("whitegrid")

def load_algorithm_results():
    tryby = [("gaussian", "linear"), ("gaussian", "inverse_linear"),
             ("linear", "gaussian"), ("linear", "inverse_linear"),
             ("inverse_linear", "gaussian"), ("inverse_linear", "linear")]
    
    results = []
    for tryb in tryby:
        try:
            with open(f"graphs/wyniki_{tryb[0]}_{tryb[1]}.txt", 'r') as f:
                data = list(map(float, f.read().split()))
                results.append({
                    'Tryb': f"{tryb[0]}_{tryb[1]}",
                    'Algorytm': ['FCFS', 'SJF', 'SJF wyw.', 'RR'],
                    'WT': [data[0], data[2], data[4], data[6]],
                    'TAT': [data[1], data[3], data[5], data[7]]
                })
        except FileNotFoundError:
            print(f"Plik graphs/wyniki_{tryb[0]}_{tryb[1]}.txt nie istnieje")
            continue
    return results

def on_closing():
    plt.close('all')
    root.destroy()

# Główne okno
root = tk.Tk()
root.title("Porównanie algorytmów - podział na tryby")
root.geometry("1200x900")
root.protocol("WM_DELETE_WINDOW", on_closing)

# Ramka z przewijaniem
main_frame = tk.Frame(root)
main_frame.pack(fill=tk.BOTH, expand=True)

canvas = tk.Canvas(main_frame)
canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

scrollbar = ttk.Scrollbar(main_frame, orient=tk.VERTICAL, command=canvas.yview)
scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

canvas.configure(yscrollcommand=scrollbar.set)
canvas.bind('<Configure>', lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

# Ramka na wykresy
plot_frame = tk.Frame(canvas)
canvas.create_window((0, 0), window=plot_frame, anchor="nw")

# Wczytanie danych
results = load_algorithm_results()

if not results:
    tk.Label(plot_frame, text="Brak danych do wyświetlenia").pack()
else:
    figures = []  # Przechowujemy referencje do figur
    
    for result in results:
        df = pd.DataFrame({
            'Algorytm': result['Algorytm'],
            'WT': result['WT'],
            'TAT': result['TAT']
        })
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))
        fig.suptitle(f"Tryb: {result['Tryb']}", fontsize=14)
        figures.append(fig)  # Zapamiętujemy figurę
        
        sns.barplot(data=df, x='Algorytm', y='WT', ax=ax1, palette="Blues_d")
        ax1.set_title("Średni czas oczekiwania (WT)")
        ax1.set_ylabel("Czas")
        ax1.grid(True, linestyle='--', alpha=0.7)
        
        sns.barplot(data=df, x='Algorytm', y='TAT', ax=ax2, palette="Reds_d")
        ax2.set_title("Średni czas realizacji (TAT)")
        ax2.set_ylabel("Czas")
        ax2.grid(True, linestyle='--', alpha=0.7)
        
        plt.tight_layout()
        
        canvas_fig = FigureCanvasTkAgg(fig, master=plot_frame)
        canvas_fig.draw()
        canvas_fig.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        ttk.Separator(plot_frame, orient='horizontal').pack(fill='x', padx=10, pady=5)

root.mainloop()
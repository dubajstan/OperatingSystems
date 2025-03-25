import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import itertools
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter as tk
from tkinter import ttk

# Konfiguracja stylu Seaborn
sns.set_style("whitegrid")
sns.set_palette("pastel")

# Inicjalizacja okna Tkinter
root = tk.Tk()
root.title("Wizualizacja danych testowych")

# Ramka z przewijaniem
main_frame = tk.Frame(root)
main_frame.pack(fill=tk.BOTH, expand=1)

canvas = tk.Canvas(main_frame)
canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=1)

scrollbar = ttk.Scrollbar(main_frame, orient=tk.VERTICAL, command=canvas.yview)
scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

canvas.configure(yscrollcommand=scrollbar.set)
canvas.bind('<Configure>', lambda e: canvas.configure(scrollregion=canvas.bbox("all")))

# Ramka na wykresy
plot_frame = tk.Frame(canvas)
canvas.create_window((0, 0), window=plot_frame, anchor="nw")

nr_testow = 5
tryby = list(itertools.permutations(["gaussian", "linear", "inverse_linear"], 2))

# Funkcja do wczytywania danych
def load_data():
    dane_testowe = []
    nazwy = []
    for i in range(1, nr_testow + 1):
        for tryb in tryby:
            try:
                df = pd.read_csv(f"graphs/test_{i}_{tryb[0]}_{tryb[1]}.txt", 
                                header=None, sep='\s+', 
                                names=['PID', 'Arrival', 'Burst', 'Priority', 'Col5', 'Col6', 'Col7'])
                dane_testowe.append(df)
                nazwy.append(f"Test {i}: {tryb[0]}_{tryb[1]}")
            except FileNotFoundError:
                print(f"Plik graphs/test_{i}_{tryb[0]}_{tryb[1]}.txt nie istnieje")
                continue
    return dane_testowe, nazwy

# Funkcja tworząca wykresy
def create_plots():
    dane_testowe, nazwy = load_data()
    
    if not dane_testowe:
        tk.Label(plot_frame, text="Brak danych do wyświetlenia").pack()
        return
    
    for i, (df, nazwa) in enumerate(zip(dane_testowe, nazwy)):
        # Tworzenie figury dla każdego testu
        fig = plt.Figure(figsize=(14, 6), dpi=100)
        
        # Wykres Arrival Time
        ax1 = fig.add_subplot(121)
        sns.histplot(data=df, x='Arrival', ax=ax1, kde=True, 
                     color='royalblue', bins=20, edgecolor='white')
        ax1.set_title(f"Rozkład czasu przybycia\n{nazwa}", fontsize=12)
        ax1.set_xlabel("Czas przybycia", fontsize=10)
        ax1.set_ylabel("Liczba procesów", fontsize=10)
        
        # Wykres Burst Time
        ax2 = fig.add_subplot(122)
        sns.histplot(data=df, x='Burst', ax=ax2, kde=True, 
                     color='salmon', bins=20, edgecolor='white')
        ax2.set_title(f"Rozkład czasu wykonania\n{nazwa}", fontsize=12)
        ax2.set_xlabel("Czas wykonania", fontsize=10)
        ax2.set_ylabel("Liczba procesów", fontsize=10)
        
        # Dodanie figury do interfejsu
        canvas_fig = FigureCanvasTkAgg(fig, master=plot_frame)
        canvas_fig.draw()
        canvas_fig.get_tk_widget().pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Separator między wykresami
        ttk.Separator(plot_frame, orient='horizontal').pack(fill='x', padx=5, pady=5)

# Przycisk do odświeżania danych
refresh_btn = tk.Button(root, text="Odśwież dane", command=create_plots)
refresh_btn.pack(pady=10)

create_plots()
root.mainloop()
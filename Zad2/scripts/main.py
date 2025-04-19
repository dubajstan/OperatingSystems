import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('data/requests.csv')

df['IsRealTime'] = df['IsRealTime'].astype(bool)
df['IsStarved'] = df['IsStarved'].astype(bool)

plt.figure(figsize=(8,6))

colors = []
for _, row in df.iterrows():
    if not row['IsRealTime']:
        colors.append('black')
    elif row['IsStarved']:
        colors.append('red')
    else:
        colors.append('green')

plt.plot(df['Position'], df['ArrivalTime'], color='black', alpha=0.5, linestyle='-', label='Head movement')
plt.scatter(df['Position'], df['ArrivalTime'], c=colors, s=80)

plt.gca().invert_yaxis()
plt.xlabel('Position')
plt.ylabel('Step')
plt.title('Wykres ruchu głowicy')
plt.grid(True)
plt.tight_layout()

legend_elements = [
    plt.Line2D([0], [0], marker='o', color='w', label='Not RT', markerfacecolor='black', markersize=10),
    plt.Line2D([0], [0], marker='o', color='w', label='RT (not starved)', markerfacecolor='green', markersize=10),
    plt.Line2D([0], [0], marker='o', color='w', label='RT (starved)', markerfacecolor='red', markersize=10),
    plt.Line2D([0], [0], color='black', alpha=0.5, label='Head movement')
]
plt.legend(handles=legend_elements)

plt.savefig('graphs/graph_test_1.png')
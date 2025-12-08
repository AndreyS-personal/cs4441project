import pandas as pd
import matplotlib.pyplot as plt

# Load CSV
csv_file = "results.csv"  # replace with your CSV filename
df = pd.read_csv(csv_file)

# Function to calculate speedup
def calculate_speedup(group):
    single_thread_throughput = group[group['threads'] == 1]['throughput'].values[0]
    group['speedup'] = group['throughput'] / single_thread_throughput
    return group

# Apply speedup calculation per workload
df = df.groupby('workload').apply(calculate_speedup)

# Plot throughput vs threads for each workload
plt.figure(figsize=(10, 6))
for workload, group in df.groupby('workload'):
    plt.plot(group['threads'], group['throughput'], marker='o', label=f"{workload} throughput")

plt.xscale('log', base=2)
plt.xlabel("Threads")
plt.ylabel("Throughput")
plt.title("Throughput vs Threads")
plt.legend()
plt.grid(True, which="both", linestyle="--")
plt.tight_layout()
plt.show()

# Plot speedup vs threads for each workload
plt.figure(figsize=(10, 6))
for workload, group in df.groupby('workload'):
    plt.plot(group['threads'], group['speedup'], marker='o', label=f"{workload} speedup")

plt.xscale('log', base=2)
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs Threads")
plt.axhline(y=1, color='gray', linestyle='--')
plt.legend()
plt.grid(True, which="both", linestyle="--")
plt.tight_layout()
plt.show()

# Optional: print the dataframe with speedup for inspection
print(df)

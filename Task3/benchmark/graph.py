import json
import matplotlib.pyplot as plt

with open("lvn_results.json", "r") as f:
    data = json.load(f)

files = list(data.keys())

#line counts
original_lines = [data[f]["original_lines"] for f in files]
optimized_lines = [data[f]["optimized_lines"] for f in files]

#  dynamic instructions (filtering nulls)
dyn_files = []
original_dyn = []
optimized_dyn = []

for f in files:
    og = data[f]["original_dyn_inst"]
    opt = data[f]["optimized_dyn_inst"]
    if og is not None and opt is not None:
        dyn_files.append(f)
        original_dyn.append(og)
        optimized_dyn.append(opt)

# --- Plot 1: Lines ---
x = range(len(files))
plt.figure(figsize=(10, 5))
plt.bar(x, original_lines, width=0.4, label="Original Lines", align="center")
plt.bar([i + 0.4 for i in x], optimized_lines, width=0.4, label="Optimized Lines", align="center")
plt.xticks([i + 0.2 for i in x], files, rotation=45, ha="right")
plt.ylabel("Lines")
plt.title("Original vs Optimized Lines")
plt.legend()
plt.tight_layout()
plt.savefig("lines_comparison.png")
plt.show()

# --- Plot 2: Dynamic Instructions ---
x = range(len(dyn_files))
plt.figure(figsize=(10, 5))
plt.bar(x, original_dyn, width=0.4, label="Original Dyn Inst", align="center")
plt.bar([i + 0.4 for i in x], optimized_dyn, width=0.4, label="Optimized Dyn Inst", align="center")
plt.xticks([i + 0.2 for i in x], dyn_files, rotation=45, ha="right")
plt.ylabel("Dynamic Instructions")
plt.title("Original vs Optimized Dynamic Instructions")
plt.legend()
plt.tight_layout()
plt.savefig("dyn_inst_comparison.png")
plt.show()

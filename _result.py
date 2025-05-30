import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

tour_dir = "tour"
output_dir = "summary"
os.makedirs(output_dir, exist_ok=True)

all_data = []

for filename in os.listdir(tour_dir):
    if not filename.endswith(".tour"):
        continue

    tour_path = os.path.join(tour_dir, filename)

    dataset = algo = None
    length = time = None
    dimension = None

    with open(tour_path) as f:
        for line in f:
            line = line.strip()
            if line.startswith("NAME"):
                dataset = line.split(":", 1)[-1].strip()
                dataset = dataset.replace(".tsp", "").replace(".tour", "").replace(".txt", "").replace("./TSPLIB/", "")
            elif line.startswith("DIMENSION"):
                try:
                    dimension = int(line.split(":", 1)[-1].strip())
                except:
                    pass
            elif line.startswith("COMMENT"):
                content = line.split(":", 1)[-1].strip()
                if content.strip().upper() == "OPT":
                    algo = "OPT"
                elif "Algorithm" in content:
                    algo = content.replace("Algorithm", "").strip()
                elif "Length" in content or "length" in content or "Tour length" in content:
                    m = re.search(r"(\d+(?:\.\d+)?)", content)
                    if m:
                        length = float(m.group(1))
                elif "Elapsed time" in content:
                    m = re.search(r"\d+(?:\.\d+)?", content)
                    if m:
                        time = float(m.group())

    if all(v is not None for v in [dataset, algo, length, dimension]):
        all_data.append({
            "dataset": dataset,
            "algorithm": algo,
            "length": length,
            "time": time,  # may be None
            "dimension": dimension
        })

if not all_data:
    print("No valid data found.")
    exit(1)

df = pd.DataFrame(all_data)
df.sort_values(by="dimension", inplace=True)
df["dataset"] = pd.Categorical(df["dataset"], categories=df["dataset"].unique(), ordered=True)

# === 알고리즘 순서 지정: OPT 먼저, 나머지 알파벳 순 ===
algos = sorted(set(row["algorithm"] for row in all_data if row["algorithm"] != "OPT"))
algo_order = ["OPT"] + algos

# # === 그래프 1: 각 dataset별로 length 그래프 ===
# for dataset_name in df["dataset"].unique():
#     sub = df[df["dataset"] == dataset_name].copy()
#     sub["algorithm"] = pd.Categorical(sub["algorithm"], categories=algo_order, ordered=True)

#     plt.figure(figsize=(8, 5))
#     sns.lineplot(data=sub, x="algorithm", y="length", marker="o", linewidth=2)
#     plt.title(f"{dataset_name} - Tour Length")
#     plt.ylabel("Tour Length")
#     plt.xlabel("Algorithm")
#     plt.xticks(rotation=30)
#     plt.tight_layout()
#     plt.savefig(os.path.join(output_dir, f"{dataset_name}_length.png"))
#     plt.close()


# # === 그래프 2: 각 dataset별로 time 그래프 (OPT 제외) ===
# for dataset_name in df["dataset"].unique():
#     sub = df[(df["dataset"] == dataset_name) & (df["algorithm"] != "OPT")].copy()

#     # OPT 외에 시간 데이터가 없는 경우 건너뛰기
#     if sub["time"].isnull().all():
#         continue

#     sub["algorithm"] = pd.Categorical(sub["algorithm"], categories=algo_order, ordered=True)

#     plt.figure(figsize=(8, 5))
#     sns.lineplot(data=sub, x="algorithm", y="time", marker="o", linewidth=2)
#     plt.title(f"{dataset_name} - Execution Time")
#     plt.ylabel("Time (seconds)")
#     plt.xlabel("Algorithm")
#     plt.xticks(rotation=30)
#     plt.tight_layout()
#     plt.savefig(os.path.join(output_dir, f"{dataset_name}_time.png"))
#     plt.close()

from tabulate import tabulate
df["length"] = df["length"].map(lambda x: f"{int(x):d}")
df["time"] = df["time"].map(lambda x: f"{x:.6f}" if pd.notnull(x) else "")
df["dimension"] = df["dimension"].map(lambda x: f"{int(x):d}")
table = tabulate(df, headers='keys', tablefmt='grid', floatfmt=(".0f", ".0f", ".6f", ".0f"), showindex=False)
with open(output_dir + "/summary_table.md", "w") as f:
    f.write(table)

df.to_csv(os.path.join(output_dir, "summary_table.csv"), index=False)

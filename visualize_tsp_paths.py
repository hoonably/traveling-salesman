import matplotlib.pyplot as plt
import os

tsp_dir = "dataset"
tour_dir = "tour"
output_dir = "tour_image"
os.makedirs(output_dir, exist_ok=True)

def load_tsp_coords(tsp_path):
    coords = {}
    with open(tsp_path) as f:
        in_section = False
        for line in f:
            line = line.strip()
            if line == "NODE_COORD_SECTION":
                in_section = True
                continue
            if line == "EOF":
                break
            if in_section:
                parts = line.split()
                if len(parts) == 3:
                    idx = int(parts[0])
                    x, y = map(float, parts[1:])
                    coords[idx] = (x, y)
    return coords

for filename in os.listdir(tour_dir):
    if not filename.endswith(".tour"):
        continue

    tour_path = os.path.join(tour_dir, filename)
    
    if '-' not in filename:
        print(f"[SKIP] No '-' in filename: {filename}")
        continue
    base_name = filename.split("-", 1)[1].split(".")[0]
    
    tsp_path = os.path.join(tsp_dir, base_name + ".tsp")
    if not os.path.exists(tsp_path):
        print(f"[SKIP] No TSP file for: {filename}")
        continue

    coords_dict = load_tsp_coords(tsp_path)
    if not coords_dict:
        print(f"[ERROR] No coordinates found in: {tsp_path}")
        continue

    tour_order = []
    comment_lines = []
    with open(tour_path) as f:
        in_section = False
        for line in f:
            line = line.strip()
            if line == "TOUR_SECTION":
                in_section = True
                continue
            if line == "EOF":
                break
            if not in_section and line.startswith("COMMENT"):
                parts = line.split(":", 1)
                if len(parts) == 2:
                    comment_lines.append(parts[1].strip())
            if in_section:
                try:
                    idx = int(line)
                    tour_order.append(idx)
                except:
                    continue


    coords = [coords_dict[i] for i in tour_order if i in coords_dict]
    if not coords:
        print(f"[SKIP] No matching tour coordinates for: {filename}")
        continue

    coords.append(coords[0])
    xs, ys = zip(*coords)

    n = len(coords)

    # 도시 수에 따라 선과 점 두께 조절
    if n > 30000:
        linewidth = 0.1
        markersize = 0.1
    elif n > 5000:
        linewidth = 0.2
        markersize = 0.3
    elif n > 1000:
        linewidth = 0.5
        markersize = 1.0
    else:
        linewidth = 1
        markersize = 2

    fig, ax = plt.subplots(figsize=(8, 6))
    ax.plot(xs, ys, marker='o', markersize=markersize, linewidth=linewidth)
    ax.axis("equal")
    ax.grid(True)
    ax.set_title(f"TSP Tour: {filename}")

    # 플롯 위치 조정 (위쪽에 여백)
    plt.subplots_adjust(top=0.85)

    # COMMENT 들을 figure 기준으로 위쪽에 출력 (0~1 좌표계 기준)
    for i, comment in enumerate(comment_lines):
        fig.text(0.01, 0.98 - i * 0.03, comment, fontsize=9, ha='left', va='top')

    output_path = os.path.join(output_dir, filename.replace(".tour", ".png"))
    plt.savefig(output_path, dpi=150)
    plt.close()

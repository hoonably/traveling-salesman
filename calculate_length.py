from math import sqrt
from pathlib import Path

def euclidean_distance(p1, p2):
    dx = p1[0] - p2[0]
    dy = p1[1] - p2[1]
    return int(round(sqrt(dx * dx + dy * dy)))  # TSPLIB EUC_2D

def read_tsp_coords(tsp_path):
    coords = {}
    reading = False
    with open(tsp_path) as f:
        for line in f:
            if line.strip() == "NODE_COORD_SECTION":
                reading = True
                continue
            if reading:
                if line.strip() == "EOF":
                    break
                parts = line.strip().split()
                if len(parts) == 3:
                    city_id, x, y = parts
                    coords[int(city_id)] = (float(x), float(y))
    return coords

def read_tour_order(tour_path):
    tour = []
    reading = False
    with open(tour_path) as f:
        for line in f:
            if line.strip() == "TOUR_SECTION":
                reading = True
                continue
            if reading:
                if line.strip() in ("-1", "EOF"):
                    break
                tour.append(int(line.strip()))
    return tour

def compute_tour_length(tsp_path, tour_path):
    coords = read_tsp_coords(tsp_path)
    tour = read_tour_order(tour_path)

    tour.append(tour[0])  # close the tour

    total_length = 0
    for i in range(len(tour) - 1):
        total_length += euclidean_distance(coords[tour[i]], coords[tour[i+1]])
    return total_length

tsp_path = "dataset/a280.tsp"
tour_path = "optimal_tour/a280.opt.tour"

length = compute_tour_length(tsp_path, tour_path)
print("Computed length:", length)

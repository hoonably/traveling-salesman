//! If you want to more than 10K cities, you should use util2.h
#include "util.h"
// #include "util2.h"

#include <random>

// This file generates a random tour for TSP problems.
// It used for 2-opt optimization without any specific algorithm.

// You should put this file in the dataset/ directory
vector<string> files = {
    "weird20.tsp",
    "a280.tsp", 
    "xql662.tsp", 
    "kz9976.tsp", 
    // "mona_lisa100K.tsp"
};
bool use_2opt = true;  // 2-opt 최적화도 측정할지

vector<int> random_tour() {
    vector<int> tour(n);
    for (int i = 0; i < n; ++i) {
        tour[i] = i + 1;
    }

    // 셔플 수행
    random_device rd;
    mt19937 gen(rd());
    shuffle(tour.begin(), tour.end(), gen);

    tour.push_back(tour.front());  // 시작점으로 복귀
    return tour;
}


int main() {
    run("Random", random_tour, files, use_2opt);
    return 0;
}

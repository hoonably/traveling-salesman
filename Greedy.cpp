//! If you want to more than 10K cities, you should use util2.h
#include "util.h"
// #include "util2.h"

// You should put this file in the dataset/ directory
vector<string> files = {
    "weird20.tsp",
    "a280.tsp", 
    "xql662.tsp", 
    // "kz9976.tsp", 
    // "mona_lisa100K.tsp"
};

vector<int> Greedy_tour() {
    vector<int> tour;
    vector<bool> visited(n + 1, false);
    
    tour.reserve(n + 1);

    int cur = 1;  // 시작 도시를 1로 고정
    tour.push_back(cur);
    visited[cur] = true;

    for (int step = 1; step < n; ++step) {
        int next = -1;
        int bestCost = INF;
        for (int j = 1; j <= n; ++j) {
            int d = get_dist(cur,j);
            if (!visited[j] && d < bestCost) {
                bestCost = d;
                next = j;
            }
        }
        visited[next] = true;
        tour.push_back(next);
        cur = next;
    }
    // 시작점으로 돌아오기
    tour.push_back(tour.front());

    return tour;
}


int main() {
    bool use_2opt = true;  // 2-opt 최적화도 측정할지
    run("Greedy", Greedy_tour, files, use_2opt);
    return 0;
}

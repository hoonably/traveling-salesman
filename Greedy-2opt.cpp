#define ALGO "Greedy_2opt"
#include "util.h"

// 전통적인 Greedy(Nearest-Neighbor) TSP 구현
vector<int> greedy() {
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
            if (!visited[j] && dist[cur][j] < bestCost) {
                bestCost = dist[cur][j];
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

// 2-opt optimization
void apply_2_opt(vector<int>& tour) {
    bool improved=true;
    while(improved) {
        improved=false;
        int m=tour.size();
        for(int i=0;i<m-2 && !improved;++i){
            for(int k=i+2;k<m-1;++k){
                int a=tour[i], b=tour[i+1], c=tour[k], d=tour[k+1];
                int before=dist[a][b]+dist[c][d];
                int after=dist[a][c]+dist[b][d];
                if(after<before){ reverse(tour.begin()+i+1, tour.begin()+k+1); improved=true; break; }
            }
        }
    }
}

// Compute the total cost of the tour
int computeCost(vector<int>& tour) {
    int cost = 0;
    for (int i = 0; i < tour.size() - 1; ++i)
        cost += dist[tour[i]][tour[i + 1]];
    return cost;
}

int main() {
    for (const string& filename : filenames) {
        if (!loadTSPFile(filename)){
            cout << "Skipping " << filename << " due to large size: " << n << " cities.\n";
            continue;
        }

        auto start = chrono::high_resolution_clock::now();

        vector<int> tour = greedy();
        apply_2_opt(tour);
        int total_length=computeCost(tour);

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        // file에 tour 결과 저장
        save(filename, tour, total_length, elapsed);
    }
    return 0;
}

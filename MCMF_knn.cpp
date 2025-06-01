//! If you want to more than 10K cities, you should use util2.h
#include "util.h"
// #include "util2.h"

// You should put this file in the dataset/ directory
vector<string> files = {
    "weird20.tsp",
    "a280.tsp", 
    "xql662.tsp", 
    "kz9976.tsp", 
    // "mona_lisa100K.tsp"
};
bool use_2opt = true;  // 2-opt 최적화도 측정할지

#include "MCMF.h"

// Get K-Nearest Neighbors for each city
vector<vector<int>> get_knn(int k) {
    vector<vector<int>> knn(n + 1);
    for (int i = 1; i <= n; ++i) {
        vector<pair<int, int>> distances; // {distance, city}
        for (int j = 1; j <= n; ++j) {
            if (i != j) {
                distances.push_back({get_dist(i,j), j});
            }
        }
        sort(distances.begin(), distances.end());
        for (int j = 0; j < min(k, (int)distances.size()); ++j) {
            knn[i].push_back(distances[j].second);
        }
    }
    return knn;
}

// MCMF tour construction using KNN
vector<int> MCMF_knn_tour(){

        vector<vector<int>> knn = get_knn(K);

        MCMF mcmf;
        mcmf.init(n);
        // 간선 추가
        for (int i = 1; i <= n; ++i) {
            mcmf.addEdge(mcmf.SRC, i, 1, 0);
            mcmf.addEdge(i + mcmf.bias, mcmf.SINK, 1, 0);
            for (int j : knn[i]) {
                mcmf.addEdge(i, j + mcmf.bias, 1, get_dist(i,j));
            }
        }
        auto result = mcmf.run();

        vector<vector<int>> adj(n + 1);
        for (int u = 1; u <= n; ++u) {
            for (const auto& e : mcmf.graph[u]) {
                if (e.cap == 0 && e.to >= mcmf.bias + 1 && e.to <= mcmf.bias + n) {
                    int from = u;
                    int to = e.to - mcmf.bias;
                    adj[from].push_back(to);
                }
            }
        }

        vector<vector<int>> subtours;
        vector<bool> visited(n + 1, false);
        for (int i = 1; i <= n; ++i) {
            if (!visited[i]) {
                vector<int> path;
                int cur = i;
                while (!visited[cur] && !adj[cur].empty()) {
                    visited[cur] = true;
                    path.push_back(cur);
                    cur = adj[cur][0];
                }
                if (!path.empty()) subtours.push_back(path);
            }
        }

        while (subtours.size() > 1) {
            int best_i = -1, best_j = -1, best_cost = INF;
            for (int i = 0; i < subtours.size(); ++i) {
                for (int j = 0; j < subtours.size(); ++j) {
                    if (i == j) continue;
                    int tail = subtours[i].back();
                    int head = subtours[j].front();
                    int cost = get_dist(tail,head);
                    if (cost < best_cost) {
                        best_cost = cost;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
            if (best_i != -1 && best_j != -1) {
                subtours[best_i].insert(subtours[best_i].end(),
                                        subtours[best_j].begin(),
                                        subtours[best_j].end());
                subtours.erase(subtours.begin() + best_j);
            }
        }

        subtours[0].push_back(subtours[0].front()); // 시작점으로 돌아오기
        return subtours[0];
}

int main() {
    run("MCMF_knn", MCMF_knn_tour, files, use_2opt);
    return 0;
}

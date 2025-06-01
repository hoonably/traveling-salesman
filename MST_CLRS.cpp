//! If you want to more than 10K cities, you should use util2.h
#include "util.h"
// #include "util2.h"

// You should put this file in the dataset/ directory
vector<string> files = {
    "a280.tsp", 
    "xql662.tsp", 
    "kz9976.tsp", 
    // "mona_lisa100K.tsp"
};

// Prim 알고리즘을 이용해 MST 생성
vector<vector<int>> primMST() {
    vector<int> key(n + 1, INF);
    vector<int> parent(n + 1, -1);
    vector<bool> inMST(n + 1, false);

    key[1] = 0;
    for (int count = 1; count <= n - 1; ++count) {
        int u = -1;
        for (int v = 1; v <= n; ++v)
            if (!inMST[v] && (u == -1 || key[v] < key[u]))
                u = v;

        inMST[u] = true;
        for (int v = 1; v <= n; ++v) {
            if (!inMST[v]) {
                int d = get_dist(u,v);
                if (d < key[v]) {
                    key[v] = d;
                    parent[v] = u;
                }
            }
        }
    }

    vector<vector<int>> adj(n + 1);
    for (int v = 2; v <= n; ++v) {
        int u = parent[v];
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    return adj;
}

// DFS 순회로 Euler tour 생성
void dfs(int u, const vector<vector<int>>& adj, vector<bool>& visited, vector<int>& tour) {
    visited[u] = true;
    tour.push_back(u);
    for (int v : adj[u])
        if (!visited[v])
            dfs(v, adj, visited, tour);
}

vector<int> MST_CLRS_tour() {
    vector<vector<int>> adj = primMST();
    vector<int> tour;
    vector<bool> visited(n + 1, false);
    dfs(1, adj, visited, tour);
    
    // 시작점으로 돌아오기
    tour.push_back(tour.front());
    
    return tour;
}

int main() {
    bool use_2opt = true;  // 2-opt 최적화도 측정할지
    run("MST_CLRS", MST_CLRS_tour, files, use_2opt);
    return 0;
}

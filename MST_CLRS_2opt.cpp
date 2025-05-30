#define ALGO "MST_CLRS_2opt"
#include "util.h"

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
                int d = dist[u][v];
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
        if (!loadTSPFile(filename)) {
            cout << "Skipping " << filename << " due to large size: " << n << " cities.\n";
            continue;
        }

        auto start = chrono::high_resolution_clock::now();

        vector<vector<int>> adj = primMST();

        vector<int> tour;
        vector<bool> visited(n + 1, false);
        dfs(1, adj, visited, tour);
        apply_2_opt(tour);

        int total_length = computeCost(tour);

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        save(filename, tour, total_length, elapsed);
    }
    return 0;
}

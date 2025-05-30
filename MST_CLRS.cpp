#define ALGO "MST_CLRS"
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
        
int main() {
    for (const string& filename : filenames) {
        if (!loadTSPFile(filename)){
            cout << "Skipping " << filename << " due to large size: " << n << " cities.\n";
            continue;
        }

        // Start algorithm
        cout << "Algorithm: " << string(ALGO) << endl;
        cout << "Dataset: " << filename << endl;

        auto start = chrono::high_resolution_clock::now();
        //! Put algorithm here

        vector<vector<int>> adj = primMST();
        vector<int> tour;
        vector<bool> visited(n + 1, false);
        dfs(1, adj, visited, tour);
        
        //!
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsed = end - start;
        int total_length = computeCost(tour);
        save(filename, string(ALGO), tour, total_length, elapsed);
        cout << fixed << setprecision(6);  // 콘솔 출력도 동일하게 설정
        cout << "Final tour length : " << total_length << endl;
        cout << "Elapsed time: " << elapsed.count() << " seconds\n\n";


        // 2-opt optimization
        cout << "Algorithm: " << string(ALGO) + "(+2opt)" << endl;
        cout << "Dataset: " << filename << endl;

        start = chrono::high_resolution_clock::now();

        apply_2_opt(tour);

        end = chrono::high_resolution_clock::now();
        
        elapsed += end - start;
        total_length = computeCost(tour);
        save(filename, string(ALGO)+"(+2opt)", tour, total_length, end-start);
        cout << fixed << setprecision(6);  // 콘솔 출력도 동일하게 설정
        cout << "Final tour length : " << total_length << endl;
        cout << "Elapsed time: " << elapsed.count() << " seconds\n\n";
    }
    return 0;
}
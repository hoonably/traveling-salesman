#define ALGO "MCMF"
#include "util.h"

struct MCMF {
    using CostType = int;
    using FlowType = int;
    const CostType EPS = 0;

    struct Edge {
        int to, rev;
        FlowType cap;
        CostType cost;
    };

    int vertexSZ, SZ, bias, SRC, SINK;
    vector<vector<Edge>> graph;
    vector<bool> inQ;
    vector<CostType> costs;
    vector<bool> chk;
    vector<int> work;

    void init(int n) {
        vertexSZ = 2 * (n + 1);
        SZ = vertexSZ + 10;
        bias = n + 1;
        SRC = vertexSZ - 2;  // 2n
        SINK = vertexSZ - 1;  // 2n + 1

        graph.assign(SZ, {});
        inQ.assign(SZ, false);
        costs.assign(SZ, 0);
        chk.assign(SZ, false);
        work.assign(SZ, 0);
    }

    void addEdge(int from, int to, FlowType cap, CostType cost) {
        graph[from].push_back({to, (int)graph[to].size(), cap, cost});
        graph[to].push_back({from, (int)graph[from].size() - 1, 0, -cost});
    }

    bool spfa(int S, int T) {
        fill(costs.begin(), costs.end(), INF);
        fill(inQ.begin(), inQ.end(), false);
        queue<int> q;
        q.push(S);
        inQ[S] = true;
        costs[S] = 0;

        while (!q.empty()) {
            int now = q.front(); q.pop(); inQ[now] = false;
            for (const Edge& e : graph[now]) {
                if (e.cap > 0 && costs[e.to] > costs[now] + e.cost + EPS) {
                    costs[e.to] = costs[now] + e.cost;
                    if (!inQ[e.to]) inQ[e.to] = true, q.push(e.to);
                }
            }
        }
        return costs[T] < INF;
    }

    FlowType dfs(int now, int T, FlowType flow) {
        chk[now] = true;
        if (now == T) return flow;
        for (; work[now] < (int)graph[now].size(); ++work[now]) {
            Edge &e = graph[now][work[now]];
            if (!chk[e.to] && costs[e.to] == costs[now] + e.cost && e.cap > 0) {
                FlowType ret = dfs(e.to, T, min(flow, e.cap));
                if (ret > 0) {
                    e.cap -= ret;
                    graph[e.to][e.rev].cap += ret;
                    return ret;
                }
            }
        }
        return 0;
    }

    pair<CostType, FlowType> run(int S = -1, int T = -1) {
        if (S == -1) S = SRC;
        if (T == -1) T = SINK;
        CostType cost = 0;
        FlowType flow = 0;
        while (spfa(S, T)) {
            fill(chk.begin(), chk.end(), false);
            fill(work.begin(), work.end(), 0);
            while (FlowType f = dfs(S, T, INF)) {
                cost += costs[T] * f;
                flow += f;
                fill(chk.begin(), chk.end(), false);
            }
        }
        return {cost, flow};
    }
};

// Get K-Nearest Neighbors for each city
vector<vector<int>> get_knn(int k) {
    vector<vector<int>> knn(n + 1);
    for (int i = 1; i <= n; ++i) {
        vector<pair<int, int>> distances; // {distance, city}
        for (int j = 1; j <= n; ++j) {
            if (i != j) {
                distances.push_back({dist[i][j], j});
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
vector<int> MCMF_tour(){
        MCMF mcmf;
        mcmf.init(n);
        // 간선 추가
        for (int i = 1; i <= n; ++i) {
            mcmf.addEdge(mcmf.SRC, i, 1, 0);
            mcmf.addEdge(i + mcmf.bias, mcmf.SINK, 1, 0);
            for (int j = 1; j <= n; ++j) {
                if (i == j) continue;
                mcmf.addEdge(i, j + mcmf.bias, 1, dist[i][j]);
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
                    int cost = dist[tail][head];
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

        return subtours[0];
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

        vector<int> tour = MCMF_tour();
        int total_length=computeCost(tour);

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        // file에 tour 결과 저장
        save(filename, tour, total_length, elapsed);
    }
    return 0;
}

#define ALGO "MCMF_knn"

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <chrono>
#include <algorithm>
#include <queue>
using namespace std;

const int INF = 0x3f3f3f3f;
const int MAX_N = INF;  // 메모리 초과 방지용 상한
int K = 20;  // 20~40 정도로 잡고 비교

struct City {
    double x, y;
};

vector<City> cities;
int n;

vector<vector<int>> dist;

bool loadTSPFile(const string& filename) {
    ifstream infile("dataset/" + filename);
    if (!infile) {
        cerr << "파일 열기 실패: " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(infile, line)) {
        if (line.find("DIMENSION") != string::npos)
            n = stoi(line.substr(line.find(":") + 1));
        if (line == "NODE_COORD_SECTION") break;
    }

    if (n > 1000){
        infile.close();
        return false; // 파일 크기가 너무 큼 -> memory 초과 방지
    }

    if (n < 100) K = n-1;

    cities.resize(n + 1);
    for (int i = 1; i <= n; ++i) {
        int id;
        double x, y;
        infile >> id >> x >> y;
        cities[id] = {x, y};
    }
    infile.close();
    // 거리 행렬 전처리
    dist.assign(n + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= n; ++j) {
            double dx = cities[i].x - cities[j].x;
            double dy = cities[i].y - cities[j].y;
            dist[i][j] = static_cast<int>(round(sqrt(dx * dx + dy * dy)));
        }
    return true;
}

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
        SRC = vertexSZ - 2;
        SINK = vertexSZ - 1;

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

int computeCost(vector<int>& tour) {
    int cost = 0;
    for (int i = 0; i < tour.size() - 1; ++i)
        cost += dist[tour[i]][tour[i + 1]];
    return cost;
}

int main() {
    vector<string> filenames = {
        "a280.tsp", "xql662.tsp", "kz9976.tsp", "mona-lisa100K.tsp"
    };

    for (const string& filename : filenames) {
        if (!loadTSPFile(filename)){
            cout << "Skipping " << filename << " due to large size: " << n << " cities.\n";
            continue;
        }

        auto start = chrono::high_resolution_clock::now();

        MCMF mcmf;
        mcmf.init(n);

        // 미리 k-NN 후보 리스트 생성
        vector<vector<int>> knn(n + 1);
        for (int i = 1; i <= n; ++i) {
            vector<pair<int, int>> candidates;
            for (int j = 1; j <= n; ++j) {
                if (i == j) continue;
                candidates.emplace_back(dist[i][j], j);
            }
            // 거리 기준 상위 k개만 추출
            nth_element(candidates.begin(), candidates.begin() + K, candidates.end());
            for (int k = 0; k < K; ++k)
                knn[i].push_back(candidates[k].second);
        }

        // 간선 추가
        for (int i = 1; i <= n; ++i) {
            mcmf.addEdge(mcmf.SRC, i, 1, 0);
            mcmf.addEdge(i + mcmf.bias, mcmf.SINK, 1, 0);
            for (int j : knn[i]) {
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

        vector<int> tour = subtours[0];
        int total_length = computeCost(tour);

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        // file에 tour 결과 저장
        string basename = filename.substr(0, filename.find(".tsp"));
        string outname = "tour/" + string(ALGO) + "-" + basename + ".tour";
        ofstream out(outname);
        out << "NAME : " << basename << "\n";
        out << "COMMENT : Algorithm " + string(ALGO) + "\n";
        out << "COMMENT : Length " << total_length << " \n";
        out << "COMMENT : Elapsed time " << elapsed.count() << " seconds\n";
        out << "TYPE : TOUR\n";
        out << "DIMENSION : " << n << "\n";
        out << "TOUR_SECTION\n";
        for (int node : tour) out << node << "\n";
        out << "EOF\n";
        out.close();
        
        // 결과 터미널에 출력
        cout << "Filename: " << filename << endl;
        cout << "Final tour length (after stitching): " << total_length << endl;
        cout << "Elapsed time: " << elapsed.count() << " seconds\n\n";
    }
    return 0;
}

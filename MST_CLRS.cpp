#define ALGO "MST_CLRS"

#include <bits/stdc++.h>
using namespace std;

const int INF = 0x3f3f3f3f;
const int MAX_N = INF; // 메모리 초과 방지용 상한

struct City {
    double x, y;
};

vector<City> cities;
int n;

vector<vector<int>> mst_adj;
vector<bool> visited;
vector<int> tour;

// Dist 2차원 배열 사용시 Mona Lisa 100K 파일에서 메모리 초과 발생 -> 바로바로 계산하는 방식으로 변경
int getDist(int i, int j) {
    double dx = cities[i].x - cities[j].x;
    double dy = cities[i].y - cities[j].y;
    return static_cast<int>(round(sqrt(dx * dx + dy * dy)));
}

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

    if (n > MAX_N){
        infile.close();
        return false; // 파일 크기가 너무 큼 -> memory 초과 방지
    }

    cities.resize(n + 1);
    for (int i = 1; i <= n; ++i) {
        int id;
        double x, y;
        infile >> id >> x >> y;
        cities[id] = {x, y};
    }
    infile.close();
    // 거리 행렬 전처리
    // dist.assign(n + 1, vector<int>(n + 1, 0));
    // for (int i = 1; i <= n; ++i)
    //     for (int j = 1; j <= n; ++j) {
    //         double dx = cities[i].x - cities[j].x;
    //         double dy = cities[i].y - cities[j].y;
    //         dist[i][j] = static_cast<int>(round(sqrt(dx * dx + dy * dy)));
    //     }
    return true;
}

void primMST() {
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
                int d = getDist(u, v);
                if (d < key[v]) {
                    key[v] = d;
                    parent[v] = u;
                }
            }
        }
    }

    mst_adj.assign(n + 1, vector<int>());
    for (int v = 2; v <= n; ++v) {
        int u = parent[v];
        mst_adj[u].push_back(v);
        mst_adj[v].push_back(u);
    }
}

void dfs(int u) {
    visited[u] = true;
    tour.push_back(u);
    for (int v : mst_adj[u])
        if (!visited[v])
            dfs(v);
}

int computeCost() {
    int cost = 0;
    for (int i = 0; i < tour.size() - 1; ++i)
        cost += getDist(tour[i], tour[i + 1]);
    cost += getDist(tour.back(), tour.front());
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

        primMST();
        visited.assign(n + 1, false);
        tour.clear();
        dfs(1);

        int total_length = computeCost();

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
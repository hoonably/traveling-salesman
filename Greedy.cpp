#define ALGO "Greedy"
#include "util.h"

// 전통적인 Greedy TSP
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

        vector<int> tour = greedy();
        
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

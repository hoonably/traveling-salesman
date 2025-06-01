#include <vector>
#include <limits>
#include "util.h"

using namespace std;
#define INF 0x3f3f3f3f

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
    run("Greedy", greedy);
    return 0;
}

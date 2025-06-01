#include "util.h"

//! This algorithm is O(n^2 * 2^n), so it can handle up to 20 cities.
// You should put this file in the dataset/ directory
vector<string> files = {
    "weird20.tsp",
};
bool use_2opt = false;  // 2-opt 최적화도 측정할지

using State = pair<set<int>, int>;  // (subset, last_node)

vector<int> HeldKarp_tour() {
    const int START = 1;
    int N = n;

    map<State, int> dp;
    map<State, int> parent;

    // 초기 상태: START → i
    for (int i = 1; i <= N; ++i) {
        if (i == START) continue;
        set<int> s = {START, i};
        dp[{s, i}] = get_dist(START, i);
        parent[{s, i}] = START;
    }

    // DP: subset 크기 3~N
    for (int sz = 3; sz <= N; ++sz) {
        vector<set<int>> subsets;

        // 가능한 크기 sz의 subset 생성 (START 포함)
        vector<int> nodes;
        for (int i = 1; i <= N; ++i) nodes.push_back(i);
        vector<bool> take(N, false);
        fill(take.begin(), take.begin() + sz, true);

        do {
            set<int> s;
            for (int i = 0; i < N; ++i)
                if (take[i]) s.insert(nodes[i]);
            if (s.count(START)) subsets.push_back(s);
        } while (prev_permutation(take.begin(), take.end()));

        // 각 subset에 대해
        for (auto& S : subsets) {
            for (int j : S) {
                if (j == START) continue;
                int best = INF, best_k = -1;

                set<int> S_ = S;
                S_.erase(j);

                for (int k : S_) {
                    auto it = dp.find({S_, k});
                    if (it == dp.end()) continue;

                    int cost = it->second + get_dist(k, j);
                    if (cost < best) {
                        best = cost;
                        best_k = k;
                    }
                }

                if (best < INF) {
                    dp[{S, j}] = best;
                    parent[{S, j}] = best_k;
                }
            }
        }
    }

    // 마지막 단계: 전체 subset에서 START로 복귀
    set<int> full;
    for (int i = 1; i <= N; ++i) full.insert(i);

    int best_cost = INF, last_node = -1;
    for (int j = 1; j <= N; ++j) {
        if (j == START) continue;
        auto it = dp.find({full, j});
        if (it == dp.end()) continue;

        int cost = it->second + get_dist(j, START);
        if (cost < best_cost) {
            best_cost = cost;
            last_node = j;
        }
    }

    // 경로 복원
    vector<int> tour;
    set<int> S = full;
    int curr = last_node;
    while (curr != START) {
        tour.push_back(curr);
        int next = parent[{S, curr}];
        S.erase(curr);
        curr = next;
    }
    tour.push_back(START);
    reverse(tour.begin(), tour.end());
    tour.push_back(START);  // 종료점으로 복귀

    return tour;
}

int main() {
    run("HeldKarp", HeldKarp_tour, files, use_2opt);
    return 0;
}

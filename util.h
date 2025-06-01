#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <chrono>
#include <algorithm>
#include <queue>
#include <iomanip>

using namespace std;

//! Use this file for not large datasets (<= 10K cities)
// This use vector dist[i][j], so it can handle up to 10K cities.

vector<string> filenames = {
    "a280.tsp", 
    "xql662.tsp", 
    // "kz9976.tsp", 
};

struct City {
    double x, y;
};

const int MAX_N = 10000;
int n;
int K = 20;
vector<City> cities;
vector<vector<int>> dist;

//! You should use O2 optimization flag when compiling this file.
// Then it replaces the get_dist(i, j) function with dist[i][j] to save time.
int get_dist(int i, int j) {
    return dist[i][j];
}

int computeCost(vector<int>& tour) {
    int cost = 0;
    for (int i = 0; i < tour.size() - 1; ++i)
        cost += dist[tour[i]][tour[i + 1]];
    return cost;
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

bool loadTSPFile(const string& filename) {
    ifstream infile("dataset/" + filename);
    if (!infile) {
        cerr << "파일 열기 실패: " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(infile, line)) {
        if (line.find("DIMENSION") != string::npos){
            n = stoi(line.substr(line.find(":") + 1));
            if (n > MAX_N){
                infile.close();
                return false;
            }
        }
        if (line == "NODE_COORD_SECTION") break;
    }

    if (n < 100) K = n - 1;

    cities.resize(n + 1);
    for (int i = 1; i <= n; ++i) {
        int id;
        double x, y;
        infile >> id >> x >> y;
        cities[id] = {x, y};
    }
    infile.close();

    // 10K 이상의 데이터는 거리 계산시 메모리 초과
    dist.assign(n + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= n; ++j) {
            double dx = cities[i].x - cities[j].x;
            double dy = cities[i].y - cities[j].y;
            dist[i][j] = static_cast<int>(round(sqrt(dx * dx + dy * dy)));
        }
    return true;
}

void save(string filename, string algo, vector<int>& tour, int total_length, chrono::duration<double> elapsed) {
    string basename = filename.substr(0, filename.find(".tsp"));
    string outname = "tour/" + algo + "-" + basename + ".tour";
    ofstream out(outname);

    out << fixed << setprecision(6);  // 소수점 6자리까지 고정 소수점 출력
    out << "NAME : " << basename << "\n";
    out << "COMMENT : Algorithm " + algo + "\n";
    out << "COMMENT : Length " << total_length << " \n";
    out << "COMMENT : Elapsed time " << elapsed.count() << " seconds\n";
    out << "TYPE : TOUR\n";
    out << "DIMENSION : " << n << "\n";
    out << "TOUR_SECTION\n";
    for (int node : tour) out << node << "\n";
    out << "EOF\n";
    out.close();
}

void run(const string& algo_name, vector<int>(*algorithm)()) {
    for (const string& filename : filenames) {
        if (!loadTSPFile(filename)) {
            cout << "Skipping " << filename << " due to large size: " << n << " cities.\n";
            continue;
        }

        cout << "Algorithm: " << algo_name << endl;
        cout << "Dataset: " << filename << endl;

        auto start = chrono::high_resolution_clock::now();
        vector<int> tour = algorithm();
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsed = end - start;
        int total_length = computeCost(tour);
        save(filename, algo_name, tour, total_length, elapsed);
        cout << "Final tour length : " << total_length << endl;
        cout << "Elapsed time: " << elapsed.count() << " seconds\n\n";

        cout << "Algorithm: " << algo_name + "(+2opt)" << endl;
        cout << "Dataset: " << filename << endl;

        start = chrono::high_resolution_clock::now();
        apply_2_opt(tour);
        end = chrono::high_resolution_clock::now();

        elapsed += end - start;
        total_length = computeCost(tour);
        save(filename, algo_name + "(+2opt)", tour, total_length, elapsed);
        cout << "Final tour length : " << total_length << endl;
        cout << "Elapsed time: " << elapsed.count() << " seconds\n\n";
    }
}

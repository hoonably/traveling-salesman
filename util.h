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
#include <set>
#include <map>
#include <limits>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>

using namespace std;
const int INF = numeric_limits<int>::max();

//! Use this file for not large datasets (<= 10K cities)
// This use vector dist[i][j], so it can handle up to 10K cities.

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

bool loadTSPFile(const string& file) {
    ifstream infile("dataset/" + file);
    if (!infile) {
        cerr << "파일 열기 실패: " << file << endl;
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

void create_directories(const string& path) {
    stringstream ss(path);
    string segment;
    string current_path = "";

    while (getline(ss, segment, '/')) {
        if (!segment.empty()) {
            current_path += segment + "/";
            struct stat info;
            if (stat(current_path.c_str(), &info) != 0) {
                mkdir(current_path.c_str(), 0777); // 상위 디렉토리 포함 생성
            }
        }
    }
}

void save(string file, string algo, vector<int>& tour, int total_length, chrono::duration<double> elapsed) {
    string basename = file.substr(0, file.find(".tsp"));
    string folder = "tour_paths/" + basename;
    string outname = folder + "/" + algo + "-" + basename + ".tour";

    create_directories(folder);

    ofstream out(outname);
    out << fixed << setprecision(6);
    out << "NAME : " << basename << "\n";
    out << "COMMENT : Algorithm " + algo << "\n";
    out << "COMMENT : Length " << total_length << " \n";

    stringstream ss;
    ss << fixed << setprecision(6) << elapsed.count();
    out << "COMMENT : Elapsed time " << ss.str() << " seconds\n";

    out << "TYPE : TOUR\n";
    out << "DIMENSION : " << n << "\n";
    out << "TOUR_SECTION\n";
    for (int node : tour) out << node << "\n";
    out << "EOF\n";
    out.close();
}


void run(const string& algo_name, vector<int>(*algorithm)(), vector<string>& files, bool use_2opt = false) {
    for (const string& file : files) {
        if (!loadTSPFile(file)) {
            cout << "Skipping " << file << " due to large size: " << n << " cities.\n";
            continue;
        }

        cout << "Algorithm: " << algo_name << endl;
        cout << "Dataset: " << file << endl;

        auto start = chrono::high_resolution_clock::now();
        vector<int> tour = algorithm();
        auto end = chrono::high_resolution_clock::now();

        chrono::duration<double> elapsed = end - start;
        int total_length = computeCost(tour);
        save(file, algo_name, tour, total_length, elapsed);
        cout << "Final tour length : " << total_length << endl;
        cout << fixed << setprecision(6)
             << "Elapsed time: " << elapsed.count() << " seconds\n\n";

        if (!use_2opt) continue;

        // Apply 2-opt optimization
        cout << "Algorithm: " << algo_name + "(+2opt)" << endl;
        cout << "Dataset: " << file << endl;

        start = chrono::high_resolution_clock::now();
        apply_2_opt(tour);
        end = chrono::high_resolution_clock::now();

        elapsed += end - start;
        total_length = computeCost(tour);
        save(file, algo_name + "(+2opt)", tour, total_length, elapsed);
        cout << "Final tour length : " << total_length << endl;
        cout << fixed << setprecision(6)
             << "Elapsed time: " << elapsed.count() << " seconds\n\n";
    }
}

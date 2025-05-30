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

using namespace std;
#define INF 0x3f3f3f3f

vector<string> filenames = {
    "a280.tsp", 
    "xql662.tsp", 
    "kz9976.tsp", 
    // "mona_lisa100K.tsp",
};

struct City {
    double x, y;
};

const int MAX_N = INF;  // for depend vector size
int n;
int K = 20;
vector<City> cities;
vector<vector<int>> dist;

int get_dist(int i, int j) {
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

void save(string filename, vector<int>& tour, int total_length, chrono::duration<double> elapsed) {
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
    cout << "Final tour length : " << total_length << endl;
    cout << "Elapsed time: " << elapsed.count() << " seconds\n\n";
}
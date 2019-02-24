#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>
#include <assert.h>
#include <sstream>
#include <utility>
#include <optional>
#include <set>

using namespace std;

const vector<string> files = {};

struct Edge {
    double len, cost;
    int a, b;
    bool taken;
};

class Graph {
public:
    Graph(string input_name);

    vector<set<Edge>> get_graph();

    vector<vector<Edge>> get_full_graph() const;

    int s() const;

    int cars_number() const;

    int t() const;

    int n() const;

    int m() const;

    int c() const;

    optional<Edge> get_edge(int a, int b) const;

    void add_edge(int a, int b, double cost, double len);

    void delete_edge(const Edge& edge);

private:
    int n_;
    int m_;

    vector<set<Edge>> g_;
    vector<vector<Edge>> full_g_;
};

//bool validation(string file_name, double& res) {
//    const string input_name = "input.in";
//    Graph g(input_name);
//    ifstream cin(file_name);
//    stringstream ss;
//
//    int k;
//    cin >> k;
//
//    if (k < 0 || k > g.cars_number()) {
//        ss << "bad cars number: " << k;
//        throw ss.str();
//    }
//
//    for (int i = 0; i < k; i++) {
//        int sz;
//        cin >> sz;
//        if (sz <= 0) {
//            ss << "bad path length: " << sz;
//            throw ss.str();
//        }
//        vector<int> path;
//        for (int j = 0; j < sz; j++) {
//            int cur;
//            cin >> cur;
//            path.push_back(cur);
//        }
//        if (path[0] != g.start_vertex()) {
//            ss << "bad start vertex: " << path[0];
//            throw ss.str();
//        }
//        for (size_t i = 0; i + 1 < path.size(); i++) {
//            auto edge = g.get_edge(path[i], path[i + 1]);
//            if (!edge) {
//                ss << "no edge found between " << path[i] << " and " << path[i + 1] << "\n";
//                throw ss.str();
//            }
//        }
//    }
//}

#ifndef TEST

int main() {


}

#endif


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

#ifndef TEST

int main() {


}

#endif


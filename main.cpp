#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>
#include <fstream>
#include <assert.h>
#include <sstream>
#include <utility>
#include <optional>
#include <set>

using namespace std;

#define int int64_t
struct Point
{
  Point(double x, double y): x(x), y(y) {}
  Point operator-(const Point & point) {
    return {x - point.x, y - point.y};
  }

  double x, y;
};

namespace helper {
template <typename TFloat> inline TFloat DegToRad(TFloat deg)
{
  return deg * TFloat(M_PI) / TFloat(180);
}

template <typename T>
inline T clamp(T const x, T const xmin, T const xmax)
{
  if (x > xmax)
    return xmax;
  if (x < xmin)
    return xmin;
  return x;
}
template <typename TFloat> inline TFloat RadToDeg(TFloat rad)
{
  return rad * TFloat(180) / TFloat(M_PI);
}
static double constexpr kMinY = -180.0;
static double constexpr kMaxY = 180.0;
static double LonToX(double lon) { return lon; }
static double ClampY(double d) { return clamp(d, kMinY, kMaxY); }
static double LatToY(double lat)
{
  double const sinx = sin(DegToRad(clamp(lat, -86.0, 86.0)));
  double const res = RadToDeg(0.5 * log((1.0 + sinx) / (1.0 - sinx)));
  return ClampY(res);
}
static Point FromLatLon(double lat, double lon)
{
  return {LonToX(lon), LatToY(lat)};
}

}  // namespace

struct Edge {
    int a, b;
    int len, cost;
    bool taken;
    int id;
};

struct Coordinates {
    double latitude, longitude;
};

class Graph {
public:
    Graph(string input_name) {
        ifstream in(input_name);
        in >> n_ >> m_ >> t_ >> c_ >> s_;
        coordinates_.resize(n_);
        for (int i = 0; i < n_; i++) {
            in >> coordinates_[i].latitude >> coordinates_[i].longitude;
        }
        full_g_.resize(n_);
        for (int i = 0; i < n_; i++) {
            full_g_[i].resize(n_);
            for (int j = 0; j < n_; j++) {
                full_g_[i][j] = {-1, -1, -1, -1, false, -1};
            }
        }
        for (int i = 0; i < m_; i++) {
            int a, b, d, c, l;
            in >> a >> b >> d >> c >> l;
            g_[a].push_back({a, b, l, c, true, i});
            full_g_[a][b] = {a, b, l, c, true, i};
            if (d == 2) {
                g_[b].push_back({b, a, l, c, true, i});
                full_g_[b][a] = {b, a, l, c, true, i};
            }
        }
    }

    vector<vector<Edge>> graph() const {
        return g_;
    }

    vector<vector<Edge>> full_graph() const {
        return full_g_;
    }

    vector<vector<Edge>>& graph() {
        return g_;
    }

    vector<vector<Edge>>& full_graph() {
        return full_g_;
    }

    vector<Coordinates> coordinates() const {
        return coordinates_;
    }

    vector<Coordinates>& coordinates() {
        return coordinates_;
    }

    int s() const {
        return s_;
    }

    int t() const {
        return t_;
    }

    int n() const {
        return n_;
    }

    int m() const {
        return m_;
    }

    int c() const {
        return c_;
    }

private:
    int n_;
    int m_;
    int c_;
    int s_;
    int t_;

    vector<vector<Edge>> g_;
    vector<vector<Edge>> full_g_;
    vector<Coordinates> coordinates_;
};

void validation(const string& graph_file_name, const string& file_name, int& cost) {
    const string input_name = "input.in";
    const Graph g(graph_file_name);
    ifstream cin(file_name);
    stringstream ss;

    int k;
    cin >> k;

    if (k < 0 || k > g.c()) {
        ss << "bad cars number: " << k;
        throw ss.str();
    }

    cost = 0;

    set<int> visited_edges;

    for (int i = 0; i < k; i++) {
        int sz;
        cin >> sz;
        if (sz <= 0) {
            ss << "bad path length: " << sz;
            throw ss.str();
        }
        vector<int> path;
        for (int j = 0; j < sz; j++) {
            int cur;
            cin >> cur;
            path.push_back(cur);
        }
        if (path[0] != g.s()) {
            ss << "bad start vertex: " << path[0];
            throw ss.str();
        }
        double sum_time = 0;
        for (size_t i = 0; i + 1 < path.size(); i++) {
            int a = path[i];
            int b = path[i + 1];
            const Edge& edge = g.full_graph()[a][b];
            if (edge.a == -1) {
                ss << "no edge found between " << path[i] << " and " << path[i + 1] << "\n";
                throw ss.str();
            }
            sum_time += edge.cost;
            if (sum_time > g.t()) {
                break;
            }
            if (visited_edges.find(edge.id) == visited_edges.end()) {
                visited_edges.insert(edge.id);
                cost += edge.len;
            }
        }
        if (sum_time > g.t()) {
            ss << "time " << sum_time << " for car: " << i << "\n";
            throw ss.str();
        }
    }
}

#undef int
#ifndef TEST

int main() {

}

#endif


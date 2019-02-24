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
#include <unordered_map>

using namespace std;

#define int int64_t
struct Point
{
    Point() = default;
    Point(double x, double y): x(x), y(y) {}
    Point operator-(const Point & point) {
        return {x - point.x, y - point.y};
    }

    Point & operator +=(Point const & p)
    {
        x += p.x;
        y += p.y;
        return *this;
    }

    Point(const Point & from) {
      x = from.x;
      y = from.y;
    }

    double x = 0.0;
    double y = 0.0;
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

template <typename T = bool>
struct Edge {
    int a, b;
    int len, cost;
    bool taken;
    int id;
    T data = T();
};

template<typename T = bool>
class Graph {
public:
    Graph(string input_name) {
        ifstream in(input_name);
        in >> n_ >> m_ >> t_ >> c_ >> s_;
        double lat, lon;
        coordinates_.resize(n_);
        for (int i = 0; i < n_; i++) {
            in >> lat >> lon;
            coordinates_[i] = helper::FromLatLon(lat, lon);
        }
        PrepareSegments();

        full_g_.resize(n_);
        g_.resize(n_);
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

    vector<vector<Edge<T>>> graph() const {
        return g_;
    }

    vector<vector<Edge<T>>> full_graph() const {
        return full_g_;
    }

    vector<vector<Edge<T>>>& graph() {
        return g_;
    }

    vector<vector<Edge<T>>>& full_graph() {
        return full_g_;
    }

    vector<Point> coordinates() const {
        return coordinates_;
    }

    vector<Point>& coordinates() {
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

    static unordered_map<int, int> GetSegments(vector<Point> const & coordinates, size_t startJunctionNumber)
    {
        vector<pair<Point, int>> right;
        vector<pair<Point, int>> left;

        vector<pair<Point, int>> preLeft;
        vector<pair<Point, int>> preRight;
        Point start = coordinates[startJunctionNumber];
        for (size_t i = 0; i < coordinates.size(); ++i)
        {
            if (i == startJunctionNumber)
                continue;

            auto const & p = coordinates[i];
            if (p.x == start.x)
            {
                if (p.y > 0)
                    preLeft.emplace_back(p, i);
                else
                    preRight.emplace_back(p, i);

                continue;
            }

            if (p.x - start.x > 0)
                right.emplace_back(p, i);
            else
                left.emplace_back(p, i);
        }

        auto const comparator = [&start](auto const & a, auto const & b)
        {
            double aCoef = (a.first.y - start.y) / (a.first.x - start.x);
            double bCoef = (b.first.y - start.y) / (b.first.x - start.x);

            return aCoef < bCoef;
        };

        std::sort(right.begin(), right.end(), comparator);

        std::sort(left.begin(), left.end(), comparator);

        preRight.insert(preRight.end(), right.begin(), right.end());
        preLeft.insert(preLeft.end(), left.begin(), left.end());

        preRight.insert(preRight.end(), preLeft.begin(), preLeft.end());

        size_t n = static_cast<int64_t>(coordinates.size() / 8);
        size_t cnt = 0;
        int currentSector = 0;
        unordered_map<int, int> result;
        for (size_t i = 0; i < coordinates.size(); ++i)
        {
            if (cnt >= n)
            {
                ++currentSector;
                cnt = 0;
            }

            result[preRight[i].second] = currentSector;
            cnt++;
        }

        result[startJunctionNumber] = 0;
        return result;
    }

    void PrepareSegments()
    {
        sectors_ = GetSegments(coordinates_, s_);
    }

    int GetSegmentNumber(int junctionNumber) {
        auto const it = sectors_.find(junctionNumber);
        if (it == sectors_.cend())
        {
            std::stringstream ss;
            ss << "bad junction number: " << junctionNumber;
            throw std::runtime_error(ss.str());
        }

        return it->second;
    }

private:
    int n_;
    int m_;
    int c_;
    int s_;
    int t_;

    vector<vector<Edge<T>>> g_;
    vector<vector<Edge<T>>> full_g_;
    vector<Point> coordinates_;

    // JunctionNumber => sector number
    unordered_map<int, int> sectors_;
};

void validation(string graph_file_name, string file_name, int& cost) {
    string input_name = "input.in";
    Graph g(graph_file_name);
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
            auto& edge = g.full_graph()[a][b];
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
}  // namespace helper

namespace gleb {
    using namespace helper;

    const int ITERATION = 1000;
    const int PATH_TO_TAKE = 10000;

    void Solve(const string& out) {
        Graph<double> gr("input.in");
        auto my_gr = gr.full_graph();

        auto T = gr.t();

        auto gen_path = [&](int start) {

        };

        for (int i = 0; i < ITERATION; i++) {

        }


    }

} // namespace gleb

#undef int
#ifndef TEST

int main() {

}

#endif


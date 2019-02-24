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
#include <map>
#include <random>

using namespace std;

#define int int64_t
struct Point
{
    Point() = default;
    Point(double x, double y): x(x), y(y) {}
    Point operator-(const Point & point) {
    return {x - point.x, y - point.y};
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

struct Coordinates {
    double latitude, longitude;
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
        g_.resize(n_);
        for (int i = 0; i < m_; i++) {
            int a, b, d, c, l;
            in >> a >> b >> d >> c >> l;
            g_[a].push_back({a, b, l, c, true, i});
            if (d == 2) {
                g_[b].push_back({b, a, l, c, true, i});
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

    int GetSegmentNumber(int junctionNumber) {
        return 0;
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
            Edge edge;
            for (const auto& e : g.graph()[a]) {
                if (e.b == b) {
                    edge = e;
                }
            }
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

    cout << "you score: " << cost << "\n";
}

}  // namespace helper

namespace gleb {
    using namespace helper;

    const int ITERATION = 100;
    const int PATH_TO_TAKE = 1000;
    const int BEST_PATH_TO_TAKE = 10;

    void Solve(const string &out) {
        Graph<double> gr("input.in");
        auto &my_gr = gr.graph();

        auto T = gr.t();

        std::random_device rd;
        std::mt19937 gen(rd());

        auto gen_path = [&](int start, int64_t &score) {
            int cur = start;
            vector<Edge<double> *> path;
            set<int> st;
            int64_t cur_t = T;
            score = 0;
            while (cur_t) {
                vector<Edge<double> *> vv;
                double sum = 0;
                for (auto &e : my_gr[cur]) {
                    if (T >= e.cost) {
                        vv.push_back(&e);
                        sum += e.data;
                    }
                }
                std::uniform_int_distribution<> dis(0, sum);
                if (sum == 0) {
                    return path;
                }
                int64_t rd_res = dis(gen);
                sum = 0;
                for (auto &v : vv) {
                    sum += v->data;
                    if (sum >= rd_res) {
                        path.push_back(v);
                        cur_t -= v->cost;
                        auto hs = v->a * 20000 + v->b;
                        if (st.count(hs)) {
                            score += v->len;
                        }
                        st.insert(hs);
                        assert(v->a == cur);
                        cur = v->b;
                        break;
                    }
                }
            }
            return path;
        };
        cerr << "ITERATIONS started" << endl;
//        for (int car = 0; car < gr.c(); car++) {
        for (int i = 0; i < ITERATION; i++) {
            auto cmp = [](auto &l, auto &r) {
                return l.first > r.first;
            };
            set<pair<int64_t, vector<Edge<double> *>>, decltype(cmp)> st(cmp);
            for (int j = 0; j < PATH_TO_TAKE; j++) {
                int64_t score = 0;
                auto vec = gen_path(gr.s(), score);
                st.emplace(score, vec);
                if (st.size() > BEST_PATH_TO_TAKE) {
                    st.erase(*st.rbegin());
                }
            }
            cerr << "ITERATION " << i << " SCORE FOUND " << st.begin()->first << endl;

            for (auto &bst : st) {
                for (auto &el : bst.second) {
                    el->data++;
                }
            }
        }

    }
}  // namespace gleb;

namespace george {
    struct Edge : public helper::Edge<> {
        double profit;
        int rev_index;
    };

    void print_ans(const vector<vector<int>>& paths) {
        ofstream out("george_output.txt");
        out << paths.size() << endl;
        for (const auto& path : paths) {
            out << path.size() << " ";
            for (auto v : path) {
                out << v << " ";
            }
            out << "\n";
        }
        out.close();
    }

    void solve() {
        helper::Graph graph("input.in");
        vector<vector<Edge>> g(graph.n());
        map<pair<int, int>, int> used_edges;
        for (size_t i = 0; i < graph.n(); i++) {
            for (size_t j = 0; j < graph.graph()[i].size(); j++) {
                const auto& e = graph.graph()[i][j];
                int rev_index = -1;
                if (used_edges.count({e.a, e.b})) {
                    rev_index = used_edges[{e.a, e.b}];
                } else {
                    used_edges[{e.b, e.a}] = j;
                }
                Edge new_edge;
                new_edge.a = e.a;
                new_edge.b = e.b;
                new_edge.len = e.len;
                new_edge.cost = e.cost;
                if (rev_index != -1) {
                    new_edge.rev_index = rev_index;
                    g[e.b][rev_index].rev_index = j + 1;
                } else {
                    new_edge.rev_index = -1;
                }
                g[i].push_back(std::move(new_edge));
            }
            sort(g[i].begin(), g[i].end(), [](const Edge& lhs, const Edge& rhs) -> bool {
                return lhs.b < rhs.b;
            });
        }
        vector<vector<int>> paths;
        int expected_cost = 0;
        for (size_t car = 0; car < graph.c(); car++) {
            int cur_time = 0;
            while (cur_time <= graph.t()) {
                int cur_vertex = graph.s();
                vector<pair<double, int>> edges_to_go;
                vector<int> path{cur_vertex};
                for (size_t i = 0; i < g[cur_vertex].size(); i++) {
                    if (g[cur_vertex][i].taken) {
                        continue;
                    }
                    if (cur_time + g[cur_vertex][i].cost > graph.c()) {
                        continue;
                    }
                    edges_to_go.emplace_back(g[cur_vertex][i].profit, i);
                }
                if (edges_to_go.empty()) {
                    paths.push_back(path);
                    break;
                }
                int pos_max = max_element(edges_to_go.begin(), edges_to_go.end()) - edges_to_go.begin();
                int index = edges_to_go[pos_max].second;
                expected_cost += g[cur_vertex][index].len;
                cur_time += g[cur_vertex][index].cost;
                g[cur_vertex][index].taken = true;
                if (g[cur_vertex][index].rev_index != -1) {
                    auto b = g[cur_vertex][index].b;
                    auto rev_index = g[cur_vertex][index].rev_index;
                    g[b][rev_index].taken = true;
                }
            }
        }

        print_ans(paths);

        cout << "expected score: " << expected_cost << "\n";

        int final_score;
        helper::validation("input.in", "george_output.txt", final_score);
        assert(final_score == expected_cost);
    }
}  // namespace george

#undef int
#ifndef TEST

int main() {
<<<<<<< HEAD
    george::solve();
=======
    gleb::Solve("input.in");
>>>>>>> d9c6f76a704b38d6b039618eb05c03ed1efdb895
}

#endif


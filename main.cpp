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


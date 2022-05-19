#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <cstdlib>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

/**
 * Program to simulate a robot sautering arm using a few TSP (Traveling Sales
 * Person) heuristics: Nearest Neighbor and Closest Pair
 *
 * Because this is simulating a sautering arm, sets of less than 2 points are
 * ignored.
 *
 * Anyway, the point was to compare these two heuristics.
 * */

struct point {
  int x;
  int y;

  bool operator==(const point &other) const {
    return x == other.x && y == other.y;
  }

  friend ostream &operator<<(ostream &out, const point &p) {
    out << p.x << ", " << p.y;
    return out;
  }

  size_t operator()(const point &point) const noexcept {
    union {
      int a[2];
      size_t s;
    } temp;
    temp.a[0] = point.x;
    temp.a[1] = point.y;
    return temp.s;
  }
};

typedef unordered_map<point, unordered_set<point, point>, point> shitty_graph;

unordered_set<point, point> mock_data() {
  int min_points = 3;
  int max_points = 3;
  int range = max_points - min_points;

  int max_x = 10;
  int max_y = 10;

  int number_of_points =
      ((double)rand() / (double)RAND_MAX) * range + min_points;

  unordered_set<point, point> points;

  for (size_t i = 0; i < number_of_points; i++) {
    int x = ((double)rand() / (double)RAND_MAX) * max_x - max_x / 2;
    int y = ((double)rand() / (double)RAND_MAX) * max_y - max_y / 2;
    point p;
    p.x = x;
    p.y = y;
    points.insert(p);
  }

  return points;
}

double distance(point a, point b) {
  double xDiff = double(b.x - a.x);
  double yDiff = double(b.y - a.y);
  return sqrt(pow(xDiff, 2.0) + pow(yDiff, 2.0));
}

bool is_connected(const shitty_graph &g, const point &a, const point &b) {
  queue<point> q;
  unordered_set<point, point> visited;

  q.push(a);
  while (q.size()) {
    point p = q.front();

    if (p == b) return true;

    visited.insert(p);

    for (point connected : g.at(p)) {
      if (!visited.count(connected)) q.push(connected);
    }

    q.pop();
  }

  return false;
}

double nearest_neighbor(unordered_set<point, point> points) {
  if (points.size() < 2) return 0.0;

  size_t starting_size = points.size();
  point starting_point{0, 0};
  double total_distance = 0.0;

  point current_point = starting_point;
  point first_choice;

  cout << current_point << endl;

  while (points.size()) {
    double shortest_distance = INFINITY;
    point nearest_neighbor;

    for (point neighbor : points) {
      double neighbor_distance = distance(current_point, neighbor);

      if (neighbor_distance < shortest_distance) {
        shortest_distance = neighbor_distance;
        nearest_neighbor = neighbor;
      }
    }

    if (points.size() == starting_size) {
      first_choice = nearest_neighbor;
    }

    points.erase(nearest_neighbor);
    total_distance += shortest_distance;
    current_point = nearest_neighbor;

    cout << current_point << endl;
  }

  total_distance += distance(current_point, first_choice);
  current_point = first_choice;

  cout << current_point << endl;

  total_distance += distance(current_point, starting_point);

  cout << point{0, 0} << endl;

  return total_distance;
}

double closest_pair(unordered_set<point, point> points) {
  if (points.size() < 2) return 0.0;

  size_t max_connections = points.size() == 2 ? 1 : 2;

  point starting_point{0, 0};
  double total_distance = 0.0;

  shitty_graph graph;
  for (point p : points) {
    graph.insert({p, {}});
  }

  point current_point = starting_point;

  cout << current_point << endl;

  size_t n = points.size();
  for (size_t i = 0; i < n; i++) {
    double shortest_distance = INFINITY;
    point closest_pairA;
    point closest_pairB;

    for (point p1 : points) {
      for (point p2 : points) {
        if (p1 == p2 || graph.at(p1).size() == max_connections ||
            graph.at(p2).size() == max_connections ||
            (is_connected(graph, p1, p2) && i != n - 1)) {
          continue;
        }

        double pair_distance = distance(p1, p2);

        if (pair_distance < shortest_distance) {
          closest_pairA = p1;
          closest_pairB = p2;
          shortest_distance = pair_distance;
        }
      }
    }

    point next_point = distance(current_point, closest_pairA) <
                               distance(current_point, closest_pairB)
                           ? closest_pairA
                           : closest_pairB;
    total_distance += distance(current_point, next_point);
    current_point = next_point;

    cout << current_point << endl;

    next_point = current_point == closest_pairA ? closest_pairB : closest_pairA;
    total_distance += distance(current_point, next_point);
    current_point = next_point;

    cout << current_point << endl;

    graph.at(closest_pairA).insert(closest_pairB);
    graph.at(closest_pairB).insert(closest_pairA);
  }
  total_distance += distance(current_point, starting_point);

  cout << point{0, 0} << endl;

  return total_distance;
}

int main() {
  srand(time(NULL));

  unordered_set<point, point> points = mock_data();

  cout << "POINTS:" << endl;
  cout << "-----------------------------" << endl;
  for (point p : points) {
    cout << p << endl;
  }
  cout << "nearest neighbor heuristic: " << endl
       << nearest_neighbor(points) << endl;
  cout << "closest pair heuristic: " << endl << closest_pair(points) << endl;

  return 0;
}

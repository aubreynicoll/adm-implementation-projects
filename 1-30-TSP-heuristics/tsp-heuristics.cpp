#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <cstdlib>
#include <iostream>
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

unordered_set<point, point> mock_data() {
  int min_points = 3;
  int max_points = 10;
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

double nearest_neighbor(unordered_set<point, point> points) {
  if (points.size() < 2) return 0.0;

  point starting_point{0, 0};
  double total_distance = 0.0;

  point current_point = starting_point;
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

    points.erase(nearest_neighbor);
    total_distance += shortest_distance;
    current_point = nearest_neighbor;
  }
  total_distance += distance(current_point, starting_point);

  return total_distance;
}

double closest_pair(unordered_set<point, point> points) {
  if (points.size() < 2) return 0.0;

  size_t max_connections = points.size() == 2 ? 1 : 2;

  point starting_point{0, 0};
  double total_distance = 0.0;

  unordered_map<point, unordered_set<point, point>, point> connected;
  for (point p : points) {
    connected.insert({p, {}});
  }

  point current_point = starting_point;
  size_t n = points.size();
  for (size_t i = 0; i < n; i++) {
    double shortest_distance = INFINITY;
    point closest_pairA;
    point closest_pairB;

    for (point p1 : points) {
      for (point p2 : points) {
        if (p1 == p2 || connected.at(p1).count(p2)) {
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

    next_point = current_point == closest_pairA ? closest_pairB : closest_pairA;
    total_distance += distance(current_point, next_point);
    current_point = next_point;

    connected.at(closest_pairA).insert(closest_pairB);
    connected.at(closest_pairB).insert(closest_pairA);
  }
  total_distance += distance(current_point, starting_point);

  return total_distance;
}

int main() {
  srand(time(NULL));

  unordered_set<point, point> points = mock_data();

  for (point p : points) {
    cout << p.x << ", " << p.y << endl;
  }
  cout << "nearest neighbor heuristic: " << nearest_neighbor(points) << endl;
  cout << "closest pair heuristic: " << closest_pair(points) << endl;

  return 0;
}

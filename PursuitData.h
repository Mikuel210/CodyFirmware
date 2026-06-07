#pragma once
#include <vector>

struct PursuitData {
  std::vector<Vector3> points;
  double lookaheadDistance;
  int lineIndex;
};
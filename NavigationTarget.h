#pragma once
#include "Vector3.h"

struct NavigationTarget {
  Vector3 target;
  double decelerationDistance;
};
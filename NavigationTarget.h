#pragma once
#include "Vector3.h"

struct NavigationTarget {
  Vector3 target;
  Vector3 steeringTarget;
  double decelerationDistance;
};
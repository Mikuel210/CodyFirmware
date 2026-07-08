#pragma once
#include "Vector3.h"
#include "Color.h"

struct FusionData {
  float orientation;
  Vector3 position;

  Vector3 toolheadPosition;
  Vector3 wheelsPosition;
  Vector3 millPosition;

  double voltage;
  Color color;
};

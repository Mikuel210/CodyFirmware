#pragma once
#include "Vector3.h"

struct Line {
  Line(Vector3 start_, Vector3 end_) : start(start_), end(end_) {}

  Vector3 start;
  Vector3 end;
};
#pragma once
#include <cstdint>

struct ColorData {
  uint16_t r, g, b;

  ColorData(uint16_t r_, uint16_t g_, uint16_t b_) : r(r_), g(g_), b(b_) {}
  ColorData() {}
};

#pragma once
#include <Arduino.h>

class Plotter {
  public:
    static void plot(const char* name, float number) {
      Serial.print(name);
      Serial.print(":");
      Serial.print(number);
      Serial.print(",");
    }

    static void setLimits(float lowerEnd, float upperEnd) {
      plot("LOWER_LIMIT", lowerEnd);
      plot("UPPER_LIMIT", upperEnd);
    }

    static void endPlot() {
      Serial.println();
    }
};
#pragma once
#include "Vector3.h"
#include "SensorData.h"
#include "FusionData.h"
#include "Color.h"
#include <SensorFusion.h>
#include <vector>

// Fusion parameters
#define TRAVEL_PER_REVOLUTION_MM 276.460153516
#define X_AXIS_MM_PER_REVOLUTION 43.9822971503
#define Z_AXIS_MM_PER_REVOLUTION 56.5486677646
#define WHEELS_MM_PER_REVOLUTION 38.2790981791
#define TICKS_PER_REVOLUTION 600.0
#define N20_TICKS_PER_REVOLUTION 5000.0
#define DISTANCE_BETWEEN_WHEELS_MM 228.0

class Fusion {
  public:
    static FusionData getData(SensorData sensorData) {
      // Get orientation
      FusionData fusionData;

      // Get position
      double leftDistanceMm = (sensorData.leftPulses - previousSensorData.leftPulses) * TRAVEL_PER_REVOLUTION_MM / TICKS_PER_REVOLUTION;
      double rightDistanceMm = (sensorData.rightPulses - previousSensorData.rightPulses) * TRAVEL_PER_REVOLUTION_MM / TICKS_PER_REVOLUTION;
      double deltaDistanceMm = (leftDistanceMm + rightDistanceMm) / 2.0;

      double deltaOrientation = (leftDistanceMm - rightDistanceMm) / DISTANCE_BETWEEN_WHEELS_MM * (180.0 / M_PI);
      double orientation = previousFusionData.orientation + deltaOrientation;
      double averageOrientation = previousFusionData.orientation + deltaOrientation / 2.0;

      double deltaX = deltaDistanceMm * sin(averageOrientation * DEG_TO_RAD);
      double deltaY = deltaDistanceMm * cos(averageOrientation * DEG_TO_RAD);

      // Get toolhead position
      double toolheadDeltaX = (sensorData.xAxisPulses - previousSensorData.xAxisPulses) * X_AXIS_MM_PER_REVOLUTION / N20_TICKS_PER_REVOLUTION;
      double toolheadDeltaZ = (sensorData.zAxisPulses - previousSensorData.zAxisPulses) * Z_AXIS_MM_PER_REVOLUTION / N20_TICKS_PER_REVOLUTION;

      // Get wheels position
      double wheelsDelta = (sensorData.wheelsPulses - previousSensorData.wheelsPulses) * WHEELS_MM_PER_REVOLUTION / N20_TICKS_PER_REVOLUTION;

      // Get mill position
      double millDelta = (sensorData.millPulses - previousSensorData.millPulses) / N20_TICKS_PER_REVOLUTION * 360.0;

      // Get voltage
      double averageVoltage = sensorData.bms3 * 2.0;

      // Get color
      fusionData.color = rgbToColor(sensorData.colorData);

      // Construct fusion data
      fusionData.orientation = orientation;
      fusionData.position = Vector3(previousFusionData.position.x + deltaX, previousFusionData.position.y + deltaY, 0);
      fusionData.toolheadPosition = Vector3(previousFusionData.toolheadPosition.x + toolheadDeltaX, previousFusionData.toolheadPosition.y + toolheadDeltaZ);
      fusionData.wheelsPosition = Vector3(previousFusionData.wheelsPosition.x + wheelsDelta);
      fusionData.millPosition = Vector3(previousFusionData.millPosition.x + millDelta);
      fusionData.voltage = averageVoltage;

      // Set previous data
      previousSensorData = sensorData;
      previousFusionData = fusionData;

      return fusionData;
    }

    static void homingComplete() {
      previousFusionData.toolheadPosition = Vector3();
    }

  private:
    static float deltat;
    static SF fusion;

    static FusionData previousFusionData;
    static SensorData previousSensorData;
    static ColorData colorValues[5];

    static Color rgbToColor(ColorData colorData) {
      std::vector<int> differences;

      for (ColorData compared : colorValues) {
        int differenceR = std::abs(colorData.r - compared.r);
        int differenceG = std::abs(colorData.g - compared.g);
        int differenceB = std::abs(colorData.b - compared.b);
        differences.push_back(differenceR + differenceG + differenceB);
      }

      // Get min difference
      int minIndex = 0;
      int minValue = 255;

      for (int i = 0; i < differences.size(); i++) {
        int value = differences[i];
        if (value >= minValue) continue;

        minValue = value;
        minIndex = i;
      }

      // Return closest match
      return static_cast<Color>(minIndex);
    }
};

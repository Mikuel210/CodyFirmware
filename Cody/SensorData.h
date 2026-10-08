#pragma once
#include "Vector3.h"
#include "ColorData.h"

struct SensorData {
  // IMU
  Vector3 acceleration;
  Vector3 gyroscope;
  Vector3 magnetometer;

  // Encoders
  int leftPulses;
  int rightPulses;
  int xAxisPulses;
  int zAxisPulses;
  int wheelsPulses;
  int millPulses;

  // BMS
  double bms1;
  double bms2;
  double bms3;

  // Switches
  bool xLimit;
  bool zLimit;
  bool button;

  // Color sensor
  ColorData colorData;
};

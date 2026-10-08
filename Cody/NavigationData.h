#pragma once
#include "Vector3.h"

struct MotorData {
  MotorData(bool forwards_, int pwm_) : forwards(forwards_), pwm(pwm_) {}
  MotorData() = default;

  bool forwards;
  int pwm;
};

struct NavigationData {
  MotorData leftMotor;
  MotorData rightMotor;
};
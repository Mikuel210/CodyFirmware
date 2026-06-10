#pragma once
#include <algorithm>
#include <cmath>
#include "Vector3.h"
#include "NavigationTarget.h"
#include "NavigationData.h"
#include "ToolheadData.h"
#include "WheelsData.h"
#include "MillData.h"
#include "FusionData.h"
#include "PID.h"

// Navigation parameters
#define ERROR_DECELERATION 0.0025

class Navigation {
  public:
    static NavigationTarget drive;
    static NavigationTarget toolhead;
    static NavigationTarget wheels;
    static NavigationTarget mill;

    static NavigationData getData(FusionData fusionData, double speed) {
      NavigationData navigationData;

      // Get orientation correction
      float targetOrientation = atan2(drive.steeringTarget.x - fusionData.position.x, drive.steeringTarget.y - fusionData.position.y) * (180.0 / M_PI);
      float error = targetOrientation - fusionData.orientation;

      while (error > 180.0) { error -= 360.0; targetOrientation -= 360.0; }
      while (error < -180.0) { error += 360.0; targetOrientation += 360.0; }

      // Get distance correction
      double distance = getDistance(fusionData.position, drive.target);
      double orientationCorrection = -orientationPid.getCorrection(error);

      double distanceAuthority = std::clamp(1.0 - std::abs(orientationCorrection) * ERROR_DECELERATION, 0.0, 1.0);
      double distancePwm = 255.0 / 2.0 * distanceAuthority;

      // Construct navigation data
      navigationData.leftMotor = getMotorData(distancePwm + orientationCorrection, speed);
      navigationData.rightMotor = getMotorData(distancePwm - orientationCorrection, speed);

      return navigationData;
    }

    static ToolheadData getToolheadData(FusionData fusionData, double speed) {
      ToolheadData toolheadData;

      // Get toolhead correction
      xAxisPid.setSetpoint(toolhead.target.x);
      zAxisPid.setSetpoint(toolhead.target.z);

      double xCorrection = xAxisPid.getCorrection(fusionData.toolheadPosition.x);
      double zCorrection = zAxisPid.getCorrection(fusionData.toolheadPosition.z);

      double xAxisPwm = correctionToPwm(xCorrection, toolhead);
      double zAxisPwm = correctionToPwm(zCorrection, toolhead);

      // Construct toolhead data
      toolheadData.xAxisMotor = getMotorData(xAxisPwm, speed);
      toolheadData.zAxisMotor = getMotorData(zAxisPwm, speed);

      return toolheadData;
    }

    static WheelsData getWheelsData(FusionData fusionData, double speed) {
      WheelsData wheelsData;

      // Get wheels correction
      wheelsPid.setSetpoint(wheels.target.z);
      double pwm = correctionToPwm(wheelsPid.getCorrection(fusionData.wheelsPosition.z), wheels);

      // Construct wheels data
      wheelsData.wheelsMotor = getMotorData(pwm, speed);
      return wheelsData;
    }

    static MillData getMillData(FusionData fusionData, double speed) {
      MillData millData;

      // Get mill correction
      millPid.setSetpoint(mill.target.x);
      double pwm = correctionToPwm(millPid.getCorrection(fusionData.millPosition.z), mill);

      // Construct mill data
      millData.millMotor = getMotorData(pwm, speed);
      return millData;
    }

    static double getDistance(Vector3 a, Vector3 b) {
      double dx = b.x - a.x;
      double dy = b.y - a.y;
      return sqrt(dx * dx + dy * dy);
    }

    static double dmap(double x, double in_min, double in_max, double out_min, double out_max) {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

  private:
    static PID orientationPid;
    static PID distancePid;
    static PID xAxisPid;
    static PID zAxisPid;
    static PID wheelsPid;
    static PID millPid;

    static double correctionToPwm(double correction, NavigationTarget target) {
      return std::clamp(correction * 255.0 / target.decelerationDistance, -255.0, 255.0);
    }

    static MotorData getMotorData(double pwm, double speed, int minPwm = 0) {
      pwm = std::clamp(pwm * speed, -255.0, 255.0);
      
      if (pwm < minPwm && pwm >= 0) pwm = minPwm;
      if (pwm > -minPwm && pwm < 0) pwm = -minPwm;

      return MotorData(pwm >= 0.0, std::abs(pwm));
    }
};

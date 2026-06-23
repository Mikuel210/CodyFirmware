#pragma once
#include "IHardwareProvider.h"
#include "GPIO.h"
#include "Cody.h"
#include <Arduino.h>
#include <Wire.h>

// PWM
#define L_PWM 2
#define R_PWM 4
#define A1_PWM 16 // X
#define A2_PWM 17 // Z
#define A3_PWM 18 // Wheels
#define A4_PWM 19 // Mill

// Direction
#define L_IN_1 0
#define L_IN_2 1
#define R_IN_1 2
#define R_IN_2 3
#define A1_IN_1 4
#define A1_IN_2 5
#define A2_IN_1 6
#define A2_IN_2 7
#define A3_IN_1 8
#define A3_IN_2 9
#define A4_IN_1 10
#define A4_IN_2 11

// Max switch speed
#define MIN_DIRECTION_SWITCH_MS 500.0
#define MAX_PWM_CHANGE 

// State indication
#define LED 12
#define BUZZER 5

class RobotHardwareProvider : public IHardwareProvider {
  public:
    void initialize() override {
      pinMode(L_PWM, OUTPUT);
      pinMode(R_PWM, OUTPUT);
      pinMode(A1_PWM, OUTPUT);
      pinMode(A2_PWM, OUTPUT);
      pinMode(A3_PWM, OUTPUT);
      pinMode(A4_PWM, OUTPUT);
      pinMode(BUZZER, OUTPUT);
    }

    void move(NavigationData navigationData) override {
      moveMotor(navigationData.rightMotor, L_IN_1, L_IN_2, L_PWM);
      moveMotor(navigationData.leftMotor, R_IN_1, R_IN_2, R_PWM);
    }


    int xAxisPwm = 0;
    bool xAxisForwards = false;
    bool xAxisDirectionChange = 0;

    int zAxisPwm = 0;
    bool zAxisForwards = false;
    bool zAxisDirectionChange = 0;

    void moveToolhead(ToolheadData toolheadData) override {
      toolheadData.zAxisMotor.forwards = !toolheadData.zAxisMotor.forwards;

      if ()
      
      if (toolheadData.xAxisMotor.forwards != xAxisForwards && (millis() - xAxisDirectionChange) < MIN_DIRECTION_CHANGE_MS)
      {
        toolheadData.xAxisMotor.forwards = xAxisForwards;
      } 
      else
      {
        xAxisForwards = toolheadData.xAxisMotor.forwards;
        xAxisDirectionChange = millis();
      }

      if (toolheadData.zAxisMotor.forwards != zAxisForwards && (millis() - zAxisDirectionChange) < MIN_DIRECTION_CHANGE_MS)
      {
        toolheadData.zAxisMotor.forwards = zAxisForwards;
      } 
      else
      {
        zAxisForwards = toolheadData.zAxisMotor.forwards;
        zAxisDirectionChange = millis();
      }

      moveMotor(toolheadData.xAxisMotor, A1_IN_1, A1_IN_2, A1_PWM);
      moveMotor(toolheadData.zAxisMotor, A2_IN_1, A2_IN_2, A2_PWM);
    }

    void moveWheels(WheelsData wheelsData) override {
      moveMotor(wheelsData.wheelsMotor, A3_IN_1, A3_IN_2, A3_PWM);
    }

    void moveMill(MillData millData) override {
      moveMotor(millData.millMotor, A4_IN_1, A4_IN_2, A4_PWM);
    }

    void writeLed(uint8_t value) override {
      GPIO::digitalWrite(LED, value);
    }

    void toneBuzzer(unsigned int frequency, unsigned long duration = 0) override {
      if (duration == 0) tone(BUZZER, frequency);
      else tone(BUZZER, frequency, duration);
    }

    void noToneBuzzer() override {
      noTone(BUZZER);
    }

  private:
    void moveMotor(MotorData motorData, unsigned int in1, unsigned int in2, unsigned int pwm) {
      if (motorData.forwards) {
        GPIO::digitalWrite(in1, HIGH);
        GPIO::digitalWrite(in2, LOW);
      } else {
        GPIO::digitalWrite(in1, LOW);
        GPIO::digitalWrite(in2, HIGH);
      }

      analogWrite(pwm, motorData.pwm);
    }
};

#pragma once
#include "IHardwareProvider.h"
#include "Task.h"
#include "WheelsData.h"
#include <ArduinoJson.h>

class SimulationHardwareProvider : public IHardwareProvider {
  public:
    void move(NavigationData navigationData) override {
      JsonDocument document;

      document["method"] = "Move";
      document["parameters"][0] = navigationData.leftMotor.forwards;
      document["parameters"][1] = navigationData.leftMotor.pwm;
      document["parameters"][2] = navigationData.rightMotor.forwards;
      document["parameters"][3] = navigationData.rightMotor.pwm;

      send(document);
    }

    void moveToolhead(ToolheadData toolheadData) override {
      JsonDocument document;

      document["method"] = "MoveToolhead";
      document["parameters"][0] = toolheadData.xAxisMotor.forwards;
      document["parameters"][1] = toolheadData.xAxisMotor.pwm;
      document["parameters"][2] = toolheadData.zAxisMotor.forwards;
      document["parameters"][3] = toolheadData.zAxisMotor.pwm;

      send(document);
    }

    void moveWheels(WheelsData wheelsData) override {
      JsonDocument document;

      document["method"] = "MoveWheels";
      document["parameters"][0] = wheelsData.wheelsMotor.forwards;
      document["parameters"][1] = wheelsData.wheelsMotor.pwm;

      send(document);
    }

    void moveMill(MillData millData) override {
      JsonDocument document;

      document["method"] = "MoveMill";
      document["parameters"][0] = millData.millMotor.forwards;
      document["parameters"][1] = millData.millMotor.pwm;

      send(document);
    }

    void writeLed(uint8_t value) override {
      JsonDocument document;

      document["method"] = "WriteLed";
      document["parameters"][0] = value == HIGH;

      send(document);
    }

    void toneBuzzer(unsigned int frequency, unsigned long duration = 0) override {
      JsonDocument document;

      document["method"] = "ToneBuzzer";
      document["parameters"][0] = frequency;
      document["parameters"][1] = duration;

      send(document);
    }

    void noToneBuzzer() override {
      JsonDocument document;
      document["method"] = "NoToneBuzzer";

      send(document);
    }

  private:
    void send(JsonDocument document) {
      serializeJson(document, Serial);
      Serial.println();
    }
};

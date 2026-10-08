#pragma once
#include "IDataProvider.h"
#include "Vector3.h"
#include <Arduino.h>
#include <ArduinoJson.h>

class SimulationDataProvider : public IDataProvider {
  public:
    SensorData getData() override {
      SensorData data;
      JsonDocument document;
      if (!Serial.available()) return previousData;

      // Deserialize incoming data
      String json = Serial.readStringUntil('\n');
      DeserializationError error = deserializeJson(document, json);
      if (error) return previousData;

      // IMU
      data.acceleration = Vector3(document["acceleration"][0], document["acceleration"][1], document["acceleration"][2]);
      data.gyroscope = Vector3(document["gyroscope"][0], document["gyroscope"][1], document["gyroscope"][2]);
      data.magnetometer = Vector3(document["magnetometer"][0], document["magnetometer"][1], document["magnetometer"][2]);

      // Encoders
      data.leftPulses = document["leftPulses"];
      data.rightPulses = document["rightPulses"];
      data.xAxisPulses = document["xAxisPulses"];
      data.zAxisPulses = document["zAxisPulses"];
      data.wheelsPulses = document["wheelsPulses"];
      data.millPulses = document["millPulses"];

      // BMS
      data.bms1 = document["bms1"];
      data.bms2 = document["bms2"];
      data.bms3 = document["bms3"];

      // Switches
      data.xLimit = document["xLimit"];
      data.zLimit = document["zLimit"];
      data.button = document["button"];

      // Color
      data.colorData = ColorData(document["r"], document["g"], document["b"]);

      previousData = data;
      return data;
    }

  private:
    SensorData previousData;
};

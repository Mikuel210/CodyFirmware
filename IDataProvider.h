#pragma once
#include "SensorData.h"
#include <Arduino.h>

class IDataProvider {
  public:
    virtual ~IDataProvider() {}
    
    virtual void initialize() {}
    virtual SensorData getData() = 0;
    virtual SensorData getPulses() = 0;
    virtual SensorData getButtons() = 0;
};

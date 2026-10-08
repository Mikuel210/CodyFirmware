#pragma once
#include "NavigationData.h"
#include "ToolheadData.h"
#include "WheelsData.h"
#include "MillData.h"

class IHardwareProvider {
  public:
    virtual ~IHardwareProvider() {}
    virtual void initialize() {}

    virtual void move(NavigationData navigationData) {}
    virtual void moveToolhead(ToolheadData toolheadData) {}
    virtual void moveWheels(WheelsData wheelsData) {}
    virtual void moveMill(MillData millData) {}

    virtual void writeLed(uint8_t value) {}
    virtual void toneBuzzer(unsigned int frequency, unsigned long duration = 0) {}
    virtual void noToneBuzzer() {}
};

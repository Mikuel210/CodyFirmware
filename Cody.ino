// Global parameters
// #define SIMULATION
#define BAUD_RATE 115200

// Include dependencies
#include "Cody.h"
#include "BMS.h"
#include "Program.h"

#ifdef SIMULATION
  #include "SimulationDataProvider.h"
  #include "SimulationHardwareProvider.h"
  SimulationDataProvider dataProvider;
  SimulationHardwareProvider hardwareProvider;
#else
  #include "SensorDataProvider.h"
  #include "RobotHardwareProvider.h"
  SensorDataProvider dataProvider;
  RobotHardwareProvider hardwareProvider;
#endif

// Setup
void setup() {
  Serial.begin(BAUD_RATE);
  delay(500);
  
  dataProvider.initialize();
  hardwareProvider.initialize();
  Cody::initialize(dataProvider, hardwareProvider);
  BMS::initialize();

  waitForButton();
  Program::go();
}

void waitForButton() {
  SensorData sensorData;

  while (!sensorData.button) {
    sensorData = dataProvider.getData();
    delay(10);
  }
}

void loop() {}
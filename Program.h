#pragma once
#include "Task.h"
#include "Cody.h"

#pragma region Program Parameters

// Note: 0, 0 is the center of the start area

// General
#define START_X 4
#define ALIGN_SET_X 48
#define ALIGN_SET_Y -48
#define ALIGN_DISTANCE 500
#define ALIGN_SPEED 40
#define ALIGN_MS 2000

// Blocks
#define FIRST_GROUP_WALL_X_MM -170
#define BLOCK_GROUPS_INCREMENT -160
#define BLOCK_DISTANCE_START 64
#define BLOCK_DISTANCE_MOSAIC 50
#define BLOCK_HEIGHT 30

// Toolhead
#define TOOLHEAD_UP 40
#define TOOLHEAD_DOWN 0
#define TOOLHEAD_PICK_START_X 50
#define TOOLHEAD_LEAVE_START_X 80
#define COLOR_Y_OFFSET 87

// Map
#define MOSAIC_X -510
#define MOSAIC_Y 850
#define MOSAIC_X_OFFSET 30
#define BLOCKS_LINE_DETECT_Y 300
#define FIRST_LINE_X -275

#pragma endregion


class Program {
  public:
    static void go() {
      Fusion::restart();
      items();
      cement();
    }

  private:
    static Task* moveTask;
    static Task* toolheadTask;
    static Task* millTask;

    static void items() {
      // Align
      align(0, -ALIGN_DISTANCE);
      Cody::setPosition(START_X, ALIGN_SET_Y, 0);

      Cody::addPathPoint(START_X, 400);
      Cody::addPathPoint(-250, 400);
      Cody::followPathAsync(50, false)->await();

      align(ALIGN_DISTANCE, 400, 2000);
      Cody::setXOrientation(ALIGN_SET_X, -90);

      // Carry 1st item
      Cody::addPathPoint(-60, 400);
      Cody::addPathPoint(-60, 1000);
      Cody::followPathAsync(30)->await();

      Cody::addPathPoint(0, 800);
      Cody::addPathPoint(25, 600);
      Cody::followPathAsync(30, true, 50, 100)->await();

      // Carry 2nd item
      Cody::addPathPoint(45, 800);
      Cody::addPathPoint(45, 1000);
      Cody::addPathPoint(-40, 1200);
      Cody::addPathPoint(-40, 1500);
      Cody::addPathPoint(20, 1600);
      Cody::addPathPoint(20, 1650);
      Cody::followPathAsync(20, false, 100, 150)->await();

      Cody::hardwareProvider->move({{false, 0}, {true, 25}});

      while (true) {
        SensorData sensorData = Cody::dataProvider->getPulses();
        FusionData fusionData = Fusion::getData(sensorData);

        float targetOrientation = -185;
        float error = targetOrientation - fusionData.orientation;
        while (error > 180.0) { error -= 360.0; targetOrientation -= 360.0; }
        while (error < -180.0) { error += 360.0; targetOrientation += 360.0; }

        if (error > -1.0 && error < 1.0) break;
        delay(10);
      }

      Cody::hardwareProvider->move({{false, 0}, {true, 0}});

      // Carry 3rd item
      Cody::moveMillMsAsync(250);
      Cody::addPathPoint(30, 1500);
      Cody::followPathAsync(30, true)->await();

      Cody::rotateToAsync(-30, 15)->await();

      Cody::addPathPoint(45, 1300);
      Cody::addPathPoint(45, 1200);
      Cody::addPathPoint(-65, 700);
      Cody::addPathPoint(-100, 500);
      Cody::addPathPoint(-150, 350);
      Cody::addPathPoint(-150, 150);
      Cody::followPathAsync(30, true)->await();

      // Go to white
      Cody::addPathPoint(-100, 550);
      Cody::addPathPoint(-15, 725);
      Cody::addPathPoint(-15, 1250);
      Cody::addPathPoint(-30, 1400);
      Cody::addPathPoint(-30, 1525);
      moveTask = Cody::followPathAsync();

      delay(1000);
      Cody::moveMillMsAsync(900)->await();
      moveTask->await();

      Cody::addPathPoint(-30, 1600);
      Cody::detectColorAsync(250)->await();
    }

    static void cement() {
      Cody::setY(1665 - COLOR_Y_OFFSET);

      // Align
      Cody::addPathPoint(0, 1700);
      Cody::addPathPoint(-100, 1700);
      Cody::followPathAsync(30, false, 50, 75, 50)->await();

      align(500, 1700, 1500);
      Cody::setXOrientation(ALIGN_SET_X, -90);

      // Pick white
      Cody::moveAsync(0, 1700)->await();
      Cody::rotateToAsync(180)->await();
      pick(0);

      // Leave white
      double center = 1015;
      Cody::addPathPoint(0, center);
      Cody::followPathAsync()->await();

      Cody::rotateToAsync(90)->await();
      
      Cody::addPathPoint(-550, center);
      Cody::followPathAsync(40, true)->await();

      Cody::moveMillMsAsync(800, true)->await();
      delay(100);
      Cody::moveMillMsAsync(50)->await();

      // Pick blue
      Cody::addPathPoint(-10, center);
      Cody::moveAsync(-50, center)->await();
      Cody::rotateToAsync(-90)->await();
      
      align(ALIGN_DISTANCE, center);
      Cody::setXOrientation(ALIGN_SET_X, -90);

      Cody::addPathPoint(-25, 1005);
      Cody::addPathPoint(-25, 1500);
      Cody::followPathAsync()->await();

      Cody::addPathPoint(-25, 1600);
      Cody::detectColorAsync(250)->await();
      Cody::setY(1665 - COLOR_Y_OFFSET);

      Cody::addPathPoint(-25, 1700);
      Cody::addPathPoint(-200, 1700);
      Cody::followPathAsync()->await();

      Cody::addPathPoint(-400, 1700);
      Cody::detectColorAsync(250)->await();
      Cody::setX(-250 + COLOR_Y_OFFSET);

      Cody::rotateToAsync(-180)->await();
      pick(-250);

      // Leave blue
      Cody::addPathPoint(-250, 1700);
      Cody::addPathPoint(-500, 1700);
      Cody::addPathPoint(-500, 900);
      Cody::followPathAsync(40)->await();

      Cody::moveMillMsAsync(800, true)->await();
      delay(100);
      Cody::moveMillMsAsync(50)->await();

      // Pick green
      Cody::addPathPoint(-500, 900);
      Cody::addPathPoint(-400, 1300);
      Cody::addPathPoint(-400, 1700);
      Cody::followPathAsync(40, true);

      Cody::rotateToAsync(-90);

      Cody::addPathPoint(-700, 1700);
      Cody::detectColorAsync(250)->await();
      Cody::setX(-500 + COLOR_Y_OFFSET);

      Cody::rotateToAsync(-180)->await();
      pick(-500);

      // Leave green
      Cody::moveAsync(-500, 1100)->await();

      Cody::moveMillMsAsync(800, true)->await();
      delay(100);
      Cody::moveMillMsAsync(50)->await();

      // Pick yellow
      Cody::addPathPoint(-500, 1700);
      Cody::followPathAsync(40, true)->await();

      align(-2000, 1700, 2500);
      Cody::setXOrientation(-968, 90);

      Cody::moveAsync(-750, 1700)->await();
      Cody::rotateToAsync(-180)->await();
      pick(-750);

      // Leave yellow
      Cody::addPathPoint(-750, 1500);
      Cody::addPathPoint(0, 1500);
      Cody::addPathPoint(0, center);
      Cody::addPathPoint(300, center);
      Cody::followPathAsync()->await();
      
      Cody::moveMillMsAsync(800, true)->await();
      delay(100);
      Cody::moveMillMsAsync(50)->await();
    }

    static void pick(double x) {
      millTask = Cody::moveMillMsAsync(500, false, 15.0);
      Cody::addPathPoint(x, 2105);

      moveTask = Cody::followPathAsync(60, true, 100, 250, 50);
      unsigned int msStart = millis();

      while (!(*(moveTask->finished)) && (millis() - msStart) < 1500)
        delay(10);

      delete moveTask->requestStop;
      moveTask->requestStop = new bool(true);
      moveTask->await();
      delete moveTask->requestStop;

      millTask->await();
      millTask = Cody::moveMillMsAsync(750, false);
      moveTask = Cody::moveAsync(x, 1900, 13);

      millTask->await();
      moveTask->await();
    }

    static void align(double x, double y, int ms = ALIGN_MS, double speed = ALIGN_SPEED, double lookaheadDistance = MOVEMENT_LOOKAHEAD, 
      double transitionLookahead = TRANSITION_LOOKAHEAD, double decelerationMm = MOVEMENT_DECELERATION_MM) {

      Cody::addPathPoint(x, y);
      moveTask = Cody::followPathAsync(speed, true, lookaheadDistance, transitionLookahead, decelerationMm);
      delay(ms);

      delete moveTask->requestStop;
      moveTask->requestStop = new bool(true);
      moveTask->await();
    }
};

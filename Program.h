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
      cement();
    }

  private:
    static Task* moveTask;
    static Task* toolheadTask;
    static Task* millTask;

    static void cement() {
      // Align
      align(0, -ALIGN_DISTANCE);
      Cody::setPosition(START_X, ALIGN_SET_Y, 0);

      Cody::addPathPoint(START_X, 400);
      Cody::addPathPoint(-250, 400);
      Cody::followPathAsync(50, false)->await();

      align(ALIGN_DISTANCE, 400, 2000);
      Cody::setXOrientation(ALIGN_SET_X, -90);

      // Carry 1st thingy
      Cody::addPathPoint(-65, 400);
      Cody::addPathPoint(-65, 1000);
      Cody::followPathAsync(30)->await();

      Cody::addPathPoint(0, 800);
      Cody::addPathPoint(25, 600);
      Cody::followPathAsync(20, true, 50, 100)->await();

      // Carry 2nd thingy
      Cody::addPathPoint(35, 800);
      Cody::addPathPoint(35, 1000);
      Cody::addPathPoint(-40, 1200);
      Cody::addPathPoint(-40, 1500);
      Cody::followPathAsync(20)->await();

      Cody::rotateToAsync(-90, 15)->await();

      // Carry 3rd thingy
      Cody::addPathPoint(0, 1500);
      Cody::followPathAsync(20, true)->await();

      Cody::rotateToAsync(-30, 15)->await();

      Cody::addPathPoint(25, 1300);
      Cody::addPathPoint(35, 1200);
      Cody::addPathPoint(-65, 700);
      Cody::addPathPoint(-100, 500);
      Cody::addPathPoint(-150, 350);
      Cody::addPathPoint(-100, 150);
      Cody::followPathAsync(20, true)->await();
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

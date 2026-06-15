#pragma once
#include "IDataProvider.h"
#include "IHardwareProvider.h"
#include "SensorData.h"
#include "FusionData.h"
#include "Fusion.h"
#include "Navigation.h"
#include "NavigationTarget.h"
#include "NavigationData.h"
#include "ToolheadData.h"
#include "WheelsData.h"
#include "MillData.h"
#include "Pursuit.h"
#include "PursuitData.h"
#include "Task.h"
#include "TaskArgs.h"
#include "Plotter.h"
#include <vector>

// Task parameters
#define HZ 50.0

// Movement
#define MOVEMENT_ACCELERATION_MS 500.0
#define MOVEMENT_DECELERATION_MM 250.0
#define MOVEMENT_MIN_SPEED 10.0
#define MOVEMENT_LOOKAHEAD 100.0
#define TRANSITION_LOOKAHEAD 250.0

// Toolhead
#define TOOLHEAD_ACCELERATION_MS 100.0
#define TOOLHEAD_DECELERATION_MM 25.0
#define TOOLHEAD_MIN_SPEED 60.0
#define TOOLHEAD_LOOKAHEAD 10.0

// Wheels
#define WHEELS_ACCELERATION_MS 100.0
#define WHEELS_DECELERATION_MM 10.0
#define WHEELS_MIN_SPEED 30.0
#define WHEELS_LOOKAHEAD 10.0

// Mill
#define MILL_ACCELERATION_MS 100.0
#define MILL_DECELERATION_MM 10.0
#define MILL_MIN_SPEED 30.0
#define MILL_LOOKAHEAD 25.0

class Cody {
  public:
    static IDataProvider* dataProvider;
    static IHardwareProvider* hardwareProvider;

    static void initialize(IDataProvider& dataProvider_, IHardwareProvider& hardwareProvider_) {
      dataProvider = &dataProvider_;
      hardwareProvider = &hardwareProvider_;
    }

    // Drive
    static Task* moveAsync(double x, double y, double speed = 50, double lookaheadDistance = MOVEMENT_LOOKAHEAD) {
      addPathPoint(x, y);
      return followPathAsync(speed, lookaheadDistance);
    }

    static void addPathPoint(double x, double y) {
      Vector3 point = Vector3(x, y);
      pathData.points.push_back(point);
    }

    static Task* followPathAsync(double speed = 50, double lookaheadDistance = MOVEMENT_LOOKAHEAD) {
      Task* task = new Task("followPath", followPathTask);
      FollowPathArgs* args = new FollowPathArgs();
      pathData.lookaheadDistance = lookaheadDistance;

      args->task = task;
      args->data = &pathData;
      args->speed = speed / 100.0;
      args->minSpeed = MOVEMENT_MIN_SPEED / 100.0;
      args->accelerationMs = MOVEMENT_ACCELERATION_MS;
      args->decelerationMm = MOVEMENT_DECELERATION_MM;
      args->transitionLookahead = TRANSITION_LOOKAHEAD;
      args->positionMember = &FusionData::position;
      args->navigationTarget = &Navigation::drive;
      args->moveFunction = &moveRobot;
      args->stopFunction = &stopRobot;

      task->start(args);
      return task;
    }

    // Toolhead
    static Task* moveToolheadAsync(double x, double z, double speed = 100, double lookaheadDistance = TOOLHEAD_LOOKAHEAD) {
      addToolheadPathPoint(x, z);
      return followToolheadPathAsync(speed, lookaheadDistance);
    }

    static void addToolheadPathPoint(double x, double z) {
      Vector3 point = Vector3(x, z);
      toolheadPathData.points.push_back(point);
    }

    static Task* followToolheadPathAsync(double speed = 100, double lookaheadDistance = TOOLHEAD_LOOKAHEAD) {
      FollowPathArgs* args = new FollowPathArgs();
      Task* task = new Task("followToolheadPath", followPathTask);
      toolheadPathData.lookaheadDistance = lookaheadDistance;

      args->task = task;
      args->data = &toolheadPathData;
      args->speed = speed / 100.0;
      args->minSpeed = TOOLHEAD_MIN_SPEED / 100.0;
      args->accelerationMs = TOOLHEAD_ACCELERATION_MS;
      args->decelerationMm = TOOLHEAD_DECELERATION_MM;
      args->transitionLookahead = lookaheadDistance;
      args->positionMember = &FusionData::toolheadPosition;
      args->navigationTarget = &Navigation::toolhead;
      args->moveFunction = &moveToolhead;
      args->stopFunction = &stopToolhead;

      task->start(args);
      return task;
    }

    static Task* homeAsync(double speed = 100) {
      HomeArgs* args = new HomeArgs();
      Task* task = new Task("homeTask", homeTask);

      args->task = task;
      args->speed = speed / 100.0;

      task->start(args);
      return task;
    }

    // Wheels
    static Task* moveWheelsAsync(double z, double speed = 100, double lookaheadDistance = WHEELS_LOOKAHEAD) {
      addWheelsPathPoint(z);
      return followWheelsPathAsync(speed, lookaheadDistance);
    }

    static void addWheelsPathPoint(double z) {
      Vector3 point = Vector3(z);
      wheelsPathData.points.push_back(point);
    }

    static Task* followWheelsPathAsync(double speed = 100, double lookaheadDistance = WHEELS_LOOKAHEAD) {
      FollowPathArgs* args = new FollowPathArgs();
      Task* task = new Task("followWheelsPath", followPathTask);
      wheelsPathData.lookaheadDistance = lookaheadDistance;

      args->task = task;
      args->data = &wheelsPathData;
      args->speed = speed / 100.0;
      args->minSpeed = WHEELS_MIN_SPEED / 100.0;
      args->accelerationMs = WHEELS_ACCELERATION_MS;
      args->decelerationMm = WHEELS_DECELERATION_MM;
      args->transitionLookahead = lookaheadDistance;
      args->positionMember = &FusionData::wheelsPosition;
      args->navigationTarget = &Navigation::wheels;
      args->moveFunction = &moveWheels;
      args->stopFunction = &stopWheels;

      task->start(args);
      return task;
    }

    // Mill
    static Task* moveMillAsync(double z, double speed = 100, double lookaheadDistance = MILL_LOOKAHEAD) {
      addMillPathPoint(z);
      return followMillPathAsync(speed, lookaheadDistance);
    }

    static void addMillPathPoint(double z) {
      Vector3 point = Vector3(z);
      millPathData.points.push_back(point);
    }

    static Task* followMillPathAsync(double speed = 100, double lookaheadDistance = MILL_LOOKAHEAD) {
      FollowPathArgs* args = new FollowPathArgs();
      Task* task = new Task("followMillPath", followPathTask);
      millPathData.lookaheadDistance = lookaheadDistance;

      args->task = task;
      args->data = &millPathData;
      args->speed = speed / 100.0;
      args->minSpeed = MILL_MIN_SPEED / 100.0;
      args->accelerationMs = MILL_ACCELERATION_MS;
      args->decelerationMm = MILL_DECELERATION_MM;
      args->transitionLookahead = lookaheadDistance;
      args->positionMember = &FusionData::millPosition;
      args->navigationTarget = &Navigation::mill;
      args->moveFunction = &moveMill;
      args->stopFunction = &stopMill;

      task->start(args);
      return task;
    }

    // LED
    static void writeLed(uint8_t value) {
      hardwareProvider->writeLed(value);
    }

    // Rotation
    static Task* rotateToAsync(double heading, double speed = 50) {
      Task* task = new Task("rotate", rotateTask);
      RotateArgs* args = new RotateArgs();

      args->task = task;
      args->heading = heading;
      args->speed = speed / 100.0;

      task->start(args);
      return task;
    }

  private:
    static PursuitData pathData;
    static PursuitData toolheadPathData;
    static PursuitData wheelsPathData;
    static PursuitData millPathData;

    using PositionMember = Vector3 (FusionData::*);
    using MoveFunction = void (*)(FusionData, double);
    using StopFunction = void (*)();

    // Follow path
    struct FollowPathArgs : TaskArgs {
      PursuitData* data;

      double speed;
      double minSpeed;
      double accelerationMs;
      double decelerationMm;
      double transitionLookahead;

      PositionMember positionMember;
      NavigationTarget* navigationTarget;
      MoveFunction moveFunction;
      StopFunction stopFunction;
    };

    static void followPathTask(void* task) {
      FollowPathArgs* args = (FollowPathArgs*)task;
      PursuitData* data = args->data;
      
      // Set first point
      SensorData sensorData = dataProvider->getData();
      FusionData fusionData = Fusion::getData(sensorData);
      args->navigationTarget->decelerationDistance = data->lookaheadDistance;
      
      data->lineIndex = 0;
      data->points.insert(data->points.begin(), fusionData.*(args->positionMember));
      
      // Get last segment
      int pointCount = data->points.size();
      Line lastSegment(data->points[pointCount - 2], data->points[pointCount - 1]);
      
      // Transition data
      PursuitData* transitionData = new PursuitData(data->points, args->transitionLookahead, data->lineIndex);
      Vector3 currentTransitionPoint;
      double currentTransitionTime;
      int currentTransitionLineIndex;

      unsigned long msStart = millis();

      while (true) {
        unsigned long msLoop = millis();

        SensorData sensorData = dataProvider->getData();
        FusionData fusionData = Fusion::getData(sensorData);
        Vector3 position = fusionData.*(args->positionMember);

        // Find lookahead and transition points
        Vector3 lookaheadPoint = Pursuit::findLookahead(position, data, true);
        Line lookaheadLine = { data->points[data->lineIndex], data->points[data->lineIndex + 1] };
        double lookaheadTime = Pursuit::findLookaheadTime(position, lookaheadLine, data->lookaheadDistance);

        Vector3 transitionPoint = Pursuit::findLookahead(position, transitionData, true);
        Line transitionLine = { data->points[transitionData->lineIndex], data->points[transitionData->lineIndex + 1] };
        double transitionTime = Pursuit::findLookaheadTime(position, transitionLine, transitionData->lookaheadDistance);

        if (transitionData->lineIndex > currentTransitionLineIndex) {
          currentTransitionPoint = transitionPoint;
          currentTransitionTime = transitionTime;
          currentTransitionLineIndex = transitionData->lineIndex;
        }

        if (data->lineIndex == currentTransitionLineIndex && lookaheadTime > currentTransitionTime)
          args->navigationTarget->target = lookaheadPoint;
        else
          args->navigationTarget->target = currentTransitionPoint;

        // Debug
        Plotter::setLimits(0.0, 1000.0);
        Plotter::plot("lx", lookaheadPoint.x);
        Plotter::plot("ly", lookaheadPoint.y);
        Plotter::plot("lz", lookaheadPoint.z);
        Plotter::plot("x", position.x);
        Plotter::plot("y", position.y);
        Plotter::plot("z", position.z);
        Plotter::plot("θ", fusionData.orientation);
        Plotter::plot("v", fusionData.voltage);
        Plotter::endPlot();

        // End condition
        bool inLastSegment = data->lineIndex == data->points.size() - 2;
        double time = Pursuit::getClosestTime(lastSegment, fusionData.position);
        if (inLastSegment && time >= 1) break;

        // Acceleration and deceleration
        double distance = Navigation::getDistance(position, lastSegment.end);
        double acceleration = Navigation::dmap((msLoop - msStart) / args->accelerationMs, 0.0, 1.0, args->minSpeed, args->speed);
        double deceleration = Navigation::dmap(distance, args->decelerationMm, 0.0, args->speed, args->minSpeed);
        double speed = std::min((inLastSegment && distance <= args->decelerationMm) ? deceleration : acceleration, args->speed);
        args->moveFunction(fusionData, speed);

        vTaskDelay(max(1000.0 / HZ - (millis() - msLoop), 0.0));
      }

      args->stopFunction();
      data->points.clear();
      args->task->stop();

      delete transitionData;
      delete args;
    }

    // Home
    struct HomeArgs : TaskArgs {
      double speed;
    };

    static void homeTask(void* task) {
      HomeArgs* args = (HomeArgs*)task;
      int pwm = (int)(args->speed * 255.0);

      bool xLimit = false;
      bool zLimit = false;

      while (true) {
        unsigned long msStart = millis();

        SensorData sensorData = dataProvider->getData();
        FusionData fusionData = Fusion::getData(sensorData);

        xLimit = xLimit || sensorData.xLimit;
        zLimit = zLimit || sensorData.zLimit;

        MotorData xAxis(false, xLimit ? 0 : pwm);
        MotorData zAxis(false, zLimit ? 0 : pwm);
        ToolheadData toolheadData { xAxis, zAxis };

        hardwareProvider->moveToolhead(toolheadData);

        if (sensorData.xLimit && sensorData.zLimit) break;
        vTaskDelay(max(1000.0 / HZ - (millis() - msStart), 0.0));
      }

      Fusion::homingComplete();
      args->task->stop();
      delete args;
    }

    // Rotate
    struct RotateArgs : TaskArgs {
      double heading;
      double speed;
    };

    static void rotateTask(void* task) {
      RotateArgs* args = (RotateArgs*)task;

      while (true) {
        unsigned long msStart = millis();

        // TODO

        vTaskDelay(max(1000.0 / HZ - (millis() - msStart), 0.0));
      }

      stopRobot();
      args->task->stop();
      delete args;
    }

    // Move functions
    static void moveRobot(FusionData fusionData, double speed) {
      NavigationData navigationData = Navigation::getData(fusionData, speed);
      hardwareProvider->move(navigationData);
    }

    static void moveToolhead(FusionData fusionData, double speed) {
      ToolheadData toolheadData = Navigation::getToolheadData(fusionData, speed);
      hardwareProvider->moveToolhead(toolheadData);
    }

    static void moveWheels(FusionData fusionData, double speed) {
      WheelsData wheelsData = Navigation::getWheelsData(fusionData, speed);
      hardwareProvider->moveWheels(wheelsData);
    }

    static void moveMill(FusionData fusionData, double speed) {
      MillData millData = Navigation::getMillData(fusionData, speed);
      hardwareProvider->moveMill(millData);
    }

    // Stop functions
    static void stopRobot() {
      hardwareProvider->move({{true, 0}, {true, 0}});
    }

    static void stopToolhead() {
      hardwareProvider->moveToolhead({{true, 0}, {true, 0}});
    }

    static void stopWheels() {
      hardwareProvider->moveWheels({{true, 0}});
    }

    static void stopMill() {
      hardwareProvider->moveMill({{true, 0}});
    }
};

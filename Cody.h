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
#define ACCELERATION_MS 500.0
#define DECELERATION_MM 250.0
#define MIN_SPEED 10.0
#define MOVEMENT_LOOKAHEAD 100.0
#define TOOLHEAD_LOOKAHEAD 50
#define WHEELS_LOOKAHEAD 10
#define MILL_LOOKAHEAD 25

class Cody {
  public:
    static IDataProvider* dataProvider;
    static IHardwareProvider* hardwareProvider;

    static void initialize(IDataProvider& dataProvider_, IHardwareProvider& hardwareProvider_) {
      dataProvider = &dataProvider_;
      hardwareProvider = &hardwareProvider_;
    }

    // Drive
    static Task* moveAsync(double x, double y, double speed = 100, double decelerationDistance = MOVEMENT_LOOKAHEAD) {
      Task* task = new Task("move", moveTask);
      TargetArgs* args = new TargetArgs();
      Navigation::drive.decelerationDistance = decelerationDistance;

      args->task = task;
      args->target = new Vector3(x, y);
      args->speed = speed / 100.0;
      args->positionMember = &FusionData::position;
      args->navigationTarget = &Navigation::drive;
      args->moveFunction = &moveRobot;
      args->stopFunction = &stopRobot;

      task->start(args);
      return task;
    }

    static void addPathPoint(double x, double y) {
      Vector3 point = Vector3(x, y);
      pathData.points.push_back(point);
    }

    static Task* followPathAsync(double speed = 100, double lookaheadDistance = MOVEMENT_LOOKAHEAD) {
      Task* task = new Task("followPath", followPathTask);
      FollowPathArgs* args = new FollowPathArgs();
      pathData.lookaheadDistance = lookaheadDistance;

      args->task = task;
      args->data = &pathData;
      args->speed = speed / 100.0;
      args->positionMember = &FusionData::position;
      args->navigationTarget = &Navigation::drive;
      args->moveFunction = &moveRobot;
      args->stopFunction = &stopRobot;

      task->start(args);
      return task;
    }

    // Toolhead
    static Task* moveToolheadAsync(double x, double z, double speed = 100, double decelerationDistance = TOOLHEAD_LOOKAHEAD) {
      Task* task = new Task("moveToolhead", moveTask);
      TargetArgs* args = new TargetArgs();
      Navigation::toolhead.decelerationDistance = decelerationDistance;

      args->task = task;
      args->target = new Vector3(x, 0, z);
      args->speed = speed / 100.0;
      args->positionMember = &FusionData::toolheadPosition;
      args->navigationTarget = &Navigation::toolhead;
      args->moveFunction = &moveToolhead;
      args->stopFunction = &stopToolhead;

      task->start(args);
      return task;
    }

    static void addToolheadPathPoint(double x, double z) {
      Vector3 point = Vector3(x, 0, z);
      toolheadPathData.points.push_back(point);
    }

    static Task* followToolheadPathAsync(double speed = 100, double lookaheadDistance = TOOLHEAD_LOOKAHEAD) {
      FollowPathArgs* args = new FollowPathArgs();
      Task* task = new Task("followToolheadPath", followPathTask);
      pathData.lookaheadDistance = lookaheadDistance;

      args->task = task;
      args->data = &toolheadPathData;
      args->speed = speed / 100.0;
      args->positionMember = &FusionData::toolheadPosition;
      args->navigationTarget = &Navigation::toolhead;
      args->moveFunction = &moveToolhead;
      args->stopFunction = &stopToolhead;

      task->start(args);
      return task;
    }

    static Task* homeAsync(double speed = 10) {
      HomeArgs* args = new HomeArgs();
      Task* task = new Task("homeTask", homeTask);

      args->task = task;
      args->speed = speed / 100.0;

      task->start(args);
      return task;
    }

    // Wheels
    static Task* moveWheelsAsync(double z, double speed = 100, double decelerationDistance = WHEELS_LOOKAHEAD) {
      Task* task = new Task("moveWheels", moveTask);
      TargetArgs* args = new TargetArgs();
      Navigation::wheels.decelerationDistance = decelerationDistance;

      args->task = task;
      args->target = new Vector3(0, 0, z);
      args->speed = speed / 100.0;
      args->positionMember = &FusionData::wheelsPosition;
      args->navigationTarget = &Navigation::wheels;
      args->moveFunction = &moveWheels;
      args->stopFunction = &stopWheels;

      task->start(args);
      return task;
    }

    static void addWheelsPathPoint(double z) {
      Vector3 point = Vector3(0, 0, z);
      wheelsPathData.points.push_back(point);
    }

    static Task* followWheelsPathAsync(double speed = 100, double lookaheadDistance = WHEELS_LOOKAHEAD) {
      FollowPathArgs* args = new FollowPathArgs();
      Task* task = new Task("followWheelsPath", followPathTask);
      pathData.lookaheadDistance = lookaheadDistance;

      args->task = task;
      args->data = &wheelsPathData;
      args->speed = speed / 100.0;
      args->positionMember = &FusionData::wheelsPosition;
      args->navigationTarget = &Navigation::wheels;
      args->moveFunction = &moveWheels;
      args->stopFunction = &stopWheels;

      task->start(args);
      return task;
    }

    // Mill
    static Task* moveMillAsync(double z, double speed = 100, double decelerationDistance = MILL_LOOKAHEAD) {
      Task* task = new Task("moveMill", moveTask);
      TargetArgs* args = new TargetArgs();
      Navigation::mill.decelerationDistance = decelerationDistance;

      args->task = task;
      args->target = new Vector3(z);
      args->speed = speed / 100.0;
      args->positionMember = &FusionData::millPosition;
      args->navigationTarget = &Navigation::mill;
      args->moveFunction = &moveMill;
      args->stopFunction = &stopMill;

      task->start(args);
      return task;
    }

    static void addMillPathPoint(double z) {
      Vector3 point = Vector3(z);
      millPathData.points.push_back(point);
    }

    static Task* followMillPathAsync(double speed = 100, double lookaheadDistance = MILL_LOOKAHEAD) {
      FollowPathArgs* args = new FollowPathArgs();
      Task* task = new Task("followMillPath", followPathTask);
      pathData.lookaheadDistance = lookaheadDistance;

      args->task = task;
      args->data = &millPathData;
      args->speed = speed / 100.0;
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


  private:
    static PursuitData pathData;
    static PursuitData toolheadPathData;
    static PursuitData wheelsPathData;
    static PursuitData millPathData;

    // Move
    using PositionMember = Vector3 (FusionData::*);
    using MoveFunction = void (*)(FusionData, double);
    using StopFunction = void (*)();

    struct TargetArgs : TaskArgs {
      Vector3* target;
      double speed;

      PositionMember positionMember;
      NavigationTarget* navigationTarget;
      MoveFunction moveFunction;
      StopFunction stopFunction;
    };

    static void moveTask(void* task) {
      TargetArgs* args = (TargetArgs*)task;
      Vector3* target = args->target;
      args->navigationTarget->target = *target;

      // Get movement line
      SensorData sensorData = dataProvider->getData();
      FusionData fusionData = Fusion::getData(sensorData);
      Line line(fusionData.position, *target);

      // Acceleration
      double speed = 0;
      unsigned long msStart = millis();

      while (true) {
        unsigned long msLoop = millis();

        SensorData sensorData = dataProvider->getData();
        FusionData fusionData = Fusion::getData(sensorData);
        Vector3 position = fusionData.*(args->positionMember);

        if (Pursuit::getClosestTime(line, position) >= 1) break;

        speed = std::min((msLoop - msStart) / ACCELERATION_MS * args->speed, args->speed);
        args->moveFunction(fusionData, speed);

        vTaskDelay(max(1000.0 / HZ - (millis() - msLoop), 0.0));
      }

      args->stopFunction();
      args->task->stop();
      delete args;
    }

    // Follow path
    struct FollowPathArgs : TaskArgs {
      PursuitData* data;
      double speed;

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

      unsigned long msStart = millis();

      while (true) {
        unsigned long msLoop = millis();

        SensorData sensorData = dataProvider->getData();
        FusionData fusionData = Fusion::getData(sensorData);
        Vector3 position = fusionData.*(args->positionMember);

        Vector3 lookaheadPoint = Pursuit::findLookahead(position, data);
        Vector3 overflowLookahead = Pursuit::findLookahead(position, data, true);

        args->navigationTarget->target = lookaheadPoint;
        args->navigationTarget->steeringTarget = overflowLookahead;

        Plotter::setLimits(0.0, 1000.0);
        Plotter::plot("lx", lookaheadPoint.x);
        Plotter::plot("ly", lookaheadPoint.y);
        Plotter::plot("x", position.x);
        Plotter::plot("y", position.y);
        Plotter::plot("θ", fusionData.orientation);
        Plotter::plot("v", fusionData.voltage);
        Plotter::endPlot();

        bool inLastSegment = data->lineIndex == data->points.size() - 2;
        double time = Pursuit::getClosestTime(lastSegment, fusionData.position);
        if (inLastSegment && time >= 1) break;

        double distance = Navigation::getDistance(position, lastSegment.end);
        double acceleration = Navigation::dmap((msLoop - msStart) / ACCELERATION_MS, 0.0, 1.0, MIN_SPEED / 100.0, args->speed);
        double deceleration = Navigation::dmap(distance, DECELERATION_MM, 0.0, args->speed, MIN_SPEED / 100.0);
        double speed = std::min((inLastSegment && distance <= DECELERATION_MM) ? deceleration : acceleration, args->speed);
        args->moveFunction(fusionData, speed);

        vTaskDelay(max(1000.0 / HZ - (millis() - msLoop), 0.0));
      }

      args->stopFunction();
      data->points.clear();
      args->task->stop();
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

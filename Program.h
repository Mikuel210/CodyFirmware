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
#define ALIGN_SPEED 45
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

// Mosaic
#define MOSAIC_X -510
#define MOSAIC_Y 850

#pragma endregion


class Program {
  public:
    static void go() {
      Fusion::restart();
      blocks();
    }

  private:
    static Task* moveTask;
    static Task* toolheadTask;
    static Task* millTask;

    static void blocks() {
      // Home and align
      toolheadTask = Cody::homeAsync();

      Cody::addPathPoint(0, -ALIGN_DISTANCE);
      align();
      Cody::setPosition(START_X, ALIGN_SET_Y, 0);

      // Go to mosaic
      Cody::addPathPoint(START_X, 400);
      Cody::addPathPoint(-300, 400);
      Cody::followPathAsync(50, 100, 150, 100)->await();

      Cody::addPathPoint(ALIGN_DISTANCE, 400);
      align();
      Cody::setPosition(ALIGN_SET_X, 400, -90);

      Cody::addPathPoint(MOSAIC_X, 400);
      Cody::addPathPoint(MOSAIC_X, MOSAIC_Y);
      Cody::followPathAsync()->await();

      // Take picture
      Serial2.println("GO");
      std::vector<Color> colors;

      while (colors.size() < 12) {
        if (!Serial2.available()) continue;

        String message = Serial2.readStringUntil('\n');
        colors.push_back(static_cast<Color>(message.toInt()));
      }
      
      // Initialize variables
      int yellow = std::count(colors.begin(), colors.end(), YELLOW);
      int blue   = std::count(colors.begin(), colors.end(), BLUE);
      int green  = std::count(colors.begin(), colors.end(), GREEN);
      int white  = std::count(colors.begin(), colors.end(), WHITE);

      int colorCounts[4] = { yellow, blue, green, white };
      int pickedCounts[4] = { 0, 0, 0, 0 };
      int totalPicked = 0;

      int mosaic[4][3] = {
        { colors[0], colors[1],  colors[2]  },
        { colors[3], colors[4],  colors[5]  },
        { colors[6], colors[7],  colors[8]  },
        { colors[9], colors[10], colors[11] },
      };

      bool positionsLeft[4][3];

      // Carry blocks
      pickBatch(&colorCounts, &pickedCounts, &totalPicked);
      leaveBatch(&pickedCounts, &mosaic, &positionsLeft, &totalPicked);
      pickBatch(&colorCounts, &pickedCounts, &totalPicked);
      leaveBatch(&pickedCounts, &mosaic, &positionsLeft, &totalPicked);
    }

    static void pickBatch(int (*colorCounts)[4], int (*pickedCounts)[4], int* totalPicked) {
      // Go to start
      millTask = Cody::moveMillAsync(0);

      Cody::addPathPoint(MOSAIC_X, 400);
      Cody::addPathPoint(0, 400);
      Cody::addPathPoint(0, -ALIGN_DISTANCE);
      
      moveTask = Cody::followPathAsync(ALIGN_SPEED, true);
      align();
      Cody::setPosition(0, ALIGN_SET_Y, 0);

      toolheadTask->await();
      toolheadTask = Cody::moveToolheadAsync(TOOLHEAD_PICK_START_X, TOOLHEAD_UP);
      
      Cody::addPathPoint(0, 0);
      Cody::addPathPoint(-100, 0);
      Cody::followPathAsync(35, false, 75, 75, 50)->await();

      Cody::addPathPoint(ALIGN_DISTANCE, 0);
      align();
      Cody::setPosition(ALIGN_SET_X, 0, -90);

      // Pick blocks
      for (int i = 0; i < 4; i++) {
        if (*colorCounts[i] == 0 || *pickedCounts[i] == *colorCounts[i]) continue;

        if (i != 0) toolheadTask = Cody::moveToolheadAsync(TOOLHEAD_PICK_START_X, TOOLHEAD_UP);
        moveTask = Cody::moveAsync(FIRST_GROUP_WALL_X_MM + BLOCK_GROUPS_INCREMENT * i, 0);

        millTask->await();
        millTask = Cody::moveMillAsync(90);

        toolheadTask->await();
        moveTask->await();
        millTask->await();

        for (int j = *pickedCounts[i]; j < *colorCounts[i]; j++)
        {
          if (j == 3)
          {
            toolheadTask = Cody::moveToolheadAsync(TOOLHEAD_PICK_START_X, TOOLHEAD_UP);
            Cody::forwards(BLOCK_DISTANCE_START);

            Cody::followPathAsync()->await();
            toolheadTask->await();
          }

          totalPicked++;
          pickedCounts[0][i]++;
          pick(j % 3, *totalPicked);

          if (*totalPicked == 6)
            return;
        }
      }
    }

    static void leaveBatch(int (*pickedCounts)[4], int (*mosaic)[4][3], bool (*positionsLeft)[4][3], int* totalPicked) {
      // Go to mosaic
      Cody::addPathPoint(0, 0);
      Cody::followPathAsync(50, true);

      Cody::addPathPoint(0, 400);
      Cody::addPathPoint(MOSAIC_X, 400);
      Cody::addPathPoint(MOSAIC_X, MOSAIC_Y);
      Cody::followPathAsync()->await();

      // Color
      for (int i = 3; i >= 0; i--) {
        if (pickedCounts[i] == 0) continue;

        Color color =
          (i == 0) ? YELLOW :
          (i == 1) ? BLUE :
          (i == 2) ? GREEN : WHITE;

        // Row
        for (int j = 0; j < 4; j++) {
          std::vector<int> positions;

          // Column
          for (int k = 0; k < 3; k++) {
            if (*mosaic[j][k] != color || *positionsLeft[j][k]) continue;
            positions.push_back(k);
          }

          if (positions.size() == 0) continue;

          moveTask = Cody::moveAsync(MOSAIC_X, MOSAIC_Y + BLOCK_DISTANCE_MOSAIC * j);
          toolheadTask = Cody::moveToolheadAsync(TOOLHEAD_LEAVE_START_X + BLOCK_DISTANCE_MOSAIC * positions[0], TOOLHEAD_UP);

          moveTask->await();
          toolheadTask->await();

          for (int k : positions)
          {
            totalPicked--;
            positionsLeft[0][j][k] = true;
            pick(k, *totalPicked);

            if (*totalPicked == 0) return;
          }
        }
      }
    }

    static void align() {
      moveTask = Cody::followPathAsync(ALIGN_SPEED, true);
      delay(ALIGN_MS);
      delete moveTask->requestStop;
      moveTask->requestStop = new bool(true);
      moveTask->await();
    }

    // x = 0, z = 1 for first block
    static void pick(int x, int z) {
      double xPosition = TOOLHEAD_PICK_START_X + BLOCK_DISTANCE_START * x;
      pickLeave(xPosition, BLOCK_HEIGHT * z);
    }

    static void leave(int x, int z) {
      double xPosition = TOOLHEAD_LEAVE_START_X + BLOCK_DISTANCE_MOSAIC * x;
      pickLeave(xPosition, BLOCK_HEIGHT * z);
    }

    static void pickLeave(double xPosition, double zPosition) {
      Cody::moveToolheadAsync(xPosition, TOOLHEAD_UP)->await();
      Cody::moveToolheadAsync(xPosition, TOOLHEAD_DOWN)->await();
      Cody::moveWheelsAsync(zPosition)->await();
      Cody::moveToolheadAsync(xPosition, TOOLHEAD_UP)->await();
    }
};

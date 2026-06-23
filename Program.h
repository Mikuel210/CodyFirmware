
#pragma region Program Parameters

// Note: 0, 0 is the center of the start area

// General
#define START_X -10
#define ALIGN_SET_X 50
#define ALIGN_SET_Y -50
#define ALIGN_DISTANCE 150
#define ALIGN_SPEED 30

// Blocks
#define FIRST_GROUP_WALL_X_MM 150
#define BLOCK_GROUPS_INCREMENT 100
#define BLOCK_DISTANCE_START 50
#define BLOCK_DISTANCE_MOSAIC 40
#define BLOCK_HEIGHT 30

// Toolhead
#define TOOLHEAD_UP 40
#define TOOLHEAD_DOWN 0
#define TOOLHEAD_PICK_START_X 50
#define TOOLHEAD_LEAVE_START_X 80

// Mosaic
#define MOSAIC_X -525
#define MOSAIC_Y 900

#pragma endregion


class Program {
  public:
    static void go() {
      Fusion::restart();
      blocks();
    }

  private:
    int colorCounts[3];
    int pickedCounts[3];
    int totalPicked = 0;
    
    Color mosaic[4][3];
    bool positionsLeft[4][3];

    Task* moveTask;
    Task* toolheadTask;
    Task* millTask;

    static void blocks() {
      // Home and align
      toolheadTask = Cody::homeAsync();

      Cody::addPathPoint(0, -ALIGN_DISTANCE);
      Cody::followPathAsync(ALIGN_SPEED, true);
      Cody::setPosition(START_X, ALIGN_SET_Y, 0);

      // Go to mosaic
      Cody::addPathPoint(START_X, 400);
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

      colorCounts[3] = { yellow, blue, green, white };
      pickedCounts[3] = { 0, 0, 0, 0 };

      mosaic[4][3] = {
        { colors[0], colors[1],  colors[2]  },
        { colors[3], colors[4],  colors[5]  },
        { colors[6], colors[7],  colors[8]  },
        { colors[9], colors[10], colors[11] },
      };

      // Carry blocks
      pickBatch();
      leaveBatch();
      pickBatch();
      leaveBatch();
    }

    static void pickBatch() {
      // Go to start
      millTask = Cody::moveMillAsync(0);

      Cody::addPathPoint(MOSAIC_X, 400);
      Cody::addPathPoint(0, 400);
      Cody::addPathPoint(0, -ALIGN_DISTANCE);
      Cody::followPathAsync(ALIGN_SPEED, true)->await();
      Cody::setPosition(0, ALIGN_SET_Y, 0);

      toolheadTask->await();
      toolheadTask = Cody::moveToolheadAsync(TOOLHEAD_PICK_START_X, TOOLHEAD_UP);
      
      Cody::addPathPoint(0, 0);
      Cody::addPathPoint(-100, 0);
      Cody::followPathAsync(35, false, 75, 75, 50)->await();

      Cody::addPathPoint(ALIGN_DISTANCE, 0);
      Cody::followPathAsync(ALIGN_SPEED, true)->await();
      Cody::setPosition(START_WALL_X_MM, 0, -90);

      // Pick blocks
      for (int i = 0; i < 4; i++) {
        if (colorCounts[i] == 0 || pickedCounts[i] == colorCounts[i]) continue;

        if (i != 0) toolheadTask = Cody::moveToolheadAsync(TOOLHEAD_PICK_START_X, TOOLHEAD_UP);
        moveTask = Cody::moveAsync(FIRST_GROUP_WALL_X_MM + BLOCK_GROUPS_INCREMENT * i, 0);

        millTask->await();
        millTask = Cody::moveMillAsync(90);

        toolheadTask->await();
        moveTask->await();
        millTask->await();

        for (int j = pickedCounts[i]; j < colorCounts[i]; j++)
        {
          if (j == 3)
          {
            toolheadTask = Cody::moveToolheadAsync(TOOLHEAD_PICK_START_X, TOOLHEAD_UP);
            Cody::forwards(BLOCK_DISTANCE_START);

            Cody::followPathAsync()->await();
            toolheadTask->await();
          }

          pick(j % 3, ++totalPicked);
          pickedCounts[i]++;

          if (totalPicked == 6)
            return;
        }
      }
    }

    static void leaveBatch() {
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
          std::vector positions;

          // Column
          for (int k = 0; k < 3; k++) {
            if (mosaic[j][k] != color || positionsLeft[j][k]) continue;
            positions.push_back(k);
          }

          if (positions.size() == 0) continue;

          moveTask = Cody::moveAsync(MOSAIC_X, MOSAIC_Y + BLOCK_DISTANCE_MOSAIC * j);
          toolheadTask = Cody::moveToolheadAsync(TOOLHEAD_LEAVE_START_X + BLOCK_DISTANCE_MOSAIC * positions[0]);

          moveTask->await();
          toolheadTask->await();

          for (int k : positions)
          {
            pick(k, --totalPicked);
            positionsLeft[j][k] = true;

            if (totalPicked == 0) return;
          }
        }
      }
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

    static void pickLeave(double xPosition, double zPosition)
    {
      Cody::moveToolheadAsync(xPosition, TOOLHEAD_UP)->await();
      Cody::moveToolheadAsync(xPosition, TOOLHEAD_DOWN)->await();
      Cody::moveWheelsAsync(zPosition)->await();
      Cody::moveToolheadAsync(xPosition, TOOLHEAD_UP)->await();
    }
};


#pragma region Program Parameters

// General
#define START_WALL_X_MM 50
#define FIRST_GROUP_WALL_X_MM 150

// Blocks
#define BLOCK_GROUPS_INCREMENT 100
#define BLOCK_DISTANCE_START 50
#define BLOCK_DISTANCE_MOSAIC 40
#define BLOCK_HEIGHT 30

// Toolhead
#define TOOLHEAD_UP 50
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
    Task* toolhead;

    Color mosaic[4][3];
    bool positionsLeft[4][3];

    static void blocks() {
      // Home and align
      Task* home = Cody::homeAsync();

      Cody::backwards(200);
      Cody::followPathAsync(50, true);
      Cody::setPosition(0, 0, 0);

      // Go to mosaic
      Cody::addPathPoint(0, 400);
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

      // Go to start
      Cody::addPathPoint(MOSAIC_X, 400);
      Cody::addPathPoint(-50, 400);
      Cody::addPathPoint(-50, 0);
      Cody::followPathAsync(40, true)->await();

      // Align and move toohead
      home->await();
      toolhead = Cody::moveToolheadAsync(TOOLHEAD_PICK_START_X, TOOLHEAD_UP);
      
      Cody::addPathPoint(100, 0);
      Cody::followPathAsync(35, false, 50, 50, 50)->await();
      Cody::setPosition(START_WALL_X_MM, 0, -90);
      
      // Pick first batch
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

      pickBatch();

      // Leave first batch
      Cody::addPathPoint(0, 0);
      Cody::followPathAsync(50, true);

      Cody::addPathPoint(0, 400);
      Cody::addPathPoint(MOSAIC_X, 400);
      Cody::addPathPoint(MOSAIC_X, MOSAIC_Y);
      Cody::followPathAsync()->await();

      leaveBatch();

      // Pick second batch
      Cody::addPathPoint(MOSAIC_X, 400);
      Cody::addPathPoint(0, 400);
      Cody::followPathAsync(50, true)->await();

      Cody::addPathPoint(0, 0);
      Cody::addPathPoint(-100, 0);
      Cody::followPathAsync(35, false, 75, 75, 50)->await();

      Cody::addPathPoint(100, 0);
      Cody::followPathAsync(35, false, 50, 50, 50)->await();
      Cody::setPosition(START_WALL_X_MM, 0, -90);

      pickBatch();

      // Leave second batch
      Cody::addPathPoint(0, 0);
      Cody::followPathAsync(50, true);

      Cody::addPathPoint(0, 400);
      Cody::addPathPoint(MOSAIC_X, 400);
      Cody::addPathPoint(MOSAIC_X, MOSAIC_Y);
      Cody::followPathAsync()->await();

      leaveBatch();
    }

    static void pickBatch() {
      for (int i = 0; i < 4; i++) {
        if (colorCounts[i] == 0 || pickedCounts[i] == colorCounts[i]) continue;

        if (i != 0) toolhead = Cody::moveToolheadAsync(TOOLHEAD_PICK_START_X, TOOLHEAD_UP);
        Cody::moveAsync(FIRST_GROUP_WALL_X_MM + BLOCK_GROUPS_INCREMENT * i, 0)->await();

        toolhead->await();

        for (int j = pickedCounts[i]; j < colorCounts[i]; j++)
        {
          if (j == 3)
          {
            toolhead = Cody::moveToolheadAsync(TOOLHEAD_PICK_START_X, TOOLHEAD_UP);
            Cody::forwards(BLOCK_DISTANCE_START);

            Cody::followPathAsync()->await();
            toolhead->await();
          }

          pick(j % 3, ++totalPicked);
          pickedCounts[i]++;

          if (totalPicked == 6)
            return;
        }
      }
    }

    static void leaveBatch() {
      // Colors
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

          Task* move = Cody::moveAsync(MOSAIC_X, MOSAIC_Y + BLOCK_DISTANCE_MOSAIC * j);
          toolhead = Cody::moveToolheadAsync(TOOLHEAD_LEAVE_START_X + BLOCK_DISTANCE_MOSAIC * positions[0]);

          move->await();
          toolhead->await();

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

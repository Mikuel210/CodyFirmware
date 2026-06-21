#define TOOLHEAD_X1 20
#define TOOLHEAD_X2 70
#define TOOLHEAD_X3 120
#define TOOLHEAD_UP 50
#define TOOLHEAD_DOWN 0
#define WHEELS_INCREMENT 30
#define MOVEMENT_INCREMENT 50

class Program {
  public:
    static void go() {
      Fusion::restart();

      bloques();
    }

  private:
    static void bloques() {
      // Ir al mosaico
      Task* home = Cody::homeAsync();

      Cody::addPathPoint(0, 350);
      Cody::addPathPoint(-600, 350);
      Cody::addPathPoint(-600, 950);
      Cody::followPathAsync(40)->await();

      // Foto
      delay(1000);

      // Volver a la posición inicial y subir tubo
      Cody::addPathPoint(-600, 350);
      Cody::followPathAsync(40, true)->await();

      // home->await();
      Task* toolhead = Cody::moveToolheadAsync(0, TOOLHEAD_UP);

      Cody::addPathPoint(0, 350);
      Cody::addPathPoint(0, 0);
      Cody::addPathPoint(-100, 0);
      Cody::followPathAsync(35, false, 75, 75, 50)->await();

      // toolhead->await();

      // Coger 6 bloques
      // pick3Blocks();

      Cody::addPathPoint(-100 - MOVEMENT_INCREMENT, 0);
      Task* move = Cody::followPathAsync(40);
      toolhead = Cody::moveToolheadAsync(TOOLHEAD_X1, TOOLHEAD_UP);

      move->await();
      // toolhead->await();

      // pick3Blocks();

      // Ir al mosaico
      Cody::addPathPoint(0, 0);
      Cody::followPathAsync(40, true)->await();

      toolhead = Cody::moveToolheadAsync(TOOLHEAD_X1, TOOLHEAD_UP);

      Cody::addPathPoint(0, 350);
      Cody::addPathPoint(-600, 350);
      Cody::addPathPoint(-600, 950);
      Cody::followPathAsync(30)->await();

      // toolhead->await();

      // Dejar bloques
      // leave3Blocks();

      move = Cody::moveAsync(-600, 950 + MOVEMENT_INCREMENT);
      toolhead = Cody::moveToolheadAsync(TOOLHEAD_X1, TOOLHEAD_UP);

      move->await();
      // toolhead->await();

      leave3Blocks();
    }

    static void pick3Blocks() {
      Cody::moveToolheadAsync(TOOLHEAD_X1, TOOLHEAD_UP)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X1, TOOLHEAD_DOWN)->await();
      Cody::moveWheelsAsync(WHEELS_INCREMENT * 1)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X1, TOOLHEAD_UP)->await();

      Cody::moveToolheadAsync(TOOLHEAD_X2, TOOLHEAD_UP)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X2, TOOLHEAD_DOWN)->await();
      Cody::moveWheelsAsync(WHEELS_INCREMENT * 2)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X2, TOOLHEAD_UP)->await();

      Cody::moveToolheadAsync(TOOLHEAD_X3, TOOLHEAD_UP)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X3, TOOLHEAD_DOWN)->await();
      Cody::moveWheelsAsync(WHEELS_INCREMENT * 3)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X3, TOOLHEAD_UP)->await();
    }

    static void leave3Blocks() {
      Cody::moveToolheadAsync(TOOLHEAD_X1, TOOLHEAD_UP)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X1, TOOLHEAD_DOWN)->await();
      Cody::moveWheelsAsync(WHEELS_INCREMENT * 2)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X1, TOOLHEAD_UP)->await();

      Cody::moveToolheadAsync(TOOLHEAD_X2, TOOLHEAD_UP)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X2, TOOLHEAD_DOWN)->await();
      Cody::moveWheelsAsync(WHEELS_INCREMENT * 1)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X2, TOOLHEAD_UP)->await();

      Cody::moveToolheadAsync(TOOLHEAD_X3, TOOLHEAD_UP)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X3, TOOLHEAD_DOWN)->await();
      Cody::moveWheelsAsync(WHEELS_INCREMENT * 0)->await();
      Cody::moveToolheadAsync(TOOLHEAD_X3, TOOLHEAD_UP)->await();
    }

    static void cemento() {

    }

    static void herramientas() {

    }
};

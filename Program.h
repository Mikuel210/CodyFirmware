
class Program {
  public:
    static void go() {
      cemento();
      bloques();
      herramientas();
    }

  private:
    static void cemento() {
      // Cody::addPathPoint(0, 500);
      // Cody::addPathPoint(500, 500);
      // Cody::followPathAsync(40)->await();

      Cody::homeAsync()->await();
      delay(1000);
      //Cody::moveToolheadAsync(0, 50)->await();

      Cody::addToolheadPathPoint(0, 50);
      Cody::followToolheadPathAsync()->await();

      //Cody::moveAsync(0, 500, 40)->await();
    }

    static void bloques() {

    }

    static void herramientas() {

    }
};


class Program {
  public:
    static void go() {
      cemento();
    }

  private:
    static void cemento() {
      Cody::addPathPoint(0, 350);
      Cody::addPathPoint(-600, 350);
      Cody::addPathPoint(-600, 950);
      Cody::followPathAsync(40)->await();

      Cody::addPathPoint(-600, 350);
      Cody::followPathAsync(40, true)->await();

      Cody::addPathPoint(0, 350);
      Cody::addPathPoint(0, 0);
      Cody::addPathPoint(-100, 0);
      Cody::followPathAsync(35, false, 50, 75, 50)->await();
    }

    static void bloques() {

    }

    static void herramientas() {

    }
};

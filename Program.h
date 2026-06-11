
class Program {
  public:
    static void go() {
      cemento();
      bloques();
      herramientas();
    }

  private:
    static void cemento() {
      Cody::addPathPoint(500, 0);
      Cody::followPathAsync(40, 250.0)->await();
    }

    static void bloques() {

    }

    static void herramientas() {

    }
};

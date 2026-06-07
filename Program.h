
class Program {
  public:
    static void go() {
      cemento();
      bloques();
      herramientas();
    }

  private:
    static void cemento() {
      Cody::addPathPoint(0, 500);
      Cody::addPathPoint(-500, 500);
      Cody::followPathAsync(40)->await();
    }

    static void bloques() {

    }

    static void herramientas() {

    }
};

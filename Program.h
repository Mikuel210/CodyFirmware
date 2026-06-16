
class Program {
  public:
    static void go() {
      cemento();
      bloques();
      herramientas();
    }

  private:
    static void cemento() {
      Cody::addPathPoint(0, 1000);
      Cody::followPathAsync()->await();
    }

    static void bloques() {

    }

    static void herramientas() {

    }
};

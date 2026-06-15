
class Program {
  public:
    static void go() {
      cemento();
      bloques();
      herramientas();
    }

  private:
    static void cemento() {
      //Cody::addPathPoint(0, 500);
      //Cody::addPathPoint(500, 500);
      //Cody::followPathAsync()->await();

      Cody::homeAsync()->await();
      delay(1000);
      Cody::moveToolheadAsync(0, 100, 40)->await();

      /*
      while (true) {
        Cody::hardwareProvider->moveToolhead({{0,0}, {1,100}});
        Serial.println(Cody::dataProvider->getData().zAxisPulses);
      }
      */

      //Cody::addToolheadPathPoint(0, 50);
      //Cody::followToolheadPathAsync()->await();

      //Cody::moveAsync(0, 500, 70)->await();
    }

    static void bloques() {

    }

    static void herramientas() {

    }
};

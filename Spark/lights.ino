#pragma SPARK_NO_PREPROCESSOR

#include "application.h"
#include "Color.h"

// Default color defn's
Color red     (255, 000, 000);
Color yellow  (255, 255, 000);
Color green   (000, 255, 000);
Color cyan    (000, 255, 255);
Color blue    (000, 000, 255);
Color magenta (255, 000, 255);
Color orange  (000, 190, 255);
Color white   (255, 255, 255);
Color black   (000, 000, 000);

Color lastColor(0,0,0);

int redPin = A7;
int grnPin = A6;
int bluPin = A5;

int colorPins[3] = {redPin, grnPin, bluPin};
int cycleSpeed = 500;

char *role = "lights";
char *routines[4] = {"off", "random", "fade", "static"};
int routine = 0;

void setColor(Color color) {
    analogWrite(redPin,color.getR());
    analogWrite(grnPin,color.getG());
    analogWrite(bluPin,color.getB());
    lastColor = color;
}

int colorFromWeb(String args) {
    RGB.control(true);
    RGB.color(255,000,000);
    delay(50);
    // find the seperators
    int redSep = args.indexOf(':');
    int grnSep = args.indexOf(':', redSep + 1);
    
    unsigned int redValue = args.substring(0,redSep).toInt();
    unsigned int grnValue = args.substring(redSep + 1, grnSep).toInt();
    unsigned int bluValue = args.substring(grnSep + 1).toInt();
    if(redValue <= 255 && grnValue <= 255 && bluValue <= 255) {
        setColor(Color(redValue,grnValue,bluValue));
        RGB.control(false);
        return 1;
    }
    RGB.control(false);
    return 0;
}

int setRoutine(String args) {
    RGB.control(true);
    RGB.color(255,0,0);
    routine = args.toInt();
    EEPROM.write(1,routine);
    delay(50);
    RGB.control(false);
    return routine;
}

void setup() {
    routine = (int) EEPROM.read(1);
    Spark.function("setColor", colorFromWeb);
    Spark.function("setRoutine", setRoutine);
    
    Spark.variable("role", role, STRING);
    int r = lastColor.getR();
    int g = lastColor.getG();
    int b = lastColor.getB();
    Spark.variable("redValue", &r, INT);
    Spark.variable("grnValue", &g, INT);
    Spark.variable("bluValue", &b, INT);
  
    for(int i; i < 3; i++) {
        pinMode(colorPins[i], OUTPUT);
        analogWrite(colorPins[i], 255);
    }
}

void loop() {
  switch(routine) {
      case 0:
        // the current LED i'm using has 255 as off
        setColor(black);
        break;
      case 1:
        setColor(Color(random(0,256),random(0,256),random(0,256)));
        delay(cycleSpeed);
        break;
      case 2:
        break;
      case 3:
        setColor(lastColor);
        delay(cycleSpeed);
        break;
  }
}

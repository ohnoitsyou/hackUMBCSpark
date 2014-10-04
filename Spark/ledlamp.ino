int red = A7;
int grn = A6;
int blu = A5;

int colors[3] = {red, grn, blu};
int colorsIndex;
int cycleSpeed = 500;
int redValue, grnValue, bluValue;

char *role = "lights";
char *routines[4] = {"off", "random", "fade", "static"};
int  routine = 1;

int setColors(String args) {
    int redSep = args.indexOf(':');
    int grnSep = args.indexOf(':',redSep + 1);

    redValue = args.substring(0,redSep).toInt();
    grnValue = args.substring(redSep + 1, grnSep).toInt();
    bluValue = args.substring(grnSep + 1).toInt();
    if(redValue >= 0 && redValue <= 255 &&
       grnValue >= 0 && grnValue <= 255 &&
       bluValue >= 0 && bluValue <= 255) {
      analogWrite(colors[0],255 - redValue);
      analogWrite(colors[1],255 - grnValue);
      analogWrite(colors[2],255 - bluValue);
    } else {
        return 0;
    }
    return 1;
}
void setColors() {
    String rgbString = String(redValue) + ":" + String(grnValue) + ":" + String(bluValue);
    setColors(rgbString);
}
int setRoutine(String args) {
    routine = args.toInt();
    // we changed the routine, we should save it
    uint8_t value = routine;
    EEPROM.write(1,value);
    return args.toInt();
}

void setup() {
    // read in the last routine set
    routine = (int) EEPROM.read(1);

    Spark.function("setColors", setColors);
    Spark.function("setRoutine", setRoutine);

    Spark.variable("role", role, STRING);
    Spark.variable("redValue",&redValue, INT);
    Spark.variable("grnValue",&grnValue, INT);
    Spark.variable("bluValue",&bluValue, INT);

    for(int i = 0; i < 3; i++) {
        pinMode(colors[i], OUTPUT);
    }
    for(int i = 0; i < 3; i++) {
        analogWrite(colors[i],255);
    }
}

void loop() {
    String rgbString = "";
    switch(routine) {
      case 0:
        // This is the configuration for the LED i'm using now
        // The string of LED's I have require 0's instead.
        // see the setColors function above
        rgbString = "255:255:255";
        setColors(rgbString);
        break;
      case 1:
        rgbString = String(random(0,256)) + ":" + String(random(0,256)) + ":" + String(random(0,256));
        setColors(rgbString);
        delay(cycleSpeed);
        break;
      case 2:
        break;
      case 3:
        rgbString = String(redValue) + ":" + String(grnValue) + ":" + String(bluValue);
        setColors(rgbString);
        delay(cycleSpeed);
        break;
    }
}

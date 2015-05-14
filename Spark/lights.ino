#pragma SPARK_NO_PREPROCESSOR

#include "application.h"
#include "Color.h"
#include "Fader.h"

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

int const numColors = 7;
Color colorsArray[numColors] = {red, yellow, green, cyan, blue, magenta,red};

Color lastColor(0,0,0);

int redPin = A7;
int grnPin = A6;
int bluPin = A5;

int colorPins[3] = {redPin, grnPin, bluPin};
int cycleSpeed = 500;
int stepSpeed  = 10;

Fader fader = Fader();
bool nextFade = true;
int stepCounter = 0;
int fadeCounter = -1;

char *role = "lights";
char *routines[4] = {"off", "random", "fade", "static"};
int routine = 0;

int r = 0;
int g = 0;
int b = 0;

bool silent;

void setColor(Color color) {
    analogWrite(redPin,color.getR());
    analogWrite(grnPin,color.getG());
    analogWrite(bluPin,color.getB());
    r = color.getR();
    g = color.getG();
    b = color.getB();
    lastColor = color;
}

// I really should do this as a fixed width number.
// I didn't want to mess with it on the node end though.
int colorFromWeb(String args) {
    RGB.control(true);
    RGB.color(255,000,000);
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
    EEPROM.write(1, routine);
    delay(50);
    RGB.control(false);
    return routine;
}

int toggleSilent(String args) {
    silent = silent ? false : true;
    EEPROM.write(2, silent);
    return (int) silent;
}

int setCycleSpeed(String args) {
    RGB.control(true);
    RGB.color(255,0,0);
    cycleSpeed = args.toInt();
    EEPROM.write(3, cycleSpeed);
    RGB.control(false);
    return cycleSpeed;
}

int setStepSpeed(String args) {
    RGB.control(true);
    RGB.color(255,0,0);
    stepSpeed = args.toInt();
    EEPROM.write(5, stepSpeed);
    RGB.control(false);
    return stepSpeed;
}

int setConfig(String args) {
    RGB.control(true);
    RGB.color(255,0,0);
    int ret = 0;
    String command = args.substring(0,2);
    String cmdargs = args.substring(2);
    if(command == "ro") { // routine
        ret = setRoutine(cmdargs);
    } else if(command == "co") { // color
        ret = colorFromWeb(cmdargs);
    } else if(command == "cs") { // cycle speed
        ret = setCycleSpeed(cmdargs);
    } else if(command == "ss") { // step speed
        ret = setStepSpeed(cmdargs);
    } else if(command == "ts") { // toggle silent
        ret = toggleSilent(cmdargs);
    } else {
        ret = -1;
    }
    RGB.control(false);
    return ret;
}

void setup() {
    Serial.begin(9600);
    routine = (int) EEPROM.read(1);
    silent = (int) EEPROM.read(2);
    //cycleSpeed = (int) EEPROM.read(3) + (int) EEPROM.read(4);
    
    //stepSpeed = (int) EEPROM.read(5);
    
    Spark.function("setConfig",setConfig);

    Spark.variable("role", role, STRING);
    Spark.variable("routine", &routine, INT);
    Spark.variable("redValue", &r, INT);
    Spark.variable("grnValue", &g, INT);
    Spark.variable("bluValue", &b, INT);
  
    for(int i; i < 3; i++) {
        analogWrite(colorPins[i], 255);
        pinMode(colorPins[i], OUTPUT);
    }
}

void loop() {
    if(silent && WiFi.ready()) {
        RGB.control(true);
        RGB.color(0,0,0);
    } else if(!silent || !WiFi.ready()) {
        RGB.control(false);
    }
  switch(routine) {
      case 0:
        // the current LED i'm using has 000 as off
        setColor(black);
        break;
      case 1:
        setColor(Color(random(0,256),random(0,256),random(0,256)));
        delay(cycleSpeed);
        break;
      case 2:
        // Setup the initial fade. Everything else will be handled later
        // If we change routine mid fade, we will pickup where we left off
        //   this is an interesting side-effect of this method
        // I could have setRoutine reset these values
        if(fadeCounter == -1 || nextFade) {
            fadeCounter++;
            stepCounter = 0;
            nextFade = false;
            fader = Fader(colorsArray[fadeCounter], colorsArray[fadeCounter + 1]);
        }
        if(fadeCounter < numColors - 1 ) {
            // display the current step
            if(stepCounter < 256) {
                setColor(fader.getStep(stepCounter));
                stepCounter++;
                delay(stepSpeed);
            }
            // we hit the last step, go to the next fade
            if(stepCounter >= 256) {
                nextFade = true;
            }
        } else {
            fadeCounter = -1;
        }
        break;
      case 3:
        setColor(lastColor);
        delay(cycleSpeed);
        break;
  }
}

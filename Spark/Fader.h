#ifndef __FADER__
#define __FADER__
#pragma SPARK_NO_PREPROCESSOR
#include "Color.h"

class Fader {
    public:
    Fader(Color start, Color end):
    start(start), end(end){
      calculateCross(start, end);
    };

    Color getStep(int i) {
        return fadeArray[i];
    };

    void calculateCross(Color start, Color end) {
        Color lastColor = start;
        int stepR = calculateStep(lastColor.getR(), end.getR());
        int stepG = calculateStep(lastColor.getG(), end.getG());
        int stepB = calculateStep(lastColor.getB(), end.getB());
        for(int i = 0; i < 256; i++) {

            fadeArray[i] = Color(calculateVal(stepR, lastColor.getR(), i),
                                 calculateVal(stepG, lastColor.getG(), i),
                                 calculateVal(stepB, lastColor.getB(), i));
            lastColor = fadeArray[i];
        }
    }
    // code adapted from http://www.arduino.cc/en/Tutorial/ColorCrossfader
    int calculateStep(int prevValue, int endValue) {
        int step = endValue - prevValue;
        if (step) {
            step = 256/step;
        }
        return step;
    };

    int calculateVal(int step, int val, int i) {
        if ((step) && i % step == 0) {
            if (step > 0) {
                val += 1;
            } else if (step < 0) {
                val -= 1;
            }
        }
        if (val > 255) {
            val = 255;
        } else if (val < 0) {
            val = 0;
        }
        return val;
    };

    private:
    Color start, end, fadeArray[256];
};

#endif

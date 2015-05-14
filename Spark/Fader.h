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
    /*
    Fader(Color start, Color end, int steps):
    start(start),end(end), steps(steps){
        calculateCross(start,end);
    };
    
    Fader(int steps):
    start(Color(0,0,0)), end(Color(0,0,0)),steps(steps){
        calculateCross(start, end);
    };
    */
    Fader():
    start(Color(0,0,0)), end(Color(0,0,0)){
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
    int calculateStep(int prevValue, int endValue) {
        int step = endValue - prevValue; // What's the overall gap?
        if (step) {                      // If its non-zero, 
            step = 256/step;              //   divide by 1020
        } 
        return step;
    };

    int calculateVal(int step, int val, int i) {
        if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
            if (step > 0) {              //   increment the value if step is positive...
                val += 1;           
            } else if (step < 0) {         //   ...or decrement it if step is negative
                val -= 1;
            }
        }
        // Defensive driving: make sure val stays in the range 0-255
        
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

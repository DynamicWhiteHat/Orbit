#include <math.h>

float lastX = 0.0f;
float lastY = 0.0f;

void setupLocator() {
    setupDWM();
}

void loopLocator() {
    loopDWM(); 
}
float getDistance() {
    float dL = getLeftDistance();
    float dR = getRightDistance();
    float d  = UWB_SPACING;

    float x = (sq(dL) - sq(dR)) / (2.0f * d);
    float y = sqrt(sq(dL) - sq(x + d / 2.0f));

    lastX = x;
    lastY = y;

    return sqrt(sq(x) + sq(y));
}

float getAngle() {
    return atan2(lastY, lastX); 
}

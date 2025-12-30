#include <QMC5883LCompass.h>

QMC5883LCompass compass;

struct {
    float x;  // Gauss, FLU frame
    float y;
    float z;
} compassValues;

void setupCompass() {
    compass.init();

    // Optional but recommended:
    // compass.setCalibration(-1537, 1266, -1961, 958, -1342, 1492);

}

void loopCompass() {
    compass.read();  

    int rawX = compass.getX();
    int rawY = compass.getY();
    int rawZ = compass.getZ();

    // Assume 2G range → 12000 LSB/Gauss
    constexpr float LSB_PER_GAUSS = 12000.0f;

    compassValues.x = rawX / LSB_PER_GAUSS;
    compassValues.y = rawY / LSB_PER_GAUSS;
    compassValues.z = rawZ / LSB_PER_GAUSS;
}
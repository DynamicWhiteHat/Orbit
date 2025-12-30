#if BAROMETER_ENABLED

#include "MS5611.h"

MS5611 baro(0x77);

struct
{
    float pa;          
    float temperature;
} barometerData;

float seaLevelPa = 100600;

void setupBarometer() {
    print("Setup Barometer\n");
    while (!Serial);
    Serial.println();
    Serial.println(__FILE__);
    Serial.print("MS5611_LIB_VERSION: ");
    Serial.println(MS5611_LIB_VERSION);
    Serial.println();

    Wire.begin(8,9);
    if (baro.begin() == true) {
        Serial.print("MS5611 found: ");
        Serial.println(baro.getAddress());
    } else {
        Serial.println("MS5611 not found. halt.");
        setLED(0);
        while (1);
    }
}

void loopBarometer() {
    static double lastUpdateTime = 0.0;

    if (t - lastUpdateTime >= BAROMETER_UPDATE_INTERVAL) {
        barometerData.temperature = celsiusToFahrenheit(baro.getTemperature());
        barometerData.pa = baro.getPressurePascal();

        lastUpdateTime = t;
    }
}

float getCurrentHeight() {
    return paToAltitude(barometerData.pa);
}

float paToAltitude(float pressurePa) {
    return 44330.0f * (1.0f - powf(pressurePa / seaLevelPa, 0.1903f));
}

static float celsiusToFahrenheit(float celsius) {
    return (celsius * 9.0f / 5.0f) + 32.0f;
}

#endif

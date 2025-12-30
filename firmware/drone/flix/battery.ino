#if BATTERY_ENABLED
extern bool armed; // Needed for disarming motors when battery low

// Voltage divider configuration
#define VSENSE_PIN 4               // ADC pin
#define R1 1000000.0               // 1MΩ
#define R2 1000000.0               // 1MΩ
#define ADC_MAX 4095.0             // 12-bit ADC (for ESP32), use 1023 for 10-bit Arduino
#define VREF 3.3                   // Reference voltage in volts

// Thresholds
#define BATTERY_DISARMED_VOLTAGE_THRESHOLD 6.0  // Example: 6V
#define BATTERY_UPDATE_INTERVAL 0.5             // seconds

// Battery data storage
struct battery_data
{
    float voltage;  // Volts
} batteryData;

// Setup the battery voltage reading
void setupBattery()
{
    print("Setup Battery Voltage Reading\n");
    pinMode(VSENSE_PIN, INPUT);
}

// Loop to read voltage periodically
void loopBattery()
{
    static double lastUpdateTime = 0.0;
    if (t - lastUpdateTime >= BATTERY_UPDATE_INTERVAL)
    {
        updateBatteryVoltage();
        lastUpdateTime = t;

        // Check disarm condition
        if (armed && batteryData.voltage < BATTERY_DISARMED_VOLTAGE_THRESHOLD)
        {
            setLED(0);
            armed = false;
            print("Disarmed due to low battery: %.2f V < %.2f V\n",
                  batteryData.voltage, BATTERY_DISARMED_VOLTAGE_THRESHOLD);
        }
    }
}

// Read voltage from the voltage divider
static void updateBatteryVoltage()
{
    uint16_t adcRaw = analogRead(VSENSE_PIN);

    // Convert ADC to voltage at the pin
    float vPin = ((float)adcRaw / ADC_MAX) * VREF;

    // Convert pin voltage to battery voltage using voltage divider formula
    // Vbattery = Vpin * (R1 + R2) / R2
    batteryData.voltage = vPin * (R1 + R2) / R2;

    // Optional: print for debugging
    // print("Raw ADC: %d, Vpin: %.2f V, Vbattery: %.2f V\n", adcRaw, vPin, batteryData.voltage);
}
#endif

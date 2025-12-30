#include <SPI.h>
#include <DW1000.h>

#define PIN_RST 4
#define PIN_IRQ 6
#define PIN_SS  5

#define POLL 0
#define LEN_DATA 16

byte data[LEN_DATA];

void setup() {
    Serial.begin(115200);
    DW1000.begin(PIN_IRQ, PIN_RST);
    DW1000.select(PIN_SS);

    DW1000.newConfiguration();
    DW1000.setDefaults();
    DW1000.setDeviceAddress(10);  // arbitrary address for the beacon
    DW1000.setNetworkId(10);
    DW1000.enableMode(DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
    DW1000.commitConfiguration();

    Serial.println("Beacon initialized...");
}

void loop() {
    // transmit a simple POLL packet
    DW1000.newTransmit();
    DW1000.setDefaults();
    data[0] = POLL;
    DW1000.setData(data, LEN_DATA);
    DW1000.startTransmit();
    Serial.println("Beacon transmitted POLL");

    delay(100); // transmit every 100ms
}

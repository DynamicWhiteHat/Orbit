#include <SPI.h>
#include <DW1000.h>

// ----- Pins -----
const uint8_t PIN_RST = 15;

// Left DW1000
const uint8_t PIN_IRQ_LEFT = 21;
const uint8_t PIN_SS_LEFT  = 18;

// Right DW1000
const uint8_t PIN_IRQ_RIGHT = 36;
const uint8_t PIN_SS_RIGHT  = 38;

// ----- Message types -----
#define POLL      0
#define POLL_ACK  1
#define RANGE     2

// ----- Device IDs -----
#define LEFT_ID  1
#define RIGHT_ID 2

// ----- Reply delay -----
uint16_t replyDelayTimeUS = 3000;

// ----- Module struct -----
struct DWModule {
    uint8_t id;
    uint8_t irq;
    uint8_t ss;
    volatile bool sentAck;
    volatile bool receivedAck;
    DW1000Time timePollSent;
    DW1000Time timePollReceived;
    DW1000Time timePollAckSent;
    DW1000Time timePollAckReceived;
    DW1000Time timeRangeSent;
    DW1000Time timeRangeReceived;
    float distance;
};

// ----- Modules -----
DWModule leftModule  = {LEFT_ID, PIN_IRQ_LEFT, PIN_SS_LEFT, false, false};
DWModule rightModule = {RIGHT_ID, PIN_IRQ_RIGHT, PIN_SS_RIGHT, false, false};

// ----- Data buffer -----
#define LEN_DATA 16
byte data[LEN_DATA];

// ----- Function prototypes -----
void selectModule(DWModule &mod);
void handleSentLeft();
void handleReceivedLeft();
void handleSentRight();
void handleReceivedRight();
void transmitPollAck(DWModule &mod);
float computeAsymmetricRange(DWModule &mod);
float getLeftDistance();
float getRightDistance();

// ----- Setup -----
void setupDWM() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("### Dual-anchor DW1000 Drone ###");

    // Initialize left module
    selectModule(leftModule);
    DW1000.newConfiguration();
    DW1000.setDefaults();
    DW1000.setDeviceAddress(leftModule.id);
    DW1000.setNetworkId(10);
    DW1000.enableMode(DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
    DW1000.commitConfiguration();
    DW1000.attachSentHandler(handleSentLeft);
    DW1000.attachReceivedHandler(handleReceivedLeft);
    DW1000.newReceive();
    DW1000.receivePermanently(true);
    DW1000.startReceive();

    // Initialize right module
    selectModule(rightModule);
    DW1000.newConfiguration();
    DW1000.setDefaults();
    DW1000.setDeviceAddress(rightModule.id);
    DW1000.setNetworkId(10);
    DW1000.enableMode(DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
    DW1000.commitConfiguration();
    DW1000.attachSentHandler(handleSentRight);
    DW1000.attachReceivedHandler(handleReceivedRight);
    DW1000.newReceive();
    DW1000.receivePermanently(true);
    DW1000.startReceive();
}

void loopDWM() {
    byte data[LEN_DATA];

    // ----- LEFT MODULE -----
    selectModule(leftModule);
    if (leftModule.receivedAck) {
        leftModule.receivedAck = false;
        DW1000.getData(data, LEN_DATA);
        byte msgId = data[0];
        if (msgId == POLL) {
            DW1000.getReceiveTimestamp(leftModule.timePollReceived);
            transmitPollAck(leftModule);
        }
        else if (msgId == RANGE) {
            DW1000.getReceiveTimestamp(leftModule.timeRangeReceived);
            leftModule.timePollSent.setTimestamp(data + 2);
            leftModule.timePollAckReceived.setTimestamp(data + 7);
            leftModule.timeRangeSent.setTimestamp(data + 12);
            leftModule.distance = computeAsymmetricRange(leftModule);
        }
    }

    // ----- RIGHT MODULE -----
    selectModule(rightModule);
    if (rightModule.receivedAck) {
        rightModule.receivedAck = false;
        DW1000.getData(data, LEN_DATA);
        byte msgId = data[0];
        if (msgId == POLL) {
            DW1000.getReceiveTimestamp(rightModule.timePollReceived);
            transmitPollAck(rightModule);
        }
        else if (msgId == RANGE) {
            DW1000.getReceiveTimestamp(rightModule.timeRangeReceived);
            rightModule.timePollSent.setTimestamp(data + 2);
            rightModule.timePollAckReceived.setTimestamp(data + 7);
            rightModule.timeRangeSent.setTimestamp(data + 12);
            rightModule.distance = computeAsymmetricRange(rightModule);
        }
    }

    // Print distances
    Serial.print("Left distance: "); Serial.print(getLeftDistance()); Serial.print(" m\t");
    Serial.print("Right distance: "); Serial.println(getRightDistance()); Serial.print(" m");

    delay(100);
}

// ----- Helper functions -----
void selectModule(DWModule &mod) {
    DW1000.begin(mod.irq, PIN_RST);
    DW1000.select(mod.ss);
}

void handleSentLeft()  { leftModule.sentAck = true; }
void handleReceivedLeft() { leftModule.receivedAck = true; }
void handleSentRight() { rightModule.sentAck = true; }
void handleReceivedRight() { rightModule.receivedAck = true; }

void transmitPollAck(DWModule &mod) {
    DW1000.newTransmit();
    DW1000.setDefaults();
    byte data[LEN_DATA];
    data[0] = POLL_ACK;
    data[1] = mod.id;

    DW1000Time deltaTime(replyDelayTimeUS, DW1000Time::MICROSECONDS);
    DW1000.setDelay(deltaTime);
    DW1000.setData(data, LEN_DATA);
    DW1000.startTransmit();
}

float computeAsymmetricRange(DWModule &mod) {
    DW1000Time round1 = (mod.timePollAckReceived - mod.timePollSent).wrap();
    DW1000Time reply1 = (mod.timeRangeSent - mod.timePollAckReceived).wrap();
    DW1000Time round2 = (mod.timeRangeReceived - mod.timePollAckSent).wrap();
    DW1000Time reply2 = (mod.timeRangeSent - mod.timePollAckReceived).wrap();
    DW1000Time tof = (round1 * round2 - reply1 * reply2) / (round1 + round2 + reply1 + reply2);
    return tof.getAsMeters();
}

float getLeftDistance()  { return leftModule.distance; }
float getRightDistance() { return rightModule.distance; }

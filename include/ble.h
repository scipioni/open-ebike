#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

class BLE : public BLEServerCallbacks
{
protected:
    bool deviceConnected = false;

    BLEServer *pServer;
    BLECharacteristic *pCharacteristic;

    BLEService *pService1816;
    BLECharacteristic *pChar1816_0013;
    BLECharacteristic *pChar1816_2A5C;
    BLECharacteristic *pChar1816_2A55;
    BLECharacteristic *pChar1816_2A5B;

    BLEService *pService180a;
    BLECharacteristic *pChar180a_2a29;
    BLECharacteristic *pChar180a_2a28;

    BLEService *pService0003;
    BLECharacteristic *pChar0003_0013;

    BLEService *pService0001;
    BLECharacteristic *pChar0001_0011;
    BLECharacteristic *pChar0001_0021;

    BLEService *pService0002;
    BLECharacteristic *pChar0002_0012;

public:
    BLE();

    void onConnect(BLEServer *);
    void onDisconnect(BLEServer *);
    void notify_ble(uint8_t);
    void setup_ble();
};

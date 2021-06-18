
#include "ble.h"
#include "led.h"

#define BLE_NAME "SPECIALIZED"

static BLEUUID service_1816("00001816-0000-1000-8000-00805f9b34fb");
static BLEUUID char_2A55("00002a55-0000-1000-8000-00805f9b34fb");
static BLEUUID char_2A5B("00002a5b-0000-1000-8000-00805f9b34fb");
static BLEUUID char_2A5C("00002a5c-0000-1000-8000-00805f9b34fb");

static BLEUUID service_180a("0000180a-0000-1000-8000-00805f9b34fb");
static BLEUUID char_2a29("00002a29-0000-1000-8000-00805f9b34fb");
static BLEUUID char_2a28("00002a28-0000-1000-8000-00805f9b34fb");

static BLEUUID service_0003("00000003-0000-4b49-4e4f-525441474947");
static BLEUUID char_0013("00000013-0000-4b49-4e4f-525441474947");

static BLEUUID service_0001("00000001-0000-4b49-4e4f-525441474947");
static BLEUUID char_0011("00000011-0000-4b49-4e4f-525441474947"); //
static BLEUUID char_0021("00000001-0000-4b49-4e4f-525441474947"); //

static BLEUUID service_0002("00000002-0000-4b49-4e4f-525441474947");
static BLEUUID char_0012("00000012-0000-4b49-4e4f-525441474947");

uint8_t battery_soc[20] = {00, 0x0c, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

uint8_t Adv_DATA[] = {0x0D, 0x02, 0x02, 0xE7, 0x03, 0x01, 0xff, 0xff};

BLE::BLE()
{
}

void BLE::setup_ble()
{
    // 0x09	«Complete Local Name»
    BLEDevice::init(BLE_NAME);

    // 0x01	«Flags»

    /** Optional: set the transmit power, default is 3db */
    //BLEDevice::setPower(ESP_PWR_LVL_P7); /** +7db */

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(this);

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); //pServer->getAdvertising();
    BLEAdvertisementData advertisementData;

    // 0xFF	«Manufacturer Specific Data»
    advertisementData.setManufacturerData(std::string((char *)&Adv_DATA[0], 8)); // 8 is length of Adv_DATA
    pAdvertising->setAdvertisementData(advertisementData);

    pService1816 = pServer->createService(service_1816);
    pChar1816_2A5B = pService1816->createCharacteristic(char_2A5B, BLECharacteristic::PROPERTY_NOTIFY);
    pChar1816_2A5B->addDescriptor(new BLE2902());
    pChar1816_2A55 = pService1816->createCharacteristic(char_2A55, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_INDICATE);
    pChar1816_2A55->addDescriptor(new BLE2902());
    pChar1816_2A5C = pService1816->createCharacteristic(char_2A5C, BLECharacteristic::PROPERTY_READ);
    uint8_t value2A5C[] = {0x30, 0x00};
    pChar1816_2A5C->setValue(value2A5C, 2);
    pService1816->start();
    pAdvertising->addServiceUUID(service_1816);

    pService180a = pServer->createService(service_180a);
    pChar180a_2a29 = pService180a->createCharacteristic(char_2a29, BLECharacteristic::PROPERTY_READ);
    pChar180a_2a29->setValue("SPECIALIZED");
    pChar180a_2a28 = pService180a->createCharacteristic(char_2a28, BLECharacteristic::PROPERTY_READ);
    pChar180a_2a28->setValue("V1.7.0");
    pService180a->start();

    pService0001 = pServer->createService(service_0001);
    pChar0001_0011 = pService0001->createCharacteristic(char_0011, BLECharacteristic::PROPERTY_READ);
    uint8_t value0011[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    pChar0001_0011->setValue(value0011, 20);
    pChar0001_0021 = pService0001->createCharacteristic(char_0021, BLECharacteristic::PROPERTY_WRITE);
    pService0001->start();

    pService0002 = pServer->createService(service_0002);
    pChar0002_0012 = pService0002->createCharacteristic(char_0012, BLECharacteristic::PROPERTY_WRITE);
    pService0002->start();

    pService0003 = pServer->createService(service_0003);
    pChar0003_0013 = pService0003->createCharacteristic(char_0013, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    uint8_t value0013[] = {0x01, 0x10, 0x24, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    pChar0003_0013->setValue(value0013, 20);
    pChar0003_0013->addDescriptor(new BLE2902());
    pService0003->start();

    pAdvertising->setScanResponse(true); // better false for battery devices
    BLEDevice::startAdvertising();
}

void BLE::onConnect(BLEServer *pServer)
{
    Serial.println("Client connected: ");
    deviceConnected = true;
    led_fast();
}

void BLE::onDisconnect(BLEServer *pServer)
{
    Serial.println("Client disconnected: ");
    deviceConnected = false;
    led_slow();
    vTaskDelay(pdMS_TO_TICKS(500)); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();
}

void BLE::notify_ble(uint8_t soc)
{
    if (!deviceConnected)
        return;
    //Serial.println("notify");
    battery_soc[2] = soc;
    pChar0003_0013->setValue(battery_soc, 20);
    pChar0003_0013->notify();
}
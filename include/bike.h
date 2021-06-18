
#ifdef BOARD_WEMOS_D1_MINI32
#define BUZZER_PIN 25
#define BUTTON_PIN 27 // no PWM on startup
#endif
#define BATTERY_V_MIN_mV 32000
#define BATTERY_V_MAX_mV 42000
#define BATTERY_K 40850 / 288 // empiric: V_battery_mV/mean_pin

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include "battery.h"
#include "canbus.h"
#include "ble.h"

class Bike : public BLE
{
private:
public:
    Bike();

    void setup();
    static void notify_task(void *parameter);
    void notify();
    void poweroff();
};

extern Bike bike;
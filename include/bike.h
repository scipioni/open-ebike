
#ifdef BOARD_WEMOS_D1_MINI32
#define BUTTON_PIN 27 // no PWM on startup
#define BUZZER_PIN 25 // TODO on .. because WIFI
#endif

#ifdef BOARD_LOLIN32_LITE
#define BUTTON_PIN 33 // no PWM on startup and no ADC2 in use to WIFI
#define BUZZER_PIN 16 // no ADC2 in use to WIFI
#endif

#define OTA_ENABLED 0

#define BATTERY_V_MIN_mV 32000
#define BATTERY_V_MAX_mV 42000
#define BATTERY_K 40850 / 288 // empiric: V_battery_mV/mean_pin

#define IDLE_OFF 300000 // in ms

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include "battery.h"
#include "canbus.h"
#include "ble.h"

#if OTA_ENABLED == 1
#include "ota.h"
#endif

class Bike : public BLE
{
private:

public:
    unsigned long motor_seen = 0;
    Bike();

    void setup();
    static void notify_task(void *parameter);
    void notify();
    void poweroff();
    void wifi_on();
    bool idle();
    void is_active();
};

extern Bike bike;
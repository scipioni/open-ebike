#include <Arduino.h>
#include <BLE2902.h>

#include "bike.h"
#include "led.h"
#include "buzzer.h"

#if OTA_ENABLED == 1
#include "ota.h"
#endif

#define BLE_NOTIFY_INTERVAL 1000

Bike bike = Bike();
Battery battery = Battery(BATTERY_K, BATTERY_V_MIN_mV, BATTERY_V_MAX_mV, BUTTON_PIN);

Bike::Bike()
{
    ;
}

void Bike::setup()
{
    led_setup();
    buzzer_setup();
    setup_ble();

    xTaskCreatePinnedToCore(this->notify_task, "notify", 10500, NULL, 5, NULL, ARDUINO_RUNNING_CORE);

#if CANBUS_ENABLE == 1
    delay(CANBUS_START_DELAY); // to avoid brownout
    canbus_setup();
#endif
}

void Bike::poweroff()
{
    battery.power = false;
    led_interval = BLINK_FAST;
    buzzer_on();
    delay(500);
    Serial.printf("power off, bye...");
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);
}

void Bike::notify()
{
    this->notify_ble(battery.capacity());
}

void Bike::notify_task(void *parameter) // static
{
    for (;;)
    {
        bike.notify();
        delay(BLE_NOTIFY_INTERVAL);
    }
}

void Bike::wifi_on()
{
    Serial.println("power on wifi");
    buzzer_play(2);
#if OTA_ENABLED == 1
    ota_setup();
#endif
}
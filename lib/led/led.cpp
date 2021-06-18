#include <Arduino.h>
#include "led.h"

int led_interval = BLINK_SLOW;

void toggleLED(void *parameter)
{

    for (;;)
    { // infinite loop
        // Turn the LED on
        digitalWrite(LED_PIN, HIGH);

        // Pause the task for 500ms
        vTaskDelay(led_interval / portTICK_PERIOD_MS);

        // Turn the LED off
        digitalWrite(LED_PIN, LOW);

        // Pause the task again for 500ms
        vTaskDelay(led_interval / portTICK_PERIOD_MS);
    }
}

void led_fast()
{
    led_interval = BLINK_FAST;
}

void led_slow()
{
    led_interval = BLINK_SLOW;
}

void led_setup()
{
    pinMode(LED_PIN, OUTPUT);

    xTaskCreate(
        toggleLED,    // Function that should be called
        "Toggle LED", // Name of the task (for debugging)
        1000,         // Stack size (bytes)
        NULL,         // Parameter to pass
        1,            // Task priority
        NULL          // Task handle
    );
    Serial.println("led initialized");
}
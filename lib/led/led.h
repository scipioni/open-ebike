#pragma once

#define LED_PIN LED_BUILTIN
#define BLINK_FAST 300
#define BLINK_SLOW 900

extern int led_interval;

void led_setup();
void led_fast();
void led_slow();
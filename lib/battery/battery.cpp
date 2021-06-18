// https://github.com/rlogiacco/BatterySense

#include "battery.h"

void pollBattery(void *parameter)
{
	Serial.print("waiting for a while to read POWER BUTTON");
	vTaskDelay(POWER_BUTTON_DELAY);
	for (;;)
	{ // infinite loop
		vTaskDelay(V_POLL_INTERVAL);
		if (battery.power)
			Serial.printf("Battery mean=%dmV mean_pin=%d last=%d capacity=%d%%: button=%d\n",
						  battery.voltage(),
						  battery.voltage_pin_mean,
						  battery.voltage_last,
						  battery.capacity(),
						  battery.button_pressed);

#ifdef DEBUG_BATTERY
		battery.debug();
#endif
	}
}

Battery::Battery(float k, uint16_t minVoltage, uint16_t maxVoltage, uint8_t sensePin)
{
	button_pressed = 0;
	this->k = k;
	this->sensePin = sensePin;
	this->minVoltage = minVoltage;
	this->maxVoltage = maxVoltage;
	analogReadResolution(9); // 9 bit for max 511

#if LATCH_MODE == CHANNEL_N
	pinMode(sensePin, INPUT_PULLUP); // analogRead attaches the pin to ADC channel, which remaps it off the PU circuit. You have to set the mode back to INPUT_PULLUP after the read,
#else
	pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
#endif

	voltage_mean = 0;
	current_sample = 0;

	this->mapFunction = (mapFunction ? mapFunction : &sigmoidal);

	for (int i = 0; i < V_SAMPLE; i++)
		voltages[i] = 0;

	xTaskCreate(
		pollBattery,	// Function that should be called
		"poll battery", // Name of the task (for debugging)
		4096,			// Stack size (bytes)
		NULL,			// Parameter to pass
		1,				// Task priority
		NULL			// Task handle
	);
}

uint8_t Battery::capacity()
{
	if (voltage_mean <= minVoltage)
	{
		return 0;
	}
	else if (voltage_mean >= maxVoltage)
	{
		return 100;
	}
	else
	{
		return (*mapFunction)(voltage_mean, minVoltage, maxVoltage);
	}
}

bool Battery::button()
{
	if (!power)
		return false;
	return button_pressed > 1;
}

void Battery::debug()
{
	Serial.printf("current=%d   values=", current_sample);
	for (int i = 0; i < V_SAMPLE; i++)
		Serial.printf("%d ", voltages[i]);
	Serial.println("");
}

uint16_t Battery::voltage()
{
	if (current_sample >= V_SAMPLE)
		current_sample = 0;

	int totalValue = 0;
	for (int i = 0; i < V_SAMPLE; i++)
	{
		totalValue += readButtonPin();
	}
	voltage_last = totalValue / V_SAMPLE;

	if (voltage_last < V_BUTTON_TRIGGER)
	{
		button_pressed++;
		return voltage_mean; // non sporchiamo le medie
	}
	else
	{
		button_pressed = 0;
	}

	voltages[current_sample++] = voltage_last;

	long tot = 0;
	int samples_valid = 0;
	for (int i = 0; i < V_SAMPLE; i++)
	{
		if (voltages[i] > 0)
		{
			tot += voltages[i];
			samples_valid++;
		}
	}
	if (samples_valid == 0)
		return 0;

	voltage_pin_mean = tot / samples_valid;	   // 9 bit resolution 512-1
	voltage_mean = this->k * voltage_pin_mean; // 9 bit resolution 512-1

	return voltage_mean;
}

uint16_t Battery::readButtonPin()
{
	uint16_t value = analogRead(sensePin);
	pinMode(sensePin, INPUT_PULLUP); // analogRead attaches the pin to ADC channel, which remaps it off the PU circuit. You have to set the mode back to INPUT_PULLUP after the read,
	return value;
}
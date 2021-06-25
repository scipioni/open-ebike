#include "bike.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("setup()");
  bike.setup();
}

void loop()
{
  if (battery.button())
  {
    bike.poweroff();
  }

#if OTA_ENABLED == 1
  if (battery.wifi()) {
    bike.wifi_on();
  }
  
  ota_handle();
#endif

  delay(1000);
}
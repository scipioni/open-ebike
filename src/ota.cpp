// example https://github.com/espressif/arduino-esp32/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino

#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
//#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "ota.h"

void ota_setup()
{
    Serial.println("ota setup");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        Serial.println("WIFI not connected, exit from OTA");
        return;
    }
    MDNS.end();
    if (!MDNS.begin("open-ebike"))
    {

        Serial.println("Error starting mDNS");
    }
    MDNS.addService("http", "tcp", 80); // The ESP actually had to offer a service (e.g. HTTP on port 80) for it to answer at open-ebike.local, even just for pings

    ArduinoOTA
        .onStart([]()
                 {
                     String type;
                     if (ArduinoOTA.getCommand() == U_FLASH)
                         type = "sketch";
                     else // U_SPIFFS
                         type = "filesystem";

                     // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                     Serial.println("Start updating " + type);
                 })
        .onEnd([]()
               { Serial.println("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)
                 {
                     Serial.printf("Error[%u]: ", error);
                     if (error == OTA_AUTH_ERROR)
                         Serial.println("Auth Failed");
                     else if (error == OTA_BEGIN_ERROR)
                         Serial.println("Begin Failed");
                     else if (error == OTA_CONNECT_ERROR)
                         Serial.println("Connect Failed");
                     else if (error == OTA_RECEIVE_ERROR)
                         Serial.println("Receive Failed");
                     else if (error == OTA_END_ERROR)
                         Serial.println("End Failed");
                 });

    ArduinoOTA.begin();

    Serial.print("WIFI connected: IP=");
    Serial.println(WiFi.localIP());
}

void ota_handle()
{
    //Serial.println("ota handle");
    ArduinoOTA.handle();
}
#include <Mokosh.hpp>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#include "rover.h"

Mokosh mokosh;

Adafruit_BMP280 bmp280;

void updateMeasurements()
{
    float temperature, pressure, altitude;
    temperature = bmp280.readTemperature();
    pressure = bmp280.readPressure();
    altitude = bmp280.readAltitude();
    mdebugI("Temp: %f, pres: %f, alt: %f", temperature, pressure, altitude);

    StaticJsonDocument<256> doc;
    doc["temperature"] = temperature;
    doc["pressure"] = pressure;
    doc["altitude"] = altitude;

    float voltage_ctrl = map(analogRead(35), 0, 4095, 0, 4300) / 1000.0;
    mdebugI("Battery voltage: %f", voltage_ctrl);

    doc["voltage_ctrl"] = voltage_ctrl;

    String output;
    serializeJson(doc, output);

    if (mokosh.isWifiConnected())
    {
        mokosh.publish("telemetry", output.c_str());
    }
}

void customCommand(uint8_t *message, unsigned int length)
{
    if (length < 32)
    {
        char msg[32] = {0};
        for (unsigned int i = 0; i < length; i++)
        {
            msg[i] = message[i];
        }
        msg[length + 1] = 0;

        mdebugD("Received custom command: %s", msg);
        String cmd = String(msg, length);
        executeString(cmd);
    }
    else
    {
        mdebugE("Message too long for buffer.");
    }
}

void setup()
{
    Wire.begin();

    mokosh.setDebugLevel(DebugLevel::DEBUG)
        ->setForceWiFiReconnect(true)
        ->setHeartbeat(false)
        ->setMDNS(false);

    mokosh.onCommand = customCommand;
    mokosh.onInterval(updateMeasurements, 10000, "UPDATEMQTT");

    mokosh.begin("Umbral Rose");
    if (!bmp280.begin(BMP280_ADDRESS_ALT))
    {
        mdebugE("BMP280 error");
        while (1)
            ;
    }

    pinMode(35, INPUT);
    setupRover();
}

void loop()
{
    mokosh.loop();
}
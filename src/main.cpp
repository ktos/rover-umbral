#include <Mokosh.hpp>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>

#include "rover.h"

Mokosh mokosh;

Adafruit_BMP280 bmp280;
Adafruit_AHTX0 aht;

void updateMeasurements()
{
    float temperature, pressure, altitude;
    temperature = bmp280.readTemperature();
    pressure = bmp280.readPressure();
    altitude = bmp280.readAltitude();
    mdebugI("Temperature: %.2f*C, pressure: %.2f hPa, altitude: %.2f m", temperature, pressure / 100, altitude);

    DynamicJsonDocument doc(512);
    doc["temperature"] = temperature;
    doc["pressure"] = pressure;
    doc["altitude"] = altitude;

    float voltage_ctrl = map(analogRead(35), 0, 4095, 0, 4300) / 1000.0;
    mdebugI("Control Li-Po voltage: %.2f V", voltage_ctrl);

    // TODO: motors Li-Po/NIMH voltage

    doc["voltage_ctrl"] = voltage_ctrl;

    sensors_event_t humidity;
    sensors_event_t temperature2;
    if (aht.getEvent(&humidity, &temperature2))
    {
        mdebugI("Temperature: %.2f*C, Humidity: %.2f \%", temperature2.temperature, humidity.relative_humidity);
        doc["temperature2"] = temperature2.temperature;
        doc["humidity"] = humidity.relative_humidity;
    }

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

    if (!aht.begin())
    {
        mdebugE("AHT error");
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
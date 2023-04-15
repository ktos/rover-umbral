#include <Mokosh.hpp>

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_MPU6050.h>
#include <TickTwo.h>
#include <SensorFusion.h>
#include <Adafruit_VL53L0X.h>

#include "rover.h"

Mokosh mokosh;

Adafruit_BMP280 bmp280;
Adafruit_AHTX0 aht;
Adafruit_MPU6050 mpu;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
SF fusion;
uint16_t distance;
Rover rover;

void telemetry()
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

    // pitch and roll are switched due to orientation of the sensor on the board
    // TODO: fix this in final board
    float pitch = fusion.getRoll();
    float roll = fusion.getPitch();
    float yaw = fusion.getYaw() - 180; // because of the orientation
    mdebugI("Pitch: %.2f deg, Roll: %.2f deg, Yaw: %.2f", pitch, roll, yaw);

    doc["pitch"] = pitch;
    doc["roll"] = roll;
    doc["yaw"] = yaw;

    if (distance != 65535)
    {
        doc["distance"] = distance / 10.0;
        mdebugD("Distance: %.2f cm", distance / 10.0);
    }
    else
    {
        mdebugE("Distance sensor is not working");
    }

    String output;
    serializeJson(doc, output);

    if (mokosh.isWifiConnected())
    {
        mokosh.publish("telemetry", output.c_str());
    }
}

float deltaT;

void updateImu()
{
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

    deltaT = fusion.deltatUpdate();
    fusion.MadgwickUpdate(gyro.gyro.x, gyro.gyro.y, gyro.gyro.z, accel.acceleration.x, accel.acceleration.y, accel.acceleration.z, deltaT);
}

void customCommand(String cmd)
{
    mdebugD("Received command: %s, executing as rover instructions.", cmd.c_str());
    rover.executeString(cmd);
}

void setup()
{
    Wire.begin();

    mokosh.setDebugLevel(DebugLevel::DEBUG)
        ->setForceWiFiReconnect(true)
        ->setHeartbeat(false)
        ->setMDNS(false);

    mokosh.onCommand = customCommand;
    mokosh.registerIntervalFunction(telemetry, 10000);

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

    if (!mpu.begin())
    {
        mdebugE("MPU6050 error");
        while (1)
            ;
    }

    if (!lox.begin())
    {
        mdebugE("Failed to boot VL53L0X");
        while (1)
            ;
    }

    // start continuous ranging
    lox.startRangeContinuous();

    pinMode(35, INPUT);

    rover.begin();
}

void loop()
{
    updateImu();

    if (lox.isRangeComplete())
    {
        distance = lox.readRange();
        rover.updateDistance(distance);
    }

    mokosh.loop();
}
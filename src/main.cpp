#include <Mokosh.hpp>

#include <Wire.h>
#include <SPI.h>

#include "rover.h"

Mokosh mokosh;

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
    mokosh.setDebugLevel(DebugLevel::DEBUG)
        ->setForceWiFiReconnect(true)
        ->setHeartbeat(false)
        ->setMDNS(false);

    mokosh.onCommand = customCommand;

    mokosh.begin("Umbral Rose");

    setupRover();
}

void loop()
{
    mokosh.loop();
}
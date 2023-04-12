#include <Mokosh.hpp>

Mokosh mokosh;

void setup()
{
    mokosh.setDebugLevel(DebugLevel::DEBUG)
        ->setForceWiFiReconnect(true)
        ->setHeartbeat(false)
        ->setMDNS(false);

    mokosh.begin("UmbralRose");
}

void loop()
{
    mokosh.loop();
}
#include <Mokosh.hpp>

Mokosh mokosh;

void setup()
{
    mokosh.setDebugLevel(DebugLevel::DEBUG)
        ->setForceWiFiReconnect(true)
        ->setHeartbeat(false)
        ->setMDNS(false);

    mokosh.begin("Umbral Rose");
}

void loop()
{
    mokosh.loop();
}
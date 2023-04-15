#include <Mokosh.hpp>
#include <Arduino.h>

class Rover
{
public:
    void begin(int forwardLeft = 25, int backwardLeft = 26, int forwardRight = 32, int backwardRight = 33)
    {
        this->forwardLeft = forwardLeft;
        this->backwardLeft = backwardLeft;
        this->forwardRight = forwardRight;
        this->backwardRight = backwardRight;

        pinMode(backwardLeft, OUTPUT);
        pinMode(backwardRight, OUTPUT);
        pinMode(forwardLeft, OUTPUT);
        pinMode(forwardRight, OUTPUT);

        stop();
    }

    void setAutonomous(bool state)
    {
        isAutonomous = state;
    }

    bool getAutonomous()
    {
        return this->isAutonomous;
    }

    void stop()
    {
        digitalWrite(backwardLeft, false);
        digitalWrite(backwardRight, false);
        digitalWrite(forwardLeft, false);
        digitalWrite(forwardRight, false);
    }

    void forward()
    {
        mdebugV("forward");
        digitalWrite(backwardLeft, false);
        digitalWrite(backwardRight, false);
        digitalWrite(forwardLeft, true);
        digitalWrite(forwardRight, true);
    }

    void backward()
    {
        mdebugV("backward");
        digitalWrite(backwardLeft, true);
        digitalWrite(backwardRight, true);
        digitalWrite(forwardLeft, false);
        digitalWrite(forwardRight, false);
    }

    void left()
    {
        mdebugV("left");
        digitalWrite(backwardLeft, true);
        digitalWrite(backwardRight, false);
        digitalWrite(forwardLeft, false);
        digitalWrite(forwardRight, true);
    }

    void right()
    {
        mdebugV("right");
        digitalWrite(backwardLeft, false);
        digitalWrite(backwardRight, true);
        digitalWrite(forwardLeft, true);
        digitalWrite(forwardRight, false);
    }

    void wait(int milliseconds)
    {
        mdebugV("wait");
        delay(milliseconds * 100);
    }

    void executeString(String inputString)
    {
        int strLength = inputString.length();

        if (strLength > 1)
            isAutonomous = true;

        for (int i = 0; i < strLength; i++)
        {
            char direction = inputString.charAt(i);
            int waitTime = 0;
            if (i < strLength - 1 && isdigit(inputString.charAt(i + 1)))
            {
                waitTime = inputString.charAt(i + 1) - '0';
                if (waitTime == 0)
                    waitTime = 10;

                i++;
            }
            switch (direction)
            {
            case 'f':
                forward();
                break;
            case 'b':
                backward();
                break;
            case 'l':
                left();
                break;
            case 'r':
                right();
                break;
            case 's':
                stop();
                break;
            default:
                break;
            }
            wait(waitTime);
        }

        isAutonomous = false;
    }

    void updateDistance(uint16_t frontDistance)
    {
        this->frontDistance = frontDistance;

        if (frontDistance < safeDistance)
            stop();
    }

private:
    int forwardLeft;
    int backwardLeft;
    int forwardRight;
    int backwardRight;
    uint16_t frontDistance;
    bool isAutonomous = false;
    uint16_t safeDistance = 50;
};
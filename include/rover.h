#include <Mokosh.hpp>
#include <Arduino.h>

class Rover
{
public:
    void begin(int forwardLeft = 25, int backwardLeft = 26, int forwardRight = 32, int backwardRight = 33)
    {
        // this->forwardLeft = forwardLeft;
        // this->backwardLeft = backwardLeft;
        // this->forwardRight = forwardRight;
        // this->backwardRight = backwardRight;

        // pinMode(backwardLeft, OUTPUT);
        // pinMode(backwardRight, OUTPUT);
        // pinMode(forwardLeft, OUTPUT);
        // pinMode(forwardRight, OUTPUT);

        ledcSetup(0, 5000, 8);
        ledcSetup(1, 5000, 8);
        ledcSetup(2, 5000, 8);
        ledcSetup(3, 5000, 8);
        ledcAttachPin(forwardLeft, 0);
        ledcAttachPin(backwardLeft, 1);
        ledcAttachPin(forwardRight, 2);
        ledcAttachPin(backwardRight, 3);

        this->forwardLeft = 0;
        this->backwardLeft = 1;
        this->forwardRight = 2;
        this->backwardRight = 3;

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
        ledcWrite(backwardLeft, 0);
        ledcWrite(backwardRight, 0);
        ledcWrite(forwardLeft, 0);
        ledcWrite(forwardRight, 0);
    }

    void forward()
    {
        mdebugV("forward");
        ledcWrite(backwardLeft, 0);
        ledcWrite(backwardRight, 0);
        ledcWrite(forwardLeft, speed);
        ledcWrite(forwardRight, speed);
    }

    void backward()
    {
        mdebugV("backward");
        ledcWrite(backwardLeft, speed);
        ledcWrite(backwardRight, speed);
        ledcWrite(forwardLeft, 0);
        ledcWrite(forwardRight, 0);
    }

    void left()
    {
        mdebugV("left");
        ledcWrite(backwardLeft, speed);
        ledcWrite(backwardRight, 0);
        ledcWrite(forwardLeft, 0);
        ledcWrite(forwardRight, speed);
    }

    void right()
    {
        mdebugV("right");
        ledcWrite(backwardLeft, 0);
        ledcWrite(backwardRight, speed);
        ledcWrite(forwardLeft, speed);
        ledcWrite(forwardRight, 0);
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
        {
            mdebugW("Stopping because distance %d is lower than safe %d", frontDistance, safeDistance);
            stop();
        }
    }

    void setSpeed(int speed)
    {
        this->speed = speed;
    }

private:
    int forwardLeft;
    int backwardLeft;
    int forwardRight;
    int backwardRight;
    uint16_t frontDistance;
    bool isAutonomous = false;
    uint16_t safeDistance = 50;
    int speed = 155;
};
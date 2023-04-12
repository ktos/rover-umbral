#include <Mokosh.hpp>
#include <Arduino.h>

const int FORWARD_LEFT = 25;
const int BACKWARD_LEFT = 26;

const int FORWARD_RIGHT = 32;
const int BACKWARD_RIGHT = 33;

void stop()
{
    digitalWrite(BACKWARD_LEFT, false);
    digitalWrite(BACKWARD_RIGHT, false);
    digitalWrite(FORWARD_LEFT, false);
    digitalWrite(FORWARD_RIGHT, false);
}

void setupRover()
{
    pinMode(BACKWARD_LEFT, OUTPUT);
    pinMode(BACKWARD_RIGHT, OUTPUT);
    pinMode(FORWARD_LEFT, OUTPUT);
    pinMode(FORWARD_RIGHT, OUTPUT);

    stop();
}

void forward()
{
    mdebugV("forward");
    digitalWrite(BACKWARD_LEFT, false);
    digitalWrite(BACKWARD_RIGHT, false);
    digitalWrite(FORWARD_LEFT, true);
    digitalWrite(FORWARD_RIGHT, true);
}

void backward()
{
    mdebugV("backward");
    digitalWrite(BACKWARD_LEFT, true);
    digitalWrite(BACKWARD_RIGHT, true);
    digitalWrite(FORWARD_LEFT, false);
    digitalWrite(FORWARD_RIGHT, false);
}

void left()
{
    mdebugV("left");
    digitalWrite(BACKWARD_LEFT, true);
    digitalWrite(BACKWARD_RIGHT, false);
    digitalWrite(FORWARD_LEFT, false);
    digitalWrite(FORWARD_RIGHT, true);
}

void right()
{
    mdebugV("right");
    digitalWrite(BACKWARD_LEFT, false);
    digitalWrite(BACKWARD_RIGHT, true);
    digitalWrite(FORWARD_LEFT, true);
    digitalWrite(FORWARD_RIGHT, false);
}

void wait(int milliseconds)
{
    mdebugV("wait");
    delay(milliseconds * 100);
}

void executeString(String inputString)
{
    int strLength = inputString.length();
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
}
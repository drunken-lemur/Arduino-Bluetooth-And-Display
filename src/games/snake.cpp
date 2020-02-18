#include <Arduino.h>

#include "./snake.h"

Snake::Snake(Adafruit_SSD1306 *display)
{
    Serial.println("construct Snake");

    this->display = display;
}

Snake::~Snake()
{
    Serial.println("destruct Snake");
}

void Snake::update()
{
    if (Serial.available())
    {
        int c = Serial.read();

        if (c < '0')
        {
            this->display->clearDisplay();
            delay(10);
            this->display->println(String(c));
            delay(10);
        }
    }
}
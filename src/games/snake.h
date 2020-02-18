#include "Adafruit_SSD1306.h"

class Snake
{
private:
    Adafruit_SSD1306 *display;

public:
    Snake(Adafruit_SSD1306 *display);
    ~Snake();

    void update();
};

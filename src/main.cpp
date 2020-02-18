#include <Arduino.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include "./games/snake.h"

#define ARDUINO_SPEED 115200
#define DEFAULT_BT_SPEED 38400
#define START_UPLOAD_SKETCH_COMMAND_1 48
#define START_UPLOAD_SKETCH_COMMAND_2 32

#define PIN_LED 13
#define PIN_REBOOT 3
#define PIN_AT_MODE 4

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

#define DISPLAY_LINES 4
#define DISPLAY_LINE_HEIGHT 8

// Defines
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT);

Snake *snake;

enum Command
{
  GREETING = 1,
  TOGGLE_LED = 2,
  BLINK_LED = 3,
  TEST_DISPLAY = 4,
  REBOOT = 9,
  HELP = 'h' - '0',
};

byte ResetDTR[2];

int displayLineNumber = 0;

// utils
void displayPrintln(const char str[], int fontSize = 1, int x = 0, int y = 0)
{
  // display.clearDisplay();

  display.setCursor(x, y);
  display.setTextSize(fontSize);
  display.setTextColor(SSD1306_WHITE);
  display.println(str);

  display.display();

  delay(10);
}

void displayReset(const char str[] = NULL)
{
  displayLineNumber = 0;
  display.clearDisplay();
}

void info(const char str[], int fontSize = 1)
{

  Serial.println(str);
  displayPrintln(str, fontSize, 0, displayLineNumber * DISPLAY_LINE_HEIGHT);

  displayLineNumber++;
  if (displayLineNumber >= DISPLAY_LINES)
  {
    displayReset();
  }
}

void ATcommandMode()
{
  pinMode(PIN_AT_MODE, OUTPUT);
  digitalWrite(PIN_AT_MODE, HIGH);

  Serial.println("AT");
  delay(500);

  Serial.println("AT+POLAR=1,0");
  Serial.println("AT+UART?");

  if (Serial.available())
  {
    Serial.print(Serial.read());
  }

  digitalWrite(PIN_AT_MODE, LOW);
}

void preReboot()
{
  delete snake;
}

void reboot()
{
  pinMode(PIN_REBOOT, OUTPUT);
}

void toogleLed()
{
  delay(10);
  digitalWrite(PIN_LED, !digitalRead(PIN_LED));
}

bool isCommand(char input, Command command)
{
  return input - '0' == command;
}

void testscrolltext(void)
{
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("Loved by you!!!"));
  display.display(); // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(4000);
  display.stopscroll();
  delay(2000);
  display.startscrollleft(0x00, 0x0F);
  delay(4000);
  display.stopscroll();
  delay(2000);
  display.startscrolldiagright(0x00, 0x07);
  delay(4000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(4000);
  display.stopscroll();
  delay(2000);
}

// Commands
void commandGreeting()
{
  info("Hi there!");
}

void commandReboot()
{
  info("Rebooting...");

  preReboot();

  Serial.end();

  delay(10);

  reboot();
}

void commandToggleLed()
{
  toogleLed();

  Serial.print("Led is ");
  Serial.println(analogRead(PIN_LED) ? "on" : "off");

  info("Toggle led");
}

void commandBlinkLed(int count = 20, int delayMs = 200)
{
  info("Blink led");

  for (int i = 0; i < count; i++)
  {
    toogleLed();
    delay(delayMs);
  }
}

void commandHelp()
{
  displayReset();

  info("Commands:");
  info("h - Help");
  info("1 - Greeting");
  info("2 - Toggle led");
  info("3 - Blink led");
  info("4 - Test display");
  info("9 - Reboot");
}

void commandTestDisplay()
{
  info("Test Display");

  delay(10);

  testscrolltext();
}

// Main logic
void processCommands(char input)
{
  if (isCommand(input, GREETING))
  {
    return commandGreeting();
  }

  if (isCommand(input, TOGGLE_LED))
  {
    return commandToggleLed();
  }

  if (isCommand(input, BLINK_LED))
  {
    return commandBlinkLed();
  }

  if (isCommand(input, TEST_DISPLAY))
  {
    return commandTestDisplay();
  }

  if (isCommand(input, REBOOT))
  {
    return commandReboot();
  }

  if (isCommand(input, HELP))
  {
    return commandHelp();
  }
}

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC);
  Serial.begin(ARDUINO_SPEED);
  pinMode(PIN_LED, OUTPUT);

  // ATcommandMode();

  displayReset();

  commandBlinkLed(10, 100);

  displayReset();

  info("Ready...");

  snake = new Snake(&display);
}

void loop()
{
  if (Serial.available())
  {
    ResetDTR[0] = Serial.read();

    if (ResetDTR[1] == START_UPLOAD_SKETCH_COMMAND_1 && ResetDTR[0] == START_UPLOAD_SKETCH_COMMAND_2)
    {
      displayReset();
      info("Flashing...");

      return reboot();
    }

    processCommands(ResetDTR[0]);

    ResetDTR[1] = ResetDTR[0];
  }

  snake->update();
}

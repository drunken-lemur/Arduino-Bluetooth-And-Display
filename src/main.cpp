#include <Arduino.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define ARDUINO_SPEED 115200
#define DEFAULT_BT_SPEED 38400
#define START_UPLOAD_SKETCH_COMMAND_1 48
#define START_UPLOAD_SKETCH_COMMAND_2 32

#define PIN_LED 13
#define PIN_REBOOT 3
#define PIN_AT_MODE 4

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

// Defines
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT);

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

// utils
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

void initDisplay()
{
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println(F("Arduino & Bluetooth"));

  display.display();
  delay(10);
}

// Commands
void commandGreeting()
{
  Serial.println("Hi there!");
}

void commandReboot()
{
  Serial.println("Rebooting...");
  Serial.end();

  delay(10);

  reboot();
}

void commandToggleLed()
{
  toogleLed();

  Serial.print("Led is ");
  Serial.println(analogRead(PIN_LED) ? "on" : "off");
}

void commandBlinkLed(int count = 20, int delayMs = 200)
{
  Serial.println("Blink led");

  for (int i = 0; i < count; i++)
  {
    toogleLed();
    delay(delayMs);
  }
}

void commandHelp()
{
  Serial.print("Commands: \n\t");
  Serial.print("h - Help \n\t");
  Serial.print("1 - Greeting \n\t");
  Serial.print("2 - Toggle led \n\t");
  Serial.print("3 - Blink led \n\t");
  Serial.print("4 - Test display \n\t");
  Serial.println("9 - Reboot");
}

void commandTestDisplay()
{
  Serial.println("Test Display");

  delay(10);

  testscrolltext();

  delay(10);

  initDisplay();
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

  ATcommandMode();

  commandBlinkLed(10, 100);

  initDisplay();

  Serial.println("Ready...");
}

void loop()
{
  if (Serial.available())
  {
    ResetDTR[0] = Serial.read();

    if (ResetDTR[1] == START_UPLOAD_SKETCH_COMMAND_1 && ResetDTR[0] == START_UPLOAD_SKETCH_COMMAND_2)
    {
      return reboot();
    }

    processCommands(ResetDTR[0]);

    ResetDTR[1] = ResetDTR[0];
  }
}

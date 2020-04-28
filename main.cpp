/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "Adafruit_ST7735.h"
#include <atomic>
#include <iostream>

typedef enum
{
   NONE = 0,
   UP = 1,
   RIGHT = 2,
   DOWN = 3,
   LEFT = 4,
   PRESS = 5
} Direction_e;

typedef struct
{
   Direction_e direction;
   std::atomic<bool> validity;
   uint16_t joystickHexdata;
} direction_t;

// Macrodefs
#define SAMPLING_RATE_ADC 25
#define MAIN_THREAD_WRITE_WAIT 2

direction_t joystickDirection;
Thread joystickReadThreadHandle;
Adafruit_ST7735 tft(D11, D12, D13, D10, D8, D9); // MOSI, MISO, SCLK, SSEL, TFT_DC, TFT_RST
// Use this initializer if you're using a 1.8" TFT
void AnalogCallback()
{
   AnalogIn joystick(A3);
   Direction_e joystickData = Direction_e::NONE;

   while (true)
   {
      if (joystickDirection.validity == false)
      {

         joystickDirection.joystickHexdata = joystick.read_u16();
         if ((joystickDirection.joystickHexdata >= 0x2e20) &&
             (joystickDirection.joystickHexdata <= 0x2e30))
         {
            joystickDirection.direction = Direction_e::UP;
            joystickDirection.validity = true;
         }
         else if ((joystickDirection.joystickHexdata >= 0x0570) &&
                  (joystickDirection.joystickHexdata <= 0x0590))
         {
            joystickDirection.direction = Direction_e::RIGHT;
            joystickDirection.validity = true;
         }
         else if ((joystickDirection.joystickHexdata >= 0xe900) &&
                  (joystickDirection.joystickHexdata <= 0xe940))
         {
            joystickDirection.direction = Direction_e::DOWN;
            joystickDirection.validity = true;
         }
         else if ((joystickDirection.joystickHexdata >= 0x8020) &&
                  (joystickDirection.joystickHexdata <= 0x8050))
         {
            joystickDirection.direction = Direction_e::LEFT;
            joystickDirection.validity = true;
         }
         else if ((joystickDirection.joystickHexdata >= 0x5200) &&
                  (joystickDirection.joystickHexdata <= 0x5250))
         {
            joystickDirection.direction = Direction_e::PRESS;
            joystickDirection.validity = true;
         }
         else
         {
            joystickDirection.direction = Direction_e::NONE;
            joystickDirection.validity = true;
         }
      }
      thread_sleep_for(SAMPLING_RATE_ADC);
   }
}

void drawDirection(Direction_e currentDirection, bool eraseShape)
{
   switch (currentDirection)
   {
   case Direction_e::UP:
      tft.fillTriangle(60, 88, 80, 44, 100, 88, eraseShape ? ST7735_BLACK : ST7735_GREEN);
      break;
   case Direction_e::RIGHT:
      tft.fillTriangle(60, 44, 60, 88, 100, 66, eraseShape ? ST7735_BLACK : ST7735_GREEN);
      break;
   case Direction_e::DOWN:
      tft.fillTriangle(60, 44, 80, 88, 100, 44, eraseShape ? ST7735_BLACK : ST7735_GREEN);
      break;
   case Direction_e::LEFT:
      tft.fillTriangle(100, 44, 100, 88, 60, 66, eraseShape ? ST7735_BLACK : ST7735_GREEN);
      break;
   case Direction_e::PRESS:
      tft.fillCircle(80, 64, 20, eraseShape ? ST7735_BLACK : ST7735_GREEN);
      break;
   case Direction_e::NONE:
   default:
      break;
   }
}

void redrawDirection(Direction_e currentDirection, Direction_e oldDirection)
{
   if (currentDirection != oldDirection)
   {
      drawDirection(oldDirection, true);
   }
   drawDirection(currentDirection, false);
}

void StartJoystickThread()
{
   joystickReadThreadHandle.start(AnalogCallback);
}

int main()
{
   tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab
   std::string directonText;
   Direction_e oldDirection = Direction_e::NONE;
   tft.setCursor(0, 0);
   tft.setRotation(1);
   tft.fillScreen(ST7735_BLACK);
   tft.setTextColor(ST7735_RED);
   tft.printf("TFT Initialized...done");
   tft.setCursor(0, 16);
   tft.setTextWrap(true);
   tft.printf("Starting Joystick thread...");
   StartJoystickThread();
   thread_sleep_for(500);
   tft.setCursor(0, 24);
   tft.printf("done");
   tft.setCursor(0, 32);
   tft.printf("Initializing");
   thread_sleep_for(250);
   tft.printf(".");
   thread_sleep_for(250);
   tft.printf(".");
   thread_sleep_for(250);
   tft.printf(".done");
   thread_sleep_for(250);
   tft.fillScreen(ST7735_BLACK);
   tft.setTextColor(ST7735_WHITE);
   while (true)
   {
      tft.setRotation(1);
      tft.setCursor(20 + (ST7735_TFTWIDTH / 4), 0);
      if (joystickDirection.validity == true)
      {
         if (Direction_e::NONE == joystickDirection.direction)
         {
            drawDirection(oldDirection, true);
            tft.fillRect(20 + (ST7735_TFTWIDTH / 4), 0, ST7735_TFTWIDTH, 8, ST7735_BLACK);
         }

         switch (joystickDirection.direction)
         {
         case Direction_e::UP:
            tft.printf("   UP");
            break;
         case Direction_e::LEFT:
            tft.printf(" LEFT");
            break;
         case Direction_e::DOWN:
            tft.printf(" DOWN");
            break;
         case Direction_e::RIGHT:
            tft.printf("RIGHT");
            break;
         case Direction_e::PRESS:
            tft.printf("PRESS");
            break;
         case Direction_e::NONE:
            break;
         }
         redrawDirection(joystickDirection.direction, oldDirection);

         if (Direction_e::NONE != joystickDirection.direction)
         {
            oldDirection = joystickDirection.direction;
         }
         joystickDirection.validity = false;
      }

      thread_sleep_for(MAIN_THREAD_WRITE_WAIT);
   }
}

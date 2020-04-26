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
} direction_t;

// Macrodefs
#define SAMPLING_RATE_ADC 200
#define MAIN_THREAD_WRITE_WAIT 50

direction_t *joystickDirection;
Thread joystickReadThreadHandle;

void AnalogCallback()
{
   AnalogIn joystick(A3);
   uint16_t joystickData = 0;

   while (true)
   {
      if (joystickDirection->validity == false)
      {
         joystickData = joystick.read() * 1000;
         if ((joystickData <= 190) && (joystick >= 150))
         {
            joystickDirection->direction = Direction_e::UP;
            joystickDirection->validity = true;
         }
         else if ((joystickData <= 520) && (joystick >= 480))
         {
            joystickDirection->direction = Direction_e::LEFT;
            joystickDirection->validity = true;
         }
         else if ((joystickData <= 920) && (joystick >= 880))
         {
            joystickDirection->direction = Direction_e::DOWN;
            joystickDirection->validity = true;
         }
         else if ((joystickData <= 30) && (joystick >= 10))
         {
            joystickDirection->direction = Direction_e::RIGHT;
            joystickDirection->validity = true;
         }
         else if ((joystickData <= 330) && (joystick >= 290))
         {
            joystickDirection->direction = Direction_e::PRESS;
            joystickDirection->validity = true;
         }
         thread_sleep_for(SAMPLING_RATE_ADC / 4);
      }
      else
      {
         thread_sleep_for(SAMPLING_RATE_ADC);
      }
   }
}

void StartJoystickThread()
{
   joystickReadThreadHandle.start(AnalogCallback);
}

int main()
{
   Adafruit_ST7735 tft(D11, D12, D13, D10, D8, D9); // MOSI, MISO, SCLK, SSEL, TFT_DC, TFT_RST
   // Use this initializer if you're using a 1.8" TFT
   tft.initR(INITR_BLACKTAB); // initialize a ST7735S chip, black tab

   std::string directonText;
   Direction_e old_joystick_value = Direction_e::NONE;
   tft.setCursor(0, 0);
   tft.setRotation(1);
   tft.fillScreen(ST7735_BLACK);
   tft.setTextColor(ST7735_RED);
   tft.setTextWrap(true);
   tft.printf("Starting Joystick thread..");
   //direction_t *joystickDirection;
   StartJoystickThread();
   thread_sleep_for(1000);
   tft.fillScreen(ST7735_BLACK);
   tft.setCursor(0, 0);
   tft.printf("Initialized");
   thread_sleep_for(1000);
   tft.fillScreen(ST7735_BLACK);
   tft.setTextColor(ST7735_RED);
   while (true)
   {
      tft.setCursor(0, 0);
      if (joystickDirection->validity == true)
      {
         tft.fillRect(0, 0, ST7735_TFTWIDTH, 8, ST7735_BLACK);
         if (joystickDirection->direction != old_joystick_value)
         {
            old_joystick_value = joystickDirection->direction;
            switch (joystickDirection->direction)
            {
            case Direction_e::UP:
               tft.printf("UP");
               break;
            case Direction_e::LEFT:
               tft.printf("LEFT");
               break;
            case Direction_e::DOWN:
               tft.printf("DOWN");
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
         }
         joystickDirection->validity = false;
      }
      thread_sleep_for(MAIN_THREAD_WRITE_WAIT);
   }
}

#include <Arduino.h>
#include <pico/time.h>
#include "display.hpp"
#include "controller.h"
#include <LovyanGFX.hpp>


static LGFX display;



struct repeating_timer ui_refresh_timer;
struct repeating_timer ui_fps_timer;

static float fps = 0;
static uint64_t frames;

LGFX_Sprite bg;

bool refresh_display(struct repeating_timer *t)
{

  display.clear();

  display.drawPng(__CONTROLLER_PNG, 0,0);
  
  display.drawFloat(fps, 2, 0, 152);
  frames++;

  return true;
}

bool calc_fps(struct repeating_timer *t) {
fps = frames/ ( abs((*t).delay_us) / 1000000.0f);
frames = 0;
return true;
}

void setup()
{
  
  Serial.begin(9600);
  Serial.println("Setting up screen...");

  display.init();
  display.clear();
  display.print("hello");
  Serial.flush();
  delay(1000);
  
  display.fillScreen(TFT_DARKGRAY);
  delay(500);
  
  display.releasePngMemory();

  if(!display.drawPng(__CONTROLLER_PNG, 50, 100 )) {
    Serial.println("Failed to draw PNG???");
    while(1)delay(1);
  }

  delay(5000);

  add_repeating_timer_us(-3333, refresh_display, NULL, &ui_refresh_timer);
  frames = 0;
  add_repeating_timer_ms(-2000, calc_fps, NULL, &ui_fps_timer);

  Serial.println("Adding timer to pool");
  /*
   */
  Serial.println("Finished setup");
}

void loop()
{
  sleep_us(100);
}
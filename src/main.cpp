#include <Arduino.h>
#include <pico/time.h>
#include "mutex"
#include "display.hpp"
#include "controller.h"
#include "background.h"
#include <LovyanGFX.hpp>


static LGFX display;

struct repeating_timer ui_refresh_timer;
struct repeating_timer ui_fps_timer;

static float fps = 0;
static uint_fast64_t frames;
static int yPos= 16;
bool screen_damaged = false; 
uint_fast64_t frametime;

LGFX_Sprite bg;
LGFX_Sprite ctlr;
LGFX_Sprite screen;

auto_init_mutex(screen_damaed_mutex);

bool calc_fps(struct repeating_timer *t) {

auto period = ( abs((*t).delay_us) / 1000000.0f);

  fps = frames/ period;
  float usFrame = (float)frametime/(float)frames;
Serial.printf("fps = %g, %fuS/frame\n", fps, usFrame);
frametime = 0;
frames = 0;
return true;
}

void drawUI () {

  
  auto _framestart = time_us_64();
  screen.clear();
  screen.drawPng(__BACKGROUND_PNG, __BACKGROUND_PNG_SIZE);
  screen.drawPng(__CONTROLLER_PNG, __CONTROLLER_PNG_SIZE, 10, yPos);
  screen.drawNumber(frames, 0,146);
  screen.drawFloat(fps, 2, 0, 152);
  
  
  display.startWrite();
  screen.pushSprite(&display, 0,0);
  display.endWrite();
  auto cframetime = time_us_64() - _framestart;
  frametime += + cframetime;
  frames++;
  screen_damaged = false;
}

void loopUI()
{
  while(1) {
    if(!screen_damaged || screen.displayBusy() ) yield();
    mutex_enter_blocking(&screen_damaed_mutex);
    drawUI();
    mutex_exit(&screen_damaed_mutex);
    yield();
  }
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
  
  display.setBrightness(128);


  bg.createSprite(80,160);
  bg.drawPng(__BACKGROUND_PNG, __BACKGROUND_PNG_SIZE);

  screen.createSprite(80,160);
  frames = 0;

  // Start the UI loop
  multicore_launch_core1(loopUI);
  
  // FPS accounting is done by a timer.
  add_repeating_timer_ms(1000, calc_fps, NULL, &ui_fps_timer);

}

void damage_screen() {
  // take the mutex
  mutex_enter_blocking(&screen_damaed_mutex);
  screen_damaged = true;
  mutex_exit(&screen_damaed_mutex);
}

int dY = 1;
void loop()
{
  auto deadline = millis();
  if(yPos > 128) dY = -1;
  if(yPos < 10) dY = 1;
  yPos += dY;
  
  damage_screen();

  while( millis() < deadline + 1) yield();
}
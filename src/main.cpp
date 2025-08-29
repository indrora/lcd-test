#include <Arduino.h>
#include <pico/time.h>
#include "display.hpp"
#include "controller.h"
#include "background.h"
#include <LovyanGFX.hpp>


static LGFX display;

struct repeating_timer ui_refresh_timer;
struct repeating_timer ui_fps_timer;

static float fps = 0;
static uint64_t frames;
static int yPos= 16;

LGFX_Sprite bg;
LGFX_Sprite ctlr;


bool calc_fps(struct repeating_timer *t) {
fps = frames/ ( abs((*t).delay_us) / 1000000.0f);
frames = 0;

Serial.printf("fps = %3d\n", fps);

return true;
}

bool update_ui(struct repeating_timer *t) {

  if(display.displayBusy()) return true;

  display.startWrite();
  display.clear();

  //display.drawPng(__BACKGROUND_PNG, __BACKGROUND_PNG_SIZE, 0,0);
  
  //display.drawPng(__CONTROLLER_PNG, __CONTROLLER_PNG_SIZE, 10, yPos);

  bg.pushSprite(&display, 0,0);
  ctlr.pushSprite(&display, 10,yPos);


  display.drawNumber(millis(), 0,146);
  display.drawFloat(fps, 2, 0, 152);
  frames++;
  display.endWrite();
  display.display();

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
  
  display.setBrightness(128);


  bg.createSprite(80,160);
  bg.drawPng(__BACKGROUND_PNG, __BACKGROUND_PNG_SIZE);
  ctlr.createSprite(32,32);
  //ctlr.setColorDepth(32);
  ctlr.drawPng(__CONTROLLER_PNG, __CONTROLLER_PNG_SIZE);


  delay(5000);

  add_repeating_timer_ms(100, update_ui, NULL, &ui_refresh_timer);
  frames = 0;
  add_repeating_timer_ms(-2000, calc_fps, NULL, &ui_fps_timer);

  Serial.println("Adding timer to pool");
  /*
   */
  Serial.println("Finished setup");
}

int dY = 1;
void loop()
{

  if(yPos > 128) dY = -1;
  if(yPos < 10) dY = 1;
  yPos += dY;
  sleep_ms(100);
}
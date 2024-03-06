#include <Arduino.h>
#include "FS.h"
#include "LittleFS.h"

#include "util.h"
#include "jiskan24.h"
#include "lgfx_hub75.hpp"

#define LGFX_USE_V1
#include <LovyanGFX.hpp>


LGFX_HUB75 display;
static const lgfx::U8g2font jiskan24( jiskan24_data );
LGFX_Sprite sprite1(&display);
LGFX_Sprite sprite2(&display);
LGFX_Sprite sprite3(&display);

int offset_width = 32;  //これがないと32x64+32x32でつなげる時に32横にずれる

void setup() {
  display.init();
  LittleFS.begin();
  sprite1.createSprite(144,32);
  sprite2.createSprite(144,32);
  sprite3.createSprite(144,32);
  sprite1.drawPngFile(LittleFS,img_ko8000_led,0,0,48,32,0,0);
  sprite1.drawPngFile(LittleFS,img_ko8000_led,48,0,96,32,48,0);
  sprite2.drawPngFile(LittleFS,img_ko8000_led,0,0,48,32,0,0+32);
  sprite2.drawPngFile(LittleFS,img_ko8000_led,48,0,96,32,48,0+32);
  
}

void loop(){
  sprite1.pushSprite(offset_width,0);
  delay(2000);
  sprite2.pushSprite(offset_width,0);
  delay(2000);
  
}
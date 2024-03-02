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
LGFX_Sprite sprite;
LGFX_Sprite sprite_traintype;
LGFX_Sprite sprite_destination;

static constexpr char text_type[] = "急行";
static constexpr size_t text_typelen = sizeof(text_type) / sizeof(text_type[0]);
size_t text_typepos = 0;
static constexpr char text_destination[] = "新宿";
static constexpr size_t text_destinationlen = sizeof(text_destination) / sizeof(text_destination[0]);
size_t text_destinationpos = 0;

void setup() {
  display.init();
  LittleFS.begin();
  
    // 画面が横長になるように回転
  if (display.width() < display.height()) display.setRotation(display.getRotation() ^ 1);

  sprite_traintype.createSprite(48,display.height()); // 画面幅+１文字分の横幅を用意
  sprite_traintype.setColorDepth(8);
  sprite_traintype.setFont(&jiskan24);
  sprite_traintype.setTextSize(0.80);
  sprite_traintype.setTextWrap(false);        // 右端到達時のカーソル折り返しを禁止

  sprite_destination.createSprite(120,display.height()); // 画面幅+１文字分の横幅を用意
  sprite_destination.setColorDepth(8);
  sprite_destination.setFont(&jiskan24);
  sprite_destination.setTextSize(0.80);
  sprite_destination.setTextWrap(false);        // 右端到達時のカーソル折り返しを禁止

}

void loop(){
  
  sprite_traintype.setCursor(0,0);
  sprite_traintype.print(text_type);
  sprite_destination.setCursor(0,0);
  sprite_destination.print(text_destination);
  sprite_traintype.pushSprite(&display,0,display.height()/2 - 24/2);
  sprite_destination.pushSprite(&display,48,display.height()/2 - 24/2);
  
  delay(2000);
  
}
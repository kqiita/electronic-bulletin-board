#include <Arduino.h>
#include "FS.h"
#include "LittleFS.h"

#include <stdio.h>
#include <string.h>

#include "util.h"
#include "jiskan24.h"
#include "lgfx_hub75.hpp"

#define LGFX_USE_V1
#include <LovyanGFX.hpp>


LGFX_HUB75 display;
static const lgfx::U8g2font jiskan24(jiskan24_data);
LGFX_Sprite sprite1(&display);
LGFX_Sprite sprite2(&display);
LGFX_Sprite sprite3(&display);
LGFX_Sprite sprite4(&display);

int offset_width = 32; // これがないと32x64+32x32でつなげる時に32横にずれる

static constexpr char text[] = "本日もお越しいただきありがとうございます！！";
static constexpr size_t textlen = sizeof(text) / sizeof(text[0]);
size_t textpos = 0;
int32_t cursor_x ;
void setup()
{
  display.init();
  LittleFS.begin();
  sprite1.createSprite(144, 32);
  sprite2.createSprite(144, 32);
  sprite3.createSprite(display.width() + 24, 32);
  sprite4.createSprite(160,32);
  sprite1.drawPngFile(LittleFS, img_ko8000_led, 0, 0, 48, 32, 114 * 0 + 0, 32 * 0);
  sprite1.drawPngFile(LittleFS, img_ko8000_led, 48, 0, 96, 32, 144 * 0 + 48, 32 * 1);
  sprite2.drawPngFile(LittleFS, img_ko8000_led, 0 , 0, 48, 32, 0 , 0 + 32*1);
  sprite2.drawPngFile(LittleFS, img_ko8000_led, 48, 0, 96, 32, 48, 0 + 32*1);
  sprite2.drawPngFile(LittleFS, img_ko8000_led, 0, 0, 144, 32, 144 * 1 + 0, 32 * 5);
  sprite2.drawPngFile(LittleFS, img_ko8000_led, 48, 0, 96, 32, 144*0 + 48, 32*1);
  sprite3.setTextFont(&jiskan24);
  sprite3.setTextSize(1.0);
  sprite3.setTextWrap(false);
  sprite4.drawPngFile(LittleFS,img_uec,0,0,160,32,0,0);
}

void loop()
{
  /*
  sprite1.pushSprite(offset_width, 0);
  delay(5000);
  sprite2.pushSprite(offset_width, 0);
  delay(5000);
  
  
  */
  sprite4.pushSprite(offset_width,0);
  delay(5000);
  display.fillScreen(TFT_BLACK);

  textpos = 0;
  cursor_x = display.width();
  while (cursor_x > 0)
  {
    sprite3.setCursor(cursor_x , 8);                          // カーソル位置を更新
    sprite3.scroll(-1, 0);                                   // キャンバスの内容を1ドット左にスクロール
    while (textpos < textlen && cursor_x <= display.width()-32) // 画面右端に文字が書けるか判定
    {
      sprite3.print(text[textpos++]);  // 1バイトずつ出力 (マルチバイト文字でもこの処理で動作します)
      cursor_x = sprite3.getCursorX(); // 出力後のカーソル位置を取得
    }

    sprite3.pushSprite(&display, offset_width, 0);
    delay(20);
    cursor_x  = sprite3.getCursorX() - 1;
  }
  sprite3.fillScreen(TFT_BLACK);

}
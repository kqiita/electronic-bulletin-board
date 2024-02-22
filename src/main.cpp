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

static constexpr char text[] = "急行";
static constexpr size_t textlen = sizeof(text) / sizeof(text[0]);
size_t textpos = 0;

void setup() {
  display.init();
  // 画面が横長になるように回転
  if (display.width() < display.height()) display.setRotation(display.getRotation() ^ 1);

  sprite.setColorDepth(8);
  sprite.setFont(&jiskan24);
  sprite.setTextWrap(false);        // 右端到達時のカーソル折り返しを禁止
  sprite.createSprite(display.width() + 36, 36); // 画面幅+１文字分の横幅を用意


  LittleFS.begin();
  
  
}

void loop(){
  int32_t cursor_x = sprite.getCursorX() - 1;   // 現在のカーソル位置を取得し、1ドット左に移動
  if (cursor_x <= 0) // カーソル位置が左端に到達している場合は一周したと判断
  {
    textpos = 0;            // 文字列の読取り位置をリセット
    cursor_x = display.width(); // 新たな文字が画面右端に描画されるようにカーソル位置を変更
  }

  sprite.setCursor(cursor_x, 0); // カーソル位置を更新
  sprite.scroll(-1, 0);          // キャンバスの内容を1ドット左にスクロール
  while (textpos < textlen && cursor_x <= display.width()) // 画面右端に文字が書けるか判定
  {
    sprite.print(text[textpos++]);   // 1バイトずつ出力 (マルチバイト文字でもこの処理で動作します)
    cursor_x = sprite.getCursorX();  // 出力後のカーソル位置を取得
  }

  sprite.pushSprite(&display, 0, 0);

  delay(200);
}
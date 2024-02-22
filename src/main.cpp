#include <Arduino.h>
#include "FS.h"
#include "LittleFS.h"

#include "util.h"
#include "jiskan24.h"


#define LGFX_USE_V1
#include <LovyanGFX.hpp>


struct LGFX_HUB75 : public lgfx::LGFX_Device
{
  lgfx::Bus_HUB75 _bus_instance;

#ifdef MULTI
// 2枚以上接続する場合はこちら
  lgfx::Panel_HUB75_Multi _panel_instance;
#else
// 1枚だけで使用する場合はこちら
  lgfx::Panel_HUB75 _panel_instance;
#endif
  LGFX_HUB75(void)
  {


    {
      auto cfg = _bus_instance.config();

      // I2Sのポート番号を指定する。もし同時にI2S_DACを使用したい場合は 1に設定する。
      // (※ I2S_DACはポート0専用なので、HUB75ではポート1を使用することでDAC用にポート0を空けておく)
      cfg.port = 1;


   // 64x32パネル用
      cfg.pin_r1 = GPIO_NUM_32; // R1
      cfg.pin_r2 = GPIO_NUM_25; // R2
      cfg.pin_g1 = GPIO_NUM_19; // G1
      cfg.pin_g2 = GPIO_NUM_5 ; // G2
      cfg.pin_b1 = GPIO_NUM_33; // B1
      cfg.pin_b2 = GPIO_NUM_26 ; // B2

      cfg.pin_lat = GPIO_NUM_2; // LAT
      cfg.pin_oe  = GPIO_NUM_13; // OE
      cfg.pin_clk = GPIO_NUM_12; // CLK

      cfg.pin_addr_a = GPIO_NUM_27;
      cfg.pin_addr_b = GPIO_NUM_16;
      cfg.pin_addr_c = GPIO_NUM_14;
      cfg.pin_addr_d = GPIO_NUM_4 ;
      //cfg.pin_addr_e = GPIO_NUM_2;

      // データ送信速度
      cfg.freq_write = 13333334;

      // パネルの行選択の仕様に応じて指定する
      //cfg.address_mode = cfg.address_shiftreg;
       cfg.address_mode = cfg.address_binary;

      // LEDドライバチップの種類を指定する
      cfg.led_driver = cfg.led_driver_standard;
 //      cfg.led_driver = cfg.led_driver_FM6124;

      // DMA用のタスクの優先度 (FreeRTOSのタスク機能を使用)
      cfg.task_priority = 2;

      // DMA用のタスクに使用するCPUコア設定 (FreeRTOSのタスク機能を使用)
      cfg.task_pinned_core = PRO_CPU_NUM;
      // cfg.task_pinned_core = APP_CPU_NUM;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();

      // ここでパネルサイズを指定する
      // 複数枚使用する場合(Panel_HUB75_Multi使用時) は表示領域全体の縦横サイズを指定

#ifdef MULTI
      cfg.memory_width  = cfg.panel_width  = 128;
      cfg.memory_height = cfg.panel_height = 32;
#else
      cfg.memory_width  = cfg.panel_width  = 64;
      cfg.memory_height = cfg.panel_height = 32;
#endif

      _panel_instance.config(cfg);
      setPanel(&_panel_instance);
    }

#ifdef MULTI
// 複数枚使用する場合(Panel_HUB75_Multi使用時) この設定を行う
    {
      auto cfg = _panel_instance.config_detail();

      // 構成パネルの総枚数を指定
      cfg.panel_count = 2;

      // パネル1枚の幅を指定
      cfg.single_width = 64;

      // パネル1枚の高さを指定
      cfg.single_height = 32;

      _panel_instance.config_detail(cfg);

      // 各パネルの配置座標を設定する
      //  _panel_instance.setPanelPosition( パネル通し番号, 横座標, 縦座標, 回転方向);
      _panel_instance.setPanelPosition( 0,   0,   0,   0);
      _panel_instance.setPanelPosition( 1,  64,   0,   0);
      
    }
#endif
  }
};

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
#include "util.h"

const IPAddress ip(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

LGFX_HUB75 display;
static const lgfx::U8g2font jiskan24(jiskan24_data);
LGFX_Sprite sprite1(&display);
LGFX_Sprite sprite2(&display);
LGFX_Sprite sprite3(&display);
LGFX_Sprite sprite4(&display);
LGFX_Sprite sprite5(&display);
LGFX_Sprite sprite6(&display);

int offset_width = 32; // 32x64+32x32でつなげる時に32横にずれるため

static constexpr char text[] = "本日もお越しいただきありがとうございます！！";
static constexpr size_t textlen = sizeof(text) / sizeof(text[0]);
size_t textpos = 0;
int32_t cursor_x;

/*asyncwebserver*/
AsyncWebServer server(80);

//flag to use from web update to reboot the ESP
bool shouldReboot = false;

void setup()
{

  display.init();
  sprite6.createSprite(160,32);

  if(!LittleFS.begin(true)){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
  if (!WiFi.softAP(wificonfig::ssid, wificonfig::passphrase,1))
  {
    log_e("Soft AP creation failed.");
    while (1)
      ;
  }
  //WiFi.mode(WIFI_MODE_AP);
  //WiFi.softAP(wificonfig::ssid, wificonfig::passphrase, 1);
  delay(100);
  WiFi.softAPConfig(ip, ip, subnet);
  IPAddress myIP = WiFi.softAPIP();

  //server.serveStatic("/",LittleFS,"/www/");
  server.on("/",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(LittleFS,"/index.html",String(),false);
    Serial.printf("GET: / \n");
  });
  server.on("/",HTTP_POST,[](AsyncWebServerRequest *request){
    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      Serial.printf("HEADER[%s]: %s\n", request->headerName(i).c_str(), request->header(i).c_str());
    }
    Serial.printf("POST: %s \n",request->contentType());
    //POSTデータを取得
    String postData = "";
    if (request->hasParam("data", true)) {
      Serial.printf("name: %s",request->getParam("data",true)->name());
      Serial.printf("value: %s",request->getParam("data", true)->value());

      postData = request->getParam("data", true)->value();
    }
    Serial.printf(" %s\n",postData);
    //sprite6.setTextFont(&jiskan24);
    //sprite6.setTextSize(1.0);
    //sprite6.print(postData);
  });
  /*
  server.on("/",HTTP_POST,[](AsyncWebServerRequest *request) {
      request->send(200);
      Serial.printf("post file \n");
      int headers = request->headers();
      int i;
      for(i=0;i<headers;i++){
        Serial.printf("HEADER[%s]: %s\n", request->headerName(i).c_str(), request->header(i).c_str());
      }
    },NULL,[](AsyncWebServerRequest *request,uint8_t *data, size_t len, size_t index, size_t total){
      String postData = "";
      File file =LittleFS.open("/image.png","w");
      if (!file) {
        Serial.println("Failed to open file for writing");
        return;
      }
      Serial.printf("body: \n");
      for (size_t i = 0; i < len; i++) {
        //Serial.print((char)data[i]);
        postData += (char)data[i];
      }
      Serial.printf("%s",postData);
      if (file.print(postData)){
        Serial.println("File written");
      } else {
        Serial.println("Write failed");
      }
      file.close();
      Serial.printf("fin\n");

  });
  */
  server.onNotFound([](AsyncWebServerRequest *request) {
    if (request->method() == HTTP_OPTIONS) {
      request->send(200);
      Serial.printf("option \n");
    } else {
      request->send(404);
      Serial.printf("not found \n");
    }
  });

  server.begin();
  //server is setuped
  Serial.print("SSID= ");
  Serial.println(wificonfig::ssid);
  Serial.print("Fixed IP addr= ");
  Serial.println(myIP);
  Serial.println("Server started");
  /*
  sprite1.createSprite(144, 32);
  sprite2.createSprite(144, 32);
  sprite3.createSprite(display.width() + 24, 32);
  sprite4.createSprite(160, 32);
  sprite5.createSprite(160, 32);
  
  sprite1.drawPngFile(LittleFS, img_ko8000_led, 0, 0, 48, 32, 114 * 0 + 0, 32 * 0);
  sprite1.drawPngFile(LittleFS, img_ko8000_led, 48, 0, 96, 32, 144 * 0 + 48, 32 * 1);
  sprite2.drawPngFile(LittleFS, img_ko8000_led, 0, 0, 48, 32, 0, 0 + 32 * 1);
  sprite2.drawPngFile(LittleFS, img_ko8000_led, 48, 0, 96, 32, 48, 0 + 32 * 1);
  sprite2.drawPngFile(LittleFS, img_ko8000_led, 0, 0, 144, 32, 144 * 1 + 0, 32 * 5);
  sprite2.drawPngFile(LittleFS, img_ko8000_led, 48, 0, 96, 32, 144 * 0 + 48, 32 * 1);
  sprite3.setTextFont(&jiskan24);
  sprite3.setTextSize(1.0);
  sprite3.setTextWrap(false);
  sprite4.drawPngFile(LittleFS, img_uec, 0, 0, 160, 32, 0, 0);
  sprite5.drawPngFile(LittleFS, img_uec, 0, 0, 160, 32, 0, 32);
  */
  //     sprite6.drawPngFile(LittleFS,"image.png",0,0,160,32,0,0);
}

void loop()
{
  //sprite6.pushSprite(offset_width,0);
  delay(5000);
  //ws.cleanupClients();
  /*
  sprite1.pushSprite(offset_width, 0);
  delay(5000);
  sprite2.pushSprite(offset_width, 0);
  delay(5000);


  */

 /*
  sprite4.pushSprite(offset_width, 0);
  delay(5000);
  sprite5.pushSprite(offset_width, 0);
  delay(5000);
  display.fillScreen(TFT_BLACK);

  textpos = 0;
  cursor_x = display.width();
  while (cursor_x > 0)
  {
    sprite3.setCursor(cursor_x, 8);                               // カーソル位置を更新
    sprite3.scroll(-1, 0);                                        // キャンバスの内容を1ドット左にスクロール
    while (textpos < textlen && cursor_x <= display.width() - 32) // 画面右端に文字が書けるか判定
    {
      sprite3.print(text[textpos++]);  // 1バイトずつ出力 (マルチバイト文字でもこの処理で動作します)
      cursor_x = sprite3.getCursorX(); // 出力後のカーソル位置を取得
    }

    sprite3.pushSprite(&display, offset_width, 0);
    delay(20);
    cursor_x = sprite3.getCursorX() - 1;
  }
  sprite3.fillScreen(TFT_BLACK);
  */
}
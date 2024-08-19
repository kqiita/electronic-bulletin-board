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

int offset_width = 32; // 32x64+32x32でつなげる時に32横にずれるため

static constexpr char text[] = "本日もお越しいただきありがとうございます！！";
static constexpr size_t textlen = sizeof(text) / sizeof(text[0]);
size_t textpos = 0;
int32_t cursor_x;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    
    data[len] = 0;
    Serial.println((char *)data); 
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, (char *)data);
  }

}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  }

}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < info->len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < info->len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if(info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

      if(info->opcode == WS_TEXT){
        for(size_t i=0; i < len; i++) {
          msg += (char) data[i];
        }
      } else {
        char buff[3];
        for(size_t i=0; i < len; i++) {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }
      Serial.printf("%s\n",msg.c_str());

      if((info->index + len) == info->len){
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}



void setup()
{
  display.init();
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

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);


  server.on("/", HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(LittleFS,"/www/index.html",String());
  });
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
	  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });
  server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });


  server.begin();
  Serial.print("SSID= ");
  Serial.println(wificonfig::ssid);
  Serial.print("Fixed IP addr= ");
  Serial.println(myIP);
  Serial.println("Server started");

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
}

void loop()
{

  ws.cleanupClients();
  /*
  sprite1.pushSprite(offset_width, 0);
  delay(5000);
  sprite2.pushSprite(offset_width, 0);
  delay(5000);


  */
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
}
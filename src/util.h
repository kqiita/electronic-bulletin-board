
#define MULTI
#define LGFX_USE_V1

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <LovyanGFX.hpp>
#include "jiskan24.h"
#include "lgfx_hub75.hpp"

#include "wificonfig.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ArduinoJson.h>

#define img_ko8000_led "/KO8000-led.png"
#define img_uec "/for_uec_event_32x.png"
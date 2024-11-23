#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <stdio.h>
#include <string.h>
#include "jiskan24.h"
#include "lgfx_hub75.hpp"
#include <LovyanGFX.hpp>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>

#define MULTI
#define LGFX_USE_V1

#define img_ko8000_led "/KO8000-led.png"
#define img_uec "/for_uec_event_32x.png"

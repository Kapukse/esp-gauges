#include <Arduino.h>
#include <lvgl.h>
#include "DisplayConfig.h"


WaveshareDisplay gfx; //from /include/DisplayConfig.h

void setup() {
  Serial.begin(115200);

  //lovayangfx init
  gfx.init();
  gfx.setRotation(1);
  gfx.initBacklight(255);

  //lvgl init
  lv_init();
}

void loop() {
}

void dashboard() {
  
}
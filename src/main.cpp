#include <Arduino.h>
#include <lvgl.h>
#include "DisplayConfig.h"

WaveshareDisplay gfx;

//lvgl display flush callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx.pushImage(area->x1, area->y1, w, h, (lgfx::rgb565_t *)&color_p->full);

    lv_disp_flush_ready(disp); 
}

//def
lv_obj_t * uiRpmValue = NULL;
lv_obj_t * uiSpeedValue = NULL;
lv_obj_t * uiGearValue = NULL;
lv_obj_t * uiRpmLed[10];
lv_obj_t * uIAbsSt = NULL;
lv_obj_t * uiLapTime = NULL;
static char rxBuffer[64];
static size_t rxIndex = 0;
    
//read and parse data from simhub
void readSimhubData(){
    while (Serial.available() > 0){
        char a = Serial.read();
        if (a == '\n' || a == '\r') {
            if (rxIndex > 0) {
                rxBuffer[rxIndex] = '\0';
                int speed = 0;
                int rpm = 0;
                int rpmPercentage = 0;
                char gear[8] = "N";
                int abs = 0;
                int parsed = sscanf(rxBuffer, "%d;%d;%d;%7s;%d", &speed, &rpm, &rpmPercentage, gear, &abs);
                lv_label_set_text_fmt(uiSpeedValue, "%d", speed);
                lv_label_set_text_fmt(uiRpmValue, "%d", rpm);
                lv_label_set_text(uiGearValue, gear);
                if(abs == 1){
                    lv_obj_set_style_text_color(uIAbsSt, lv_color_hex(0xfc0b03), LV_PART_MAIN);
                }else{
                    lv_obj_set_style_text_color(uIAbsSt, lv_color_hex(0x687d9e), LV_PART_MAIN);                   
                }
                
                int numLedsOn = 0;
                if(rpmPercentage >= 10){
                    numLedsOn = rpmPercentage - 10;
                }
                if (numLedsOn > 10) numLedsOn = 10;

                for (int i = 0; i < 10; i++) {
                    if (i < numLedsOn) {
                        if (i < 4) {
                            lv_obj_set_style_bg_color(uiRpmLed[i], lv_color_hex(0x56fc03), LV_PART_MAIN);
                        }
                        else if (i < 7) {
                            lv_obj_set_style_bg_color(uiRpmLed[i], lv_color_hex(0xfcf403), LV_PART_MAIN);
                        }
                        else {
                            lv_obj_set_style_bg_color(uiRpmLed[i], lv_color_hex(0xfc0b03), LV_PART_MAIN);
                        }
                    }
                    else {
                        lv_obj_set_style_bg_color(uiRpmLed[i], lv_color_hex(0x687d9e), LV_PART_MAIN);
                    }
                }
            
                rxIndex = 0;
            }
        } 
            else {
                if (rxIndex < sizeof(rxBuffer) - 1) {
                    rxBuffer[rxIndex++] = a;
                } else {
                    rxIndex = 0;
                }
            }
        }
    }

void setup() {
    Serial.begin(115200);

    //lovyangfx init
    gfx.init();
    gfx.setRotation(1);
    gfx.initBacklight(255);

    //lvgl init
    lv_init();

    //framebuffer and memory allocation (wrtitten by ai)
    const size_t buf_pixels = gfx.width() * 45; 
    static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    static lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, buf_pixels);

    //display driver initialization and registration for lvgl
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = gfx.width();
    disp_drv.ver_res = gfx.height();
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    //ui

    //change the background color
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN);

    //drawing rpm gauge
    for (int i = 0; i < 10; i++){
        uiRpmLed[i] = lv_obj_create(lv_scr_act()); 
        lv_obj_remove_style_all(uiRpmLed[i]);    
        lv_obj_set_size(uiRpmLed[i], 40, 40);
        lv_obj_align(uiRpmLed[i], LV_ALIGN_CENTER, -225 + i * (40 + 10), -120);
        lv_obj_set_style_bg_color(uiRpmLed[i], lv_color_hex(0x687d9e), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(uiRpmLed[i], LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_radius(uiRpmLed[i], LV_RADIUS_CIRCLE, LV_PART_MAIN);        
    }


    //rpm value
    uiRpmValue = lv_label_create(lv_scr_act());
    lv_obj_align(uiRpmValue, LV_ALIGN_CENTER, 0, -50);
    lv_label_set_text(uiRpmValue, "1000");

    //rpm value style
    static lv_style_t style_RpmValue;
    lv_style_init(&style_RpmValue);
    lv_style_set_text_font(&style_RpmValue, &lv_font_montserrat_28);
    lv_style_set_text_color(&style_RpmValue, lv_color_hex(0xEAEAEA));
    lv_obj_add_style(uiRpmValue, &style_RpmValue, LV_PART_MAIN); 


    //speed value
    uiSpeedValue = lv_label_create(lv_scr_act());
    lv_obj_align(uiSpeedValue, LV_ALIGN_CENTER, -200, 0);
    lv_label_set_text(uiSpeedValue, "8");

    //speed value style
    static lv_style_t style_SpeedValue;
    lv_style_init(&style_SpeedValue);
    lv_style_set_text_font(&style_SpeedValue, &lv_font_montserrat_48);
    lv_style_set_text_color(&style_SpeedValue, lv_color_hex(0xEAEAEA));
    lv_obj_add_style(uiSpeedValue, &style_SpeedValue, LV_PART_MAIN); 


    //gear value
    uiGearValue = lv_label_create(lv_scr_act());
    lv_obj_align(uiGearValue, LV_ALIGN_CENTER, 200, 0);
    lv_label_set_text(uiGearValue, "N");

    //gear value style
    static lv_style_t style_GearValue;
    lv_style_init(&style_GearValue);
    lv_style_set_text_font(&style_GearValue, &lv_font_montserrat_48);
    lv_style_set_text_color(&style_GearValue, lv_color_hex(0xEAEAEA));
    lv_obj_add_style(uiGearValue, &style_GearValue, LV_PART_MAIN); 

    //abs status
    uIAbsSt = lv_label_create(lv_scr_act());
    lv_obj_align(uIAbsSt, LV_ALIGN_CENTER, 320, 110);
    lv_label_set_text(uIAbsSt, "abs");
    lv_obj_set_style_text_color(uIAbsSt, lv_color_hex(0x687d9e), LV_PART_MAIN);

} 

void loop() {
    readSimhubData();
    static uint32_t lastUpdate = 0;
  	if (millis() - lastUpdate >= 16) {
	   lastUpdate = millis();
            lv_timer_handler();
  	}
}

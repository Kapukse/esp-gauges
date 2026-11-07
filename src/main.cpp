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
lv_obj_t * uiRpmGauge = NULL;
lv_obj_t * uiRpmValue = NULL;
lv_obj_t * uiSpeedValue = NULL;
lv_obj_t * uiGearValue = NULL;
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
                int rpmPercent = 0;
                char gear[8] = "N";
                int parsed = sscanf(rxBuffer, "%d;%d;%d;%7s", &speed, &rpm, &rpmPercent, gear);
                if (parsed >= 3) {
                    lv_label_set_text_fmt(uiSpeedValue, "%d", speed);
                    lv_label_set_text_fmt(uiRpmValue, "%d", rpm);
                    lv_slider_set_value(uiRpmGauge, rpmPercent, LV_ANIM_OFF);
                    lv_label_set_text(uiGearValue, gear);
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
    const size_t buf_pixels = gfx.width() * 40;
    static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!buf1) {
        buf1 = (lv_color_t *)malloc(buf_pixels * sizeof(lv_color_t));
    }

    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, buf_pixels);

    //display driver initialization and registration for lvgl
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = gfx.width();
    disp_drv.ver_res = gfx.height();
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    //ui

    uiRpmGauge = lv_slider_create(lv_scr_act());
    uiRpmValue = lv_label_create(lv_scr_act());
    uiSpeedValue = lv_label_create(lv_scr_act());
    uiGearValue = lv_label_create(lv_scr_act());

    //change the background color
    lv_obj_set_style_bg_color(lv_scr_act(), lv_palette_darken(LV_PALETTE_GREY, 3), LV_PART_MAIN);

    //rpm gauge
    lv_obj_align(uiRpmGauge, LV_ALIGN_CENTER, 0, -100);
    lv_obj_set_size(uiRpmGauge, 600, 60);
    lv_slider_set_value(uiRpmGauge, 70, LV_ANIM_ON);
    lv_slider_set_range(uiRpmGauge, 0, 100);

    //rpm gauge background style    
    static lv_style_t style_RpmGaugeMain;
    lv_style_init(&style_RpmGaugeMain);
    lv_style_set_bg_color(&style_RpmGaugeMain, lv_color_hex(0x4DED2D));
    lv_style_set_bg_grad_color(&style_RpmGaugeMain, lv_color_hex(0xFF0000));
    lv_style_set_bg_grad_dir(&style_RpmGaugeMain, LV_GRAD_DIR_HOR);
    lv_style_set_bg_main_stop(&style_RpmGaugeMain, 100);
    lv_style_set_bg_grad_stop(&style_RpmGaugeMain, 225);
    lv_style_set_border_width(&style_RpmGaugeMain, 2);
    lv_style_set_border_color(&style_RpmGaugeMain, lv_color_black());
    lv_obj_add_style(uiRpmGauge, &style_RpmGaugeMain, LV_PART_MAIN);

    //rpm gauge indicator style
    static lv_style_t style_RpmGaugeInd;
    lv_style_init(&style_RpmGaugeInd);
    lv_style_set_bg_color(&style_RpmGaugeInd, lv_color_hex(0x4DED2D));
    lv_style_set_bg_grad_color(&style_RpmGaugeInd, lv_color_hex(0xFF0000));
    lv_style_set_bg_grad_dir(&style_RpmGaugeInd, LV_GRAD_DIR_HOR);
    lv_style_set_bg_main_stop(&style_RpmGaugeInd, 100);
    lv_style_set_bg_grad_stop(&style_RpmGaugeInd, 225);
    lv_style_set_border_width(&style_RpmGaugeInd, 2);
    lv_obj_add_style(uiRpmGauge, &style_RpmGaugeInd, LV_PART_INDICATOR);

    //diable knob in rpm gauge
    static lv_style_t diableRpmKnob;
    lv_style_init(&diableRpmKnob);
    lv_style_set_bg_opa(&diableRpmKnob, 0);
    lv_obj_add_style(uiRpmGauge, &diableRpmKnob, LV_PART_KNOB);


    //rpm value
    lv_obj_align(uiRpmValue, LV_ALIGN_CENTER, 0, -50);
    lv_label_set_text(uiRpmValue, "1000");

    //rpm value style
    static lv_style_t style_RpmValue;
    lv_style_init(&style_RpmValue);
    lv_style_set_text_font(&style_RpmValue, &lv_font_montserrat_28);
    lv_style_set_text_color(&style_RpmValue, lv_color_hex(0xEAEAEA));
    lv_obj_add_style(uiRpmValue, &style_RpmValue, LV_PART_MAIN); 


    //speed value
    lv_obj_align(uiSpeedValue, LV_ALIGN_CENTER, -200, 0);
    lv_label_set_text(uiSpeedValue, "8");

    //speed value style
    static lv_style_t style_SpeedValue;
    lv_style_init(&style_SpeedValue);
    lv_style_set_text_font(&style_SpeedValue, &lv_font_montserrat_48);
    lv_style_set_text_color(&style_SpeedValue, lv_color_hex(0xEAEAEA));
    lv_obj_add_style(uiSpeedValue, &style_SpeedValue, LV_PART_MAIN); 

    //gear value
    lv_obj_align(uiGearValue, LV_ALIGN_CENTER, 200, 0);
    lv_label_set_text(uiGearValue, "N");

    //gear value style
    static lv_style_t style_GearValue;
    lv_style_init(&style_GearValue);
    lv_style_set_text_font(&style_GearValue, &lv_font_montserrat_48);
    lv_style_set_text_color(&style_GearValue, lv_color_hex(0xEAEAEA));
    lv_obj_add_style(uiGearValue, &style_GearValue, LV_PART_MAIN); 
}

void loop() {
    readSimhubData();
    lv_timer_handler();
    delay(5);
}

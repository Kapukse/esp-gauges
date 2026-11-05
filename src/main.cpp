#include <Arduino.h>
#include <lvgl.h>
#include "DisplayConfig.h"

WaveshareDisplay gfx; // from /include/DisplayConfig.h

/* LVGL display flush callback */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx.pushImage(area->x1, area->y1, w, h, (lgfx::rgb565_t *)&color_p->full);

    lv_disp_flush_ready(disp); 
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
    //rpm gauge
    lv_obj_t * uiRpmSlider = lv_slider_create(lv_scr_act());
    lv_obj_set_x(uiRpmSlider, 0);
    lv_obj_set_y(uiRpmSlider, -100);
    lv_obj_set_size(uiRpmSlider, 600, 60);
    lv_slider_set_value(uiRpmSlider, 30, LV_ANIM_OFF);

    //rpm value
    lv_obj_t * uiRpmValue = lv_label_create(lv_scr_act());
    lv_obj_set_x(uiRpmValue, 0);
    lv_obj_set_y(uiRpmValue, -50);
    
    //speed value
    lv_obj_t * uiSpeedValue = lv_label_create(lv_scr_act());
    lv_obj_set_x(uiSpeedValue, -200);
    lv_obj_set_y(uiSpeedValue, 0);

    //gear value
    lv_obj_t * uiGearValue = lv_label_create(lv_scr_act());
    lv_obj_set_x(uiGearValue, 200);
    lv_obj_set_y(uiGearValue, 0);

}

void loop() {
    lv_timer_handler();
    delay(5);
}

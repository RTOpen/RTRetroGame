#include "lvgl.h"
#include "splash_ui.h"
#include "launcher_ui.h"
#include "calculator_ui.h"
#include "logo.h"
#include "stdio.h"

static lv_obj_t* process = NULL;
static lv_timer_t* timer = NULL;
static lv_obj_t* label1 = NULL;
static lv_obj_t* logo_img = NULL;

static void timer_cb(lv_timer_t* t)
{
    static uint8_t time=0;
    time++;
    if (time > 10)
    {
        //ui_calculator_init();
        //ui_splash_deinit();
        //main_ui_init();
       // ui_factory_init();
    }
    else
    {
        char buffer[64];
        lv_bar_set_value(process, time * 10, LV_ANIM_ON);
        sprintf(buffer, "Loding ... %d", time * 10);
        lv_label_set_text(label1, buffer);
    }

}

void ui_splash_init(void)
{  

    logo_img = lv_img_create(lv_scr_act());
    lv_img_set_src(logo_img, &logo);
    lv_obj_align(logo_img, LV_ALIGN_CENTER, 0, -20);
    process = lv_bar_create(lv_scr_act());
    lv_obj_set_size(process, LV_HOR_RES_MAX*3/4, 8);
    lv_obj_align(process, LV_ALIGN_CENTER, 0, 40);
    lv_bar_set_value(process, 0, LV_ANIM_OFF);
    timer = lv_timer_create(timer_cb, 100, NULL);
    label1 = lv_label_create(lv_scr_act());
    lv_obj_align(label1, LV_ALIGN_CENTER,0,0);       /*Center aligned lines*/
    lv_label_set_text(label1, "System initialize...");
    lv_obj_set_width(label1, 150);
    lv_obj_align(label1,  LV_ALIGN_CENTER, 0, 56);
    //lv_timer_start(timer);
}

void ui_splash_deinit(void) 
{
    lv_timer_pause(timer);
    lv_timer_del(timer);
    timer = NULL;
    lv_obj_del(logo_img);
    logo_img = NULL;
    lv_obj_del(process);
    process = NULL;
    lv_obj_del(label1);
    label1 = NULL;
}

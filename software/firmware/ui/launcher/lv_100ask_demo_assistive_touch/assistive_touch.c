/**
 * @file assistive_touch.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_100ask_modules.h"

#if LV_USE_100ASK_DEMO_ASSISTIVE_TOUCH

#include <stdio.h>
#include <signal.h>
#include <dbus/dbus.h>
#include "assistive_touch.h"

#define ASSISTIVE_TOUCH_SIZE (60)

static void assistive_touch_event_handler(lv_event_t * e);

#if 0
static void assistive_touch_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
#if 1
    if(code == LV_EVENT_CLICKED) {
        lv_obj_set_style_img_recolor_opa(obj, LV_OPA_100, 0);
        lv_obj_set_style_img_recolor(obj, lv_color_hex(0x515151), 0);
        dbus_method_call("net.ask100.lvgl.main", "/net/ask100/lvgl/main", "net.ask100.lvgl.main", "states", 1, getpid());
    }
#endif
#if 0

    static int flag = 1;
    if(code == LV_EVENT_CLICKED) {
        if(1 == flag)
        {
            lv_obj_set_style_img_recolor_opa(obj, LV_OPA_100, 0);
            lv_obj_set_style_img_recolor(obj, lv_color_hex(0x515151), 0);
            flag = 0;
        }
        else
        {
            lv_obj_set_style_img_recolor_opa(obj, LV_OPA_100, 0);
            lv_obj_set_style_img_recolor(obj, lv_color_hex(0xbfbfbf), 0);
            flag = 1;
        }
    }
#endif    
}
#endif 



static void assistive_touch_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        printf("assistive_touch_event_handler LV_EVENT_CLICKED\n");
        dbus_method_call("net.ask100.lvgl.Main", "/net/ask100/lvgl/Main", "net.ask100.lvgl.Main", "states", 1, getpid());
    }
#if 0
    else if(code == LV_EVENT_PRESSING) {

        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect);

        lv_coord_t x = lv_obj_get_x(obj) + vect.x;
        lv_coord_t y = lv_obj_get_y(obj) + vect.y;
        lv_obj_set_pos(obj, x, y);
    }
#endif    
}


static void assistive_touch_drag_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);

    lv_coord_t x = lv_obj_get_x(obj) + vect.x;
    lv_coord_t y = lv_obj_get_y(obj) + vect.y;
    lv_obj_set_pos(obj, x, y);   
}

void assistive_touch_init(void)
{

#if 0
    lv_obj_t * img = lv_img_create(lv_layer_top());
	lv_img_set_src(img, "/root/assets/assistive_pic.png");
    lv_obj_align(img, LV_ALIGN_RIGHT_MID, -50, -200);
    lv_obj_add_flag(img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(img, assistive_touch_event_handler, LV_EVENT_CLICKED, NULL);
#endif

#if 1
    /*Init the style for the default state*/
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_radius(&style, 360);

    lv_style_set_bg_opa(&style, LV_OPA_100);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_GREEN, 2));
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

    lv_style_set_border_opa(&style, LV_OPA_40);
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

    //lv_style_set_shadow_width(&style, 8);
    //lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
    //lv_style_set_shadow_ofs_y(&style, 8);

    lv_style_set_outline_opa(&style, LV_OPA_COVER);
    lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_GREEN));

    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_pad_all(&style, 10);

    /*Init the pressed style*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Ad a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

    lv_style_set_translate_y(&style_pr, 5);
    //lv_style_set_shadow_ofs_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_GREEN, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_GREEN, 4));

    /*Add a transition to the the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);
    lv_style_set_transition(&style_pr, &trans);

    lv_obj_t * btn1 = lv_btn_create(lv_layer_top());
    lv_obj_remove_style_all(btn1);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(btn1, &style, 0);
    lv_obj_add_style(btn1, &style_pr, LV_STATE_PRESSED);
    //lv_obj_set_size(btn1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_size(btn1, ASSISTIVE_TOUCH_SIZE, ASSISTIVE_TOUCH_SIZE);
    //lv_obj_center(btn1);
    lv_obj_align(btn1, LV_ALIGN_RIGHT_MID, -60, -200);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(btn1, assistive_touch_event_handler, LV_EVENT_CLICKED, NULL);
    //lv_obj_add_event_cb(btn1, assistive_touch_drag_event_handler, LV_EVENT_PRESSING, NULL);

    lv_obj_t * label = lv_label_create(btn1);
    lv_label_set_text(label, "100ASK");
    lv_obj_center(label);

#endif

}


#endif


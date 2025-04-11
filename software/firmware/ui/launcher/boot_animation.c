/**
 * @file lv_100ask_boot_animation.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_100ask_modules.h"

#if LV_USE_BOOT_ANIMATION

#include <stdio.h>
#include "lv_100ask_boot_animation.h"

static void set_value(void *bar, int32_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_OFF);
}

void lv_100ask_boot_animation(void (*end_cb)(lv_anim_t *), uint32_t boot_time)
{
    static lv_style_t style_logo;
    static lv_style_t style_bar_bg;
    static lv_style_t style_indic;

    /**/
    lv_style_init(&style_logo);
    lv_style_set_radius(&style_logo, 20);
    lv_style_set_bg_opa(&style_logo, 0);
    lv_style_set_bg_color(&style_logo, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_color(&style_logo, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_logo, LV_GRAD_DIR_VER);

    lv_style_set_border_opa(&style_logo, LV_OPA_0);
    lv_style_set_border_width(&style_logo, 2);
    lv_style_set_border_color(&style_logo, lv_palette_main(LV_PALETTE_GREY));

    lv_style_set_outline_opa(&style_logo, LV_OPA_COVER);
    lv_style_set_outline_color(&style_logo, lv_palette_main(LV_PALETTE_GREEN));

    lv_style_set_text_color(&style_logo, lv_color_white());
    lv_style_set_pad_all(&style_logo, 10);

    /*Init the pressed style*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Ad a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_GREEN, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_GREEN, 4));

    /*Add a transition to the the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);
    lv_style_set_transition(&style_pr, &trans);

    /**/
    lv_style_init(&style_bar_bg);
    lv_style_set_border_color(&style_bar_bg, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_border_width(&style_bar_bg, 2);
    lv_style_set_pad_all(&style_bar_bg, 5); /*To make the indicator smaller*/
    lv_style_set_radius(&style_bar_bg, 6);

    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_radius(&style_indic, 3);

    /* 主面板 */
    lv_obj_t * panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(panel, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_radius(panel, 0, 0);

    lv_obj_t * bar = lv_bar_create(panel);
    lv_obj_add_style(bar, &style_bar_bg, 0);
    lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(bar, LV_PCT(40), 15);
    lv_obj_align(bar, LV_ALIGN_CENTER, 0, 20);

    lv_obj_t * btn_logo = lv_btn_create(panel);
    lv_obj_set_size(btn_logo, 158,41);
    lv_obj_remove_style_all(btn_logo);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(btn_logo, &style_logo, 0);
    lv_obj_add_style(btn_logo, &style_pr, LV_STATE_PRESSED);
    //lv_obj_align_to(btn_logo, bar, LV_ALIGN_OUT_TOP_MID, 0, 0);
    lv_obj_align(btn_logo, LV_ALIGN_CENTER, 0, -30);


    lv_obj_t * img = lv_img_create(btn_logo);
    lv_img_set_src(img, "./100ask_logo.png");

    lv_anim_t a_bar;
    lv_anim_init(&a_bar);
    lv_anim_set_var(&a_bar, bar);
    lv_anim_set_values(&a_bar, 0, 100);
    lv_anim_set_exec_cb(&a_bar, set_value);
    lv_anim_set_ready_cb(&a_bar, end_cb);
    lv_anim_set_time(&a_bar, boot_time);
    //lv_anim_set_playback_time(&a_bar, 2000);
    lv_anim_start(&a_bar);
}


#endif

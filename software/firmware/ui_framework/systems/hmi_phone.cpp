/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <algorithm>
#include "esp_brookesia_phone_type.h"
#include "esp_brookesia_phone.hpp"

#if !ESP_BROOKESIA_LOG_ENABLE_DEBUG_PHONE_PHONE
#undef ESP_BROOKESIA_LOGD
#define ESP_BROOKESIA_LOGD(...)
#endif

using namespace std;

bool  inited = false;
ESP_Brookesia_PhoneStylesheet_t ESP_Brookesia_Phone::_default_stylesheet_dark = { 0 };//ESP_BROOKESIA_PHONE_DEFAULT_DARK_STYLESHEET();

ESP_Brookesia_Phone::ESP_Brookesia_Phone(lv_disp_t *display):
    ESP_Brookesia_Core(_active_stylesheet.core, _home, _manager, display),
    ESP_Brookesia_PhoneStylesheet(),
    _home(*this, _active_stylesheet.home),
    _manager(*this, _home, _active_stylesheet.manager)
{
    if(!inited)
    {
        _default_stylesheet_dark.core.name = "Default Dark";
        _default_stylesheet_dark.core.screen_size.width_percent = 100;
        _default_stylesheet_dark.core.screen_size.height_percent = 100;
        _default_stylesheet_dark.core.screen_size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.core.screen_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.core.home.background.color.color = (0x1A1A1A);
        _default_stylesheet_dark.core.home.background.color.opacity = 255;
        _default_stylesheet_dark.core.home.text.default_fonts_num = 21;
        _default_stylesheet_dark.core.home.text.default_fonts[0].size_px = 8;
        _default_stylesheet_dark.core.home.text.default_fonts[0].font_resource = &esp_brookesia_font_maison_neue_book_8;
        _default_stylesheet_dark.core.home.text.default_fonts[1].size_px = 10;
        _default_stylesheet_dark.core.home.text.default_fonts[1].font_resource = &esp_brookesia_font_maison_neue_book_10;
        _default_stylesheet_dark.core.home.text.default_fonts[2].size_px = 12;
        _default_stylesheet_dark.core.home.text.default_fonts[2].font_resource = &esp_brookesia_font_maison_neue_book_12;
        _default_stylesheet_dark.core.home.text.default_fonts[3].size_px = 14;
        _default_stylesheet_dark.core.home.text.default_fonts[3].font_resource = &esp_brookesia_font_maison_neue_book_14;
        _default_stylesheet_dark.core.home.text.default_fonts[4].size_px = 16;
        _default_stylesheet_dark.core.home.text.default_fonts[4].font_resource = &esp_brookesia_font_maison_neue_book_16;
        _default_stylesheet_dark.core.home.text.default_fonts[5].size_px = 18;
        _default_stylesheet_dark.core.home.text.default_fonts[5].font_resource = &esp_brookesia_font_maison_neue_book_18;
        _default_stylesheet_dark.core.home.text.default_fonts[6].size_px = 20;
        _default_stylesheet_dark.core.home.text.default_fonts[6].font_resource = &esp_brookesia_font_maison_neue_book_20;
        _default_stylesheet_dark.core.home.text.default_fonts[7].size_px = 22;
        _default_stylesheet_dark.core.home.text.default_fonts[7].font_resource = &esp_brookesia_font_maison_neue_book_22;
        _default_stylesheet_dark.core.home.text.default_fonts[8].size_px = 24;
        _default_stylesheet_dark.core.home.text.default_fonts[8].font_resource = &esp_brookesia_font_maison_neue_book_24;
        _default_stylesheet_dark.core.home.text.default_fonts[9].size_px = 26;
        _default_stylesheet_dark.core.home.text.default_fonts[9].font_resource = &esp_brookesia_font_maison_neue_book_26;
        _default_stylesheet_dark.core.home.text.default_fonts[10].size_px = 28;
        _default_stylesheet_dark.core.home.text.default_fonts[10].font_resource = &esp_brookesia_font_maison_neue_book_28;
        _default_stylesheet_dark.core.home.text.default_fonts[11].size_px = 30;
        _default_stylesheet_dark.core.home.text.default_fonts[11].font_resource = &esp_brookesia_font_maison_neue_book_30;
        _default_stylesheet_dark.core.home.text.default_fonts[12].size_px = 32;
        _default_stylesheet_dark.core.home.text.default_fonts[12].font_resource = &esp_brookesia_font_maison_neue_book_32;
        _default_stylesheet_dark.core.home.text.default_fonts[13].size_px = 34;
        _default_stylesheet_dark.core.home.text.default_fonts[13].font_resource = &esp_brookesia_font_maison_neue_book_34;
        _default_stylesheet_dark.core.home.text.default_fonts[14].size_px = 36;
        _default_stylesheet_dark.core.home.text.default_fonts[14].font_resource = &esp_brookesia_font_maison_neue_book_36;
        _default_stylesheet_dark.core.home.text.default_fonts[15].size_px = 38;
        _default_stylesheet_dark.core.home.text.default_fonts[15].font_resource = &esp_brookesia_font_maison_neue_book_38;
        _default_stylesheet_dark.core.home.text.default_fonts[16].size_px = 40;
        _default_stylesheet_dark.core.home.text.default_fonts[16].font_resource = &esp_brookesia_font_maison_neue_book_40;
        _default_stylesheet_dark.core.home.text.default_fonts[17].size_px = 42;
        _default_stylesheet_dark.core.home.text.default_fonts[17].font_resource = &esp_brookesia_font_maison_neue_book_42;
        _default_stylesheet_dark.core.home.text.default_fonts[18].size_px = 44;
        _default_stylesheet_dark.core.home.text.default_fonts[18].font_resource = &esp_brookesia_font_maison_neue_book_44;
        _default_stylesheet_dark.core.home.text.default_fonts[19].size_px = 46;
        _default_stylesheet_dark.core.home.text.default_fonts[19].font_resource = &esp_brookesia_font_maison_neue_book_46;
        _default_stylesheet_dark.core.home.text.default_fonts[20].size_px = 48;
        _default_stylesheet_dark.core.home.text.default_fonts[20].font_resource = &esp_brookesia_font_maison_neue_book_48;
        _default_stylesheet_dark.core.home.container.styles[0].outline_width = 1;
        _default_stylesheet_dark.core.home.container.styles[0].outline_color.color = 0xeb3b5a;
        _default_stylesheet_dark.core.home.container.styles[0].outline_color.opacity = 255;
        _default_stylesheet_dark.core.home.container.styles[1].outline_width = 2;
        _default_stylesheet_dark.core.home.container.styles[1].outline_color.color = 0xfa8231;
        _default_stylesheet_dark.core.home.container.styles[1].outline_color.opacity = 255;
        _default_stylesheet_dark.core.home.container.styles[2].outline_width = 1;
        _default_stylesheet_dark.core.home.container.styles[2].outline_color.color = 0xf7b731;
        _default_stylesheet_dark.core.home.container.styles[2].outline_color.opacity = 255;
        _default_stylesheet_dark.core.home.container.styles[3].outline_width = 2;
        _default_stylesheet_dark.core.home.container.styles[3].outline_color.color = 0x20bf6b;
        _default_stylesheet_dark.core.home.container.styles[3].outline_color.opacity = 255;
        _default_stylesheet_dark.core.home.container.styles[4].outline_width = 1;
        _default_stylesheet_dark.core.home.container.styles[4].outline_color.color = 0x0fb9b1;
        _default_stylesheet_dark.core.home.container.styles[4].outline_color.opacity = 255;
        _default_stylesheet_dark.core.home.container.styles[5].outline_width = 2;
        _default_stylesheet_dark.core.home.container.styles[5].outline_color.color = 0x2d98da;
        _default_stylesheet_dark.core.home.container.styles[5].outline_color.opacity = 255;
        _default_stylesheet_dark.core.manager.app.max_running_num = 3;
        _default_stylesheet_dark.core.manager.flags.enable_app_save_snapshot = 1;
        _default_stylesheet_dark.home.status_bar.data.main.size.height_percent = 100;
        _default_stylesheet_dark.home.status_bar.data.main.size.width_percent = 100;
        _default_stylesheet_dark.home.status_bar.data.main.size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.main.size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.main.size_min.height = 24;
        _default_stylesheet_dark.home.status_bar.data.main.size_min.width_percent = 100;
        _default_stylesheet_dark.home.status_bar.data.main.size_min.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.main.size_max.height = 50;
        _default_stylesheet_dark.home.status_bar.data.main.size_max.width_percent = 100;
        _default_stylesheet_dark.home.status_bar.data.main.size_max.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.main.background_color.color = 0x38393A;
        _default_stylesheet_dark.home.status_bar.data.main.background_color.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.main.text_font.height_percent = 60;
        _default_stylesheet_dark.home.status_bar.data.main.text_font.flags.enable_height =  1;
        _default_stylesheet_dark.home.status_bar.data.main.text_font.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.main.text_color.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.main.text_color.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.area.num = 2;
        _default_stylesheet_dark.home.status_bar.data.area.data[0].size.width_percent = 50;
        _default_stylesheet_dark.home.status_bar.data.area.data[0].size.height_percent = 100;
        _default_stylesheet_dark.home.status_bar.data.area.data[0].size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.area.data[0].size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.area.data[0].layout_column_align = ESP_BROOKESIA_STATUS_BAR_AREA_ALIGN_START;
        _default_stylesheet_dark.home.status_bar.data.area.data[0].layout_column_start_offset = 10;
        _default_stylesheet_dark.home.status_bar.data.area.data[0].layout_column_pad = 3;
        _default_stylesheet_dark.home.status_bar.data.area.data[1].size.width_percent = 50;
        _default_stylesheet_dark.home.status_bar.data.area.data[1].size.height_percent = 100;
        _default_stylesheet_dark.home.status_bar.data.area.data[1].size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.area.data[1].size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.area.data[1].layout_column_align = ESP_BROOKESIA_STATUS_BAR_AREA_ALIGN_END;
        _default_stylesheet_dark.home.status_bar.data.area.data[1].layout_column_start_offset = 10;
        _default_stylesheet_dark.home.status_bar.data.area.data[1].layout_column_pad = 3;
        _default_stylesheet_dark.home.status_bar.data.icon_common_size.width_percent = 60;
        _default_stylesheet_dark.home.status_bar.data.icon_common_size.height_percent = 60;
        _default_stylesheet_dark.home.status_bar.data.icon_common_size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.icon_common_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.status_bar.data.icon_common_size.flags.enable_square = 1;
        _default_stylesheet_dark.home.status_bar.data.battery.area_index = 1;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.image_num = 5;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[0].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[0].recolor.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[0].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[0].resource = &esp_brookesia_image_large_status_bar_battery_level1_36_36;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[1].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[1].recolor.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[1].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[1].resource = &esp_brookesia_image_large_status_bar_battery_level2_36_36;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[2].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[2].recolor.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[2].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[2].resource = &esp_brookesia_image_large_status_bar_battery_level3_36_36;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[3].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[3].recolor.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[3].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[3].resource = &esp_brookesia_image_large_status_bar_battery_level4_36_36;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[4].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[4].recolor.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[4].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.status_bar.data.battery.icon_data.icon.images[4].resource = &esp_brookesia_image_large_status_bar_battery_charge_36_36;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.image_num = 4;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[0].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[0].recolor.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[0].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[0].resource = &esp_brookesia_image_large_status_bar_wifi_close_36_36;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[1].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[1].recolor.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[1].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[1].resource = &esp_brookesia_image_large_status_bar_wifi_level1_36_36;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[2].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[2].recolor.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[2].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[2].resource = &esp_brookesia_image_large_status_bar_wifi_level2_36_36;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[3].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[3].recolor.opacity = 255;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[3].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.status_bar.data.wifi.icon_data.icon.images[3].resource = &esp_brookesia_image_large_status_bar_wifi_level3_36_36;
        _default_stylesheet_dark.home.status_bar.data.flags.enable_main_size_max = 1;
        _default_stylesheet_dark.home.status_bar.data.flags.enable_main_size_min = 1;
        _default_stylesheet_dark.home.status_bar.data.flags.enable_battery_icon = 1;
        _default_stylesheet_dark.home.status_bar.data.flags.enable_battery_icon_common_size = 1;
        _default_stylesheet_dark.home.status_bar.data.flags.enable_wifi_icon = 1;
        _default_stylesheet_dark.home.status_bar.data.flags.enable_wifi_icon_common_size = 1;
        _default_stylesheet_dark.home.status_bar.data.flags.enable_clock = 1;
        _default_stylesheet_dark.home.status_bar.visual_mode = ESP_BROOKESIA_STATUS_BAR_VISUAL_MODE_SHOW_FIXED;
        _default_stylesheet_dark.home.navigation_bar.data.main.size.width_percent = 100;
        _default_stylesheet_dark.home.navigation_bar.data.main.size.height_percent = 15;
        _default_stylesheet_dark.home.navigation_bar.data.main.size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.navigation_bar.data.main.size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.navigation_bar.data.main.size_min.width_percent = 100;
        _default_stylesheet_dark.home.navigation_bar.data.main.size_min.height = 36;
        _default_stylesheet_dark.home.navigation_bar.data.main.size_min.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.navigation_bar.data.main.size_max.width_percent = 100;
        _default_stylesheet_dark.home.navigation_bar.data.main.size_max.height = 60;
        _default_stylesheet_dark.home.navigation_bar.data.main.size_max.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.navigation_bar.data.main.background_color.color = 0x38393A;
        _default_stylesheet_dark.home.navigation_bar.data.main.background_color.opacity = 255;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_size.width_percent = 50;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_size.height_percent = 50;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_size.flags.enable_square = 1;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[0].resource = &esp_brookesia_image_large_navigation_bar_back_36_36;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[0].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[0].recolor.opacity = 255;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[0].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[1].resource = &esp_brookesia_image_large_navigation_bar_home_36_36;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[1].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[1].recolor.opacity = 255;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[1].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[2].resource = &esp_brookesia_image_large_navigation_bar_recents_screen_36_36;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[2].recolor.color = 0xFFFFFF;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[2].recolor.opacity = 255;
        _default_stylesheet_dark.home.navigation_bar.data.button.icon_images[2].flags.enable_recolor = 1;
        _default_stylesheet_dark.home.navigation_bar.data.button.navigate_types[0] = ESP_BROOKESIA_CORE_NAVIGATE_TYPE_BACK;
        _default_stylesheet_dark.home.navigation_bar.data.button.navigate_types[1] = ESP_BROOKESIA_CORE_NAVIGATE_TYPE_HOME;
        _default_stylesheet_dark.home.navigation_bar.data.button.navigate_types[2] = ESP_BROOKESIA_CORE_NAVIGATE_TYPE_RECENTS_SCREEN;
        _default_stylesheet_dark.home.navigation_bar.data.button.active_background_color.color = 0xFFFFFF;
        _default_stylesheet_dark.home.navigation_bar.data.button.active_background_color.opacity = LV_OPA_50;
        _default_stylesheet_dark.home.navigation_bar.data.visual_flex.show_animation_time_ms = 200;
        _default_stylesheet_dark.home.navigation_bar.data.visual_flex.show_animation_delay_ms = 0;
        _default_stylesheet_dark.home.navigation_bar.data.visual_flex.show_animation_path_type = ESP_BROOKESIA_LV_ANIM_PATH_TYPE_EASE_OUT;
        _default_stylesheet_dark.home.navigation_bar.data.visual_flex.show_duration_ms = 2000;
        _default_stylesheet_dark.home.navigation_bar.data.visual_flex.hide_animation_time_ms = 200;
        _default_stylesheet_dark.home.navigation_bar.data.visual_flex.hide_animation_delay_ms = 0;
        _default_stylesheet_dark.home.navigation_bar.data.visual_flex.hide_animation_path_type = ESP_BROOKESIA_LV_ANIM_PATH_TYPE_EASE_IN;
        _default_stylesheet_dark.home.navigation_bar.data.flags.enable_main_size_max = 1;
        _default_stylesheet_dark.home.navigation_bar.data.flags.enable_main_size_min = 1;
        _default_stylesheet_dark.home.navigation_bar.visual_mode = ESP_BROOKESIA_NAVIGATION_BAR_VISUAL_MODE_HIDE;
        _default_stylesheet_dark.home.app_launcher.data.main.y_start = 0;
        _default_stylesheet_dark.home.app_launcher.data.main.size.width_percent = 100;
        _default_stylesheet_dark.home.app_launcher.data.main.size.height_percent = 100;
        _default_stylesheet_dark.home.app_launcher.data.main.size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.app_launcher.data.main.size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.app_launcher.data.table.default_num = 3;
        _default_stylesheet_dark.home.app_launcher.data.table.size.width_percent = 100;
        _default_stylesheet_dark.home.app_launcher.data.table.size.height_percent = 70;
        _default_stylesheet_dark.home.app_launcher.data.table.size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.app_launcher.data.table.size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.app_launcher.data.indicator.main_size.height = 20;
        _default_stylesheet_dark.home.app_launcher.data.indicator.main_size.width_percent = 100;
        _default_stylesheet_dark.home.app_launcher.data.indicator.main_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.app_launcher.data.indicator.main_layout_column_pad = 10;
        _default_stylesheet_dark.home.app_launcher.data.indicator.main_layout_bottom_offset = 30;
        _default_stylesheet_dark.home.app_launcher.data.indicator.spot_inactive_size.width = 12;
        _default_stylesheet_dark.home.app_launcher.data.indicator.spot_inactive_size.height = 12;
        _default_stylesheet_dark.home.app_launcher.data.indicator.spot_active_size.width = 40;
        _default_stylesheet_dark.home.app_launcher.data.indicator.spot_active_size.height = 12;
        _default_stylesheet_dark.home.app_launcher.data.indicator.spot_inactive_background_color.color = 0xC6C6C6;
        _default_stylesheet_dark.home.app_launcher.data.indicator.spot_inactive_background_color.opacity = 255;
        _default_stylesheet_dark.home.app_launcher.data.indicator.spot_active_background_color.color = 0xFFFFFF;
        _default_stylesheet_dark.home.app_launcher.data.indicator.spot_active_background_color.opacity = 255;
        _default_stylesheet_dark.home.app_launcher.data.icon.main.size.width = 140;
        _default_stylesheet_dark.home.app_launcher.data.icon.main.size.height = 140;
        _default_stylesheet_dark.home.app_launcher.data.icon.main.layout_row_pad = 10;
        _default_stylesheet_dark.home.app_launcher.data.icon.image.default_size.width = 98;
        _default_stylesheet_dark.home.app_launcher.data.icon.image.default_size.height = 98;
        _default_stylesheet_dark.home.app_launcher.data.icon.image.press_size.width = 98;
        _default_stylesheet_dark.home.app_launcher.data.icon.image.press_size.height = 98;
        _default_stylesheet_dark.home.app_launcher.data.icon.label.text_font.size_px = 16;
        _default_stylesheet_dark.home.app_launcher.data.icon.label.text_color.color = 0xFFFFFF;
        _default_stylesheet_dark.home.app_launcher.data.icon.label.text_color.opacity = 255;
        _default_stylesheet_dark.home.app_launcher.data.flags.enable_table_scroll_anim = 0;
        _default_stylesheet_dark.home.app_launcher.default_image.resource = &esp_brookesia_image_small_app_launcher_default_98_98;
        _default_stylesheet_dark.home.recents_screen.data.main.y_start = 0;
        _default_stylesheet_dark.home.recents_screen.data.main.size.width_percent = 100;
        _default_stylesheet_dark.home.recents_screen.data.main.size.height_percent = 100;
        _default_stylesheet_dark.home.recents_screen.data.main.size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.main.size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.main.layout_row_pad = 10;
        _default_stylesheet_dark.home.recents_screen.data.main.layout_top_pad = 10;
        _default_stylesheet_dark.home.recents_screen.data.main.layout_bottom_pad = 10;
        _default_stylesheet_dark.home.recents_screen.data.main.background_color.color = 0x1A1A1A;
        _default_stylesheet_dark.home.recents_screen.data.main.background_color.opacity = 255;
        _default_stylesheet_dark.home.recents_screen.data.memory.main_size.height = 16;
        _default_stylesheet_dark.home.recents_screen.data.memory.main_size.width_percent = 100;
        _default_stylesheet_dark.home.recents_screen.data.memory.main_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.memory.main_layout_x_right_offset = 10;
        _default_stylesheet_dark.home.recents_screen.data.memory.label_text_font.height_percent = 100;
        _default_stylesheet_dark.home.recents_screen.data.memory.label_text_font.flags.enable_height = 1;
        _default_stylesheet_dark.home.recents_screen.data.memory.label_text_font.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.memory.label_text_color.color = 0xFFFFFF;
        _default_stylesheet_dark.home.recents_screen.data.memory.label_text_color.opacity = 255;
        _default_stylesheet_dark.home.recents_screen.data.memory.label_unit_text = "KB";
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.main_size.width_percent = 100;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.main_size.height_percent = 100;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.main_size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.main_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.main_layout_column_pad = 20;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.main_size.height_percent = 47;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.main_size.width_percent = 40;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.main_size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.main_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.main_size.height = 16;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.main_size.width_percent = 40;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.main_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.main_layout_column_pad = 10;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.icon_size.width_percent = 100;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.icon_size.height_percent = 100;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.icon_size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.icon_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.text_font.height_percent = 80;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.text_font.flags.enable_height = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.text_font.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.text_color.color = 0xFFFFFF;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.title.text_color.opacity = 255;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.image.main_size.height_percent = 40;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.image.main_size.width_percent = 40;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.image.main_size.flags.enable_width_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.image.main_size.flags.enable_height_percent = 1;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.image.radius = 20;
        _default_stylesheet_dark.home.recents_screen.data.snapshot_table.snapshot.flags.enable_all_main_size_refer_screen = 1;
        _default_stylesheet_dark.home.recents_screen.data.trash_icon.default_size.width = 48;
        _default_stylesheet_dark.home.recents_screen.data.trash_icon.default_size.height = 48;
        _default_stylesheet_dark.home.recents_screen.data.trash_icon.press_size.width = 43;
        _default_stylesheet_dark.home.recents_screen.data.trash_icon.press_size.height = 43;
        _default_stylesheet_dark.home.recents_screen.data.trash_icon.image.resource = &esp_brookesia_image_large_recents_screen_trash_64_64;
        _default_stylesheet_dark.home.recents_screen.data.flags.enable_memory = 1;
        _default_stylesheet_dark.home.recents_screen.data.flags.enable_table_height_flex = 1;
        _default_stylesheet_dark.home.recents_screen.data.flags.enable_table_snapshot_use_icon_image = 0;
        _default_stylesheet_dark.home.recents_screen.data.flags.enable_table_scroll_anim = 0;
        _default_stylesheet_dark.home.recents_screen.status_bar_visual_mode = ESP_BROOKESIA_STATUS_BAR_VISUAL_MODE_HIDE;
        _default_stylesheet_dark.home.recents_screen.navigation_bar_visual_mode = ESP_BROOKESIA_NAVIGATION_BAR_VISUAL_MODE_HIDE;
        _default_stylesheet_dark.home.flags.enable_status_bar = 1;
        _default_stylesheet_dark.home.flags.enable_navigation_bar = 1;
        _default_stylesheet_dark.home.flags.enable_app_launcher_flex_size = 1;
        _default_stylesheet_dark.home.flags.enable_recents_screen = 1;
        _default_stylesheet_dark.home.flags.enable_recents_screen_flex_size = 1;
        _default_stylesheet_dark.manager.gesture.detect_period_ms = 20;
        _default_stylesheet_dark.manager.gesture.threshold.direction_vertical = 50;
        _default_stylesheet_dark.manager.gesture.threshold.direction_horizon = 50;
        _default_stylesheet_dark.manager.gesture.threshold.direction_angle = 60;
        _default_stylesheet_dark.manager.gesture.threshold.horizontal_edge = 10;
        _default_stylesheet_dark.manager.gesture.threshold.vertical_edge = 20;
        _default_stylesheet_dark.manager.gesture.threshold.duration_short_ms = 800;
        _default_stylesheet_dark.manager.gesture.threshold.speed_slow_px_per_ms = 0.1;

        .manager = { .gesture = {.detect_period_ms = 20, 
                                 .threshold = {.direction_vertical = 50, .direction_horizon = 50, .direction_angle = 60, .horizontal_edge = 10, .vertical_edge = 20, .duration_short_ms = 800, .speed_slow_px_per_ms = 0.1, }, 
                                 .indicator_bars = { [ESP_BROOKESIA_GESTURE_INDICATOR_BAR_TYPE_LEFT] = {.main = {.size_min = {.width = 10, .height = 0, }, .size_max = {.width = 10, .height_percent = 50, .flags = {.enable_height_percent = 1, }, }, .radius = 5, .layout_pad_all = 2, .color = {.color = 0x000000, .opacity = 255, }, }, .indicator = {.radius = 5, .color = {.color = 0xFFFFFF, .opacity = 255, }, }, .animation = {.scale_back_path_type = ESP_BROOKESIA_LV_ANIM_PATH_TYPE_BOUNCE, .scale_back_time_ms = 500, }, }
                                                    ,[ESP_BROOKESIA_GESTURE_INDICATOR_BAR_TYPE_RIGHT] = {.main = {.size_min = {.width = 10, .height = 0, }, .size_max = {.width = 10, .height_percent = 50, .flags = {.enable_height_percent = 1, }, }, .radius = 5, .layout_pad_all = 2, .color = {.color = 0x000000, .opacity = 255, }, }, .indicator = {.radius = 5, .color = {.color = 0xFFFFFF, .opacity = 255, }, }, .animation = {.scale_back_path_type = ESP_BROOKESIA_LV_ANIM_PATH_TYPE_BOUNCE, .scale_back_time_ms = 500, }, },
                                                     [ESP_BROOKESIA_GESTURE_INDICATOR_BAR_TYPE_BOTTOM] = {.main = {.size_min = {.width = 0, .height = 10, }, .size_max = {.height = 10, .width_percent = 50, .flags = {.enable_width_percent = 1, }, }, .radius = 5, .layout_pad_all = 2, .color = {.color = 0x1A1A1A, .opacity = 255, }, }, .indicator = {.radius = 5, .color = {.color = 0xFFFFFF, .opacity = 255, }, }, .animation = {.scale_back_path_type = ESP_BROOKESIA_LV_ANIM_PATH_TYPE_BOUNCE, .scale_back_time_ms = 500, }, }, },
                                 .flags = {.enable_indicator_bars = { [ESP_BROOKESIA_GESTURE_INDICATOR_BAR_TYPE_LEFT] = 0,[ESP_BROOKESIA_GESTURE_INDICATOR_BAR_TYPE_RIGHT] = 0,[ESP_BROOKESIA_GESTURE_INDICATOR_BAR_TYPE_BOTTOM] = 1, }, }, }, .gesture_mask_indicator_trigger_time_ms = 0, .recents_screen = {.drag_snapshot_y_step = 10, .drag_snapshot_y_threshold = 50, .drag_snapshot_angle_threshold = 60, .delete_snapshot_y_threshold = 50, }, .flags = {.enable_gesture = 1, .enable_gesture_navigation_back = 0, .enable_recents_screen_snapshot_drag = 1, .enable_recents_screen_hide_when_no_snapshot = 1, }, }, }
        inited = true;
    }
}

ESP_Brookesia_Phone::~ESP_Brookesia_Phone()
{
    ESP_BROOKESIA_LOGD("Destroy phone(@0x%p)", this);
    if (!del()) {
        ESP_BROOKESIA_LOGE("Delete failed");
    }
}

bool ESP_Brookesia_Phone::begin(void)
{
    bool ret = true;
    const ESP_Brookesia_PhoneStylesheet_t *default_find_data = nullptr;
    ESP_Brookesia_StyleSize_t display_size = {};

    ESP_BROOKESIA_LOGD("Begin phone(@0x%p)", this);
    ESP_BROOKESIA_CHECK_FALSE_RETURN(!checkCoreInitialized(), false, "Already initialized");

    // Check if any phone stylesheet is added, if not, add default stylesheet
    if (getStylesheetCount() == 0) {
        ESP_BROOKESIA_LOGW("No phone stylesheet is added, adding default dark stylesheet(%s)",
                           _default_stylesheet_dark.core.name);
        ESP_BROOKESIA_CHECK_FALSE_GOTO(ret = addStylesheet(_default_stylesheet_dark), end,
                                       "Failed to add default stylesheet");
    }
    // Check if any phone stylesheet is activated, if not, activate default stylesheet
    if (_active_stylesheet.core.name == nullptr) {
        ESP_BROOKESIA_CHECK_NULL_RETURN(_display, false, "Invalid display");
        display_size.width = lv_disp_get_hor_res(_display);
        display_size.height = lv_disp_get_ver_res(_display);

        ESP_BROOKESIA_LOGW("No phone stylesheet is activated, try to find first stylesheet with display size(%dx%d)",
                           display_size.width, display_size.height);
        default_find_data = getStylesheet(display_size);
        ESP_BROOKESIA_CHECK_NULL_GOTO(default_find_data, end, "Failed to get default stylesheet");

        ret = activateStylesheet(*default_find_data);
        ESP_BROOKESIA_CHECK_FALSE_GOTO(ret, end, "Failed to activate default stylesheet");
    }

    ESP_BROOKESIA_CHECK_FALSE_GOTO(ret = beginCore(), end, "Failed to begin core");
    ESP_BROOKESIA_CHECK_FALSE_GOTO(ret = _home.begin(), end, "Failed to begin home");
    ESP_BROOKESIA_CHECK_FALSE_GOTO(ret = _manager.begin(), end, "Failed to begin manager");

end:
    return ret;
}

bool ESP_Brookesia_Phone::del(void)
{
    ESP_BROOKESIA_LOGD("Delete(@0x%p)", this);

    if (!checkCoreInitialized()) {
        return true;
    }

    if (!_manager.del()) {
        ESP_BROOKESIA_LOGE("Delete manager failed");
    }
    if (!_home.del()) {
        ESP_BROOKESIA_LOGE("Delete home failed");
    }
    if (!ESP_Brookesia_PhoneStylesheet::del()) {
        ESP_BROOKESIA_LOGE("Delete core template failed");
    }
    if (!delCore()) {
        ESP_BROOKESIA_LOGE("Delete core failed");
    }

    return true;
}

bool ESP_Brookesia_Phone::addStylesheet(const ESP_Brookesia_PhoneStylesheet_t &stylesheet)
{
    ESP_BROOKESIA_LOGD("Add phone(0x%p) stylesheet", this);

    ESP_BROOKESIA_CHECK_FALSE_RETURN(
        ESP_Brookesia_PhoneStylesheet::addStylesheet(stylesheet.core.name, stylesheet.core.screen_size, stylesheet),
        false, "Failed to add phone stylesheet"
    );

    return true;
}

bool ESP_Brookesia_Phone::addStylesheet(const ESP_Brookesia_PhoneStylesheet_t *stylesheet)
{
    ESP_BROOKESIA_LOGD("Add phone(0x%p) stylesheet", this);

    ESP_BROOKESIA_CHECK_FALSE_RETURN(addStylesheet(*stylesheet), false, "Failed to add phone stylesheet");

    return true;
}

bool ESP_Brookesia_Phone::activateStylesheet(const ESP_Brookesia_PhoneStylesheet_t &stylesheet)
{
    ESP_BROOKESIA_LOGD("Activate phone(0x%p) stylesheet", this);

    ESP_BROOKESIA_CHECK_FALSE_RETURN(
        ESP_Brookesia_PhoneStylesheet::activateStylesheet(stylesheet.core.name, stylesheet.core.screen_size),
        false, "Failed to activate phone stylesheet"
    );

    if (checkCoreInitialized() && !sendDataUpdateEvent()) {
        ESP_BROOKESIA_LOGE("Send update data event failed");
    }

    return true;
}

bool ESP_Brookesia_Phone::activateStylesheet(const ESP_Brookesia_PhoneStylesheet_t *stylesheet)
{
    ESP_BROOKESIA_LOGD("Activate phone(0x%p) stylesheet", this);

    ESP_BROOKESIA_CHECK_FALSE_RETURN(activateStylesheet(*stylesheet), false, "Failed to activate phone stylesheet");

    return true;
}

bool ESP_Brookesia_Phone::calibrateStylesheet(const ESP_Brookesia_StyleSize_t &screen_size, ESP_Brookesia_PhoneStylesheet_t &stylesheet)
{
    ESP_BROOKESIA_LOGD("Calibrate phone(0x%p) stylesheet", this);

    // Core
    ESP_BROOKESIA_CHECK_FALSE_RETURN(calibrateCoreData(stylesheet.core), false, "Invalid core data");

    // Home
    if (!_active_stylesheet.manager.flags.enable_gesture && _active_stylesheet.home.flags.enable_recents_screen) {
        ESP_BROOKESIA_LOGW("Gesture is disabled, but recents_screen is enabled, disable recents_screen automatically");
        _active_stylesheet.home.flags.enable_recents_screen = 0;
    }
    ESP_BROOKESIA_CHECK_FALSE_RETURN(_home.calibrateData(screen_size, stylesheet.home), false, "Invalid home data");
    ESP_BROOKESIA_CHECK_FALSE_RETURN(_manager.calibrateData(screen_size, _home, stylesheet.manager), false,
                                     "Invalid manager data");

    return true;
}

bool ESP_Brookesia_Phone::calibrateScreenSize(ESP_Brookesia_StyleSize_t &size)
{
    ESP_BROOKESIA_LOGD("Calibrate phone(0x%p) screen size", this);

    ESP_Brookesia_StyleSize_t display_size = {};
    ESP_BROOKESIA_CHECK_FALSE_RETURN(getDisplaySize(display_size), false, "Get display size failed");
    ESP_BROOKESIA_CHECK_FALSE_RETURN(_core_home.calibrateCoreObjectSize(display_size, size), false, "Invalid screen size");

    return true;
}

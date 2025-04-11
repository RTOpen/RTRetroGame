#ifndef LV_100ASK_BOOT_ANIMATION_H
#define LV_100ASK_BOOT_ANIMATION_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "../lv_100ask_modules.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_100ask_boot_animation(void (*end_cb)(lv_anim_t *), uint32_t boot_time);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_100ASK_BOOT_ANIMATION_H */






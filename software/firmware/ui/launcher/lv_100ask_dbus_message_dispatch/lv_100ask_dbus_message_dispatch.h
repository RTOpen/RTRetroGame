#ifndef LV_100ASK_DBUS_MESSAGE_DISPATCH_H
#define LV_100ASK_DBUS_MESSAGE_DISPATCH_H

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
//void lv_100ask_dbus_message_dispatch_init(void);
void dispatch_dbus_message(int data);
void wait_for_become_front_ground(void);
void dbus_message_dispatch(int data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_100ASK_DLV_100ASK_DBUS_MESSAGE_DISPATCH_HBUS_H */






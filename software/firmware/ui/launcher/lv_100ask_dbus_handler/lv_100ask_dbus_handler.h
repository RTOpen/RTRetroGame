#ifndef LV_100ASK_DBUS_H
#define LV_100ASK_DBUS_H

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
void lv_100ask_dbus_handler_init(const char *path, const char *iface);
void dbus_method_call(const char *destination, const char *path, const char *iface, const char *method, const int state, const int pid);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_100ASK_DBUS_H */






/**
 * @file lv_port_fs.h
 *
 */

#ifndef __LV_PORT_FS_H
#define __LV_PORT_FS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <lvgl.h>



/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register driver(s) for the File system interface
 */
void lv_port_fs_init(void);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*__LV_PORT_FS_H*/


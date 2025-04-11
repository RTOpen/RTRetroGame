/**
 * @file lv_100ask_dbus_message_dispatch.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_100ask_modules.h"

#if LV_USE_100ASK_DEMO_MSG_DISP

#include <stdio.h>
#include <pthread.h>
#include <dbus/dbus.h>
#include "lv_100ask_dbus_message_dispatch.h"

static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;
static int lvgl_states = 1;

static void set_app_fore_groud(int states);
static void wake_up_lvgl_thread(void);



static void set_app_fore_groud(int states)
{
    lvgl_states = states;
}

static void sleep_lvgl_thread(void)
{
    pthread_mutex_lock(&g_tMutex);
}

static void wake_up_lvgl_thread(void)
{
    pthread_mutex_unlock(&g_tMutex);
}


int is_app_fore_ground(void)
{
    return lvgl_states;
}

void wait_for_become_front_ground(void)
{
    pthread_mutex_lock(&g_tMutex);
    pthread_mutex_unlock(&g_tMutex);
}


void dbus_message_dispatch(int data)
{
    if (0 == data)
    {
        sleep_lvgl_thread();
        set_app_fore_groud(0); /* 后台 */
    }
    else if (1 == data)
    {
        set_app_fore_groud(1); /* 前台 */
        wake_up_lvgl_thread();
    }
}


#endif /* LV_USE_100ASK_DEMO_MSG_DISP */





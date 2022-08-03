#ifndef __DRV_KEY_H
#define __DRV_KEY_H

#include "stm32h7xx.h"

typedef enum
{
    KEY_NOPRESS = 0x00,
    KEY_PRESS,
} key_state_t;
typedef enum
{
    KEY_ID_HOME = 0x00,
    KEY_ID_OK,
    KEY_ID_DOWN,
    KEY_ID_UP,
    KEY_ID_LEFT,
    KEY_ID_RIGHT,
    KEY_ID_MAX,
} key_id_t;

int key_hw_init(void);

key_state_t key_scan(key_id_t key_id);

#endif

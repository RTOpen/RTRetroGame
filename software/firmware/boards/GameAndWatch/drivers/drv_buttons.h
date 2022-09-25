#ifndef __DRV_BUTTONS_H_
#define __DRV_BUTTONS_H_

#include <stdint.h>

typedef enum
{
  BUTTON_ID_LEFT = 0x01,
  BUTTON_ID_UP = 0x02,
  BUTTON_ID_RIGHT = 0x04,
  BUTTON_ID_DOWN = 0x08,
  BUTTON_ID_A = 0x10,
  BUTTON_ID_B = 0x20,
  BUTTON_ID_TIME = 0x40,
  BUTTON_ID_GAME = 0x80,
  BUTTON_ID_PAUSE = 0x100,
  BUTTON_ID_POWER = 0x200,
}button_id_t; 


uint32_t buttons_scan(void);

#endif  /* __DRV_BUTTONS_H_ */

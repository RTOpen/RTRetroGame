#include "rtthread.h"
#include "drv_key.h"
#include "hw_config.h"

int key_hw_init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct;
	LL_GPIO_StructInit(&GPIO_InitStruct);
	LL_AHB4_GRP1_EnableClock(KEY_GPIO_RCC);
	/* Configure IO in output push-pull mode to drive external LED */
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	GPIO_InitStruct.Pin = KEY_OK_PIN;
  LL_GPIO_Init(KEY_OK_PORT,&GPIO_InitStruct);
	GPIO_InitStruct.Pin = KEY_DOWN_PIN;
  LL_GPIO_Init(KEY_DOWN_PORT,&GPIO_InitStruct);
	GPIO_InitStruct.Pin = KEY_LEFT_PIN;
  LL_GPIO_Init(KEY_LEFT_PORT,&GPIO_InitStruct);
	GPIO_InitStruct.Pin = KEY_RIGHT_PIN;
  LL_GPIO_Init(KEY_RIGHT_PORT,&GPIO_InitStruct);
  GPIO_InitStruct.Pin = KEY_UP_PIN;
  LL_GPIO_Init(KEY_UP_PORT,&GPIO_InitStruct);
	GPIO_InitStruct.Pin = KEY_HOME_PIN;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(KEY_HOME_PORT,&GPIO_InitStruct);
	
	return 0;
}
INIT_BOARD_EXPORT(key_hw_init);

key_state_t key_scan(key_id_t key_id)
{
    key_state_t state = KEY_NOPRESS;
    switch (key_id)
    {
    case KEY_ID_HOME:
      if(KEY_HOME_PORT->IDR & KEY_HOME_PIN)
      {
        state = KEY_PRESS;
      }
      break;
    case KEY_ID_OK:
      if((KEY_OK_PORT->IDR & KEY_OK_PIN)!=KEY_OK_PIN)
      {
        state = KEY_PRESS;
      }
      break;
    case KEY_ID_UP:
      if((KEY_UP_PORT->IDR & KEY_UP_PIN)!=KEY_UP_PIN)
      {
        state = KEY_PRESS;
      }
      break;  
          case KEY_ID_DOWN:
      if((KEY_DOWN_PORT->IDR & KEY_DOWN_PIN)!=KEY_DOWN_PIN)
      {
        state = KEY_PRESS;
      }
      break;     
          case KEY_ID_LEFT:
      if((KEY_LEFT_PORT->IDR & KEY_LEFT_PIN)!=KEY_LEFT_PIN)
      {
        state = KEY_PRESS;
      }
      break;  
          case KEY_ID_RIGHT:
      if((KEY_RIGHT_PORT->IDR & KEY_RIGHT_PIN)!=KEY_RIGHT_PIN)
      {
        state = KEY_PRESS;
      }
      break;  
    default:
      break;
    }
}

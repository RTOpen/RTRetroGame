
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FT6X06_H
#define FT6X06_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ft6x06_reg.h"
#include <stddef.h>

#define FT6X06_AUTO_CALIBRATION_ENABLED      0U
#define FT6X06_MAX_X_LENGTH                  800U
#define FT6X06_MAX_Y_LENGTH                  480U
  
/** @defgroup FT6X06_Exported_Constants FT6X06 Exported Constants
 * @{
 */
#define FT6X06_OK                      (0)
#define FT6X06_ERROR                   (-1)

/* Max detectable simultaneous touches */
#define FT6X06_MAX_NB_TOUCH             2U
  
/* Touch FT6XX6 IDs */
#define FT6X06_ID                        0x11U
#define FT6X36_ID                        0xCDU

/* Possible values of FT6X06_DEV_MODE_REG */
#define FT6X06_DEV_MODE_WORKING          0x00U
#define FT6X06_DEV_MODE_FACTORY          0x04U
     
/* Possible values of FT6X06_GEST_ID_REG */
#define FT6X06_GEST_ID_NO_GESTURE        0x00U
#define FT6X06_GEST_ID_MOVE_UP           0x10U
#define FT6X06_GEST_ID_MOVE_RIGHT        0x14U
#define FT6X06_GEST_ID_MOVE_DOWN         0x18U
#define FT6X06_GEST_ID_MOVE_LEFT         0x1CU
#define FT6X06_GEST_ID_ZOOM_IN           0x48U
#define FT6X06_GEST_ID_ZOOM_OUT          0x49U
     
/* Values Pn_XH and Pn_YH related */
#define FT6X06_TOUCH_EVT_FLAG_PRESS_DOWN 0x00U
#define FT6X06_TOUCH_EVT_FLAG_LIFT_UP    0x01U
#define FT6X06_TOUCH_EVT_FLAG_CONTACT    0x02U
#define FT6X06_TOUCH_EVT_FLAG_NO_EVENT   0x03U

/* Possible values of FT6X06_GMODE_REG */
#define FT6X06_G_MODE_INTERRUPT_POLLING  0x00U
#define FT6X06_G_MODE_INTERRUPT_TRIGGER  0x01U


/* Exported types ------------------------------------------------------------*/

/** @defgroup FT6X06_Exported_Types FT6X06 Exported Types
 * @{
 */
typedef struct
{ 
  uint32_t  Radian;
  uint32_t  OffsetLeftRight;
  uint32_t  OffsetUpDown;
  uint32_t  DistanceLeftRight;
  uint32_t  DistanceUpDown;
  uint32_t  DistanceZoom;  
}FT6X06_Gesture_Init_t;

typedef struct
{
  uint32_t  TouchDetected;
  uint32_t  TouchX;
  uint32_t  TouchY;
} FT6X06_State_t;

typedef struct
{
  uint32_t  TouchDetected;
  uint32_t  TouchX[FT6X06_MAX_NB_TOUCH];
  uint32_t  TouchY[FT6X06_MAX_NB_TOUCH];
  uint32_t  TouchWeight[FT6X06_MAX_NB_TOUCH];
  uint32_t  TouchEvent[FT6X06_MAX_NB_TOUCH];
  uint32_t  TouchArea[FT6X06_MAX_NB_TOUCH];
} FT6X06_MultiTouch_State_t;


typedef struct 
{       
  uint8_t   MultiTouch;      
  uint8_t   Gesture;
  uint8_t   MaxTouch;
  uint32_t  MaxXl;
  uint32_t  MaxYl;
} FT6X06_Capabilities_t;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @addtogroup FT6X06_Exported_Functions
 * @{
 */

int32_t FT6X06_Init(void);
int32_t FT6X06_GestureConfig(FT6X06_Gesture_Init_t *GestureInit);
int32_t FT6X06_ReadID(uint32_t *Id);
int32_t FT6X06_GetState(FT6X06_State_t *State);
int32_t FT6X06_GetMultiTouchState(FT6X06_MultiTouch_State_t *State);
int32_t FT6X06_GetGesture(uint8_t *GestureId);
int32_t FT6X06_EnableIT(void);
int32_t FT6X06_DisableIT(void);
int32_t FT6X06_ITStatus(void);
int32_t FT6X06_ClearIT(void);
int32_t FT6X06_GetCapabilities(FT6X06_Capabilities_t *Capabilities);


#ifdef __cplusplus
}
#endif
#endif /* FT6X06_H */


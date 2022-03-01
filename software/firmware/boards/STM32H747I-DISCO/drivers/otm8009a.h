
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OTM8009A_H
#define OTM8009A_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "otm8009a_reg.h"


/** @addtogroup OTM8009A_Exported_Variables
  * @{
  */


/** @addtogroup OTM8009A_Exported_Types
  * @{
  */


typedef struct
{
  uint32_t  Orientation;
  uint32_t  ColorCode;
  uint32_t  Brightness;
  uint8_t               IsInitialized;
} OTM8009A_Data_t;


#define OTM8009A_OK                (0)
#define OTM8009A_ERROR             (-1)

/* OTM8009A ID */
#define OTM8009A_ID                 0x40U

/**
 *  Possible values of Display Orientation
 */
#define OTM8009A_ORIENTATION_PORTRAIT    0U /* Portrait orientation choice of LCD screen  */
#define OTM8009A_ORIENTATION_LANDSCAPE   1U /* Landscape orientation choice of LCD screen */

/**
 *  @brief  Possible values of
 *  pixel data format (ie color coding) transmitted on DSI Data lane in DSI packets
 */
#define OTM8009A_FORMAT_RGB888    ((uint32_t)0x00) /* Pixel format chosen is RGB888 : 24 bpp */
#define OTM8009A_FORMAT_RBG565    ((uint32_t)0x02) /* Pixel format chosen is RGB565 : 16 bpp */

/**
  * @brief  otm8009a_480x800 Size
  */

/* Width and Height in Portrait mode */
#define  OTM8009A_480X800_WIDTH             ((uint16_t)480)     /* LCD PIXEL WIDTH   */
#define  OTM8009A_480X800_HEIGHT            ((uint16_t)800)     /* LCD PIXEL HEIGHT  */

/* Width and Height in Landscape mode */
#define  OTM8009A_800X480_WIDTH             ((uint16_t)800)     /* LCD PIXEL WIDTH   */
#define  OTM8009A_800X480_HEIGHT            ((uint16_t)480)     /* LCD PIXEL HEIGHT  */

/**
  * @brief  OTM8009A_480X800 Timing parameters for Portrait orientation mode
  */
#define  OTM8009A_480X800_HSYNC             ((uint16_t)2)      /* Horizontal synchronization */
#define  OTM8009A_480X800_HBP               ((uint16_t)34)     /* Horizontal back porch      */
#define  OTM8009A_480X800_HFP               ((uint16_t)34)     /* Horizontal front porch     */
#define  OTM8009A_480X800_VSYNC             ((uint16_t)1)      /* Vertical synchronization   */
#define  OTM8009A_480X800_VBP               ((uint16_t)15)     /* Vertical back porch        */
#define  OTM8009A_480X800_VFP               ((uint16_t)16)     /* Vertical front porch       */

/**
  * @brief  OTM8009A_800X480 Timing parameters for Landscape orientation mode
  *         Same values as for Portrait mode in fact.
  */
#define  OTM8009A_800X480_HSYNC             OTM8009A_480X800_HSYNC  /* Horizontal synchronization */
#define  OTM8009A_800X480_HBP               OTM8009A_480X800_HBP    /* Horizontal back porch      */
#define  OTM8009A_800X480_HFP               OTM8009A_480X800_HFP    /* Horizontal front porch     */
#define  OTM8009A_800X480_VSYNC             OTM8009A_480X800_VSYNC  /* Vertical synchronization   */
#define  OTM8009A_800X480_VBP               OTM8009A_480X800_VBP    /* Vertical back porch        */
#define  OTM8009A_800X480_VFP               OTM8009A_480X800_VFP    /* Vertical front porch       */

/**
  * @brief  OTM8009A_480X800 frequency divider
  */
#define OTM8009A_480X800_FREQUENCY_DIVIDER  2   /* LCD Frequency divider      */

   
/* Exported macro ------------------------------------------------------------*/
   
/** @defgroup OTM8009A_Exported_Macros OTM8009A Exported Macros
  * @{
  */ 

/* Exported functions --------------------------------------------------------*/
  
/** @addtogroup OTM8009A_Exported_Functions
  * @{
  */
int32_t OTM8009A_Init(uint32_t ColorCoding, uint32_t Orientation);
int32_t OTM8009A_DeInit(void);
int32_t OTM8009A_ReadID(uint32_t *Id);
int32_t OTM8009A_DisplayOn(void);
int32_t OTM8009A_DisplayOff(void);
int32_t OTM8009A_SetBrightness(uint32_t Brightness);
int32_t OTM8009A_GetBrightness(uint32_t *Brightness);
int32_t OTM8009A_SetOrientation(uint32_t Orientation);
int32_t OTM8009A_GetOrientation(uint32_t *Orientation);


int32_t OTM8009A_SetCursor(uint32_t Xpos, uint32_t Ypos);
int32_t OTM8009A_GetXSize(uint32_t *XSize);
int32_t OTM8009A_GetYSize(uint32_t *YSize);


#ifdef __cplusplus
}
#endif

#endif /* OTM8009A_H */


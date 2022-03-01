/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DRV_ETH_H
#define __DRV_ETH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtthread.h"
#include "stm32h7xx.h"

//Number of TX buffers
#ifndef ETH_TX_BUFFER_COUNT
   #define ETH_TX_BUFFER_COUNT 8
#elif (ETH_TX_BUFFER_COUNT < 1)
   #error ETH_TX_BUFFER_COUNT parameter is not valid
#endif

//TX buffer size
#ifndef ETH_TX_BUFFER_SIZE
   #define ETH_TX_BUFFER_SIZE 1536
#elif (ETH_TX_BUFFER_SIZE != 1536)
   #error ETH_TX_BUFFER_SIZE parameter is not valid
#endif

//Number of RX buffers
#ifndef ETH_RX_BUFFER_COUNT
   #define ETH_RX_BUFFER_COUNT 8
#elif (ETH_RX_BUFFER_COUNT < 1)
   #error ETH_RX_BUFFER_COUNT parameter is not valid
#endif

//RX buffer size
#ifndef ETH_RX_BUFFER_SIZE
   #define ETH_RX_BUFFER_SIZE 1536
#elif (ETH_RX_BUFFER_SIZE != 1536)
   #error ETH_RX_BUFFER_SIZE parameter is not valid
#endif

//Interrupt priority grouping
#ifndef ETH_IRQ_PRIORITY_GROUPING
   #define ETH_IRQ_PRIORITY_GROUPING 3
#elif (ETH_IRQ_PRIORITY_GROUPING < 0)
   #error ETH_IRQ_PRIORITY_GROUPING parameter is not valid
#endif

//Ethernet interrupt group priority
#ifndef ETH_IRQ_GROUP_PRIORITY
   #define ETH_IRQ_GROUP_PRIORITY 12
#elif (ETH_IRQ_GROUP_PRIORITY < 0)
   #error ETH_IRQ_GROUP_PRIORITY parameter is not valid
#endif

//Ethernet interrupt subpriority
#ifndef ETH_IRQ_SUB_PRIORITY
   #define ETH_IRQ_SUB_PRIORITY 0
#elif (ETH_IRQ_SUB_PRIORITY < 0)
   #error SETH_IRQ_SUB_PRIORITY parameter is not valid
#endif

//MMCRIMR register
#ifndef ETH_MMCRIMR_RXLPITRCIM
   #define ETH_MMCRIMR_RXLPITRCIM  0x08000000
   #define ETH_MMCRIMR_RXLPIUSCIM  0x04000000
   #define ETH_MMCRIMR_RXUCGPIM    0x00020000
   #define ETH_MMCRIMR_RXALGNERPIM 0x00000040
   #define ETH_MMCRIMR_RXCRCERPIM  0x00000020
#endif

//MMCTIMR register
#ifndef ETH_MMCTIMR_TXLPITRCIM
   #define ETH_MMCTIMR_TXLPITRCIM  0x08000000
   #define ETH_MMCTIMR_TXLPIUSCIM  0x04000000
   #define ETH_MMCTIMR_TXGPKTIM    0x00200000
   #define ETH_MMCTIMR_TXMCOLGPIM  0x00008000
   #define ETH_MMCTIMR_TXSCOLGPIM  0x00004000
#endif

//Transmit normal descriptor (read format)
#define ETH_TDES0_BUF1AP        0xFFFFFFFF
#define ETH_TDES1_BUF2AP        0xFFFFFFFF
#define ETH_TDES2_IOC           0x80000000
#define ETH_TDES2_TTSE          0x40000000
#define ETH_TDES2_B2L           0x3FFF0000
#define ETH_TDES2_VTIR          0x0000C000
#define ETH_TDES2_B1L           0x00003FFF
#define ETH_TDES3_OWN           0x80000000
#define ETH_TDES3_CTXT          0x40000000
#define ETH_TDES3_FD            0x20000000
#define ETH_TDES3_LD            0x10000000
#define ETH_TDES3_CPC           0x0C000000
#define ETH_TDES3_SAIC          0x03800000
#define ETH_TDES3_THL           0x00780000
#define ETH_TDES3_TSE           0x00040000
#define ETH_TDES3_CIC           0x00030000
#define ETH_TDES3_FL            0x00007FFF

//Transmit normal descriptor (write-back format)
#define ETH_TDES0_TTSL          0xFFFFFFFF
#define ETH_TDES1_TTSH          0xFFFFFFFF
#define ETH_TDES3_OWN           0x80000000
#define ETH_TDES3_CTXT          0x40000000
#define ETH_TDES3_FD            0x20000000
#define ETH_TDES3_LD            0x10000000
#define ETH_TDES3_TTSS          0x00020000
#define ETH_TDES3_ES            0x00008000
#define ETH_TDES3_JT            0x00004000
#define ETH_TDES3_FF            0x00002000
#define ETH_TDES3_PCE           0x00001000
#define ETH_TDES3_LOC           0x00000800
#define ETH_TDES3_NC            0x00000400
#define ETH_TDES3_LC            0x00000200
#define ETH_TDES3_EC            0x00000100
#define ETH_TDES3_CC            0x000000F0
#define ETH_TDES3_ED            0x00000008
#define ETH_TDES3_UF            0x00000004
#define ETH_TDES3_DB            0x00000002
#define ETH_TDES3_IHE           0x00000001

//Transmit context descriptor
#define ETH_TDES0_TTSL          0xFFFFFFFF
#define ETH_TDES1_TTSH          0xFFFFFFFF
#define ETH_TDES2_IVT           0xFFFF0000
#define ETH_TDES2_MSS           0x00003FFF
#define ETH_TDES3_OWN           0x80000000
#define ETH_TDES3_CTXT          0x40000000
#define ETH_TDES3_OSTC          0x08000000
#define ETH_TDES3_TCMSSV        0x04000000
#define ETH_TDES3_CDE           0x00800000
#define ETH_TDES3_IVLTV         0x00020000
#define ETH_TDES3_VLTV          0x00010000
#define ETH_TDES3_VT            0x0000FFFF

//Receive normal descriptor (read format)
#define ETH_RDES0_BUF1AP        0xFFFFFFFF
#define ETH_RDES2_BUF2AP        0xFFFFFFFF
#define ETH_RDES3_OWN           0x80000000
#define ETH_RDES3_IOC           0x40000000
#define ETH_RDES3_BUF2V         0x02000000
#define ETH_RDES3_BUF1V         0x01000000

//Receive normal descriptor (write-back format)
#define ETH_RDES0_IVT           0xFFFF0000
#define ETH_RDES0_OVT           0x0000FFFF
#define ETH_RDES1_OPC           0xFFFF0000
#define ETH_RDES1_TD            0x00008000
#define ETH_RDES1_TSA           0x00004000
#define ETH_RDES1_PV            0x00002000
#define ETH_RDES1_PFT           0x00001000
#define ETH_RDES1_PMT           0x00000F00
#define ETH_RDES1_IPCE          0x00000080
#define ETH_RDES1_IPCB          0x00000040
#define ETH_RDES1_IPV6          0x00000020
#define ETH_RDES1_IPV4          0x00000010
#define ETH_RDES1_IPHE          0x00000008
#define ETH_RDES1_PT            0x00000007
#define ETH_RDES2_L3L4FM        0xE0000000
#define ETH_RDES2_L4FM          0x10000000
#define ETH_RDES2_L3FM          0x08000000
#define ETH_RDES2_MADRM         0x07F80000
#define ETH_RDES2_HF            0x00040000
#define ETH_RDES2_DAF           0x00020000
#define ETH_RDES2_SAF           0x00010000
#define ETH_RDES2_VF            0x00008000
#define ETH_RDES2_ARPRN         0x00000400
#define ETH_RDES3_OWN           0x80000000
#define ETH_RDES3_CTXT          0x40000000
#define ETH_RDES3_FD            0x20000000
#define ETH_RDES3_LD            0x10000000
#define ETH_RDES3_RS2V          0x08000000
#define ETH_RDES3_RS1V          0x04000000
#define ETH_RDES3_RS0V          0x02000000
#define ETH_RDES3_CE            0x01000000
#define ETH_RDES3_GP            0x00800000
#define ETH_RDES3_RWT           0x00400000
#define ETH_RDES3_OE            0x00200000
#define ETH_RDES3_RE            0x00100000
#define ETH_RDES3_DE            0x00080000
#define ETH_RDES3_LT            0x00070000
#define ETH_RDES3_ES            0x00008000
#define ETH_RDES3_PL            0x00007FFF

//Receive context descriptor
#define ETH_RDES0_RTSL          0xFFFFFFFF
#define ETH_RDES1_RTSH          0xFFFFFFFF
#define ETH_RDES3_OWN           0x80000000
#define ETH_RDES3_CTXT          0x40000000


/**
 * @brief Transmit descriptor
 **/

typedef struct
{
   uint32_t tdes0;
   uint32_t tdes1;
   uint32_t tdes2;
   uint32_t tdes3;
} Stm32h7xxTxDmaDesc;


/**
 * @brief Receive descriptor
 **/

typedef struct
{
   uint32_t rdes0;
   uint32_t rdes1;
   uint32_t rdes2;
   uint32_t rdes3;
} Stm32h7xxRxDmaDesc;

#define ETH_DMARXDESC_FRAME_LENGTHSHIFT  16

#define MAX_ADDR_LEN 6


#define ETH_Speed_10M        ((uint32_t)0x00000000)
#define ETH_Speed_100M       ((uint32_t)0x00004000)

#define ETH_Mode_FullDuplex       ((uint32_t)0x00000800)
#define ETH_Mode_HalfDuplex       ((uint32_t)0x00000000)


#define PHY_READ_TO                     ((uint32_t)0x0008FFFF)
#define PHY_WRITE_TO                    ((uint32_t)0x0008FFFF)


/** @defgroup PHY_Reset_Delay
  * @{
  */
#define PHY_ResetDelay                  ((uint32_t)0x000FFFFF)


/** @defgroup PHY_Config_Delay
  * @{
  */
#define PHY_ConfigDelay                 ((uint32_t)0x00FFFFFF)




/** @defgroup PHY_basic_Control_register
  * @{
  */
#define PHY_Reset                       ((uint16_t)0x8000)      /*!< PHY Reset */
#define PHY_Loopback                    ((uint16_t)0x4000)      /*!< Select loop-back mode */
#define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100)      /*!< Set the full-duplex mode at 100 Mb/s */
#define PHY_HALFDUPLEX_100M             ((uint16_t)0x2000)      /*!< Set the half-duplex mode at 100 Mb/s */
#define PHY_FULLDUPLEX_10M              ((uint16_t)0x0100)      /*!< Set the full-duplex mode at 10 Mb/s */
#define PHY_HALFDUPLEX_10M              ((uint16_t)0x0000)      /*!< Set the half-duplex mode at 10 Mb/s */
#define PHY_AutoNegotiation             ((uint16_t)0x1000)      /*!< Enable auto-negotiation function */
#define PHY_Restart_AutoNegotiation     ((uint16_t)0x0200)      /*!< Restart auto-negotiation function */
#define PHY_Powerdown                   ((uint16_t)0x0800)      /*!< Select the power down mode */
#define PHY_Isolate                     ((uint16_t)0x0400)      /*!< Isolate PHY from MII */


/** @defgroup PHY_basic_status_register
  * @{
  */
#define PHY_AutoNego_Complete           ((uint16_t)0x0020)      /*!< Auto-Negotioation process completed */
#define PHY_Linked_Status               ((uint16_t)0x0004)      /*!< Valid link established */
#define PHY_Jabber_detection            ((uint16_t)0x0002)      /*!< Jabber condition detected */


/** @defgroup LAN8742_Registers_Mapping LAN8742 Registers Mapping
  * @{
  */ 
#define LAN8742_BCR      ((uint16_t)0x0000U)
#define LAN8742_BSR      ((uint16_t)0x0001U)
#define LAN8742_PHYI1R   ((uint16_t)0x0002U)
#define LAN8742_PHYI2R   ((uint16_t)0x0003U)
#define LAN8742_ANAR     ((uint16_t)0x0004U)
#define LAN8742_ANLPAR   ((uint16_t)0x0005U)
#define LAN8742_ANER     ((uint16_t)0x0006U)
#define LAN8742_ANNPTR   ((uint16_t)0x0007U)
#define LAN8742_ANNPRR   ((uint16_t)0x0008U)
#define LAN8742_MMDACR   ((uint16_t)0x000DU)
#define LAN8742_MMDAADR  ((uint16_t)0x000EU)
#define LAN8742_ENCTR    ((uint16_t)0x0010U)
#define LAN8742_MCSR     ((uint16_t)0x0011U)
#define LAN8742_SMR      ((uint16_t)0x0012U)
#define LAN8742_TPDCR    ((uint16_t)0x0018U)
#define LAN8742_TCSR     ((uint16_t)0x0019U)
#define LAN8742_SECR     ((uint16_t)0x001AU)
#define LAN8742_SCSIR    ((uint16_t)0x001BU)
#define LAN8742_CLR      ((uint16_t)0x001CU)
#define LAN8742_ISFR     ((uint16_t)0x001DU)
#define LAN8742_IMR      ((uint16_t)0x001EU)
#define LAN8742_PHYSCSR  ((uint16_t)0x001FU)


/** @defgroup LAN8742_BCR_Bit_Definition LAN8742 BCR Bit Definition
  * @{
  */    
#define LAN8742_BCR_SOFT_RESET         ((uint16_t)0x8000U)
#define LAN8742_BCR_LOOPBACK           ((uint16_t)0x4000U)
#define LAN8742_BCR_SPEED_SELECT       ((uint16_t)0x2000U)
#define LAN8742_BCR_AUTONEGO_EN        ((uint16_t)0x1000U)
#define LAN8742_BCR_POWER_DOWN         ((uint16_t)0x0800U)
#define LAN8742_BCR_ISOLATE            ((uint16_t)0x0400U)
#define LAN8742_BCR_RESTART_AUTONEGO   ((uint16_t)0x0200U)
#define LAN8742_BCR_DUPLEX_MODE        ((uint16_t)0x0100U) 


/** @defgroup LAN8742_BSR_Bit_Definition LAN8742 BSR Bit Definition
  * @{
  */   
#define LAN8742_BSR_100BASE_T4       ((uint16_t)0x8000U)
#define LAN8742_BSR_100BASE_TX_FD    ((uint16_t)0x4000U)
#define LAN8742_BSR_100BASE_TX_HD    ((uint16_t)0x2000U)
#define LAN8742_BSR_10BASE_T_FD      ((uint16_t)0x1000U)
#define LAN8742_BSR_10BASE_T_HD      ((uint16_t)0x0800U)
#define LAN8742_BSR_100BASE_T2_FD    ((uint16_t)0x0400U)
#define LAN8742_BSR_100BASE_T2_HD    ((uint16_t)0x0200U)
#define LAN8742_BSR_EXTENDED_STATUS  ((uint16_t)0x0100U)
#define LAN8742_BSR_AUTONEGO_CPLT    ((uint16_t)0x0020U)
#define LAN8742_BSR_REMOTE_FAULT     ((uint16_t)0x0010U)
#define LAN8742_BSR_AUTONEGO_ABILITY ((uint16_t)0x0008U)
#define LAN8742_BSR_LINK_STATUS      ((uint16_t)0x0004U)
#define LAN8742_BSR_JABBER_DETECT    ((uint16_t)0x0002U)
#define LAN8742_BSR_EXTENDED_CAP     ((uint16_t)0x0001U)


/** @defgroup LAN8742_PHYI1R_Bit_Definition LAN8742 PHYI1R Bit Definition
  * @{
  */
#define LAN8742_PHYI1R_OUI_3_18           ((uint16_t)0xFFFFU)


/** @defgroup LAN8742_PHYI2R_Bit_Definition LAN8742 PHYI2R Bit Definition
  * @{
  */
#define LAN8742_PHYI2R_OUI_19_24          ((uint16_t)0xFC00U)
#define LAN8742_PHYI2R_MODEL_NBR          ((uint16_t)0x03F0U)
#define LAN8742_PHYI2R_REVISION_NBR       ((uint16_t)0x000FU)


/** @defgroup LAN8742_ANAR_Bit_Definition LAN8742 ANAR Bit Definition
  * @{
  */
#define LAN8742_ANAR_NEXT_PAGE               ((uint16_t)0x8000U)
#define LAN8742_ANAR_REMOTE_FAULT            ((uint16_t)0x2000U)
#define LAN8742_ANAR_PAUSE_OPERATION         ((uint16_t)0x0C00U)
#define LAN8742_ANAR_PO_NOPAUSE              ((uint16_t)0x0000U)
#define LAN8742_ANAR_PO_SYMMETRIC_PAUSE      ((uint16_t)0x0400U)
#define LAN8742_ANAR_PO_ASYMMETRIC_PAUSE     ((uint16_t)0x0800U)
#define LAN8742_ANAR_PO_ADVERTISE_SUPPORT    ((uint16_t)0x0C00U)
#define LAN8742_ANAR_100BASE_TX_FD           ((uint16_t)0x0100U)
#define LAN8742_ANAR_100BASE_TX              ((uint16_t)0x0080U)
#define LAN8742_ANAR_10BASE_T_FD             ((uint16_t)0x0040U)
#define LAN8742_ANAR_10BASE_T                ((uint16_t)0x0020U)
#define LAN8742_ANAR_SELECTOR_FIELD          ((uint16_t)0x000FU)


/** @defgroup LAN8742_ANLPAR_Bit_Definition LAN8742 ANLPAR Bit Definition
  * @{
  */
#define LAN8742_ANLPAR_NEXT_PAGE            ((uint16_t)0x8000U)
#define LAN8742_ANLPAR_REMOTE_FAULT         ((uint16_t)0x2000U)
#define LAN8742_ANLPAR_PAUSE_OPERATION      ((uint16_t)0x0C00U)
#define LAN8742_ANLPAR_PO_NOPAUSE           ((uint16_t)0x0000U)
#define LAN8742_ANLPAR_PO_SYMMETRIC_PAUSE   ((uint16_t)0x0400U)
#define LAN8742_ANLPAR_PO_ASYMMETRIC_PAUSE  ((uint16_t)0x0800U)
#define LAN8742_ANLPAR_PO_ADVERTISE_SUPPORT ((uint16_t)0x0C00U)
#define LAN8742_ANLPAR_100BASE_TX_FD        ((uint16_t)0x0100U)
#define LAN8742_ANLPAR_100BASE_TX           ((uint16_t)0x0080U)
#define LAN8742_ANLPAR_10BASE_T_FD          ((uint16_t)0x0040U)
#define LAN8742_ANLPAR_10BASE_T             ((uint16_t)0x0020U)
#define LAN8742_ANLPAR_SELECTOR_FIELD       ((uint16_t)0x000FU)


/** @defgroup LAN8742_ANER_Bit_Definition LAN8742 ANER Bit Definition
  * @{
  */
#define LAN8742_ANER_RX_NP_LOCATION_ABLE    ((uint16_t)0x0040U)
#define LAN8742_ANER_RX_NP_STORAGE_LOCATION ((uint16_t)0x0020U)
#define LAN8742_ANER_PARALLEL_DETECT_FAULT  ((uint16_t)0x0010U)
#define LAN8742_ANER_LP_NP_ABLE             ((uint16_t)0x0008U)
#define LAN8742_ANER_NP_ABLE                ((uint16_t)0x0004U)
#define LAN8742_ANER_PAGE_RECEIVED          ((uint16_t)0x0002U)
#define LAN8742_ANER_LP_AUTONEG_ABLE        ((uint16_t)0x0001U)


/** @defgroup LAN8742_ANNPTR_Bit_Definition LAN8742 ANNPTR Bit Definition
  * @{
  */
#define LAN8742_ANNPTR_NEXT_PAGE         ((uint16_t)0x8000U)
#define LAN8742_ANNPTR_MESSAGE_PAGE      ((uint16_t)0x2000U)
#define LAN8742_ANNPTR_ACK2              ((uint16_t)0x1000U)
#define LAN8742_ANNPTR_TOGGLE            ((uint16_t)0x0800U)
#define LAN8742_ANNPTR_MESSAGGE_CODE     ((uint16_t)0x07FFU)


/** @defgroup LAN8742_ANNPRR_Bit_Definition LAN8742 ANNPRR Bit Definition
  * @{
  */
#define LAN8742_ANNPTR_NEXT_PAGE         ((uint16_t)0x8000U)
#define LAN8742_ANNPRR_ACK               ((uint16_t)0x4000U)
#define LAN8742_ANNPRR_MESSAGE_PAGE      ((uint16_t)0x2000U)
#define LAN8742_ANNPRR_ACK2              ((uint16_t)0x1000U)
#define LAN8742_ANNPRR_TOGGLE            ((uint16_t)0x0800U)
#define LAN8742_ANNPRR_MESSAGGE_CODE     ((uint16_t)0x07FFU)


/** @defgroup LAN8742_MMDACR_Bit_Definition LAN8742 MMDACR Bit Definition
  * @{
  */
#define LAN8742_MMDACR_MMD_FUNCTION       ((uint16_t)0xC000U) 
#define LAN8742_MMDACR_MMD_FUNCTION_ADDR  ((uint16_t)0x0000U)
#define LAN8742_MMDACR_MMD_FUNCTION_DATA  ((uint16_t)0x4000U)
#define LAN8742_MMDACR_MMD_DEV_ADDR       ((uint16_t)0x001FU)


/** @defgroup LAN8742_ENCTR_Bit_Definition LAN8742 ENCTR Bit Definition
  * @{
  */
#define LAN8742_ENCTR_TX_ENABLE             ((uint16_t)0x8000U)
#define LAN8742_ENCTR_TX_TIMER              ((uint16_t)0x6000U)
#define LAN8742_ENCTR_TX_TIMER_1S           ((uint16_t)0x0000U)
#define LAN8742_ENCTR_TX_TIMER_768MS        ((uint16_t)0x2000U)
#define LAN8742_ENCTR_TX_TIMER_512MS        ((uint16_t)0x4000U)
#define LAN8742_ENCTR_TX_TIMER_265MS        ((uint16_t)0x6000U)
#define LAN8742_ENCTR_RX_ENABLE             ((uint16_t)0x1000U)
#define LAN8742_ENCTR_RX_MAX_INTERVAL       ((uint16_t)0x0C00U)
#define LAN8742_ENCTR_RX_MAX_INTERVAL_64MS  ((uint16_t)0x0000U)
#define LAN8742_ENCTR_RX_MAX_INTERVAL_256MS ((uint16_t)0x0400U)
#define LAN8742_ENCTR_RX_MAX_INTERVAL_512MS ((uint16_t)0x0800U)
#define LAN8742_ENCTR_RX_MAX_INTERVAL_1S    ((uint16_t)0x0C00U)
#define LAN8742_ENCTR_EX_CROSS_OVER         ((uint16_t)0x0002U)
#define LAN8742_ENCTR_EX_MANUAL_CROSS_OVER  ((uint16_t)0x0001U)


/** @defgroup LAN8742_MCSR_Bit_Definition LAN8742 MCSR Bit Definition
  * @{
  */
#define LAN8742_MCSR_EDPWRDOWN        ((uint16_t)0x2000U)
#define LAN8742_MCSR_FARLOOPBACK      ((uint16_t)0x0200U)
#define LAN8742_MCSR_ALTINT           ((uint16_t)0x0040U)
#define LAN8742_MCSR_ENERGYON         ((uint16_t)0x0002U)


/** @defgroup LAN8742_SMR_Bit_Definition LAN8742 SMR Bit Definition
  * @{
  */
#define LAN8742_SMR_MODE       ((uint16_t)0x00E0U)
#define LAN8742_SMR_PHY_ADDR   ((uint16_t)0x001FU)


/** @defgroup LAN8742_TPDCR_Bit_Definition LAN8742 TPDCR Bit Definition
  * @{
  */
#define LAN8742_TPDCR_DELAY_IN                 ((uint16_t)0x8000U)
#define LAN8742_TPDCR_LINE_BREAK_COUNTER       ((uint16_t)0x7000U)
#define LAN8742_TPDCR_PATTERN_HIGH             ((uint16_t)0x0FC0U)
#define LAN8742_TPDCR_PATTERN_LOW              ((uint16_t)0x003FU)
/**
  * @}
  */

/** @defgroup LAN8742_TCSR_Bit_Definition LAN8742 TCSR Bit Definition
  * @{
  */
#define LAN8742_TCSR_TDR_ENABLE           ((uint16_t)0x8000U)
#define LAN8742_TCSR_TDR_AD_FILTER_ENABLE ((uint16_t)0x4000U)
#define LAN8742_TCSR_TDR_CH_CABLE_TYPE    ((uint16_t)0x0600U)
#define LAN8742_TCSR_TDR_CH_CABLE_DEFAULT ((uint16_t)0x0000U)
#define LAN8742_TCSR_TDR_CH_CABLE_SHORTED ((uint16_t)0x0200U)
#define LAN8742_TCSR_TDR_CH_CABLE_OPEN    ((uint16_t)0x0400U)
#define LAN8742_TCSR_TDR_CH_CABLE_MATCH   ((uint16_t)0x0600U)
#define LAN8742_TCSR_TDR_CH_STATUS        ((uint16_t)0x0100U)
#define LAN8742_TCSR_TDR_CH_LENGTH        ((uint16_t)0x00FFU)


/** @defgroup LAN8742_SCSIR_Bit_Definition LAN8742 SCSIR Bit Definition
  * @{
  */
#define LAN8742_SCSIR_AUTO_MDIX_ENABLE    ((uint16_t)0x8000U)
#define LAN8742_SCSIR_CHANNEL_SELECT      ((uint16_t)0x2000U)
#define LAN8742_SCSIR_SQE_DISABLE         ((uint16_t)0x0800U)
#define LAN8742_SCSIR_XPOLALITY           ((uint16_t)0x0010U)


/** @defgroup LAN8742_CLR_Bit_Definition LAN8742 CLR Bit Definition
  * @{
  */
#define LAN8742_CLR_CABLE_LENGTH       ((uint16_t)0xF000U)


/** @defgroup LAN8742_IMR_ISFR_Bit_Definition LAN8742 IMR ISFR Bit Definition
  * @{
  */
#define LAN8742_INT_8       ((uint16_t)0x0100U)
#define LAN8742_INT_7       ((uint16_t)0x0080U)
#define LAN8742_INT_6       ((uint16_t)0x0040U)
#define LAN8742_INT_5       ((uint16_t)0x0020U)
#define LAN8742_INT_4       ((uint16_t)0x0010U)
#define LAN8742_INT_3       ((uint16_t)0x0008U)
#define LAN8742_INT_2       ((uint16_t)0x0004U)
#define LAN8742_INT_1       ((uint16_t)0x0002U)


/** @defgroup LAN8742_PHYSCSR_Bit_Definition LAN8742 PHYSCSR Bit Definition
  * @{
  */
#define LAN8742_PHYSCSR_AUTONEGO_DONE   ((uint16_t)0x1000U)
#define LAN8742_PHYSCSR_HCDSPEEDMASK    ((uint16_t)0x001CU)
#define LAN8742_PHYSCSR_10BT_HD         ((uint16_t)0x0004U)
#define LAN8742_PHYSCSR_10BT_FD         ((uint16_t)0x0014U)
#define LAN8742_PHYSCSR_100BTX_HD       ((uint16_t)0x0008U)
#define LAN8742_PHYSCSR_100BTX_FD       ((uint16_t)0x0018U) 

    
/** @defgroup LAN8742_Status LAN8742 Status
  * @{
  */    

#define  LAN8742_STATUS_READ_ERROR            ((int32_t)-5)
#define  LAN8742_STATUS_WRITE_ERROR           ((int32_t)-4)
#define  LAN8742_STATUS_ADDRESS_ERROR         ((int32_t)-3)
#define  LAN8742_STATUS_RESET_TIMEOUT         ((int32_t)-2)
#define  LAN8742_STATUS_ERROR                 ((int32_t)-1)
#define  LAN8742_STATUS_OK                    ((int32_t) 0)
#define  LAN8742_STATUS_LINK_DOWN             ((int32_t) 1)
#define  LAN8742_STATUS_100MBITS_FULLDUPLEX   ((int32_t) 2)
#define  LAN8742_STATUS_100MBITS_HALFDUPLEX   ((int32_t) 3)
#define  LAN8742_STATUS_10MBITS_FULLDUPLEX    ((int32_t) 4)
#define  LAN8742_STATUS_10MBITS_HALFDUPLEX    ((int32_t) 5)
#define  LAN8742_STATUS_AUTONEGO_NOTDONE      ((int32_t) 6)


/** @defgroup LAN8742_IT_Flags LAN8742 IT Flags
  * @{
  */     
#define  LAN8742_WOL_IT                        LAN8742_INT_8
#define  LAN8742_ENERGYON_IT                   LAN8742_INT_7
#define  LAN8742_AUTONEGO_COMPLETE_IT          LAN8742_INT_6
#define  LAN8742_REMOTE_FAULT_IT               LAN8742_INT_5
#define  LAN8742_LINK_DOWN_IT                  LAN8742_INT_4
#define  LAN8742_AUTONEGO_LP_ACK_IT            LAN8742_INT_3
#define  LAN8742_PARALLEL_DETECTION_FAULT_IT   LAN8742_INT_2
#define  LAN8742_AUTONEGO_PAGE_RECEIVED_IT     LAN8742_INT_1

/* STM32 ETH HW initialization */
int eth_hw_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __DRV_ETH_H */





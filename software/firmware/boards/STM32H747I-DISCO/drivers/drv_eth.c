/*
 * STM32 Eth Driver for RT-Thread
 * Change Logs:
 * Date           Author       Notes
 * 2019-9-25     xiaonong      eth interface driver for STM32H7
 */
/* RT-Thread Device Interface */
/* Includes ------------------------------------------------------------------*/
#include "rtthread.h"
#include "drv_eth.h"
#include "hw_config.h"

#ifdef RT_USING_LWIP

#include "lwip/ip.h"
#include "netif/ethernetif.h"
#include "netif/etharp.h"
#include "lwip/icmp.h"
#include "lwipopts.h"

/* debug option */
//#define ETH_TX_DUMP
//#define ETH_RX_DUMP#ifdef RT_USING_LWIP
#define DBG_SECTION_NAME  "drv.eth"
#define DBG_COLOR
#define DBG_LEVEL DBG_LOG
#include <rtdbg.h>

#define ETH_RXBUFNB         7
#define ETH_TXBUFNB         3

//IAR EWARM compiler?
#if defined(__ICCARM__)

//Transmit buffer
#pragma data_alignment = 4
#pragma location = ".ram_no_cache"
static uint8_t txBuffer[ETH_TX_BUFFER_COUNT][ETH_TX_BUFFER_SIZE];
//Receive buffer
#pragma data_alignment = 4
#pragma location = ".ram_no_cache"
static uint8_t rxBuffer[ETH_RX_BUFFER_COUNT][ETH_RX_BUFFER_SIZE];
//Transmit DMA descriptors
#pragma data_alignment = 4
#pragma location = ".ram_no_cache"
static Stm32h7xxTxDmaDesc txDmaDesc[ETH_TX_BUFFER_COUNT];
//Receive DMA descriptors
#pragma data_alignment = 4
#pragma location = ".ram_no_cache"
static Stm32h7xxRxDmaDesc rxDmaDesc[ETH_RX_BUFFER_COUNT];

//Keil MDK-ARM or GCC compiler?
#else

//Transmit buffer
static uint8_t txBuffer[ETH_TX_BUFFER_COUNT][ETH_TX_BUFFER_SIZE]
                __attribute__((aligned(4), __section__(".ram_no_cache")));
//Receive buffer
static uint8_t rxBuffer[ETH_RX_BUFFER_COUNT][ETH_RX_BUFFER_SIZE]
                __attribute__((aligned(4), __section__(".ram_no_cache")));
//Transmit DMA descriptors
static Stm32h7xxTxDmaDesc txDmaDesc[ETH_TX_BUFFER_COUNT]
                __attribute__((aligned(4), __section__(".ram_no_cache")));
//Receive DMA descriptors
static Stm32h7xxRxDmaDesc rxDmaDesc[ETH_RX_BUFFER_COUNT]
                __attribute__((aligned(4), __section__(".ram_no_cache")));

#endif

typedef struct 
{
    /* inherit from ethernet device */
    struct eth_device parent;

    /* interface address info. */
    rt_uint8_t  dev_addr[MAX_ADDR_LEN];         /* hw address   */

    uint32_t    ETH_Speed; /*!< @ref ETH_Speed */
    uint32_t    ETH_Mode;  /*!< @ref ETH_Duplex_Mode */

    uint32_t ETH_HashTableHigh;
    uint32_t ETH_HashTableLow;
}stm32_eth_t;

uint8_t txIndex = 0, rxIndex = 0;

stm32_eth_t stm32_eth_device;
static struct rt_semaphore tx_wait;
static rt_bool_t tx_is_waiting = RT_FALSE;

/**
 * @brief Write PHY register
 * @param[in] opcode Access type (2 bits)
 * @param[in] phyAddr PHY address (5 bits)
 * @param[in] regAddr Register address (5 bits)
 * @param[in] data Register value
 **/

rt_err_t ETH_WritePHYRegister(uint8_t phy_addr, uint8_t reg, uint16_t data)
{
      uint32_t temp;
      volatile uint32_t timeout = 0;
      //Take care not to alter MDC clock configuration
      temp = ETH->MACMDIOAR & ETH_MACMDIOAR_CR;
      //Set up a write operation
      temp |= ETH_MACMDIOAR_MOC_WR | ETH_MACMDIOAR_MB;
      //PHY address
      temp |= (phy_addr << 21) & ETH_MACMDIOAR_PA;
      //Register address
      temp |= (reg << 16) & ETH_MACMDIOAR_RDA;

      //Data to be written in the PHY register
      ETH->MACMDIODR = data & ETH_MACMDIODR_MD;

      //Start a write operation
      ETH->MACMDIOAR = temp;
      //Wait for the write to complete
      while(ETH->MACMDIOAR & ETH_MACMDIOAR_MB)
      {
					timeout++;
					if((timeout >= (uint32_t)PHY_WRITE_TO))
					{
					 return RT_ERROR;
					}
      }
			
    return RT_EOK;
}


/**
 * @brief Read PHY register
 * @param[in] opcode Access type (2 bits)
 * @param[in] phyAddr PHY address (5 bits)
 * @param[in] regAddr Register address (5 bits)
 * @return Register value
 **/

uint16_t ETH_ReadPHYRegister(uint8_t phy_addr, uint8_t reg)
{
	 volatile uint32_t timeout = 0;
   uint32_t temp;

   //Valid opcode?
   //Take care not to alter MDC clock configuration
   temp = ETH->MACMDIOAR & ETH_MACMDIOAR_CR;
   //Set up a read operation
   temp |= ETH_MACMDIOAR_MOC_RD | ETH_MACMDIOAR_MB;
   //PHY address
   temp |= (phy_addr << 21) & ETH_MACMDIOAR_PA;
   //Register address
   temp |= (reg << 16) & ETH_MACMDIOAR_RDA;

   //Start a read operation
   ETH->MACMDIOAR = temp;
   //Wait for the read to complete
   while(ETH->MACMDIOAR & ETH_MACMDIOAR_MB)
   {
		 timeout++;
		 if(timeout >= PHY_READ_TO)
     {
		  return 0;
		 }
   }

   //Return the value of the PHY register
   return ETH->MACMDIODR & ETH_MACMDIODR_MD;
}


/**
 * @brief Initialize DMA descriptor lists
 * @param[in] interface Underlying network interface
 **/

void Eth_InitDmaDesc(void)
{
   uint8_t i;

   //Initialize TX DMA descriptor list
   for(i = 0; i < ETH_TX_BUFFER_COUNT; i++)
   {
      //The descriptor is initially owned by the application
      txDmaDesc[i].tdes0 = 0;
      txDmaDesc[i].tdes1 = 0;
      txDmaDesc[i].tdes2 = 0;
      txDmaDesc[i].tdes3 = 0;
   }

   //Initialize TX descriptor index
   txIndex = 0;

   //Initialize RX DMA descriptor list
   for(i = 0; i < ETH_RX_BUFFER_COUNT; i++)
   {
      //The descriptor is initially owned by the DMA
      rxDmaDesc[i].rdes0 = (uint32_t) rxBuffer[i];
      rxDmaDesc[i].rdes1 = 0;
      rxDmaDesc[i].rdes2 = 0;
      rxDmaDesc[i].rdes3 = ETH_RDES3_OWN | ETH_RDES3_IOC | ETH_RDES3_BUF1V;
   }

   //Initialize RX descriptor index
   rxIndex = 0;

   //Start location of the TX descriptor list
   ETH->DMACTDLAR = (uint32_t) &txDmaDesc[0];
   //Length of the transmit descriptor ring
   ETH->DMACTDRLR = ETH_TX_BUFFER_COUNT - 1;

   //Start location of the RX descriptor list
   ETH->DMACRDLAR = (uint32_t) &rxDmaDesc[0];
   //Length of the receive descriptor ring
   ETH->DMACRDRLR = ETH_RX_BUFFER_COUNT - 1;
}


/* interrupt service routine */
void ETH_IRQHandler(void)
{
    rt_uint32_t status;

    /* enter interrupt */
    rt_interrupt_enter();

	   //Read DMA status register
    status = ETH->DMACSR;
   // ier = ETH->DMACIER;
	 if(status & ETH_DMACSR_NIS)
	 {
	 volatile	rt_uint32_t nis_clear = ETH_DMACSR_NIS;
   //A packet has been transmitted?
   if(status & ETH_DMACSR_TI)
   {
      //Clear TI interrupt flag
      nis_clear |= ETH_DMACSR_TI;

      LOG_D("ETH_DMACSR_TI\r\n");
     if(!(txDmaDesc[txIndex].tdes3 & ETH_TDES3_OWN))
      {
			 if (tx_is_waiting == RT_TRUE)
			 {
					 tx_is_waiting = RT_FALSE;
					 rt_sem_release(&tx_wait);
			 }
			}
   }

   //A packet has been received?
   if(status & ETH_DMACSR_RI)
   {
      //Clear RI interrupt flag
      nis_clear |= ETH_DMACSR_RI;

      LOG_D("ETH_DMACSR_RI\r\n");
       /* a frame has been received */
      eth_device_ready(&(stm32_eth_device.parent));
   }

   //Clear NIS interrupt flag
   ETH->DMACSR = nis_clear;
   }

    if (status & ETH_DMACSR_AIS)
    {
        rt_uint32_t ais_clear = ETH_DMACSR_AIS;
        LOG_D("ETH_DMA_IT_AIS\r\n");

        /* [1]:Transmit Process Stopped. */
        if (status & ETH_DMACSR_TPS)
        {
            LOG_D("AIS ETH_DMACSR_TPS\r\n");
            ais_clear |= ETH_DMACSR_TPS;
        }

        /* [2]: Transmit Buffer Unavailable. */
        if (status & ETH_DMACSR_TBU)
        {
            LOG_D("AIS ETH_DMACSR_TBU\r\n");
            ais_clear |= ETH_DMACSR_TBU;
        }

        /* [3]: Receive Buffer Unavailable. */
        if (status & ETH_DMACSR_RBU)
        {
            LOG_D("AIS ETH_DMACSR_RBU\r\n");
            ais_clear |= ETH_DMACSR_RBU;
        }

        /* [4]: Receive Process Stopped. */
        if (status & ETH_DMACSR_RPS)
        {
            LOG_D("AIS ETH_DMACSR_RPS\r\n");
            ais_clear |= ETH_DMACSR_RPS;
        }

        /* [5]: Receive Watchdog Timeout. */
        if (status & ETH_DMACSR_RWT)
        {
            LOG_D("AIS ETH_DMACSR_RWT\r\n");
            ais_clear |= ETH_DMACSR_RWT;
        }

        /* [6]: Early Transmit Interrupt. */
        if (status & ETH_DMACSR_ETI)
        {
            LOG_D("AIS ETH_DMACSR_ETI\r\n");
            ais_clear |= ETH_DMACSR_ETI;
        }
				
				/* [7]: Early Receive Interrupt. */
        if (status & ETH_DMACSR_ERI)
        {
            LOG_D("AIS ETH_DMACSR_ERI\r\n");
            ais_clear |= ETH_DMACSR_ERI;
        }
				
        /* [8]: Fatal Bus Error. */
        if (status & ETH_DMACSR_FBE)
        {
            LOG_D("AIS ETH_DMACSR_FBE\r\n");
            ais_clear |= ETH_DMACSR_FBE;
        }

         ETH->DMACSR = ais_clear;
    }

    /* leave interrupt */
    rt_interrupt_leave();
}
/**
 * @brief Adjust MAC configuration parameters for proper operation
 * @param[in] interface Underlying network interface
 * @return Error code
 **/

static rt_err_t ETH_UpdateMacConfig(stm32_eth_t *stm32_eth)
{
   uint32_t config;

   //Read current MAC configuration
   config = ETH->MACCR;

   //10BASE-T or 100BASE-TX operation mode?
   if(stm32_eth->ETH_Speed == ETH_Speed_100M)
      config |= ETH_MACCR_FES;
   else
      config &= ~ETH_MACCR_FES;

   //Half-duplex or full-duplex mode?
   if(stm32_eth->ETH_Mode == ETH_Mode_FullDuplex)
      config |= ETH_MACCR_DM;
   else
      config &= ~ETH_MACCR_DM;

   //Update MAC configuration register
   ETH->MACCR = config;

   //Successful processing
   return RT_EOK;
}


#if (LWIP_IPV4 && LWIP_IGMP) || (LWIP_IPV6 && LWIP_IPV6_MLD)
/* polynomial: 0x04C11DB7 */
static uint32_t ethcrc(const uint8_t *data, size_t length)
{
    uint32_t crc = 0xffffffff;
    size_t i;
    int j;

    for (i = 0; i < length; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if (((crc >> 31) ^ (data[i] >> j)) & 0x01)
            {
                /* x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1 */
                crc = (crc << 1) ^ 0x04C11DB7;
            }
            else
            {
                crc = crc << 1;
            }
        }
    }

    return ~crc;
}

#define HASH_BITS   6       /* #bits in hash */
static void register_multicast_address(stm32_eth_t *stm32_eth, const uint8_t *mac)
{
    uint32_t crc;
    uint8_t hash;

    /* calculate crc32 value of mac address */
    crc = ethcrc(mac, 6);

    /* only upper 6 bits (HASH_BITS) are used
    * which point to specific bit in he hash registers
    */
    hash = (crc >> 26) & 0x3F;
    //rt_kprintf("register_multicast_address crc: %08X hash: %02X\n", crc, hash);

    if (hash > 31)
    {
        stm32_eth->ETH_HashTableHigh |= 1 << (hash - 32);
        ETH->MACHT1R = stm32_eth->ETH_HashTableHigh;
    }
    else
    {
        stm32_eth->ETH_HashTableLow |= 1 << hash;
        ETH->MACHT0R = stm32_eth->ETH_HashTableLow;
    }
}
#endif /* (LWIP_IPV4 && LWIP_IGMP) || (LWIP_IPV6 && LWIP_IPV6_MLD) */

#if LWIP_IPV4 && LWIP_IGMP
static signed char igmp_mac_filter(struct netif *netif, const ip4_addr_t *ip4_addr, enum netif_mac_filter_action action)
{
    uint8_t mac[6];
    const uint8_t *p = (const uint8_t *)ip4_addr;
    stm32_eth_t *stm32_eth = (stm32_eth_t *)netif->state;

    mac[0] = 0x01;
    mac[1] = 0x00;
    mac[2] = 0x5E;
    mac[3] = *(p + 1) & 0x7F;
    mac[4] = *(p + 2);
    mac[5] = *(p + 3);

    register_multicast_address(stm32_eth, mac);

    if (1)
    {
        LOG_D("%s %s %s ", __FUNCTION__, (action == NETIF_ADD_MAC_FILTER) ? "add" : "del", ip4addr_ntoa(ip4_addr));
        LOG_D("%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    return 0;
}
#endif /* LWIP_IPV4 && LWIP_IGMP */

#if LWIP_IPV6 && LWIP_IPV6_MLD
static err_t mld_mac_filter(struct netif *netif, const ip6_addr_t *ip6_addr, enum netif_mac_filter_action action)
{
    uint8_t mac[6];
    const uint8_t *p = (const uint8_t *)&ip6_addr->addr[3];
    struct rt_stm32_eth *stm32_eth = (struct rt_stm32_eth *)netif->state;

    mac[0] = 0x33;
    mac[1] = 0x33;
    mac[2] = *(p + 0);
    mac[3] = *(p + 1);
    mac[4] = *(p + 2);
    mac[5] = *(p + 3);

    register_multicast_address(stm32_eth, mac);

    if (1)
    {
        LOG_D("%s %s %s ", __FUNCTION__, (action == NETIF_ADD_MAC_FILTER) ? "add" : "del", ip6addr_ntoa(ip6_addr));
        LOG_D("%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    return 0;
}
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

/* initialize the interface */
static rt_err_t rt_stm32_eth_init(rt_device_t dev)
{
    stm32_eth_t *stm32_eth = (stm32_eth_t *)dev;
   //Debug message
   LOG_D("Initializing STM32H7 Ethernet MAC...\r\n");

   //Enable Ethernet MAC clock
	  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ETH1MAC);
	  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ETH1TX);
	  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ETH1RX);

   //Reset Ethernet MAC peripheral
   LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ETH1MAC);
   LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ETH1MAC);

   //Perform a software reset
   ETH->DMAMR |= ETH_DMAMR_SWR;
   //Wait for the reset to complete
   while(ETH->DMAMR & ETH_DMAMR_SWR)
   {
   }

   //Adjust MDC clock range depending on HCLK frequency
   ETH->MACMDIOAR = ETH_MACMDIOAR_CR_DIV124;

   //Use default MAC configuration
   ETH->MACCR = ETH_MACCR_FES  | ETH_MACCR_DR | ETH_MACCR_IPC |ETH_MACCR_DM;

   //Set the MAC address of the station
   ETH->MACA0LR = ((uint32_t)stm32_eth->dev_addr[3] << 24) | ((uint32_t)stm32_eth->dev_addr[2] << 16) | ((uint32_t)stm32_eth->dev_addr[1] << 8) | stm32_eth->dev_addr[0];
   ETH->MACA0HR = ((uint32_t)stm32_eth->dev_addr[5] << 8) | stm32_eth->dev_addr[4];

   //The MAC supports 3 additional addresses for unicast perfect filtering
   ETH->MACA1LR = 0;
   ETH->MACA1HR = 0;
   ETH->MACA2LR = 0;
   ETH->MACA2HR = 0;
   ETH->MACA3LR = 0;
   ETH->MACA3HR = 0;

   //Initialize hash table
   ETH->MACHT0R = stm32_eth->ETH_HashTableLow;
   ETH->MACHT1R = stm32_eth->ETH_HashTableHigh;
	 
   //Configure the receive filter
   ETH->MACPFR = ETH_MACPFR_HPF | ETH_MACPFR_HMC;

   //Disable flow control
   ETH->MACTFCR = 0;
   ETH->MACRFCR = 0;

   //Configure DMA operating mode
   ETH->DMAMR = ETH_DMAMR_INTM_0 | ETH_DMAMR_PR_2_1;
   //Configure system bus mode
   ETH->DMASBMR |= ETH_DMASBMR_AAL | ETH_DMASBMR_FB;
   //The DMA takes the descriptor table as contiguous
   ETH->DMACCR = ETH_DMACCR_DSL_0BIT;

   //Configure TX features
   ETH->DMACTCR = ETH_DMACRCR_RPBL_32PBL;

   //Configure RX features
   ETH->DMACRCR = ETH_DMACRCR_RPBL_32PBL;
   ETH->DMACRCR |= (ETH_RX_BUFFER_SIZE << 1) & ETH_DMACRCR_RBSZ;

   //Enable store and forward mode
   ETH->MTLTQOMR |= ETH_MTLTQOMR_TSF;
   ETH->MTLRQOMR |= ETH_MTLRQOMR_RSF;

   //Initialize DMA descriptor lists
   Eth_InitDmaDesc();

   //Prevent interrupts from being generated when the transmit statistic
   //counters reach half their maximum value
   ETH->MMCTIMR = ETH_MMCTIMR_TXLPITRCIM | ETH_MMCTIMR_TXLPIUSCIM |
      ETH_MMCTIMR_TXGPKTIM | ETH_MMCTIMR_TXMCOLGPIM | ETH_MMCTIMR_TXSCOLGPIM;

   //Prevent interrupts from being generated when the receive statistic
   //counters reach half their maximum value
   ETH->MMCRIMR = ETH_MMCRIMR_RXLPITRCIM | ETH_MMCRIMR_RXLPIUSCIM |
      ETH_MMCRIMR_RXUCGPIM | ETH_MMCRIMR_RXALGNERPIM | ETH_MMCRIMR_RXCRCERPIM;

   //Disable MAC interrupts
   ETH->MACIER = 0;
   //Enable the desired DMA interrupts
   ETH->DMACIER = ETH_DMACIER_NIE | ETH_DMACIER_RIE | ETH_DMACIER_TIE;

   //Enable MAC transmission and reception
   ETH->MACCR |= ETH_MACCR_TE | ETH_MACCR_RE;

   //Enable DMA transmission and reception
   ETH->DMACTCR |= ETH_DMACTCR_ST;
   ETH->DMACRCR |= ETH_DMACRCR_SR;

    /* Clear Tx and Rx process stopped flags */
    ETH->DMACSR |= (ETH_DMACSR_TPS | ETH_DMACSR_RPS);
#if LWIP_IPV4 && LWIP_IGMP
    netif_set_igmp_mac_filter(stm32_eth->parent.netif, igmp_mac_filter);
#endif /* LWIP_IPV4 && LWIP_IGMP */

#if LWIP_IPV6 && LWIP_IPV6_MLD
    netif_set_mld_mac_filter(stm32_eth->parent.netif, mld_mac_filter);
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

    return RT_EOK;
}

static rt_err_t rt_stm32_eth_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rt_stm32_eth_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_size_t rt_stm32_eth_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_size_t rt_stm32_eth_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_err_t rt_stm32_eth_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd)
    {
    case NIOCTL_GADDR:
        /* get mac address */
        if (args) rt_memcpy(args, stm32_eth_device.dev_addr, 6);
        else return -RT_ERROR;
        break;

    default :
        break;
    }

    return RT_EOK;
}

/* ethernet device interface */
/* transmit packet. */
rt_err_t rt_stm32_eth_tx(rt_device_t dev, struct pbuf *p)
{
    struct pbuf *q;
    rt_uint32_t offset;

    /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
    while ((txDmaDesc[txIndex].tdes3 & ETH_TDES3_OWN) != (uint32_t)RESET)
    {
        rt_err_t result;
        rt_uint32_t level;

        level = rt_hw_interrupt_disable();
        tx_is_waiting = RT_TRUE;
        rt_hw_interrupt_enable(level);

        /* it's own bit set, wait it */
        result = rt_sem_take(&tx_wait, RT_WAITING_FOREVER);
        if (result == RT_EOK) break;
        if (result == -RT_ERROR) return -RT_ERROR;
    }

    offset = 0;
    for (q = p; q != NULL; q = q->next)
    {
        uint8_t *to;

        /* Copy the frame to be sent into memory pointed by the current ETHERNET DMA Tx descriptor */
        to = (uint8_t *)((txBuffer[txIndex]) + offset);
        rt_memcpy(to, q->payload, q->len);
        offset += q->len;
    }
#ifdef ETH_TX_DUMP
    {
        rt_uint32_t i;
        rt_uint8_t *ptr = (rt_uint8_t *)(txDmaDesc[txIndex].tdes0);

        LOG_D("tx_dump, len:%d\r\n", p->tot_len);
        for (i = 0; i < p->tot_len; i++)
        {
            LOG_RAW("%02x ", *ptr);
            ptr++;

            if (((i + 1) % 8) == 0)
            {
                LOG_RAW("  ");
            }
            if (((i + 1) % 16) == 0)
            {
                LOG_RAW("\r\n");
            }
        }
        LOG_RAW("\r\ndump done!\r\n");
    }
#endif

   //Set the start address of the buffer
   txDmaDesc[txIndex].tdes0 = (uint32_t) txBuffer[txIndex];
   //Write the number of bytes to send
   txDmaDesc[txIndex].tdes2 = ETH_TDES2_IOC | (p->tot_len & ETH_TDES2_B1L);
   //Give the ownership of the descriptor to the DMA
   txDmaDesc[txIndex].tdes3 = ETH_TDES3_OWN | ETH_TDES3_FD | ETH_TDES3_LD;

#ifdef CHECKSUM_BY_HARDWARE
    DMATxDescToSet->Status |= ETH_DMATxDesc_ChecksumTCPUDPICMPFull;
    /* clean ICMP checksum STM32F need */
    {
        struct eth_hdr *ethhdr = (struct eth_hdr *)(txDmaDesc[txIndex].tdes0);
        /* is IP ? */
        if (ethhdr->type == htons(ETHTYPE_IP))
        {
            struct ip_hdr *iphdr = (struct ip_hdr *)(txDmaDesc[txIndex].tdes0 + SIZEOF_ETH_HDR);
            /* is ICMP ? */
            if (IPH_PROTO(iphdr) == IP_PROTO_ICMP)
            {
                struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)(txDmaDesc[txIndex].tdes0 + SIZEOF_ETH_HDR + sizeof(struct ip_hdr));
                iecho->chksum = 0;
            }
        }
    }
#endif
   //Data synchronization barrier
   __DSB();
   //Clear TBU flag to resume processing
   ETH->DMACSR = ETH_DMACSR_TBU;
   //Instruct the DMA to poll the transmit descriptor list
   ETH->DMACTDTPR = 0;

   //Increment index and wrap around if necessary
   if(++txIndex >= ETH_TX_BUFFER_COUNT)
      txIndex = 0;

    /* Return SUCCESS */
    return RT_EOK;
}

/* reception packet. */
struct pbuf *rt_stm32_eth_rx(rt_device_t dev)
{
    struct pbuf *p;
    rt_uint32_t framelength = 0;
    rt_uint32_t offset = 0;
    /* init p pointer */
    p = RT_NULL;

    /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
    if (((rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_OWN) != (uint32_t)RESET))
        return p;
    if (((rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_ES) == (uint32_t)RESET) &&
            ((rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_LD) != (uint32_t)RESET) &&
            ((rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_FD) != (uint32_t)RESET))
    {
        //Retrieve the length of the frame
        framelength = rxDmaDesc[rxIndex].rdes3 & ETH_RDES3_PL;
        //Limit the number of data to read
			  framelength = (framelength>ETH_RX_BUFFER_SIZE) ? ETH_RX_BUFFER_SIZE:framelength;
			
        /* allocate buffer */
        p = pbuf_alloc(PBUF_LINK, framelength, PBUF_RAM);
        if (p != RT_NULL)
        {
            struct pbuf* q;

            for (q = p; q != RT_NULL; q= q->next)
            {
                /* Copy the received frame into buffer from memory pointed by the current ETHERNET DMA Rx descriptor */
                rt_memcpy(q->payload, (uint8_t *)((rxDmaDesc[rxIndex].rdes0) + offset), q->len);
                offset += q->len;
            }
#ifdef ETH_RX_DUMP
            {
                rt_uint32_t i;
                rt_uint8_t *ptr = (rt_uint8_t*)(rxDmaDesc[rxIndex].rdes0);

                LOG_D("rx_dump, len:%d\r\n", p->tot_len);
                for(i=0; i<p->tot_len; i++)
                {
                    LOG_RAW("%02x ", *ptr);
                    ptr++;

                    if(((i+1)%8) == 0)
                    {
                        LOG_RAW("  ");
                    }
                    if(((i+1)%16) == 0)
                    {
                        LOG_RAW("\r\n");
                    }
                }
                LOG_RAW("\r\ndump done!\r\n");
            }
#endif
        }
    }

      //Set the start address of the buffer
      rxDmaDesc[rxIndex].rdes0 = (uint32_t) rxBuffer[rxIndex];
      //Give the ownership of the descriptor back to the DMA
      rxDmaDesc[rxIndex].rdes3 = ETH_RDES3_OWN | ETH_RDES3_IOC | ETH_RDES3_BUF1V;

      //Increment index and wrap around if necessary
      if(++rxIndex >= ETH_RX_BUFFER_COUNT)
         rxIndex = 0;

	//Clear RBU flag to resume processing
    ETH->DMACSR = ETH_DMACSR_RBU;
    //Instruct the DMA to poll the receive descriptor list
    ETH->DMACRDTPR = 0;
		
    return p;
}

static void NVIC_Configuration(void)
{
   //Configure Ethernet interrupt priority
   NVIC_SetPriority(ETH_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                              ETH_IRQ_GROUP_PRIORITY, ETH_IRQ_SUB_PRIORITY));
	 NVIC_EnableIRQ(ETH_IRQn);
}

/*
 * GPIO Configuration for ETH
 */
static void GPIO_Configuration(void)
{
   LL_GPIO_InitTypeDef GPIO_InitStructure;
   //Enable SYSCFG clock
   LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SYSCFG);

   //Enable GPIO clocks
   LL_AHB4_GRP1_EnableClock(ETH_GPIO_RCC);

	 //Select RMII interface mode
   LL_SYSCFG_SetPHYInterface(LL_SYSCFG_ETH_RMII);
	
   //Configure RMII pins
   GPIO_InitStructure.Mode = LL_GPIO_MODE_ALTERNATE;
   GPIO_InitStructure.Pull = LL_GPIO_PULL_NO;
   GPIO_InitStructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
   GPIO_InitStructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  

   //Configure ETH_RMII_REF_CLK, ETH_MDIO , ETH_MDC, RMII_TX_EN and ETH_RMII_CRS_DV
   GPIO_InitStructure.Pin = ETH_REF_CLK_PIN;
   GPIO_InitStructure.Alternate = ETH_REF_CLK_AF;
   LL_GPIO_Init(ETH_REF_CLK_PORT, &GPIO_InitStructure);
   GPIO_InitStructure.Pin = ETH_MDIO_PIN;
   GPIO_InitStructure.Alternate = ETH_MDIO_AF;
   LL_GPIO_Init(ETH_MDIO_PORT, &GPIO_InitStructure);
   GPIO_InitStructure.Pin = ETH_MDC_PIN;
   GPIO_InitStructure.Alternate = ETH_MDC_AF;
   LL_GPIO_Init(ETH_MDC_PORT, &GPIO_InitStructure);
   GPIO_InitStructure.Pin = ETH_TX_EN_PIN;
   GPIO_InitStructure.Alternate = ETH_TX_EN_AF;
   LL_GPIO_Init(ETH_TX_EN_PORT, &GPIO_InitStructure);
   GPIO_InitStructure.Pin = ETH_CRS_DV_PIN;
   GPIO_InitStructure.Alternate = ETH_CRS_DV_AF;
   LL_GPIO_Init(ETH_CRS_DV_PORT, &GPIO_InitStructure);

   //Configure  ETH_RMII_TXD0  and ETH_RMII_TXD1
   GPIO_InitStructure.Pin = ETH_TXD0_PIN;
   GPIO_InitStructure.Alternate = ETH_TXD0_AF;
   LL_GPIO_Init(ETH_TXD0_PORT, &GPIO_InitStructure);
   GPIO_InitStructure.Pin = ETH_TXD1_PIN;
   GPIO_InitStructure.Alternate = ETH_TXD1_AF;
   LL_GPIO_Init(ETH_TXD1_PORT, &GPIO_InitStructure);

   //Configure ETH_RMII_RXD0  and ETH_RMII_RXD1 
   GPIO_InitStructure.Pin = ETH_RXD0_PIN;
   GPIO_InitStructure.Alternate = ETH_RXD0_AF;
   LL_GPIO_Init(ETH_RXD0_PORT, &GPIO_InitStructure);
   GPIO_InitStructure.Pin = ETH_RXD1_PIN;
   GPIO_InitStructure.Alternate = ETH_RXD1_AF;
   LL_GPIO_Init(ETH_RXD1_PORT, &GPIO_InitStructure);
	 
}
/**
  * @brief  Get the link state of LAN8742 device.
  * @param  pObj: Pointer to device object. 
  * @param  pLinkState: Pointer to link state
  * @retval LAN8742_STATUS_LINK_DOWN  if link is down
  *         LAN8742_STATUS_AUTONEGO_NOTDONE if Auto nego not completed 
  *         LAN8742_STATUS_100MBITS_FULLDUPLEX if 100Mb/s FD
  *         LAN8742_STATUS_100MBITS_HALFDUPLEX if 100Mb/s HD
  *         LAN8742_STATUS_10MBITS_FULLDUPLEX  if 10Mb/s FD
  *         LAN8742_STATUS_10MBITS_HALFDUPLEX  if 10Mb/s HD       
  *         LAN8742_STATUS_READ_ERROR if connot read register
  *         LAN8742_STATUS_WRITE_ERROR if connot write to register
  */
//int32_t LAN8742_GetLinkState(lan8742_Object_t *pObj)
//{
//  uint32_t readval = 0;
//  
//  /* Read Status register  */
//  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BSR, &readval) < 0)
//  {
//    return LAN8742_STATUS_READ_ERROR;
//  }
//  
//  /* Read Status register again */
//  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BSR, &readval) < 0)
//  {
//    return LAN8742_STATUS_READ_ERROR;
//  }
//  
//  if((readval & LAN8742_BSR_LINK_STATUS) == 0)
//  {
//    /* Return Link Down status */
//    return LAN8742_STATUS_LINK_DOWN;    
//  }
//  
//  /* Check Auto negotiaition */
//  if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_BCR, &readval) < 0)
//  {
//    return LAN8742_STATUS_READ_ERROR;
//  }
//  
//  if((readval & LAN8742_BCR_AUTONEGO_EN) != LAN8742_BCR_AUTONEGO_EN)
//  {
//    if(((readval & LAN8742_BCR_SPEED_SELECT) == LAN8742_BCR_SPEED_SELECT) && ((readval & LAN8742_BCR_DUPLEX_MODE) == LAN8742_BCR_DUPLEX_MODE)) 
//    {
//      return LAN8742_STATUS_100MBITS_FULLDUPLEX;
//    }
//    else if ((readval & LAN8742_BCR_SPEED_SELECT) == LAN8742_BCR_SPEED_SELECT)
//    {
//      return LAN8742_STATUS_100MBITS_HALFDUPLEX;
//    }        
//    else if ((readval & LAN8742_BCR_DUPLEX_MODE) == LAN8742_BCR_DUPLEX_MODE)
//    {
//      return LAN8742_STATUS_10MBITS_FULLDUPLEX;
//    }
//    else
//    {
//      return LAN8742_STATUS_10MBITS_HALFDUPLEX;
//    }  		
//  }
//  else /* Auto Nego enabled */
//  {
//    if(pObj->IO.ReadReg(pObj->DevAddr, LAN8742_PHYSCSR, &readval) < 0)
//    {
//      return LAN8742_STATUS_READ_ERROR;
//    }
//    
//    /* Check if auto nego not done */
//    if((readval & LAN8742_PHYSCSR_AUTONEGO_DONE) == 0)
//    {
//      return LAN8742_STATUS_AUTONEGO_NOTDONE;
//    }
//    
//    if((readval & LAN8742_PHYSCSR_HCDSPEEDMASK) == LAN8742_PHYSCSR_100BTX_FD)
//    {
//      return LAN8742_STATUS_100MBITS_FULLDUPLEX;
//    }
//    else if ((readval & LAN8742_PHYSCSR_HCDSPEEDMASK) == LAN8742_PHYSCSR_100BTX_HD)
//    {
//      return LAN8742_STATUS_100MBITS_HALFDUPLEX;
//    }
//    else if ((readval & LAN8742_PHYSCSR_HCDSPEEDMASK) == LAN8742_PHYSCSR_10BT_FD)
//    {
//      return LAN8742_STATUS_10MBITS_FULLDUPLEX;
//    }
//    else
//    {
//      return LAN8742_STATUS_10MBITS_HALFDUPLEX;
//    }				
//  }
//}

/* PHY: LAN8720 */
static uint8_t phy_speed = 0;
#define PHY_LINK_MASK       (1<<0)
#define PHY_100M_MASK       (1<<1)
#define PHY_DUPLEX_MASK     (1<<2)
static void phy_monitor_thread_entry(void *parameter)
{
    uint8_t phy_addr = 0xFF;
    uint8_t phy_speed_new = 0;
    rt_uint16_t regvalue;
    /* phy search */
    {
        rt_uint32_t addr;
     /* Get the device address from special mode register */  
     for(addr = 0; addr <= 31; addr ++)
     {
       regvalue = ETH_ReadPHYRegister(addr, LAN8742_SMR);
       if((regvalue & LAN8742_SMR_PHY_ADDR) == addr)
       {
         phy_addr = addr;
         break;
       }
     }
    } /* phy search */

    if (phy_addr == 0xFF)
    {
        LOG_E("phy not probe!\r\n");
        return;
    }
    else
    {
        LOG_D("found a phy, address:0x%02X\r\n", phy_addr);
    }

    /* RESET PHY */
    LOG_D("RESET PHY!\r\n");
	 
    ETH_WritePHYRegister(phy_addr, LAN8742_BCR, PHY_Reset);
    rt_thread_delay(RT_TICK_PER_SECOND * 2);
		regvalue = ETH_ReadPHYRegister(phy_addr, LAN8742_BCR);
		regvalue |= LAN8742_BCR_AUTONEGO_EN;
    ETH_WritePHYRegister(phy_addr, LAN8742_BCR, regvalue);

    while (1)
    {
			   /* Read Status register  */
        uint16_t status  = ETH_ReadPHYRegister(phy_addr, LAN8742_BSR);
			   
         /* Read Status register again */
        status  = ETH_ReadPHYRegister(phy_addr, LAN8742_BSR);
	
        LOG_D("LAN8720 status:0x%04X\r\n", status);

        phy_speed_new = 0;

        if (status & (PHY_AutoNego_Complete | PHY_Linked_Status))
        {
            uint16_t SR;

            SR = ETH_ReadPHYRegister(phy_addr, 31);
            LOG_D("LAN8720 REG 31:0x%04X\r\n", SR);

            SR = (SR >> 2) & 0x07; /* LAN8720, REG31[4:2], Speed Indication. */
            phy_speed_new = PHY_LINK_MASK;

            if ((SR & 0x03) == 2)
            {
                phy_speed_new |= PHY_100M_MASK;
            }

            if (SR & 0x04)
            {
                phy_speed_new |= PHY_DUPLEX_MASK;
            }
        }

        /* linkchange */
        if (phy_speed_new != phy_speed)
        {
            if (phy_speed_new & PHY_LINK_MASK)
            {
                LOG_D("link up ");

                if (phy_speed_new & PHY_100M_MASK)
                {
                    LOG_D("100Mbps");
                    stm32_eth_device.ETH_Speed = ETH_Speed_100M;
                }
                else
                {
                    stm32_eth_device.ETH_Speed = ETH_Speed_10M;
                    LOG_D("10Mbps");
                }

                if (phy_speed_new & PHY_DUPLEX_MASK)
                {
                    LOG_D(" full-duplex\r\n");
                    stm32_eth_device.ETH_Mode = ETH_Mode_FullDuplex;
                }
                else
                {
                    LOG_D(" half-duplex\r\n");
                    stm32_eth_device.ETH_Mode = ETH_Mode_HalfDuplex;
                }
                ETH_UpdateMacConfig(&stm32_eth_device);

                /* send link up. */
                eth_device_linkchange(&stm32_eth_device.parent, RT_TRUE);
            } /* link up. */
            else
            {
                LOG_D("link down\r\n");
                /* send link down. */
                eth_device_linkchange(&stm32_eth_device.parent, RT_FALSE);
            } /* link down. */

            phy_speed = phy_speed_new;
        } /* linkchange */

        rt_thread_delay(RT_TICK_PER_SECOND);
    } /* while(1) */
}

int eth_hw_init(void)
{
    GPIO_Configuration();
    NVIC_Configuration();

    stm32_eth_device.ETH_Speed = ETH_Speed_100M;
    stm32_eth_device.ETH_Mode  = ETH_Mode_FullDuplex;

    stm32_eth_device.dev_addr[0] = 0;
    stm32_eth_device.dev_addr[1] = 0x80;
	  stm32_eth_device.dev_addr[2] = 0xCD;
	  stm32_eth_device.dev_addr[3] = 0xEF;
	  stm32_eth_device.dev_addr[4] = 0x07;
	  stm32_eth_device.dev_addr[5] = 0x43;
	
    stm32_eth_device.parent.parent.init       = rt_stm32_eth_init;
    stm32_eth_device.parent.parent.open       = rt_stm32_eth_open;
    stm32_eth_device.parent.parent.close      = rt_stm32_eth_close;
    stm32_eth_device.parent.parent.read       = rt_stm32_eth_read;
    stm32_eth_device.parent.parent.write      = rt_stm32_eth_write;
    stm32_eth_device.parent.parent.control    = rt_stm32_eth_control;
    stm32_eth_device.parent.parent.user_data  = RT_NULL;

    stm32_eth_device.parent.eth_rx     = rt_stm32_eth_rx;
    stm32_eth_device.parent.eth_tx     = rt_stm32_eth_tx;

    /* init tx semaphore */
    rt_sem_init(&tx_wait, "tx_wait", 0, RT_IPC_FLAG_FIFO);

    /* register eth device */
    eth_device_init(&(stm32_eth_device.parent), "e0");

    /* start phy monitor */
    {
        rt_thread_t tid;
        tid = rt_thread_create("phy",
                               phy_monitor_thread_entry,
                               RT_NULL,
                               2048,
                               RT_THREAD_PRIORITY_MAX - 2,
                               2);
        if (tid != RT_NULL)
            rt_thread_startup(tid);
    }
    return 0;
}
INIT_PREV_EXPORT(eth_hw_init);
#endif

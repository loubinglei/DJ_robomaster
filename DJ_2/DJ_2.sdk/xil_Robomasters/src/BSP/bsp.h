#ifndef _BSP_H_
#define _BSP_H_

/************************** Constant Definitions *****************************/
#define     BSP_USART1_RX_BUF_SIZE            128u
#define     BSP_USART3_RX_BUF_SIZE            1024u
#define     BSP_USART3_TX_BUF_SIZE            512u

/************************** Function Prototypes ******************************/
void BSP_Init(void);

/************************** Variable Definitions *****************************/
extern XCanPs *CanInstPtr_0;
extern XCanPs *CanInstPtr_1;

#endif


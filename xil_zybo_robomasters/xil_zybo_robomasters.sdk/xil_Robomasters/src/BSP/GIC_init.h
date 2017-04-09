#ifndef __GIC_ININ_H__
#define __GIC_ININ_H__

/************************** Constant Definitions *****************************/
#define INTC				XScuGic
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID

/************************** Function Prototypes ******************************/
int GIC_init(INTC *IntcInstancePtr);

#endif 

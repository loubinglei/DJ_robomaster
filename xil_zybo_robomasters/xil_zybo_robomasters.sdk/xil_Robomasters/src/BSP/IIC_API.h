#ifndef __IIC_API_H_
#define __IIC_API_H_

/************************** Constant Definitions *****************************/
#define IIC_0_DEVICE_ID      	XPAR_XIICPS_0_DEVICE_ID
#define IIC_SCLK_RATE		 	300000  //100Khz
#define TEST_BUFFER_SIZE_IIC	10

/************************** Function Prototypes ******************************/
extern int IIC_Init(XIicPs *IicInstPtr,u16 DeviceId);
extern void IIC_Write(u8 dev_addr,u8 reg_addr, u8 SendData, int ByteCount);
extern void IIC_Read(u8 dev_addr,u8 reg_addr, u8 *RecvData, int ByteCount);

/************************** Variable Definitions *****************************/
extern XIicPs Iic;

#endif

#include "../main.h"

XIicPs Iic;



 int IIC_Init(XIicPs *IicInstPtr,u16 DeviceId)
 {
	int Status;
	XIicPs_Config *Config;

	Config = XIicPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(IicInstPtr, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XIicPs_SetSClk(IicInstPtr, IIC_SCLK_RATE);
	return XST_SUCCESS;
}

void IIC_Write(u8 dev_addr,u8 reg_addr, u8 SendData, int ByteCount) {
	SendBuffer[0] = reg_addr;
	SendBuffer[1] = SendData;
	XIicPs_MasterSendPolled(&Iic, SendBuffer, ByteCount, dev_addr);

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(&Iic)) {
		/* NOP */
	}
}

void IIC_Read(u8 dev_addr,u8 reg_addr, u8 *RecvData, int ByteCount) {
	u8 RecvAddr[1];
	RecvAddr[0] = reg_addr;

	XIicPs_MasterSendPolled(&Iic, RecvAddr, 1	     , dev_addr );
	XIicPs_MasterRecvPolled(&Iic, RecvData, ByteCount, dev_addr );
}

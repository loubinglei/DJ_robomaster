/*
 * eeprom_API.h
 *
 *  Created on: 2015��8��20��
 *      Author: harryfan
 */

#ifndef EEPROM_API_H_
#define EEPROM_API_H_

#define IIC_0_DEVICE_ID      XPAR_XIICPS_0_DEVICE_ID

#define IIC_SCLK_RATE		300000  //100Khz

#define MPU6050_SLAVE_ADDR		0x68   //the address of MPU6050
#define IIC_SLAVE_ADDR_R	0xE8   //the address of MPU6050 when reading it


#define TEST_BUFFER_SIZE_IIC	10

extern XIicPs Iic;

int IIC_Init(XIicPs *IicInstPtr,u16 DeviceId);
void IIC_Write(u8 dev_addr,u8 reg_addr, u8 SendData, int ByteCount);
void IIC_Read(u8 dev_addr,u8 reg_addr, u8 *RecvData, int ByteCount);

u8  MPU6050_receive[16];  //
u16  GYRO_receive;  //
u16  ACC_Z_receive;  //

#endif /* EEPROM_API_H_ */
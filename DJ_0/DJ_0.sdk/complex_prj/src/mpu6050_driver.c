#include "main.h"

volatile MPU6050_RAW_DATA    MPU6050_Raw_Data;    //ԭʼ����
volatile MPU6050_REAL_DATA   MPU6050_Real_Data;
AHRS ahrs;
uint8_t mpu_buf[20]={0};       //save the data of acc gyro & mag using iic

int16_t MPU6050_FIFO[6][11] = {{0},{0}};//[0]-[9]Ϊ���10������ [10]Ϊ10�����ݵ�ƽ��ֵ
int16_t HMC5883_FIFO[3][11] = {{0},{0}};//[0]-[9]Ϊ���10������ [10]Ϊ10�����ݵ�ƽ��ֵ ע���Ŵ������Ĳ���Ƶ���������Ե����г�
MagMaxMinData_t MagMaxMinData;


float HMC5883_lastx,HMC5883_lasty,HMC5883_lastz;
//MPU6050 ��ʼ�����ɹ�����0  ʧ�ܷ��� 0xff
int MPU6050_Init(void)    //IIC_Write�е�dev_addr ֻ��д0x68  ����д0xD0 ��Ȼ����������
{
    unsigned char temp_data = 0x00;

    //IIC_GPIO_Init();  //��ʼ��IIC�ӿ�
    //HEAT_Configuration();
    
 //   IIC_Read(MPU6050_DEVICE_ADDRESS,WHO_AM_I,&temp_data,1);
 //   if(temp_data != MPU6050_ID)
 //   {
 //       printf("error 1A\r\n");
 //       return 0xff;
 //   }
 //   printf("temp_data= %x \r\n",temp_data);
    IIC_Write(MPU6050_ID,PWR_MGMT_1  ,0x01,2);   //�������״̬

    IIC_Write(MPU6050_ID,CONFIG      ,0x03,2);   //Digital Low-Pass Filter:DLPF_CFG is 3, Fs is 1khz
                                    //acc bandwidth 44Hz,gyro 42Hz

    IIC_Write(MPU6050_ID,GYRO_CONFIG ,0x10,2); //AFS_SEL 1: accelerometer full scale range is +-2g

    IIC_Write(MPU6050_ID,ACCEL_CONFIG ,0x00,2);   //AFS_SEL 1: accelerometer full scale range is +-2g

    IIC_Write(MPU6050_ID,INT_PIN_CFG ,0x02,2);  //logic level for the INT pin is active high
                                      //the INT pin emits a 50us long pulse, not latched    bypass mode enabled

    IIC_Write(MPU6050_ID,INT_ENABLE ,0x00,2);   //disable data ready interrupt

    //����mpu6050 IIC masters mode  disabled ����mpu6050����aux IIC�ӿ�
    IIC_Write(MPU6050_ID,MPU6050_RA_USER_CTRL ,0x00,2); //disable data ready interrupt
		
		
    //5883��ʼ��
    HMC5883_Init();

    return 0;
}

int MPU6050_EnableInt(void)
{
/*	  if(IIC_WriteData(MPU6050_DEVICE_ADDRESS,SMPLRT_DIV,0x01)==0xff)      //Sample Rate: Gyro output rate / (1 + 1) = 500Hz
	  {
        printf("Cannot enable interrupt successfully.\r\n");
        return 0xff;
	  }
    printf("MPU6050 set sample rate done.\n");
	  Delay_ms(10);
	  if(IIC_WriteData(MPU6050_DEVICE_ADDRESS,INT_ENABLE,0x01) == 0xff)     //enable data ready interrupt 
    {
      printf("error 1I\r\n");
      return 0xff;
    } 
#if 0
    printf("MPU6050 enable interrupt done.\n"); 
//	unsigned char  buf[14] ={0};    
//    Delay_ms(2);
//	IIC_ReadData(MPU6050_DEVICE_ADDRESS,MPU6050_DATA_START,buf,14);    //dummy read to clear the data registers
//	Delay_ms(10);
#endif
	return 0;
*/
}

void MPU6050_Initialize(void)
{
 /*   while(MPU6050_Init() == 0xff)
    {                       
       Delay_ms(200);              
    }
  */
}

//MPU6050  ���ݶ�ȡ���ɹ�����0  ʧ�ܷ��� 0xff



int MPU6050_ReadData(uint8_t Slave_Addr, uint8_t Reg_Addr, uint8_t * Data, uint8_t Num)
{    
    IIC_Read(Slave_Addr,Reg_Addr,Data,Num);
    return 0;
}

/**********************************************************************************/
/*��MPU6050_ax,MPU6050_ay, MPU6050_az,MPU6050_gx, MPU6050_gy, MPU6050_gz�����洢*/
/**********************************************************************************/
void MPU6050_DataSave(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz) //[0]-[9]Ϊ���10������ [10]Ϊ10�����ݵ�ƽ��ֵ
{
	uint8_t i = 0;
	int32_t sum=0;
	
	for(i=1;i<10;i++)
	{
		MPU6050_FIFO[0][i-1]=MPU6050_FIFO[0][i];
		MPU6050_FIFO[1][i-1]=MPU6050_FIFO[1][i];
		MPU6050_FIFO[2][i-1]=MPU6050_FIFO[2][i];
		MPU6050_FIFO[3][i-1]=MPU6050_FIFO[3][i];
		MPU6050_FIFO[4][i-1]=MPU6050_FIFO[4][i];
		MPU6050_FIFO[5][i-1]=MPU6050_FIFO[5][i];
	}
	
	MPU6050_FIFO[0][9]=ax;//���µ����ݷ��õ� ���ݵ������
	MPU6050_FIFO[1][9]=ay;
	MPU6050_FIFO[2][9]=az;
	MPU6050_FIFO[3][9]=gx;
	MPU6050_FIFO[4][9]=gy;
	MPU6050_FIFO[5][9]=gz;
	
	for(i=0;i<10;i++)//��ǰ����ĺϣ���ȡƽ��ֵ
	{	
		 sum+=MPU6050_FIFO[0][i];
	}
	MPU6050_FIFO[0][10]=sum/10;

	sum=0;
	for(i=0;i<10;i++){
		 sum+=MPU6050_FIFO[1][i];
	}
	MPU6050_FIFO[1][10]=sum/10;

	sum=0;
	for(i=0;i<10;i++){
		 sum+=MPU6050_FIFO[2][i];
	}
	MPU6050_FIFO[2][10]=sum/10;

	sum=0;
	for(i=0;i<10;i++){
		 sum+=MPU6050_FIFO[3][i];
	}
	MPU6050_FIFO[3][10]=sum/10;

	sum=0;
	for(i=0;i<10;i++){
		 sum+=MPU6050_FIFO[4][i];
	}
	MPU6050_FIFO[4][10]=sum/10;

	sum=0;
	for(i=0;i<10;i++){
		 sum+=MPU6050_FIFO[5][i];
	}
	MPU6050_FIFO[5][10]=sum/10;
	
}


int16_t MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz
				,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz;
/**************************ʵ�ֺ���********************************************
*����ԭ��:		void MPU6050_getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
*��������:	    ��ȡ MPU6050�ĵ�ǰ����ֵ
*******************************************************************************/
void MPU6050_getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz) {
  

		MPU6050_ReadData(MPU6050_ID,MPU6050_DATA_START,mpu_buf,14);  //
		HMC58X3_ReadData(&(mpu_buf[14]));  //14-19Ϊ����������
		MPU6050_Lastax=(((int16_t)mpu_buf[0]) << 8) | mpu_buf[1];
		MPU6050_Lastay=(((int16_t)mpu_buf[2]) << 8) | mpu_buf[3];
		MPU6050_Lastaz=(((int16_t)mpu_buf[4]) << 8) | mpu_buf[5];
		//�����¶�ADC
		MPU6050_Lastgx=(((int16_t)mpu_buf[8]) << 8) | mpu_buf[9];
		MPU6050_Lastgy=(((int16_t)mpu_buf[10]) << 8) | mpu_buf[11];
		MPU6050_Lastgz=(((int16_t)mpu_buf[12]) << 8) | mpu_buf[13];
			
		MPU6050_DataSave(MPU6050_Lastax,MPU6050_Lastay,MPU6050_Lastaz,MPU6050_Lastgx,MPU6050_Lastgy,MPU6050_Lastgz);  		
		*ax  =MPU6050_FIFO[0][10];
		*ay  =MPU6050_FIFO[1][10];
		*az = MPU6050_FIFO[2][10];
		*gx  =MPU6050_FIFO[3][10] - GyroSavedCaliData.GyroXOffset;
		*gy = MPU6050_FIFO[4][10] - GyroSavedCaliData.GyroYOffset;
		*gz = MPU6050_FIFO[5][10] - GyroSavedCaliData.GyroZOffset;

}

void MPU6050_getlastMotion6(int16_t* ax, int16_t* ay, 
		int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz)
{
/*	*ax  =MPU6050_FIFO[0][10];
	*ay  =MPU6050_FIFO[1][10];
	*az = MPU6050_FIFO[2][10];
	*gx  =MPU6050_FIFO[3][10]-GyroSavedCaliData.GyroXOffset;
	*gy = MPU6050_FIFO[4][10]-GyroSavedCaliData.GyroYOffset;
	*gz = MPU6050_FIFO[5][10]-GyroSavedCaliData.GyroZOffset;
*/
}

uint8_t HMC5883_Init(void)  //д��ַ0X3C 0X48�Ǵ��    0X1E�ǶԵ�
{
    uint8_t tmp_ch = 0;

    MPU6050_ReadData(0x1E, HMC58X3_R_IDA, &tmp_ch, 1);
    printf("tmp_ch = %x\n\r",tmp_ch);   //tem_ch����0X48˵�� �豸������ȷ����


    IIC_Write(0x1E, HMC58X3_R_CONFA,0x70,2);
    usleep(5 * 1000); //5ms
    IIC_Write(0x1E, HMC58X3_R_CONFB,0xA0,2);
    usleep(5 * 1000); //5ms
    IIC_Write(0x1E, HMC58X3_R_MODE,0x00,2);    //�����ʼ��Ϊ0x00 ����ģʽ
    //wait the response of the hmc5883 stabalizes, 6 milliseconds  
    usleep(6 * 1000); //6ms
    //set DOR
    IIC_Write(0x1E, HMC58X3_R_CONFA,0x18,2);   //75HZ����
    usleep(6 * 1000); //6ms


    return 0;

}


/**************************ʵ�ֺ���********************************************
*����ԭ��:	  void HMC58X3_getRaw(int16_t *x,int16_t *y,int16_t *z)
*��������:	   дHMC5883L�ļĴ�����ȡ5883�Ĵ�����ֵ
���������    reg  �Ĵ�����ַ
			  val   Ҫд���ֵ	
���������  ��
*******************************************************************************/
void HMC58X3_ReadData(u8 *vbuff) {   
   MPU6050_ReadData(0x1E,HMC58X3_R_XM,vbuff,6);   //��ȡ������������
}


/**************************ʵ�ֺ���********************************************
*����ԭ��:	   void  HMC58X3_newValues(int16_t x,int16_t y,int16_t z)
*��������:	   ����һ�����ݵ�FIFO����
���������  �������������Ӧ��ADCֵ
���������  ��
*******************************************************************************/
void  HMC58X3_newValues(int16_t x,int16_t y,int16_t z)
{
	uint8_t i = 0;
	int32_t sum=0;

	for(i=1;i<10;i++)
	{
		HMC5883_FIFO[0][i-1]=HMC5883_FIFO[0][i];
		HMC5883_FIFO[1][i-1]=HMC5883_FIFO[1][i];
		HMC5883_FIFO[2][i-1]=HMC5883_FIFO[2][i];
	}
	HMC5883_FIFO[0][9]= x;//���µ����ݷ��õ� ���ݵ������
	HMC5883_FIFO[1][9]= y;
	HMC5883_FIFO[2][9]= z;
	
	for(i=0;i<10;i++)//��ǰ����ĺϣ���ȡƽ��ֵ
	{	
		 sum+=HMC5883_FIFO[0][i];
	}
	HMC5883_FIFO[0][10]=sum/10;

	sum=0;
	for(i=0;i<10;i++){
		 sum+=HMC5883_FIFO[1][i];
	}
	HMC5883_FIFO[1][10]=sum/10;

	sum=0;
	for(i=0;i<10;i++){
		 sum+=HMC5883_FIFO[2][i];
	}
	HMC5883_FIFO[2][10]=sum/10;
	//����ȫ��ΪδУ׼����
	if(MagMaxMinData.MinMagX>HMC5883_FIFO[0][10])
	{
		MagMaxMinData.MinMagX=(int16_t)HMC5883_FIFO[0][10];
	}
	if(MagMaxMinData.MinMagY>HMC5883_FIFO[1][10])
	{
		MagMaxMinData.MinMagY=(int16_t)HMC5883_FIFO[1][10];
	}
	if(MagMaxMinData.MinMagZ>HMC5883_FIFO[2][10])
	{
		MagMaxMinData.MinMagZ=(int16_t)HMC5883_FIFO[2][10];
	}

	if(MagMaxMinData.MaxMagX<HMC5883_FIFO[0][10])
	{
		MagMaxMinData.MaxMagX=(int16_t)HMC5883_FIFO[0][10];		
	}
	if(MagMaxMinData.MaxMagY<HMC5883_FIFO[1][10])
	{
		MagMaxMinData.MaxMagY = HMC5883_FIFO[1][10];
	}
	if(MagMaxMinData.MaxMagZ<HMC5883_FIFO[2][10])
	{
		MagMaxMinData.MaxMagZ=(int16_t)HMC5883_FIFO[2][10];
	}		
}


/**************************ʵ�ֺ���********************************************
*����ԭ��:	  void HMC58X3_getRaw(int16_t *x,int16_t *y,int16_t *z)
*��������:	   дHMC5883L�ļĴ���
���������    reg  �Ĵ�����ַ
			  val   Ҫд���ֵ	
���������  ��
*******************************************************************************/
void HMC58X3_getRaw(int16_t *x,int16_t *y,int16_t *z) 
{
    HMC58X3_ReadData(&mpu_buf[14]);
    //printf("mpu_buf[14] = %x\n\r",mpu_buf[19]);
    HMC58X3_newValues((((int16_t)mpu_buf[18] << 8) | mpu_buf[19]), -(((int16_t)mpu_buf[14] << 8) | mpu_buf[15]), ((int16_t)mpu_buf[16] << 8) | mpu_buf[17]);
    *x = HMC5883_FIFO[0][10];
    *y = HMC5883_FIFO[1][10];
    *z = HMC5883_FIFO[2][10];
    //printf("*x = %x\n\r",*x);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:	  void HMC58X3_getValues(int16_t *x,int16_t *y,int16_t *z)
*��������:	   ��ȡ �����Ƶĵ�ǰADCֵ
���������    �������Ӧ�����ָ��	
���������  ��
*******************************************************************************/
void HMC58X3_getlastValues(int16_t *x,int16_t *y,int16_t *z) 
{
    *x = HMC5883_FIFO[0][10];
    *y = HMC5883_FIFO[1][10]; 
    *z = HMC5883_FIFO[2][10]; 
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:	  void HMC58X3_mgetValues(volatile float *arry)
*��������:	   ��ȡ У����� ������ADCֵ
���������    �������ָ��	
���������  ��
*******************************************************************************/
void HMC58X3_mgetValues(float *arry)
{
    int16_t xr,yr,zr;
    HMC58X3_getRaw(&xr, &yr, &zr);
    arry[0]= HMC5883_lastx=((float)(xr - MagSavedCaliData.MagXOffset)) * MagSavedCaliData.MagXScale;
    arry[1]= HMC5883_lasty=((float)(yr - MagSavedCaliData.MagYOffset)) * MagSavedCaliData.MagYScale;
    arry[2]= HMC5883_lastz=((float)(zr - MagSavedCaliData.MagZOffset)) * MagSavedCaliData.MagZScale;
   // printf("arry[0] = %f\n\r",arry[0]);
}







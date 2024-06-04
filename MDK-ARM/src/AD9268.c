#include "AD9268.h"
#include "usart.h"

unsigned char SPI_TxBuf[256];
unsigned char SPI_RxBuf[16];
unsigned char SPI1_ReadBytes(unsigned char RegAddr, unsigned char *ReadBuf)
{
    CS_ADC_LOW();//首先使能片选
    SPI_TxBuf[0] = 0X80;
		SPI_TxBuf[1] = RegAddr;
		
    HAL_SPI_Transmit(&hspi1, SPI_TxBuf, 2, 100);//发送设备地址,这里的命令是去读设备的寄存器
    HAL_SPI_Receive(&hspi1, ReadBuf, sizeof (ReadBuf), 100);
		HAL_StatusTypeDef status;
		status = HAL_SPI_Receive(&hspi1, ReadBuf, sizeof(ReadBuf), 100);
		CS_ADC_HIGH();//拉高片选
	
		if (status == HAL_OK) {
			// 接收成功
			printf("SPI Read Successful: ");
			for (int i = 0; i < sizeof(ReadBuf); i++) {
					printf("%d ", ReadBuf[i]);
					
			}
			printf("%d ", sizeof(ReadBuf));
			printf("\r\n");
		} else {
			// 接收失败
			printf("SPI Read Failed\r\n");
		}
    
    return 0;
}
unsigned char SPI1_WriteBytes(unsigned char RegAddr, const unsigned char *WriteBuf, unsigned char Len)//spi写寄存器
{
    CS_ADC_LOW();//首先使能片选
    SPI_TxBuf[0] = 0X00;  //这里是写命令控制和每次写入1字节
    SPI_TxBuf[1] = RegAddr;

    for(int i=0;i<Len;i++)
    {
        SPI_TxBuf[i+2] = WriteBuf[i];//这里写的是寄存器的值
    }
    HAL_SPI_Transmit(&hspi1, SPI_TxBuf, Len+2, 100);//传输数据
    CS_ADC_HIGH();
    return 0;
}
void AD9268_WriteByte(unsigned char RegAddr, unsigned char *WriteBuf, unsigned char Len) {
    CS_ADC_LOW(); // Enable the device
    delay_us(20);//此处的延时有没有必要？要不要改小或改大？
    SPI1_WriteBytes(RegAddr, WriteBuf, Len);  // SPI write to register
    delay_us(20);
    CS_ADC_HIGH(); // Disable the device
}


/*************************AD9268 Initialization****************************/
void Init_AD9268() {
    HAL_SPI_Init(&hspi1);

    // 配置寄存器0x0014，写入数据格式为二进制补码输出
    AD9268_WriteByte(0x14, (unsigned char[]){0x01}, 1);
    AD9268_WriteByte(0x14, (unsigned char[]){0x01}, 1);
//		SPI1_ReadBytes(0x14,SPI_RxBuf);

    // 启用抖动模式，相当于添加一个随机的DAC模拟信号量
    AD9268_WriteByte(0x30, (unsigned char[]){0x10}, 1);
		
    // 设置测试模式为0x000D，默认值0x00，普通ADC
    AD9268_WriteByte(0x0D, (unsigned char[]){0x00}, 1);

    // 电源模式设置为默认值 0x80
    AD9268_WriteByte(0x08, (unsigned char[]){0x80}, 1);

    // 选择ADC通道，默认值0x03，两个ADC都激活
    AD9268_WriteByte(0x05, (unsigned char[]){0x03}, 1);

    // 将延迟设置为 2258ps
    AD9268_WriteByte(0x17, (unsigned char[]){0x1C}, 1);

    // 直到将0x01写入地址0xFF，发出传输命令，
    // 设置传输位，寄存器开始更新
    AD9268_WriteByte(0xFF, (unsigned char[]){0x01}, 1);
}




/***************微妙级延迟***************/
void delay_us(__IO uint32_t delay)
{
    int last, curr, val;
    int temp;

    while (delay != 0)
    {
        temp = delay > 900 ? 900 : delay;
        last = SysTick->VAL;
        curr = last - CPU_FREQUENCY_MHZ * temp;
        if (curr >= 0)
        {
            do
            {
                val = SysTick->VAL;
            }
            while ((val < last) && (val >= curr));
        }
        else
        {
            curr += CPU_FREQUENCY_MHZ * 1000;
            do
            {
                val = SysTick->VAL;
            }
            while ((val <= last) || (val > curr));
        }
        delay -= temp;
    }
}

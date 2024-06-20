#include "AD9268.h"
#include "usart.h"
#define receive_len  1
unsigned char SPI_TxBuf[256];
unsigned char SPI_RxBuf[16];
unsigned char SPI1_ReadBytes(unsigned char RegAddr, unsigned char *ReadBuf)
{
    HAL_StatusTypeDef status;

    // 首先使能片选
    CS_ADC_LOW();
    delay_us(20);
	
    // 准备发送数据
    SPI_TxBuf[0] = 0X80; // 设置读命令
    SPI_TxBuf[1] = RegAddr; // 设置寄存器地址
    
    // 发送读命令和寄存器地址
    status = HAL_SPI_Transmit(&hspi1, SPI_TxBuf, 2, 100);
    if (status != HAL_OK) {
        CS_ADC_HIGH();
        printf("SPI Transmit Failed\r\n");
        return 1; // 传输失败，返回错误
    }
		
    // 接收寄存器数据
    status = HAL_SPI_Receive(&hspi1, ReadBuf, receive_len, 100);
		
		delay_us(20);
    CS_ADC_HIGH(); // 拉高片选

    // 检查接收状态
    if (status == HAL_OK) {
        // 接收成功
        // printf("SPI Read Successful: ");
        for (unsigned int i = 0; i < receive_len; i++) {
            printf("0x%02X ", ReadBuf[i]);
        }
        printf("\r\n");
    } else {
        // 接收失败
        printf("SPI Read Failed\r\n");
    }

    return ReadBuf[0];
    
    
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
	
		//配置MSB优先输出
		AD9268_WriteByte(0x00, (unsigned char[]){0x18}, 1);
			
    // 配置寄存器0x0014，写入数据格式为二进制补码输出，此时的输出变量应该是带符号的16位变量
    AD9268_WriteByte(0x14, (unsigned char[]){0x01}, 1);
    AD9268_WriteByte(0x14, (unsigned char[]){0x01}, 1);
		

    // 启用抖动模式，相当于添加一个随机的DAC模拟信号量
    AD9268_WriteByte(0x30, (unsigned char[]){0x10}, 1);
		
		
    // 设置测试模式为0x000D，默认值0x00，普通ADC
    AD9268_WriteByte(0x0D, (unsigned char[]){0x00}, 1);
		
		
    // 电源模式设置为默认值 0x80
    AD9268_WriteByte(0x08, (unsigned char[]){0x80}, 1);
	
		
    // 选择ADC通道，默认值0x02，ADC激活B通道
    AD9268_WriteByte(0x05, (unsigned char[]){0x02}, 1);
	
		
    // 将延迟设置为 2258ps
    AD9268_WriteByte(0x17, (unsigned char[]){0x1C}, 1);
		
    // 直到将0x01写入地址0xFF，发出传输命令，
    // 设置传输位，寄存器开始更新
		// 寄存器写完0x01写完自动清零
    AD9268_WriteByte(0xFF, (unsigned char[]){0x01}, 1);


		
		/**读数据检查**/
		//检查MSB优先输出
		while(SPI1_ReadBytes(0x00,SPI_RxBuf)!=0x18)
		{
			AD9268_WriteByte(0x00, (unsigned char[]){0x18}, 1);
			HAL_Delay(50);
		}
		//读取芯片ID，ID默认值0X32
		while(SPI1_ReadBytes(0x01,SPI_RxBuf)!=0x32)
		{
			HAL_Delay(50);
		}		
		while(SPI1_ReadBytes(0x14,SPI_RxBuf)!=0x01)
		{
			AD9268_WriteByte(0x14, (unsigned char[]){0x01}, 1);
			HAL_Delay(50);
		}
		while(SPI1_ReadBytes(0x30,SPI_RxBuf)!=0x10)
		{
			AD9268_WriteByte(0x30, (unsigned char[]){0x10}, 1);
			HAL_Delay(50);
		}
		while(SPI1_ReadBytes(0x0D,SPI_RxBuf)!=0x00)
		{
			AD9268_WriteByte(0x0D, (unsigned char[]){0x00}, 1);
			HAL_Delay(50);
		}
		while(SPI1_ReadBytes(0x08,SPI_RxBuf)!=0x80)
		{
			AD9268_WriteByte(0x08, (unsigned char[]){0x80}, 1);
			HAL_Delay(50);
		}	
		while(SPI1_ReadBytes(0x05,SPI_RxBuf)!=0x02)
		{
			AD9268_WriteByte(0x05, (unsigned char[]){0x02}, 1);
			HAL_Delay(50);
		}
		while(SPI1_ReadBytes(0x17,SPI_RxBuf)!=0x1C)
		{
			AD9268_WriteByte(0x17, (unsigned char[]){0x1C}, 1);
			HAL_Delay(50);
		}
		while(SPI1_ReadBytes(0xFF,SPI_RxBuf)!=0x00)
		{
			AD9268_WriteByte(0xFF, (unsigned char[]){0x01}, 1);
			HAL_Delay(50);
		}	
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
/****  ns级别的延时  ****/
void delay_500ns(void) {
    // 100 MHz CPU时钟周期为10纳秒
    // 500纳秒 / 10纳秒 = 50个CPU周期
	
	
	  /*
	     480M,周期为1/480us
	  */
    for (int i = 0; i < 2; i++) {
        __asm("nop");
    }
}



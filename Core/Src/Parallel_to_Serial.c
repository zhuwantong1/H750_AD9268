//
// Created by zhu on 2024/5/29.
//

#include "Parallel_to_Serial.h"
#include "stdio.h"
#include "usart.h"



//void Parallel_to_Serial(void){
//    /*** 在main中的速度要比中断定时器快得多，理论来讲应该是先运行Parallel_to_Serial再运行中断++ ***/
//    if(Data_Counter==0){
//        HAL_GPIO_WritePin(GPIOB, PL_Pin, GPIO_PIN_RESET);//第一个上升沿先拉低，开始传输数据
//    }
//    else if(Data_Counter >= 1&&Data_Counter<=16){
//        if(Data_Counter<=8)
//        {
//            INTERRUPT_FLAG_TIMER3[8-Data_Counter] = HAL_GPIO_ReadPin(DATA_S_GPIO_Port, DATA_S_Pin);
//        }else if(Data_Counter>8){
//            INTERRUPT_FLAG_TIMER3[24-Data_Counter] = HAL_GPIO_ReadPin(DATA_S_GPIO_Port, DATA_S_Pin);
//        }
//    }
//    else if(Data_Counter >=17){//如果已经接收到了16位数据，那就拉高PL，这时去处理数据，处理完再拉低
//        __disable_irq();//此时不能被中断，count不能计数，进行数据处理
//        HAL_GPIO_WritePin(GPIOB, PL_Pin, GPIO_PIN_SET);
//        Data_Counter = 0;
//        for(int i=0;i<16;i++)
//        {
//            ans =  (ans<<1) + INTERRUPT_FLAG_TIMER3[15-i];
//        }
//        HAL_UART_Transmit(&huart1, (const uint8_t *) &ans, sizeof(ans), HAL_MAX_DELAY);
//        printf("\r\n");
//        __enable_irq();
//    }
//}

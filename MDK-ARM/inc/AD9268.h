//
// Created by zhu on 2024/1/22.
//

#ifndef H750_SPI_AD9268_AD9268_H
#define H750_SPI_AD9268_AD9268_H
#include "spi.h"
#define CPU_FREQUENCY_MHZ    480		// STM32时钟主频


void Init_AD9268();
void AD9268_WriteByte(unsigned char RegAddr, unsigned char *WriteBuf, unsigned char Len);
void delay_us(__IO uint32_t delay);
#endif //H750_SPI_AD9268_AD9268_H

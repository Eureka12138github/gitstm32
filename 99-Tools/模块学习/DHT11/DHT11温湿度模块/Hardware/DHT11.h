//#ifndef __DHT11_H
//#define __DHT11_H
//#include "stm32f10x.h"
//// Device header
//#define DHT11_High GPIO_SetBits(GPIOB, GPIO_Pin_5)
//#define DHT11_Low GPIO_ResetBits(GPIOB, GPIO_Pin_5)
//#define DHT11_DQ GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)
//u8 DHT11_Init(void);
//u8 DHT11_Read_Data(u8 *temp,u8 *humi);
//u8 DHT11_Check(void);
//void DHT11_Rst(void);

//#endif 

#ifndef DHT11_H
#define DHT11_H
#include "stm32f10x.h"
#include <stdbool.h> 
// Device header

#define DHT11_GPIO_PORT      GPIOB              
#define DHT11_GPIO_CLK       RCC_APB2Periph_GPIOB
#define DHT11_PIN 			 GPIO_Pin_5
#define DHT11_HIGH() 		 GPIO_SetBits(DHT11_GPIO_PORT, DHT11_PIN)
#define DHT11_LOW()  	     GPIO_ResetBits(DHT11_GPIO_PORT, DHT11_PIN)
#define DHT11_READ()        GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_PIN)

bool DHT11_Init(void);
bool DHT11_Read_Data(u16 *temp,u16 *humi);
bool DHT11_Check(void);
void DHT11_Rst(void);

#endif 


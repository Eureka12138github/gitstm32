#ifndef __SERIAL_H
#define __SERIAL_H
#include <stdio.h>
extern uint8_t Serial_TxPaket[];
extern uint8_t Serial_RxPaket[];
void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t*Array,uint16_t Length);
void Serial_SendString(char*String);
void Serial_SendNum(uint32_t Num,uint8_t Length);
void Serial_Printf(char *format,...);
uint8_t Serial_GetRxFlag(void);
void Serial_SendPacket(void);
#endif

//以空行结尾

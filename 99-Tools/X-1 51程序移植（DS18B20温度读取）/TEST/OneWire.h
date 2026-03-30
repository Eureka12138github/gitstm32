#ifndef __ONEWIRE_H
#define __ONEWIRE_H
uint8_t OneWire_ReceiveByte(void);
void OneWire_SendByte(uint8_t Byte);
uint8_t OneWire_ReceiveBit(void);
void OneWire_SendBit(uint8_t Bit);
uint8_t OneWire_Init(void);
#endif

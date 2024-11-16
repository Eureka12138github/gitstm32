#ifndef __ENCODER_H
#define __ENCODER_H
int16_t Encoder_Get(void);//如果是uint16_t则到零后会变成65525，若是int16_t，则表示被强制类型转换，变成了负数
void Encoder_Init(void);
#endif 

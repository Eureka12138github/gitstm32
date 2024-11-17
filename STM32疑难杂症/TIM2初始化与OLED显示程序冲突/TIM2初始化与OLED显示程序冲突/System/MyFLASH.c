#include "stm32f10x.h"

u32 MyFLASH_ReadWord(u32 Address)
{
	return *((__IO u32 *)(Address));//__IO等同于volatile,是防止编译器优化的
}

u16 MyFLASH_ReadHalfWord(u32 Address)
{
	return *((__IO u16 *)(Address));//__IO等同于volatile,是防止编译器优化的
}

u8 MyFLASH_ReadByte(u32 Address)
{
	return *((volatile u8 *)(Address));//先将地址强转为指针，再读取指针数据
}

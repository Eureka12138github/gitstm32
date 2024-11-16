#include "stm32f10x.h"                 // Device header
#include "Delay.h"
#include "LED.h"
#include "OLED.h"
#include "W25Q64.h"
uint8_t MID;
uint16_t DID;
uint8_t ArrayWrite[] = {0x01,0x02,0x03,0x04};
uint8_t ArrayRead[4];
int main(void)
{
	W25164_Init();
	OLED_Init();
	OLED_ShowString(1,1,"MID:   DID:");
	OLED_ShowString(2,1,"W:");
	OLED_ShowString(3,1,"R:");
//	uint8_t MID;
//	uint16_t DID;
	W25Q64_ReadID(&MID,&DID);
//	OLED_ShowString(1,5,"MID:");
	OLED_ShowHexNum(1,5,MID,2);
//	OLED_ShowString(2,1,"DID:");
	OLED_ShowHexNum(1,12,DID,4);
	//擦除，写入，读出
	W25Q64_Sector_Erase(0X000000);//每个扇区的地址都是XXX000~XXXFFF；故只要地址后三位为0，那么该地址肯定是某个扇区的起始地址
	W25Q64_PageProgram(0X000000,ArrayWrite,4);//将数组ArrayWrite中的四个数写入到扇区中
	W25Q64_ReadData(0X000000,ArrayRead,4);
	
	//显示
	OLED_ShowHexNum(2,3,ArrayWrite[0],2);
	OLED_ShowHexNum(2,7,ArrayWrite[1],2);
	OLED_ShowHexNum(2,11,ArrayWrite[2],2);
	OLED_ShowHexNum(2,15,ArrayWrite[3],2);
	
	OLED_ShowHexNum(3,3,ArrayRead[0],2);
	OLED_ShowHexNum(3,7,ArrayRead[1],2);
	OLED_ShowHexNum(3,11,ArrayRead[2],2);
	OLED_ShowHexNum(3,15,ArrayRead[3],2);
	//目前读取有问题，看到11-3 48分
	while(1)
	{
	
	}
}

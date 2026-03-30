#include "stm32f10x.h"                  // Device header
#include "MySPI.h"  
#include "W25Q64_INS.h"  
void W25164_Init(void)
{
	MySPI_Init();
}	

void W25Q64_ReadID(uint8_t *MID,uint16_t *DID)
{
	MySPI_Start();//CS引脚置低，开始传输
	MySPI_SwapByte(W25Q64_JEDEC_ID);//此0X9F代表读ID指令，在接下来的传输中从机将会发ID号给主机，但现在没有，故返回值不要，属于抛玉引砖
	*MID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);//此0XFF无实际意义，主要是为了把从机的数据交换过来，属于抛砖引玉，*MID收到厂商ID
	*DID = MySPI_SwapByte(W25Q64_DUMMY_BYTE);
	*DID = (*DID << 8) | MySPI_SwapByte(W25Q64_DUMMY_BYTE);	 
//	*DID <<= 8;
//	*DID |= MySPI_SwapByte(W25064_DUMMY_BYTE);
	MySPI_Stop();
}

void W25Q64_WriteEnable(void)//写使能
{
	MySPI_Start();
	MySPI_SwapByte(W25Q64_WRITE_ENABLE);//紧随MySPI_Start();的是指令码
	MySPI_Stop();
}

/**
 * @brief 等待 W25Q64 空闲（忙标志检测）
 * @note 超时时间设置为 10000 次循环
 *
 * @details 忙检测原理：
 *          1. 发送 0x05 命令（读状态寄存器 1）
 *          2. 持续读取状态字，检查 Bit0 (BUSY 标志)
 *          3. BUSY=1 表示正在执行操作，BUSY=0 表示空闲
 *
 * @attention 擦除和编程操作后必须调用此函数等待完成
 *            每次读取状态寄存器都需要独立的片选时序
 */
void W25Q64_WaitBusy(void)
{
	uint32_t TimeOut = 10000;
	uint8_t status;

	// 循环读取状态寄存器，直到 BUSY 位清零
	do
	{
		MySPI_Start();											// CS 拉低
		status = MySPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1); // 发送 0x05 命令并读取状态
		MySPI_Stop();											// CS 拉高

		TimeOut--;
		if (TimeOut == 0)
		{
			break; // 超时退出
		}
	} while ((status & 0x01) == 0x01); // 检查 BUSY 位（Bit0）
}

void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray,uint16_t Count)//指定地址写入多个字节
{
	uint16_t i;
	W25Q64_WriteEnable();
	MySPI_Start();
	MySPI_SwapByte(W25Q64_PAGE_PROGRAM);
	MySPI_SwapByte(Address >> 16);//连续发送24位地址
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	for(i = 0;i < Count;i ++)
{
	MySPI_SwapByte(DataArray[i]);	
}	
	MySPI_Stop();
	W25Q64_WaitBusy();//此为事后等待，与事前等待略有不同，事后更保险，事前更效率
}

void W25Q64_Sector_Erase(uint32_t Address)
{
	W25Q64_WriteEnable();//写操作前需使写使能，其该使能仅在接下来的一个时序内有效，故后面无需再加写失能了
	//一个时序指：从MySPI_Start();到MySPI_Stop();
	MySPI_Start();
	MySPI_SwapByte(W25Q64_SECTOR_ERASE_4KB);
	MySPI_SwapByte(Address >> 16);//连续发送24位地址
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	MySPI_Stop();
	W25Q64_WaitBusy();
}

/**
  * @brief 给地址、接收数据的数组及数量，即可将W25Q64相应的地址读出并放到给定数组中
  * @param  从地址 Address 读取、存放于 DataArray 、读取数量 Count
  * @retval 无
  */
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray,uint32_t Count)//读取数据范围可以很大，故这里是uint32_t Count
{
	uint32_t i;
	MySPI_Start();
	MySPI_SwapByte(W25Q64_READ_DATA);
	MySPI_SwapByte(Address >> 16);//连续发送24位地址
	MySPI_SwapByte(Address >> 8);
	MySPI_SwapByte(Address);
	for(i = 0;i < Count;i ++)
	{
	DataArray[i] =  MySPI_SwapByte(W25Q64_DUMMY_BYTE);		
	}
	MySPI_Stop();
	//此处无需等待，因为前两个操作写入与擦除函数中已经等待过了，执行此函数时肯定不忙
}

















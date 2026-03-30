#include "stm32f10x.h"
#include <time.h>
uint16_t MyRTC_Time[7] = {2024,10,18,19,19,55};//时间设定
void MyRTC_SetTime(void);
void MyRTC_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);
	
	PWR_BackupAccessCmd(ENABLE);
	
	if(BKP_ReadBackupRegister(BKP_DR1)!=0XA5A5)
	{
	RCC_LSEConfig(RCC_LSE_ON);//启动外部LSE晶振
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);//等待LSE启动完成
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);//选择时钟源
	RCC_RTCCLKCmd(ENABLE);//时钟使能
	RTC_WaitForSynchro();//等待同步
	RTC_WaitForLastTask();//等待上次操作完成
	RTC_SetPrescaler(32768-1);//配置预分频器，现在是1HZ
	RTC_WaitForLastTask();//等待上次操作完成
	MyRTC_SetTime();//初始化设置时间	
	BKP_WriteBackupRegister(BKP_DR1,0XA5A5);
	}
	else 
	{
	RCC_RTCCLKCmd(ENABLE);//时钟使能
	RTC_WaitForSynchro();//等待同步
	}
	//上面加了if语句判断主要是为了避免重复初始化导致时间复位，
	/*现在达到的效果是，初始化完一次后，按下复位初始化不会将
	时间重置，只有备用电源即VB断电且主电源断电后，重新连接时才会将时间
	重置，实现原理是第一次初始化时在BKP中写入一个值，即0XA5A5，
	（并非一定该值，也可以是其他的值，这只是个标志位），那么在第二次初始化时
	BKP_DR1中的值就应该是0XA5A5，根据If语句判断，就不会将时间重置。
	*/
	/*
	对RTC任何寄存器的写操作，都必须在前一次写操作结束后进行
	这是调用RTC_WaitForLastTask();的原因
	*/
//	RTC_SetCounter(1729244162);
//	RTC_WaitForLastTask();
//	MyRTC_SetTime();//设置初始时间
	/*
	初始化结果：CNT的值将从 1672588795 秒数开始，以一秒的时间间隔不断自增，读取CNT就能获取时间
	即调取：RTC_GetCounter();
	
	*/
	
}

void MyRTC_SetTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	time_date.tm_year = MyRTC_Time[0] - 1900;//将数组中的数据填充到结构体中
	time_date.tm_mon = MyRTC_Time[1] - 1;//减去的部分是偏移，至于为什么这样减，见12-1 Unix时间戳
	time_date.tm_mday = MyRTC_Time[2];
	time_date.tm_hour = MyRTC_Time[3];
	time_date.tm_min = MyRTC_Time[4];
	time_date.tm_sec = MyRTC_Time[5];
	
	time_cnt = mktime(&time_date)-8*60*60;//至此，日期时间到秒数的转换完成
	
	RTC_SetCounter(time_cnt);//将指定秒数写入到CNT中
	RTC_WaitForLastTask();//等待上次操作完成
}
void MyRTC_ReadTime(void)
{
	time_t time_cnt;
	struct tm time_date;
	time_cnt = RTC_GetCounter() +8*60*60;//获取CNT值，加上时区偏移
	time_date = *localtime(&time_cnt);//得到日期时间
	
	MyRTC_Time[0] = time_date.tm_year + 1900;//将结构体中的数据填充到数组中
	MyRTC_Time[1] = time_date.tm_mon + 1;//加上的部分是偏移，至于为什么这样加，见12-1 Unix时间戳
	MyRTC_Time[2] = time_date.tm_mday;
	MyRTC_Time[3] = time_date.tm_hour;
	MyRTC_Time[4] = time_date.tm_min;
	MyRTC_Time[5] = time_date.tm_sec;
	MyRTC_Time[6] = time_date.tm_wday;
	
}


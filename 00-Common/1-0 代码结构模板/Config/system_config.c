// system_config.c
#include "system_config.h"

// ⚠️ 此部分应为系统的整体配置，可能存在某些敏感信息，需在此定义然后外部调用，避免硬编码

//ESP8266
const char* ESP8266_SNTP_CONFIG = "AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n";

//一些全局变量
u16 Reset_Count = 0;



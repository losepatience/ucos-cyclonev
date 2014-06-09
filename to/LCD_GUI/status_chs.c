
#include "resources.h"

#ifdef IIC_Key_Board_LCD
#ifdef LCD_MODULE_ZX19264M1A
unsigned short ErrorCodeCHS[]={'0','X','0','0','0','0','0','0','0','0',0 };

//在USB chip中服务错误.
unsigned short *CHS_SVC_SX2 = (unsigned short*)"在USB chip中服务错误.";


//Service Call: chip1.
unsigned short *CHS_SVC_FPGA0= (unsigned short*)"Service Call: chip1.";


//Service Call: chip2.
unsigned short *CHS_SVC_FPGA1= (unsigned short*)"Service Call: chip2.";


//Service Call: chip3.
unsigned short *CHS_SVC_FPGA2= (unsigned short*)"Service Call: chip3.";


//Service Call: chip4
unsigned short *CHS_SVC_FPGA3= (unsigned short*)"Service Call: chip4.";


//Service Call: chip5.
unsigned short *CHS_SVC_UPDATE_FAIL= (unsigned short*)"Service Call: chip5.";


//Service Call: 6.
unsigned short *CHS_SVC_BYHX_DATA= (unsigned short*)"Service Call: chip6.";


//BYHX_TOOL check EEPROM 没有通过。
unsigned short *CHS_SVC_EEPROM_CHK= (unsigned short*)"BYHX_TOOL check EEPROM 没有通过";


//SDRAM 的Check没有通过
unsigned short *CHS_SVC_SDRAM_CHECK= (unsigned short*)"SDRAM 的Check没有通过";


//无法从Falsh读取FPGA.
unsigned short *CHS_SVC_FLASH= (unsigned short*)"无法从Falsh读取FPGA";

//SDRAM 太小，不能支持当前的设置。
unsigned short *CHS_SVC_SDRAM_NOMATCH= (unsigned short*)"SDRAM 太小，不能支持当前的设置。";

//读主板错误。
unsigned short *CHS_SVC_READBOARDID_FAIL= (unsigned short*)"读主板错误";

//MOTION初始化失败
unsigned short *CHS_SVC_MOTION_FAIL= (unsigned short*)"MOTION初始化失败";

//初始化头版加密芯片
unsigned short *CHS_SVC_HEADBOARD_FAIL= (unsigned short*)"初始化头版加密芯片失败";

//关机模式，请关闭打印机
unsigned short *CHS_SVC_POWEROFF_MODE= (unsigned short*)"关机模式，请关闭打印机";

//USB chip异常重启
unsigned short *CHS_FTA_SX2RESET= (unsigned short*)"USB chip异常重启";


//内部错误 1.
unsigned short *CHS_FTA_INTERNAL_WRONGHEADER= (unsigned short*)"内部错误 1";

//内部错误 2.
unsigned short *CHS_FTA_INTERNAL_WRONGHEADERSIZE= (unsigned short*)"内部错误 2";

//内部错误 3.
unsigned short *CHS_FTA_INTERNAL_JOBSTARTHEADER= (unsigned short*)"内部错误 3";

//内部错误 4.
unsigned short *CHS_FTA_INTERNAL_BANDDATASIZE = (unsigned short*)"内部错误 4";

//内部错误 5.
unsigned short *CHS_FTA_INTERNAL_WRONGFORMAT= (unsigned short*)"内部错误 5";

//内部错误 6.
unsigned short *CHS_FTA_INTERNAL_DMA0WORKING= (unsigned short*)"内部错误 6";
//内部错误 7.
unsigned short *CHS_FTA_INTERNAL_PRINTPOINT= (unsigned short*)"内部错误 7";

//内部错误 8.
unsigned short *CHS_FTA_INTERNAL_OLIMIT= (unsigned short*)"内部错误 8";

//内部错误 9.
unsigned short *CHS_FTA_INTERNAL_OPPLIMIT= (unsigned short*)"内部错误 9";

//运动step 1.
unsigned short *CHS_FTA_DSPINITS1= (unsigned short*)"运动step 1";

//运动step 2.
unsigned short *CHS_FTA_DSPINITS2 = (unsigned short*)"运动step 2";


//头板step1.
unsigned short *CHS_FTA_HEADINITS1 = (unsigned short*)"头板step1";

//头板step2.
unsigned short *CHS_FTA_HEADINITS2 = (unsigned short*)"头板step2";

//LVDS错误.
unsigned short *CHS_FTA_HEADTOMAINROAD = (unsigned short*)"LVDS错误";

//Band定义中的方向值超出定义
unsigned short *CHS_FTA_INTERNAL_BANDDIRECTION = (unsigned short*)"Band定义中的方向值超出定义";

//DSP升级失败.
unsigned short *CHS_FTA_DSPUPDATE_FAIL=(unsigned short*)"DSP升级失败";

//EEPROM读取失败	
unsigned short *CHS_FTA_EEPROM_READ=(unsigned short*)"EEPROM读取失败	";

//EEPROM写入失败
unsigned short *CHS_FTA_EEPROM_WRITE=(unsigned short*)"EEPROM写入失败";


//板子出厂没有初始化.
unsigned short *CHS_FTA_FACTORY_DATA=(unsigned short*)"板子出厂没有初始化";

//头板复位.
unsigned short *CHS_FTA_HEADBOARD_RESET=(unsigned short*)"头板复位";

//Spectra高压板无法在第1步初始化.
unsigned short *CHS_FTA_SPECTRAHVBINITS1=(unsigned short*)"Spectra高压板无法在第1步初始化";

//头板与出厂数据不匹配,请重置出厂数据或者更换头板.
unsigned short *CHS_FTA_PRINTHEAD_NOTMATCH=(unsigned short*)"头板与出厂数据不匹配,请重置出厂数据或者更换头板";

//板子的厂商ID不匹配,请升级固件.
unsigned short *CHS_FTA_MANUFACTURERID_NOTMATCH=(unsigned short*)"板子的厂商ID不匹配,请升级固件";

//严重错误: 超过限时时间，请输入新的密码.
unsigned short *CHS_FTA_LIMITEDTIME_RUNOUT=(unsigned short*)"严重错误: 超过限时时间，请输入新的密码";


//内部错误,空白Band或Y向距离为零.
unsigned short *CHS_FTA_INTERNAL_SW1=(unsigned short*)"内部错误,空白Band或Y向距离为零";


//USB1.1连接.
unsigned short *CHS_FTA_USB1_USB1CONNECT=(unsigned short*)"USB1.1错误\n USB连接重新建立中...";

//UI使用的语言与权限不符， 请重新设置语言或者重新输入密码
unsigned short *CHS_FTA_UILANGUAGE_NOT_MATCH=(unsigned short*)"UI使用的语言与权限不符， 请重新设置语言或者重新输入密码";

//PINCONF写入FPGA错误
unsigned short *CHS_FTA_INTERNAL_WRONGPINCONF=(unsigned short*)"PINCONF写入FPGA错误";


//工厂设置中写入的颜色数错误.
unsigned short *CHS_FTA_FACTORY_COLORNUM_WRONG=(unsigned short*)"工厂设置中写入的颜色数错误";


//头板的EEPROM写入失败
unsigned short *CHS_FTA_HB_EEPROM_WRT_ERR=(unsigned short*)"头板的EEPROM写入失败";


//喷头过热，所有加热被强行关闭。
unsigned short *CHS_FTA_HB_OVERHEAT=(unsigned short*)"喷头过热，所有加热被强行关闭。";

//软件错误或者版本错误，没有通过握手协议。
unsigned short *CHS_FTA_SHAKEHAND_ERR=(unsigned short*)"软件错误或者版本错误，没有通过握手协议";

//固件要求特殊的软件版本，版本错误。
unsigned short *CHS_FTA_SWVERSION_ERR=(unsigned short*)"固件要求特殊的软件版本，版本错误";


//当前系统设置不支持一头两色。
unsigned short *CHS_FTA_NOT_SUPPORT_2C_SHARE_1H=(unsigned short*)"当前系统设置不支持一头两色";

//超过墨水用量限制，请重新输入口令
unsigned short *CHS_FTA_LIMITEDINK_RUNOUT=(unsigned short*)"超过墨水用量限制，请重新输入口令";

//固件要求软件版本支持墨水密码，版本错误，
unsigned short *CHS_FTA_SWVERSION_ERR_INK_PWD=(unsigned short*)"固件要求软件版本支持墨水密码，版本错误";

//读板子ID错误。
unsigned short *CHS_FTA_EPSON_HEAD_BAD=(unsigned short*)"读板子ID错误";


//头板FPGA错误.
unsigned short *CHS_FTA_EPSON_HEAD_FPGA_ERR=(unsigned short*)"头板FPGA错误";


//写主板错误.
unsigned short *CHS_FTA_SECURITY_CHIP_ERR=(unsigned short*)"写主板错误";

//出厂初始化设置出错。
unsigned short *CHS_FTA_CALIBRATION_DATA=(unsigned short*)"出厂初始化设置出错";

//打印数据传输错误或丢失
unsigned short *CHS_FTA_PRINT_DATA_ERROR=(unsigned short*)"USB打印数据传输错误或丢失，请更换USB线或USB端口";
//FACTORY_DATA数据错误，需要重新写入
unsigned short *CHS_FTA_FACTORY_DATA_ERROR=(unsigned short*)"FACTORY_DATA数据错误，需要重新写入";
//废墨溢出
unsigned short *CHS_FTA_WASTE_INK_FULL = (unsigned short *)"废墨溢出";
//EP0命令被打断
unsigned short *CHS_ERR_EP0OVERUN_SETUPDATA=(unsigned short*)"EP0命令被打断";

//头板COM超时.
unsigned short *CHS_ERR_UART1_TXTIMEOUT=(unsigned short*)"头板COM超时";

//运动COM超时.
unsigned short *CHS_ERR_UART2_TXTIMEOUT=(unsigned short*)"运动COM超时";

//打印数据和点火频率不匹配.
unsigned short *CHS_ERR_INTERNAL_PRINTDATA=(unsigned short*)"打印数据和点火频率不匹配";

//打印数据小于点火频率
unsigned short *CHS_ERR_FPGA_LESSDATA=(unsigned short*)"打印数据小于点火频率";

//打印数据超出点火频率.
unsigned short *CHS_ERR_FPGA_ULTRADATA=(unsigned short*)"打印数据超出点火频率";

//打印的数据和开火数不匹配.
unsigned short *CHS_ERR_FPGA_WRONGCHS=(unsigned short*)"打印的数据和开火数不匹配";

//内部状态.
unsigned short *CHS_ERR_UV_CTR_WRONGCHS=(unsigned short*)"内部状态";

//FPGA 收到错误的命令
unsigned short *CHS_ERR_FPGA_WRONGCOMMAND =(unsigned short*)"FPGA 收到错误的命令";


//运动速度太快或不均匀，数据还在出书，却有点火命令
unsigned short *CHS_ERR_MOTION_SPEED_BAD =(unsigned short*)"运动速度太快或不均匀，数据还在出书，却有点火命令";

//运动命令发出30秒后，没有收到运动完成命令
unsigned short *CHS_ERR_INTERNAL_MOVING_TIMEOUT =(unsigned short*)"运动命令发出30秒后，没有收到运动完成命令";



//错误的AUTOCLEAN命令.
unsigned short *CHS_ERR_INTERNAL_WRONGAUTOCLEAN=(unsigned short*)"错误的AUTOCLEAN命令";

//错误的AUTOCLEAN命令.
unsigned short *CHS_ERR_INTERNAL_WRONGBANDINFO=(unsigned short*)"错误的AUTOCLEAN命令";


//调试状态1
unsigned short *CHS_ERR_INTERNAL_DEBUG1=(unsigned short*)"调试状态1";

//调试状态2
unsigned short *CHS_ERR_INTERNAL_DEBUG2=(unsigned short*)"调试状态2";


//正在初始化ARM.
unsigned short *CHS_INI_ARM=(unsigned short*)"正在初始化ARM";


//正在初始化USB
unsigned short *CHS_INI_SX2=(unsigned short*)"正在初始化USB";


//正在初始化FPGA
unsigned short *CHS_INI_FPGA=(unsigned short*)"正在初始化FPGA";



//正在初始化DSP.
unsigned short *CHS_INI_DSP=(unsigned short*)"正在初始化DSP";

//正在初始化头板
unsigned short *CHS_INI_HEADBOARD=(unsigned short*)"正在初始化头板";


//安装 Spectra 高压板.
unsigned short *CHS_INI_HVB=(unsigned short*)"安装 Spectra 高压板";

//不知道的警告
unsigned short *CHS_WAR_UNKNOWNHEADERTYPE=(unsigned short*)"不知道的警告";


//EP0溢出.
unsigned short *CHS_WAR_EP0OVERUN_REQUEST_IGNORE=(unsigned short*)"EP0溢出";


//兰色正在泵墨.
unsigned short *CHS_WAR_PUMP_CYAN=(unsigned short*)"兰色正在泵墨";

//品色正在泵墨.
unsigned short *CHS_WAR_PUMP_MAGENTA=(unsigned short*)"品色正在泵墨";


//黄色正在泵墨.
unsigned short *CHS_WAR_PUMP_YELLOW=(unsigned short*)"黄色正在泵墨";



//黑色正在泵墨.
unsigned short *CHS_WAR_PUMP_BLACK=(unsigned short*)"黑色正在泵墨";

//浅兰色正在泵墨.
unsigned short *CHS_WAR_PUMP_LIGHTCYAN=(unsigned short*)"浅兰色正在泵墨";
//浅品色正在泵墨.
unsigned short *CHS_WAR_PUMP_LIGHTMAGENTA=(unsigned short*)"浅品色正在泵墨";

//输入限时密码错误，含有含有非法字符;
unsigned short *CHS_WAR_TIME_PASSWORD_WRONGINPUT=(unsigned short*)"输入限时密码错误，含有含有非法字符";


//不是一个合法限时密码;
unsigned short *CHS_WAR_TIME_PASSWORD_WRONG=(unsigned short*)"不是一个合法限时密码";


//限时密码和厂商不匹配
unsigned short *CHS_WAR_TIME_PASSWORD_MANUFACTURERIDNOTMATCH=(unsigned short*)"限时密码和板子不匹配";

//限时密码和板子不匹配
unsigned short *CHS_WAR_TIME_PASSWORD_BOARDIDNOTMATCH=(unsigned short*)"限时密码和板子不匹配";

//第一次警告:再有100个小时就超出限时了, 请向厂家索取密码.
unsigned short *CHS_WAR_LIMITEDTIME_FIRST=(unsigned short*)"第一次警告:再有100个小时就超出限时了, 请向厂家索取密码";


//第二次警告: 再有50个小时就超出限时了,请向厂家索取密码.
unsigned short *CHS_WAR_LIMITEDTIME_SECOND=(unsigned short*)"第二次警告: 再有50个小时就超出限时了,请向厂家索取密码.";

//最后一次警告: 再有1个小时就超出限时并停止打印, 请立即向厂家索取密码.
unsigned short *CHS_WAR_LIMITEDTIME_LAST=(unsigned short*)"最后一次警告: 再有1个小时就超出限时并停止打印, 请立即向厂家索取密码";

//输入选配密码错误，含有非0~F的值
unsigned short *CHS_WAR_OPTION_PASSWORD_WRONGINPUT=(unsigned short*)"输入选配密码错误，含有非0~F的值";

//不是一个合法选配密码
unsigned short *CHS_WAR_OPTION_PASSWORD_WRONG=(unsigned short*)"不是一个合法选配密码";

//选配密码和厂商不匹配
unsigned short *CHS_WAR_OPTION_PASSWORD_MANUFACTURERIDNOTMATCH=(unsigned short*)"选配密码和厂商不匹配";

//选配密码和板子不匹配
unsigned short *CHS_WAR_OPTION_PASSWORD_BOARDIDNOTMATCH=(unsigned short*)"选配密码和板子不匹配";

//泵墨安全瓶满了, 所有墨泵停止供墨.
unsigned short *CHS_WAR_PUMP_PROHIBIT_SENSOR=(unsigned short*)"泵墨安全瓶满了, 所有墨泵停止供墨";


//试图在不支持超频的头板版本上使用超频, 系统强制使用高速打印.
unsigned short *CHS_WAR_OVER_SPEED_NOT_SUPPORT=(unsigned short*)"试图在不支持超频的头板版本上使用超频, 系统强制使用高速打印";

//输入墨水密码错误，含有非0~F的值
unsigned short *CHS_WAR_INK_PASSWORD_WRONGINPUT=(unsigned short*)"输入墨水密码错误，含有非0~F的值";


//不是一个合法墨水密码
unsigned short *CHS_WAR_INK_PASSWORD_WRONG=(unsigned short*)"不是一个合法墨水密码";

//墨水密码和厂商不匹配
unsigned short *CHS_WAR_INK_PASSWORD_MANUFACTURERIDNOTMATCH=(unsigned short*)"墨水密码和厂商不匹配";

//墨水密码和板子不匹配
unsigned short *CHS_WAR_INK_PASSWORD_BOARDIDNOTMATCH=(unsigned short*)"墨水密码和板子不匹配";

//再用10升就超出墨水限制了，第一次警告
unsigned short *CHS_WAR_LIMITEDINK_FIRST=(unsigned short*)"再用10升就超出墨水限制了，第一次警告";

//再用5升就超出墨水限制了，第二次警告
unsigned short *CHS_WAR_LIMITEDINK_SECOND=(unsigned short*)"再用5升就超出墨水限制了，第二次警告";

//再用1升就超出墨水限制了，最后一次警告
unsigned short *CHS_WAR_LIMITEDINK_LAST=(unsigned short*)"再用1升就超出墨水限制了，最后一次警告";


//当前喷头不支持墨水密码功能.
unsigned short *CHS_WAR_INK_PASSWORD_UNKNOWN_DOT_VOLUME=(unsigned short*)"当前喷头不支持墨水密码功能";


//专色1正在泵墨.
unsigned short *CHS_WAR_PUMP_SPOTCOLOR1=(unsigned short*)"专色1正在泵墨";


//专色2正在泵墨.
unsigned short *CHS_WAR_PUMP_SPOTCOLOR2 =(unsigned short*)"专色2正在泵墨";


//打印数据小于点火数.
unsigned short *CHS_WAR_FPGA_LESSDATA=(unsigned short*)"打印数据小于点火数";

//打印数据多于点火数.
unsigned short *CHS_WAR_FPGA_ULTRADATA=(unsigned short*)"打印数据多于点火数";

//喷头温度太低.
unsigned short *CHS_WAR_EPSONHEAD_TEMP_TOO_LOW=(unsigned short*)"喷头温度太低";


//喷头温度太高.
unsigned short *CHS_WAR_EPSONHEAD_TEMP_TOO_HIGH=(unsigned short*)"喷头温度太高";


//喷头未连接.
unsigned short *CHS_WAR_EPSONHEAD_NOT_CONNECT=(unsigned short*)"喷头未连接";


//主板FPGA报告的数据太多.
unsigned short *CHS_WAR_EPSON_FPGA_ULTRADATA=(unsigned short*)"主板FPGA报告的数据太多";

//主板FPGA报告的数据太少.
unsigned short *CHS_WAR_EPSON_FPGA_LESSDATA=(unsigned short*)"主板FPGA报告的数据太少";

//头板FPGA错误.
unsigned short *CHS_WAR_EPSON_FPGA_ERR=(unsigned short*)"头板FPGA错误";

//主板LVDS失锁
unsigned short *CHS_WAR_EPSON_LVDS_ERR=(unsigned short*)"主板LVDS失锁";

//头板LVDS失锁
unsigned short *CHS_WAR_EPSON_REPORT_LVDS_ERR=(unsigned short*)"头板LVDS失锁";
//当前喷头的喷嘴数未知。
unsigned short *CHS_WAR_UNKNOWN_NOZZLE_COUNT=(unsigned short*)"当前喷头的喷嘴数未知";

//升级成功.
unsigned short *CHS_UPDATE_SUCCESS=(unsigned short*)"升级成功";


//DSP开始升级命令超时.
unsigned short *CHS_UPDATEERR_DSP_BEGIN_TIMEOUT=(unsigned short*)"DSP开始升级命令超时";


//DSP数据超时.
unsigned short *CHS_UPDATEERR_DSP_DATA_TIMEOUT=(unsigned short*)"DSP数据超时";

//DSP结束超时.
unsigned short *CHS_UPDATEERR_DSP_END_TIMEOUT=(unsigned short*)"DSP结束超时";


//错误的升级文件格式.
unsigned short *CHS_UPDATEERR_ILIGALFILE=(unsigned short*)"错误的升级文件格式";

//不合法的升级文件.
unsigned short *CHS_UPDATEERR_INTERNAL_DATA=(unsigned short*)"不合法的升级文件";


//升级校验码错误.
unsigned short *CHS_UPDATEERR_CHECKSUM=(unsigned short*)"升级校验码错误";


//ARM 闪存擦除.
unsigned short *CHS_UPDATEERR_EREASE=(unsigned short*)"ARM 闪存擦除";


//Flash错误.
unsigned short *CHS_UPDATEERR_FLASHOP=(unsigned short*)"Flash错误";


//需要特别固件。
unsigned short *CHS_UPDATEERR_GZ_ILIGALFILE=(unsigned short*)"需要特别固件";


//需要特殊墨水密码固件。
unsigned short *CHS_UPDATEERR_INKP_ILIGALFILE=(unsigned short*)"需要特殊墨水密码固件";


//8头硬件无法升级16头的固件。
unsigned short *CHS_UPDATEERR_16H_ILIGALFILE=(unsigned short*)"8头硬件无法升级16头的固件";

//未知的错误
unsigned short *CHS_UNKNOWN_ERR=(unsigned short*)"未知的错误";

unsigned short *CHS_STATUS_SVC = (unsigned short*)"SVC错误:";
unsigned short *CHS_STATUS_FTA = (unsigned short*)"FTA错误:";
unsigned short *CHS_STATUS_ERR = (unsigned short*)"ERR错误:";
unsigned short *CHS_STATUS_ERR_AUTO= (unsigned short*)"ERR_AUTO错误:";
unsigned short *CHS_STATUS_WAR = (unsigned short*)"错误:";
unsigned short *CHS_STATUS_INI = (unsigned short*)"初始化:";
unsigned short *CHS_STATUS_RDY =  (unsigned short*)"就绪";
#ifdef FUNC_MAINTAIN
unsigned short *CHS_STATUS_MAINTAIN = (unsigned short *)"维护";
#endif
unsigned short *CHS_STATUS_MOVING = (unsigned short*)"移动";
unsigned short *CHS_STATUS_PRINT = (unsigned short*)"打印";
unsigned short *CHS_STATUS_PAUSE = (unsigned short*)"暂停";
unsigned short *CHS_STATUS_PAUSE_1 = (unsigned short*)"暂停(压纸杆抬起)";
unsigned short *CHS_STATUS_PAUSE_2 = (unsigned short*)"暂停(前面板抬起)";
unsigned short *CHS_STATUS_PAUSE_3 = (unsigned short*)"暂停(缺纸)";
unsigned short *CHS_STATUS_CLEANING =  (unsigned short*)"清洗";
unsigned short *CHS_STATUS_CANCELING = (unsigned short*)"退出";
unsigned short *CHS_STATUS_UPDATING = (unsigned short*)"下载";
#else
unsigned short ErrorCodeCHS[]={'0','X','0','0','0','0','0','0','0','0',0 };

//在USB chip中服务错误.
unsigned short CHS_SVC_SX2[] = {
	CHAR_TABLE_OFFSET + 143,85,83,66,32,
	99,104,105,112,CHAR_TABLE_OFFSET + 93,
	CHAR_TABLE_OFFSET + 144,CHAR_TABLE_OFFSET + 145,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,46, 0 };


//Service Call: chip1.
unsigned short CHS_SVC_FPGA0[] = {
	83,101,114,118,105,
	99,101,32,67,97,
	108,108,58,32,99,
	104,105,112,49,46, 0 };


//Service Call: chip2.
unsigned short CHS_SVC_FPGA1[] = {
	83,101,114,118,105,
	99,101,32,67,97,
	108,108,58,32,99,
	104,105,112,50,46, 0 };


//Service Call: chip3.
unsigned short CHS_SVC_FPGA2[] = {
	83,101,114,118,105,
	99,101,32,67,97,
	108,108,58,32,99,
	104,105,112,51,46, 0 };


//Service Call: chip4
unsigned short CHS_SVC_FPGA3[] = {
	83,101,114,118,105,
	99,101,32,67,97,
	108,108,58,32,99,
	104,105,112,52, 0 };


//Service Call: chip5.
unsigned short CHS_SVC_UPDATE_FAIL[] = {
	83,101,114,118,105,
	99,101,32,67,97,
	108,108,58,32,99,
	104,105,112,53,46, 0 };


//Service Call: 6.
unsigned short CHS_SVC_BYHX_DATA[] = {
	83,101,114,118,105,
	99,101,32,67,97,
	108,108,58,32,54,
	46, 0 };


//BYHX_TOOL check EEPROM 没有通过。
unsigned short CHS_SVC_EEPROM_CHK[] = {
	66,89,72,88,95,
	84,79,79,76,32,
	99,104,101,99,107,
	32,69,69,80,82,
	79,77,32,CHAR_TABLE_OFFSET + 146,CHAR_TABLE_OFFSET + 147,
	CHAR_TABLE_OFFSET + 148,CHAR_TABLE_OFFSET + 149,CHAR_TABLE_OFFSET + 150, 0 };


//SDRAM 的Check没有通过
unsigned short CHS_SVC_SDRAM_CHECK[] = {
	83,68,82,65,77,
	32,CHAR_TABLE_OFFSET + 151,67,104,101,
	99,107,CHAR_TABLE_OFFSET + 146,CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 148,
	CHAR_TABLE_OFFSET + 149, 0 };


//无法从Falsh读取FPGA.
unsigned short CHS_SVC_FLASH[] = {
	CHAR_TABLE_OFFSET + 119,CHAR_TABLE_OFFSET + 152,CHAR_TABLE_OFFSET + 153,70,97,
	108,115,104,CHAR_TABLE_OFFSET + 154,CHAR_TABLE_OFFSET + 155,
	70,80,71,65,46,
	32, 0 };


//SDRAM 太小，不能支持当前的设置。
unsigned short CHS_SVC_SDRAM_NOMATCH[] = {
	83,68,82,65,77,
	32,CHAR_TABLE_OFFSET + 156,CHAR_TABLE_OFFSET + 157,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 6,
	CHAR_TABLE_OFFSET + 31,CHAR_TABLE_OFFSET + 159,CHAR_TABLE_OFFSET + 160,CHAR_TABLE_OFFSET + 161,CHAR_TABLE_OFFSET + 34,
	CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 19,CHAR_TABLE_OFFSET + 20,CHAR_TABLE_OFFSET + 150, 0 };


//读主板错误。
unsigned short CHS_SVC_READBOARDID_FAIL[] = {
	CHAR_TABLE_OFFSET + 154,CHAR_TABLE_OFFSET + 162,CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,
	CHAR_TABLE_OFFSET + 150, 0 };


//USB chip异常重启
unsigned short CHS_FTA_SX2RESET[] = {
	85,83,66,32,99,
	104,105,112,CHAR_TABLE_OFFSET + 163,CHAR_TABLE_OFFSET + 164,
	CHAR_TABLE_OFFSET + 165,CHAR_TABLE_OFFSET + 166, 0 };


//内部错误 1.
unsigned short CHS_FTA_INTERNAL_WRONGHEADER[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,32,
	49,46, 0 };


//内部错误 2.
unsigned short CHS_FTA_INTERNAL_WRONGHEADERSIZE[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,32,
	50,46, 0 };


//内部错误 3.
unsigned short CHS_FTA_INTERNAL_JOBSTARTHEADER[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,32,
	51,46, 0 };


//内部错误 4.
unsigned short CHS_FTA_INTERNAL_BANDDATASIZE[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,32,
	52,46, 0 };


//内部错误 5.
unsigned short CHS_FTA_INTERNAL_WRONGFORMAT[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,32,
	53,46, 0 };


//内部错误 6.
unsigned short CHS_FTA_INTERNAL_DMA0WORKING[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,32,
	54,46, 0 };


//内部错误 7.
unsigned short CHS_FTA_INTERNAL_PRINTPOINT[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,32,
	55,46, 0 };


//内部错误 8.
unsigned short CHS_FTA_INTERNAL_OLIMIT[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,32,
	56,46, 0 };


//内部错误 9.
unsigned short CHS_FTA_INTERNAL_OPPLIMIT[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,32,
	57,46, 0 };


//运动step 1.
unsigned short CHS_FTA_DSPINITS1[] = {
	CHAR_TABLE_OFFSET + 168,CHAR_TABLE_OFFSET + 18,115,116,101,
	112,32,49,46, 0 };


//运动step 2.
unsigned short CHS_FTA_DSPINITS2[] = {
	CHAR_TABLE_OFFSET + 168,CHAR_TABLE_OFFSET + 18,115,116,101,
	112,32,50,46, 0 };


//头板step1.
unsigned short CHS_FTA_HEADINITS1[] = {
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 36,115,116,101,
	112,49,46, 0 };


//头板step2.
unsigned short CHS_FTA_HEADINITS2[] = {
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 36,115,116,101,
	112,50,46, 0 };


//LVDS错误.
unsigned short CHS_FTA_HEADTOMAINROAD[] = {
	76,86,68,83,CHAR_TABLE_OFFSET + 69,
	CHAR_TABLE_OFFSET + 138,46, 0 };


//Band定义中的方向值超出定义
unsigned short CHS_FTA_INTERNAL_BANDDIRECTION[] = {
	66,97,110,100,CHAR_TABLE_OFFSET + 90,
	CHAR_TABLE_OFFSET + 91,CHAR_TABLE_OFFSET + 93,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 99,CHAR_TABLE_OFFSET + 67,
	CHAR_TABLE_OFFSET + 64,CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 90,CHAR_TABLE_OFFSET + 91, 0 };


//DSP升级失败.
unsigned short CHS_FTA_DSPUPDATE_FAIL[] = {
	68,83,80,CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 132,
	CHAR_TABLE_OFFSET + 172,CHAR_TABLE_OFFSET + 173,46, 0 };


//EEPROM读取失败	
unsigned short CHS_FTA_EEPROM_READ[] = {
	69,69,80,82,79,
	77,CHAR_TABLE_OFFSET + 154,CHAR_TABLE_OFFSET + 155,CHAR_TABLE_OFFSET + 172,CHAR_TABLE_OFFSET + 173,
	9, 0 };


//EEPROM写入失败
unsigned short CHS_FTA_EEPROM_WRITE[] = {
	69,69,80,82,79,
	77,CHAR_TABLE_OFFSET + 174,CHAR_TABLE_OFFSET + 23,CHAR_TABLE_OFFSET + 172,CHAR_TABLE_OFFSET + 173, 0 };


//板子出厂没有初始化.
unsigned short CHS_FTA_FACTORY_DATA[] = {
	CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 175,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 146,
	CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 177,CHAR_TABLE_OFFSET + 178,CHAR_TABLE_OFFSET + 55,46, 0 };


//头板复位.
unsigned short CHS_FTA_HEADBOARD_RESET[] = {
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 179,CHAR_TABLE_OFFSET + 180,46, 0 };


//Spectra高压板无法在第1步初始化.
unsigned short CHS_FTA_SPECTRAHVBINITS1[] = {
	83,112,101,99,116,
	114,97,CHAR_TABLE_OFFSET + 7,CHAR_TABLE_OFFSET + 181,CHAR_TABLE_OFFSET + 36,
	CHAR_TABLE_OFFSET + 119,CHAR_TABLE_OFFSET + 152,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 182,49,
	CHAR_TABLE_OFFSET + 61,CHAR_TABLE_OFFSET + 177,CHAR_TABLE_OFFSET + 178,CHAR_TABLE_OFFSET + 55,46, 0 };


//头板与出厂数据不匹配,请重置出厂数据或者更换头板.
unsigned short CHS_FTA_PRINTHEAD_NOTMATCH[] = {
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 183,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 176,
	CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186,
	44,CHAR_TABLE_OFFSET + 187,CHAR_TABLE_OFFSET + 165,CHAR_TABLE_OFFSET + 20,CHAR_TABLE_OFFSET + 170,
	CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 188,CHAR_TABLE_OFFSET + 189,
	CHAR_TABLE_OFFSET + 190,CHAR_TABLE_OFFSET + 191,CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 36,46, 0 };


//板子的厂商ID不匹配,请升级固件.
unsigned short CHS_FTA_MANUFACTURERID_NOTMATCH[] = {
	CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 175,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 192,
	73,68,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186,
	44,CHAR_TABLE_OFFSET + 187,CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 132,CHAR_TABLE_OFFSET + 193,
	CHAR_TABLE_OFFSET + 194,46,32, 0 };


//严重错误: 超过限时时间，请输入新的密码.
unsigned short CHS_FTA_LIMITEDTIME_RUNOUT[] = {
	CHAR_TABLE_OFFSET + 195,CHAR_TABLE_OFFSET + 165,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,58,
	32,CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 149,CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 103,
	CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 74,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 187,CHAR_TABLE_OFFSET + 22,
	CHAR_TABLE_OFFSET + 23,CHAR_TABLE_OFFSET + 197,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,
	46, 0 };


//内部错误,空白Band或Y向距离为零.
unsigned short CHS_FTA_INTERNAL_SW1[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,44,
	CHAR_TABLE_OFFSET + 115,CHAR_TABLE_OFFSET + 29,66,97,110,
	100,CHAR_TABLE_OFFSET + 188,89,CHAR_TABLE_OFFSET + 67,CHAR_TABLE_OFFSET + 75,
	CHAR_TABLE_OFFSET + 200,CHAR_TABLE_OFFSET + 201,CHAR_TABLE_OFFSET + 202,46, 0 };


//USB1.1连接.
unsigned short CHS_FTA_USB1_USB1CONNECT[] = {
	85,83,66,49,46,
	49,CHAR_TABLE_OFFSET + 203,CHAR_TABLE_OFFSET + 204,46, 0 };


//UI使用的语言与权限不符， 请重新设置语言或者重新输入密码
unsigned short CHS_FTA_UILANGUAGE_NOT_MATCH[] = {
	85,73,CHAR_TABLE_OFFSET + 205,CHAR_TABLE_OFFSET + 206,CHAR_TABLE_OFFSET + 151,
	CHAR_TABLE_OFFSET + 126,CHAR_TABLE_OFFSET + 127,CHAR_TABLE_OFFSET + 183,CHAR_TABLE_OFFSET + 207,CHAR_TABLE_OFFSET + 196,
	CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 208,CHAR_TABLE_OFFSET + 158,32,CHAR_TABLE_OFFSET + 187,
	CHAR_TABLE_OFFSET + 165,CHAR_TABLE_OFFSET + 197,CHAR_TABLE_OFFSET + 19,CHAR_TABLE_OFFSET + 20,CHAR_TABLE_OFFSET + 126,
	CHAR_TABLE_OFFSET + 127,CHAR_TABLE_OFFSET + 188,CHAR_TABLE_OFFSET + 189,CHAR_TABLE_OFFSET + 165,CHAR_TABLE_OFFSET + 197,
	CHAR_TABLE_OFFSET + 22,CHAR_TABLE_OFFSET + 23,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199, 0 };


//PINCONF写入FPGA错误
unsigned short CHS_FTA_INTERNAL_WRONGPINCONF[] = {
	80,73,78,67,79,
	78,70,CHAR_TABLE_OFFSET + 174,CHAR_TABLE_OFFSET + 23,70,
	80,71,65,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138, 0 };


//工厂设置中写入的颜色数错误.
unsigned short CHS_FTA_FACTORY_COLORNUM_WRONG[] = {
	CHAR_TABLE_OFFSET + 209,CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 19,CHAR_TABLE_OFFSET + 20,CHAR_TABLE_OFFSET + 93,
	CHAR_TABLE_OFFSET + 174,CHAR_TABLE_OFFSET + 23,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 210,CHAR_TABLE_OFFSET + 211,
	CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,46, 0 };


//头板的EEPROM写入失败
unsigned short CHS_FTA_HB_EEPROM_WRT_ERR[] = {
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 151,69,69,
	80,82,79,77,CHAR_TABLE_OFFSET + 174,
	CHAR_TABLE_OFFSET + 23,CHAR_TABLE_OFFSET + 172,CHAR_TABLE_OFFSET + 173, 0 };


//喷头过热，所有加热被强行关闭。
unsigned short CHS_FTA_HB_OVERHEAT[] = {
	CHAR_TABLE_OFFSET + 58,CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 149,CHAR_TABLE_OFFSET + 46,CHAR_TABLE_OFFSET + 158,
	CHAR_TABLE_OFFSET + 212,CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 45,CHAR_TABLE_OFFSET + 46,CHAR_TABLE_OFFSET + 213,
	CHAR_TABLE_OFFSET + 92,CHAR_TABLE_OFFSET + 214,CHAR_TABLE_OFFSET + 48,CHAR_TABLE_OFFSET + 215,CHAR_TABLE_OFFSET + 150, 0 };


//软件错误或者版本错误，没有通过握手协议。
unsigned short CHS_FTA_SHAKEHAND_ERR[] = {
	CHAR_TABLE_OFFSET + 216,CHAR_TABLE_OFFSET + 194,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 188,
	CHAR_TABLE_OFFSET + 189,CHAR_TABLE_OFFSET + 217,CHAR_TABLE_OFFSET + 218,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,
	CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 146,CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 148,CHAR_TABLE_OFFSET + 149,
	CHAR_TABLE_OFFSET + 219,CHAR_TABLE_OFFSET + 21,CHAR_TABLE_OFFSET + 220,CHAR_TABLE_OFFSET + 221,CHAR_TABLE_OFFSET + 150, 0 };


//固件要求特殊的软件版本，版本错误。
unsigned short CHS_FTA_SWVERSION_ERR[] = {
	CHAR_TABLE_OFFSET + 193,CHAR_TABLE_OFFSET + 194,CHAR_TABLE_OFFSET + 222,CHAR_TABLE_OFFSET + 223,CHAR_TABLE_OFFSET + 224,
	CHAR_TABLE_OFFSET + 225,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 216,CHAR_TABLE_OFFSET + 194,CHAR_TABLE_OFFSET + 217,
	CHAR_TABLE_OFFSET + 218,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 217,CHAR_TABLE_OFFSET + 218,CHAR_TABLE_OFFSET + 69,
	CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 150, 0 };


//当前系统设置不支持一头两色。
unsigned short CHS_FTA_NOT_SUPPORT_2C_SHARE_1H[] = {
	CHAR_TABLE_OFFSET + 161,CHAR_TABLE_OFFSET + 34,CHAR_TABLE_OFFSET + 130,CHAR_TABLE_OFFSET + 131,CHAR_TABLE_OFFSET + 19,
	CHAR_TABLE_OFFSET + 20,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 159,CHAR_TABLE_OFFSET + 160,CHAR_TABLE_OFFSET + 118,
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 226,CHAR_TABLE_OFFSET + 211,CHAR_TABLE_OFFSET + 150, 0 };


//超过墨水用量限制，请重新输入口令
unsigned short CHS_FTA_LIMITEDINK_RUNOUT[] = {
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 149,CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 206,
	CHAR_TABLE_OFFSET + 26,CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 227,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 187,
	CHAR_TABLE_OFFSET + 165,CHAR_TABLE_OFFSET + 197,CHAR_TABLE_OFFSET + 22,CHAR_TABLE_OFFSET + 23,CHAR_TABLE_OFFSET + 228,
	CHAR_TABLE_OFFSET + 229, 0 };


//固件要求软件版本支持墨水密码，版本错误，
unsigned short CHS_FTA_SWVERSION_ERR_INK_PWD[] = {
	CHAR_TABLE_OFFSET + 193,CHAR_TABLE_OFFSET + 194,CHAR_TABLE_OFFSET + 222,CHAR_TABLE_OFFSET + 223,CHAR_TABLE_OFFSET + 216,
	CHAR_TABLE_OFFSET + 194,CHAR_TABLE_OFFSET + 217,CHAR_TABLE_OFFSET + 218,CHAR_TABLE_OFFSET + 159,CHAR_TABLE_OFFSET + 160,
	CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 158,
	CHAR_TABLE_OFFSET + 217,CHAR_TABLE_OFFSET + 218,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 158, 0 };


//读板子ID错误。
unsigned short CHS_FTA_EPSON_HEAD_BAD[] = {
	CHAR_TABLE_OFFSET + 154,CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 175,73,68,
	CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 150, 0 };


//头版FPGA错误.
unsigned short CHS_FTA_EPSON_HEAD_FPGA_ERR[] = {
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 217,70,80,71,
	65,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,46, 0 };


//写主板错误.
unsigned short CHS_FTA_SECURITY_CHIP_ERR[] = {
	CHAR_TABLE_OFFSET + 174,CHAR_TABLE_OFFSET + 162,CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,
	46, 0 };


//出厂初始化设置出错。
unsigned short CHS_FTA_CALIBRATION_DATA[] = {
	CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 177,CHAR_TABLE_OFFSET + 178,CHAR_TABLE_OFFSET + 55,
	CHAR_TABLE_OFFSET + 19,CHAR_TABLE_OFFSET + 20,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 150, 0 };


//EP0命令被打断
unsigned short CHS_ERR_EP0OVERUN_SETUPDATA[] = {
	69,80,48,CHAR_TABLE_OFFSET + 230,CHAR_TABLE_OFFSET + 229,
	CHAR_TABLE_OFFSET + 213,CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 231, 0 };


//头板COM超时.
unsigned short CHS_ERR_UART1_TXTIMEOUT[] = {
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 36,67,79,77,
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 103,46, 0 };


//运动COM超时.
unsigned short CHS_ERR_UART2_TXTIMEOUT[] = {
	CHAR_TABLE_OFFSET + 168,CHAR_TABLE_OFFSET + 18,67,79,77,
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 103,46, 0 };


//打印数据和点火频率不匹配.
unsigned short CHS_ERR_INTERNAL_PRINTDATA[] = {
	CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 52,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 89,
	CHAR_TABLE_OFFSET + 15,CHAR_TABLE_OFFSET + 232,CHAR_TABLE_OFFSET + 110,CHAR_TABLE_OFFSET + 111,CHAR_TABLE_OFFSET + 6,
	CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186,46, 0 };


//打印数据小于点火频率
unsigned short CHS_ERR_FPGA_LESSDATA[] = {
	CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 52,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 157,
	CHAR_TABLE_OFFSET + 233,CHAR_TABLE_OFFSET + 15,CHAR_TABLE_OFFSET + 232,CHAR_TABLE_OFFSET + 110,CHAR_TABLE_OFFSET + 111, 0 };


//打印数据超出点火频率.
unsigned short CHS_ERR_FPGA_ULTRADATA[] = {
	CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 52,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 169,
	CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 15,CHAR_TABLE_OFFSET + 232,CHAR_TABLE_OFFSET + 110,CHAR_TABLE_OFFSET + 111,
	46, 0 };


//打印的数据和开火数不匹配.
unsigned short CHS_ERR_FPGA_WRONGCHS[] = {
	CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 52,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,
	CHAR_TABLE_OFFSET + 89,CHAR_TABLE_OFFSET + 47,CHAR_TABLE_OFFSET + 232,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 6,
	CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186,46, 0 };


//内部状态.
unsigned short CHS_ERR_UV_CTR_WRONGCHS[] = {
	CHAR_TABLE_OFFSET + 114,CHAR_TABLE_OFFSET + 167,CHAR_TABLE_OFFSET + 133,CHAR_TABLE_OFFSET + 134,46, 0 };


//FPGA 收到错误的命令
unsigned short CHS_ERR_FPGA_WRONGCOMMAND[] = {
	70,80,71,65,32,
	CHAR_TABLE_OFFSET + 234,CHAR_TABLE_OFFSET + 235,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 151,
	CHAR_TABLE_OFFSET + 230,CHAR_TABLE_OFFSET + 229, 0 };


//运动速度太快或不均匀，数据还在出书，却有点火命令
unsigned short CHS_ERR_MOTION_SPEED_BAD[] = {
	CHAR_TABLE_OFFSET + 168,CHAR_TABLE_OFFSET + 18,CHAR_TABLE_OFFSET + 108,CHAR_TABLE_OFFSET + 50,CHAR_TABLE_OFFSET + 156,
	CHAR_TABLE_OFFSET + 97,CHAR_TABLE_OFFSET + 188,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 236,CHAR_TABLE_OFFSET + 237,
	CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 238,CHAR_TABLE_OFFSET + 143,
	CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 239,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 240,CHAR_TABLE_OFFSET + 147,
	CHAR_TABLE_OFFSET + 15,CHAR_TABLE_OFFSET + 232,CHAR_TABLE_OFFSET + 230,CHAR_TABLE_OFFSET + 229, 0 };


//运动命令发出30秒后，没有收到运动完成命令
unsigned short CHS_ERR_INTERNAL_MOVING_TIMEOUT[] = {
	CHAR_TABLE_OFFSET + 168,CHAR_TABLE_OFFSET + 18,CHAR_TABLE_OFFSET + 230,CHAR_TABLE_OFFSET + 229,CHAR_TABLE_OFFSET + 241,
	CHAR_TABLE_OFFSET + 170,51,48,CHAR_TABLE_OFFSET + 242,CHAR_TABLE_OFFSET + 243,
	CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 146,CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 234,CHAR_TABLE_OFFSET + 235,
	CHAR_TABLE_OFFSET + 168,CHAR_TABLE_OFFSET + 18,CHAR_TABLE_OFFSET + 244,CHAR_TABLE_OFFSET + 245,CHAR_TABLE_OFFSET + 230,
	CHAR_TABLE_OFFSET + 229, 0 };


//错误的AUTOCLEAN命令.
unsigned short CHS_ERR_INTERNAL_WRONGAUTOCLEAN[] = {
	CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 151,65,85,
	84,79,67,76,69,
	65,78,CHAR_TABLE_OFFSET + 230,CHAR_TABLE_OFFSET + 229,46, 0 };


//错误的AUTOCLEAN命令.
unsigned short CHS_ERR_INTERNAL_WRONGBANDINFO[] = {
	CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 151,65,85,
	84,79,67,76,69,
	65,78,CHAR_TABLE_OFFSET + 230,CHAR_TABLE_OFFSET + 229,46, 0 };


//调试状态1
unsigned short CHS_ERR_INTERNAL_DEBUG1[] = {
	CHAR_TABLE_OFFSET + 246,CHAR_TABLE_OFFSET + 68,CHAR_TABLE_OFFSET + 133,CHAR_TABLE_OFFSET + 134,49, 0 };


//调试状态2
unsigned short CHS_ERR_INTERNAL_DEBUG2[] = {
	CHAR_TABLE_OFFSET + 246,CHAR_TABLE_OFFSET + 68,CHAR_TABLE_OFFSET + 133,CHAR_TABLE_OFFSET + 134,50, 0 };


//正在初始化ARM.
unsigned short CHS_INI_ARM[] = {
	CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 177,CHAR_TABLE_OFFSET + 178,CHAR_TABLE_OFFSET + 55,
	65,82,77,46, 0 };


//正在初始化USB
unsigned short CHS_INI_SX2[] = {
	CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 177,CHAR_TABLE_OFFSET + 178,CHAR_TABLE_OFFSET + 55,
	85,83,66, 0 };


//正在初始化FPGA
unsigned short CHS_INI_FPGA[] = {
	CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 177,CHAR_TABLE_OFFSET + 178,CHAR_TABLE_OFFSET + 55,
	70,80,71,65, 0 };


//正在初始化DSP.
unsigned short CHS_INI_DSP[] = {
	CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 177,CHAR_TABLE_OFFSET + 178,CHAR_TABLE_OFFSET + 55,
	68,83,80,46, 0 };


//正在初始化头板
unsigned short CHS_INI_HEADBOARD[] = {
	CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 177,CHAR_TABLE_OFFSET + 178,CHAR_TABLE_OFFSET + 55,
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 36, 0 };


//安装 Spectra 高压板.
unsigned short CHS_INI_HVB[] = {
	CHAR_TABLE_OFFSET + 37,CHAR_TABLE_OFFSET + 247,32,83,112,
	101,99,116,114,97,
	32,CHAR_TABLE_OFFSET + 7,CHAR_TABLE_OFFSET + 181,CHAR_TABLE_OFFSET + 36,46, 0 };


//不知道的警告
unsigned short CHS_WAR_UNKNOWNHEADERTYPE[] = {
	CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 248,CHAR_TABLE_OFFSET + 249,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 250,
	CHAR_TABLE_OFFSET + 251, 0 };


//EP0溢出.
unsigned short CHS_WAR_EP0OVERUN_REQUEST_IGNORE[] = {
	69,80,48,CHAR_TABLE_OFFSET + 252,CHAR_TABLE_OFFSET + 170,
	46, 0 };


//兰色正在泵墨.
unsigned short CHS_WAR_PUMP_CYAN[] = {
	CHAR_TABLE_OFFSET + 253,CHAR_TABLE_OFFSET + 211,CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 254,
	CHAR_TABLE_OFFSET + 102,46, 0 };


//品色正在泵墨.
unsigned short CHS_WAR_PUMP_MAGENTA[] = {
	CHAR_TABLE_OFFSET + 255,CHAR_TABLE_OFFSET + 211,CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 254,
	CHAR_TABLE_OFFSET + 102,46, 0 };


//黄色正在泵墨.
unsigned short CHS_WAR_PUMP_YELLOW[] = {
	CHAR_TABLE_OFFSET + 256,CHAR_TABLE_OFFSET + 211,CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 254,
	CHAR_TABLE_OFFSET + 102,46, 0 };


//黑色正在泵墨.
unsigned short CHS_WAR_PUMP_BLACK[] = {
	CHAR_TABLE_OFFSET + 257,CHAR_TABLE_OFFSET + 211,CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 254,
	CHAR_TABLE_OFFSET + 102,46, 0 };


//浅兰色正在泵墨.
unsigned short CHS_WAR_PUMP_LIGHTCYAN[] = {
	CHAR_TABLE_OFFSET + 258,CHAR_TABLE_OFFSET + 253,CHAR_TABLE_OFFSET + 211,CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,
	CHAR_TABLE_OFFSET + 254,CHAR_TABLE_OFFSET + 102,46, 0 };


//浅品色正在泵墨.
unsigned short CHS_WAR_PUMP_LIGHTMAGENTA[] = {
	CHAR_TABLE_OFFSET + 258,CHAR_TABLE_OFFSET + 255,CHAR_TABLE_OFFSET + 211,CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,
	CHAR_TABLE_OFFSET + 254,CHAR_TABLE_OFFSET + 102,46, 0 };


//输入限时密码错误，含有含有非法字符;
unsigned short CHS_WAR_TIME_PASSWORD_WRONGINPUT[] = {
	CHAR_TABLE_OFFSET + 22,CHAR_TABLE_OFFSET + 23,CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 198,
	CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 259,
	CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 259,CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 260,CHAR_TABLE_OFFSET + 152,
	CHAR_TABLE_OFFSET + 261,CHAR_TABLE_OFFSET + 208,59,32, 0 };


//不是一个合法限时密码;
unsigned short CHS_WAR_TIME_PASSWORD_WRONG[] = {
	CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 262,CHAR_TABLE_OFFSET + 118,CHAR_TABLE_OFFSET + 263,CHAR_TABLE_OFFSET + 264,
	CHAR_TABLE_OFFSET + 152,CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,
	59,32, 0 };


//限时密码和厂商不匹配
unsigned short CHS_WAR_TIME_PASSWORD_MANUFACTURERIDNOTMATCH[] = {
	CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 89,
	CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 192,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186, 0 };


//限时密码和板子不匹配
unsigned short CHS_WAR_TIME_PASSWORD_BOARDIDNOTMATCH[] = {
	CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 89,
	CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 175,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186, 0 };


//第一次警告:再有100个小时就超出限时了, 请向厂家索取密码.
unsigned short CHS_WAR_LIMITEDTIME_FIRST[] = {
	CHAR_TABLE_OFFSET + 182,CHAR_TABLE_OFFSET + 118,CHAR_TABLE_OFFSET + 101,CHAR_TABLE_OFFSET + 250,CHAR_TABLE_OFFSET + 251,
	58,CHAR_TABLE_OFFSET + 265,CHAR_TABLE_OFFSET + 147,49,48,
	48,CHAR_TABLE_OFFSET + 263,CHAR_TABLE_OFFSET + 157,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 266,
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 267,
	44,32,CHAR_TABLE_OFFSET + 187,CHAR_TABLE_OFFSET + 67,CHAR_TABLE_OFFSET + 176,
	CHAR_TABLE_OFFSET + 268,CHAR_TABLE_OFFSET + 269,CHAR_TABLE_OFFSET + 155,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,
	46, 0 };


//第二次警告: 再有50个小时就超出限时了,请向厂家索取密码.
unsigned short CHS_WAR_LIMITEDTIME_SECOND[] = {
	CHAR_TABLE_OFFSET + 182,CHAR_TABLE_OFFSET + 123,CHAR_TABLE_OFFSET + 101,CHAR_TABLE_OFFSET + 250,CHAR_TABLE_OFFSET + 251,
	58,32,CHAR_TABLE_OFFSET + 265,CHAR_TABLE_OFFSET + 147,53,
	48,CHAR_TABLE_OFFSET + 263,CHAR_TABLE_OFFSET + 157,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 266,
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 267,
	44,CHAR_TABLE_OFFSET + 187,CHAR_TABLE_OFFSET + 67,CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 268,
	CHAR_TABLE_OFFSET + 269,CHAR_TABLE_OFFSET + 155,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,46, 0 };


//最后一次警告: 再有1个小时就超出限时并停止打印, 请立即向厂家索取密码.
unsigned short CHS_WAR_LIMITEDTIME_LAST[] = {
	CHAR_TABLE_OFFSET + 270,CHAR_TABLE_OFFSET + 243,CHAR_TABLE_OFFSET + 118,CHAR_TABLE_OFFSET + 101,CHAR_TABLE_OFFSET + 250,
	CHAR_TABLE_OFFSET + 251,58,32,CHAR_TABLE_OFFSET + 265,CHAR_TABLE_OFFSET + 147,
	49,CHAR_TABLE_OFFSET + 263,CHAR_TABLE_OFFSET + 157,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 266,
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 103,CHAR_TABLE_OFFSET + 271,
	CHAR_TABLE_OFFSET + 272,CHAR_TABLE_OFFSET + 273,CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 52,44,
	32,CHAR_TABLE_OFFSET + 187,CHAR_TABLE_OFFSET + 274,CHAR_TABLE_OFFSET + 275,CHAR_TABLE_OFFSET + 67,
	CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 268,CHAR_TABLE_OFFSET + 269,CHAR_TABLE_OFFSET + 155,CHAR_TABLE_OFFSET + 198,
	CHAR_TABLE_OFFSET + 199,46, 0 };


//输入选配密码错误，含有非0~F的值
unsigned short CHS_WAR_OPTION_PASSWORD_WRONGINPUT[] = {
	CHAR_TABLE_OFFSET + 22,CHAR_TABLE_OFFSET + 23,CHAR_TABLE_OFFSET + 276,CHAR_TABLE_OFFSET + 186,CHAR_TABLE_OFFSET + 198,
	CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 259,
	CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 260,48,126,70,
	CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 64, 0 };


//不是一个合法选配密码
unsigned short CHS_WAR_OPTION_PASSWORD_WRONG[] = {
	CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 262,CHAR_TABLE_OFFSET + 118,CHAR_TABLE_OFFSET + 263,CHAR_TABLE_OFFSET + 264,
	CHAR_TABLE_OFFSET + 152,CHAR_TABLE_OFFSET + 276,CHAR_TABLE_OFFSET + 186,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199, 0 };


//选配密码和厂商不匹配
unsigned short CHS_WAR_OPTION_PASSWORD_MANUFACTURERIDNOTMATCH[] = {
	CHAR_TABLE_OFFSET + 276,CHAR_TABLE_OFFSET + 186,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 89,
	CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 192,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186, 0 };


//选配密码和板子不匹配
unsigned short CHS_WAR_OPTION_PASSWORD_BOARDIDNOTMATCH[] = {
	CHAR_TABLE_OFFSET + 276,CHAR_TABLE_OFFSET + 186,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 89,
	CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 175,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186, 0 };


//泵墨安全瓶满了, 所有墨泵停止供墨.
unsigned short CHS_WAR_PUMP_PROHIBIT_SENSOR[] = {
	CHAR_TABLE_OFFSET + 254,CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 37,CHAR_TABLE_OFFSET + 38,CHAR_TABLE_OFFSET + 277,
	CHAR_TABLE_OFFSET + 278,CHAR_TABLE_OFFSET + 267,44,32,CHAR_TABLE_OFFSET + 212,
	CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 254,CHAR_TABLE_OFFSET + 272,CHAR_TABLE_OFFSET + 273,
	CHAR_TABLE_OFFSET + 279,CHAR_TABLE_OFFSET + 102,46, 0 };


//试图在不支持超频的头板版本上使用超频, 系统强制使用高速打印.
unsigned short CHS_WAR_OVER_SPEED_NOT_SUPPORT[] = {
	CHAR_TABLE_OFFSET + 68,CHAR_TABLE_OFFSET + 280,CHAR_TABLE_OFFSET + 143,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 159,
	CHAR_TABLE_OFFSET + 160,CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 110,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 73,
	CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 217,CHAR_TABLE_OFFSET + 218,CHAR_TABLE_OFFSET + 24,CHAR_TABLE_OFFSET + 205,
	CHAR_TABLE_OFFSET + 206,CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 110,44,32,
	CHAR_TABLE_OFFSET + 130,CHAR_TABLE_OFFSET + 131,CHAR_TABLE_OFFSET + 92,CHAR_TABLE_OFFSET + 227,CHAR_TABLE_OFFSET + 205,
	CHAR_TABLE_OFFSET + 206,CHAR_TABLE_OFFSET + 7,CHAR_TABLE_OFFSET + 108,CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 52,
	46, 0 };


//输入墨水密码错误，含有非0~F的值
unsigned short CHS_WAR_INK_PASSWORD_WRONGINPUT[] = {
	CHAR_TABLE_OFFSET + 22,CHAR_TABLE_OFFSET + 23,CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 198,
	CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 259,
	CHAR_TABLE_OFFSET + 147,CHAR_TABLE_OFFSET + 260,48,126,70,
	CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 64, 0 };


//不是一个合法墨水密码
unsigned short CHS_WAR_INK_PASSWORD_WRONG[] = {
	CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 262,CHAR_TABLE_OFFSET + 118,CHAR_TABLE_OFFSET + 263,CHAR_TABLE_OFFSET + 264,
	CHAR_TABLE_OFFSET + 152,CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199, 0 };


//墨水密码和厂商不匹配
unsigned short CHS_WAR_INK_PASSWORD_MANUFACTURERIDNOTMATCH[] = {
	CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 89,
	CHAR_TABLE_OFFSET + 176,CHAR_TABLE_OFFSET + 192,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186, 0 };


//墨水密码和板子不匹配
unsigned short CHS_WAR_INK_PASSWORD_BOARDIDNOTMATCH[] = {
	CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 89,
	CHAR_TABLE_OFFSET + 36,CHAR_TABLE_OFFSET + 175,CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 185,CHAR_TABLE_OFFSET + 186, 0 };


//再用10升就超出墨水限制了，第一次警告
unsigned short CHS_WAR_LIMITEDINK_FIRST[] = {
	CHAR_TABLE_OFFSET + 265,CHAR_TABLE_OFFSET + 206,49,48,CHAR_TABLE_OFFSET + 171,
	CHAR_TABLE_OFFSET + 266,CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,
	CHAR_TABLE_OFFSET + 196,CHAR_TABLE_OFFSET + 227,CHAR_TABLE_OFFSET + 267,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 182,
	CHAR_TABLE_OFFSET + 118,CHAR_TABLE_OFFSET + 101,CHAR_TABLE_OFFSET + 250,CHAR_TABLE_OFFSET + 251, 0 };


//再用5升就超出墨水限制了，第二次警告
unsigned short CHS_WAR_LIMITEDINK_SECOND[] = {
	CHAR_TABLE_OFFSET + 265,CHAR_TABLE_OFFSET + 206,53,CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 266,
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 196,
	CHAR_TABLE_OFFSET + 227,CHAR_TABLE_OFFSET + 267,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 182,CHAR_TABLE_OFFSET + 123,
	CHAR_TABLE_OFFSET + 101,CHAR_TABLE_OFFSET + 250,CHAR_TABLE_OFFSET + 251, 0 };


//再用1升就超出墨水限制了，最后一次警告
unsigned short CHS_WAR_LIMITEDINK_LAST[] = {
	CHAR_TABLE_OFFSET + 265,CHAR_TABLE_OFFSET + 206,49,CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 266,
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 170,CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 196,
	CHAR_TABLE_OFFSET + 227,CHAR_TABLE_OFFSET + 267,CHAR_TABLE_OFFSET + 158,CHAR_TABLE_OFFSET + 270,CHAR_TABLE_OFFSET + 243,
	CHAR_TABLE_OFFSET + 118,CHAR_TABLE_OFFSET + 101,CHAR_TABLE_OFFSET + 250,CHAR_TABLE_OFFSET + 251, 0 };


//当前喷头不支持墨水密码功能.
unsigned short CHS_WAR_INK_PASSWORD_UNKNOWN_DOT_VOLUME[] = {
	CHAR_TABLE_OFFSET + 161,CHAR_TABLE_OFFSET + 34,CHAR_TABLE_OFFSET + 58,CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 6,
	CHAR_TABLE_OFFSET + 159,CHAR_TABLE_OFFSET + 160,CHAR_TABLE_OFFSET + 102,CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 198,
	CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 30,CHAR_TABLE_OFFSET + 31,46, 0 };


//专色1正在泵墨.
unsigned short CHS_WAR_PUMP_SPOTCOLOR1[] = {
	CHAR_TABLE_OFFSET + 281,CHAR_TABLE_OFFSET + 211,49,CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,
	CHAR_TABLE_OFFSET + 254,CHAR_TABLE_OFFSET + 102,46, 0 };


//专色2正在泵墨.
unsigned short CHS_WAR_PUMP_SPOTCOLOR2[] = {
	CHAR_TABLE_OFFSET + 281,CHAR_TABLE_OFFSET + 211,50,CHAR_TABLE_OFFSET + 63,CHAR_TABLE_OFFSET + 143,
	CHAR_TABLE_OFFSET + 254,CHAR_TABLE_OFFSET + 102,46, 0 };


//打印数据小于点火数.
unsigned short CHS_WAR_FPGA_LESSDATA[] = {
	CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 52,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 157,
	CHAR_TABLE_OFFSET + 233,CHAR_TABLE_OFFSET + 15,CHAR_TABLE_OFFSET + 232,CHAR_TABLE_OFFSET + 84,46, 0 };


//打印数据多于点火数.
unsigned short CHS_WAR_FPGA_ULTRADATA[] = {
	CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 52,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 282,
	CHAR_TABLE_OFFSET + 233,CHAR_TABLE_OFFSET + 15,CHAR_TABLE_OFFSET + 232,CHAR_TABLE_OFFSET + 84,46, 0 };


//喷头温度太低.
unsigned short CHS_WAR_EPSONHEAD_TEMP_TOO_LOW[] = {
	CHAR_TABLE_OFFSET + 58,CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 49,CHAR_TABLE_OFFSET + 50,CHAR_TABLE_OFFSET + 156,
	CHAR_TABLE_OFFSET + 283,46, 0 };


//喷头温度太高.
unsigned short CHS_WAR_EPSONHEAD_TEMP_TOO_HIGH[] = {
	CHAR_TABLE_OFFSET + 58,CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 49,CHAR_TABLE_OFFSET + 50,CHAR_TABLE_OFFSET + 156,
	CHAR_TABLE_OFFSET + 7,46, 0 };


//喷头未连接.
unsigned short CHS_WAR_EPSONHEAD_NOT_CONNECT[] = {
	CHAR_TABLE_OFFSET + 58,CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 284,CHAR_TABLE_OFFSET + 203,CHAR_TABLE_OFFSET + 204,
	46, 0 };


//主板FPGA报告的数据太多.
unsigned short CHS_WAR_EPSON_FPGA_ULTRADATA[] = {
	CHAR_TABLE_OFFSET + 162,CHAR_TABLE_OFFSET + 36,70,80,71,
	65,CHAR_TABLE_OFFSET + 285,CHAR_TABLE_OFFSET + 251,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 84,
	CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 156,CHAR_TABLE_OFFSET + 282,46, 0 };


//主板FPGA报告的数据太少.
unsigned short CHS_WAR_EPSON_FPGA_LESSDATA[] = {
	CHAR_TABLE_OFFSET + 162,CHAR_TABLE_OFFSET + 36,70,80,71,
	65,CHAR_TABLE_OFFSET + 285,CHAR_TABLE_OFFSET + 251,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 84,
	CHAR_TABLE_OFFSET + 184,CHAR_TABLE_OFFSET + 156,CHAR_TABLE_OFFSET + 286,46, 0 };


//当前喷头的喷嘴数未知。
unsigned short CHS_WAR_UNKNOWN_NOZZLE_COUNT[] = {
	CHAR_TABLE_OFFSET + 161,CHAR_TABLE_OFFSET + 34,CHAR_TABLE_OFFSET + 58,CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 151,
	CHAR_TABLE_OFFSET + 58,CHAR_TABLE_OFFSET + 59,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 284,CHAR_TABLE_OFFSET + 248,
	CHAR_TABLE_OFFSET + 150, 0 };


//升级成功.
unsigned short CHS_UPDATE_SUCCESS[] = {
	CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 132,CHAR_TABLE_OFFSET + 245,CHAR_TABLE_OFFSET + 30,46, 0 };


//DSP开始升级命令超时.
unsigned short CHS_UPDATEERR_DSP_BEGIN_TIMEOUT[] = {
	68,83,80,CHAR_TABLE_OFFSET + 47,CHAR_TABLE_OFFSET + 178,
	CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 132,CHAR_TABLE_OFFSET + 230,CHAR_TABLE_OFFSET + 229,CHAR_TABLE_OFFSET + 169,
	CHAR_TABLE_OFFSET + 103,46, 0 };


//DSP数据超时.
unsigned short CHS_UPDATEERR_DSP_DATA_TIMEOUT[] = {
	68,83,80,CHAR_TABLE_OFFSET + 84,CHAR_TABLE_OFFSET + 184,
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 103,46, 0 };


//DSP结束超时.
unsigned short CHS_UPDATEERR_DSP_END_TIMEOUT[] = {
	68,83,80,CHAR_TABLE_OFFSET + 287,CHAR_TABLE_OFFSET + 288,
	CHAR_TABLE_OFFSET + 169,CHAR_TABLE_OFFSET + 103,46, 0 };


//错误的升级文件格式.
unsigned short CHS_UPDATEERR_ILIGALFILE[] = {
	CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 132,
	CHAR_TABLE_OFFSET + 129,CHAR_TABLE_OFFSET + 194,CHAR_TABLE_OFFSET + 289,CHAR_TABLE_OFFSET + 88,46, 0 };


//不合法的升级文件.
unsigned short CHS_UPDATEERR_INTERNAL_DATA[] = {
	CHAR_TABLE_OFFSET + 6,CHAR_TABLE_OFFSET + 264,CHAR_TABLE_OFFSET + 152,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 171,
	CHAR_TABLE_OFFSET + 132,CHAR_TABLE_OFFSET + 129,CHAR_TABLE_OFFSET + 194,46, 0 };


//升级校验码错误.
unsigned short CHS_UPDATEERR_CHECKSUM[] = {
	CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 132,CHAR_TABLE_OFFSET + 56,CHAR_TABLE_OFFSET + 290,CHAR_TABLE_OFFSET + 199,
	CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,46, 0 };


//ARM 闪存擦除.
unsigned short CHS_UPDATEERR_EREASE[] = {
	65,82,77,32,CHAR_TABLE_OFFSET + 109,
	CHAR_TABLE_OFFSET + 291,CHAR_TABLE_OFFSET + 292,CHAR_TABLE_OFFSET + 293,46, 0 };


//Flash错误.
unsigned short CHS_UPDATEERR_FLASHOP[] = {
	70,108,97,115,104,
	CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138,46, 0 };


//需要特别固件。
unsigned short CHS_UPDATEERR_GZ_ILIGALFILE[] = {
	CHAR_TABLE_OFFSET + 294,CHAR_TABLE_OFFSET + 222,CHAR_TABLE_OFFSET + 224,CHAR_TABLE_OFFSET + 295,CHAR_TABLE_OFFSET + 193,
	CHAR_TABLE_OFFSET + 194,CHAR_TABLE_OFFSET + 150, 0 };


//需要特殊墨水密码固件。
unsigned short CHS_UPDATEERR_INKP_ILIGALFILE[] = {
	CHAR_TABLE_OFFSET + 294,CHAR_TABLE_OFFSET + 222,CHAR_TABLE_OFFSET + 224,CHAR_TABLE_OFFSET + 225,CHAR_TABLE_OFFSET + 102,
	CHAR_TABLE_OFFSET + 76,CHAR_TABLE_OFFSET + 198,CHAR_TABLE_OFFSET + 199,CHAR_TABLE_OFFSET + 193,CHAR_TABLE_OFFSET + 194,
	CHAR_TABLE_OFFSET + 150, 0 };


//8头硬件无法升级16头的固件。
unsigned short CHS_UPDATEERR_16H_ILIGALFILE[] = {
	56,CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 296,CHAR_TABLE_OFFSET + 194,CHAR_TABLE_OFFSET + 119,
	CHAR_TABLE_OFFSET + 152,CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 132,49,54,
	CHAR_TABLE_OFFSET + 73,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 193,CHAR_TABLE_OFFSET + 194,CHAR_TABLE_OFFSET + 150, 0 };

//未知的错误
unsigned short CHS_UNKNOWN_ERR[] = {
	CHAR_TABLE_OFFSET + 284,CHAR_TABLE_OFFSET + 248,CHAR_TABLE_OFFSET + 151,CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138, 0 };

unsigned short CHS_STATUS_SVC[] = {CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138, 0 };
unsigned short CHS_STATUS_FTA[] = {CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138, 0 };
unsigned short CHS_STATUS_ERR[] = {CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138, 0 };
unsigned short CHS_STATUS_ERR_AUTO[] = {CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138, 0 };
unsigned short CHS_STATUS_WAR[] = {CHAR_TABLE_OFFSET + 69,CHAR_TABLE_OFFSET + 138, 0 };
unsigned short CHS_STATUS_INI[] = {CHAR_TABLE_OFFSET + 177,CHAR_TABLE_OFFSET + 178,CHAR_TABLE_OFFSET + 55, 0 };
unsigned short CHS_STATUS_RDY[] = {CHAR_TABLE_OFFSET + 266,CHAR_TABLE_OFFSET + 297, 0 };
unsigned short CHS_STATUS_MOVING[] = {CHAR_TABLE_OFFSET + 298,CHAR_TABLE_OFFSET + 18, 0 };
unsigned short CHS_STATUS_PRINT[] = {CHAR_TABLE_OFFSET + 51,CHAR_TABLE_OFFSET + 52, 0 };
unsigned short CHS_STATUS_PAUSE[] = {CHAR_TABLE_OFFSET + 299,CHAR_TABLE_OFFSET + 272,32, 0 };
unsigned short CHS_STATUS_CLEANING[] = {CHAR_TABLE_OFFSET + 85,CHAR_TABLE_OFFSET + 86, 0 };
unsigned short CHS_STATUS_CANCELING[] = {CHAR_TABLE_OFFSET + 155,CHAR_TABLE_OFFSET + 300, 0 };
unsigned short CHS_STATUS_UPDATING[] = {CHAR_TABLE_OFFSET + 171,CHAR_TABLE_OFFSET + 132, 0 };
#endif
#endif

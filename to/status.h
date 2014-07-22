#ifndef _STATUS_H_
#define _STATUS_H_

#include "os_includes.h"

#define STATUS_BITS				0x1FFFFF00

//#define STATUS_NO_PRINT_BITS	0x0FCFE800

//#define STATUS_NO_PRINT_BITS	0x0FCFC800  //20090910 Change, Print huge band, pause sometimes will cause hung up
//#define STATUS_NO_PRINT_BITS	0x0FCFCA00  //20100128 V2.4.0 Add STATUS_ERR_AUTO
//#define STATUS_NO_PRINT_BITS	0x0FCFEA00  //20110520 Add CMD_PAUSE. 
#define STATUS_NO_PRINT_BITS	0x0FCFCA00  //20111029 remove CMD_PAUSE. see the comment at main.c UART_DSP_RPT_STOP.

#define STATUS_NO_X_MOVE_BITS	0x0FAFCE00  
#define STATUS_NO_Y_MOVE_BITS	0x0FEFCE00  

#define STATUS_NO_CLEAN_BITS	0x0FAFAE00  
#define STATUS_NO_CLEAN_BITS_FOR_PAUSE	0x0F8F8E00  

//0x80000000 is used for STATUS_CHANGE in status.c

/*
#define EXT_STATUS_LEVEL_RDY	0		
#define EXT_STATUS_LEVEL_SVC	1
#define EXT_STATUS_LEVEL_FTA	2
#define EXT_STATUS_LEVEL_ERR	3
#define EXT_STATUS_LEVEL_WAR	4*/

//Status number must be less than 255, 
//DSP number: 0xD0~0xFF 

#define STATUS_NO_CANCEL  	0x10000000

#define STATUS_SVC		0x08000000 	//Highest priority
#define STATUS_FTA		0x04000000
#define STATUS_ERR		0x02000000

#define STATUS_INI		0x01000000
#define STATUS_MOVING   	0x00800000  
#define STATUS_PAUSE    	0x00400000
#define STATUS_PRINT		0x00200000  
#define STATUS_WAR		0x00100000
#define STATUS_RDY		0x00000000	//Lowest priority
#define STATUS_CLEANING 	0x00080000

#define CMD_CLEAN_MANUAL 	0x00040000
#define CMD_CLEAN_SINGLE	0x00020000
#define CMD_CLEAN_AUTO		0x00010000
#define CMD_START_MOVE		0x00008000
//#define CMD_MEDIA_MOVE	0x00004000
#define CMD_EPSON_OPERATION     0x00004000
#define CMD_PAUSE		0x00002000
#define CMD_CANCEL_REQ	        0x00001000

#define STATUS_UPDATING		0x00000800  //For updating
#define STATUS_MOVING_FLASH	0x00000400  //MOVING=>MOVING_FLASH=>NOT MOVING_FLASH=>NOT MOVING: 表示是否是"非X"运动，这种运动不影响闪喷
//当STATUS_MOVING_FLASH设置时，STATUS_MOVING必须已经设置
//当STATUS_MOVING清除时，STATUS_MOVING_FLASH必须已经被清除
#define STATUS_ERR_AUTO		0x00000200  //20100128, V2.4.0

#define STATUS_CAPPED		0x00000100  //20100128, V2.4.0

#define BUF_STATUS_BITS	(STATUS_UPDATING|STATUS_WAR|STATUS_ERR|STATUS_ERR_AUTO|STATUS_INI|STATUS_FTA|STATUS_SVC)


//If have high priority status, only show higher priority status
//Warning maybe can show together with printing, pause, and move
//But: PRINT & PAUSE & MOVING 		== Pausing
//	   PRINT & PAUSE & !MOVING		== Pause
//	   PRINT & !PAUSE & MOVING		== Printing		
//	   PRINT & !PAUSE & !MOVING		== Printing		
//	   !PRINT & PAUSE & MOVING 		== Moving;   XXX error status
//	   !PRINT & PAUSE & !MOVING		== Ready;	 XXX error status
//	   !PRINT & !PAUSE & MOVING		== Moving		
//	   !PRINT & !PAUSE & !MOVING	== Ready		

//	   CANCEL & PRINT & PAUSE & MOVING 		== Canceling
//	   CANCEL & PRINT & PAUSE & !MOVING		== Canceling
//	   CANCEL & PRINT & !PAUSE & MOVING		== Printing		
//	   CANCEL & PRINT & !PAUSE & !MOVING		== Printing		
//	   CANCEL & !PRINT & PAUSE & MOVING 		== Moving;   XXX error status
//	   CANCEL & !PRINT & PAUSE & !MOVING		== Ready;	 XXX error status
//	   CANCEL & !PRINT & !PAUSE & MOVING		== Moving		
//	   CANCEL & !PRINT & !PAUSE & !MOVING	== Ready		


#define STATUS_SVC_SX2						(STATUS_SVC+0) //USB chip. 经过0.5秒，USB芯片还没有正常启动，USB芯片或者损坏
#define STATUS_SVC_FPGA0					(STATUS_SVC+1) //FPGA chip 1. 上电后，FPGA的nSTATUS持续为高，FPGA芯片或者损坏 
#define STATUS_SVC_FPGA1					(STATUS_SVC+2) //FPGA chip 2. 拉低nCONFIG之后，CONFIG_Done或者nSTATUS还是高
#define STATUS_SVC_FPGA2					(STATUS_SVC+3) //FPGA chip 3. nCONFIG拉高之后, nSTATUS保持高.
#define STATUS_SVC_FPGA3					(STATUS_SVC+4) //FPGA chip 4. When config FPGA, FPGA report err, and retry 10 times
#define STATUS_SVC_UPDATE_FAIL				(STATUS_SVC+5) //UPDATE Main Board Failed.
#define STATUS_SVC_BYHX_DATA				(STATUS_SVC+6) //板子没有经过BYHX初始化
#define STATUS_SVC_EEPROM_CHK				(STATUS_SVC+16) //BYHX_TOOL check EEPROM 没有通过。
#define STATUS_SVC_SDRAM_CHECK				(STATUS_SVC+7)  //SDRAM 的Check没有通过
#define STATUS_SVC_FLASH					(STATUS_SVC+8) //Flash. Cann't read FPGA from flash. 
#define STATUS_SVC_SDRAM_NOMATCH			(STATUS_SVC+9)  //SDRAM 太小，不能支持当前的设置。
#define STATUS_SVC_READBOARDID_FAIL			(STATUS_SVC+10)  //Read board id Error。实际上是操作加密芯片失败
#define STATUS_SVC_ARM_MOTION       		(STATUS_SVC+11)  //ARM Motion system initial error.
#define STATUS_SVC_HEADBOARD_INIT_FAIL		(STATUS_SVC+12)  //HEAD board initial fail。实际上是EPSON头板操作加密芯片失败
#define STATUS_SVC_POWER_OFF_MODE		(STATUS_SVC+13)  //关机模式

#define STATUS_FTA_SX2RESET					(STATUS_FTA+0) //USB chip异常重启
#define STATUS_FTA_INTERNAL_WRONGHEADER		(STATUS_FTA+1) //Wrong data header
#define STATUS_FTA_INTERNAL_WRONGHEADERSIZE (STATUS_FTA+2) //Wrong data header size
#define STATUS_FTA_INTERNAL_JOBSTARTHEADER	(STATUS_FTA+3) //Job header不应附带额外数据
#define STATUS_FTA_INTERNAL_BANDDATASIZE	(STATUS_FTA+4) //Band Header中的BAND数据数量和实际BAND数据数量不符
#define STATUS_FTA_INTERNAL_WRONGFORMAT		(STATUS_FTA+5) //得到的串口数据格式不对
#define STATUS_FTA_INTERNAL_DMA0WORKING		(STATUS_FTA+6) //DMA0 still working after a band complete
#define STATUS_FTA_INTERNAL_PRINTPOINT		(STATUS_FTA+7) //Wrong startpoint and endpoint when print
#define STATUS_FTA_INTERNAL_OLIMIT			(STATUS_FTA+8) //Band的打印起始点小于原点
#define STATUS_FTA_INTERNAL_OPPLIMIT		(STATUS_FTA+9) //图像结束位置超出了打印机最远点,Image too width
#define STATUS_FTA_DSPINITS1				(STATUS_FTA+10) //运动控制第一阶段初始化没有通过
#define STATUS_FTA_DSPINITS2				(STATUS_FTA+11) //运动控制第二阶段初始化没有通过
#define STATUS_FTA_HEADINITS1				(STATUS_FTA+12) //头板第一阶段初始化没有通过
#define STATUS_FTA_HEADINITS2				(STATUS_FTA+13) //头板第二阶段初始化没有通过
#define STATUS_FTA_HEADTOMAINROAD			(STATUS_FTA+14) //主板的LVDS接收芯片没有LOCK,或线没有插
#define STATUS_FTA_INTERNAL_BANDDIRECTION   (STATUS_FTA+15) //Band定义中的方向值超出定义
#define STATUS_FTA_DSPUPDATE_FAIL			(STATUS_FTA+16) //更新失败：主板写入阶段
#define STATUS_FTA_EEPROM_READ				(STATUS_FTA+17) //读取EEPROM失败	
#define STATUS_FTA_EEPROM_WRITE				(STATUS_FTA+18) //写入EEPROM失败
#define STATUS_FTA_FACTORY_DATA				(STATUS_FTA+19) //板子没有经过出厂初始化设置
#define STATUS_FTA_HEADBOARD_RESET			(STATUS_FTA+20) //头板被重新启动
#define STATUS_FTA_SPECTRAHVBINITS1			(STATUS_FTA+21) //Spectra High Voltage Board第一阶段初始化失败
#define STATUS_FTA_PRINTHEAD_NOTMATCH		(STATUS_FTA+22) //头板报告的喷头种类与FactoryData里面的设定不匹配， 请更换头板或重新设定硬件设置。
#define STATUS_FTA_MANUFACTURERID_NOTMATCH  (STATUS_FTA+23) //控制系统与FW的生产厂商不匹配，需更换系统或者升级FW 
#define STATUS_FTA_LIMITEDTIME_RUNOUT		(STATUS_FTA+24) //超过限时时间，请重新输入口令
#define STATUS_FTA_INTERNAL_SW1				(STATUS_FTA+25) //Internal error, blank band and y-distance == 0
#define STATUS_FTA_USB1_USB1CONNECT			(STATUS_FTA+26)  //连接到USB1口   //This error is moved to Fatal error, 2.0版本这里写错了，写为(STATUS_ERR+26)
#define STATUS_FTA_UILANGUAGE_NOT_MATCH		(STATUS_FTA+27)  //UI使用的语言与权限不符， 请重新设置语言或者重新输入密码
#define STATUS_FTA_INTERNAL_WRONGPINCONF	(STATUS_FTA+28)  //PINCONF写入FPGA错误
#define STATUS_FTA_FACTORY_COLORNUM_WRONG	(STATUS_FTA+29)  //Factory 中写入的color number 不对
#define STATUS_FTA_HB_EEPROM_WRT_ERR		(STATUS_FTA+30)  //头板的EEPROM写入失败
#define STATUS_FTA_HB_OVERHEAT				(STATUS_FTA+31)  ////喷头加热超过了55度，所有加热被强行关闭。
#define STATUS_FTA_SHAKEHAND_ERR			(STATUS_FTA+32)  //软件错误或者版本错误，没有通过握手协议。
#define STATUS_FTA_SWVERSION_ERR			(STATUS_FTA+33)  //固件要求特殊的软件版本，版本错误。
#define STATUS_FTA_NOT_SUPPORT_2C_SHARE_1H	(STATUS_FTA+34)  //当前系统设置不支持一头两色。
#define STATUS_FTA_LIMITEDINK_RUNOUT		(STATUS_FTA+35)  //超过墨水用量限制，请重新输入口令
#define STATUS_FTA_SWVERSION_ERR_INK_PWD	(STATUS_FTA+36)  //固件要求软件版本支持墨水密码，版本错误，
#define STATUS_FTA_EPSON_HEAD_BAD			(STATUS_FTA+37)  //EPSON打印头报错.
#define STATUS_FTA_EPSON_HEAD_FPGA_ERR		(STATUS_FTA+38)  //EPSON头板FPGA报错.通常是超频或数据被干扰。
#define STATUS_FTA_SECURITY_CHIP_ERR		(STATUS_FTA+39)  //Write board id error.
#define STATUS_FTA_CALIBRATION_DATA 		(STATUS_FTA+41)  //出厂初始化设置出错。
#define STATUS_FTA_PRINT_DATA_ERROR 		(STATUS_FTA+47)  //打印数据传输错误或丢失
#define STATUS_FTA_FACTORY_DATA_ERROR 		(STATUS_FTA+48)  //FACTORY_DATA数据错误
#define STATUS_FTA_WASTE_INK_FULL 			(STATUS_FTA+49)	//废墨溢出
#define STATUS_FTA_DPI_NOT_SUPPORT 			(STATUS_FTA+50)	//不支持的分辨率

#define STATUS_ERR_EP0OVERUN_SETUPDATA		(STATUS_ERR+0)  //EP0命令被打断
//#define STATUS_ERR_USB1_USB1CONNECT			(STATUS_ERR+1)  //连接到USB1口   //This error is moved to Fatal error
#define STATUS_ERR_UART1_TXTIMEOUT			(STATUS_ERR+3)  //头板与主板通讯超时
#define STATUS_ERR_UART2_TXTIMEOUT			(STATUS_ERR+2)  //运动通讯超时
#define STATUS_ERR_INTERNAL_PRINTDATA		(STATUS_ERR+4)  //Band数据没有打印完成
#define STATUS_ERR_FPGA_LESSDATA			(STATUS_ERR+5)  //Print data is less than fire number or empty when trigger
#define STATUS_ERR_FPGA_ULTRADATA			(STATUS_ERR+6)  //Print data is more than fire number
#define STATUS_ERR_FPGA_WRONGSTATUS			(STATUS_ERR+7)
#define STATUS_ERR_UV_CTR_WRONGSTATUS		(STATUS_ERR+8)	//Internal Status
#define STATUS_ERR_FPGA_WRONGCOMMAND		(STATUS_ERR+9)  //FPGA 收到错误的命令
#define STATUS_ERR_MOTION_SPEED_BAD			(STATUS_ERR+10) //运动速度太快或不均匀，数据还在出书，却有点火命令
#define STATUS_ERR_INTERNAL_MOVING_TIMEOUT	(STATUS_ERR+11) //运动命令发出30秒后，没有收到运动完成命令
#define STATUS_ERR_INTERNAL_WRONGAUTOCLEAN	(STATUS_ERR+12)  //错误的AUTOCLEAN命令，不应该发生
#define STATUS_ERR_INTERNAL_WRONGBANDINFO	(STATUS_ERR+13)  //错误的AUTOCLEAN命令，不应该发生

#define STATUS_ERR_INTERNAL_DEBUG1	(STATUS_ERR+0x50) //Debug status 1
#define STATUS_ERR_INTERNAL_DEBUG2	(STATUS_ERR+0x51) //Debug status 1



#define STATUS_INI_ARM				(STATUS_INI+0)  //正在初始化主控板
#define STATUS_INI_SX2				(STATUS_INI+1)  //正在初始化USB通讯
#define STATUS_INI_FPGA				(STATUS_INI+2)  //正在初始化FPGA
#define STATUS_INI_DSP				(STATUS_INI+3)	//正在初始化运动
#define STATUS_INI_HEADBOARD		(STATUS_INI+4)  //正在初始化头板
#define STATUS_INI_HVB				(STATUS_INI+5)  //Spectra 正在初始化高压板

#define STATUS_WAR_UNKNOWNHEADERTYPE (STATUS_WAR+0) //未定义的数据头标示，将被忽略
#define STATUS_WAR_EP0OVERUN_REQUEST_IGNORE	(STATUS_WAR+1) //EP0数据传输未完成，又收到新的EP0命令，旧的数据传输忽略
#define STATUS_WAR_PUMP_CYAN	(STATUS_WAR+2)
#define STATUS_WAR_PUMP_MAGENTA	(STATUS_WAR+3)
#define STATUS_WAR_PUMP_YELLOW	(STATUS_WAR+4)
#define STATUS_WAR_PUMP_BLACK	(STATUS_WAR+5)
#define STATUS_WAR_PUMP_LIGHTCYAN		(STATUS_WAR+6)
#define STATUS_WAR_PUMP_LIGHTMAGENTA	(STATUS_WAR+7)

#define STATUS_WAR_TIME_PASSWORD_WRONGINPUT	(STATUS_WAR+8) //输入限时密码错误，含有非0~F的值
#define STATUS_WAR_TIME_PASSWORD_WRONG		(STATUS_WAR+9)	//不是一个合法限时密码
#define STATUS_WAR_TIME_PASSWORD_MANUFACTURERIDNOTMATCH (STATUS_WAR+10)  //限时密码和厂商不匹配
#define STATUS_WAR_TIME_PASSWORD_BOARDIDNOTMATCH (STATUS_WAR+11) //限时密码和板子不匹配

#define STATUS_WAR_LIMITEDTIME_FIRST	(STATUS_WAR+12) //再有100个小时就超出限时了，第一次警告
#define STATUS_WAR_LIMITEDTIME_SECOND	(STATUS_WAR+13) //再有50个小时就超出限时了，第二次警告
#define STATUS_WAR_LIMITEDTIME_LAST		(STATUS_WAR+14) //再有1个小时就超出限时了，最后一次警告

#define STATUS_WAR_OPTION_PASSWORD_WRONGINPUT	(STATUS_WAR+16) //输入选配密码错误，含有非0~F的值
#define STATUS_WAR_OPTION_PASSWORD_WRONG		(STATUS_WAR+17)	//不是一个合法选配密码
#define STATUS_WAR_OPTION_PASSWORD_MANUFACTURERIDNOTMATCH (STATUS_WAR+18)  //选配密码和厂商不匹配
#define STATUS_WAR_OPTION_PASSWORD_BOARDIDNOTMATCH (STATUS_WAR+19) //选配密码和板子不匹配

#define STATUS_WAR_PUMP_PROHIBIT_SENSOR	(STATUS_WAR+20) //使能禁止墨盒泵墨传感器时
#define STATUS_WAR_OVER_SPEED_NOT_SUPPORT	(STATUS_WAR+21) //KM512LN 试图超频，但是头板不支持，取消超频

#define STATUS_WAR_INK_PASSWORD_WRONGINPUT	(STATUS_WAR+22) //输入墨水密码错误，含有非0~F的值
#define STATUS_WAR_INK_PASSWORD_WRONG		(STATUS_WAR+23)	//不是一个合法墨水密码
#define STATUS_WAR_INK_PASSWORD_MANUFACTURERIDNOTMATCH (STATUS_WAR+24)  //墨水密码和厂商不匹配
#define STATUS_WAR_INK_PASSWORD_BOARDIDNOTMATCH (STATUS_WAR+25) //墨水密码和板子不匹配

#define STATUS_WAR_LIMITEDINK_FIRST	(STATUS_WAR+26) //再用10升就超出墨水限制了，第一次警告
#define STATUS_WAR_LIMITEDINK_SECOND	(STATUS_WAR+27) //再用5升就超出墨水限制了，第二次警告
#define STATUS_WAR_LIMITEDINK_LAST		(STATUS_WAR+28) //再用1升就超出墨水限制了，最后一次警告

#define STATUS_WAR_INK_PASSWORD_UNKNOWN_DOT_VOLUME	(STATUS_WAR+29) //当前，只支持Konica和Polaris，其他的头的墨滴大小不对，总是42pl。

#define STATUS_WAR_PUMP_SPOTCOLOR1	(STATUS_WAR+30)
#define STATUS_WAR_PUMP_SPOTCOLOR2	(STATUS_WAR+31)

#define STATUS_WAR_FPGA_LESSDATA	(STATUS_WAR+32)  //Print data is less than fire number or empty when trigger
#define STATUS_WAR_FPGA_ULTRADATA	(STATUS_WAR+33)  //Print data is more than fire number

#define STATUS_WAR_EPSONHEAD_TEMP_TOO_LOW	(STATUS_WAR+34)  //Head temperature too low.
#define STATUS_WAR_EPSONHEAD_TEMP_TOO_HIGH	(STATUS_WAR+35)  //Head temperature too high.
#define STATUS_WAR_EPSONHEAD_NOT_CONNECT	(STATUS_WAR+36)  //Print head is not connected.
#define STATUS_WAR_EPSON_FPGA_ULTRADATA		(STATUS_WAR+37)  //core board FPGA report data too more.
#define STATUS_WAR_EPSON_FPGA_LESSDATA		(STATUS_WAR+38)  //core board FPGA report data too less. .

#define STATUS_WAR_UNKNOWN_NOZZLE_COUNT	(STATUS_WAR+39) //当前喷头的喷嘴数未知。

#define STATUS_WAR_EPSON_DRIVER_OVERHEAT	(STATUS_WAR+40)  //EPSON Head Driver over heat.
#define STATUS_WAR_EPSON_XHOT_ERROR	(STATUS_WAR+41)     //XHOT error.
#define STATUS_WAR_HB_FW_VERSION			(STATUS_WAR+44)	// 升级包包含头板的FW版本太低，已不支持
#define STATUS_WAR_HB_FW_ID_ERROR			(STATUS_WAR+45)	// 头板FW ID不匹配，当前头板不能升级该FW
#define STATUS_WAR_HB_FW_UPDATER			(STATUS_WAR+46)	// 头板升级过程发生错误
#define STATUS_WAR_EPSON_HEAD_FPGA_ERR		(STATUS_WAR+49)  //头板FPGA错误 原FTA38错误 改为waring
#define STATUS_WAR_EPSON_HEAD_LVDS_ERR		(STATUS_WAR+50)  //初始化后 主板LVDS失锁改为waring
#define STATUS_WAR_EPSON_HEAD_REPORT_LVDS_ERR		(STATUS_WAR+51)  //头板报告LVDS失锁

#define STATUS_WAR_K_LACK			(STATUS_WAR+52)  //k 缺墨
#define STATUS_WAR_C_LACK			(STATUS_WAR+53)  //C 缺墨
#define STATUS_WAR_M_LACK			(STATUS_WAR+54)  //M 缺墨
#define STATUS_WAR_Y_LACK			(STATUS_WAR+55)  //Y 缺墨
#define STATUS_WAR_W_LACK		(STATUS_WAR+56)  //W 缺墨
#define STATUS_WAR_V_LACK			(STATUS_WAR+57)  //V 缺墨
#define STATUS_WAR_Waste_Ink_FULL 			(STATUS_WAR+58)  //废墨溢出

#define STATUS_UPDATE_SUCCESS 		(STATUS_UPDATING+1) //Can not use (STATUS_UPDATING+0)
#define STATUS_UPDATEERR_DSP_BEGIN_TIMEOUT	(STATUS_UPDATING+2)
#define STATUS_UPDATEERR_DSP_DATA_TIMEOUT	(STATUS_UPDATING+3)
#define STATUS_UPDATEERR_DSP_END_TIMEOUT	(STATUS_UPDATING+4)
#define STATUS_UPDATEERR_ILIGALFILE 		(STATUS_UPDATING+5)
#define STATUS_UPDATEERR_INTERNAL_DATA		(STATUS_UPDATING+6)
#define STATUS_UPDATEERR_CHECKSUM			(STATUS_UPDATING+7)
#define STATUS_UPDATEERR_EREASE				(STATUS_UPDATING+8)
#define STATUS_UPDATEERR_FLASHOP			(STATUS_UPDATING+9) //ARM flash erease or write error, 10 times retry
#define STATUS_UPDATEERR_GZ_ILIGALFILE 		(STATUS_UPDATING+10)
#define STATUS_UPDATEERR_INKP_ILIGALFILE 	(STATUS_UPDATING+11)
#define STATUS_UPDATEERR_16H_ILIGALFILE 	(STATUS_UPDATING+12)
#define STATUS_UPDATEERR_HB_BEGIN_TIMEOUT	(STATUS_UPDATING+13)
#define STATUS_UPDATEERR_HB_DATA_TIMEOUT	(STATUS_UPDATING+14)
#define STATUS_UPDATEERR_HB_END_TIMEOUT		(STATUS_UPDATING+15)
#define STATUS_UPDATEERR_UVP_ILLEGALFILE	(STATUS_UPDATING+16)
#define STATUS_UPDATEERR_TRY_AGAIN       	(STATUS_UPDATING+17)

#ifdef CONVERSION_BOARD
#define HEAD_ERROR_SET 2
#define HEAD_ERROR_CLEAR 3
#endif

#define STATUS_SET	1
#define STATUS_CLR	0

void status_ReportStatus(INT32U number, INT8U op);
INT8U status_IsPrinting(void);
void status_FlushAllErrStatus(void);

enum SciError
{
	SciError_OK      = 0xF0,  //没有错误
	
	SciError_Command     = 0xF1,  //错误的命令
	SciError_Parameter     = 0xF2,  //错误的参数
	SciError_MoveAgain     = 0xF3,  //运行中错误 已经运动,又发运动命令 
	SciError_TxTimeOut     = 0xF4,  //发送超时
	SciError_FormatErr     = 0xF5,  //校验和错误
	SciError_Encoder     = 0xF6,     //告诉上位机光栅错误
	SciError_MeasureSensor    = 0xF7,  //纸宽传感器错误
	SciError_NoPaper     = 0xF8,  //没有介质
	SciError_PaperJamX     = 0xF9,  //X轴运动受阻
	SciError_PaperJamY     = 0xFA,  //Y轴运动受阻
	SciError_IndexNoMatch    = 0xFB,//索引错误
	SciError_LimitSensor    = 0xFC,     //接触限位传感器
	SciError_StepEncoder    = 0xFD,     //没有发现步进编码器
	
	SciError_ReadEEPROM     = 0xE1,//读 EEPROM 错误
	SciError_WriteEEPROM     = 0xE2,//写 EEPROM 错误
	
	SciError_WriteEEPROMTwice   = 0xE3,//写EEPROM  2 次
	SciError_TimeOver     = 0xE4,//限制的时间就要用光了
	SciError_TimeWarning    = 0xE5,//到限制的时间还有50个小时
	SciError_Lang      = 0xE6,//语言和限制的不匹配
	SciError_IllegalContent    = 0xE7,//错误的EEPROM 的内容，EEPROM 配置没有完成
	SciError_IllegalPwd        = 0xE8,//错误的密码
	
	SciError_SwDogEey        = 0xE9, //软件没有使用加密狗
	
	SciError_SensorPause       = 0xEE,//接触定位传感器
	//cheney add.
	SciError_OverCurrent_Protect = 0xDF, //过流保护
	SciError_OverTemp_Protect = 0xDE,   //温度过高保护 
	SciError_InternalErr_1 = 0xDD,//运动内部错误1
	SciError_InternalErr_2 = 0xDC //运动内部错误2
};
#ifdef MANUFACTURER_DYSS
void changeLeddisplay(INT8U state);
#endif
#ifdef FUNC_MAINTAIN
//same with the flag "CMD_CLEAN_MANUAL", can not use as flag,
//用来在给PM发送的状态中表示"维护状态“
#define STATUS_MAINTAIN 0x00040000		
extern INT8U bStatus_Maintain;
#endif	//end FUNC_MAINTAIN
#endif


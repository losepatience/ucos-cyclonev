#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "os_includes.h"
#include "printers.h"
#include <stdlib.h>

#ifdef _GLOBAL_C_
#define EXTERN
#else
#define EXTERN extern
#endif
//#define STEP_ALL_WIDTH //整幅宽步进

//#define EPSON_DX5E
//#define LB_TEST
//#define DEBUG_Y_ENCODER

//#define PUMP_INTERMITTENT		//间歇泵墨
//#define TIMER_ON				//定时器,open this macro in "ucos_ii.h" or project option

#define PASSWORD_LEN 16

#define MAX_HEAD_NUMBER 16
#define MAX_COLOR_NUMBER 8

#define VOLTAGE_NUMBER_PER_HEAD	2
#define MAX_VOLTAGENUMBER (MAX_HEAD_NUMBER * VOLTAGE_NUMBER_PER_HEAD)  //For KM512

#define THERMISTOR_NUMBER_PER_HEAD 2
#define MAX_THERMISTORNUMBER MAX_COLOR_NUMBER  //For KM512

//EEPROM saved host parameters

//#define EEPROM_SIZE	4096
#define EEPROM_SIZE	8192 //8K bytes, 20091116

#define MAX_CMD_LEN 32
#define LEN_OFFSET_INCMD 3 //The "LEN" byte offset in command, and LEN must be the first byte in real command
//EEPROM Plan
// 1: u 0~63:  	64 bytes, OEM product setup data(Factory data): 62 bytes used
// 2: n 64~127:	64 bytes, reserved

// 3: 	u 128~191:  64 bytes, BYHX data: 62 bytes used   	
// 4.1: u 192~255:  64 bytes, Heater target data

// 5: n/u 256~511:  256 bytes, User Data
// 		5.1 u 256~319: 64 bytes, voltage, extend to 128 bytes
//		5.2 n 320~383: 64 bytes,  pulse width.	, move this to other area
//		5.3 u 384~415: 32 bytes, Time limited Password.
//		5.4 u 416~423: 4 bytes + 4 bytes, 
//		5.5 n 424~447: 24 bytes, Reserved
//		5.6 u 448~479: 32 bytes, Option password
//		5.7 n 480~511: 32 bytes, Ink limited Password.	

// 6: u 512~3583: 3072 bytes, Save host data: <3072

// 7: n 3584~4063:512-32 bytes. Reserved
//		3584~3647: 64 bytes,  pulse width.	, move from user data
//		3648~3776: 128 bytes, Ink used volume,total 8 colors, the order is decided by Printer manager, see UsedInkVolumeType. unit: pl.

// 8: u 4064~4079:16 bytes Factory Authority flag: 8 bytes used
// 9: u 4080~4095:16 bytes BYHX Authority flag: 8 bytes used

// 10: u 4096~4159:64 bytes, Factory data extended for EPSON calibration.
// 11: u 4160~4224:64 bytes, EPSON calibration configure.
// 11+: u 4224~4288:64 bytes, LCD Menu configure.

// 12: u 5120~6144:1024 bytes, DSP Data.

// 13: u 6144~?? : about 2K bytes. Calibration data. we allocate 2K byte for it.

// 14: u 8192~??: 6K byte, for EPSON motion control.

// 15: u 14336~??: 64byte,

#define EPR_FACTORY_DATA_OFFSET 0
#define ERP_FACTORY_DATA_SIZE 64

#define EPR_BYHX_DATA_OFFSET 128
#define ERP_BYHX_DATA_SIZE 64

#define EPR_HEAT_TEMPRATURE_DATA_OFFSET	192
#define EPR_HEAT_TEMPRATURE_DATA_SIZE	64

#define EPR_VOLTAGE_DATA_OFFSET	256
#define EPR_VOLTAGE_DATA_SIZE	128
//#define EPR_VOLTAGE_DATA_SIZE	64   ///Changed 20090110

#define EPR_FIREPULSEW_DATA_OFFSET	3584 ///Changed 20090110
#define EPR_FIREPULSEW_DATA_SIZE 	64

#define EPR_TIME_PASSWORD_STR_OFFSET	384
#define EPR_TIME_PASSWORD_STR_SIZE	PASSWORD_LEN

#define EPR_INK_PASSWORD_STR_OFFSET	480
#define EPR_INK_PASSWORD_STR_SIZE	PASSWORD_LEN

#define EPR_USED_INK_VOLUME_OFFSET	3648
#define EPR_USED_INK_VOLUME_SIZE		128

#define EPR_LIMIT_TIME_OFFSET	416
#define EPR_LIMIT_TIME_SIZE		8

#define EPR_OPTION_PASSWORD_STR_OFFSET	448
#define EPR_OPTION_PASSWORD_STR_SIZE	PASSWORD_LEN

#define EPR_HOST_PARA_OFFSET 512	
#define EPR_HOST_PARA_MAXLEN (3072 - 8)

#define EPR_RESEVED_OFFSET (4096 - (512-64-128))
#define EPR_RESERVED_SIZE  (512-32-64-128)

#define EPR_FACTORY_AUTHORITY_FLAG_OFFSET (4096-32)
#define EPR_FACTORY_AUTHORITY_FLAG_SIZE 8

#define EPR_BYHX_AUTHORITY_FLAG_OFFSET (4096-16)
#define EPR_BYHX_AUTHORITY_FLAG_SIZE 8

#define EPR_HEAD_VOLTAGE_FLAG 0x350F
#define EPR_HEAD_FIREPULSEW_FLAG 0x360F
#ifdef CONVERSION_BOARD
#define EPR_HEAD_HEATTEMPRATURE_FLAG 0x371F
#else
#define EPR_HEAD_HEATTEMPRATURE_FLAG 0x370F
#endif

//20091116开始，使用4K以上字节
#define EPR_DSP_DISK_OFFSET	5120
#define EPR_DSP_DISK_SIZE	1024

//20101202开始，使用4K~5K和6K~8K之间字节
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#define EPR_FACTORY_DATA_EX_OFFSET 4096
#define ERP_FACTORY_DATA_EX_SIZE 64

#define EPR_CALIBRATION_CONFIG_OFFSET 4160
#define ERP_CALIBRATION_CONFIG_SIZE 64

#define EPR_LCDMENU_SETTING_OFFSET 4224
#define ERP_LCDMENU_SETTING_SIZE 64

#define EPR_CLEANPARAM_DATA_OFFSET	4288
#define EPR_CLEANPARAM_DATA_SIZE 	384

#define EPR_CALIBRATION_SETTING_OFFSET 6144
#define ERP_CALIBRATION_SETTING_SIZE 2048
#endif

#if defined(SUPPORT_MOTOR_CONTROL)||defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
#define EPR_MOTOR_PARAM_OFFSET	8192
#define EPR_MOTOR_PARAM_SIZE	6144
#endif

#if defined( EPSON_BOTTOM_BOARD_V3)||defined( EPSON_BOTTOM_BOARD_V2_1)
#define EPR_HEATER_PARAM_OFFSET	14336
#define EPR_HEATER_PARAM_SIZE	128
#endif

#define EPR_LCDMENU_SETTING_EX_OFFSET 14464
#define ERP_LCDMENU_SETTING_EX_SIZE 256


#define EPR_NEW_UV_SETTING_OFFSET 14720
#define ERP_NEW_UV_SETTING_SIZE 64

#ifdef HEAD_RICOH_G4
#define EPR_REAL_VOLTAGE_OFFSET 14784
#define EPR_REAL_VOLTAGE_OFFSET_SIZE 64
#endif

#define EPR_PRINT_AREA_OFFSET 14848
#define EPR_PRINT_AREA_OFFSET_SIZE 16

#define MAX_HEAD_LIST_LEN	16

//定义喷头种类, 不能改动，跟上位机定义有关
#define	HeadNo_Xaar_126 			1
#define	HeadNo_Xaar_128_360 		2
#define	HeadNo_Xaar_128_PLUS 		3
#define	HeadNo_Xaar_500 			4
#define	HeadNo_Spectra_S_128 		5
#define	HeadNo_Spectra_NOVA		 	6
#define	HeadNo_Spectra_GALAXY	 	7
#define	HeadNo_Konica_KM_512_M 		8
#define	HeadNo_Konica_KM_256_M 		9
#define	HeadNo_Konica_KM_128_M 		10
#define	HeadNo_Konica_KM_512_L 		11
#define	HeadNo_Konica_KM_256_L 		12
#define	HeadNo_Konica_KM_128_L 		13
#define	HeadNo_Xaar_Electron_360 	14
#define	HeadNo_Xaar_Electron_PLUS 	15
#define HeadNo_Xaar_382			 	16
#define HeadNo_Xaar_1001		 	17
#define HeadNo_Konica_New512		18 //No use at last, become to KM512LNX
#define HeadNo_Spectra_Polaris		19 //Polaris 15PL
#define HeadNo_Konica_KM512LNX		20 //35PL
#define HeadNo_Spectra_Polaris_35	21 //Polaris 35PL
#define HeadNo_Epson_Gen5		    22
#define HeadNo_Konica_KM1024_M      23
#define HeadNo_Konica_KM1024_L      24
#define HeadNo_Spectra_Polaris_80	25 //Polaris 80PL
#define HeadNo_Xaar_382_60PL      		26 //20110309aah add

//Tony add below 
#define HeadNo_Konica_KM512MAX_14pl	27
#define HeadNo_Konica_KM512LAX_30pl	28

#define HeadNo_Spectra_Emerald_10pl 29
#define HeadNo_Spectra_Emerald_30pl 30

#define HeadNo_Konica_KM512i_M_12pl 31  ///tony for 
#define HeadNo_Konica_KM512i_L_30pl 32  ///tony for KM512i-MHB
#define HeadNo_Spectra_PolarisColor4_10pl 33
#define HeadNo_Spectra_PolarisColor4_30pl 34
#define HeadNo_Spectra_PolarisColor4_80pl 35

#define HeadNo_Xaar_Proton382_15pl 36

#define HeadNo_Konica_KM512i_MAB_C_15pl =	37,
#define HeadNo_Konica_KM1024i_MHE_13pl = 38,
#define HeadNo_Konica_KM1024S_6pl = 39,

#define HeadNo_Ricoh_Gen4_64Pin     40  //G4P
#define HeadNo_Ricoh_Gen4_G4          41
#define HeadNo_Ricoh_Gen4_G4L        42

#define HeadNo_Ricoh_Gen5_2C_100Pin   47 


#define	HEAD_CATEGORY_XAAR_128		1 //Support 2 & 3 & 14 & 15
#define	HEAD_CATEGORY_KONICA_KM256	2 //Support 9 & 12
#define	HEAD_CATEGORY_SPECTRA		3 //Support 5, 6, 7
#define	HEAD_CATEGORY_KONICA_KM512	4 //Support 8, 0x11
#define	HEAD_CATEGORY_XAAR_382		5 //Support 16
#define HEAD_CATEGORY_KONICA_NEW512 6 //Support 17
#define	HEAD_CATEGORY_SPECTRA_POLARIS	7 
#define HEAD_CATEGORY_EPSON_GEN5	8 //Support 22
#define HEAD_CATEGORY_KONICA_KM1024 9 //Support 23
#define HEAD_CATEGORY_KONICA_WATER 10 //only support water printhead 20110418
#define HEAD_CATEGORY_RICOH        11 //Support 40,41,42
#define HEAD_CATEGORY_RICOH_G5        12 //Support 47

#define	HEAD_BOARD_TYPE_XAAR128_12HEAD  1  //XAAR128 12 head board.
#define	HEAD_BOARD_TYPE_KM256_12HEAD 	2  //Konica KM256 12 head board.
#define HEAD_BOARD_TYPE_SPECTRA			3
#define HEAD_BOARD_TYPE_KM512_8HEAD_8VOL	4
#define HEAD_BOARD_TYPE_KM512_8HEAD_16VOL	5
#define	HEAD_BOARD_TYPE_KM256_8HEAD 	6   //Konica KM256 8 head board.
#define HEAD_BOARD_TYPE_KM512_6HEAD		7	//Konica KM512 6 head board , just for cost
#define HEAD_BOARD_TYPE_KM512_16HEAD	8
#define HEAD_BOARD_TYPE_KM256_16HEAD	9	//Future	
#define HEAD_BOARD_TYPE_SPECTRA_256_GZ	10	//For GZ Spectra NOVA 256
#define	HEAD_BOARD_TYPE_XAAR128_16HEAD  11  //XAAR128 12 head board.
#define	HEAD_BOARD_TYPE_XAAR382_8HEAD   12  //XAAR128 12 head board.
#define HEAD_BOARD_TYPE_KM512_BANNER	13	//4 KM512 
#define HEAD_BOARD_TYPE_NEW512_8HEAD	14
#define HEAD_BOARD_TYPE_SPECTRA_POLARIS_4 15 //For GZ spectra polaris, 2/4 heads
#define HEAD_BOARD_TYPE_SPECTRA_POLARIS_6 16 //For GZ spectra polaris, 6 heads
#define HEAD_BOARD_TYPE_KM512_16HEAD_V2	  17 //KM512 16heads version 2 	
#define HEAD_BOARD_TYPE_SPECTRA_BYHX_V4	  18 //BYHX_spectra V4, 4 polaris
#define HEAD_BOARD_TYPE_SPECTRA_BYHX_V5	  19 //BYHX_spectra V4, 4 polaris
#define HEAD_BOARD_TYPE_SPECTRA_BYHX_V5_8 20 //BYHX_spectra V4, 4 polaris
#define HEAD_BOARD_TYPE_SPECTRA_POLARIS_8 21 //GZ Polaris 8 heads
#define HEAD_BOARD_TYPE_EPSON_GEN5_2HEAD  22 
#define HEAD_BOARD_TYPE_KM1024_8HEAD      23 //KM1024 8 heads
#define HEAD_BOARD_TYPE_KM512_8H_GRAY	24///aah add 20110317km512 8head gray
#define HEAD_BOARD_TYPE_KM512_8H_WATER		25///aah add 20110418 km512 8head WATER
#define HEAD_BOARD_TYPE_KM512_16H_WATER	26///aah add 20110418 km512 8head WATER
#define HEAD_BOARD_TYPE_KM1024_16HEAD	  27 //KM512 16heads version 2 	
#define HEAD_BOARD_TYPE_RICOH_GEN4_64Pin_8H	  28 	
#define HEAD_BOARD_TYPE_RICOH_GEN4_DA_8H	  29 	
#define HEAD_BOARD_TYPE_RICOH_GEN4_PULSE_8H	  30 	
#define HEAD_BOARD_TYPE_RICOH_GEN4_DA_16H	  31 	
#define HEAD_BOARD_TYPE_RICOH_GEN4_PULSE_16H  32
#define HEAD_BOARD_TYPE_EPSON_GEN5_4HEAD  36 
#define HEAD_BOARD_TYPE_RICOH_GEN4_3H	  37 	
#define HEAD_BOARD_TYPE_RICOH_GEN5_3H	  38 	

#define HEAD_BOARD_TYPE_RICOH_GEN5_4H	  39 	

#define PRINT_SPEED_OVER_HIGH	0  //For KM512LN over speed
#define PRINT_SPEED_HIGH	1
#define PRINT_SPEED_MIDDLE	2
#define PRINT_SPEED_LOW		4

#define IS_2COLOR_1HEAD    (factoryData.group_num < 0)
#define FACTORYDATA_LOGIC_GROUP_NUM   (abs(factoryData.group_num))

#ifdef ALLWIN_EPSON_SAME
#define FAST_CLEAN
#endif

#ifdef RIPSTAR_FLAT_EX
#define STEP_NUM_CAL 32
#define EX_MAX_BYTE (STEP_NUM_CAL*sizeof(float))
#endif

EXTERN INT8U Head_Board_Num;
#ifdef CONVERSION_BOARD
#define HEAD_BIT_STATUS 12
#define HEAD_BIT_MASK 0xF
#define MAX_HB_NUM 3

#define MAX_HEAT_CHANNEL_PRE_HB 8
#endif

#ifdef RICOH_G5_3H
#define MAX_TEMP_CHANNEL 3
#elif defined(RICOH_G5_4H)
#define MAX_TEMP_CHANNEL 4
#elif defined(RICOH_3H)
#define MAX_TEMP_CHANNEL 6
#else
#define MAX_TEMP_CHANNEL 16
#endif


#ifdef RICOH_G5_3H
#define MAX_HEAT_CHANNEL 3
#elif defined(RICOH_G5_4H)
#define MAX_HEAT_CHANNEL 4
#elif defined(RICOH_3H)
#define MAX_HEAT_CHANNEL 6
#else
#define MAX_HEAT_CHANNEL 8
#endif

//the following way is error. such as, when FACTORYDATA_LOGIC_GROUP_NUM is 1, FACTORYDATA_PHYSICS_GROUP_NUM will be 0 !!!!.
//  ought to replace it with GetPhysicsHeadNum().
//#define FACTORYDATA_PHYSICS_GROUP_NUM   (IS_2COLOR_1HEAD ? FACTORYDATA_LOGIC_GROUP_NUM/2 : FACTORYDATA_LOGIC_GROUP_NUM)

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#define EPSON_ACTIVE_GROUPNUM	FACTORYDATA_LOGIC_GROUP_NUM
//for ricoh, EPSON_PERMIT_GROUPNUM means RICOH_PERMIT_GROUPNUM.
#define EPSON_PERMIT_GROUPNUM	(factoryDataEx.MaxGroupNumber)
#endif

#ifdef FULGENCY_DEBUG
typedef __packed struct EPR_PRT_SPEED
{
	INT8U Flag; //0xBB
	INT32U VSD1_LOW_SPEED;
	INT32U VSD1_MIDDLE_SPEED;
	INT32U VSD1_HIGH_SPEED;
	INT32U VSD2_LOW_SPEED;
	INT32U VSD2_MIDDLE_SPEED;
	INT32U VSD2_HIGH_SPEED;
}EPR_DSP_PRT_SPEED;

typedef __packed struct EPR_MOVE_SPEED
{
	INT8U Flag; //0xCC
	INT32U X_PRT_BACK_SPEED;
	INT32U Y_PRT_STEP_SPEED;
	INT32U X_MOVE_SPEED;
	INT32U Y_MOVE_SPEED;
	INT32U Z_MOVE_SPEED;
}EPR_DSP_MOVE_SPEED;

#endif

typedef struct sEPM_Head
{
	INT16U flag; //EPR_CLEANPARA_FLAG
	INT16U version;
	INT16U structsize; //sizeof struct CLEANPARA_EPSON_ALLWIN_USB
	INT16U Checksum;
} EPM_Head;

typedef struct MEDIAEDGE
{
	INT32S paper_X_NearEdgePos;
	INT32S paper_X_FarEdgePos;
	INT32S paper_Y_NearEdgePos;
	INT32S paper_Y_FarEdgePos;
}MediaEdgeType;

typedef struct PASSWORD
{
	INT8U m_bHaveTimePwd; 
	INT8U m_bHaveOptionPwd;
	INT8U m_bHaveInkPwd;
	INT8U m_sTimePwd[PASSWORD_LEN+1]; //密码
	INT8U m_sOptionPwd[PASSWORD_LEN+1];
	INT8U m_sInkPwd[PASSWORD_LEN+1];
	//	INT8U saved;
	INT16U m_bTimeLimited; 
	INT8U m_bLanguage; //0: Sim chinese, 1: trad chinese, 3: english, 4: tailand
	INT16U m_bInkLimited; 
}PasswordType;

typedef struct status_EP0Buf_Type{
	INT16U requestCode;
	INT8U  flag; 
	INT8U  percent;
	INT32U status;	
	INT32U statusCode[10];	
	INT8U statusCodeNum;
#ifdef DEBUG_Y_ENCODER	
	INT8U nop[3];
	INT32U step_distance;
	INT32U passAdvance;
#endif	
}status_EP0BUF_TYPE;

typedef struct FWINFO //Actrually FW information
{
	INT16U requestCode;
	INT8U  flag; 
	INT8U  nouse;
	INT32U version; //byte[4]: hardware version, main version, sub version, build,
	INT32U mt_version; //Motion FW version	
	INT16U manufacturerID;
	INT16U productID;
	INT32U boardID; 
	char mfw_date[12];
	char mtfw_date[12];
	char hdfw_date[12]; //Head board date
	INT32U hd_version;
}FWInfoType;

typedef struct CHECKBOARD
{
	INT16U requestCode;
	INT8U  flag; 
	INT8U  uiAlgorithm;
	INT32U uiByteCount;
	INT8U  uiInputValue[8];
	INT8U  desResult[8];
	INT32U swVersion; //0,	main version, sub version, 0
	INT8U  uiInputBuffer[64-28];
}CheckBoardType, *PCheckBoardType;

//We get this 2 info from head board
typedef struct HEADBOARDINFO
{
	INT32U headCategory; //Head Category 是指能用同一个头板的喷头大类
	INT32U headBoardType;
}HeadBoardInfoType;

//EncoderType;
//对于平板+卷对卷机器
//Bit0 X编码器 1: 使用光栅， 0: 使用司服编码
//Bit1 Y编码器 1: CN12A		 0: CN31B  //************对于其它项目，此值设为0********
//Bit5 Y轴使用哪一个PWM  	 0: PWM2 	 1:PWM4 
#define FDF_EPSON_LCD   0x4

#define UV_SETTING_FLAT 0x1
typedef __packed struct EPR_UVSetting
{
	INT8U len;
	INT8U flag;
	float Front_Light_Head0_Length;//cm
	float Back_Light_Head0_Length;
	float Head_Region_length;
	float UV_Light_Length;
	float Advance_Open_Length;
	float Delay_Close_Length;
	INT8U Power[13];
	/*
	1.    前灯功率(24~31BIT)和后灯功率(16~23BIT)，是百分比数值，例如50%就是50，前后灯要根据原点设置判断，例如 右原点左灯是前灯
	
	2.    UV灯开关定义如下：（1打开，0关闭）
	
	0bit：前灯奇数pass方向打印开关，例如：右原点就是左打印，左原点是右打印。
	
	1bit：前灯偶数pass方向打印开关，例如：右原点就是右打印，左原点是左打印。
	
	2bit：后灯奇数pass方向打印开关。
	
	3bit：后灯偶数pass方向打印开关。
	
	4bit：就绪时前灯打开
	
	5bit：就绪时后灯打开
	
	*/	
	INT32U UV_SETTING;
	INT8U reserved[21];
}EPR_FactoryUVSetting_Type;
extern INT8U SaveFactoryUV_Setting(void);
typedef struct EPR_FactoryData
{
	INT8U len;
	INT8U EncoderType; //!0 使用光栅， 0 使用司服编码  //对于卷对卷机器，
	INT8U HeadType;    // Head Type是指具体的某一个喷头种类
	INT8U paper_w_type;
	INT8U color_num;
	INT8S group_num;   //if negative, means two colors share one same head.
	INT8U nop1;		//For Software bug, can be used as other function
	INT8U nope2;	//For software bug, can be used as other function
	
	//Cheney: use the real data structure because embbeded calibration need it.
	//INT8U hostData[8];//No use for FW
	//INT8U hostData1[10];//No use for FW
	float m_fHeadXColorSpace;
	float m_fHeadXGroupSpace;
	float m_fHeadYSpace;
	float m_fHeadAngle;
	INT8U m_nVersion;
	INT8U m_nCrc;
	
	INT8U spot_color1_headnum;  //m_nWhiteInkNum
	INT8U spot_color2_headnum;  //m_nOverCoatInkNum
	
	INT32U m_nBitFlag; //Bit0 : OriginIsRight. Bit3 : EPSON LCD
	INT8U paper_w_left; //unit : mm.
	
	INT8U reserved[29];
	
	INT8U NoUsed1; //unit : mm.
	INT8U NoUsed2; //unit : mm.
}EPR_FactoryDataType, *PEPR_FactoryDataType;

#define PAPER_MEDIA_WIDTH_INCH ((float)((INT32U)factoryData.paper_w_type*100 + factoryData.paper_w_left)/25.4)
#define PAPER_MEDIA_WIDTH_MM ((INT32U)factoryData.paper_w_type*100 + factoryData.paper_w_left)

#if defined( HEAD_EPSON_GEN5) ||  defined(HEAD_RICOH_G4)
#define MAX_NAME_LEN 16
#ifdef HEAD_EPSON_GEN5
#define FACTORYDATA_EX_FLAG 0x2A3D
//#define FACTORYDATA_EX_VER 0x1
#define FACTORYDATA_EX_VER 0x2  //add MaxGroupNumber.
#else
#define FACTORYDATA_EX_FLAG 0x2A3E
#define FACTORYDATA_EX_VER 0x2  //add MaxGroupNumber.
#endif
#define FACTORYDATA_EX_MASK_HEADDIR_IS_POS  0x1
#define FACTORYDATA_EX_DX5E  (0x4)
#define FACTORYDATA_EX_GZ_KEY  (0x8)
#define FACTORYDATA_EX_EPSON_GEN6  (1<<4)
#define FACTORYDATA_EX_RIPSTAR_EX  (1<<5)
typedef struct EPR_FactoryData_Ex
{
	INT8U len;  //current len is sizeof(struct EPR_FactoryData_Ex)
	INT8U version;  //current version is 0x02
	
	INT16U m_nXEncoderDPI;  //720/600 DPI.
	INT32U m_nBitFlagEx; //Bit0 : bHeadDirIsPositive, current dir is positive. Bit1 : bIsWeakSoventPrinter, default is 0.
	INT8U m_nColorOrder[MAX_COLOR_NUMBER]; //content is color code. index is color index in a group.
	//following X axis, the first color's color index is 0....
	
	//有可能超过喷头自身的拼插数。
	// 存在三种情况。
	//  YInterleaveNum 小于 喷头自身的拼插数。是一头两色的例子，如，EPSON 1头八色。
	//  YInterleaveNum 等于 喷头自身的拼插数。是普通的例子，如 EPSON 2头八色（前四色，后四色）。
	//  YInterleaveNum 大于 喷头自身的拼插数。是喷头拼插的例子，如 EPSON 2头4色，Y向拼插成720DPI。
	INT8U YInterleaveNum; //Ｙ向拼插的数目。对于EPSON单头，双四色打印机为2; 8色机器为1. 对于ricoh 单group，应该为2.
	
	//factoryDataEx.LayoutType bit0 == 0, 在X方向的坐标，先Ycontinue，再Yinterleave。
	//factoryDataEx.LayoutType bit0 == 1, 在X方向的坐标，先Yinterleave，再Ycontinue。
	//  内部喷头编号总是，先Ycontinue，再Yinterleave。
	//factoryDataEx.LayoutType bit1 == 1, 对称色序。Y interleave 必须是2的倍数。2012-7-2: still not support fully.
	//factoryDataEx.LayoutType bit2，对称色序的对称方式。当允许多种对称方式的时候（如，EPSON Y 拼插成720DPI)，
	//  0，以最外层为对称单位（在不同的头上颜色是对称的）。1，以最内层为对称单位（在一个头内部颜色是对称的）。
	//  方式0，在消除因覆盖关系导致的色差，效果应该更好。方式1，更容易拼插，两个颜色之间的距离比较窄。
	//factoryDataEx.LayoutType bit3 == 0, Ycontinue group 在X方向是回绕。通常，回绕更节省空间。
	//  例如，EPSON4H/4C，在X向位置只有两个位置，13在一个位置，24在一个位置。34的位置回绕了。
	INT8U LayoutType;   //reserved for special layout. default is 0. 0 means PrintHead is Y continue. 
	
	INT8U ManufacturerName[MAX_NAME_LEN];
	INT8U PrinterName[MAX_NAME_LEN];
	
	INT8U MaxGroupNumber; //New element. for epson, 打印机的最大的喷头数。可以由用户关闭其中的若干组。
	//for ricoh. 打印机的最大的组数。可以由用户关闭其中的若干组。
	
	INT8U Vsd2ToVsd3;      //VSD mode
	INT8U Vsd2ToVsd3_ColorDeep; //ColorDeep
	INT8U Only_Used_1head; //是否开启单头打印
	INT8U Mask_head_used;//那个头是打印头，1: head1，2: head2,  4: head3, 8: head4, 等等
	
	
	INT8U reserved[9 - sizeof(EPM_Head)];
}EPR_FactoryDataExType, *PEPR_FactoryDataExType;
#endif

//Can not change the number oder, refer to SX2 command 0x5D
typedef struct EPR_BYHXData
{
	INT8U len;
	INT8U  hwVersion;
	INT16U manufacturerID;
	INT32U boardID;
	INT8U supportedHeadList[MAX_HEAD_LIST_LEN];
	INT8U Reserved[36]; 
}EPR_BYHXDataType;

typedef struct HOSTPARAS
{
	INT16U length;
	INT8U dirty; //part of the para is changed, but not changed completely
	INT8U saved; //Para is changed, need save to flash memory
	INT8U buf[EPR_HOST_PARA_MAXLEN]; //buffer
	INT8U saving;
}HostParaType, *PHostParaType;

typedef struct VOLTPARAS
{
	INT16U flag; //Alway 0x0F+0x35
	INT8U headUpdated;
	INT8U autoCon;
	INT8U manualData[MAX_VOLTAGENUMBER];  
	INT8U autoBaseData[MAX_VOLTAGENUMBER];
}VoltageParaType, *PVoltageParaType;

typedef struct FIREPULSEWPARAS
{
	INT16U flag; //Alway 0x0F+0x36
	INT16U headUpdated;
	INT8U data[MAX_HEAD_NUMBER];
}FirePulseWParaType, *PFirePulseWParaType;

//For UV printer heater
#ifdef SUPPORT_HEAD_HEATER
typedef struct UVHEATTEMPRATURE
{
	INT16U flag; //Alway 0x0F+0x37
	INT16U headUpdated;
#if defined(HEAD_RICOH_G4)
#ifdef CONVERSION_BOARD
	INT8U  data[MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM + MAX_COLOR_NUMBER];
#else
	INT8U  data[MAX_HEAD_NUMBER + MAX_COLOR_NUMBER];
#endif
#else
#error //need check buf for data used
	//INT8U  data[MAX_HEAD_NUMBER];
#endif
}UVHeatTempratureType, *PUVHeatTempratureType;
#endif
typedef struct LIMITEDTIME
{
	INT32U flag; //0
	INT32U time_elapse;
	INT8U dirty;
}ElapseTimeType, *PElapseTimeType;

typedef struct USEDINKVOLUME
{
	INT32U flag; //0x12345678
	INT32U InkVolume[MAX_COLOR_NUMBER][2];  //unit is 16pl. because 1pl = 10e-15 L, one color need two INT32U.
	INT8U dirty;
	INT16U DotVolume; //pl per dot, based on head type. doesn't consider variable dots.
	INT32U CurrentTotalInk;
}UsedInkVolumeType, *PUsedInkVolumeType;

typedef struct POSITION
{
	INT32S x;
	INT32S y;
	INT32S z;
	INT32S f;
#ifdef SUPPORT_MOTOR_CONTROL_ONLY_STEP
	INT32S z_bak;
	INT32S f_bak;	
#endif
}PositionType, *PPositionType;

typedef struct PHYSICALLENGTH
{
	INT32S x;
	INT32S y;
	INT32S z;
	INT32S f;
}PhysicalLength, *PPhysicalLength;

//#ifdef Y_ENCODER_ENABLE
struct Struct_YEncoderInf
{
	INT32U checksum;   //异或
	INT32S bandIndex;	//Get from band info
	INT32U encoderPos; //Get from motion
	INT32U bandY;   		//Get from band info
	INT32U nextBandY;  //Get from band info
	INT32U bandResY;	//Get from band info   
	float  fStepTotal;
};

struct Struct_AdjustInf
{
	INT32U	checksum;   //异或
	INT32S  passStepValue;  //每PASS的步进值
	INT32S  passAdvance;    //每PASS的PIXEL值
	INT32S  feedBackY;      // 暂时不用
	INT32S  bidirectionValue;
};
//#endif

#ifdef HEAD_XAAR382
typedef struct HEAD_INFO_TYPE
{
	INT32U SerNo; //3 values
	INT16U FirmVer;
	INT8U Type;
	INT16U WfmID1;
	INT16U WfmID2;
	INT16U WfmID3;
	INT16U WfmID4;
}HeadInfoType;


EXTERN HeadInfoType headInfo[MAX_HEAD_NUMBER];
EXTERN INT16U PWM_values[MAX_HEAD_NUMBER];
EXTERN INT8S Vtrim_values[MAX_HEAD_NUMBER];
EXTERN INT8U dualBank_values[MAX_HEAD_NUMBER];
#endif

extern INT8U factoryAuthorityStr[9];
extern INT8U byhxAuthorityStr[9];

extern HostParaType host_paras;
extern FWInfoType fwInfo;
extern INT8U epromDataRead;
extern EPR_FactoryDataType factoryData FAST_DATA_SECTION;
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
extern EPR_FactoryDataExType factoryDataEx;
#endif
extern EPR_BYHXDataType byhxData;
extern HeadBoardInfoType headboardInfo;

extern status_EP0BUF_TYPE status_EP0Buf;
extern volatile INT8S clean_single_indexgl; //Single clean index
extern volatile INT8S move_dir;
EXTERN volatile INT8U move_type;  //USB command different, Print + Panel + 0x22: 0/0x25:1/0x60:0/0x67:3
extern volatile INT32S move_distance;
extern OS_FLAG_GRP *status_FLAG_GRP;
EXTERN volatile INT8U measure_media_method;  

//extern volatile INT32S curPos; //Current X position of the carriadge
EXTERN volatile PositionType curPos;
EXTERN PhysicalLength printerPhysicalSize;
extern MediaEdgeType mediaEdge[MAX_PAPER_VOLUME];
extern INT8S volatile mediaVolNum;
extern CleanParaType cleanPara;
extern PrinterParaType printer; 
extern PasswordType password;

extern OS_FLAG_GRP *status_FLAG_GRP;
extern OS_FLAG_GRP *mix_FLAG_GRP;
extern OS_FLAG_GRP *mix_FLAG_GRP_2;

extern volatile INT8U FPGAOK;
extern volatile INT8U printerFlashing;
extern volatile INT8U cleanParaDirty;  
extern volatile INT8U cleanParaDirty_ToPM;  //for epson , report to PM that cleanPara is Dirty
extern volatile INT32U curBandByteLeft ;

extern volatile INT32U debug_int1;
extern volatile INT32U debug_int2;
extern volatile INT32U debug_int3;

extern volatile INT16U g_FPGADMA_sendPkg ;

extern INT32U g_DSP_FireNumber;
extern INT32U g_FPGA_FireNumber;

extern volatile INT8U headBusUsedByPrintData;

extern INT8U g_USBTransStoped;
extern OS_EVENT *IICSem; //Sem to lock IIC bus for EEPRom
extern OS_EVENT *IIC_KeyboardSem; //Sem to lock IIC bus for keyboard
extern OS_EVENT *CleaningParamSem; //Sem to lock CleaningParam
extern OS_EVENT *CalibrationParamSem; //Sem to lock all Calibration Setting
//extern OS_EVENT *PMCaliSettingSem; //Sem to lock PM Calibration Setting
extern OS_EVENT *LCDMenuConfigureSem; //
extern OS_EVENT *MenuSem; //Sem to lock LCD menu operate

#ifdef COORD_NEW_UV
extern EPR_FactoryUVSetting_Type Ripstar_UV_Setting;
#endif

extern volatile VoltageParaType g_headVoltageData;
extern volatile FirePulseWParaType g_headFirePulseWData;
#ifdef SUPPORT_HEAD_HEATER
EXTERN volatile UVHeatTempratureType g_UVheatTemprature;
#endif
EXTERN INT32U g_uiRandomNumber[4];

EXTERN volatile INT8U g_curHeadVoltage[MAX_VOLTAGENUMBER];
#ifdef CONVERSION_BOARD
EXTERN volatile INT8U g_headTemeprature[MAX_HEAD_NUMBER*MAX_HB_NUM];
#else
EXTERN volatile INT8U g_headTemeprature[MAX_HEAD_NUMBER];
#endif

EXTERN volatile INT8U g_headHeaterThermistorTemeprature[MAX_COLOR_NUMBER]; //加热器的温度传感器的温度

#ifdef HEAD_EPSON_GEN5
#define MAX_EPSON_HEAD_CHANNEL  8
#elif defined(HEAD_RICOH_G4)
#define MAX_EPSON_HEAD_CHANNEL  16  //here, MAX_EPSON_HEAD_CHANNEL means max Head driver channel. In the future, we will support 16 Head.
#endif
EXTERN INT8S g_headParameterPercent[MAX_EPSON_HEAD_CHANNEL];
//EpsonChannelMap is NOT make sense. 
//however, we keep it.
extern INT8U EpsonChannelMap[MAX_EPSON_HEAD_CHANNEL]; 

extern INT8U bMotionCfgDirty;
extern INT8U bDyssMeasureDirty;

extern INT8U bCleanSprayDirty;
EXTERN volatile INT8U dspDebugInfo[32];
EXTERN volatile INT8U dspPrivateData[32];
void Global_Init(void);
INT8U security_CheckTimePassword(void);
INT8U security_CheckOptionPassword(void);
INT8U security_CheckInkPassword(void);
INT16U GetDotVolume(INT8U HeadType);
void FlushInkVolume(INT32U * pInkDotNumber, INT8U bForceFlush);
INT8U CheckInkVolume(void);

INT8U GetPhysicsHeadNum(void);
INT16U GetNozzleCount(INT8U HeadType);
INT8U IsNeed128M(void);

#define INKDOT_CHK_NO_PASS	0
#define INKDOT_CHK_OLD_SW	1
#define INKDOT_CHK_PASS		2
#define INKDOT_CHK_NO_LIMIT	3
INT8U HandShakeInkDot(INT32U * pInkDotNumber);

EXTERN volatile INT32U printedBands;
//#ifdef Y_ENCODER_ENABLE
EXTERN volatile struct Struct_YEncoderInf g_yEncoderInf;
EXTERN volatile struct Struct_AdjustInf g_adjustInf;
//#endif
EXTERN volatile INT8U g_shakehandPassed;

EXTERN volatile UsedInkVolumeType usedInkVolume;

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
EXTERN volatile INT32U FireCount, FireCountEx;
EXTERN volatile INT8U bFPGADebugInfoUpdated;
EXTERN volatile INT8U headFPGADebugInfo[48];
EXTERN volatile INT8U bReadHeadBoardParam;
EXTERN volatile INT16U HBReadAddr, HBReadLen;
EXTERN volatile INT8U HBParamBuf[32];

#define CALI_CONFIG_VERSION 0x1
#define CALI_CONFIG_FLAG    0x3D84
typedef struct CaliConfig
{
	INT8U len;
	INT8U version;
	
	INT8U BaseColor; //color code. it define the title color. 
	//for most calibration, it define calibration base color.
	
	INT8U YStepColor[2]; //current is K,M
	
	INT8U Reserved[59 - sizeof(EPM_Head)];
}CaliConfigType, * P_CaliConfigType;

extern CaliConfigType caliConfig;

#define PM_MAX_HEAD_NUM   32
#if defined(RICOH_VSD2)&&defined(MANUFACTURER_DYSS)
#define PM_MAX_SPEED_NUM  6
#else
#define PM_MAX_SPEED_NUM  4
#endif
#define PM_MAX_PASS_NUM   108
//#define PM_MAX_RESLIST_NUM    5   //the index is 720,360,540,270,1440
#define PM_MAX_RESLIST_NUM    4   //the index is 720,360,540,270 //doesn't support 1440 because PM doesn't support.

typedef struct SCalibrationHorizonSetting
{   //PM's left is from image's left to image's right. 
	//Embedded'e left is from printer's right to printer's left. 
	//For allwin platform, PM's Left is embedded's left.
	char XLeftArray[PM_MAX_HEAD_NUM]; //顺序是包含H0.K0的颜色组，然后是H1.K0,直到最后一个头的K0,再到H0.K1。
	char XRightArray[PM_MAX_HEAD_NUM]; //对齐到Right的K0.
	int  nBidirRevise;
}SCaliHorSettingType;
typedef struct SCalibrationSetting
{
	unsigned short len;
	unsigned short version;
	
	int nStepPerHead;     
	int nPassStepArray[PM_MAX_PASS_NUM];
	
	int nVerticalArray[PM_MAX_HEAD_NUM];
	
	int nLeftAngle;
	int nRightAngle;
	
	SCaliHorSettingType sCalibrationHorizonArray[PM_MAX_RESLIST_NUM][PM_MAX_SPEED_NUM];
}SPMCaliSettingType;

extern SPMCaliSettingType pmCaliSettingFWFormat, pmCaliSettingPMFormat;
extern INT8U bPMCaliSettingDirty;
extern INT8U bCaliSettingDirty;
extern INT8U bCaliStepDirty;
extern INT8U bResistanceDirty;

#ifdef FULGENCY_FUN
#define Y_NORMAL 0
#define Y_GO_HOME 1
#define Y_GO_LIMIT 2

#define Y_GOHOME_IDLE 10
#define Y_GOHOME_FLAG 11
#define Y_GOHOME_CMD 12
#define Y_GOHOME_WAITE 13
extern INT8U Y_GOHOME_Dirty;
extern INT8U Head_Pump_Dirty;
extern INT8U Z_SAFE_Dirty;
extern INT8U FIND_ORIGIN_Dirty;
#endif

enum Cali_Pattern_Type
{
	DotCheck,
	NozzleCheck,
	StepCheck,
	StepCheck_Micro, //only support epson，已经废弃。
	StepVerify, //not implement.
	LeftCheck,
	RightCheck,
	BiDirCheck,
	AngleCheck, //check head angle. not calibration. 
	InterleaveCheck, 
	HeadYIntervalCheck,
	AngleLeftCheck, //only support epson, waiting. 已经废弃。
	AngleRightCheck, //only support epson, waiting. 已经废弃。
	VerticalCheck, //need to check positive/negitive of PM.
	TestPrint, //not implement.
	HWDynOffsetTest, //???, need to check if support ricoh.
	TestStep, //not implement.
	GroupOffsetCheck,
	VerticalAlignCheck  //仅仅检查喷头是否在垂直方向对齐，用于机械校正。
};

struct CaliPrintSetting
{
	/*第一个Byte是VSD Model,合法值是1-4。
	第二个Byte是分辨率,合法值是1-2-3-4-5。含义为720,360,540,270,1440。default是360。
	第三四个Byte是起始位置,单位是上面DPI的一个点。*/
	
	enum Cali_Pattern_Type type;
	
	INT8U VSDModel;
	INT8U DotSetting;
	
	INT8U DPIModel;
	INT16U StartPos;
	
	//for Y Step calibration.
	INT8U MediaType;
	INT8U PassNum;
	
	INT8U Option;
};

EXTERN volatile struct CaliPrintSetting CaliPrintSettingFromUSB;

void InitCleanParam_EPSON_ALLWIN();

void Capping(); //waiting action completed
void StartCapping();
void UnCapping(); //waiting action completed
#if defined(EPSON_CLEAN_INTEGRATE_3_CAP)
void StartUnCapping(INT8U bmove_C_Axis);
#else
void StartUnCapping();
#endif
INT8U RegPostMoveOfUncapping(INT8U dir, INT32S distance);
void RegPostCapping();
INT8U CapPostHandleAfterMoveCompleted();

#define LCD_MENU_CONFIG_FLAG 0x5459
//#define LCD_MENU_CONFIG_VERSION 0x1
//#define LCD_MENU_CONFIG_VERSION 0x2 //add Head_Height_Level.
//#define LCD_MENU_CONFIG_VERSION 0x3 //add Active_HeadMask.
//#define LCD_MENU_CONFIG_VERSION 0x4 //add Auto_PowerOn_Cleaning and Auto_GotoHome
//#define LCD_MENU_CONFIG_VERSION 0x5 //add PlateFanMode.
//#define LCD_MENU_CONFIG_VERSION 0x6 //add Spotcolor Name List.
//#define LCD_MENU_CONFIG_VERSION 0x7 //add AutoStep.
#define LCD_MENU_CONFIG_VERSION 0x8 //add MeasureSensorOffset
#ifdef MANUFACTURER_DYSS
#define PLATEFANMODE_AUTO     0
#define PLATEFANMODE_OPEN       1
#define PLATEFANMODE_OFF        2

#define PLATEFANMODE_STRONG     0//no use
#else
#define PLATEFANMODE_STRONG     0
#define PLATEFANMODE_WEEK       1
#define PLATEFANMODE_OFF        2
#endif

#define AUTO_FLASH_BEFORE_PRINT (INT8U)1
//#define AUTO_FEED_AFTER_CUT (INT8U)2
#define DYSS_MEASURE_PROTECT_SWITCH (INT8U)4
__packed struct LCD_Menu_Configure
{
	INT16U len;
	INT16U version;
	
	float OriginX;
	float MediaWidth;
	float Margin;
	
	INT8U SheetType;
	INT8U SheetIsTransparent;
	
	INT8U AutoCut;
	INT8U AutoMeasure;
	INT8U AutoLoad;
	INT8U AutoLackMediaCheck;
	INT8U AutoCoverCheck;
	INT8U AutoPlateFan;
	INT8U AutoFixCheck;
	
	//Index: 0, OFF; 1, ECLOSION type1(gradient); 2, ECLOSION type2(Wave).
	INT8U PrintQuality;
	
	INT8U LanguageIndex;
	
	INT8U Step_PassIndex;
	
	INT8U TestPrint_DPIIndex;
	INT8U TestPrint_VSDIndex;
	INT8U TestPrint_PassIndex;
	
	INT8U Head_Height_Level; //0: high, 1: low.
	
	INT8U Active_HeadMask; //from now, Active_HeadMask means Active_GroupMask.
	
	INT8U AutoPowerOnClean;
	INT8U AutoGoHome;
	
	INT8U PlateFanMode;  //0: strong, 1: week, 2: off.
	
	INT8U SpotColorName[4];
	
	INT8U AutoStep;//打印前自动步进
	
	INT8U cfg_bit;//打印前自动闪喷
	
	float MeasureSensorOffset;	
	INT8U Reserved[28 - 1 - 4 - 1 - 4 - sizeof(EPM_Head)];
};
EXTERN struct LCD_Menu_Configure LCDMenuConfig;
INT8U SaveLCDMenuConfig();

#ifdef EPSON_FLASH_IDLE
extern INT8U UIAction_FLAH_IDLE(void * pData, int Index);
extern INT8U flash_idle_on;

#endif

#ifdef RIPSTAR_FLAT_EX
#ifdef HEAD_EPSON_GEN5
#define DYSS_Y_PWM_RATIO (epsonCaliSetting_runtime.stepPass.BaseStep)
#elif defined(HEAD_RICOH_G4)

#ifdef RICOH_G5_4H
#define DYSS_Y_PWM_RATIO (epsonCaliSetting_runtime.stepPass.BaseStep*150/320)
#else
#define DYSS_Y_PWM_RATIO (epsonCaliSetting_runtime.stepPass.BaseStep*150/192)
#endif

#else
#error
#endif
#ifdef MANUFACTURER_KINCOLOR_FLAT
#define Y_PLAT_LENGTH 600
#elif defined(MANUFACTURER_TATE_EPSON_UV)
#define Y_PLAT_LENGTH 0
#elif defined(MANUFACTURER_DYSS)
#define Y_PLAT_LENGTH (34188 + LCDMenuConfig_EX.Y_Plat_Correction*DYSS_Y_PWM_RATIO)
#elif defined(MANUFACTURER_LONGKE_FLAT_UV_150)
#define Y_PLAT_LENGTH DYSS_Y_PWM_RATIO*2
#elif defined(MANUFACTURER_LECAI_FLAT_UV_2H)
#define Y_PLAT_LENGTH DYSS_Y_PWM_RATIO*1.25f
#elif defined(MANUFACTURER_ORIC_FLAT)
#define Y_PLAT_LENGTH (DYSS_Y_PWM_RATIO/2.54f)
#else 
#define Y_PLAT_LENGTH DYSS_Y_PWM_RATIO
#endif

#if defined(MANUFACTURER_DYSS)
#define Z_BASE_RES 600
#elif defined(MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)
#define Z_BASE_RES 20320
#elif defined(MANUFACTURER_FULGENCY_FLAT_UV)
#define Z_BASE_RES 25400
#else
#define Z_BASE_RES 10000
#endif

#endif

#ifdef MANUFACTURER_FULGENCY_FLAT_UV
#define Z_PLAT_LENGTH (Z_BASE_RES/25.4*9 + LCDMenuConfig_EX.Z_Plat_Correction*Z_BASE_RES)

#define Z_SAFE_POSITION (Z_BASE_RES*((LCDMenuConfig_EX.OriginZ > 0.15f/2.54f)?LCDMenuConfig_EX.OriginZ:0.15f/2.54f) + Z_PLAT_LENGTH)
#define Z_FLASH_POSITION (Z_BASE_RES/25.4*4)
#define Z_WIPE_POSITION (Z_BASE_RES/25.4*4 + Z_PLAT_LENGTH)
#define Z_SUCK_POSITION (0)
#elif defined(MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)
#define Z_SAFE_POSITION (37584)//数值由客户反馈
#define Z_PLAT_LENGTH 0
#else
#define Z_SAFE_POSITION 0
#define Z_PLAT_LENGTH 0
#endif

#ifdef MANUFACTURER_DYSS	
#define  DYSS_Y_LIMIT 0X1
#define  DYSS_FLASH_AFTER_SUCK 0X2
#endif

#define LCD_MENU_CONFIG_EX_VERSION_1 0x1 //INIT
#define LCD_MENU_CONFIG_EX_VERSION_2 0x2
#define LCD_MENU_CONFIG_EX_VERSION_3 0x3
#define LCD_MENU_CONFIG_EX_VERSION_4 0x4
#define LCD_MENU_CONFIG_EX_VERSION_5 0x5
#define LCD_MENU_CONFIG_EX_VERSION 0x6
__packed struct LCD_Menu_Configure_EX
{
	INT16U len;
	INT16U version;
	float OriginY;
	float MediaL;
	float OriginZ;
	float MediaH;
	INT8U PRINT_Y_CONTINUE;
	INT8U Calc_X_Speed;	
#ifdef MANUFACTURER_DYSS	
	volatile float add[STEP_NUM_CAL];
#endif
//#define LCD_MENU_CONFIG_EX_VERSION 0x2 
	INT8U Color_Bar_Flash_On;
	float Color_Bar_POSI;
	INT8U Move_Paper_Speed_Percent;
//#define LCD_MENU_CONFIG_EX_VERSION 0x3 
	float X_Plat_Correction;
	float Y_Plat_Correction;
//#define LCD_MENU_CONFIG_EX_VERSION 0x4 
	INT8U Head_Encry_Mask;
	INT8U Head_PUMP_Mask;
	INT32U Head_PUMP1_ON;
	INT32U Head_PUMP1_OFF;
	INT32U Head_PUMP2_ON;
	INT32U Head_PUMP2_OFF;
	INT16U Interval_Pump_On_Time;		//配置间歇泵墨，开启（泵墨）时间,unit:ms
	INT16U Interval_Pump_Off_Time;		//配置间歇泵墨，停止（泵墨）时间,unit:ms
//#define LCD_MENU_CONFIG_EX_VERSION 0x5
	/* b_Sundry主要表示一些二值（True/Fause)杂项
	*  bit0: 是否开启“Head carriage move to rear”功能，1：开启，0：不开启
	*/
	INT32U b_Sundry;		
	float Z_Plat_Correction;
};
EXTERN struct LCD_Menu_Configure_EX LCDMenuConfig_EX;
EXTERN INT8U SaveLCDMenuConfig_EX();
EXTERN INT8U ResetMenuConfig_EX();
EXTERN INT8U ReadLCDMenuConfig_EX();
void AdjustPlatSpace();
INT8U ReadFactoryDataEx_EPSON(EPR_FactoryDataExType *pData);
INT8U SaveFactoryDataEx_EPSON(EPR_FactoryDataExType *pData);
INT8U ReadFactoryData(EPR_FactoryDataType *pData);
#endif

#if defined(EPSON_BOTTOM_BOARD_V3) ||defined(EPSON_BOTTOM_BOARD_V2_1) 

#define MAX_HEATER_NUM  4
#define HEATER_PRE_INDEX    0
#define HEATER_MIDDLE_INDEX  1
#define HEATER_POST_INDEX   2
#define TMP_ENVIRONMENT_INDEX   3

#define HEATER_INFO_VERSION 0x1 
#define HEATER_INFO_FLAG    0x9433 
struct sHeaterInfo
{
	INT16U len;
	INT16U version;
	
	INT32U HeaterOnMask; //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
	
	float TargetTemp[MAX_HEATER_NUM];
	float ResistanceOffset[MAX_HEATER_NUM];//temperature modify value.
	
	float adcRefV;  //3.3V
	float adcSharedResistance[MAX_HEATER_NUM]; //120
	
};
extern volatile struct sHeaterInfo HeaterInfo;

#endif

extern volatile INT32U TickAfterLastComm;
#define MAX_TICK_INTO_OFFLINE   10000   //10s
#define IS_ONLINE (TickAfterLastComm < MAX_TICK_INTO_OFFLINE)

#ifdef MICOLOR_AUTOFUCTION
extern INT8U Media_Lack;
extern INT8U Cover_up;
extern INT8U Media_Not_Fixed;
#endif

#ifdef FUNCTION_CLOSE_LCD
//extern unsigned char MenuStr_EM_4_ID1C8[MAX_HEAD_DRIVE_NUM][];
#endif

#undef EXTERN
#endif


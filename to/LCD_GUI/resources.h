#ifndef RESOURCES_H
#define RESOURCES_H
#include "Menu_GUI.h"
#include "calibration.h"
#define CHAR_TABLE_OFFSET (256)
extern struct MenuItem MainScreenRoot;
extern struct MenuItem MainMenuRoot;

#define MAX_HEAD_NUM 8

#ifdef HEAD_EPSON_GEN5

#ifdef EPSON_4H
#define MAX_HEAD_DRIVE_NUM 8
#else
#define MAX_HEAD_DRIVE_NUM 4
#endif

#else
#ifdef RICOH_G5_3H
#define MAX_HEAD_DRIVE_NUM 12
#elif defined(RICOH_G5_4H)
#define MAX_HEAD_DRIVE_NUM 16
#else
#define MAX_HEAD_DRIVE_NUM 8
#endif
#endif
#ifdef CONVERSION_BOARD
#ifdef RICOH_G4_16H
#define MAX_HEAD_DRIVE_NUM 16
#endif

#if defined(HEAD_EPSON_GEN5)||defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
#error //need check
#endif

#endif

#ifdef EPSON_4H
#define MAX_NOZZLE_LINE (MAX_HEAD_NUMBER_CALIBRATION *NOZZLE_LINE_COUNT_PER_HEAD)
#else
#define MAX_NOZZLE_LINE (MAX_HEAD_NUM *2)
#endif
extern struct MenuItem MI_EM_1_ID3F3_Nozzleparameter[];
extern struct MenuItem MI_UM_3_ID3E_Head_Space[4];
extern struct MenuItem MI_EM_3_ID3E_Head_Space[4];
extern struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD1[MAX_HEAD_NUM];
extern struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD2[MAX_HEAD_NUM];
extern struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD3[MAX_HEAD_NUM];
extern struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD4[MAX_HEAD_NUM];

extern struct MenuItem MI_EM_6_ID74_VSD1_CorrectionValue[3];
extern struct MenuItem MI_EM_6_ID74_VSD2_CorrectionValue[3];
extern struct MenuItem MI_EM_6_ID74_VSD3_CorrectionValue[3];
extern struct MenuItem MI_EM_6_ID74_VSD4_CorrectionValue[3];

extern struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD1[MAX_HEAD_NUM];
extern struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD2[MAX_HEAD_NUM];
extern struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD3[MAX_HEAD_NUM];
extern struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD4[MAX_HEAD_NUM];

extern struct MenuItem MI_UM_6_ID74_VSD1_CorrectionValue[3];
extern struct MenuItem MI_UM_6_ID74_VSD2_CorrectionValue[3];
extern struct MenuItem MI_UM_6_ID74_VSD3_CorrectionValue[3];
extern struct MenuItem MI_UM_6_ID74_VSD4_CorrectionValue[3];

//Horizotal alignment 
extern struct MenuItem MI_EM_6_ID74_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID8A_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID9E_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_IDB4_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_IDC8_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_IDDE_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_IDF2_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID108_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID11D_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID133_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID147_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID15D_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID171_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID187_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID19B_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID1B1_CorrectionValue[MAX_NOZZLE_LINE];

extern struct MenuItem MI_EM_6_ID75_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID8B_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID9F_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_IDB5_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_IDC9_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_IDDF_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_IDF3_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID109_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID11E_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID134_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID148_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID15E_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID172_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID188_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID19C_CorrectionValue[MAX_NOZZLE_LINE];
extern struct MenuItem MI_EM_6_ID1B2_CorrectionValue[MAX_NOZZLE_LINE];
//Vertical alignment 
#define MAX_COLOR_NUM   8
extern struct MenuItem MI_EM_3_ID25_CorrectionValue[MAX_COLOR_NUM];
extern struct MenuItem MI_EM_3_ID1C7_CorrectionValue[MAX_COLOR_NUM];
extern struct MenuItem MI_EM_2_ID25_Verticalalignment[2];
//Angle Adjustment
extern struct MenuItem MI_EM_6_ID1D1_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID1D6_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID1DC_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID1E1_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID1E7_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID1EC_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID1F2_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID1F7_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID1FE_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID203_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID209_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID20E_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID214_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID219_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID21F_CorrectionValue[2];
extern struct MenuItem MI_EM_6_ID224_CorrectionValue[2];
extern struct MenuItem MI_EM_1_ID4B_ADJUSTMENT[11];
extern struct MenuItem MI_1_ID20_QUALITY[5];
extern struct MenuItem MI_1_ID3FB_System[];
#ifdef FUNC_WAVE_MAP
extern unsigned char MenuStr_VSD_WaveName_EN[8][8] ;
extern unsigned char MenuStr_VSD_WaveName_CN[8][8] ;
#endif
#ifdef LCD_MODULE_ZX19264M1A
#define MAX_COLOR_NAME  6
extern unsigned char MenuStr_Color_Name[MAX_COLOR_NUM][MAX_COLOR_NAME];
extern unsigned char MenuStr_Color_Name_E[MAX_COLOR_NUM][MAX_COLOR_NAME];

#define MAX_DPI_NAME 14
#define MAX_DPI_NUM 5
extern unsigned char MenuStr_DPI_Name[MAX_DPI_NUM][MAX_DPI_NAME];
extern unsigned char MenuStr_DPI_Name_E[MAX_DPI_NUM][MAX_DPI_NAME];

#define MAX_HOR_NAME 8
#define MAX_HEAD_NUM 8
extern unsigned char MenuStr_LeftRightAlign_Name[MAX_NOZZLE_LINE][MAX_HOR_NAME];
extern unsigned char MenuStr_LeftRightAlign_Name_E[MAX_NOZZLE_LINE][MAX_HOR_NAME];
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)
extern unsigned char MenuStr_Clean_Color_Name[4][MAX_COLOR_NAME];
extern unsigned char MenuStr_Clean_Color_Name_E[4][MAX_COLOR_NAME];
#elif defined(EPSON_4H_CLEAN_SPLIT)
extern unsigned char MenuStr_Clean_Color_Name[4][MAX_COLOR_NAME+2];
extern unsigned char MenuStr_Clean_Color_Name_E[4][MAX_COLOR_NAME+2];
#endif
#define MAX_HEAD_SPACE_NAME 8
extern unsigned char MenuStr_Head_Space_Name[MAX_HEAD_NUM][MAX_HEAD_SPACE_NAME];
extern unsigned char MenuStr_Head_Space_Name_E[MAX_HEAD_NUM][MAX_HEAD_SPACE_NAME];
#define MAX_HEAD_PARAM_NAME 20
extern unsigned char MenuStr_EM_4_ID1C8[MAX_HEAD_DRIVE_NUM][MAX_HEAD_PARAM_NAME];
extern unsigned char MenuStr_EM_4_ID1C8_E[MAX_HEAD_DRIVE_NUM][MAX_HEAD_PARAM_NAME];

extern unsigned char MenuStr_ALLWIN_UM_3_CleanHead1[];
extern unsigned char MenuStr_ALLWIN_UM_3_CleanHead2[];
extern unsigned char MenuStr_ALLWIN_UM_3_CleanHeadAll[];
extern unsigned char MenuStr_ALLWIN_UM_3_CleanHead1_E[];
extern unsigned char MenuStr_ALLWIN_UM_3_CleanHead2_E[];
extern unsigned char MenuStr_ALLWIN_UM_3_CleanHeadAll_E[];

extern unsigned char MenuStr_EM_1_ID3F3_DisableHead1[];
extern unsigned char MenuStr_EM_1_ID3F3_DisableHead2[];
extern unsigned char MenuStr_EM_1_ID3F3_DisableHead1_E[];
extern unsigned char MenuStr_EM_1_ID3F3_DisableHead2_E[];
extern unsigned char MenuStr_EM_1_ID3F3_DisableGroup1[];
extern unsigned char MenuStr_EM_1_ID3F3_DisableGroup2[];
extern unsigned char MenuStr_EM_1_ID3F3_DisableGroup1_E[];
extern unsigned char MenuStr_EM_1_ID3F3_DisableGroup2_E[];

extern unsigned short ErrorCodeCHS[];
extern unsigned char MenuStr_2_ID3FF_Percent[];

extern unsigned short *CHS_SVC_SX2;//在USB chip中服务错误.
extern unsigned short *CHS_SVC_FPGA0;//Service Call: chip1.
extern unsigned short *CHS_SVC_FPGA1;//Service Call: chip2.
extern unsigned short *CHS_SVC_FPGA2;//Service Call: chip3.
extern unsigned short *CHS_SVC_FPGA3;//Service Call: chip4
extern unsigned short *CHS_SVC_UPDATE_FAIL;//Service Call: chip5.
extern unsigned short *CHS_SVC_BYHX_DATA;//Service Call: 6.
extern unsigned short *CHS_SVC_EEPROM_CHK;//BYHX_TOOL check EEPROM 没有通过。
extern unsigned short *CHS_SVC_SDRAM_CHECK;//SDRAM 的Check没有通过
extern unsigned short *CHS_SVC_FLASH;//无法从Falsh读取FPGA. 
extern unsigned short *CHS_SVC_SDRAM_NOMATCH;//SDRAM 太小，不能支持当前的设置。
extern unsigned short *CHS_SVC_READBOARDID_FAIL;//读主板错误。
extern unsigned short *CHS_SVC_MOTION_FAIL;//MOTION初始化失败
extern unsigned short *CHS_SVC_HEADBOARD_FAIL;//初始化头版加密芯片失败
extern unsigned short *CHS_SVC_POWEROFF_MODE;//关机模式，请关闭打印机
extern unsigned short *CHS_FTA_SX2RESET;//USB chip异常重启
extern unsigned short *CHS_FTA_INTERNAL_WRONGHEADER;//内部错误 1.
extern unsigned short *CHS_FTA_INTERNAL_WRONGHEADERSIZE;//内部错误 2.
extern unsigned short *CHS_FTA_INTERNAL_JOBSTARTHEADER;//内部错误 3.
extern unsigned short *CHS_FTA_INTERNAL_BANDDATASIZE;//内部错误 4.
extern unsigned short *CHS_FTA_INTERNAL_WRONGFORMAT;//内部错误 5.
extern unsigned short *CHS_FTA_INTERNAL_DMA0WORKING;//内部错误 6.
extern unsigned short *CHS_FTA_INTERNAL_PRINTPOINT;//内部错误 7.
extern unsigned short *CHS_FTA_INTERNAL_OLIMIT;//内部错误 8.
extern unsigned short *CHS_FTA_INTERNAL_OPPLIMIT;//内部错误 9.
extern unsigned short *CHS_FTA_DSPINITS1;//运动step 1.
extern unsigned short *CHS_FTA_DSPINITS2;//运动step 2.
extern unsigned short *CHS_FTA_HEADINITS1;//头板step1.
extern unsigned short *CHS_FTA_HEADINITS2;//头板step2.
extern unsigned short *CHS_FTA_HEADTOMAINROAD;//LVDS错误.
extern unsigned short *CHS_FTA_INTERNAL_BANDDIRECTION;//Band定义中的方向值超出定义
extern unsigned short *CHS_FTA_DSPUPDATE_FAIL;//DSP升级失败.
extern unsigned short *CHS_FTA_EEPROM_READ;//EEPROM读取失败	
extern unsigned short *CHS_FTA_EEPROM_WRITE;//EEPROM写入失败
extern unsigned short *CHS_FTA_FACTORY_DATA;//板子出厂没有初始化.
extern unsigned short *CHS_FTA_HEADBOARD_RESET;//头板复位.
extern unsigned short *CHS_FTA_SPECTRAHVBINITS1;//Spectra高压板无法在第1步初始化.
extern unsigned short *CHS_FTA_PRINTHEAD_NOTMATCH;//头板与出厂数据不匹配,请重置出厂数据或者更换头板.
extern unsigned short *CHS_FTA_MANUFACTURERID_NOTMATCH;//板子的厂商ID不匹配,请升级固件. 
extern unsigned short *CHS_FTA_LIMITEDTIME_RUNOUT;//严重错误: 超过限时时间，请输入新的密码.
extern unsigned short *CHS_FTA_INTERNAL_SW1;//内部错误,空白Band或Y向距离为零.
extern unsigned short *CHS_FTA_USB1_USB1CONNECT;//USB1.1连接.
extern unsigned short *CHS_FTA_UILANGUAGE_NOT_MATCH;//UI使用的语言与权限不符， 请重新设置语言或者重新输入密码
extern unsigned short *CHS_FTA_INTERNAL_WRONGPINCONF;//PINCONF写入FPGA错误
extern unsigned short *CHS_FTA_FACTORY_COLORNUM_WRONG;//工厂设置中写入的颜色数错误.
extern unsigned short *CHS_FTA_HB_EEPROM_WRT_ERR;//头板的EEPROM写入失败
extern unsigned short *CHS_FTA_HB_OVERHEAT;//喷头过热，所有加热被强行关闭。
extern unsigned short *CHS_FTA_SHAKEHAND_ERR;//软件错误或者版本错误，没有通过握手协议。
extern unsigned short *CHS_FTA_SWVERSION_ERR;//固件要求特殊的软件版本，版本错误。
extern unsigned short *CHS_FTA_NOT_SUPPORT_2C_SHARE_1H;//当前系统设置不支持一头两色。
extern unsigned short *CHS_FTA_LIMITEDINK_RUNOUT;//超过墨水用量限制，请重新输入口令
extern unsigned short *CHS_FTA_SWVERSION_ERR_INK_PWD;//固件要求软件版本支持墨水密码，版本错误，
extern unsigned short *CHS_FTA_EPSON_HEAD_BAD;//读板子ID错误。
extern unsigned short *CHS_FTA_EPSON_HEAD_FPGA_ERR;//头版FPGA错误.
extern unsigned short *CHS_FTA_SECURITY_CHIP_ERR;//写主板错误.
extern unsigned short *CHS_FTA_CALIBRATION_DATA;//出厂初始化设置出错。
extern unsigned short *CHS_FTA_PRINT_DATA_ERROR;//打印数据传输错误或丢失
extern unsigned short *CHS_FTA_FACTORY_DATA_ERROR;//FACTORY_DATA 数据错误
extern unsigned short *CHS_FTA_WASTE_INK_FULL;//废墨溢出
extern unsigned short *CHS_ERR_EP0OVERUN_SETUPDATA;//EP0命令被打断
extern unsigned short *CHS_ERR_UART1_TXTIMEOUT;//头板COM超时.
extern unsigned short *CHS_ERR_UART2_TXTIMEOUT;//运动COM超时.
extern unsigned short *CHS_ERR_INTERNAL_PRINTDATA;//打印数据和点火频率不匹配.
extern unsigned short *CHS_ERR_FPGA_LESSDATA;//打印数据小于点火频率
extern unsigned short *CHS_ERR_FPGA_ULTRADATA;//打印数据超出点火频率.
extern unsigned short *CHS_ERR_FPGA_WRONGCHS;//打印的数据和开火数不匹配.
extern unsigned short *CHS_ERR_UV_CTR_WRONGCHS;//内部状态.
extern unsigned short *CHS_ERR_FPGA_WRONGCOMMAND;//FPGA 收到错误的命令
extern unsigned short *CHS_ERR_MOTION_SPEED_BAD;//运动速度太快或不均匀，数据还在出书，却有点火命令
extern unsigned short *CHS_ERR_INTERNAL_MOVING_TIMEOUT;//运动命令发出30秒后，没有收到运动完成命令
extern unsigned short *CHS_ERR_INTERNAL_WRONGAUTOCLEAN;//错误的AUTOCLEAN命令.
extern unsigned short *CHS_ERR_INTERNAL_WRONGBANDINFO;//错误的AUTOCLEAN命令.
extern unsigned short *CHS_ERR_INTERNAL_DEBUG1;//调试状态1
extern unsigned short *CHS_ERR_INTERNAL_DEBUG2;//调试状态2
extern unsigned short *CHS_INI_ARM;//正在初始化ARM.
extern unsigned short *CHS_INI_SX2;//正在初始化USB
extern unsigned short *CHS_INI_FPGA;//正在初始化FPGA
extern unsigned short *CHS_INI_DSP;//正在初始化DSP.
extern unsigned short *CHS_INI_HEADBOARD;//正在初始化头板
extern unsigned short *CHS_INI_HVB;//安装 Spectra 高压板.
extern unsigned short *CHS_WAR_UNKNOWNHEADERTYPE;//不知道的警告
extern unsigned short *CHS_WAR_EP0OVERUN_REQUEST_IGNORE;//EP0溢出.
extern unsigned short *CHS_WAR_PUMP_CYAN;//兰色正在泵墨.
extern unsigned short *CHS_WAR_PUMP_MAGENTA;//品色正在泵墨.
extern unsigned short *CHS_WAR_PUMP_YELLOW;//黄色正在泵墨.
extern unsigned short *CHS_WAR_PUMP_BLACK;//黑色正在泵墨.
extern unsigned short *CHS_WAR_PUMP_LIGHTCYAN;//浅兰色正在泵墨.
extern unsigned short *CHS_WAR_PUMP_LIGHTMAGENTA;//浅品色正在泵墨.
extern unsigned short *CHS_WAR_TIME_PASSWORD_WRONGINPUT;//输入限时密码错误，含有含有非法字符; 
extern unsigned short *CHS_WAR_TIME_PASSWORD_WRONG;//不是一个合法限时密码; 
extern unsigned short *CHS_WAR_TIME_PASSWORD_MANUFACTURERIDNOTMATCH;//限时密码和厂商不匹配
extern unsigned short *CHS_WAR_TIME_PASSWORD_BOARDIDNOTMATCH;//限时密码和板子不匹配
extern unsigned short *CHS_WAR_LIMITEDTIME_FIRST;//第一次警告:再有100个小时就超出限时了, 请向厂家索取密码.
extern unsigned short *CHS_WAR_LIMITEDTIME_SECOND;//第二次警告: 再有50个小时就超出限时了,请向厂家索取密码.
extern unsigned short *CHS_WAR_LIMITEDTIME_LAST;//最后一次警告: 再有1个小时就超出限时并停止打印, 请立即向厂家索取密码.
extern unsigned short *CHS_WAR_OPTION_PASSWORD_WRONGINPUT;//输入选配密码错误，含有非0~F的值
extern unsigned short *CHS_WAR_OPTION_PASSWORD_WRONG;//不是一个合法选配密码
extern unsigned short *CHS_WAR_OPTION_PASSWORD_MANUFACTURERIDNOTMATCH;//选配密码和厂商不匹配
extern unsigned short *CHS_WAR_OPTION_PASSWORD_BOARDIDNOTMATCH;//选配密码和板子不匹配
extern unsigned short *CHS_WAR_PUMP_PROHIBIT_SENSOR;//泵墨安全瓶满了, 所有墨泵停止供墨.
extern unsigned short *CHS_WAR_OVER_SPEED_NOT_SUPPORT;//试图在不支持超频的头板版本上使用超频, 系统强制使用高速打印.
extern unsigned short *CHS_WAR_INK_PASSWORD_WRONGINPUT;//输入墨水密码错误，含有非0~F的值
extern unsigned short *CHS_WAR_INK_PASSWORD_WRONG;//不是一个合法墨水密码
extern unsigned short *CHS_WAR_INK_PASSWORD_MANUFACTURERIDNOTMATCH;//墨水密码和厂商不匹配
extern unsigned short *CHS_WAR_INK_PASSWORD_BOARDIDNOTMATCH;//墨水密码和板子不匹配
extern unsigned short *CHS_WAR_LIMITEDINK_FIRST;//再用10升就超出墨水限制了，第一次警告
extern unsigned short *CHS_WAR_LIMITEDINK_SECOND;//再用5升就超出墨水限制了，第二次警告
extern unsigned short *CHS_WAR_LIMITEDINK_LAST;//再用1升就超出墨水限制了，最后一次警告
extern unsigned short *CHS_WAR_INK_PASSWORD_UNKNOWN_DOT_VOLUME;//当前喷头不支持墨水密码功能.
extern unsigned short *CHS_WAR_PUMP_SPOTCOLOR1;//专色1正在泵墨.
extern unsigned short *CHS_WAR_PUMP_SPOTCOLOR2;//专色2正在泵墨.
extern unsigned short *CHS_WAR_FPGA_LESSDATA;//打印数据小于点火数.
extern unsigned short *CHS_WAR_FPGA_ULTRADATA;//打印数据多于点火数.
extern unsigned short *CHS_WAR_EPSONHEAD_TEMP_TOO_LOW;//喷头温度太低.
extern unsigned short *CHS_WAR_EPSONHEAD_TEMP_TOO_HIGH;//喷头温度太高.
extern unsigned short *CHS_WAR_EPSONHEAD_NOT_CONNECT;//喷头未连接.
extern unsigned short *CHS_WAR_EPSON_FPGA_ULTRADATA;//主板FPGA报告的数据太多.
extern unsigned short *CHS_WAR_EPSON_FPGA_LESSDATA;//主板FPGA报告的数据太少.
extern unsigned short *CHS_WAR_EPSON_FPGA_ERR;//头板FPGA错误
extern unsigned short *CHS_WAR_EPSON_LVDS_ERR;//LVDS失锁
extern unsigned short *CHS_WAR_EPSON_REPORT_LVDS_ERR;//LVDS失锁
extern unsigned short *CHS_WAR_UNKNOWN_NOZZLE_COUNT;//当前喷头的喷嘴数未知。
extern unsigned short *CHS_UPDATE_SUCCESS;//升级成功.
extern unsigned short *CHS_UPDATEERR_DSP_BEGIN_TIMEOUT;//DSP开始升级命令超时.
extern unsigned short *CHS_UPDATEERR_DSP_DATA_TIMEOUT;//DSP数据超时.
extern unsigned short *CHS_UPDATEERR_DSP_END_TIMEOUT;//DSP结束超时.
extern unsigned short *CHS_UPDATEERR_ILIGALFILE;//错误的升级文件格式.
extern unsigned short *CHS_UPDATEERR_INTERNAL_DATA;//不合法的升级文件.
extern unsigned short *CHS_UPDATEERR_CHECKSUM;//升级校验码错误.
extern unsigned short *CHS_UPDATEERR_EREASE;//ARM 闪存擦除.
extern unsigned short *CHS_UPDATEERR_FLASHOP;//Flash错误.
extern unsigned short *CHS_UPDATEERR_GZ_ILIGALFILE;//需要特别固件。
extern unsigned short *CHS_UPDATEERR_INKP_ILIGALFILE;//需要特殊墨水密码固件。
extern unsigned short *CHS_UPDATEERR_16H_ILIGALFILE;//8头硬件无法升级16头的固件。
extern unsigned short *CHS_UNKNOWN_ERR;//未知的错误

extern unsigned short *CHS_STATUS_SVC;//错误
extern unsigned short *CHS_STATUS_FTA;//错误
extern unsigned short *CHS_STATUS_ERR;//错误
extern unsigned short *CHS_STATUS_ERR_AUTO;//错误
extern unsigned short *CHS_STATUS_WAR;//错误
extern unsigned short *CHS_STATUS_INI;//初始化
extern unsigned short *CHS_STATUS_RDY;//就绪
#ifdef FUNC_MAINTAIN
extern unsigned short *CHS_STATUS_MAINTAIN;//维护
#endif
extern unsigned short *CHS_STATUS_MOVING;//移动
extern unsigned short *CHS_STATUS_PRINT;//打印
extern unsigned short *CHS_STATUS_PAUSE;//暂停 
extern unsigned short *CHS_STATUS_PAUSE_1;//暂停 
extern unsigned short *CHS_STATUS_PAUSE_2;//暂停 
extern unsigned short *CHS_STATUS_PAUSE_3;//暂停 
extern unsigned short *CHS_STATUS_CLEANING;//清洗
extern unsigned short *CHS_STATUS_CANCELING;//取消
extern unsigned short *CHS_STATUS_UPDATING;//升级
#else
extern unsigned short ErrorCodeCHS[];
extern unsigned short MenuStr_2_ID3FF_Percent[];

extern unsigned short CHS_SVC_SX2[];//在USB chip中服务错误.
extern unsigned short CHS_SVC_FPGA0[];//Service Call: chip1.
extern unsigned short CHS_SVC_FPGA1[];//Service Call: chip2.
extern unsigned short CHS_SVC_FPGA2[];//Service Call: chip3.
extern unsigned short CHS_SVC_FPGA3[];//Service Call: chip4
extern unsigned short CHS_SVC_UPDATE_FAIL[];//Service Call: chip5.
extern unsigned short CHS_SVC_BYHX_DATA[];//Service Call: 6.
extern unsigned short CHS_SVC_EEPROM_CHK[];//BYHX_TOOL check EEPROM 没有通过。
extern unsigned short CHS_SVC_SDRAM_CHECK[];//SDRAM 的Check没有通过
extern unsigned short CHS_SVC_FLASH[];//无法从Falsh读取FPGA. 
extern unsigned short CHS_SVC_SDRAM_NOMATCH[];//SDRAM 太小，不能支持当前的设置。
extern unsigned short CHS_SVC_READBOARDID_FAIL[];//读主板错误。
extern unsigned short CHS_FTA_SX2RESET[];//USB chip异常重启
extern unsigned short CHS_FTA_INTERNAL_WRONGHEADER[];//内部错误 1.
extern unsigned short CHS_FTA_INTERNAL_WRONGHEADERSIZE[];//内部错误 2.
extern unsigned short CHS_FTA_INTERNAL_JOBSTARTHEADER[];//内部错误 3.
extern unsigned short CHS_FTA_INTERNAL_BANDDATASIZE[];//内部错误 4.
extern unsigned short CHS_FTA_INTERNAL_WRONGFORMAT[];//内部错误 5.
extern unsigned short CHS_FTA_INTERNAL_DMA0WORKING[];//内部错误 6.
extern unsigned short CHS_FTA_INTERNAL_PRINTPOINT[];//内部错误 7.
extern unsigned short CHS_FTA_INTERNAL_OLIMIT[];//内部错误 8.
extern unsigned short CHS_FTA_INTERNAL_OPPLIMIT[];//内部错误 9.
extern unsigned short CHS_FTA_DSPINITS1[];//运动step 1.
extern unsigned short CHS_FTA_DSPINITS2[];//运动step 2.
extern unsigned short CHS_FTA_HEADINITS1[];//头板step1.
extern unsigned short CHS_FTA_HEADINITS2[];//头板step2.
extern unsigned short CHS_FTA_HEADTOMAINROAD[];//LVDS错误.
extern unsigned short CHS_FTA_INTERNAL_BANDDIRECTION[];//Band定义中的方向值超出定义
extern unsigned short CHS_FTA_DSPUPDATE_FAIL[];//DSP升级失败.
extern unsigned short CHS_FTA_EEPROM_READ[];//EEPROM读取失败	
extern unsigned short CHS_FTA_EEPROM_WRITE[];//EEPROM写入失败
extern unsigned short CHS_FTA_FACTORY_DATA[];//板子出厂没有初始化.
extern unsigned short CHS_FTA_HEADBOARD_RESET[];//头板复位.
extern unsigned short CHS_FTA_SPECTRAHVBINITS1[];//Spectra高压板无法在第1步初始化.
extern unsigned short CHS_FTA_PRINTHEAD_NOTMATCH[];//头板与出厂数据不匹配,请重置出厂数据或者更换头板.
extern unsigned short CHS_FTA_MANUFACTURERID_NOTMATCH[];//板子的厂商ID不匹配,请升级固件. 
extern unsigned short CHS_FTA_LIMITEDTIME_RUNOUT[];//严重错误: 超过限时时间，请输入新的密码.
extern unsigned short CHS_FTA_INTERNAL_SW1[];//内部错误,空白Band或Y向距离为零.
extern unsigned short CHS_FTA_USB1_USB1CONNECT[];//USB1.1连接.
extern unsigned short CHS_FTA_UILANGUAGE_NOT_MATCH[];//UI使用的语言与权限不符， 请重新设置语言或者重新输入密码
extern unsigned short CHS_FTA_INTERNAL_WRONGPINCONF[];//PINCONF写入FPGA错误
extern unsigned short CHS_FTA_FACTORY_COLORNUM_WRONG[];//工厂设置中写入的颜色数错误.
extern unsigned short CHS_FTA_HB_EEPROM_WRT_ERR[];//头板的EEPROM写入失败
extern unsigned short CHS_FTA_HB_OVERHEAT[];//喷头过热，所有加热被强行关闭。
extern unsigned short CHS_FTA_SHAKEHAND_ERR[];//软件错误或者版本错误，没有通过握手协议。
extern unsigned short CHS_FTA_SWVERSION_ERR[];//固件要求特殊的软件版本，版本错误。
extern unsigned short CHS_FTA_NOT_SUPPORT_2C_SHARE_1H[];//当前系统设置不支持一头两色。
extern unsigned short CHS_FTA_LIMITEDINK_RUNOUT[];//超过墨水用量限制，请重新输入口令
extern unsigned short CHS_FTA_SWVERSION_ERR_INK_PWD[];//固件要求软件版本支持墨水密码，版本错误，
extern unsigned short CHS_FTA_EPSON_HEAD_BAD[];//读板子ID错误。
extern unsigned short CHS_FTA_EPSON_HEAD_FPGA_ERR[];//头版FPGA错误.
extern unsigned short CHS_FTA_SECURITY_CHIP_ERR[];//写主板错误.
extern unsigned short CHS_FTA_CALIBRATION_DATA[];//出厂初始化设置出错。
extern unsigned short CHS_ERR_EP0OVERUN_SETUPDATA[];//EP0命令被打断
extern unsigned short CHS_ERR_UART1_TXTIMEOUT[];//头板COM超时.
extern unsigned short CHS_ERR_UART2_TXTIMEOUT[];//运动COM超时.
extern unsigned short CHS_ERR_INTERNAL_PRINTDATA[];//打印数据和点火频率不匹配.
extern unsigned short CHS_ERR_FPGA_LESSDATA[];//打印数据小于点火频率
extern unsigned short CHS_ERR_FPGA_ULTRADATA[];//打印数据超出点火频率.
extern unsigned short CHS_ERR_FPGA_WRONGCHS[];//打印的数据和开火数不匹配.
extern unsigned short CHS_ERR_UV_CTR_WRONGCHS[];//内部状态.
extern unsigned short CHS_ERR_FPGA_WRONGCOMMAND[];//FPGA 收到错误的命令
extern unsigned short CHS_ERR_MOTION_SPEED_BAD[];//运动速度太快或不均匀，数据还在出书，却有点火命令
extern unsigned short CHS_ERR_INTERNAL_MOVING_TIMEOUT[];//运动命令发出30秒后，没有收到运动完成命令
extern unsigned short CHS_ERR_INTERNAL_WRONGAUTOCLEAN[];//错误的AUTOCLEAN命令.
extern unsigned short CHS_ERR_INTERNAL_WRONGBANDINFO[];//错误的AUTOCLEAN命令.
extern unsigned short CHS_ERR_INTERNAL_DEBUG1[];//调试状态1
extern unsigned short CHS_ERR_INTERNAL_DEBUG2[];//调试状态2
extern unsigned short CHS_INI_ARM[];//正在初始化ARM.
extern unsigned short CHS_INI_SX2[];//正在初始化USB
extern unsigned short CHS_INI_FPGA[];//正在初始化FPGA
extern unsigned short CHS_INI_DSP[];//正在初始化DSP.
extern unsigned short CHS_INI_HEADBOARD[];//正在初始化头板
extern unsigned short CHS_INI_HVB[];//安装 Spectra 高压板.
extern unsigned short CHS_WAR_UNKNOWNHEADERTYPE[];//不知道的警告
extern unsigned short CHS_WAR_EP0OVERUN_REQUEST_IGNORE[];//EP0溢出.
extern unsigned short CHS_WAR_PUMP_CYAN[];//兰色正在泵墨.
extern unsigned short CHS_WAR_PUMP_MAGENTA[];//品色正在泵墨.
extern unsigned short CHS_WAR_PUMP_YELLOW[];//黄色正在泵墨.
extern unsigned short CHS_WAR_PUMP_BLACK[];//黑色正在泵墨.
extern unsigned short CHS_WAR_PUMP_LIGHTCYAN[];//浅兰色正在泵墨.
extern unsigned short CHS_WAR_PUMP_LIGHTMAGENTA[];//浅品色正在泵墨.
extern unsigned short CHS_WAR_TIME_PASSWORD_WRONGINPUT[];//输入限时密码错误，含有含有非法字符; 
extern unsigned short CHS_WAR_TIME_PASSWORD_WRONG[];//不是一个合法限时密码; 
extern unsigned short CHS_WAR_TIME_PASSWORD_MANUFACTURERIDNOTMATCH[];//限时密码和厂商不匹配
extern unsigned short CHS_WAR_TIME_PASSWORD_BOARDIDNOTMATCH[];//限时密码和板子不匹配
extern unsigned short CHS_WAR_LIMITEDTIME_FIRST[];//第一次警告:再有100个小时就超出限时了, 请向厂家索取密码.
extern unsigned short CHS_WAR_LIMITEDTIME_SECOND[];//第二次警告: 再有50个小时就超出限时了,请向厂家索取密码.
extern unsigned short CHS_WAR_LIMITEDTIME_LAST[];//最后一次警告: 再有1个小时就超出限时并停止打印, 请立即向厂家索取密码.
extern unsigned short CHS_WAR_OPTION_PASSWORD_WRONGINPUT[];//输入选配密码错误，含有非0~F的值
extern unsigned short CHS_WAR_OPTION_PASSWORD_WRONG[];//不是一个合法选配密码
extern unsigned short CHS_WAR_OPTION_PASSWORD_MANUFACTURERIDNOTMATCH[];//选配密码和厂商不匹配
extern unsigned short CHS_WAR_OPTION_PASSWORD_BOARDIDNOTMATCH[];//选配密码和板子不匹配
extern unsigned short CHS_WAR_PUMP_PROHIBIT_SENSOR[];//泵墨安全瓶满了, 所有墨泵停止供墨.
extern unsigned short CHS_WAR_OVER_SPEED_NOT_SUPPORT[];//试图在不支持超频的头板版本上使用超频, 系统强制使用高速打印.
extern unsigned short CHS_WAR_INK_PASSWORD_WRONGINPUT[];//输入墨水密码错误，含有非0~F的值
extern unsigned short CHS_WAR_INK_PASSWORD_WRONG[];//不是一个合法墨水密码
extern unsigned short CHS_WAR_INK_PASSWORD_MANUFACTURERIDNOTMATCH[];//墨水密码和厂商不匹配
extern unsigned short CHS_WAR_INK_PASSWORD_BOARDIDNOTMATCH[];//墨水密码和板子不匹配
extern unsigned short CHS_WAR_LIMITEDINK_FIRST[];//再用10升就超出墨水限制了，第一次警告
extern unsigned short CHS_WAR_LIMITEDINK_SECOND[];//再用5升就超出墨水限制了，第二次警告
extern unsigned short CHS_WAR_LIMITEDINK_LAST[];//再用1升就超出墨水限制了，最后一次警告
extern unsigned short CHS_WAR_INK_PASSWORD_UNKNOWN_DOT_VOLUME[];//当前喷头不支持墨水密码功能.
extern unsigned short CHS_WAR_PUMP_SPOTCOLOR1[];//专色1正在泵墨.
extern unsigned short CHS_WAR_PUMP_SPOTCOLOR2[];//专色2正在泵墨.
extern unsigned short CHS_WAR_FPGA_LESSDATA[];//打印数据小于点火数.
extern unsigned short CHS_WAR_FPGA_ULTRADATA[];//打印数据多于点火数.
extern unsigned short CHS_WAR_EPSONHEAD_TEMP_TOO_LOW[];//喷头温度太低.
extern unsigned short CHS_WAR_EPSONHEAD_TEMP_TOO_HIGH[];//喷头温度太高.
extern unsigned short CHS_WAR_EPSONHEAD_NOT_CONNECT[];//喷头未连接.
extern unsigned short CHS_WAR_EPSON_FPGA_ULTRADATA[];//主板FPGA报告的数据太多.
extern unsigned short CHS_WAR_EPSON_FPGA_LESSDATA[];//主板FPGA报告的数据太少.
extern unsigned short CHS_WAR_UNKNOWN_NOZZLE_COUNT[];//当前喷头的喷嘴数未知。
extern unsigned short CHS_UPDATE_SUCCESS[];//升级成功.
extern unsigned short CHS_UPDATEERR_DSP_BEGIN_TIMEOUT[];//DSP开始升级命令超时.
extern unsigned short CHS_UPDATEERR_DSP_DATA_TIMEOUT[];//DSP数据超时.
extern unsigned short CHS_UPDATEERR_DSP_END_TIMEOUT[];//DSP结束超时.
extern unsigned short CHS_UPDATEERR_ILIGALFILE[];//错误的升级文件格式.
extern unsigned short CHS_UPDATEERR_INTERNAL_DATA[];//不合法的升级文件.
extern unsigned short CHS_UPDATEERR_CHECKSUM[];//升级校验码错误.
extern unsigned short CHS_UPDATEERR_EREASE[];//ARM 闪存擦除.
extern unsigned short CHS_UPDATEERR_FLASHOP[];//Flash错误.
extern unsigned short CHS_UPDATEERR_GZ_ILIGALFILE[];//需要特别固件。
extern unsigned short CHS_UPDATEERR_INKP_ILIGALFILE[];//需要特殊墨水密码固件。
extern unsigned short CHS_UPDATEERR_16H_ILIGALFILE[];//8头硬件无法升级16头的固件。
extern unsigned short CHS_UNKNOWN_ERR[];//未知的错误

extern unsigned short CHS_STATUS_SVC[];//错误
extern unsigned short CHS_STATUS_FTA[];//错误
extern unsigned short CHS_STATUS_ERR[];//错误
extern unsigned short CHS_STATUS_ERR_AUTO[];//错误
extern unsigned short CHS_STATUS_WAR[];//错误
extern unsigned short CHS_STATUS_INI[];//初始化
extern unsigned short CHS_STATUS_RDY[];//就绪
extern unsigned short CHS_STATUS_MOVING[];//移动
extern unsigned short CHS_STATUS_PRINT[];//打印
extern unsigned short CHS_STATUS_PAUSE[];//暂停 
extern unsigned short CHS_STATUS_CLEANING[];//清洗
extern unsigned short CHS_STATUS_CANCELING[];//取消
extern unsigned short CHS_STATUS_UPDATING[];//升级
#endif
//MainInfo field
extern CHAR MainInfo_MediaWidth[8];      //value is float *. unit is inch.
extern CHAR MainInfo_PrintOrigin[8];     //value is float *. unit is inch.
#ifdef RIPSTAR_FLAT_EX
extern CHAR MainInfo_MediaLength[8];      //value is float *. unit is inch.
extern CHAR MainInfo_YOrigin[8];     //value is float *. unit is inch.
extern CHAR MainInfo_MediaHigh[8];      //value is float *. unit is inch.
extern CHAR MainInfo_ZOrigin[8];     //value is float *. unit is inch.
#endif
extern CHAR MainInfo_PassNum[8];         //value is INT8U *. 
extern CHAR MainInfo_Resolution[8];      //value is INT16U *. 
extern CHAR MainInfo_PrinterName[16];     //value is C string. 
extern CHAR MainInfo_ManufactureName[16]; //value is C string. 
extern CHAR MainInfo_HeadHeightLevel[8]; //value is INT8U *. the valid value is 1,2,3(high, middle, low).
extern CHAR MainInfo_QualitySetting[8];  //value is INT8U *. the valid value is 0,1,2(OFF/type1/type2).
extern CHAR MainInfo_CleaningStrong[8];  //value is INT8U *. the valid value is 1,2,3(weak, normal, strong).
extern CHAR MainInfo_DataReceiving[8];   //value is INT8U *. the valid value is 0,1(FALSE/TRUE).
extern CHAR MainInfo_Busy[8];            //value is INT8U *. the valid value is 0,1(FALSE/TRUE).
extern CHAR MainInfo_strinfo[16];
extern CHAR MainInfo_percent[8];
extern CHAR MainInfo_errorcode[8];

//
extern INT8U imgTop[8];
extern INT8U imgBot[8];
extern INT8U imgLeft[8];
extern INT8U imgRight[8];
extern INT8U imgZero[16];
#endif
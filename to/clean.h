#ifndef _CLEAN_H_
#define _CLEAN_H_

#ifdef EPSON_FLASH_IDLE
extern INT8U postCapping_flash;
#endif
#if defined (HEAD_RICOH_G4) || defined (EPSON_CLEAN_UPDOWN) || defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_INTEGRATE_3)
#define CMD_EPSON_T_IDLE   0
#define CMD_EPSON_T_SHEET_CUT   1
#define CMD_EPSON_T_LOAD_MEDIA  2
#define CMD_RICOH_T_START_IDEL_SPRAY    3
#define CMD_RICOH_T_STOP_IDEL_SPRAY    4
#define CMD_EPSON_T_MACHIME_TEST  5
#define CMD_EPSON_T_DGEN_MOVE_PAPER  6
extern INT8U CMD_Epson_Operate_Type;
#endif

#ifdef AUTO_CLEAN_BYTIME 
extern INT32U Clean_Wait_Time;
#endif
#ifdef MANUFACTURER_DGEN
extern INT32U IdleClean_Time;
#endif
#if defined (EPSON_CLEAN_UPDOWN) || defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_INTEGRATE_3) ||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)

#define MAX_CLEAN_CONFIG    4
#define MAX_EPSONHEAD   4
#define PERMITTED_POSI_ERROR    15 //with 720DPI, about 0.5mm.

#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_INTEGRATE_3)
extern struct USB_CleanSpray_Config   USB_CleanSpray;
#endif

#if defined(EPSON_CLEAN_UPDOWN) && !defined(EPSON_LCD)
//#error "EPSON_CLEAN_UPDOWN need EPSON_LCD"
#endif

#if (defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV)||defined(MANUFACTURER_HUMAN_EPSON)||defined(MANUFACTURER_HUMAN_STITCH)||defined(ALLWIN_EPSON_SAME) ||defined(MANUFACTURER_LONGKE_FLAT_UV)||defined(MANUFACTURER_YAODI_EPSON)||defined(MANUFACTURER_TENGFENG_EPSON)||defined(MANUFACTURER_KINCOLOR_FLAT)||defined(MANUFACTURER_SAISHUN_WATER_EPSON)||defined(MANUFACTURER_RIPSTAR_TEST_UV))&&defined(EPSON_BOTTOM_BOARD_V2)
#define TATE_CLEAN_CZ_SPEED_LEVEL 2
#elif defined(MANUFACTURER_WANMENG_EPSON)||defined(MANUFACTURER_ADDTOP_EPSON)
#define TATE_CLEAN_CZ_SPEED_LEVEL 1
#elif defined(MANUFACTURER_LONGKE_FLAT_UV_150)||defined(MANUFACTURER_LONGKE_FLAT_UV_130)||defined(MANUFACTURER_SAIBO_ROLL_UV)
#define TATE_CLEAN_CZ_SPEED_LEVEL 3
#elif defined(MANUFACTURER_DYSS)
#define TATE_CLEAN_CZ_SPEED_LEVEL 4
#elif defined(MANUFACTURER_MARTIN)||defined(MANUFACTURER_DGEN)||defined(MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)||defined(MANUFACTURER_XINHANGJIDIAN_STITCH)||defined(MANUFACTURER_ZHONGYE_FLAT_UV)||defined(MANUFACTURER_XUKE)||defined(MANUFACTURER_ZHONGYE_ROLL_UV)
#define TATE_CLEAN_CZ_SPEED_LEVEL 2
#elif defined(MANUFACTURER_INKWIN)
#ifdef SUPPORT_MOTOR_CONTROL_ONLY_STEP
#define TATE_CLEAN_CZ_SPEED_LEVEL 0
#else
#define TATE_CLEAN_CZ_SPEED_LEVEL 2
#endif
#elif defined(MANUFACTURER_BEMAJET) || defined(COLORFUL_EPSON)
#define TATE_CLEAN_CZ_SPEED_LEVEL 0
#else
#define TATE_CLEAN_CZ_SPEED_LEVEL 2
#endif

#if defined (EPSON_CLEAN_UPDOWN)
extern INT8U Send_Second_Backhaul(void);
#endif

#if defined (EPSON_CLEAN_UPDOWN)||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
#define EPR_CLEANPARA_FLAG 0x3B5C
#define EPR_CLEANPARA_FLAG_3B5A 0x3B5A
#define EPR_CLEANPARA_HEAD_FLAG 0x3B5B
//#define EPR_CLEANPARA_VER 0x101
//liubo add HeadBox_Z_FlashPos
#define EPR_CLEANPARA_VER_102 0x102
//liubo add Idle_FlashFreqInterval
#define EPR_CLEANPARA_VER_103 0x103
//liubo add IdleClean_intervalTime
#define EPR_CLEANPARA_VER 0x104
__packed struct CleanParaEpsonAllwin_Factory
{
	INT16U Carriage_X_SuckPos;
	INT16U HeadBox_Z_SuckPos;
	
	INT16U WiperPos_Y[MAX_EPSONHEAD];
	INT16U HeadBox_Z_WipePos;
	INT16U Carriage_X_WipePos_Start[MAX_EPSONHEAD];
	INT16U Carriage_X_WipePos_End[MAX_EPSONHEAD];
};

enum ALLWIN_CLEANTYPE   { 
	ACT_FULLLOOP, //follow allwin second email. (suck+wipe)loop+flash
	ACT_SUCKTHENWIPE, //follow the first email. (suck)loop+(wipe)loop. // this way is not supported now.
	ACT_QUICKSUCK //follow DIAO advice. like ACT_FULLLOOP, not down ink-box when suck.
};
__packed struct CleanParaEpsonAllwin_Config
{
	enum ALLWIN_CLEANTYPE CleanType;
	
	INT8U SuckTimes;
	INT16U SuckInkTime; //unit:0.1s;
	INT16U InputAirTime; //unit:0.1s;
	INT16U SuckWasteInkTime; //unit:0.1s;
	
	INT8U WipeTimes; //only for ACT_SUCKTHENWIPE
	INT8U Carriage_X_Wipe_Speed;
	
	//flash part
	//清洗闪喷频率1-10KHZ，清洗闪喷时间10秒，间歇闪喷周期2秒，间歇有效闪喷时间2秒摸式出墨
	//（闪喷频率和闪喷时间和闪喷周期和有效闪喷时间软件可以设置
	INT16U FlashFreqInterval; //unit: us.
	INT8U FlashTime; //unit:0.1s.
	INT8U FlashCycle; //unit:0.1s.
	INT8U FlashIdleInCycle; //unit:0.1s.
};

__packed struct CleanParaEpsonAllwin_Runtime
{
	INT8U ActivedConfig; //total cleaning configure: customized/strong/normal/weak/refill.
	INT8U HeadMask;
};

__packed struct EPR_CleanParaEpsonAllwin
{
	INT16U flag; //EPR_CLEANPARA_FLAG
	//INT16U version;
	INT16U structsize; //sizeof struct CLEANPARA_EPSON_ALLWIN_USB
	//INT16U Checksum;
	
	struct CleanParaEpsonAllwin_Factory factory;
	struct CleanParaEpsonAllwin_Config Config[MAX_CLEAN_CONFIG]; //total cleaning configure: customized/strong/normal/weak/refill.
	struct CleanParaEpsonAllwin_Runtime Runtime;
	
	INT8U bAutoClean;
	INT8U AutoClean_passInterval;
	INT8U AutoCleanConfig;
	INT16U HeadBox_Z_FlashPos;
	INT16U Idle_FlashFreqInterval; //unit: ms.
	INT8U IdleClean_intervalTime;	//空闲清洗间隔时间(小时),0:关闭空闲清洗，非0:打开空闲清洗，范围:0-12
//EPSON_PRT_FLASH need NEW EPR_CLEANPARA_VER
#if 0
    INT8U Flash_Prt_On; 
    INT8U Flash_Prt_Pass;
    INT16U Flash_Prt_Freq;
    INT32U Flash_Prt_POsi;
    INT32U Flash_Prt_POsi_1;
    INT32U Flash_Width ;
#endif	
};

typedef __packed struct CLEANPARA_EPSON_ALLWIN_USB
{
	INT8U HeadMask;
	
	//suck part:
	INT8U SuckTimes;//type: 0, standard, do down action; 0x80, Quick, don't down, directly suck the next time.
	INT16U Carriage_X_SuckPos;
	INT16U HeadBox_Z_SuckPos;
	INT16U SuckInkTime; //unit:0.1s;
	INT16U InputAirTime; //unit:0.1s;
	INT16U SuckWasteInkTime; //unit:0.1s;
	
	//wipe part:
	INT8U WipeTimes;
	INT16U WiperPos_Y[4];
	INT16U HeadBox_Z_WipePos;
	INT16U Carriage_X_WipePos_Start[4];
	INT16U Carriage_X_WipePos_End[4];
	INT8U Carriage_X_Wipe_Speed;
	
	//flash part
	//清洗闪喷频率1-10KHZ，清洗闪喷时间10秒，间歇闪喷周期2秒，间歇有效闪喷时间2秒摸式出墨
	//（闪喷频率和闪喷时间和闪喷周期和有效闪喷时间软件可以设置
	INT16U FlashFreqInterval; //unit: us.
	INT8U FlashTime; //unit:0.1s.
	INT8U FlashCycle; //unit:0.1s.
	INT8U FlashIdleInCycle; //unit:0.1s.
	INT16U HeadBox_Z_FlashPos;//闪喷时Z轴高度
	INT16U Idle_FlashFreqInterval;//空闲闪喷间隔ms
}CleanParaType_EPSON_ALLWIN_USB;

extern struct EPR_CleanParaEpsonAllwin cleanparam_EPSON_ALLWIN;
extern volatile INT8U ActiveCleanConfig;

void SetCleanParam_EPSON_Allwin(CleanParaType_EPSON_ALLWIN_USB * pUsb,INT8U index);
void OutCleanParam_EPSON_Allwin(CleanParaType_EPSON_ALLWIN_USB * pUsb,INT8U index);
void SetAutoClean_EPSON_Allwin(CleanParaType * pUsb);
void OutAutoClean_EPSON_Allwin(CleanParaType * pUsb);
void InitialCleanParam_EPSON_Allwin();
INT8U ReadCleanParamEPR_EPSON_Allwin(struct EPR_CleanParaEpsonAllwin *pCleanPara);
INT8U SaveCleanParamEPR_EPSON_Allwin(struct EPR_CleanParaEpsonAllwin *pCleanPara, INT16S start, INT16U len);

#elif defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) || defined(EPSON_CLEAN_INTEGRATE_3) 
#define EPR_CLEANPARA_FLAG_VER_3A5C 0x3A5C
//cheney 20110723: Add SprayInterval and passNum
#define EPR_CLEANPARA_FLAG 0x3A5D
#define EPR_CLEANPARA_HEAD_FLAG 0x3B5E
#define EPR_CLEANPARA_VER_101 0x101
//liubo 20110928: Add Clean_Fill_Safe
#define EPR_CLEANPARA_VER_102 0x102
//wangruixuan 20120620:add Carriage_X_ReleasePos_1 ,Carriage_X_WipePos_1_Start ,Carriage_X_WipePos_1_End 
#define EPR_CLEANPARA_VER_103 0x103
//LIUBO ADD AUTO CLEAN 20130321
#define EPR_CLEANPARA_VER 0x104
__packed struct CleanParaEpsonMicolor_Factory
{
	INT8U Speed_Interval[4]; //100us. for SUPPORT_MOTOR_CONTROL, 10us
	INT8U ElectricRotateDir; //Electric Rotate dir's Pos dir: (A_L,B_L)->(A_H,B_L)->(A_H,B_H)->(A_L,B_H)->(A_L,B_L).
	
	//Pos dir : hide(idle status)->Wipe->Suck.
	//Neg dir : Suck->Wipe->hide.
	INT16U Wiper_Y_HideToWipeDistance; //the rotated step from hide status to Wipe status.
	INT16U Wiper_Y_WipeToSuckDistance; //the rotated step from Wipe status to suck status.
	INT16U Wiper_Y_SuckToHideDistance; //the rotated step from Suck status to hide status.
	
	INT16S Carriage_X_SuckPos;
	INT16S Carriage_X_ReleasePos;
	INT16S Carriage_X_WipePos_Start;
	INT16S Carriage_X_WipePos_End;
	INT16S Carriage_X_FlashPos;
	INT16S Carriage_X_FlashPos_1;
	
	INT16S Carriage_X_SprayPos_Start;
	INT16S Carriage_X_SprayPos_End;
	INT16S Carriage_X_SprayPos_1_Start;
	INT16S Carriage_X_SprayPos_1_End;
};

enum MicolorCleanType{None = 0, SuckOnly, SuckAndWipe, SuckWipeAndFlash, FlashOnly, PreClean};
__packed struct CLEANSECTION_EPSON_MICOLOR
{
	enum MicolorCleanType type;
	INT8U LoopTimes; //循环次数。
	INT16U SuckInkTime; //unit:0.1s;
	INT16U StayTime; //unit:0.1s;
	INT16U ReleaseTime; //unit:0.1s;
	
	INT16U FlashFreqInterval; //unit: us.
	INT8U FlashTime; //unit:0.1s.
	INT8U FlashCycle; //unit:0.1s.
	INT8U FlashIdleInCycle; //unit:0.1s.
	
	INT8U SuckSpeed; 
	INT8U MoveSpeed; 
	INT8U Option;
};
__packed struct CleanParaEpsonMicolor_Config
{
	INT8U Carriage_X_Wipe_Speed;
	INT8U sectionCount;
	struct CLEANSECTION_EPSON_MICOLOR sections[4];
};
__packed struct CleanParaEpsonMicolor_Runtime
{
	INT8U ActivedConfig; //total cleaning configure: customized/strong/normal/weak/refill.
	
	INT8U HeadMask; //for multi-head system.
};

__packed struct EPR_CleanParaEpsonMicolor
{
	INT16U flag; //EPR_CLEANPARA_FLAG
	INT16U structsize; //sizeof struct CLEANPARA_EPSON_ALLWIN_USB
	
	struct CleanParaEpsonMicolor_Factory factory;
	struct CleanParaEpsonMicolor_Config Config[MAX_CLEAN_CONFIG]; //total cleaning configure: customized/strong/normal/weak/refill.
	struct CleanParaEpsonMicolor_Runtime Runtime;
	
	INT8U bAutoSpray;
	INT16U SprayWidth; //720dpi.
	
	//add from version 0x3A5D  
	INT32U SprayInterval; //unit: us.
	INT16U passNum;
	//add from version 0x102   
	INT8U Clean_Fill_Safe;
	//add from version 0x103
	INT16S Wiper_Y_HideToWipeDistance_1;
	INT16S Carriage_X_WipePos_1_Start;
	INT16S Carriage_X_WipePos_1_End;
	//add from version 0x104
	INT8U bAutoClean;
	INT8U AutoClean_passInterval;
	INT8U AutoCleanConfig;
	INT16U AutoClean_timeInterval;
};
__packed struct USB_CleanSpray_Config
{
	INT8U bAutoSpray;
	INT16U SprayWidth; //720dpi.
	INT32U SprayInterval; //unit: us.
	INT16U passNum;
};
typedef __packed struct CLEANPARA_EPSON_MICOLOR_USB
{
	//motor part:
	INT8U Carriage_X_Wipe_Speed;
	INT8U Speed_Interval[4]; //100us.
	INT8U RotateDir; //Electric Rotate dir's Pos dir: (A_L,B_L)->(A_H,B_L)->(A_H,B_H)->(A_L,B_H)->(A_L,B_L).
	
	//Pos dir : hide(idle status)->Wipe->Suck.
	//Neg dir : Suck->Wipe->hide.
	INT16U Wiper_Y_HideToWipeDistance; //the rotated step from hide status to Wipe status.
	INT16U Wiper_Y_WipeToSuckDistance; //the rotated step from Wipe status to suck status.
	INT16U Wiper_Y_SuckToHideDistance; //the rotated step from Suck status to hide status.
	//when 2 head case, it will be the next head's FlashPos.
	
	INT8U sectionCount;
	//position part
	INT16S Carriage_X_SuckPos;
	INT16S Carriage_X_ReleasePos;
	INT16S Carriage_X_WipePos_Start;
	INT16S Carriage_X_WipePos_End;
	INT16S Carriage_X_FlashPos;
	
	INT16S Wiper_Y_HideToWipeDistance_1;
	INT16S Carriage_X_WipePos_1_Start;
	INT16S Carriage_X_WipePos_1_End;
	//user part:
	struct CLEANSECTION_EPSON_MICOLOR sections[4];
	
}CleanParaType_EPSON_MICOLOR_USB;
extern struct EPR_CleanParaEpsonMicolor cleanparam_EPSON_MICOLOR;
extern volatile INT8U ActiveCleanConfig;

void SetCleanParam_EPSON_Micolor(CleanParaType_EPSON_MICOLOR_USB * pUsb,INT8U index);
void OutCleanParam_EPSON_Micolor(CleanParaType_EPSON_MICOLOR_USB * pUsb,INT8U index);
void SetAutoClean_EPSON_Micolor(CleanParaType * pUsb);
void OutAutoClean_EPSON_Micolor(CleanParaType * pUsb);
void InitialCleanParam_EPSON_Micolor();
INT8U ReadCleanParamEPR_EPSON_Micolor(struct EPR_CleanParaEpsonMicolor *pCleanPara);
INT8U SaveCleanParamEPR_EPSON_Micolor(struct EPR_CleanParaEpsonMicolor *pCleanPara, INT16S start, INT16U len);

void RotateCleanMotor(INT8U RotateDir, INT32U distance, INT8U speedIndx);

void QuickClean_EPSON_MICOLOR();

void SlowLeaveXOrigin();

#endif

INT8U GetCleanPartCount();

#endif

#endif

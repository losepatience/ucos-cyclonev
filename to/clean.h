#ifndef _CLEAN_H_
#define _CLEAN_H_

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

#if defined (EPSON_CLEAN_UPDOWN) || defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_INTEGRATE_3) ||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)

#define MAX_CLEAN_CONFIG    4
#define MAX_EPSONHEAD   4
#define PERMITTED_POSI_ERROR    15 //with 720DPI, about 0.5mm.

#define TATE_CLEAN_CZ_SPEED_LEVEL 4

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
struct CleanParaEpsonAllwin_Factory
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
struct CleanParaEpsonAllwin_Config
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

struct CleanParaEpsonAllwin_Runtime
{
	INT8U ActivedConfig; //total cleaning configure: customized/strong/normal/weak/refill.
	INT8U HeadMask;
};

struct EPR_CleanParaEpsonAllwin
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
};

typedef struct CLEANPARA_EPSON_ALLWIN_USB
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


INT8U GetCleanPartCount();

#endif

#endif

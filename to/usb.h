#ifndef __USB_H__
#define	__USB_H__

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)

#define EP6_CMD_T_PRINTACTION   0x1
#define EP6_CMD_T_PA_CANCEL     0x1
#define EP6_CMD_T_PA_PAUSE      0x2
#define EP6_CMD_T_PA_RESUME     0x3
#define EP6_CMD_T_MAINUI        0x2 //see struct USB_RPT_MainUI_Param
#define EP6_CMD_T_CALIBRATION   0x3
#define EP6_CMD_T_CLEAN_SETTING 0x4
#define EP6_CMD_T_MEDIA         0x5
#define EP6_CMD_T_PRINTQUALITY  0x6
#define EP6_CMD_T_MOTION_RECORD  0x7 
#define EP6_CMD_T_CALIBRATION_DIRTY  0x8 
#define EP6_CMD_T_STEP_DIRTY  0x9
#define EP6_CMD_T_MEDIA_HIGH  0x10

#ifdef WAVE_SET_FUN
#define EP6_CMD_T_WAVE_READ_READY  0x11
#define EP6_CMD_T_WAVE_SET_READY  0x12
#define EP6_CMD_T_SWING_READY 0x13
#define EP6_CMD_T_WAVE_PRINT_READY  0x14
#define EP6_CMD_T_WAVE_NAME_READY  0x18
#define EP6_CMD_T_WAVE_NAME_FINISH  0x19
#define EP6_CMD_T_WAVE_POINT_FINISH  0x1A
#define EP6_CMD_T_WAVE_CHANNEL_FINISH  0x1B
#endif

#if defined(DYSS_UV_CLEAN)
#define EP6_CMD_T_MEDIA_INIT_OVER  0x15
#endif

#ifdef FULGENCY_FUN
#define EP6_CMD_T_PRT_START  0x16
#define EP6_CMD_T_PRT_UNLOAD_FINISH  0x17
#endif

struct USB_RPT_MainUI_Param
{
	float PrintOrigin;
	
	//the following is valid only when PM send to FW.
	int PassNum; //指明步进修正对应的PASS数。
	int StepModify; //用于主UI上的步进修正。
#ifdef RIPSTAR_FLAT_EX
	float PrintYOrigin;
	float PrintZOrigin;
#endif
};

struct USB_RPT_Media_Info
{
	float MediaOrigin;
	float MediaWidth;
	float Margin;
#ifdef RIPSTAR_FLAT_EX
	float MediaLength;
	INT8U Prt_Y_Continue;
#endif
};

struct USB_Print_Quality
{
	//Index: 0, OFF; 1, ECLOSION type1(gradient); 2, ECLOSION type2(Wave).
	int PrintQuality;
};

extern struct USB_RPT_MainUI_Param mainUIPara;
extern struct USB_RPT_Media_Info mediaInfo;
extern struct USB_Print_Quality printQuality;
extern INT8U mainUI_dirty;
extern INT8U mediaInfo_dirty;
extern INT8U printQuality_dirty;
extern INT8U bFactoryDataDirty;
extern INT8U printratio_dirty;
extern INT8U MaxGroupNumber_is_group_num;
extern struct USB_RPT_MainUI_Param mainUIPara_ToPM;
extern struct USB_RPT_Media_Info mediaInfo_ToPM;
extern struct USB_Print_Quality printQuality_ToPM;

#endif

void USBDMA_EP2_DoneInterrupt(void);
void USBDMA_EP6_DoneInterrupt(void);

#ifdef DYSS_UV_CLEAN
extern float DYSS_MEASURE_HIGH_VALUE;
extern INT8U MEASURE_HIGH_SUCCSE;
#endif

#endif

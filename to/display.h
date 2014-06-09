#ifndef _DISPLAY_H_
#define _DISPLAY_H_

extern volatile INT8S kb_installed;

enum StatusTypeEnum 
{ //all error codes same as the one that is reported to PrinterManager.
	E_ST_SVC = 1, //errorcode is valid. 
	E_ST_FTA,     //errorcode is valid
	E_ST_ERR,     //errorcode is valid
	E_ST_ERR_AUTO,//errorcode is valid
	E_ST_WAR,     //errorcode is valid
	E_ST_INI,     //errorcode is valid. here, error code is the update progress id.
	E_ST_RDY,
	E_ST_MOVING,
	E_ST_PRINT,   //percent is valid. when percent is 0xFF, keep the old percent.
	E_ST_PAUSE,   
	E_ST_CLEANING,
	E_ST_CANCELING,
	E_ST_UPDATING, //percent is valid. when percent is 0xFF, keep the old percent.
#ifdef FUNC_MAINTAIN
	E_ST_MAINTAIN,
#endif
};

INT8U LCD_SetDisStatus(enum StatusTypeEnum status, INT32U errorcode, INT8U percent, void * extraInfo);
//when keyboard is not assembly, LCD_SetDisStatus will be very slow for LCD_MODULE_ZX19264M1A.
//  At first, LCD_SetDisStatusEx will check if keyboard is exist or not. 
INT8U LCD_SetDisStatusEx(enum StatusTypeEnum status, INT32U errorcode, INT8U percent, void * extraInfo);

enum InfoTypeEnum
{ //Until now, only implement E_IT_MediaWidth.
	E_IT_MediaWidth = 1,      //value is float *. unit is inch.
	E_IT_PrintOrigin,     //value is float *. unit is inch.
	E_IT_MediaHigh,      //value is float *. unit is inch.
	E_IT_PrintZOrigin,     //value is float *. unit is inch.
	E_IT_MediaLength,      //value is float *. unit is inch.
	E_IT_PrintYOrigin,     //value is float *. unit is inch.
	E_IT_PassNum,         //value is INT8U *. 
	E_IT_Resolution,      //value is INT16U *. 
	E_IT_PrinterName,     //value is C string. 
	E_IT_ManufactureName, //value is C string. 
	E_IT_HeadHeightLevel, //value is INT8U *. the valid value is 1,2,3(high, middle, low).
	E_IT_QualitySetting,  //value is INT8U *. the valid value is 0,1,2(OFF/type1/type2).
	E_IT_CleaningStrong,  //value is INT8U *. the valid value is 1,2,3(weak, normal, strong).
	E_IT_DataReceiving,   //value is INT8U *. the valid value is 0,1(FALSE/TRUE).
	E_IT_Busy,             //value is INT8U *. the valid value is 0,1(FALSE/TRUE).
	E_IT_DIS_INF             //value is CHAR *. ERROR INF.
};

INT8U LCD_SetMainInfo(enum InfoTypeEnum infoType, void * value);
//same as LCD_SetMainInfoEx.
INT8U LCD_SetMainInfoEx(enum InfoTypeEnum infoType, void * value);

//this enum list is temporary. it varies with the different manufacturer and customer requirement.
//However, this enum contains all LED ID though some LED ID is invalid at some manufacturer product.
enum LEDIDEnum
{
	E_LI_POWERON,    //Debug Only.
	
	E_LI_Busy = 1,  //Micolor version begin.
	E_LI_Heat_Pre,
	E_LI_Heat_Middle,
	E_LI_Heat_Post,  //Micolor version end.
	
	E_LI_DataTransfer,    //Motuh version begin
	E_LI_CleaningWeak,  
	E_LI_CleaningNormal,
	E_LI_CleaningStrong,
	E_LI_HeadLevel_High,
	E_LI_HeadLevel_Low,
	E_LI_PrintQuality_ON,
	E_LI_PrintQuality_OFF //Motuh version end
		
};

enum LEDStatusENUM
{
	E_LS_ON,
	E_LS_OFF,
	E_LS_FLASH  //flash freq. is fixed. about 1s light/1s dark.  
};
//for current version, only implement Motuh version.
INT8U UI_DisplayLED(enum LEDIDEnum ledID, enum LEDStatusENUM ledStatus);

enum KeyEventEnum
{
	KeyDown =0, KeyUp 
};
#define KEYID_LEFT  1
#define KEYID_RIGHT 2
#define KEYID_UP    3
#define KEYID_DOWN  4
#define KEYID_MENU  5
#define KEYID_ENTER 6
#define KEYID_CANCEL  7
#define KEYID_PAUSE 8
#define KEYID_CLEAN 9
//micolor special:
#define KEYID_BASEPOINT 0x101
#define KEYID_SHEETCUT  0x102
#define KEYID_PRINTQUALITY  0x103
//Allwin Special:
#define KEYID_CHECKNOZZLE       0x201
#define KEYID_CAPPING_UNCAPPING 0x202
//Qicaihong Special:
#define KEYID_HEATER 0x203

INT8U LCD_SendKey(unsigned int nKeyID,  enum KeyEventEnum keyEvent); // 传递键盘事件

//Printer configure part
INT8U GetGroupNumber();
INT8U GetGroupHeadNumber();
INT8U GetColorNumber();
INT8U IsWeakSolventPrinter();
INT8U IsIntegrateHead();
INT8U IsYIntervalReplaceVertical();

//0: Sim chinese, 1: trad chinese, 3: english, 4: tailand
INT8U GetLanguageIndex();
INT8U * GetPrintedSpotColorName(INT8U index);

typedef INT8U (*UIActionFuncType)(void * pData);
typedef INT8U (*UIActionFuncExType)(void * pData, int Index);
typedef INT8U (*UISetIntFuncType)(void * pData, int Data);
typedef INT8U (*UISetIntFuncExType)(void * pData, int Index, int Data);
typedef INT8U (*UISetFloatFuncType)(void * pData, float Data);
typedef INT8U (*UISetFloatFuncExType)(void * pData, int Index, float Data);
typedef INT8U (*UISetIndexFuncType)(void * pData, int Index);//for radio button
typedef INT8U (*UISetIndexFuncExType)(void * pData, int Index, int Data);//for radio button
typedef INT32S (*UIGetIntFuncType)(void * pData);
typedef INT32S (*UIGetIntFuncExType)(void * pData, int Index);
typedef float (*UIGetFloatFuncType)(void * pData);
typedef float (*UIGetFloatFuncExType)(void * pData, int Index);
typedef INT8U (*UIGetIndexFuncType)(void * pData);//for radio button
typedef INT8U (*UIGetIndexFuncExType)(void * pData, int Index);//for radio button

//calibration part.
//user model of calibration
INT8U Cali_UM_CheckNozzle(void * pData, int Index);

// Index: 0:GlossPaper, 1:Vinyl, 2:PP, 3:Film, 4:Other1, 5:Other2, 6:Other3.
INT8U Cali_UM_Print_Step(void * pData, int Index);
INT32S Cali_UM_Get_Step(void * pData, int Index);
INT8U Cali_UM_Set_Step_Modify(void * pData, int Index, float Data);
float Cali_UM_Get_Step_Modify(void * pData, int Index);

//Index: 0~3:VSD1~VSD4
INT8U Cali_UM_Print_BiDir(void * pData, int Index);
//Index: 0~3:VSD1~VSD4(High); 10~13:VSD1~VSD4(Low);
INT8U Cali_UM_Set_BiDir(void * pData, int Index, int Data);
INT32S Cali_UM_Get_BiDir(void * pData, int Index);

INT8U Cali_Print_GroupSpace(void * pData, int Index);
//for epson, Index: headnumber. 
//for ricoh and 1head2color, index: GroupNumber * 100 + headNumber;
//for ricoh and 1head1color, index: GroupNumber * 100 + colorNumber;
INT8U Cali_Set_GroupSpace(void * pData, int Index, int Data);
INT32S Cali_Get_GroupSpace(void * pData, int Index);

INT8U Cali_UM_Print_TestingPrint(void * pData, int Index);

//Engineer model of calibration
//Index: 0~3:VSD1~VSD4
INT8U Cali_EM_Print_CheckNozzle(void * pData, int Index);

//Index: 0~3:VSD1~VSD4
INT8U Cali_EM_Print_DotCheck(void * pData, int Index);

INT8U Cali_EM_Print_InterleaveCheck(void * pData, int Index);
INT8U Cali_EM_Print_HeadSlantCheck(void * pData, int Index);
INT8U Cali_EM_Print_HeadIntervalCheck(void * pData, int Index);

//Radio button:
//Index: 0:2pass, 1:3Pass, 2:4Pass, 3:6Pass. 4:8Pass, 5:12Pass, 
INT8U Cali_EM_Get_Step_PassNum(void * pData, int index);
INT8U Cali_EM_Set_Step_PassNum(void * pData, int Index, int Data);

// Index: 0:GlossPaper, 1:Vinyl, 2:PP, 3:Film, 4:Other1, 5:Other2, 6:Other3.
INT8U Cali_EM_Print_Step(void * pData, int Index);
INT32S Cali_EM_Get_Step(void * pData, int Index);
INT8U Cali_EM_Set_Step_Modify(void * pData, int Index, float Data);
float Cali_EM_Get_Step_Modify(void * pData, int Index);

//Index: enum RES_VSD_INDEX { E_360_VSD1=0, E_360_VSD2, E_360_VSD3, E_360_VSD4,
//    E_720_VSD1=10, E_720_VSD2, E_720_VSD3, E_720_VSD4,
//    in the future, there are 540,270,1440.... More, 360speed with 720 precision.
INT8U Cali_EM_Print_Hor_Left(void * pData, int Index);
INT8U Cali_EM_Print_Hor_BiDir(void * pData, int Index);
INT8U Cali_EM_Print_Hor_Right(void * pData, int Index);

//Index: there are three part: the first is Color&Head index, such as K1,K2,C1,C2,..., or H1K1,H1K2....
//  the second is enum RES_VSD_INDEX.
//  the third is head height level(High/Low), 0, high; 1, low
//  the final index = (head height level)*10000+(enum RES_VSD_INDEX)*100+(Color&Head index).
INT32S Cali_EM_Get_Hor_Left(void * pData, int Index);
INT32S Cali_EM_Get_Hor_Right(void * pData, int Index);
INT8U Cali_EM_Set_Hor_Left(void * pData, int Index, int Data);
INT8U Cali_EM_Set_Hor_Right(void * pData, int Index, int Data);

//Index: there are two part: the first is enum RES_VSD_INDEX.
//  the second is head height level(High/Low), 0, high; 1, low
//  the final index = (head height level)*100+(enum RES_VSD_INDEX).
INT32S Cali_EM_Get_Hor_BiDir(void * pData, int Index);
INT8U Cali_EM_Set_Hor_BiDir(void * pData, int Index, int Data);

INT8U Cali_EM_Print_Vertical(void * pData, int Index);
//Index: 0~(colorCount-1):color index.
INT32S Cali_EM_Get_Vertical(void * pData, int Index);
INT8U Cali_EM_Set_Vertical(void * pData, int Index, int Data);

INT8U Cali_EM_Print_VerticalAlign(void * pData, int Index);

//Index: enum RES_VSD_INDEX.
INT8U Cali_EM_Print_Angle_Left(void * pData, int Index);
INT8U Cali_EM_Print_Angle_Right(void * pData, int Index);

//Index: enum RES_VSD_INDEX.
INT32S Cali_EM_Get_Angle_Left_H1(void * pData, int Index);
INT32S Cali_EM_Get_Angle_Left_H2(void * pData, int Index);
INT32S Cali_EM_Get_Angle_Right_H1(void * pData, int Index);
INT32S Cali_EM_Get_Angle_Right_H2(void * pData, int Index);
INT8U Cali_EM_Set_Angle_Left_H1(void * pData, int Index, int Data);
INT8U Cali_EM_Set_Angle_Left_H2(void * pData, int Index, int Data);
INT8U Cali_EM_Set_Angle_Right_H1(void * pData, int Index, int Data);
INT8U Cali_EM_Set_Angle_Right_H2(void * pData, int Index, int Data);

INT8U Cali_EM_Print_TestingPrint(void * pData, int Index);
//Index: 0~4:720/360/540/270/1440
INT8U Cali_EM_Get_TestingPrint_DPI(void * pData, int Index);
INT8U Cali_EM_Set_TestingPrint_DPI(void * pData, int Index, int Data);
//Index: 0~3:VSD1~VSD4
INT8U Cali_EM_Get_TestingPrint_VSD(void * pData, int Index);
INT8U Cali_EM_Set_TestingPrint_VSD(void * pData, int Index, int Data);
//Index: 0:4pass, 1:6Pass, 2:8Pass, 3:12Pass.
INT8U Cali_EM_Get_TestingPrint_PassNum(void * pData, int Index);
INT8U Cali_EM_Set_TestingPrint_PassNum(void * pData, int Index, int Data);

//For language setting store.
//Index, 0: Sim chinese, 1: trad chinese, 3: english, 4: tailand. //important: default is Chinese.
//but, real index is 0 is english, 1 is Sim chinese.
INT8U UI_Get_Language(void * pData);
INT8U UI_Set_Language(void * pData, int Index);

//Check Password
INT8U UI_CheckPassword(void * pData, INT8U * password, INT8U len);

//HeadHeightLevel is readonly.
//return value: 0, high; 1, low.
INT8U UI_Get_HeadHeightLevel(void * pData);

//For parameter reset
INT8U UI_Reset_Calibration(void * pData);
INT8U UI_Reset_MotionConfig(void * pData);
INT8U UI_Reset_CleanConfig(void * pData);

//For Spotcolor name
//Index: there are two part: the first is SpotColor Name ENUM, 
//      0:S1, 1:S2, 2:S3, 3:S4, 4:Lc, 5:Lm, 6:Ly, 7:Lk, 8:Llk, 9:Mk, 10:R, 11:Gr, 12:B, 13:Or, 14:W, 15:V....
//  the second is which Spotcolor, 0: Spotcolor1, 1: Spotcolor2, 2: Spotcolor3, 3: Spotcolor4.
//  the final index = (which Spotcolor)*100+(SpotColor Name ENUM).
#define MAX_SPOTCOLOR_NUM   20
INT8U UI_EM_Get_SpotColorName(void * pData, int index);
INT8U UI_EM_Set_SpotColorName(void * pData, int Index, int Data);


//keyboard
INT8U UIAction_Move_Left_Start(void * pData);
INT8U UIAction_Move_Right_Start(void * pData);
INT8U UIAction_Move_Up_Start(void * pData);
INT8U UIAction_Move_Down_Start(void * pData);
INT8U UIAction_Move_Left_End(void * pData);
INT8U UIAction_Move_Right_End(void * pData);
INT8U UIAction_Move_Up_End(void * pData);
INT8U UIAction_Move_Down_End(void * pData);

//for USB interface
INT8U UIAction_Move(void * pData, INT8U dir, INT32S distance);
INT8U UIAction_Move_Start(void * pData, INT8U dir, INT32S distance);
INT8U UIAction_Move_End(void * pData);
INT8U UIAction_X_GoHome(void * pData);
INT8U UIAction_AllAxis_GoHome(void * pData, INT8U dir);
INT8U UIAction_MeasureMedia(void * pData, INT8U method);

//Keyboard handler.
INT8U UIAction_CancelPrint(void * pData);
INT8U UIAction_PauseResumePrint(void * pData);
INT8U UIAction_SetPrintQuality(void * pData);
INT8U UIAction_QuickCleaning(void * pData);
INT8U UIAction_SetBasePoint(void * pData);
#if defined(MICOLOR_AUTOFUCTION) || defined(LECAI_EPSON_4H_Y_ROOLENCODER)
INT8U UIAction_SheetCut(void * pData);
#endif
INT8U UIAction_CheckNozzle(void * pData);

// sheet type
//Index: 0, transparent, 1, OPAQUE.
INT8U Menu_SetMediaOption(void * pData, int Index, int Data);
INT8U Menu_GetMediaOption(void * pData, int Index);
// Index: 0:GlossPaper, 1:Vinyl, 2:PP, 3:Film, 4:Other1, 5:Other2, 6:Other3.
INT8U Menu_SetMediaType(void * pData, int Index, int Data);
INT8U Menu_GetMediaType(void * pData, int Index);

INT8U SetBasePoint(void * pData, float point, INT8U bGoHome, INT8U bFlashPM);
INT8U SetMediaInfo(float origin, float width, float margin);

INT8U UpDownPrintingStep(void * pData, unsigned char bUp);
INT16S GetPrintingStep(void * pData);

//Origin & Width
INT8U Menu_SetBasePoint(void * pData, int Index);
INT8U Menu_InputBasePoint(void * pData, int Index, float Data); //unit:inch
float Menu_GetBasePoint(void * pData, int Index); //unit:inch

INT8U GetPRTParam_Head1_Encry_OnOff(void * pData, int Index);
INT8U SetPRTParam_Head1_Encry_OnOff(void * pData, int Index, int Data);

INT8U GetPRTParam_Head2_Encry_OnOff(void * pData, int Index);
INT8U SetPRTParam_Head2_Encry_OnOff(void * pData, int Index, int Data);

#ifdef EPSON_4H
INT8U GetPRTParam_Head3_Encry_OnOff(void * pData, int Index);
INT8U SetPRTParam_Head3_Encry_OnOff(void * pData, int Index, int Data);

INT8U GetPRTParam_Head4_Encry_OnOff(void * pData, int Index);
INT8U SetPRTParam_Head4_Encry_OnOff(void * pData, int Index, int Data);
#endif

#ifdef RIPSTAR_FLAT_EX

INT8U GetCalcParam_RICOH_X_SPEED(void * pData, int Index);
INT8U SetCalcParam_RICOH_X_SPEED(void * pData, int Index, int Data);

INT8U GetPRTParam_Y_PRT_CONTINUE_OnOff(void * pData, int Index);
INT8U SetPRTParam_Y_PRT_CONTINUE_OnOff(void * pData, int Index, int Data);

INT8U Menu_InputBaseYPoint(void * pData, int Index, float Data); //unit:inch
float Menu_GetBaseYPoint(void * pData, int Index); //unit:inch

INT8U Menu_InputBaseY_CAL_EX(void * pData, int Index, float Data); //unit:inch
float Menu_GetBaseY_CAL_EX(void * pData, int Index); //unit:inch

INT8U Menu_InputBaseZPoint(void * pData, int Index, float Data); //unit:inch
float Menu_GetBaseZPoint(void * pData, int Index); //unit:inch
#endif
INT8U Menu_InputMediaWidth(void * pData, int Index, float Data); //unit:inch
float Menu_GetMediaWidth(void * pData, int Index);  //unit:inch
#ifdef RIPSTAR_FLAT_EX
INT8U Menu_InputMediaLength(void * pData, int Index, float Data); //unit:inch
float Menu_GetMediaLength(void * pData, int Index); //unit:inch
INT8U Menu_InputMediaHigh(void * pData, int Index, float Data); //unit:inch
float Menu_GetMediaHigh(void * pData, int Index); //unit:inch
INT8U Menu_Move_Zpoint(void * pData, int Index);
INT8U SetBasePoint_All(void * pData, float pointX, float pointY, float pointZ, INT8U bGoHome, INT8U bFlashPM);
#endif
#if defined(MICOLOR_AUTOFUCTION)
INT8U Menu_LoadMedia(void * pData, int Index);
#endif
INT8U Menu_MeasureMedia(void * pData, int Index);
INT8U Menu_MachineTest(void * pData, int Index);
#if defined(MICOLOR_AUTOFUCTION) || defined(LECAI_EPSON_4H_Y_ROOLENCODER)
INT8U Menu_SheetCut(void * pData, int Index);
#endif
#if defined(DGEN_AUTOFUCTION)
INT8U Menu_DGEN_Move_Paper(void * pData, int Index);
INT8U SetMove_Paper_Speed(void * pData, int Index, int Data);
INT32S GetMove_Paper_Speed(void * pData, int Index);
#endif
INT8U Menu_SetMargin(void * pData, int Index, float Data); //unit:inch
float Menu_GetMargin(void * pData, int Index); //unit:inch

//Index: 0, OFF, 1, ON.
#if defined(MICOLOR_AUTOFUCTION)
INT8U Menu_SetAutoFeed(void * pData, int Index);
INT8U Menu_GetAutoFeed(void * pData);
INT8U Menu_SetAutoCut(void * pData, int Index);
INT8U Menu_GetAutoCut(void * pData);
INT8U Menu_SetAutoMeasureWidth(void * pData, int Index);
INT8U Menu_GetAutoMeasureWidth(void * pData);
INT8U Menu_SetAutoLoadMedia(void * pData, int Index);
INT8U Menu_GetAutoLoadMedia(void * pData);
INT8U Menu_SetAutoOutofMedia(void * pData, int Index);
INT8U Menu_GetAutoOutofMedia(void * pData);
INT8U Menu_SetAutoCoverCheck(void * pData, int Index);
INT8U Menu_GetAutoCoverCheck(void * pData);
INT8U Menu_SetAutoPlateFan(void * pData, int Index);
INT8U Menu_GetAutoPlateFan(void * pData);
INT8U Menu_SetAutoFixCheck(void * pData, int Index);
INT8U Menu_GetAutoFixCheck(void * pData);
INT8U Menu_SetAutoPowerOnClean(void * pData, int Index);
INT8U Menu_GetAutoPowerOnClean(void * pData);
INT8U Menu_SetAutoGoHome(void * pData, int Index);
INT8U Menu_GetAutoGoHome(void * pData);
//Index: 0, Strong; 1, week; 2, off.
INT8U Menu_SetPlateFanMode(void * pData, int Index, int Data);
INT8U Menu_GetPlateFanMode(void * pData, int Index);
#endif

#ifdef MANUFACTURER_DYSS
INT8U Menu_SetPlateFanMode(void * pData, int Index, int Data);
INT8U Menu_GetPlateFanMode(void * pData, int Index);
#endif
INT8U Menu_SetAutoStep(void * pData, int Index);
INT8U Menu_GetAutoStep(void * pData);

INT8U Menu_SetAutoFlashBeforePrint(void * pData, int Index);
INT8U Menu_GetAutoFlashBeforePrint(void * pData);

//Index: 0, OFF; 1, ECLOSION type1(gradient); 2, ECLOSION type2(Wave).
INT8U SetPrintQuality(int Index);

//Index: 0, OFF; 1, ECLOSION type1(gradient); 2, ECLOSION type2(Wave).
INT8U Menu_SetPrintQualityLevel(void * pData, int Index, int Data);
INT8U Menu_GetPrintQualityLevel(void * pData, int Index);
INT8U Menu_SetPrintQualityType(void * pData, int Index, int Data);
INT8U Menu_GetPrintQualityType(void * pData, int Index);

//index 1 HEAD 1  2 HEAD 2,4 HEAD 3
INT8U SetMenu_EPSON_ALLWIN_SELECTHeadPrint(void * pData, int Index, int Data);
INT8U GetMenu_EPSON_ALLWIN_SELECTHeadPrint(void * pData, int Index);

//index 1 HEAD 1  2 HEAD 2,4 HEAD 3
INT8U SetMenu_EPSON_ALLWIN_1HeadPrint_OnOff(void * pData, int Index, int Data);
INT8U GetMenu_EPSON_ALLWIN_1HeadPrint_OnOff(void * pData, int Index);
//Head parameter percent.
//index: 0~3:KCMY.
INT8U SetHeadParameter(void * pData, int Index, int Data);
INT32S GetHeadParameter(void * pData, int Index);

//active head number.
//index: on/off.
INT8U Menu_SetHead4Active(void * pData, int Index, int Data);
INT8U Menu_GetHead4Active(void * pData, int Index);
INT8U Menu_SetHead1ActiveCfg(void * pData, int Index);
INT8U Menu_GetHead1ActiveCfg(void * pData);
INT8U Menu_SetHead2ActiveCfg(void * pData, int Index);
INT8U Menu_GetHead2ActiveCfg(void * pData);
INT8U Menu_SetHead3ActiveCfg(void * pData, int Index);
INT8U Menu_GetHead3ActiveCfg(void * pData);
INT8U Menu_SetHead4ActiveCfg(void * pData, int Index);
INT8U Menu_GetHead4ActiveCfg(void * pData);

//Index: 0, OFF, 1, ON.
#if defined(EPSON_BOTTOM_BOARD_V3)||defined(EPSON_BOTTOM_BOARD_V2_1)
INT8U Menu_SetHeaterPre(void * pData, int Index);
INT8U Menu_GetHeaterPre(void * pData);
INT8U Menu_SetHeaterMiddle(void * pData, int Index);
INT8U Menu_GetHeaterMiddle(void * pData);
INT8U Menu_SetHeaterPost(void * pData, int Index);
INT8U Menu_GetHeaterPost(void * pData);
INT8U Menu_SetHeaterTempPre(void * pData, int Index, float Data); //unit:`C
float Menu_GetHeaterTempPre(void * pData, int Index); //unit:`C
INT8U Menu_SetHeaterTempMiddle(void * pData, int Index, float Data); //unit:`C
float Menu_GetHeaterTempMiddle(void * pData, int Index); //unit:`C
INT8U Menu_SetHeaterTempPost(void * pData, int Index, float Data); //unit:`C
float Menu_GetHeaterTempPost(void * pData, int Index); //unit:`C

INT8U Menu_SetRealHeaterTempPre(void * pData, int Index, float Data); //unit:`C
float Menu_GetRealHeaterTempPre(void * pData, int Index); //unit:`C
INT8U Menu_SetRealHeaterTempMiddle(void * pData, int Index, float Data); //unit:`C
float Menu_GetRealHeaterTempMiddle(void * pData, int Index); //unit:`C
INT8U Menu_SetRealHeaterTempPost(void * pData, int Index, float Data); //unit:`C
float Menu_GetRealHeaterTempPost(void * pData, int Index); //unit:`C
INT8U Menu_SetRealHeaterTempEnv(void * pData, int Index, float Data); //unit:`C
float Menu_GetRealHeaterTempEnv(void * pData, int Index); //unit:`C
#endif

#ifdef DYSS_UV_CLEAN
INT8U DoPower_Off_Mode_DYSS(void * pData, int Index);
#endif
#if defined( EPSON_CLEAN_UPDOWN)||defined( RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
//UIActionFuncType
//for the clean key of panel or clean button of PM
INT8U DoKeyClean_EPSON_ALLWIN(void * pData, int Index);
//index: 0~4: custom/strong/normal/weak/Refill.
INT8U DoClean_EPSON_ALLWIN(void * pData, int Index);
INT8U DoClean_InkBox_EPSON_ALLWIN(void * pData, int Index);
INT8U DoFlash_EPSON_ALLWIN(void * pData, int Index);

//index: 0, only head1, 1 only head 2, 2 all head.
INT8U SetCleanParam_EPSON_ALLWIN_HeadMask(void * pData, int Index, int Data);
//index: 0~4: custom/strong/normal/weak/Refill.
INT8U SetCleanParam_EPSON_ALLWIN_Shortcuts(void * pData, int Index, int Data);
INT8U SetCleanParam_EPSON_ALLWIN_SuckTimes(void * pData, int Index, int Data);
INT8U SetCleanParam_EPSON_ALLWIN_SuckInkTime(void * pData, int Index, float Data); //unit is second.
INT8U SetCleanParam_EPSON_ALLWIN_InputAirTime(void * pData, int Index, float Data); //unit is second.
INT8U SetCleanParam_EPSON_ALLWIN_SuckWasteInkTime(void * pData, int Index, float Data); //unit is second.
INT8U SetCleanParam_EPSON_ALLWIN_WipeTimes(void * pData, int Index, int Data);
INT8U SetCleanParam_EPSON_ALLWIN_Carriage_X_Wipe_Speed(void * pData, int Index, int Data);
INT8U SetCleanParam_EPSON_ALLWIN_FlashFreq(void * pData, int Index, float Data); //unit: KHz.
INT8U SetCleanParam_EPSON_ALLWIN_FlashTime(void * pData, int Index, float Data); //unit: second.
INT8U SetCleanParam_EPSON_ALLWIN_FlashCycle(void * pData, int Index, float Data); //unit: second.
INT8U SetCleanParam_EPSON_ALLWIN_FlashIdleInCycle(void * pData, int Index, float Data); //unit: second.
//return value: 0, only head1, 1 only head 2, 2 all head.
INT8U GetCleanParam_EPSON_ALLWIN_HeadMask(void * pData, int Index);
//return value: 0~4: custom/strong/normal/weak/Refill.
INT8U GetCleanParam_EPSON_ALLWIN_Shortcuts(void * pData, int Index);
//index: 0~4: custom/strong/normal/weak/Refill.
INT32S GetCleanParam_EPSON_ALLWIN_SuckTimes(void * pData, int Index);
float GetCleanParam_EPSON_ALLWIN_SuckInkTime(void * pData, int Index); //unit is second.
float GetCleanParam_EPSON_ALLWIN_InputAirTime(void * pData, int Index); //unit is second.
float GetCleanParam_EPSON_ALLWIN_SuckWasteInkTime(void * pData, int Index); //unit is second.
INT32S GetCleanParam_EPSON_ALLWIN_WipeTimes(void * pData, int Index);
INT32S GetCleanParam_EPSON_ALLWIN_Carriage_X_Wipe_Speed(void * pData, int Index);
float GetCleanParam_EPSON_ALLWIN_FlashFreq(void * pData, int Index); //unit: KHz.
float GetCleanParam_EPSON_ALLWIN_FlashTime(void * pData, int Index); //unit: second.
float GetCleanParam_EPSON_ALLWIN_FlashCycle(void * pData, int Index); //unit: second.
float GetCleanParam_EPSON_ALLWIN_FlashIdleInCycle(void * pData, int Index); //unit: second.

//auto clean setting. share the same data structure with PM.
//Index: 0, off, 1, on.
INT8U SetCleanParam_EPSON_ALLWIN_AutoClean_OnOff(void * pData, int Index, int Data);
INT8U SetCleanParam_EPSON_ALLWIN_AutoClean_Pass(void * pData, int Index, int Data);
#ifdef RIPSTAR_FLAT_EX
INT8U SetCleanParam_EPSON_ALLWIN_AutoClean_Time(void * pData, int Index, int Data);
#endif
//index: 0~3: custom/strong/normal/weak.
INT8U SetCleanParam_EPSON_ALLWIN_AutoClean_Way(void * pData, int Index, int Data);
//return value: 0, off, 1, on.
INT8U GetCleanParam_EPSON_ALLWIN_AutoClean_OnOff(void * pData, int Index);
INT32S GetCleanParam_EPSON_ALLWIN_AutoClean_Pass(void * pData, int Index);
#ifdef RIPSTAR_FLAT_EX
INT32S GetCleanParam_EPSON_ALLWIN_AutoClean_Time(void * pData, int Index);
#endif
//return value: 0~3: custom/strong/normal/weak.
INT8U GetCleanParam_EPSON_ALLWIN_AutoClean_Way(void * pData, int Index);
#ifdef MANUFACTURER_DGEN
//空闲清洗
INT8U GetCleanParam_IdleClean_OnOff(void * pData, int Index);
INT8U SetCleanParam_IdleClean_OnOff(void * pData, int Index, int Data);
INT32S GetCleanParam_IdleClean_Time(void * pData, int Index);
INT8U SetCleanParam_IdleClean_Time(void * pData, int Index, int Data);
#endif
#elif defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
//auto clean setting. share the same data structure with PM.
//Index: 0, off, 1, on.
INT8U SetCleanParam_EPSON_MICOLOR_AutoClean_OnOff(void * pData, int Index, int Data);
INT8U SetCleanParam_EPSON_MICOLOR_AutoClean_Pass(void * pData, int Index, int Data);
//index: 0~3: custom/strong/normal/weak.
INT8U SetCleanParam_EPSON_MICOLOR_AutoClean_Way(void * pData, int Index, int Data);
//return value: 0, off, 1, on.
INT8U GetCleanParam_EPSON_MICOLOR_AutoClean_OnOff(void * pData, int Index);
INT32S GetCleanParam_EPSON_MICOLOR_AutoClean_Pass(void * pData, int Index);
//return value: 0~3: custom/strong/normal/weak.
INT8U GetCleanParam_EPSON_MICOLOR_AutoClean_Way(void * pData, int Index);

INT8U SetCleanParam_EPSON_MICOLOR_HeadMask(void * pData, int Index, int Data);
INT8U GetCleanParam_EPSON_MICOLOR_HeadMask(void * pData, int Index);
//UIActionFuncType
//for the clean key of panel or clean button of PM
INT8U DoKeyClean_EPSON_MICOLOR(void * pData, int Index);
//index: 0~4: custom/strong/normal/weak/Refill.
INT8U DoClean_EPSON_MICOLOR(void * pData, int Index);
INT8U DoFlash_EPSON_MICOLOR(void * pData, int Index);
INT8U GetClean_EPSON_MICOLOR(void * pData, int Index);
//index: 0~4: custom/strong/normal/weak/Refill.
INT8U SetCleanParam_EPSON_MICOLOR_Shortcuts(void * pData, int Index, int Data);
//Index: there are two part: the first is clean way, 0~4: custom/strong/normal/weak/Refill.
//  the second is, 0~3, clean step.
//  the final index = (clean way)*100+(clean step).
INT8U SetCleanParam_EPSON_MICOLOR_Type(void * pData, int Index, int Data);
INT8U SetCleanParam_EPSON_MICOLOR_LoopTimes(void * pData, int Index, int Data);
INT8U SetCleanParam_EPSON_MICOLOR_SuckInkTime(void * pData, int Index, float Data); //unit is second.
INT8U SetCleanParam_EPSON_MICOLOR_StayTime(void * pData, int Index, float Data); //unit is second.
INT8U SetCleanParam_EPSON_MICOLOR_ReleaseTime(void * pData, int Index, float Data); //unit is second.
INT8U SetCleanParam_EPSON_MICOLOR_SuckSpeed(void * pData, int Index, int Data);
INT8U SetCleanParam_EPSON_MICOLOR_FlashFreq(void * pData, int Index, float Data); //unit: KHz.
INT8U SetCleanParam_EPSON_MICOLOR_FlashTime(void * pData, int Index, float Data); //unit: second.
INT8U SetCleanParam_EPSON_MICOLOR_FlashCycle(void * pData, int Index, float Data); //unit: second.
INT8U SetCleanParam_EPSON_MICOLOR_FlashIdleInCycle(void * pData, int Index, float Data); //unit: second.
//return value: 0~4: custom/strong/normal/weak/Refill.
INT8U GetCleanParam_EPSON_MICOLOR_Shortcuts(void * pData, int Index);
//Index: there are two part: the first is clean way, 0~4: custom/strong/normal/weak/Refill.
//  the second is, 0~3, clean step.
//  the final index = (clean way)*100+(clean step).
INT32S GetCleanParam_EPSON_MICOLOR_Type(void * pData, int Index);
INT32S GetCleanParam_EPSON_MICOLOR_LoopTimes(void * pData, int Index);
float GetCleanParam_EPSON_MICOLOR_SuckInkTime(void * pData, int Index); //unit is second.
float GetCleanParam_EPSON_MICOLOR_StayTime(void * pData, int Index); //unit is second.
float GetCleanParam_EPSON_MICOLOR_ReleaseTime(void * pData, int Index); //unit is second.
INT32S GetCleanParam_EPSON_MICOLOR_SuckSpeed(void * pData, int Index);
float GetCleanParam_EPSON_MICOLOR_FlashFreq(void * pData, int Index); //unit: KHz.
float GetCleanParam_EPSON_MICOLOR_FlashTime(void * pData, int Index); //unit: second.
float GetCleanParam_EPSON_MICOLOR_FlashCycle(void * pData, int Index); //unit: second.
float GetCleanParam_EPSON_MICOLOR_FlashIdleInCycle(void * pData, int Index); //unit: second.

//auto spray setting. share the same data structure with PM.
//Data: 0, off, 1, on.
INT8U SetCleanParam_EPSON_MICOLOR_AutoSpray_OnOff(void * pData, int Index);
//return value: 0, off, 1, on.
INT8U GetCleanParam_EPSON_MICOLOR_AutoSpray_OnOff(void * pData, int Index);
INT8U SetCleanParam_EPSON_MICOLOR_FILL_PROTECT_OnOff(void * pData, int Index);
INT8U GetCleanParam_EPSON_MICOLOR_FILL_PROTECT_OnOff(void * pData, int Index);
INT8U SetCleanParam_EPSON_MICOLOR_AutoSpray_Width(void * pData, int Index, float Data); //unit: Inch.
float GetCleanParam_EPSON_MICOLOR_AutoSpray_Width(void * pData, int Index); //unit: Inch.
INT8U SetCleanParam_EPSON_MICOLOR_AutoSpray_Pass(void * pData, int Index, int Data);
INT32S GetCleanParam_EPSON_MICOLOR_AutoSpray_Pass(void * pData, int Index); 
#endif

#endif

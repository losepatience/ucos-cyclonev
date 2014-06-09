#include "os_includes.h"
#include "miscellance.h"
#include "global.h"
#include "control.h"
#include "consol.h"
#include "status.h"
#include "fpga.h"
#include "math.h"
#include "clean.h"
#include "display.h"
#include "calibration.h"
#include "uv.h"

#include "pio/pio.h"
#include "pmc/pmc.h"
#include "tsadcc/tsadcc.h"

#ifdef PUMP_INTERMITTENT
#define PUMP_PERIOD 300			//泵墨周期
#define PUMP_TIME_RATIO 1 / 20	//泵墨时间比
#endif
//for epson allwin, we use EPSON_BOTTOM_BOARD_V2,
//  for allwin, we use PumpCtrl9(the old  PIN_PUMP_DRIVER_1_EN) as PIN_CLEAN_RELEASE_1.
//      use PumpCtrl10(the old  PIN_PUMP_DRIVER_2_EN) as PIN_CLEAN_SUCK_1.
//      use PumpCtrl7 as PIN_CLEAN_RELEASE_2.
//      use PumpCtrl8 as PIN_CLEAN_SUCK_2.
//  for micolor(temp), we use PumpCtrl7 as clean motor A+
//      use PumpCtrl8 as clean motor B+
//      use PumpCtrl9 as clean motor A-
//      use PumpCtrl10 as clean motor B-.
//      Note: A,B,+,- is our private defination. it may be different with the motor standard because we have NOT the document of this step motor.


const Pin BellPin = MB_CUST_PIN_BELL;
#if !(defined( EPSON_BOTTOM_BOARD_V2) || defined( EPSON_BOTTOM_BOARD_V2_1) || defined( EPSON_BOTTOM_BOARD_V3))
const Pin PumpDriver1Pin = PIN_PUMP_DRIVER_1_EN;
const Pin PumpDriver2Pin = PIN_PUMP_DRIVER_2_EN;
#endif
#if !(defined( EPSON_BOTTOM_BOARD_V2_1) || defined( EPSON_BOTTOM_BOARD_V3))
const Pin VacuumCleanerPin = MB_CUST_PIN_CLEANER;
#endif
//Fix pump bug;
//先让黑色泵墨， 在黑色没有泵满的情况下，再让CYAN泵墨，这个时候，CYAN和BLACK会同时泵墨，并且黑色会一直泵墨，
//直到CYAN停止泵墨. 对其他颜色也是这样. K->C->M->Y->Lc->Lm. 反序则不会出现此问题.

INT32U g_PumpControl = 0;	
#ifdef TIMER_ON
Timer control_timer;
#endif
#ifdef PUMP_INTERMITTENT
#define TIMER_BELL 0x0001
#define TIMER_PUMP 0x0002
#endif

#define PUMP_DELAY_TIME	3

static const Pin InkSupplyCtrl1Pin = PIN_INK_SUPPLY_CTRL1;
static const Pin InkSupplyCtrl2Pin = PIN_INK_SUPPLY_CTRL2;
static const Pin InkSupplyCtrl3Pin = PIN_INK_SUPPLY_CTRL3;
static const Pin InkSupplyCtrl4Pin = PIN_INK_SUPPLY_CTRL4;
static const Pin InkSupplyCtrl5Pin = PIN_INK_SUPPLY_CTRL5;
static const Pin InkSupplyCtrl6Pin = PIN_INK_SUPPLY_CTRL6;
static const Pin InkSupplyCtrl7Pin = PIN_INK_SUPPLY_CTRL7;
static const Pin InkSupplyCtrl8Pin = PIN_INK_SUPPLY_CTRL8;
#ifdef EPSON_BOTTOM_BOARD_V2
static const Pin InkSupplyCtrl9Pin = PIN_CLEAN_RELEASE_1;
static const Pin InkSupplyCtrl10Pin = PIN_CLEAN_SUCK_1;
#endif
#ifdef EPSON_BOTTOM_BOARD_V2_1
static const Pin InkSupplyCtrl9Pin = PIN_INK_SUPPLY_CTRL9;
static const Pin InkSupplyCtrl10Pin = PIN_INK_SUPPLY_CTRL10;
static const Pin InkSupplyCtrl11Pin = PIN_INK_SUPPLY_CTRL11;
static const Pin InkSupplyCtrl12Pin = PIN_INK_SUPPLY_CTRL12;
static const Pin InkSupplyCtrl13Pin = PIN_INK_SUPPLY_CTRL13;
static const Pin InkSupplyCtrl14Pin = PIN_INK_SUPPLY_CTRL14;
static const Pin InkSupplyCtrl15Pin = PIN_INK_SUPPLY_CTRL15;
#ifdef HEAD_RICOH_G4
static const Pin INK_K_LEVEL = PIN_TSAD3;
static const Pin INK_C_LEVEL = PIN_TSAD2;
static const Pin INK_M_LEVEL = PIN_TSAD1;
static const Pin INK_Y_LEVEL = PIN_TSAD0;
#endif 
#endif

#define BIT_MAX11614_SETUP_MASK (1<<7)
#define BIT_MAX11614_CONFIG_MASK (1<<5)
#define BIT_MAX11614_CS_AIN0  (0)
#define BIT_MAX11614_CS_AIN1  (1)
#define BIT_MAX11614_CS_AIN2  (2)
#define BIT_MAX11614_CS_AIN3  (3)
#ifdef EPSON_BOTTOM_BOARD_V3
#ifdef MICOLOR_AUTOFUCTION
static const Pin PlateFan1Pin = PIN_INK_SUPPLY_CTRL1;
static const Pin PlateFan2Pin = PIN_INK_SUPPLY_CTRL2;
static const Pin PlateFan3Pin = PIN_INK_SUPPLY_CTRL3;
static const Pin PlateFan4Pin = PIN_INK_SUPPLY_CTRL4;
#endif

#ifdef EPSON_CLEAN_UPDOWN
static const Pin CleanRelease1Pin = PIN_INK_SUPPLY_CTRL1;
static const Pin CleanSuck1Pin = PIN_INK_SUPPLY_CTRL2;
#endif
#elif defined( EPSON_BOTTOM_BOARD_V2)
#ifdef MICOLOR_AUTOFUCTION
static const Pin PlateFan1Pin = PIN_CLEAN_RELEASE_1;
static const Pin PlateFan2Pin = PIN_CLEAN_SUCK_1;
#endif
#ifdef EPSON_CLEAN_UPDOWN
#if (defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT))
static const Pin CleanSuck2Pin = PIN_CLEAN_RELEASE_1;
static const Pin CleanSuck3Pin = PIN_INK_SUPPLY_CTRL8;
static const Pin CleanSuck4Pin = PIN_INK_SUPPLY_CTRL7;
#else
static const Pin CleanRelease1Pin = PIN_CLEAN_RELEASE_1;
#endif
static const Pin CleanSuck1Pin = PIN_CLEAN_SUCK_1;


#if defined(EPSON_CLEAN_UPDOWN_WIPER_EX)
static const Pin CleanWipe1Pin = PIN_INK_SUPPLY_CTRL6;
static const Pin CleanWipe2Pin = PIN_INK_SUPPLY_CTRL3;
#endif

#endif
#if defined(EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)
static const Pin CleanWipe1Pin = PIN_INK_SUPPLY_CTRL6;
static const Pin CleanRelease1Pin = PIN_INK_SUPPLY_CTRL7;
static const Pin CleanSuck1Pin = PIN_INK_SUPPLY_CTRL8;
static const Pin CleanWipe2Pin = PIN_INK_SUPPLY_CTRL3;
static const Pin CleanRelease2Pin = PIN_INK_SUPPLY_CTRL4;
static const Pin CleanSuck2Pin = PIN_INK_SUPPLY_CTRL5;
#endif
#elif defined( EPSON_BOTTOM_BOARD_V2_1)
#ifdef EPSON_CLEAN_UPDOWN
#ifdef A1802 //gz
//static const Pin CleanRelease1Pin = PIN_INK_SUPPLY_CTRL2;
static const Pin CleanSuck1Pin = PIN_INK_SUPPLY_CTRL1;
//static const Pin CleanRelease2Pin = PIN_INK_SUPPLY_CTRL5;
static const Pin CleanSuck2Pin = PIN_INK_SUPPLY_CTRL2;
#ifdef EPSON_4H_CLEAN_SPLIT
static const Pin CleanSuck3Pin = PIN_INK_SUPPLY_CTRL3;
static const Pin CleanSuck4Pin = PIN_INK_SUPPLY_CTRL4;
#endif
#else
static const Pin CleanRelease1Pin = PIN_INK_SUPPLY_CTRL5;
static const Pin CleanSuck1Pin = PIN_INK_SUPPLY_CTRL3;
static const Pin CleanRelease2Pin = PIN_INK_SUPPLY_CTRL2;
static const Pin CleanSuck2Pin = PIN_INK_SUPPLY_CTRL6;
#endif
#endif

#ifdef  EPSON_CLEAN_UPDOWN_WIPER_EX
static const Pin CleanWipe1Pin = PIN_INK_SUPPLY_CTRL1;
static const Pin CleanWipe2Pin = PIN_INK_SUPPLY_CTRL4;
#endif

#if defined(EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)
static const Pin CleanWipe1Pin = PIN_INK_SUPPLY_CTRL1;
static const Pin CleanRelease1Pin = PIN_INK_SUPPLY_CTRL2;
static const Pin CleanSuck1Pin = PIN_INK_SUPPLY_CTRL3;
static const Pin CleanWipe2Pin = PIN_INK_SUPPLY_CTRL4;
static const Pin CleanRelease2Pin = PIN_INK_SUPPLY_CTRL5;
static const Pin CleanSuck2Pin = PIN_INK_SUPPLY_CTRL6;
#endif
#else
static const Pin CleanRelease1Pin = PIN_INK_SUPPLY_CTRL7;
static const Pin CleanSuck1Pin = PIN_INK_SUPPLY_CTRL8;
#endif

#if defined(MICOLOR_AUTOFUCTION)
#ifdef  EPSON_BOTTOM_BOARD_V2 
static const Pin LoadMediaSensorPin = PIN_EPSON_LOAD_MEDIA;
static const Pin MediaFixedSensorPin = PIN_EPSON_MEDIA_FIXED;
static const Pin LackMediaSensorPin = PIN_EPSON_LACK_OF_MEDIA;
static const Pin CoverSensorPin = PIN_EPSON_COVER;
#elif defined(EPSON_BOTTOM_BOARD_V3)
#define FPGA_EXPAND_IOMASK_EPSON_LOAD_MEDIA		0x8//0x10
#define FPGA_EXPAND_IOMASK_EPSON_MEDIA_FIXED	0x20
#define FPGA_EXPAND_IOMASK_EPSON_LACK_OF_MEDIA	0x10//0x8
#define FPGA_EXPAND_IOMASK_EPSON_COVER			0x40
#else
static const Pin LoadMediaSensorPin = {0,0,0,0,0};
static const Pin MediaFixedSensorPin = {0,0,0,0,0};
static const Pin LackMediaSensorPin = {0,0,0,0,0};
static const Pin CoverSensorPin = {0,0,0,0,0};
static const Pin PlateFan1Pin = {0,0,0,0,0};
static const Pin PlateFan2Pin = {0,0,0,0,0};
#endif

#endif
extern void PUMP_OPEN(INT8U mask);
//------------------------------------- PUMP_TYPE 1: 8 Color-----------------------------
#if !(defined(HEAD_RICOH_G4)||defined(EPSON_4H))&&(defined(MB_LEVEL_SENSOR)||defined(HEAD_LEVEL_SENSOR))
#define PUMP_K_CLOSE() {\
	PIO_Set(&InkSupplyCtrl3Pin);\
		status_ReportStatus(STATUS_WAR_PUMP_BLACK, STATUS_CLR);\
}

#define PUMP_C_CLOSE() {\
	PIO_Set(&InkSupplyCtrl6Pin);\
		status_ReportStatus(STATUS_WAR_PUMP_CYAN, STATUS_CLR);\
}

#define PUMP_M_CLOSE() {\
	PIO_Set(&InkSupplyCtrl5Pin);\
		status_ReportStatus(STATUS_WAR_PUMP_MAGENTA, STATUS_CLR);\
}

#define PUMP_LC_CLOSE() {\
	PIO_Set(&InkSupplyCtrl2Pin);\
		status_ReportStatus(STATUS_WAR_PUMP_LIGHTCYAN, STATUS_CLR);\
}

#define PUMP_LM_CLOSE() {\
	PIO_Set(&InkSupplyCtrl1Pin);\
		status_ReportStatus(STATUS_WAR_PUMP_LIGHTMAGENTA, STATUS_CLR);\
}

#define PUMP_Y_CLOSE() {\
	PIO_Set(&InkSupplyCtrl4Pin);\
		status_ReportStatus(STATUS_WAR_PUMP_YELLOW, STATUS_CLR);\
}

#define PUMP_7_CLOSE() {\
	PIO_Set(&InkSupplyCtrl7Pin);\
		status_ReportStatus(STATUS_WAR_PUMP_SPOTCOLOR1, STATUS_CLR);\
}

#define PUMP_8_CLOSE() {\
	PIO_Set(&InkSupplyCtrl8Pin);\
		status_ReportStatus(STATUS_WAR_PUMP_SPOTCOLOR2, STATUS_CLR);\
}

#define PUMP_8_OPEN()  {\
	PIO_Clear(&InkSupplyCtrl8Pin);\
}

#define PUMP_7_OPEN()  {\
	PIO_Clear(&InkSupplyCtrl7Pin);\
}

#define PUMP_LM_OPEN()  {\
	PIO_Clear(&InkSupplyCtrl1Pin);\
}

#define PUMP_LC_OPEN()  {\
	PIO_Clear(&InkSupplyCtrl2Pin);\
}
#define PUMP_K_OPEN() {\
	PIO_Clear(&InkSupplyCtrl3Pin);\
}

#define PUMP_M_OPEN()  {\
	PIO_Clear(&InkSupplyCtrl5Pin);\
}

#define PUMP_C_OPEN()  {\
	PIO_Clear(&InkSupplyCtrl6Pin);\
}

#define PUMP_Y_OPEN() {\
	PIO_Clear(&InkSupplyCtrl4Pin);\
}
#endif
#if (defined(HEAD_RICOH_G4)||defined(EPSON_4H))
#ifdef MANUFACTURER_DOCAN_UV
#define INK_S2_BITS (1)
#define INK_S1_BITS (1<<1)
#define INK_K_BITS (1<<2)
#define INK_Y_BITS (1<<3)
#define INK_M_BITS (1<<4)
#define INK_C_BITS (1<<5)
#define INK_S3_BITS (1<<6)
#define INK_S4_BITS (1<<7)
#else
#define INK_K_BITS (1)
#define INK_C_BITS (1<<1)
#define INK_M_BITS (1<<2)
#define INK_Y_BITS (1<<3)
#define INK_S1_BITS (1<<4)
#define INK_S2_BITS (1<<5)
#define INK_S3_BITS (1<<6)
#define INK_S4_BITS (1<<7)
#endif
#define INK_LOW 1
INT32U INK_LEVEL_MASK = 0;
#endif 
extern const Pin LVDSLockPin;
#if defined(EPSON_BOTTOM_BOARD_V3)||defined(EPSON_BOTTOM_BOARD_V2_1) 
INT8U IIC_Write_Max11614(INT8U subaddr, INT8U data, INT8U pending);
float IIC_Read_Max11614(INT8U Flag);
volatile struct sHeaterInfo HeaterInfo;

//Pt100, 0~99 degree. ohm.
#define PT100_TABLE_LEN 100
static float PT100Table[PT100_TABLE_LEN] = {
	100.00,100.39,100.78,101.17,101.56,101.95,102.34,102.73,103.12,103.51, 
	103.90,104.29,104.68,105.07,105.46,105.85,106.24,106.63,107.02,107.40, 
	107.79,108.18,108.57,108.96,109.35,109.73,110.12,110.51,110.90,111.29, 
	111.67,112.06,112.45,112.83,113.22,113.61,114.00,114.38,114.77,115.15, 
	115.54,115.93,116.31,116.70,117.08,117.47,117.86,118.24,118.63,119.01, 
	119.40,119.78,120.17,120.55,120.94,121.32,121.71,122.09,122.47,122.86, 
	123.24,123.63,124.01,124.39,124.78,125.16,125.54,125.93,126.31,126.69, 
	127.08,127.46,127.84,128.22,128.61,128.99,129.37,129.75,130.13,130.52, 
	130.90,131.28,131.66,132.04,132.42,132.80,133.18,133.57,133.95,134.33, 
	134.71,135.09,135.47,135.85,136.23,136.61,136.99,137.37,137.75,138.13
};

static volatile INT8U HeaterEnable[MAX_HEATER_NUM] = {0};
INT8U TestHeaterEnable[MAX_HEATER_NUM] = {0};
float AverResistance[MAX_HEATER_NUM];
INT8U ResetHeaterInfo()
{
	HeaterInfo.len = sizeof(HeaterInfo);
	HeaterInfo.version = HEATER_INFO_VERSION;
	
	HeaterInfo.HeaterOnMask = 0; //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
	HeaterEnable[HEATER_PRE_INDEX] = False;
	HeaterEnable[HEATER_MIDDLE_INDEX] = False;
	HeaterEnable[HEATER_POST_INDEX] = False;
	
	HeaterInfo.TargetTemp[HEATER_PRE_INDEX] = 40;
	HeaterInfo.TargetTemp[HEATER_MIDDLE_INDEX] = 40;
	HeaterInfo.TargetTemp[HEATER_POST_INDEX] = 40;
	
	//for WJ1545
	//    HeaterInfo.TempOffset[HEATER_PRE_INDEX] = -6;
	//    HeaterInfo.TempOffset[HEATER_MIDDLE_INDEX] = -5.5;
	//    HeaterInfo.TempOffset[HEATER_POST_INDEX] = -7.5;
	//for weak ink, M7836
	HeaterInfo.ResistanceOffset[HEATER_PRE_INDEX] = -1; //25.4 - 39.5; //109.9 - 115.35
	HeaterInfo.ResistanceOffset[HEATER_MIDDLE_INDEX] = -1; //25.2 - 36.6; //109.8- 114.2
	HeaterInfo.ResistanceOffset[HEATER_POST_INDEX] = -1; //25.4 - 36.6; //109.85 - 114.2
	HeaterInfo.ResistanceOffset[TMP_ENVIRONMENT_INDEX] = -1; //25.4 - 36.6; //109.85 - 114.2
	//for water ink, C20118. 160cm width
	// HeaterInfo.ResistanceOffset[HEATER_PRE_INDEX] = 27.4 - 33.9; //only measure pre heater.
	// HeaterInfo.ResistanceOffset[HEATER_MIDDLE_INDEX] = 27.4 - 33.9;
	//  HeaterInfo.ResistanceOffset[HEATER_POST_INDEX] = 27.4 - 33.9;
	
	HeaterInfo.adcRefV = 3.3;  //3.3V
	HeaterInfo.adcSharedResistance[HEATER_PRE_INDEX] = 120;
	HeaterInfo.adcSharedResistance[HEATER_MIDDLE_INDEX] = 120;
	HeaterInfo.adcSharedResistance[HEATER_POST_INDEX] = 120;
	HeaterInfo.adcSharedResistance[TMP_ENVIRONMENT_INDEX] = 120;
	//		thermistorR = (Vref*data/0x3FF*120)/(temp_Vref - Vref*data/0x3FF);
	
	return True;
}

INT8U ReadHeaterInfo()
{
	INT8U err;
	INT16U length;
	EPM_Head head;
	
	length = sizeof(struct sHeaterInfo) + sizeof(EPM_Head);
	if(length > EPR_HEATER_PARAM_SIZE)
		return False;
	
	if(!IIC_ReadEEPRom_Head(EPR_HEATER_PARAM_OFFSET, &head))
	{
		return False;
	}
	
	memset((void*)&HeaterInfo, 0, sizeof(struct sHeaterInfo));
	if(head.flag == HEATER_INFO_FLAG)
	{
		if(!IIC_ReadEEPRom_Struct(EPR_HEATER_PARAM_OFFSET + sizeof(EPM_Head), 
								  (INT8U *)&HeaterInfo, sizeof(struct sHeaterInfo), &head))
		{
			return False;
		}
		
		if( HeaterInfo.len != sizeof(struct sHeaterInfo) || HeaterInfo.version != HEATER_INFO_VERSION)
		{
			HeaterInfo.len = sizeof(struct sHeaterInfo);
			HeaterInfo.version = HEATER_INFO_VERSION;
			
			return SaveHeaterInfo();
		}
	}
	else
	{
		ResetHeaterInfo();
		return SaveHeaterInfo();
	}
	
	return True;
}

INT8U SaveHeaterInfo()
{
	INT8U ret = True;
	INT16U len = sizeof(struct sHeaterInfo);
	
	EPM_Head head = 
	{
		HEATER_INFO_FLAG,
		HEATER_INFO_VERSION,
		sizeof(struct sHeaterInfo) + sizeof(EPM_Head),
		0
	};
	
	CalcCheckSum((INT8U*)&HeaterInfo, &head);
	ret = IIC_WriteEEPRom_Head(EPR_HEATER_PARAM_OFFSET, &head);
	
	ret &= IIC_WriteEEPRom_Ex(EPR_HEATER_PARAM_OFFSET + sizeof(EPM_Head), (INT8U *)&HeaterInfo, &len);
	return ret;
}

static const Pin HeaterPrePin = PIN_INK_SUPPLY_CTRL7;
#if defined(EPSON_BOTTOM_BOARD_V3)
static const Pin HeaterMiddlePin = PIN_INK_SUPPLY_CTRL6;
static const Pin HeaterPostPin = PIN_INK_SUPPLY_CTRL5;
#elif defined(EPSON_BOTTOM_BOARD_V2_1)
static const Pin HeaterMiddlePin = PIN_INK_SUPPLY_CTRL8;
static const Pin HeaterPostPin = PIN_INK_SUPPLY_CTRL9;
#else
#error
#endif

INT8U Heater_Init()
{
	//check HeaterMiddle switch.
	//if(IsWeakSolventPrinter())
	{
		//        if(EPSON_HEADNUM > 1)
		//        {
		//            HeaterInfo.HeaterOnMask &= ~(INT32U)0x2; //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		//        }
		HeaterEnable[HEATER_PRE_INDEX] = Menu_GetHeaterPre(NULL);
		HeaterEnable[HEATER_MIDDLE_INDEX] = Menu_GetHeaterMiddle(NULL);
		HeaterEnable[HEATER_POST_INDEX] = Menu_GetHeaterPost(NULL);
#ifdef EPSON_BOTTOM_BOARD_V2_1
		IIC_Write_Max11614(1,BIT_MAX11614_SETUP_MASK|1<<1,True);
		IIC_Write_Max11614(1,BIT_MAX11614_CONFIG_MASK|1,True);		
#else
		PMC_EnablePeripheral(AT91C_ID_TSADC);
		AT91C_BASE_TSADC->TSADC_WPMR = 0x54534100;
		
		TSADCC_SetOperatingMode(AT91C_TSADC_TSAMOD_ADC_ONLY_MODE);
		TSADCC_SetLowResolution(False);
		TSADCC_SetSleepMode(False);
		TSADCC_SetPenDetect(False);
		AT91C_BASE_TSADC->TSADC_MR = ((  AT91C_BASE_TSADC->TSADC_MR & ~AT91C_TSADC_PRESCAL) | (15 << 8)) |  //MCK/32,about 4MHz.
			((  AT91C_BASE_TSADC->TSADC_MR & ~AT91C_TSADC_STARTUP) | (8 << 16)) | //8 clock, about 2us.
				((  AT91C_BASE_TSADC->TSADC_MR & ~AT91C_TSADC_SHTIM) | (8 << 24)); //8 clock, about 2us.
		
		AT91C_BASE_TSADC->TSADC_WPMR = 0x54534101;
#endif
		PIO_Configure(&HeaterPrePin, PIO_LISTSIZE(HeaterPrePin));
		PIO_Configure(&HeaterMiddlePin, PIO_LISTSIZE(HeaterMiddlePin));
		//if(EPSON_HEADNUM == 1)
		PIO_Configure(&HeaterPostPin, PIO_LISTSIZE(HeaterPostPin));
		HeaterInfo.adcSharedResistance[HEATER_PRE_INDEX] = 120;
		HeaterInfo.adcSharedResistance[HEATER_MIDDLE_INDEX] = 120;
		HeaterInfo.adcSharedResistance[HEATER_POST_INDEX] = 120;
		HeaterInfo.adcSharedResistance[TMP_ENVIRONMENT_INDEX] = 120;
	}
	return True;
}

INT8U Heater_Urgent_Stop()
{
	HeaterEnable[HEATER_PRE_INDEX] = False;
	HeaterEnable[HEATER_MIDDLE_INDEX] = False;
	HeaterEnable[HEATER_POST_INDEX] = False;
	return True;
}

INT8U Menu_SetHeaterPre(void * pData, int Index)
{
	if(pData == (void *)-1)
		return True;
	
	if(Index)
	{
		HeaterInfo.HeaterOnMask |= 1; //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		HeaterEnable[HEATER_PRE_INDEX] = True;
#if defined(QICAIHONG)
		Control_OpenCose_Led(0, True);
#endif
	}
	else
	{
		HeaterInfo.HeaterOnMask &= ~(INT32U)0x1; //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		HeaterEnable[HEATER_PRE_INDEX] = False;
#if defined(QICAIHONG)
		Control_OpenCose_Led(0, False);
#endif
	}
	
	return SaveHeaterInfo();
}

INT8U Menu_GetHeaterPre(void * pData)
{
	INT8U ret;
	if(HeaterInfo.HeaterOnMask & 1) //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		ret = True; 
	else
		ret = False; 
	
	return ret;
}
INT8U Menu_SetHeaterMiddle(void * pData, int Index)
{
	if(pData == (void *)-1)
		return True;
	
	if(Index)
	{
		HeaterInfo.HeaterOnMask |= 2; //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		HeaterEnable[HEATER_MIDDLE_INDEX] = True;
#if defined(QICAIHONG)
		Control_OpenCose_Led(1, True);
#endif

	}
	else
	{
		HeaterInfo.HeaterOnMask &= ~(INT32U)0x2; //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		HeaterEnable[HEATER_MIDDLE_INDEX] = False;
#if defined(QICAIHONG)
		Control_OpenCose_Led(1, False);
#endif

	}
	
	return SaveHeaterInfo();
}
INT8U Menu_GetHeaterMiddle(void * pData)
{
	INT8U ret;
	if(HeaterInfo.HeaterOnMask & 2) //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		ret = True; 
	else
		ret = False; 
	
	return ret;
}
INT8U Menu_SetHeaterPost(void * pData, int Index)
{
	if(pData == (void *)-1)
		return True;
	
	if(Index)
	{
		HeaterInfo.HeaterOnMask |= 4; //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		HeaterEnable[HEATER_POST_INDEX] = True;
#if defined(QICAIHONG)
		Control_OpenCose_Led(2, True);
#endif

	}
	else
	{
		HeaterInfo.HeaterOnMask &= ~(INT32U)0x4; //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		HeaterEnable[HEATER_POST_INDEX] = False;
#if defined(QICAIHONG)
		Control_OpenCose_Led(2, False);
#endif

	}
	
	return SaveHeaterInfo();
}
INT8U Menu_GetHeaterPost(void * pData)
{
	INT8U ret;
	if(HeaterInfo.HeaterOnMask & 4) //bit 0 is pre heater, bit 1 is middle heater, bit 2 is post heater.
		ret = True; 
	else
		ret = False; 
	
	return ret;
}
INT8U Menu_SetHeaterTempPre(void * pData, int Index, float Data) //unit:`C
{
	if(pData == (void *)-1)
		return True;
	
	if(Data > 50.0)
		Data = 50.0;
	if(Data < 20.0)
		Data = 20.0;
	HeaterInfo.TargetTemp[0] = Data;
	return SaveHeaterInfo();
}
float Menu_GetHeaterTempPre(void * pData, int Index) //unit:`C
{
	return HeaterInfo.TargetTemp[0];
}
INT8U Menu_SetHeaterTempMiddle(void * pData, int Index, float Data) //unit:`C
{
	if(pData == (void *)-1)
		return True;
	
	if(Data > 50.0)
		Data = 50.0;
	if(Data < 20.0)
		Data = 20.0;
	HeaterInfo.TargetTemp[1] = Data;
	return SaveHeaterInfo();
}
float Menu_GetHeaterTempMiddle(void * pData, int Index) //unit:`C
{
	return HeaterInfo.TargetTemp[1];
}
INT8U Menu_SetHeaterTempPost(void * pData, int Index, float Data) //unit:`C
{
	if(pData == (void *)-1)
		return True;
	
	if(Data > 50.0)
		Data = 50.0;
	if(Data < 20.0)
		Data = 20.0;
	HeaterInfo.TargetTemp[2] = Data;
	return SaveHeaterInfo();
}
float Menu_GetHeaterTempPost(void * pData, int Index) //unit:`C
{
	return HeaterInfo.TargetTemp[2];
}

float RealTemp[MAX_HEATER_NUM] = {0};
INT8U Menu_SetRealHeaterTempPre(void * pData, int Index, float Data)
{
	if(pData == (void *)-1)
		return False;
	return False;
}
float Menu_GetRealHeaterTempPre(void * pData, int Index)
{
	return RealTemp[HEATER_PRE_INDEX];
}
INT8U Menu_SetRealHeaterTempMiddle(void * pData, int Index, float Data)
{
	if(pData == (void *)-1)
		return False;
	return False;
}
float Menu_GetRealHeaterTempMiddle(void * pData, int Index)
{
	return RealTemp[HEATER_MIDDLE_INDEX];
}
INT8U Menu_SetRealHeaterTempPost(void * pData, int Index, float Data)
{
	if(pData == (void *)-1)
		return False;
	return False;
}
float Menu_GetRealHeaterTempPost(void * pData, int Index)
{
	return RealTemp[HEATER_POST_INDEX];
}
INT8U Menu_SetRealHeaterTempEnv(void * pData, int Index, float Data)
{
	if(pData == (void *)-1)
		return False;
	return False;
}
float Menu_GetRealHeaterTempEnv(void * pData, int Index)
{
	return RealTemp[TMP_ENVIRONMENT_INDEX];
}

#define HEATER_PRE_INPUT_CHANNEL    AT91C_TSADC_CHENA1
#define HEATER_MIDDLE_INPUT_CHANNEL    AT91C_TSADC_CHENA3
#define HEATER_POST_INPUT_CHANNEL    AT91C_TSADC_CHENA2
#define TMP_ENVIRONMENT_INPUT_CHANNEL    AT91C_TSADC_CHENA0

#define SAMPLE_COUNT    14
#define TEMP_RANGE  1.2
#define TEMP_ENV_OFFSET 2.0

float Temp[MAX_HEATER_NUM] = {0};
INT8U is_power_on_heater[MAX_HEATER_NUM - 1]  = {FALSE,FALSE,FALSE};
float max_heater[MAX_HEATER_NUM - 1] = {0};
float min_heater[MAX_HEATER_NUM - 1] = {PT100_TABLE_LEN,PT100_TABLE_LEN,PT100_TABLE_LEN};
float first_temp[MAX_HEATER_NUM - 1] = {-1,-1,-1};
float start_cool_temp[MAX_HEATER_NUM - 1] = {0};
INT32U start_heat_time[MAX_HEATER_NUM - 1] = {0};
INT32U start_cool_time[MAX_HEATER_NUM - 1] = {0};
INT32U heating_time[MAX_HEATER_NUM - 1] = {0};
INT32U cooling_time[MAX_HEATER_NUM - 1] = {0};
signed char Heater_PIO_Get(INT8U index)
{
	if(index == HEATER_PRE_INDEX)
		return PIO_Get(&HeaterPrePin);
	else if(index == HEATER_MIDDLE_INDEX)
		return PIO_Get(&HeaterMiddlePin);
	else
		return PIO_Get(&HeaterPostPin);
}
void Heater_PIO_Clear(INT8U index)
{
	if(index == HEATER_PRE_INDEX)
		PIO_Clear(&HeaterPrePin);
	else if(index == HEATER_MIDDLE_INDEX)
		PIO_Clear(&HeaterMiddlePin);
	else
		PIO_Clear(&HeaterPostPin);
#if defined(QICAIHONG)
		Control_OpenCose_Led(index, False);
#endif
}
void Heater_PIO_Set(INT8U index)
{
	if(index == HEATER_PRE_INDEX)
		PIO_Set(&HeaterPrePin);
	else if(index == HEATER_MIDDLE_INDEX)
		PIO_Set(&HeaterMiddlePin);
	else
		PIO_Set(&HeaterPostPin);
#if defined(QICAIHONG)
		Control_OpenCose_Led(index, True);
#endif
}

//float tempbuf[0xC00] = {0};
//int tempbuf_i = 0;
//INT16U Rawbuf[0xC00*2] = {0};

void Control_Task (void *data)
{
	INT8U err;
	OS_FLAGS bellFlags;
	INT32S beepTick = 0;
	INT8U FanToggle = False;
	float vol = 0;
	float ratio[MAX_HEATER_NUM - 1] = {-1,-1,-1};
	if (PIO_Get(&LVDSLockPin) == 1)
	{
		Control_SetAlarm(ALARM_TYPE_LVDSLOST, ALARM_CLR);
		//			CONSOL_Printf("LVDS is Relocked!.r\n");
	}	
	else
	{
		//remove for slient
		Control_SetAlarm(ALARM_TYPE_LVDSLOST, ALARM_SET);
		CONSOL_Printf("LVDS is Unlocked!\r\n");	
	}
	while(!OSFlagAccept(status_FLAG_GRP, STATUS_INI, OS_FLAG_WAIT_CLR_ALL,&err))
	{
		bellFlags = OSFlagAccept(mix_FLAG_GRP, MISC_FLAGS_BELL|MISC_FLAGS_BELLLVDS|MISC_FLAGS_BELLMENU, OS_FLAG_WAIT_SET_ANY,&err);
		if (bellFlags & MISC_FLAGS_BELL)
		{
			BELL_ON();
			OSTimeDly(300);
			BELL_OFF();
			OSTimeDly(500);
		}else if (bellFlags & MISC_FLAGS_BELLLVDS)
		{
			BELL_ON();
			OSTimeDly(100);
			BELL_OFF();
			OSTimeDly(200);			
		}
		else if(bellFlags & MISC_FLAGS_BELLMENU)
		{
			BELL_ON();
			OSTimeDly(500);
			BELL_OFF();
			//OSTimeDly(500);			
			OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_BELLMENU, OS_FLAG_CLR, &err);
		}
	}
	
	while (1)
	{
		bellFlags = 0;
#ifdef HEAD_EPSON_GEN5	
		INT8U i,j;
		
		INT16U RawTemp[MAX_HEATER_NUM][SAMPLE_COUNT];
		float AverTemp[MAX_HEATER_NUM] = {0.0f};
		INT8U MyEnable[MAX_HEATER_NUM];
		INT8U TestMyEnable[MAX_HEATER_NUM];
		
		MyEnable[HEATER_PRE_INDEX] = HeaterEnable[HEATER_PRE_INDEX];
		MyEnable[HEATER_MIDDLE_INDEX] = HeaterEnable[HEATER_MIDDLE_INDEX];
		MyEnable[HEATER_POST_INDEX] = HeaterEnable[HEATER_POST_INDEX];
		TestMyEnable[HEATER_PRE_INDEX] = TestHeaterEnable[HEATER_PRE_INDEX];
		TestMyEnable[HEATER_MIDDLE_INDEX] = TestHeaterEnable[HEATER_MIDDLE_INDEX];
		TestMyEnable[HEATER_POST_INDEX] = TestHeaterEnable[HEATER_POST_INDEX];
		TestMyEnable[TMP_ENVIRONMENT_INDEX] = TestHeaterEnable[TMP_ENVIRONMENT_INDEX];
#if defined(EPSON_BOTTOM_BOARD_V3)       
		//get voltage;
		for(i=0; i<SAMPLE_COUNT; i++)
		{
			AT91C_BASE_TSADC->TSADC_CHER |= HEATER_PRE_INPUT_CHANNEL;
			AT91C_BASE_TSADC->TSADC_CHER |= HEATER_MIDDLE_INPUT_CHANNEL;
			AT91C_BASE_TSADC->TSADC_CHER |= HEATER_POST_INPUT_CHANNEL;
			AT91C_BASE_TSADC->TSADC_CHER |= TMP_ENVIRONMENT_INPUT_CHANNEL;
			
			AT91C_BASE_TSADC->TSADC_CR = AT91C_TSADC_START;
			
			while(!(AT91C_BASE_TSADC->TSADC_SR & HEATER_PRE_INPUT_CHANNEL));
			while(!(AT91C_BASE_TSADC->TSADC_SR & HEATER_MIDDLE_INPUT_CHANNEL));
			while(!(AT91C_BASE_TSADC->TSADC_SR & HEATER_POST_INPUT_CHANNEL));
			while(!(AT91C_BASE_TSADC->TSADC_SR & TMP_ENVIRONMENT_INPUT_CHANNEL));
			
			RawTemp[HEATER_PRE_INDEX][i] = AT91C_BASE_TSADC->TSADC_CDR1;
			RawTemp[HEATER_MIDDLE_INDEX][i] = AT91C_BASE_TSADC->TSADC_CDR3;
			RawTemp[HEATER_POST_INDEX][i] = AT91C_BASE_TSADC->TSADC_CDR2;
			RawTemp[TMP_ENVIRONMENT_INDEX][i] = AT91C_BASE_TSADC->TSADC_CDR0;
			
			//Rawbuf[tempbuf_i] = RawTemp[HEATER_MIDDLE_INDEX][i];
			//tempbuf_i ++;
			//if(tempbuf_i >= sizeof(Rawbuf)/sizeof(INT32U))
			//    tempbuf_i = 0;
		}
		
		INT8U index = 0;
		while(index < MAX_HEATER_NUM )
		{
			INT16U total = 0;
			for(i=0; i<2; i++)
			{
				for(j=i+1;j<SAMPLE_COUNT;j++)
				{
					if(RawTemp[index][i] < RawTemp[index][j])
					{
						INT16U t = RawTemp[index][i];
						RawTemp[index][i] = RawTemp[index][j];
						RawTemp[index][j] = t;
					}
				}
			}
			
			for(i = SAMPLE_COUNT - 1; i > SAMPLE_COUNT - 3; i--)
			{
				for(j=2;j<i;j++)
				{
					if(RawTemp[index][i] > RawTemp[index][j])
					{
						INT16U t = RawTemp[index][i];
						RawTemp[index][i] = RawTemp[index][j];
						RawTemp[index][j] = t;
					}
				}
			}
			//remove 2 largest and 2 smallest temp.
			for(i=2; i<SAMPLE_COUNT-2; i++)
			{
				total += RawTemp[index][i];
			}
			
			if(index != TMP_ENVIRONMENT_INDEX)
			{
				//check if temp sensor is connected.
				if(total/(SAMPLE_COUNT-4) >= 0x3F0)
				{
					MyEnable[index] = False;
				}
			}
			AverTemp[index] = (float)total/(SAMPLE_COUNT-4);
			
			//get temp
			//if(index == TMP_ENVIRONMENT_INDEX || MyEnable[index] ||TestMyEnable[index])
			{
				float thermistorR = HeaterInfo.adcSharedResistance[index] *
					AverTemp[index]/((float)0x3FF-AverTemp[index]);
				if(TestMyEnable[index])
				{
					AverResistance[index] = thermistorR;
					TestHeaterEnable[index] = FALSE;
				}
				thermistorR += HeaterInfo.ResistanceOffset[index];
				for(i=0; i < PT100_TABLE_LEN; i++)
				{
					if(thermistorR < PT100Table[i])
					{
						break;
					}
				}
				if(i == 0)
					RealTemp[index] = Temp[index] = 0.0;
				else if (i==PT100_TABLE_LEN)
				{
					if(index != TMP_ENVIRONMENT_INDEX)
						Temp[index] = PT100_TABLE_LEN - 1;
					else
						Temp[TMP_ENVIRONMENT_INDEX] = 20; //not connect, we assume that enviroment temperature is 20 degree.
					RealTemp[index] = PT100_TABLE_LEN - 1;
				}
				else
				{
					RealTemp[index] = Temp[index] = (float)i - 1 + (thermistorR - PT100Table[i-1])/(PT100Table[i] - PT100Table[i-1]);
				}
			}
			index++;
		}
#elif defined(EPSON_BOTTOM_BOARD_V2_1) 
#ifdef MANUFACTURER_BEMAJET //only work for BEMAJET
		AverTemp[HEATER_PRE_INDEX] = IIC_Read_Max11614(BIT_MAX11614_CS_AIN0);
		AverTemp[HEATER_MIDDLE_INDEX] = IIC_Read_Max11614(BIT_MAX11614_CS_AIN1);
		AverTemp[HEATER_POST_INDEX] = IIC_Read_Max11614(BIT_MAX11614_CS_AIN2);
		AverTemp[TMP_ENVIRONMENT_INDEX] = IIC_Read_Max11614(BIT_MAX11614_CS_AIN3);
#endif			
		INT8U index = 0;
		while(index < MAX_HEATER_NUM )
		{
			float thermistorR = HeaterInfo.adcSharedResistance[index] *
				AverTemp[index]/((float)5.0-AverTemp[index]);
			if(TestMyEnable[index])
			{
				AverResistance[index] = thermistorR;
				TestHeaterEnable[index] = FALSE;
			}
			thermistorR += HeaterInfo.ResistanceOffset[index];
			for(i=0; i < PT100_TABLE_LEN; i++)
			{
				if(thermistorR < PT100Table[i])
				{
					break;
				}
			}
			if(i == 0)
				RealTemp[index] = Temp[index] = 0.0;
			else if (i==PT100_TABLE_LEN)
			{
				if(index != TMP_ENVIRONMENT_INDEX)
					Temp[index] = PT100_TABLE_LEN - 1;
				else
					Temp[TMP_ENVIRONMENT_INDEX] = 20; //not connect, we assume that enviroment temperature is 20 degree.
				RealTemp[index] = PT100_TABLE_LEN - 1;
			}
			else
			{
				RealTemp[index] = Temp[index] = (float)i - 1 + (thermistorR - PT100Table[i-1])/(PT100Table[i] - PT100Table[i-1]);
			}
			index++;
		}
		//Temp[TMP_ENVIRONMENT_INDEX] = 20; 
#endif					
		//        tempbuf[tempbuf_i] = RealTemp[1];
		//        tempbuf_i ++;
		//        if(tempbuf_i >= sizeof(tempbuf)/sizeof(float))
		//            tempbuf_i = 0;
		
		//control output.
		float tmp = 0;
		float Env_Offset = 0;
		index = 0;
		while(index < MAX_HEATER_NUM -1)
		{
			if(MyEnable[index])
			{
				if(first_temp[index] <0)
					first_temp[index] = Temp[index];
				
				if(heating_time[index] == 0 )
				{
					if(first_temp[index] > HeaterInfo.TargetTemp[index])
						tmp = HeaterInfo.TargetTemp[index] - Temp[TMP_ENVIRONMENT_INDEX];
					else
					{
						tmp = (HeaterInfo.TargetTemp[index] + first_temp[index])/2 - Temp[TMP_ENVIRONMENT_INDEX];
						if(tmp < 0)
							tmp = 0;
					}
				}
				else
					tmp = HeaterInfo.TargetTemp[index] - Temp[TMP_ENVIRONMENT_INDEX];
				if(index != HEATER_MIDDLE_INDEX)
				{
					if(tmp >= 0 && tmp < 10)
						Env_Offset =  tmp*0.2;
					else if(tmp >= 10 && tmp < 20)
						Env_Offset = (tmp - 10)*0.3 + 2;
					else if(tmp >= 20)
						Env_Offset = (tmp - 20)*0.4 + 5;
					else
						Env_Offset =  2.0;
				}
				else
				{
					if(tmp >= 0 && tmp < 10)
						Env_Offset =  tmp*0.2;
					else if(tmp >= 10 && tmp < 20)
						Env_Offset = (tmp - 10)*0.3 + 2;
					else if(tmp >= 20)
						Env_Offset = (tmp - 20)*0.4 + 5;
					else
						Env_Offset =  2.0;
				}
				
				if( Heater_PIO_Get(index) == 1)
				{
					if(heating_time[index] == 0 )
					{
						float temp_target;
						if( first_temp[index] > HeaterInfo.TargetTemp[index])
							temp_target = HeaterInfo.TargetTemp[index];
						else
							temp_target = (HeaterInfo.TargetTemp[index] + first_temp[index])/2 ;
						if( Temp[index] > temp_target + Env_Offset)
						{
							Heater_PIO_Clear(index);
							is_power_on_heater[index] = FALSE;
							max_heater[index] = 0;
							min_heater[index] = Temp[index];
							heating_time[index] = OSTimeGet() - start_heat_time[index];
							start_cool_time[index] = OSTimeGet();
							start_cool_temp[index] = Temp[index];
						}
					}
					else if( Temp[index] > HeaterInfo.TargetTemp[index] + Env_Offset)
					{
						Heater_PIO_Clear(index);
						is_power_on_heater[index] = FALSE;
						max_heater[index] = 0;
						min_heater[index] = Temp[index];
						heating_time[index] = OSTimeGet() - start_heat_time[index];
					}
					else
					{
						INT32U curr_permit_heattime;
						
						Heater_PIO_Set(index);
						is_power_on_heater[index] = TRUE;
						if(ratio[index] <0)
							ratio[index] = heating_time[index]/(max_heater[index]-min_heater[index]);
						
						ratio[index] = (heating_time[index]/(max_heater[index]-min_heater[index]) + ratio[index])/2;
						
						curr_permit_heattime = (HeaterInfo.TargetTemp[index] + Env_Offset + TEMP_RANGE - min_heater[index]) * ratio[index];
						if(curr_permit_heattime < 2000)
							curr_permit_heattime = 2000;
						if(OSTimeGet() - start_heat_time[index] > curr_permit_heattime)
						{
							Heater_PIO_Clear(index);
							is_power_on_heater[index] = FALSE;
							max_heater[index] = 0;
							min_heater[index] = Temp[index];
							heating_time[index] = OSTimeGet() - start_heat_time[index];
							start_cool_time[index] = OSTimeGet();
							start_cool_temp[index] = Temp[index];
						}
					}
				}
				else
				{
					
					if(heating_time[index] != 0)
					{
						if(OSTimeGet() - start_cool_time[index] >= 2000)
						{
							if( max_heater[index] > HeaterInfo.TargetTemp[index] + Env_Offset - TEMP_RANGE + 1.0)
							{
								if( Temp[index] > HeaterInfo.TargetTemp[index] + Env_Offset - TEMP_RANGE)
								{
									Heater_PIO_Clear(index);
									is_power_on_heater[index] = FALSE;
								}
								else
								{
									Heater_PIO_Set(index);	
									start_heat_time[index] = OSTimeGet();
									is_power_on_heater[index] = TRUE;
									min_heater[index] = Temp[index] -1.0;
								}
							}
							else
							{
								if( Temp[index] > max_heater[index] - 1.0)
								{
									Heater_PIO_Clear(index);
									is_power_on_heater[index] = FALSE;
								}
								else
								{
									Heater_PIO_Set(index);			
									start_heat_time[index] = OSTimeGet();
									is_power_on_heater[index] = TRUE;
									min_heater[index] = Temp[index] -1.0;
								}
							}
						}
					}
					else
					{
						float temp_target;
						if( first_temp[index] > HeaterInfo.TargetTemp[index])
							temp_target = HeaterInfo.TargetTemp[index];
						else
							temp_target = (HeaterInfo.TargetTemp[index] + first_temp[index])/2 ;
						if( Temp[index] > temp_target + Env_Offset - TEMP_RANGE)
						{
							Heater_PIO_Clear(index);
							is_power_on_heater[index] = FALSE;
						}
						else
						{
							Heater_PIO_Set(index);		
							start_heat_time[index] = OSTimeGet();
							is_power_on_heater[index] = TRUE;
							min_heater[index] = Temp[index] -1.0;
						}
					}
				}
				if(is_power_on_heater[index] == FALSE)
				{
					if(max_heater[index] <= Temp[index] )
					{
						max_heater[index] = Temp[index];
					}
				}
			}
			else
			{
				Heater_PIO_Clear(index);
				is_power_on_heater[index] = FALSE;
				max_heater[index] = 0;
				min_heater[index] = PT100_TABLE_LEN;
				start_heat_time[index] = 0;
				heating_time[index] = 0;
				first_temp[index] = -1;
				cooling_time[index] = 0;
				start_cool_time[index]= 0;
				start_cool_temp[index] = 0;
			}
			index++;
		}
		index = 0;
#endif    
#ifdef EPSON_BOTTOM_BOARD_V2_1        
		bellFlags = OSFlagPend(mix_FLAG_GRP, MISC_FLAGS_BELLMENU, OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME, 200, &err);
#else
		bellFlags = OSFlagPend(mix_FLAG_GRP, MISC_FLAGS_BELLMENU, OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME, 100, &err);
#endif
		
		bellFlags |= OSFlagAccept(mix_FLAG_GRP, MISC_FLAGS_BELLLVDS, OS_FLAG_WAIT_SET_ANY,&err);		
#if defined(HEAD_RICOH_G4)&&defined(MB_LEVEL_SENSOR)
		CHECK_INK_LEVEL();		
#endif
		if(bellFlags & MISC_FLAGS_BELLMENU)
		{
			beepTick = 5;
		}
		else if(bellFlags&MISC_FLAGS_BELLLVDS)	
		{
			if(beepTick == -2)
				beepTick = 1;
		}
		
		if(beepTick > 0)
		{
			beepTick --;
			BELL_ON();
		}
		else
		{
			if(beepTick != -2)
				beepTick --;
			if(beepTick < -2)
				beepTick = -2;
			BELL_OFF();
		}
#if !defined(EPSON_BOTTOM_BOARD_V2_1)&& defined(MICOLOR_AUTOFUCTION)        
		if(LCDMenuConfig.PlateFanMode == PLATEFANMODE_OFF)
		{
			ClosePlateFan();
		}
		else
		{
			if((LCDMenuConfig.AutoPlateFan && !IsOutOfMedia()) || !LCDMenuConfig.AutoPlateFan)
			{
				if(LCDMenuConfig.PlateFanMode == PLATEFANMODE_STRONG)
					OpenPlateFan(PLATE_FAN_STRONG);
				else
					OpenPlateFan(PLATE_FAN_WEAK);
			}
			else
				ClosePlateFan();
		}
#endif				
	}
}
#else
#if defined(HEAD_RICOH_G4)&&defined(MB_LEVEL_SENSOR)
void CHECK_INK_LEVEL(void)
{
	if(PIO_Get(&INK_K_LEVEL) == INK_LOW)
		INK_LEVEL_MASK |=INK_K_BITS;
	else
		INK_LEVEL_MASK &=(~(INK_K_BITS&0xFFFFFFFF));
	
	if(PIO_Get(&INK_C_LEVEL) == INK_LOW)
		INK_LEVEL_MASK |=INK_C_BITS;
	else
		INK_LEVEL_MASK &=(~(INK_C_BITS&0xFFFFFFFF));
	
	if(PIO_Get(&INK_M_LEVEL) == INK_LOW)
		INK_LEVEL_MASK |=INK_M_BITS;
	else
		INK_LEVEL_MASK &=(~(INK_M_BITS&0xFFFFFFFF));
	
	if(PIO_Get(&INK_Y_LEVEL) == INK_LOW)
		INK_LEVEL_MASK |=INK_Y_BITS;
	else
		INK_LEVEL_MASK &=(~(INK_Y_BITS&0xFFFFFFFF));
}
#endif

void Control_Task (void *data)
{
	INT8U err;
	OS_FLAGS bellFlags = 0;
#if (defined(TIMER_ON)&&defined(PUMP_INTERMITTENT))
	OS_FLAGS timerFlags = 0;
	OS_FLAGS timer_pendFlags = 0;
	int i = 0;
	INT16U pump_OnTime = PUMP_PERIOD * PUMP_TIME_RATIO;
	INT16U pump_OffTime = PUMP_PERIOD - PUMP_PERIOD * PUMP_TIME_RATIO;

	/*bell_status,pump_status都有三个状态
	* 0：初始状态，当有信号到来时变为1
	* 1：开启(ON)状态，开启一段时间（定时器）后关闭，变为关闭状态2，也可根据需要直接变为初始状态0
	* 2：关闭（OFF)状态,关闭后等待一段时间（定时器）后变为初始状态0，
	*/
	int bell_status = 0;	//0:初始状态；1：bell on; 2:bell off;  0--->1--->2--->0
	int pump_status = 0;	//0:初始状态；1：pump on; 2:pump off;  0--->1--->2--->0
	
	timer_init(&control_timer);		//初始化定时器
#endif
	if (PIO_Get(&LVDSLockPin) == 1)
	{
		Control_SetAlarm(ALARM_TYPE_LVDSLOST, ALARM_CLR);
		//			CONSOL_Printf("LVDS is Relocked!.r\n");
	}	
	else
	{
		//remove for slient
		Control_SetAlarm(ALARM_TYPE_LVDSLOST, ALARM_SET);
		CONSOL_Printf("LVDS is Unlocked!\r\n");	
	}
	
#if (defined(TIMER_ON)&&defined(PUMP_INTERMITTENT))
	while(1)
	{
//#ifdef FULGENCY_FUN
		pump_OnTime = LCDMenuConfig_EX.Interval_Pump_On_Time;
		pump_OffTime = LCDMenuConfig_EX.Interval_Pump_Off_Time;
//#endif
		//BELL
		if( bell_status == 0)			
		{
			bellFlags = OSFlagAccept(mix_FLAG_GRP, MISC_FLAGS_BELL|MISC_FLAGS_BELLLVDS|MISC_FLAGS_BELLMENU, OS_FLAG_WAIT_SET_ANY, &err);
			if(bellFlags != 0)			//如果有bell信号，开启bell
			{
				BELL_ON();
				timer_pendFlags |= TIMER_BELL;
				bell_status = 1;
			}
			
			//设置BELL定时器，等待一段时间关闭BELL：定时关闭
			if (bellFlags & MISC_FLAGS_BELL)
				timer_set_cycle(&control_timer, TIMER_BELL, 300);		
			else if (bellFlags & MISC_FLAGS_BELLLVDS)
				timer_set_cycle(&control_timer, TIMER_BELL, 100);
			else if(bellFlags & MISC_FLAGS_BELLMENU)
				timer_set_cycle(&control_timer, TIMER_BELL, 500);
		}
		//PUMP
		if(pump_status == 0)
		{
			if((INT8U)INK_LEVEL_MASK != (INT8U)0)
			{
				PUMP_OPEN(INK_LEVEL_MASK);	//泵墨
				timer_set_cycle(&control_timer, TIMER_PUMP, pump_OnTime);		//设置关闭泵墨时间
				timer_pendFlags |= TIMER_PUMP;	//设置泵墨定时器
				pump_status = 1;
			}
		}
		
		if (timer_pendFlags == 0)
		{
			OSTimeDly(100);	
			continue;
		}
		timer_start(&control_timer, timer_pendFlags);		//开启定时器（PUMP TIMER/BELL TIMER)
		
		timerFlags = OSFlagPend(control_timer.timer_FLAG_GRP, timer_pendFlags, OS_FLAG_WAIT_SET_ANY | OS_FLAG_CONSUME, 0, &err);
		{
			if (timerFlags & TIMER_BELL)
			{
				if(bell_status == 1)	
				{
					BELL_OFF();
					bell_status = 2;
					
					if (bellFlags & MISC_FLAGS_BELL)
						timer_set_cycle(&control_timer, TIMER_BELL, 500);
					else if (bellFlags & MISC_FLAGS_BELLLVDS)
						timer_set_cycle(&control_timer, TIMER_BELL, 200);
					else if(bellFlags & MISC_FLAGS_BELLMENU)
					{
						timer_stop(&control_timer, TIMER_BELL);	//关闭BELL定时器
						OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_BELLMENU, OS_FLAG_CLR, &err);
						bell_status = 0;
						timer_pendFlags &= ~TIMER_BELL;
					}
				}
				else if(bell_status == 2)
				{
					timer_stop(&control_timer, TIMER_BELL);	//关闭定时器
					bell_status = 0;
					timer_pendFlags &= ~TIMER_BELL;
				}
			}
			if(timerFlags & TIMER_PUMP)
			{
				if(pump_status == 1)
				{
					pump_status = 2;
					PUMP_OPEN(0);				//关闭泵墨
					timer_set_cycle(&control_timer, TIMER_PUMP, pump_OffTime);
				}
				else if(pump_status == 2)
				{
					timer_stop(&control_timer, TIMER_PUMP);	//关闭泵墨定时器
					pump_status = 0;
					timer_pendFlags &= ~TIMER_PUMP;			//清除泵墨定时
				}
			}
		}		//end OSFlagPend
	}//end while
#else	// else no (defined(TIMER_ON)&&defined(PUMP_INTERMITTENT))
	while (1)
	{
		bellFlags = OSFlagPend(mix_FLAG_GRP, MISC_FLAGS_BELL|MISC_FLAGS_BELLLVDS|MISC_FLAGS_BELLMENU, OS_FLAG_WAIT_SET_ANY, 0, &err);
		if (bellFlags & MISC_FLAGS_BELL)
		{
			BELL_ON();
			OSTimeDly(300);
			BELL_OFF();
			OSTimeDly(500);
		}else if (bellFlags & MISC_FLAGS_BELLLVDS)
		{
			BELL_ON();
			OSTimeDly(100);
			BELL_OFF();
			OSTimeDly(200);			
		}
		else if(bellFlags & MISC_FLAGS_BELLMENU)
		{
			BELL_ON();
			OSTimeDly(500);
			BELL_OFF();
			//OSTimeDly(500);			
			OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_BELLMENU, OS_FLAG_CLR, &err);
		}	
#if defined(HEAD_RICOH_G4)&&defined(MB_LEVEL_SENSOR)
		CHECK_INK_LEVEL();		
#endif
	}
#endif
}
#endif

void Control_SetAlarm(INT8U alarmtype, INT8U opt)
{
	INT8U err;
	if (alarmtype == ALARM_TYPE_PUMPLONG)
		OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_BELL, opt, &err);
#ifndef EPSON_BOTTOM_BOARD_V3		
	else if(alarmtype == ALARM_TYPE_LVDSLOST)
	{
		OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_BELLLVDS, opt, &err);
		if(opt == 1)
			status_ReportStatus(STATUS_WAR_EPSON_HEAD_LVDS_ERR, STATUS_SET);   
		else
			status_ReportStatus(STATUS_WAR_EPSON_HEAD_LVDS_ERR, STATUS_CLR);   
	}
#endif		
	else
		OSFlagPost (mix_FLAG_GRP, MISC_FLAGS_BELLMENU, opt, &err);
}
void PUMP_OPEN(INT8U mask)
{
	INT8U i = 0;
	for(i = 0;i < COLOR_NUMBER_CALIBRATION;i++)
	{
		if((mask & (1<<i))!= 0)
		{
			if(i == 0)
				PIO_Set(&InkSupplyCtrl1Pin);
			else if(i == 1)
				PIO_Set(&InkSupplyCtrl2Pin);
			else if(i == 2)
				PIO_Set(&InkSupplyCtrl3Pin);
			else if(i == 3)
				PIO_Set(&InkSupplyCtrl4Pin);
			else if(i == 4)
				PIO_Set(&InkSupplyCtrl5Pin);
			else if(i == 5)
				PIO_Set(&InkSupplyCtrl6Pin);
			else if(i == 6)
				PIO_Set(&InkSupplyCtrl7Pin);
			else if(i == 7)
				PIO_Set(&InkSupplyCtrl8Pin);
		}
		else
		{
			if(i == 0)
				PIO_Clear(&InkSupplyCtrl1Pin);
			else if(i == 1)
				PIO_Clear(&InkSupplyCtrl2Pin);
			else if(i == 2)
				PIO_Clear(&InkSupplyCtrl3Pin);
			else if(i == 3)
				PIO_Clear(&InkSupplyCtrl4Pin);
			else if(i == 4)
				PIO_Clear(&InkSupplyCtrl5Pin);
			else if(i == 5)
				PIO_Clear(&InkSupplyCtrl6Pin);
			else if(i == 6)
				PIO_Clear(&InkSupplyCtrl7Pin);
			else if(i == 7)
				PIO_Clear(&InkSupplyCtrl8Pin);
		}
	}
}
void Control_PumpCloseAll(void)
{
#if (defined(HEAD_RICOH_G4)||defined(EPSON_4H))&&(defined(MB_LEVEL_SENSOR)||defined(HEAD_LEVEL_SENSOR))
	PUMP_OPEN(0);
#else
	PUMP_LC_CLOSE();
	PUMP_LM_CLOSE();
	PUMP_K_CLOSE();
	PUMP_C_CLOSE();
	PUMP_M_CLOSE();
	PUMP_Y_CLOSE();
	PUMP_7_CLOSE();
	PUMP_8_CLOSE();
#endif	
	Control_SetAlarm(ALARM_TYPE_PUMPLONG, ALARM_CLR);
}
#if (defined(HEAD_RICOH_G4)||defined(EPSON_4H))&&(defined(MB_LEVEL_SENSOR)||defined(HEAD_LEVEL_SENSOR))
void CLEAN_PumpCloseMask(INT8U mask)
{
	PUMP_OPEN(0);
}


void CLEAN_PumpOpenMask(INT8U mask)
{
	PUMP_OPEN(mask);
}

#endif
#if defined (EPSON_CLEAN_UPDOWN)

#if !(defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT))
void CONTROL_CLOSE_RELEASE_RELAY(INT8U HeadIndex)
{
#ifndef A1802
	if(HeadIndex == 0)
		PIO_Clear(&CleanRelease1Pin);
	else if(HeadIndex == 1)
#ifdef EPSON_BOTTOM_BOARD_V2
		PIO_Clear(&InkSupplyCtrl7Pin);
#else
	PIO_Clear(&CleanRelease2Pin);
#endif
#endif
}

void CONTROL_OPEN_RELEASE_RELAY(INT8U HeadIndex)
{
#ifndef A1802
	if(HeadIndex == 0)
	{
		PIO_Set(&CleanRelease1Pin);
	}
	else if(HeadIndex == 1)
#ifdef EPSON_BOTTOM_BOARD_V2
		PIO_Set(&InkSupplyCtrl7Pin);
#else
	PIO_Set(&CleanRelease2Pin);
#endif
#endif		
}
#endif

void CONTROL_CLOSE_SUCKER(INT8U HeadIndex)
{
	if(HeadIndex == 0)
		PIO_Clear(&CleanSuck1Pin);
	else if(HeadIndex == 1)
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT)
		PIO_Clear(&CleanSuck2Pin);
	else if(HeadIndex == 2)
		PIO_Clear(&CleanSuck3Pin);
	else if(HeadIndex == 3)
		PIO_Clear(&CleanSuck4Pin);
#else      
#ifdef EPSON_BOTTOM_BOARD_V2			
	PIO_Clear(&InkSupplyCtrl8Pin);
#else
	PIO_Clear(&CleanSuck2Pin);
#endif
#endif
}

void CONTROL_OPEN_SUCKER(INT8U HeadIndex)
{
	if(HeadIndex == 0)
	{
		PIO_Set(&CleanSuck1Pin);
	}
	else if(HeadIndex == 1)
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT)
		PIO_Set(&CleanSuck2Pin);
	else if(HeadIndex == 2)
		PIO_Set(&CleanSuck3Pin);
	else if(HeadIndex == 3)
		PIO_Set(&CleanSuck4Pin);
#else
#ifdef EPSON_BOTTOM_BOARD_V2			
	PIO_Set(&InkSupplyCtrl8Pin);
#else
	PIO_Set(&CleanSuck2Pin);
#endif
#endif
}


#endif
#if ((defined (EPSON_CLEAN_INTEGRATE_1) || defined(EPSON_CLEAN_INTEGRATE_2)) && !defined(COLORFUL_EPSON))
void CONTROL_CLOSE_RELEASE_RELAY(INT8U HeadIndex)
{
	if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 1)
		PIO_Clear(&CleanRelease1Pin);
	else if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 2)
		PIO_Clear(&CleanRelease2Pin);
	else
	{
		PIO_Clear(&CleanRelease1Pin);
		PIO_Clear(&CleanRelease2Pin);
	}
}

void CONTROL_OPEN_RELEASE_RELAY(INT8U HeadIndex)
{
	
	if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 1)
	{
		PIO_Set(&CleanRelease1Pin);
	}
	else if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 2)
		PIO_Set(&CleanRelease2Pin);
	else
	{
		PIO_Set(&CleanRelease1Pin);
		PIO_Set(&CleanRelease2Pin);
	}
}

void CONTROL_CLOSE_SUCKER(INT8U HeadIndex)
{
	if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 1)
		PIO_Clear(&CleanSuck1Pin);
	else if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 2)
		PIO_Clear(&CleanSuck2Pin);
	else
	{
		PIO_Clear(&CleanSuck1Pin);
		PIO_Clear(&CleanSuck2Pin);
	}
}

void CONTROL_OPEN_SUCKER(INT8U HeadIndex)
{
	if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 1)
	{
		PIO_Set(&CleanSuck1Pin);
	}
	else if(cleanparam_EPSON_MICOLOR.Runtime.HeadMask == 2)
		PIO_Set(&CleanSuck2Pin);
	else
	{
		PIO_Set(&CleanSuck1Pin);
		PIO_Set(&CleanSuck2Pin);
	}
}
#endif
#if defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_UPDOWN_WIPER_EX)
void wiper_down(INT8U num)//极性反转
{
	if(num == 1)
		PIO_Clear(&CleanWipe1Pin);
	else if(num == 2)
		PIO_Clear(&CleanWipe2Pin);	
	else
	{
		PIO_Clear(&CleanWipe1Pin);
		PIO_Clear(&CleanWipe2Pin);	
	}
}

void wiper_up(INT8U num)//极性反转
{	
	if(num == 1)
		PIO_Set(&CleanWipe1Pin);
	else if(num == 2)
		PIO_Set(&CleanWipe2Pin);
	else
	{
		PIO_Set(&CleanWipe1Pin);
		PIO_Set(&CleanWipe2Pin);
	}
}
#endif

#if defined(EPSON_CLEAN_UPDOWN_FULGENCY)
void wiper_down()//极性反转
{
	PIO_Clear(&InkSupplyCtrl6Pin);
}

void wiper_up(INT8U num)//极性反转
{	
	PIO_Set(&InkSupplyCtrl6Pin);
}
#endif

#if defined (MICOLOR_AUTOFUCTION)

INT8U IsLoadMediaSensorValid()
{
#ifdef EPSON_BOTTOM_BOARD_V3
	OS_CPU_SR cpu_sr;
	INT16U io;
	
	OS_ENTER_CRITICAL();	
	io = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_IO);
	OS_EXIT_CRITICAL();
	if(!( io & FPGA_EXPAND_IOMASK_EPSON_LOAD_MEDIA))
		return False;
	else
		return True;
#else
	if(PIO_Get(&LoadMediaSensorPin))
		return False;
	else
		return True;
#endif
}

INT8U IsCoverSensorValid()
{
#ifdef EPSON_BOTTOM_BOARD_V3
	OS_CPU_SR cpu_sr;
	INT16U io;
	
	OS_ENTER_CRITICAL();	
	io = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_IO);
	OS_EXIT_CRITICAL();
	if((io & FPGA_EXPAND_IOMASK_EPSON_COVER))
		return True;
	else
		return False;
#else
	if(!PIO_Get(&CoverSensorPin))
		return True;
	else
		return False;
#endif
}

INT8U IsMediaFixed()
{
#ifdef EPSON_BOTTOM_BOARD_V3
	OS_CPU_SR cpu_sr;
	INT16U io;
	
	OS_ENTER_CRITICAL();	
	io = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_IO);
	OS_EXIT_CRITICAL();
	if( !(io & FPGA_EXPAND_IOMASK_EPSON_MEDIA_FIXED))
		return True;
	else
		return False;
#else
	if(PIO_Get(&MediaFixedSensorPin))
		return True;
	else
		return False;
#endif
}

INT8U IsOutOfMedia()
{
#ifdef EPSON_BOTTOM_BOARD_V3
	OS_CPU_SR cpu_sr;
	INT16U io;
	
	OS_ENTER_CRITICAL();	
	io = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_IO);
	OS_EXIT_CRITICAL();
	if(!( io & FPGA_EXPAND_IOMASK_EPSON_LACK_OF_MEDIA))
		return True;
	else
		return False;
#else
	if(PIO_Get(&LackMediaSensorPin))
		return True;
	else
		return False;
#endif
}
#ifdef EPSON_BOTTOM_BOARD_V3
INT8U OpenPlateFanAlways()
{
	PIO_Set(&PlateFan4Pin);
	return True;
}
#endif
INT8U OpenPlateFan(INT8U mode)
{
	PIO_Set(&PlateFan1Pin);
	if(mode == PLATE_FAN_STRONG) 
		PIO_Set(&PlateFan2Pin);
	else
		PIO_Clear(&PlateFan2Pin);
#ifdef EPSON_BOTTOM_BOARD_V3
	PIO_Set(&PlateFan3Pin);
#endif
	return True;
}

INT8U ClosePlateFan()
{
	PIO_Clear(&PlateFan1Pin);
	PIO_Clear(&PlateFan2Pin);
#ifdef EPSON_BOTTOM_BOARD_V3
	PIO_Clear(&PlateFan3Pin);
#endif
	return True;
}
#endif

#if defined(USE_SPOT_COLOR)&&defined(HEAD_EPSON_GEN5)
wrong: 
#endif
	
	
	//Pump map
	//墨量传感器：从LVDS接头处开始算起，第一个是0x80, 第二个是0x40...第八个是0x01: 
	//Lc(chip1) Lm(chip1) Y(chip1) M(chip2) C(chip2) K(chip1) SPOT1(chip2) SPOT2(chip2)
	//墨泵端：	  从远离24V最远端开始为Pump1（标号为Lm），
	//墨泵端:	  pump2（标号为Lc），...最接近24V的一端为Pump6（标号为C），
	//墨泵端:     Lm(1), Lc(2), K(3), Y(4), M(5), C(6), SPOTCOLOR1(7), SPOTCOLOR2(8)
	
	//cheney : need confirm it.
#define PUMP_CHIP1_BITS 0xE4  //1110 0100   4 Color: 
#ifdef USE_SPOT_COLOR
#define PUMP_CHIP2_BITS 0x1B  //0001 1011   4 Color
#else
#define PUMP_CHIP2_BITS 0x18  //0001 1000   2 Color
#endif
#if (defined(HEAD_RICOH_G4)||defined(EPSON_4H))&&defined(HEAD_LEVEL_SENSOR)										
void Color_level_map(INT8U level,INT8U index)
{
	if(level !=0 )
		level = INK_LOW;
	if(index < COLOR_NUMBER_CALIBRATION)
	{
		switch(factoryDataEx.m_nColorOrder[index])
		{
		case COLORCODE_K:
			if(level == INK_LOW)
				INK_LEVEL_MASK |=INK_K_BITS;
			else
				INK_LEVEL_MASK &=(~(INK_K_BITS&0xFFFFFFFF));
			break;
		case COLORCODE_C:
			if(level == INK_LOW)
				INK_LEVEL_MASK |=INK_C_BITS;
			else
				INK_LEVEL_MASK &=(~(INK_C_BITS&0xFFFFFFFF));	
			break;
		case COLORCODE_M:
			if(level == INK_LOW)
				INK_LEVEL_MASK |=INK_M_BITS;
			else
				INK_LEVEL_MASK &=(~(INK_M_BITS&0xFFFFFFFF));	
			break;
		case COLORCODE_Y:
			if(level == INK_LOW)
				INK_LEVEL_MASK |=INK_Y_BITS;
			else
				INK_LEVEL_MASK &=(~(INK_Y_BITS&0xFFFFFFFF));	
			break;
		case COLORCODE_SPOTCOLOR0:
			if(level == INK_LOW)
				INK_LEVEL_MASK |=INK_S1_BITS;
			else
				INK_LEVEL_MASK &=(~(INK_S1_BITS&0xFFFFFFFF));	
			break;
		case COLORCODE_SPOTCOLOR1:
			if(level == INK_LOW)
				INK_LEVEL_MASK |=INK_S2_BITS;
			else
				INK_LEVEL_MASK &=(~(INK_S2_BITS&0xFFFFFFFF));	
			break;
		case COLORCODE_SPOTCOLOR2:
			if(level == INK_LOW)
				INK_LEVEL_MASK |=INK_S3_BITS;
			else
				INK_LEVEL_MASK &=(~(INK_S3_BITS&0xFFFFFFFF));	
			break;
		case COLORCODE_SPOTCOLOR3:
			if(level == INK_LOW)
				INK_LEVEL_MASK |=INK_S4_BITS;
			else
				INK_LEVEL_MASK &=(~(INK_S4_BITS&0xFFFFFFFF));	
			break;
		}
		
	}
}
void Ink_level_map(INT8U head_level)
{
	INT8U i = 0;
	for(i = 0;i< 8;i++)
	{
		Color_level_map(head_level&(1<<i),i);
	}
}

#ifdef FULGENCY_FUN
void Main_Color_Level_Scan(void)
{
	if(INK_STATUS_K_LACK)
		status_ReportStatus(STATUS_WAR_K_LACK, STATUS_SET);
	else
		status_ReportStatus(STATUS_WAR_K_LACK, STATUS_CLR);
	
	if(INK_STATUS_C_LACK)
		status_ReportStatus(STATUS_WAR_C_LACK, STATUS_SET);
	else
		status_ReportStatus(STATUS_WAR_C_LACK, STATUS_CLR);
	
	if(INK_STATUS_M_LACK)
		status_ReportStatus(STATUS_WAR_M_LACK, STATUS_SET);
	else
		status_ReportStatus(STATUS_WAR_M_LACK, STATUS_CLR);
	
	if(INK_STATUS_Y_LACK)
		status_ReportStatus(STATUS_WAR_Y_LACK, STATUS_SET);
	else
		status_ReportStatus(STATUS_WAR_Y_LACK, STATUS_CLR);
}
#endif

#elif defined(HEAD_LEVEL_SENSOR_NO_PUMP)
void Color_level_WARING(INT8U level,INT8U index)
{
	if(level !=0 )
		level = INK_LOW;
	if(index < COLOR_NUMBER_CALIBRATION)
	{
		switch(factoryDataEx.m_nColorOrder[index])
		{
		case COLORCODE_K:
			if(level == INK_LOW)
				status_ReportStatus(STATUS_WAR_K_LACK, STATUS_SET);
			else
				status_ReportStatus(STATUS_WAR_K_LACK, STATUS_CLR);
			break;
		case COLORCODE_C:
			if(level == INK_LOW)
				status_ReportStatus(STATUS_WAR_C_LACK, STATUS_SET);
			else
				status_ReportStatus(STATUS_WAR_C_LACK, STATUS_CLR);
			break;
		case COLORCODE_M:
			if(level == INK_LOW)
				status_ReportStatus(STATUS_WAR_M_LACK, STATUS_SET);
			else
				status_ReportStatus(STATUS_WAR_M_LACK, STATUS_CLR);
			break;
		case COLORCODE_Y:
			if(level == INK_LOW)
				status_ReportStatus(STATUS_WAR_Y_LACK, STATUS_SET);
			else
				status_ReportStatus(STATUS_WAR_Y_LACK, STATUS_CLR);
			break;
		case COLORCODE_SPOTCOLOR0:
			if(level == INK_LOW)
				status_ReportStatus(STATUS_WAR_W_LACK, STATUS_SET);
			else
				status_ReportStatus(STATUS_WAR_W_LACK, STATUS_CLR);
			break;
		case COLORCODE_SPOTCOLOR1:
			if(level == INK_LOW)
				status_ReportStatus(STATUS_WAR_V_LACK, STATUS_SET);
			else
				status_ReportStatus(STATUS_WAR_V_LACK, STATUS_CLR);
			break;
		case COLORCODE_SPOTCOLOR2:
			if(level == INK_LOW)
				status_ReportStatus(STATUS_WAR_PUMP_LIGHTCYAN, STATUS_SET);
			else
				status_ReportStatus(STATUS_WAR_PUMP_LIGHTCYAN, STATUS_CLR);
			break;
		case COLORCODE_SPOTCOLOR3:
			if(level == INK_LOW)
				status_ReportStatus(STATUS_WAR_PUMP_LIGHTMAGENTA, STATUS_SET);
			else
				status_ReportStatus(STATUS_WAR_PUMP_LIGHTMAGENTA, STATUS_CLR);
			break;
		}
		
	}
}
void Ink_level_map(INT8U head_level)
{
	INT8U i = 0;
	for(i = 0;i< 8;i++)
	{
		Color_level_WARING(head_level&(1<<i),i);
	}
}
#endif


void Control_PumpControl(INT8U ink_bits)
{
	static INT32U opentime[8] = {0,0,0,0,0,0,0,0};
	g_PumpControl = 0;	
	INT8U err;
	
	if ((opentime[0] >= 10) || (opentime[1] >= 10) || (opentime[2] >= 10) || (opentime[3] >= 10)
		||(opentime[4] >= 10) || (opentime[5] >= 10) || (opentime[6] >= 10) || (opentime[7] >= 10))	
		Control_SetAlarm(ALARM_TYPE_PUMPLONG, ALARM_SET);
	else
		Control_SetAlarm(ALARM_TYPE_PUMPLONG, ALARM_CLR);
#if !((defined(HEAD_RICOH_G4)||defined(EPSON_4H))&&(defined(MB_LEVEL_SENSOR)||defined(HEAD_LEVEL_SENSOR)))
	if (ink_bits & 0x80)	//LC ->Chip1
	{		
		opentime[0]++;
		if (opentime[0] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_LIGHTCYAN, STATUS_SET);
			PUMP_LC_OPEN();
		}
	}	
	else
	{
		PUMP_LC_CLOSE();
		opentime[0] = 0;
	}
	
	if (ink_bits & 0x40)	//LM ->Chip1
	{			
		opentime[1]++;
		if (opentime[1] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_LIGHTMAGENTA, STATUS_SET);
			PUMP_LM_OPEN();			
		}
	}
	else
	{
		opentime[1] = 0;
		PUMP_LM_CLOSE();
	}
	
	if (ink_bits & 0x20)  //Y ->Chip1
	{				
		opentime[2]++;
		if (opentime[2] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_YELLOW, STATUS_SET);
			PUMP_Y_OPEN();				
		}
	}	
	else
	{
		opentime[2] = 0;
		PUMP_Y_CLOSE();
	}
	
	if (ink_bits & 0x10)	//M ->Chip2
	{					
		opentime[3]++;
		if (opentime[3] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_MAGENTA, STATUS_SET);
			PUMP_M_OPEN();	
		}
	}
	else
	{
		opentime[3] = 0;
		PUMP_M_CLOSE();
	}
	
	if (ink_bits & 0x08)	//C ->Chip2
	{
		opentime[4]++;
		if (opentime[4] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_CYAN, STATUS_SET);
			PUMP_C_OPEN();
		}						
	}
	else
	{
		opentime[4] = 0;
		PUMP_C_CLOSE();
	}
	
	if (ink_bits & 0x04)	//K ->Chip1
	{
		opentime[5]++;
		if (opentime[5] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_BLACK, STATUS_SET);
			PUMP_K_OPEN();
		}							
	}
	else
	{
		opentime[5] = 0;
		PUMP_K_CLOSE();
	}
	
#ifdef USE_SPOT_COLOR
	if (ink_bits & 0x02)	//K ->Chip1
	{
		opentime[6]++;
		if (opentime[6] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_SPOTCOLOR1, STATUS_SET);
			PUMP_7_OPEN();
		}							
	}
	else
	{
		opentime[6] = 0;
		PUMP_7_CLOSE();
	}
	
	if (ink_bits & 0x01)	//K ->Chip1
	{
		opentime[7]++;
		if (opentime[7] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_SPOTCOLOR2, STATUS_SET);
			PUMP_8_OPEN();
		}							
	}
	else
	{
		opentime[7] = 0;
		PUMP_8_CLOSE();
	}
#endif	
#else
	//OSFlagPost(mix_FLAG_GRP, PUMP_CONTROL, OS_FLAG_SET, &err);
#ifndef ORIC_FUNCTION
#if !(defined(TIMER_ON)&&defined(PUMP_INTERMITTENT))
	PUMP_OPEN(ink_bits);
#endif
#endif	//endi if0
	if (ink_bits & INK_K_BITS)	//k
	{
		opentime[0]++;
		if (opentime[0] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_BLACK, STATUS_SET);
		}							
	}
	else
	{
		opentime[0] = 0;
		status_ReportStatus(STATUS_WAR_PUMP_BLACK, STATUS_CLR);
	}
	
	if (ink_bits & INK_C_BITS)	//c
	{			
		opentime[1]++;
		if (opentime[1] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_CYAN, STATUS_SET);			
		}
	}
	else
	{
		opentime[1] = 0;
		status_ReportStatus(STATUS_WAR_PUMP_CYAN, STATUS_CLR);
	}
	
	if (ink_bits & INK_M_BITS)  //M
	{				
		opentime[2]++;
		if (opentime[2] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_MAGENTA, STATUS_SET);			
		}
	}	
	else
	{
		opentime[2] = 0;
		status_ReportStatus(STATUS_WAR_PUMP_MAGENTA, STATUS_CLR);
	}
	
	if (ink_bits & INK_Y_BITS)	//Y
	{					
		opentime[3]++;
		if (opentime[3] >= PUMP_DELAY_TIME)
		{
			status_ReportStatus(STATUS_WAR_PUMP_YELLOW, STATUS_SET);
		}
	}
	else
	{
		opentime[3] = 0;
		status_ReportStatus(STATUS_WAR_PUMP_YELLOW, STATUS_CLR);
	}
	
	if(COLOR_NUMBER_CALIBRATION > 4)		
	{ 
		if (ink_bits & INK_S1_BITS)	//lc
		{
			opentime[4]++;
			if (opentime[4] >= PUMP_DELAY_TIME)
			{
				status_ReportStatus(STATUS_WAR_PUMP_LIGHTCYAN, STATUS_SET);
			}						
		}
		else
		{
			opentime[4] = 0;
			status_ReportStatus(STATUS_WAR_PUMP_LIGHTCYAN, STATUS_CLR);
		}
		
		if (ink_bits & INK_S2_BITS)	//lm
		{
			opentime[5]++;
			if (opentime[5] >= PUMP_DELAY_TIME)
			{
				status_ReportStatus(STATUS_WAR_PUMP_LIGHTMAGENTA, STATUS_SET);
			}							
		}
		else
		{
			opentime[5] = 0;
			status_ReportStatus(STATUS_WAR_PUMP_LIGHTMAGENTA, STATUS_CLR);
		}
	}
	
	if(COLOR_NUMBER_CALIBRATION > 6)		
	{
		if (ink_bits & INK_S3_BITS)	//S3
		{
			opentime[6]++;
			if (opentime[6] >= PUMP_DELAY_TIME)
			{
				status_ReportStatus(STATUS_WAR_PUMP_SPOTCOLOR1, STATUS_SET);
			}							
		}
		else
		{
			opentime[6] = 0;
			status_ReportStatus(STATUS_WAR_PUMP_SPOTCOLOR1, STATUS_CLR);
		}
		
		if (ink_bits & INK_S4_BITS)	//S4
		{
			opentime[7]++;
			if (opentime[7] >= PUMP_DELAY_TIME)
			{
				status_ReportStatus(STATUS_WAR_PUMP_SPOTCOLOR2, STATUS_SET);
			}							
		}
		else
		{
			opentime[7] = 0;
			status_ReportStatus(STATUS_WAR_PUMP_SPOTCOLOR2, STATUS_CLR);
		}
	}	

#endif
}
#ifdef MANUFACTURER_DYSS
INT8U OpenPlateFan(void)
{

		PIO_Set(&InkSupplyCtrl8Pin);

}

INT8U ClosePlateFan(void)
{

	 PIO_Clear(&InkSupplyCtrl8Pin);

}
#endif

#ifdef EPSON_BOTTOM_BOARD_V2
void Needle_Open(void)
{
	PIO_Set(&InkSupplyCtrl10Pin);
}

void Needle_Close(void)
{
	PIO_Clear(&InkSupplyCtrl10Pin);
}
#endif
#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)

	void Open_Induced_Draft(void)
	{
		PIO_Set(&InkSupplyCtrl6Pin);
	}
	
	void Close_Induced_Draft(void)
	{
		PIO_Clear(&InkSupplyCtrl6Pin);
	}
	
	void Open_Paper_Knife(void)
	{
		PIO_Set(&InkSupplyCtrl5Pin);
	}
	
	void Close_Paper_Knife(void)
	{
			PIO_Clear(&InkSupplyCtrl5Pin);
	}
	
	void Cut_Paper (void)
	{
			INT8U buf[32];
			INT8U err;

			//const INT32S cut_startPos = 1000 + PAPER_MEDIA_WIDTH_INCH*X_BASE_RES;
			const INT32S cut_startPos = 1000 + 20000;
			const INT32S cut_endPos = 100;


	#ifdef EPSON_CLEAN_UPDOWN
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
			buf[0] = 8; //Length
			buf[1] = UART_AXISMOVETO_CMD; 
			buf[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
			buf[3] = TATE_CLEAN_CZ_SPEED_LEVEL;					
			*((__packed INT32S *)&buf[4]) = 0; //move_distance means target position
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);					       
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL,0,&err); //Waiting moving stop	
	#endif
	
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		
			buf[0] = 7; //Length
			buf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&buf[2]) = cut_startPos;
			buf[6] = printer.movSpeed;
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);			
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop	

	#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
         Open_Paper_Knife();                   // open papaer knife
         OSTimeDly(100);
   #endif
	 
			status_ReportStatus(STATUS_MOVING, STATUS_SET);		
			buf[0] = 7; //Length
			buf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&buf[2]) = cut_endPos;
			buf[6] = printer.movSpeed;
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop

	#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
			Close_Paper_Knife();                  // close papaer knife
			OSTimeDly(100);
   #endif
                
			status_ReportStatus(STATUS_MOVING, STATUS_SET);
			buf[0] = 7; //Length
			buf[1] = UART_MOVETO_CMD;
			*((__packed INT32S *)&buf[2]) = 0;
			buf[6] = printer.movSpeed;
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
                
	#ifdef EPSON_CLEAN_UPDOWN
			Capping();
	#endif

			status_ReportStatus(STATUS_MOVING, STATUS_SET);
			buf[0] = 8; //Length									
			buf[1] = UART_PAPERFEED_CMD; 
			buf[2] = 1;
			buf[3] = printer.feedSpeed;
			*((__packed INT32S *)&buf[4]) = GetStepPerHead()*4;
			while (!UART_SendCMD(UART_MOTION_CHANNEL, buf)) 
				OSTimeDly(100);
			OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
			
			CMD_Epson_Operate_Type = 0;
			status_ReportStatus(CMD_EPSON_OPERATION, STATUS_CLR);
	}
#endif
	
	

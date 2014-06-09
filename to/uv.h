#ifndef _UV_H_
#define _UV_H_

#ifdef UV_PRINTER

#include "pio/pio.h"
#include "control.h"

#ifdef _UV_C_
#define EXTERN
#else
#define EXTERN extern
#endif

//No	PIN		I/O	Comment																	CN2-PIN,NetLabel
//1	PIN1		24V-UV灯电源控制箱输出24V	
//2	PIN2	I	UVREADY-信号输出（高电平表示UV灯处于正常工作状态，可以正常打印）。		34, UVrdy_I
//3	PIN3	I	LAMP1 FAULT -信号输出（高电平表示UV灯处于不正常工作状态，出错）			38, LP1F_I
//4	PIN5	O	LAMP1 ON -信号输入（UV灯低功率）										40, LP1ON-O,INVERT
//5	PIN6	O	LAMP2 ON -信号输入（UV灯低功率）										41, LP2ON-O, INVERT
//6	PIN7	O	LAMP1 HIGH POWER -信号输入（UV灯高功率）								43, LP1H-O, INVERT
//7	PIN8	O	LAMP2 HIGH POWER -信号输入（UV灯高功率）								45, LP2H-O, INVERT
//8	PIN9		GND（UV灯电源控制箱输出GND）	
//9	PIN11	O	LAMP1 SHUTTER OPEN/CLOSE--信号输入（高电平打开快门，低电平关闭快门）	47, LP1_SH-O, INVERT
//10	PIN12	I	LAMP2 FAULT -信号输出（高电平表示UV灯处于不正常工作状态，出错）			38, LP2F_I
//11	PIN13	O	LAMP2 SHUTTER OPEN/CLOSE--信号输入（高电平打开快门，低电平关闭快门）	49,LP2_SH-O, INVERT
//12	PIN14		HOST 0V --外部控制板卡的GND	32,LP2F_I
//13	PIN15		24V-（UV灯电源控制箱输出24V）	
//14	PIN16		SYSTEM REMOTE---信号输入（使用外部板卡控制UV灯时，此信号接高电平24V）	
//15	PIN28	I	LAMP1 SHUTTER OPEN-信号输出（指示LAMP1快门的状态）						24, LP1_SH_OP_I
//16	PIN29	I	LAMP1 SHUTTER CLOSE-信号输出（指示LAMP1快门的状态）。					22, LP1_SH_CL_I
//17	PIN30	I	LAMP2 SHUTTER OPEN-信号输出（指示LAMP2快门的状态）						26, LP2_SH_OP_I
//18	PIN31	I	LAMP2 SHUTTER CLOSE-信号输出（指示LAMP2快门的状态）						36, LP2_SH_CL_I


//22, LP1_SH_CL_I			GPB2/TOUT2
//24, LP1_SH_OP_I			GPB4/TCLK0 
//26, LP2_SH_OP_I			GPB3/TOUT3

//32, LP2F_I				GPF6/EINT6
//34, UVrdy_I				GPF4/EINT4
//36, LP2_SH_CL_I			GPF1/EINT1
//38, LP1F_I				GPF5/EINT5

//40, LP1ON-O,INVERT		GPF0/EINT0

//41, LP2ON-O, INVERT		GPD8/VD16
//43, LP1H-O, INVERT		GPD9/VD17
//45, LP2H-O, INVERT		GPD10/VD18
//47, LP1_SH-O, INVERT		GPD11/VD19
//49, LP2_SH-O, INVERT		GPD12/VD20


typedef struct UV_POSI_SET
{
	INT32S Front_Light_OPEN;
	INT32S Front_Light_CLOSE;
	INT32S Back_Light_OPEN;
	INT32S Back_Light_CLOSE;
	INT8U STOP_UV;
}UV_POSI_TYPE;

#ifdef COORD_NEW_UV
extern UV_POSI_TYPE UV_POSI;
#endif

#if defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1)
#define FPGA_EXPAND_IOMASK_UV_READY		0x1
#define FPGA_EXPAND_IOMASK_UV_L1_FAULT	0x2
#define FPGA_EXPAND_IOMASK_UV_L2_FAULT	0x4
#else
//UV Status Pin 
extern const Pin UVStatPinLP1ShCl; 
extern const Pin UVStatPinLP1ShOp; 
extern const Pin UVStatPinLP2ShCl;
extern const Pin UVStatPinLP2ShOp; 
extern const Pin UVStatPinUVRdy;
extern const Pin UVStatPinLP1Fault;
extern const Pin UVStatPinLP2Fault;
#endif

#if defined(OLD_UV)
#if defined(MANUFACTURER_FULGENCY_FLAT_UV)
extern const Pin UVCtrlPinSH1Open;
extern const Pin UVCtrlPinSH2Open;
extern Pin UVCtrlPinLP1HighPower;
extern Pin STATUS_PRT;
extern Pin Y_HOME_READY ;

#else
extern Pin UVCtrlPinSH1Open;
extern Pin UVCtrlPinSH2Open;
#endif
extern Pin UVCtrlPinLP2On;
extern Pin UVCtrlPinLP2HighPower;
extern Pin UVCtrlPinLP1On;
extern Pin UVCtrlPinLP1HighPower;
#else
extern const Pin UVCtrlPinSH1Open;
extern const Pin UVCtrlPinSH2Open;
extern const Pin UVCtrlPinLP2On;
extern const Pin UVCtrlPinLP2HighPower;
extern const Pin UVCtrlPinLP1On;
extern const Pin UVCtrlPinLP1HighPower;
#endif
extern const Pin DSPIrqPin;


#if defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1)
INT8U	UV_STATUS_LP1_FAULT();
INT8U	UV_STATUS_LP2_FAULT();
#define UV_STATUS_SHUTTER1_CLOSE	(!UV_CTR_STATUS_SH2_OPEN)
#define UV_STATUS_SHUTTER1_OPEN		UV_CTR_STATUS_SH1_OPEN
#define UV_STATUS_SHUTTER2_CLOSE	(!UV_CTR_STATUS_SH2_OPEN)
#define UV_STATUS_SHUTTER2_OPEN		UV_CTR_STATUS_SH2_OPEN
INT8U	UV_STATUS_READY();
#else
///cheney: By the above comment, High level means error, but the below macro said low level is error.
///   Which is right?
#define UV_STATUS_LP1_FAULT			(PIO_Get(&UVStatPinLP1Fault) == 0)
#define UV_STATUS_LP2_FAULT			(PIO_Get(&UVStatPinLP2Fault) == 0)
#ifdef FULGENCY_FUN

#define INK_STATUS_K_LACK	(PIO_Get(&UVStatPinLP1ShCl) == 1)
#define INK_STATUS_C_LACK		(PIO_Get(&UVStatPinLP1ShOp) == 1)
#define INK_STATUS_M_LACK	(PIO_Get(&UVStatPinLP2ShCl) == 1)
#define INK_STATUS_Y_LACK		(PIO_Get(&UVStatPinLP2ShOp) == 1)

#elif !defined(MANUFACTURER_DYSS)
#define UV_STATUS_SHUTTER1_CLOSE	(PIO_Get(&UVStatPinLP1ShCl) == 0)
#define UV_STATUS_SHUTTER1_OPEN		(PIO_Get(&UVStatPinLP1ShOp) == 0)
#define UV_STATUS_SHUTTER2_CLOSE	(PIO_Get(&UVStatPinLP2ShCl) == 0)
#define UV_STATUS_SHUTTER2_OPEN		(PIO_Get(&UVStatPinLP2ShOp) == 0)
#endif
#define UV_STATUS_READY				(PIO_Get(&UVStatPinUVRdy) == 0)
#endif

#define UV_CTR_LAMP1_ON()			PIO_Set(&UVCtrlPinLP1On)
#define UV_CTR_LAMP1_OFF()			PIO_Clear(&UVCtrlPinLP1On)

#define UV_STATUS_LAMP1_ON			(PIO_Get(&UVCtrlPinLP1On) == 1)
#define UV_STATUS_LAMP2_ON			(PIO_Get(&UVCtrlPinLP2On) == 1)

#define UV_CTR_LAMP2_ON()			PIO_Set(&UVCtrlPinLP2On)
#define UV_CTR_LAMP2_OFF()			PIO_Clear(&UVCtrlPinLP2On)

#define UV_CTR_LAMP1_HIGH()			PIO_Set(&UVCtrlPinLP1HighPower)
#define UV_CTR_LAMP1_LOW()			PIO_Clear(&UVCtrlPinLP1HighPower)
#define UV_STATUS_LAMP1_HIGH		(PIO_Get(&UVCtrlPinLP1HighPower) == 1)

#define UV_CTR_LAMP2_HIGH()			PIO_Set(&UVCtrlPinLP2HighPower) 
#define UV_CTR_LAMP2_LOW()			PIO_Clear(&UVCtrlPinLP2HighPower)
#define UV_STATUS_LAMP2_HIGH		(PIO_Get(&UVCtrlPinLP2HighPower) == 1)


#if defined(OLD_UV)
#if defined(MANUFACTURER_DYSS)
#define UV_CTR_SW1_OPEN()		{UVCtrlPinLP2On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2On, 1);PIO_Set(&UVCtrlPinLP2On);}
#define UV_CTR_SW1_CLOSE()		{UVCtrlPinLP2On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2On, 1);}

#define UV_CTR_SW2_OPEN()		{UVCtrlPinLP2HighPower.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2HighPower, 1);PIO_Set(&UVCtrlPinLP2HighPower);}
#define UV_CTR_SW2_CLOSE()		{UVCtrlPinLP2HighPower.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2HighPower,1);}

#define UV_CTR_SW3_OPEN()		{UVCtrlPinLP1On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP1On, 1);PIO_Set(&UVCtrlPinLP1On);}
#define UV_CTR_SW3_CLOSE()		{UVCtrlPinLP1On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP1On, 1);}

#define UV_CTR_SW4_OPEN()		{UVCtrlPinLP1HighPower.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP1HighPower, 1);PIO_Set(&UVCtrlPinLP1HighPower);}
#define UV_CTR_SW4_CLOSE()		{UVCtrlPinLP1HighPower.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP1HighPower,1);}

#define UV_CTR_SHUTTER2_OPEN()		{UVCtrlPinSH2Open.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinSH2Open, 1);PIO_Set(&UVCtrlPinSH2Open);}
#define UV_CTR_SHUTTER2_CLOSE()		{UVCtrlPinSH2Open.type = PIO_INPUT;PIO_Configure(&UVCtrlPinSH2Open,1);}
#define UV_CTR_STATUS_SH2_OPEN		(UVCtrlPinSH2Open.type == PIO_OUTPUT_1)

#define UV_CTR_SHUTTER1_OPEN()		{UVCtrlPinSH1Open.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinSH1Open, 1);PIO_Set(&UVCtrlPinSH1Open);}
#define UV_CTR_SHUTTER1_CLOSE()		{UVCtrlPinSH1Open.type = PIO_INPUT;PIO_Configure(&UVCtrlPinSH1Open,1);}
#define UV_CTR_STATUS_SH1_OPEN		(UVCtrlPinSH1Open.type == PIO_OUTPUT_1)
#elif defined(ORIC_FUNCTION)
#define UV_CTR_SW3_OPEN()		{UVCtrlPinLP2On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2On, 1);PIO_Set(&UVCtrlPinLP2On);}
#define UV_CTR_SW3_CLOSE()		{UVCtrlPinLP2On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2On, 1);}

#define UV_CTR_SW4_OPEN()		{UVCtrlPinLP2HighPower.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2HighPower, 1);PIO_Set(&UVCtrlPinLP2HighPower);}
#define UV_CTR_SW4_CLOSE()		{UVCtrlPinLP2HighPower.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2HighPower,1);}

#define UV_CTR_SW2_OPEN()		{UVCtrlPinLP1On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP1On, 1);PIO_Set(&UVCtrlPinLP1On);}
#define UV_CTR_SW2_CLOSE()		{UVCtrlPinLP1On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP1On, 1);}

#define UV_CTR_SW1_OPEN()		{UVCtrlPinLP1HighPower.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP1HighPower, 1);PIO_Set(&UVCtrlPinLP1HighPower);}
#define UV_CTR_SW1_CLOSE()		{UVCtrlPinLP1HighPower.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP1HighPower,1);}

#define UV_CTR_SHUTTER2_OPEN()		{UVCtrlPinSH2Open.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinSH2Open, 1);PIO_Set(&UVCtrlPinSH2Open);}
#define UV_CTR_SHUTTER2_CLOSE()		{UVCtrlPinSH2Open.type = PIO_INPUT;PIO_Configure(&UVCtrlPinSH2Open,1);}
#define UV_CTR_STATUS_SH2_OPEN		(UVCtrlPinSH2Open.type == PIO_OUTPUT_1)

#define UV_CTR_SHUTTER1_OPEN()		{UVCtrlPinSH1Open.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinSH1Open, 1);PIO_Set(&UVCtrlPinSH1Open);}
#define UV_CTR_SHUTTER1_CLOSE()		{UVCtrlPinSH1Open.type = PIO_INPUT;PIO_Configure(&UVCtrlPinSH1Open,1);}
#define UV_CTR_STATUS_SH1_OPEN		(UVCtrlPinSH1Open.type == PIO_OUTPUT_1)

#elif defined(MANUFACTURER_FULGENCY_FLAT_UV)
#define UV_CTR_SW1_OPEN()		{UVCtrlPinLP2On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2On, 1);PIO_Set(&UVCtrlPinLP2On);}
#define UV_CTR_SW1_CLOSE()		{UVCtrlPinLP2On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2On, 1);}

#define UV_CTR_SW2_OPEN()		{UVCtrlPinLP2HighPower.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2HighPower, 1);PIO_Set(&UVCtrlPinLP2HighPower);}
#define UV_CTR_SW2_CLOSE()		{UVCtrlPinLP2HighPower.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2HighPower,1);}

//#define CLEAN_CTR_WIPER_OPEN()		{UVCtrlPinLP1On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP1On, 1);PIO_Set(&UVCtrlPinLP1On);}
//#define CLEAN_CTR_WIPER_CLOSE()		{UVCtrlPinLP1On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP1On, 1);}

#define UV_CTR_PRT_START_SET()		{UVCtrlPinLP1HighPower.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP1HighPower, 1);PIO_Set(&UVCtrlPinLP1HighPower);}
#define UV_CTR_PRT_START_CLEAR()		{UVCtrlPinLP1HighPower.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP1HighPower,1);}

#define UV_CTR_PRT_FINISH_SET()		{STATUS_PRT.type = PIO_OUTPUT_1;PIO_Configure(&STATUS_PRT, 1);PIO_Set(&STATUS_PRT);}
#define UV_CTR_PRT_FINISH_CLEAR()		{STATUS_PRT.type = PIO_INPUT;PIO_Configure(&STATUS_PRT,1);}

#define UV_CTR_PRT_Y_HOME_SET()		{Y_HOME_READY.type = PIO_OUTPUT_1;PIO_Configure(&Y_HOME_READY, 1);PIO_Set(&Y_HOME_READY);}
#define UV_CTR_PRT_Y_HOME_CLEAR()		{Y_HOME_READY.type = PIO_INPUT;PIO_Configure(&Y_HOME_READY,1);}

#define UV_CTR_SHUTTER1_OPEN()		PIO_Set(&UVCtrlPinSH1Open)
#define UV_CTR_SHUTTER1_CLOSE()		PIO_Clear(&UVCtrlPinSH1Open)
#define UV_CTR_STATUS_SH1_OPEN		(PIO_Get(&UVCtrlPinSH1Open) == 1)
#define UV_CTR_SHUTTER2_OPEN()		PIO_Set(&UVCtrlPinSH2Open)
#define UV_CTR_SHUTTER2_CLOSE()		PIO_Clear(&UVCtrlPinSH2Open)
#define UV_CTR_STATUS_SH2_OPEN		(PIO_Get(&UVCtrlPinSH2Open) == 1)
#elif defined(MANUFACTURER_SAIBO)||defined(MANUFACTURER_SAIBO_ROLL_UV)

#define UV_CTR_PRT_STATUS_ON() 		{UVCtrlPinLP2On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2On, 1);PIO_Set(&UVCtrlPinLP2On);}
#define UV_CTR_PRT_STATUS_OFF()		{UVCtrlPinLP2On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2On, 1);}

#define UV_CTR_UNPRT_STATUS_ON()		{UVCtrlPinLP2HighPower.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2HighPower, 1);PIO_Set(&UVCtrlPinLP2HighPower);}
#define UV_CTR_UNPRT_STATUS_OFF()		{UVCtrlPinLP2HighPower.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2HighPower,1);}

#define UV_CTR_ERR_STATUS_ON()		{UVCtrlPinLP1On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP1On, 1);PIO_Set(&UVCtrlPinLP1On);}
#define UV_CTR_ERR_STATUS_OFF()		{UVCtrlPinLP1On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP1On, 1);}

#define UV_CTR_SHUTTER2_OPEN()		{UVCtrlPinSH2Open.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinSH2Open, 1);PIO_Set(&UVCtrlPinSH2Open);}
#define UV_CTR_SHUTTER2_CLOSE()		{UVCtrlPinSH2Open.type = PIO_INPUT;PIO_Configure(&UVCtrlPinSH2Open,1);}
#define UV_CTR_STATUS_SH2_OPEN		(UVCtrlPinSH2Open.type == PIO_OUTPUT_1)

#define UV_CTR_SHUTTER1_OPEN()		{UVCtrlPinSH1Open.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinSH1Open, 1);PIO_Set(&UVCtrlPinSH1Open);}
#define UV_CTR_SHUTTER1_CLOSE()		{UVCtrlPinSH1Open.type = PIO_INPUT;PIO_Configure(&UVCtrlPinSH1Open,1);}
#define UV_CTR_STATUS_SH1_OPEN		(UVCtrlPinSH1Open.type == PIO_OUTPUT_1)

#else

#define UV_CTR_SW1_OPEN()		{UVCtrlPinLP2On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2On, 1);PIO_Set(&UVCtrlPinLP2On);}
#define UV_CTR_SW1_CLOSE()		{UVCtrlPinLP2On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2On, 1);}

#define UV_CTR_SW2_OPEN()		{UVCtrlPinLP2HighPower.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP2HighPower, 1);PIO_Set(&UVCtrlPinLP2HighPower);}
#define UV_CTR_SW2_CLOSE()		{UVCtrlPinLP2HighPower.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP2HighPower,1);}

#define UV_CTR_SW3_OPEN()		{UVCtrlPinLP1On.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP1On, 1);PIO_Set(&UVCtrlPinLP1On);}
#define UV_CTR_SW3_CLOSE()		{UVCtrlPinLP1On.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP1On, 1);}

#define UV_CTR_SW4_OPEN()		{UVCtrlPinLP1HighPower.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinLP1HighPower, 1);PIO_Set(&UVCtrlPinLP1HighPower);}
#define UV_CTR_SW4_CLOSE()		{UVCtrlPinLP1HighPower.type = PIO_INPUT;PIO_Configure(&UVCtrlPinLP1HighPower,1);}

#define UV_CTR_SHUTTER2_OPEN()		{UVCtrlPinSH2Open.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinSH2Open, 1);PIO_Set(&UVCtrlPinSH2Open);}
#define UV_CTR_SHUTTER2_CLOSE()		{UVCtrlPinSH2Open.type = PIO_INPUT;PIO_Configure(&UVCtrlPinSH2Open,1);}
#define UV_CTR_STATUS_SH2_OPEN		(UVCtrlPinSH2Open.type == PIO_OUTPUT_1)

#define UV_CTR_SHUTTER1_OPEN()		{UVCtrlPinSH1Open.type = PIO_OUTPUT_1;PIO_Configure(&UVCtrlPinSH1Open, 1);PIO_Set(&UVCtrlPinSH1Open);}
#define UV_CTR_SHUTTER1_CLOSE()		{UVCtrlPinSH1Open.type = PIO_INPUT;PIO_Configure(&UVCtrlPinSH1Open,1);}
#define UV_CTR_STATUS_SH1_OPEN		(UVCtrlPinSH1Open.type == PIO_OUTPUT_1)

#endif
#elif defined(MANUFACTURER_TATE_EPSON_UV)||defined(MANUFACTURER_HUMAN_FLAT_EPSON)||defined(MANUFACTURER_SAIBO)||defined(MANUFACTURER_RIPSTAR_TEST_UV)||defined(MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)||defined(AW_REVERSE_UV)||defined(MANUFACTURER_SAIBO_ROLL_UV)
#define UV_CTR_SHUTTER1_OPEN()		PIO_Set(&UVCtrlPinSH1Open)
#define UV_CTR_SHUTTER1_CLOSE()		PIO_Clear(&UVCtrlPinSH1Open)
#define UV_CTR_STATUS_SH1_OPEN		(PIO_Get(&UVCtrlPinSH1Open) == 1)
#define UV_CTR_SHUTTER2_OPEN()		PIO_Set(&UVCtrlPinSH2Open)
#define UV_CTR_SHUTTER2_CLOSE()		PIO_Clear(&UVCtrlPinSH2Open)
#define UV_CTR_STATUS_SH2_OPEN		(PIO_Get(&UVCtrlPinSH2Open) == 1)
#else
#define UV_CTR_SHUTTER1_OPEN()		PIO_Clear(&UVCtrlPinSH1Open)
#define UV_CTR_SHUTTER1_CLOSE()		PIO_Set(&UVCtrlPinSH1Open)
#define UV_CTR_STATUS_SH1_OPEN		(PIO_Get(&UVCtrlPinSH1Open) == 0)
#define UV_CTR_SHUTTER2_OPEN()		PIO_Clear(&UVCtrlPinSH2Open)
#define UV_CTR_SHUTTER2_CLOSE()		PIO_Set(&UVCtrlPinSH2Open)
#define UV_CTR_STATUS_SH2_OPEN		(PIO_Get(&UVCtrlPinSH2Open) == 0)
#endif

//#define UV_REQUEST_PRINTSCAN_START (PIO_Get(&DSPIrqPin) != 0)
//#define UV_REQUEST_PRINTSCAN_END (PIO_Get(&DSPIrqPin) == 0)

//#define UV_PRINTMODE_DOUBLE_LAMP	((uv_UVMode & 0x01) == 0)
//#define UV_PRINTMODE_HIGH_POWER 	((uv_UVMode & 0x02) != 0)

EXTERN volatile INT8U uv_isShutter1Open;
EXTERN volatile INT8U uv_isShutter2Open;
EXTERN volatile INT8U uv_isShutterOpen;
EXTERN volatile INT32U uv_IdleTime;

EXTERN volatile INT8U uv_UVMode; //bit0: 1 表示单灯打印    0表示双灯打印 bit1: 1 高功率    0低功率

EXTERN volatile INT8U uv_PrintDir; //单灯模式是用于控制哪个Shutter打开	
//1:正向(远离原点, 位置值增大) 0:反向(接近原点, 位置值减小)

void uv_GetStatus(UBYTE *status);
void uv_CtrUsrAction(UBYTE Usr_Ctr);

INT8U uv_Init(void);
INT8U uv_PrintScanStart(INT8U bLeft);
INT8U uv_PrintScanEnd(INT8U bLeft);
INT8U uv_PrintPause(void);
INT8U uv_PrintScanErr(void);
INT8U uv_PrintCancelJob(void);
INT8U uv_PrintEndJob(void);

#ifdef COORD_NEW_UV
void Front_Light_on(void);
void Front_Light_off(void);
void Back_Light_on(void);
void Back_Light_off(void);
void Configure_Uv_Power(INT8U mask);
#endif

#undef EXTERN
#endif
#if defined(COORD_NEW_UV)&&defined(MANUFACTURER_DYSS)
extern void Waste_Ink_CHECK(void);
#endif

#ifdef FULGENCY_FUN
extern INT8U FULGENCY_PLC_MASK;
#define START_PRT 1
#define Y_UNLOAD_FINISH 2
#endif

#endif

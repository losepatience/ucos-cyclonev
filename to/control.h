#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "led.h"
#include "global.h"
#include "ucos_ii.h"
#include "pio/pio.h"

///LED Status: APP RUN.
//#define LED_APP_RUN()	LED_Display(1, LED_INVERSE)
#define LED_APP_RUN()	

extern const Pin BellPin;

#define BELL_ON()	PIO_Clear(&BellPin)
#define BELL_OFF()	PIO_Set(&BellPin)
#define BELL_IS_ON  (PIO_Get(&BellPin) == 0)

#if !(defined( EPSON_BOTTOM_BOARD_V2_1) || defined( EPSON_BOTTOM_BOARD_V3))
extern const Pin VacuumCleanerPin;

#ifdef OLD_MOTHER_BOARD 
#define CONTROL_CLOSE_CLEANER() PIO_Clear(&VacuumCleanerPin) //{} //¹ØÎü³¾Æ÷
#define CONTROL_OPEN_CLEANER() PIO_Set(&VacuumCleanerPin) //{} //¿ªÎü³¾Æ÷
#else
#define CONTROL_CLOSE_CLEANER() PIO_Set(&VacuumCleanerPin) //{} //¹ØÎü³¾Æ÷
#define CONTROL_OPEN_CLEANER() PIO_Clear(&VacuumCleanerPin) //{} //¿ªÎü³¾Æ÷
#endif

#endif

#if defined (EPSON_CLEAN_UPDOWN)
void CONTROL_CLOSE_RELEASE_RELAY(INT8U HeadIndex);
void CONTROL_OPEN_RELEASE_RELAY(INT8U HeadIndex);
void CONTROL_CLOSE_SUCKER(INT8U HeadIndex);
void CONTROL_OPEN_SUCKER(INT8U HeadIndex);
#endif

#define ALARM_SET	True
#define ALARM_CLR	False
#define ALARM_TYPE_PUMPLONG	1
#define ALARM_TYPE_LVDSLOST 2
#define ALARM_TYPE_MENUERR  3
void Control_SetAlarm(INT8U alarmtype, INT8U opt);
void Control_PumpControl(INT8U ink_bits);
void Control_PumpCloseAll(void);

#if defined (MICOLOR_AUTOFUCTION)
INT8U IsLoadMediaSensorValid();
INT8U IsCoverSensorValid();
INT8U IsMediaFixed();
INT8U IsOutOfMedia();

//mode : 1, strong; 0, weak.
#define PLATE_FAN_STRONG    1
#define PLATE_FAN_WEAK      0
INT8U OpenPlateFan(INT8U mode);
INT8U ClosePlateFan();

#endif

#ifdef MANUFACTURER_DYSS
INT8U OpenPlateFan();
INT8U ClosePlateFan();
#endif

#if (defined(HEAD_RICOH_G4)||defined(EPSON_4H))
extern INT32U INK_LEVEL_MASK;
void Ink_level_map(INT8U head_level);
#endif
#if defined( EPSON_BOTTOM_BOARD_V3)||defined( EPSON_BOTTOM_BOARD_V2_1)
INT8U ResetHeaterInfo();
INT8U SaveHeaterInfo();
INT8U ReadHeaterInfo();
INT8U Heater_Init();
INT8U Heater_Urgent_Stop();
#endif

#ifdef FULGENCY_FUN
void Main_Color_Level_Scan(void);
#endif

void Needle_Open(void);
void Needle_Close(void);

#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
	void Open_Induced_Draft(void);
	void Close_Induced_Draft(void);
	void Open_Paper_Knife(void);
	void Close_Paper_Knife(void);
	void Cut_Paper (void);
#endif

#ifdef TIMER_ON
extern Timer control_timer;
#endif

#endif

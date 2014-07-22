/*
************************************************************************
* FPGA nGCS = 2	ADDRESS: 0x10000000~0x18000000                      *
* ADDRESS LINE: A0, A1, A2, A3											*
* DMA: nXDREQ0		Interrupt: EINT2(GPF2)                              *
************************************************************************
*/

//On Hardware, when we configure FPGA with ARM, should use R914 and remove R913.

//GPE0: nCONFIG
//GPE1: CONF_DONE
//GPE2: nSTATUS
//GPE3: DATA
//GPE4: DCLK

//CLKOUT0/GPH9
//CLKOUT1/GPH10

//(EINT11/nSS1/)GPG3(I)		XDIR
//EINT13(/SPIMISO1/GPG5)	XPULSE
//(EINT14/SPIMOSI1/)GPG6(I)	YDIR
//EINT15(/SPICLK1/GPG7)		YPULSE

//(UEXTCLK/)GPH8(O)			nFire

//GPF7(O)	nLED

#ifndef _FPGA_H_
#define _FPGA_H_

#include "pio/pio.h"

//FPGA Registers
#define FPGA_FIFO_ADDR	AT91C_EBI_CS5 /* 0x60000000 */

//For burst write, FIFODATA use 0x60000000
#define rFPGA_FIFODAT	(*(volatile unsigned short*)FPGA_FIFO_ADDR)
#define rFPGA_STATUS	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x2))	//FPGA status register
//only for ricoh
#define rFPGA_STATUS_ERR_CMD    0x8000
#define rFPGA_STATUS_ERR_STAT   0x4000
#define rFPGA_STATUS_TOO_MUCH   0x40
#define rFPGA_STATUS_TOO_LESS   0x10


#define rFPGA_PINCONF   (*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x8)) 
//only for ricoh
#define rFPGA_PINCONF_W_MASK    0x3FFF
#define rFPGA_PINCONF_LOCKED    0x8000
#define rFPGA_PINCONF_ENBALE    0x4000

//#define rFPGA_FWCONF	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0xA)) //Fire working configuration register
#define rFPGA_COMMAND   (*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0xC))
#define rFPGA_COMMAND_BEGIN_BAND    0x55AA
#define rFPGA_COMMAND_END_BAND      0x01fe
#ifdef EPSON_FLASH_IDLE
#define rFPGA_COMMAND_BEGIN_FLASH_IDLE   0x2000
#endif
#define rFPGA_COMMAND_BEGIN_FLASH   0x1000

#ifdef CONVERSION_BOARD 
#define rFPGA_COMMAND_FLASH_HIGH_BIT   0xC000
#endif

#define rFPGA_COMMAND_END_FLASH     0x0110
//only for ricoh
#define rFPGA_COMMAND_BGN_AUTO_C_1  0x2AA2
#define rFPGA_COMMAND_BGN_AUTO_C_0  0x3553
#define rFPGA_COMMAND_BGN_LSHAKE_1  0x4AA4
#define rFPGA_COMMAND_BGN_LSHAKE_0  0x6556
#define rFPGA_COMMAND_BGN_SSHAKE    0x7AA7
#define rFPGA_COMMAND_END_SSHAKE    0x8558
#define rFPGA_COMMAND_BGN_SSHAKE_100    0x9AA9
#define rFPGA_COMMAND_BGN_SSHAKE_250    0xA55A
#define rFPGA_COMMAND_CLEAR_ERR     0xBAAB

#ifdef HEAD_EPSON_GEN5

//only for epson
#define rFPGA_DATASEG	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0xE))

//only for FPGA debug
#define rFPGA_DBG_FireNum   (*(volatile unsigned short*)(FPGA_FIFO_ADDR)) 
#define rFPGA_DBG_ReadNum_L   (*(volatile unsigned short*)(FPGA_FIFO_ADDR+4)) 
#define rFPGA_DBG_ReadNum_H   (*(volatile unsigned short*)(FPGA_FIFO_ADDR+6)) 
#define rFPGA_DBG_WriteNum_L   (*(volatile unsigned short*)(FPGA_FIFO_ADDR+0xA)) 
#define rFPGA_DBG_WriteNum_H   (*(volatile unsigned short*)(FPGA_FIFO_ADDR+0xE)) 

#if (defined(EPSON_BOTTOM_BOARD_V3)||defined (EPSON_BOTTOM_BOARD_V2_1)) && !defined(SUPPORT_MOTOR_CONTROL)
#error "Current EPSON_BOTTOM_BOARD_V3 must need SUPPORT_MOTOR_CONTROL"
#endif

#if defined(COORD_NEW_UV)
#define rFPGA_EPSON_RegAddr	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x1A))
#define rFPGA_EPSON_RegPort	 (*(volatile unsigned int*)(FPGA_FIFO_ADDR + 0x1C))
#define EPSON_REGADDR_X_PRT_COOR        0x2
#endif

#if defined(SUPPORT_MOTOR_CONTROL) || defined(FPGA_MAINTAIN_COOR)|| defined(EPSON_PRT_FLASH)

//only for epson private register
#define rFPGA_EPSON_RegAddr	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x1A))
#define rFPGA_EPSON_RegPort	 (*(volatile unsigned int*)(FPGA_FIFO_ADDR + 0x1C))
#define rFPGA_EPSON_RegPortL (*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x1C))
#define rFPGA_EPSON_RegPortH (*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x1E))

#define EPSON_REGADDR_X_MOTOR_COOR      0x1
#define EPSON_REGADDR_X_PRT_COOR        0x2
#define EPSON_REGADDR_Y_MOTOR_COOR      0x4
// #define EPSON_REGADDR_Y_FEEDBACK_COOR   0x8
//--- ER_CoorStat(0), X_Locked;
//--- ER_CoorStat(1), X_Smoothed;
//--- ER_CoorStat(2), X_Dir; 0, Pos; 1, Neg
//--- ER_CoorStat(3), X_Dir_motion;
//--- ER_CoorStat(4), Y_Locked;
//--- ER_CoorStat(5), X_InFireRegion;
//--- ER_CoorStat(6), Y_Dir;
//--- ER_CoorStat(7), Y_Dir_motion;
//--- ER_CoorStat(8), warning flag : true if not smoothed when print. read ER_CoorStat will clear it;
//--- ER_CoorStat(9), warning flag : true if fire freq overrun when print. read ER_CoorStat will clear it;
#define EPSON_REGADDR_COOR_STAT         0x10
#define ER_CoorStat_OverFreq    0x200
//--- ER_CoorCtrl(0~2), smoother_divider num(1-8); 
//--- ER_CoorCtrl(3~5), smoother_multiplier num(1-8);   
//--- ER_CoorCtrl(6~7), print_divider num(1-4);
//--- ER_CoorCtrl(8~9), print_phase(0-3);
//--- ER_CoorCtrl(10), enable fire;
//--- ER_CoorCtrl(11), enable flash;
//--- ER_CoorCtrl(12~13), pass dir when fire; 0, none, 1, positive, 2, negative, 3, all.
//--- ER_CoorCtrl(14), X coor system reset.
//--- ER_CoorCtrl(15), Y coor system reset.
//--- ER_CoorCtrl(16~17), VSD Model(1-4).
#define EPSON_REGADDR_COOR_CTRL         0x20
#define ER_CoorCtrl_SMOOTH_DIVIDER      0x7
#define ER_CoorCtrl_SMOOTH_DIVIDER_720  0x3
#define ER_CoorCtrl_SMOOTH_DIVIDER_540  0x3
#define ER_CoorCtrl_SMOOTH_DIVIDER_1440 0x3
#define ER_CoorCtrl_SMOOTH_MULTI        (0x7<<3)
#define ER_CoorCtrl_SMOOTH_MULTI_720    (0x3<<3)
#define ER_CoorCtrl_SMOOTH_MULTI_540    (0x2<<3)
#define ER_CoorCtrl_SMOOTH_MULTI_1440   (0x7<<3)
#define ER_CoorCtrl_PRINT_DIVIDER      (0x3<<6)
#define ER_CoorCtrl_PRINT_DIVIDER_NONE  (0x0<<6)
#define ER_CoorCtrl_PRINT_DIVIDER_HALF  (0x1<<6)
#define ER_CoorCtrl_PRINT_DIVIDER_THREE (0x2<<6)
#define ER_CoorCtrl_PRINT_DIVIDER_QUAD  (0x3<<6)
#define ER_CoorCtrl_PRINT_PHASE        (0x3<<8)
#define ER_CoorCtrl_PRINT_PHASE_0       (0x0<<8)
#define ER_CoorCtrl_PRINT_PHASE_1       (0x1<<8)
#define ER_CoorCtrl_PRINT_PHASE_2       (0x2<<8)
#define ER_CoorCtrl_PRINT_PHASE_3       (0x3<<8)
#define ER_CoorCtrl_FIRE        (0x1<<10)
#define ER_CoorCtrl_FLASH       (0x1<<11)
#define ER_CoorCtrl_EN_DIR      (0x3<<12)
#define ER_CoorCtrl_EN_DIR_NONE      (0x0<<12)
#define ER_CoorCtrl_EN_DIR_POS       (0x1<<12)
#define ER_CoorCtrl_EN_DIR_NEG       (0x2<<12)
#define ER_CoorCtrl_EN_DIR_ALL       (0x3<<12)
#define ER_CoorCtrl_RESET_X       (0x1<<14)
#define ER_CoorCtrl_RESET_Y       (0x1<<15)
#define ER_CoorCtrl_VSDMODEL      (0x3<<16)
#define ER_CoorCtrl_VSDMODEL_VSD1 (0x0<<16)
#define ER_CoorCtrl_VSDMODEL_VSD2 (0x1<<16)
#define ER_CoorCtrl_VSDMODEL_VSD3 (0x2<<16)
#define ER_CoorCtrl_VSDMODEL_VSD4 (0x3<<16)
#define EPSON_REGADDR_X_FIRE_START      0x40
#define EPSON_REGADDR_X_FIRE_END        0x80
//#define EPSON_REGADDR_X_FLASH_START     0x100
//#define EPSON_REGADDR_X_FLASH_END       0x200
#define EPSON_REGADDR_POSI_STAT         0x400
#define EPSON_REGADDR_POSI_CTRL         0x800
#define EPSON_REGADDR_X_POSI_0          0x1000
#define EPSON_REGADDR_X_POSI_1          0x8
//#define EPSON_REGADDR_X_POSI_2          0x78
//#define EPSON_REGADDR_X_POSI_3          0x7C
#define EPSON_REGADDR_SENSOR_CTRL       0x2000
#define EPSON_REGADDR_SENSOR_STAT       0x4000
#define EPSON_REGADDR_SENSOR_IO         0x8000

void SetEpsonRegShort(INT16U addr, INT16U data);
void SetEpsonRegInt(INT16U addr, INT32U data);
INT16U ReadEpsonRegShort(INT16U addr);
INT32U ReadEpsonRegInt(INT16U addr);
INT16U ReadSafeEpsonRegShort(INT16U addr);
INT32U ReadSafeEpsonRegInt(INT16U addr);

void SetSafeCmd(INT16U data);
#endif

#elif defined(HEAD_RICOH_G4)
//for ricoh
//must read/write low short at first, then high short.
#define EPSON_REGADDR_X_MOTOR_COOR      0x1
#define EPSON_REGADDR_X_PRT_COOR        0x2
#define EPSON_REGADDR_Y_MOTOR_COOR      0x4
#define rFPGA_RICOH_COORCTRL_L	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x10))
#define ER_CoorCtrl_SMOOTH_DIVIDER      0x7
#define ER_CoorCtrl_SMOOTH_DIVIDER_720  0x3
#define ER_CoorCtrl_SMOOTH_DIVIDER_540  0x3
#define ER_CoorCtrl_SMOOTH_DIVIDER_1440 0x3
#define ER_CoorCtrl_SMOOTH_MULTI        (0x7<<3)
#define ER_CoorCtrl_SMOOTH_MULTI_720    (0x3<<3)
#define ER_CoorCtrl_SMOOTH_MULTI_540    (0x2<<3)
#define ER_CoorCtrl_SMOOTH_MULTI_1440   (0x7<<3)
#define ER_CoorCtrl_PRINT_DIVIDER      (0x3<<6)
#define ER_CoorCtrl_PRINT_DIVIDER_NONE  (0x0<<6)
#define ER_CoorCtrl_PRINT_DIVIDER_HALF  (0x1<<6)
#define ER_CoorCtrl_PRINT_DIVIDER_THREE (0x2<<6)
#define ER_CoorCtrl_PRINT_DIVIDER_QUAD  (0x3<<6)
#define ER_CoorCtrl_PRINT_PHASE        (0x3<<8)
#define ER_CoorCtrl_PRINT_PHASE_0       (0x0<<8)
#define ER_CoorCtrl_PRINT_PHASE_1       (0x1<<8)
#define ER_CoorCtrl_PRINT_PHASE_2       (0x2<<8)
#define ER_CoorCtrl_PRINT_PHASE_3       (0x3<<8)
#define ER_CoorCtrl_FIRE        (0x1<<10)
//#define ER_CoorCtrl_FLASH       (0x1<<11) // ONLY EPSON,RICOH NOT USED
#define ER_CoorCtrl_CHECK_FREQ  (0x1<<11) // ONLY For RICOH,
#define ER_CoorCtrl_EN_DIR      (0x3<<12)
#define ER_CoorCtrl_EN_DIR_NONE      (0x0<<12)
#define ER_CoorCtrl_EN_DIR_POS       (0x1<<12)
#define ER_CoorCtrl_EN_DIR_NEG       (0x2<<12)
#define ER_CoorCtrl_EN_DIR_ALL       (0x3<<12)
#define ER_CoorCtrl_RESET_X       (0x1<<14)
#define ER_CoorCtrl_RESET_Y       (0x1<<15)
#define rFPGA_RICOH_COORCTRLSTAT_H	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x12))
#define ER_CoorCtrl_SINGLE_DOT_MODE (0x1<<29)
#define ER_CoorCtrl_VSDMODEL      (0x3<<30)
#define ER_CoorCtrl_VSDMODEL_VSD1 (0x0<<30)
#define ER_CoorCtrl_VSDMODEL_VSD2 (0x1<<30)
#define ER_CoorCtrl_VSDMODEL_VSD3 (0x2<<30)
#define ER_CoorCtrl_VSDMODEL_VSD4 (0x3<<30)
#define rFPGA_RICOH_FIRESTART_L	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x14))
#define rFPGA_RICOH_FIRESTART_END_H	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x16))
#define rFPGA_RICOH_FIREEND_L	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x18))
#define rFPGA_RICOH_XPOSI0_L    (*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x1C))
#define rFPGA_RICOH_XPOSI0_CS_H    (*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x1E))
#define rFPGA_RICOH_XPOSI0_MATCH_ENABLE (0x1 << 8)
#define rFPGA_RICOH_XPOSI0_COOR_TYPE (0x2 << 8) //0, feedback, 1, motor
#define rFPGA_RICOH_XPOSI0_PASS_DIR (0xC << 8)
#define rFPGA_RICOH_XPOSI0_DIR_NONE (0x0 << 8) 
#define rFPGA_RICOH_XPOSI0_DIR_POSITIVE (0x4 << 8) 
#define rFPGA_RICOH_XPOSI0_DIR_NEGATIVE (0x8 << 8) 
#define rFPGA_RICOH_XPOSI0_DIR_ALL (0xC << 8) 
#define rFPGA_RICOH_XPOSI0_IRQ_STAT (0x10 << 8)

#define rFPGA_RICOH_XMOTORCOOR_L	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x4))
#define rFPGA_RICOH_XMOTORCOOR_H	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x6))
#define rFPGA_RICOH_XPRTCOOR_L	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0xA))
#define rFPGA_RICOH_XPRTCOOR_H	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0xE))
#define rFPGA_RICOH_YMOTORCOOR_L	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x0))
#define rFPGA_RICOH_YMOTORCOOR_H	(*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x1A))

#endif

extern const Pin FPGAnConfigPin;
extern const Pin FPGADclkPin;
extern const Pin FPGAData0Pin;
extern const Pin FPGAnStatusPin;
extern const Pin FPGAConfDonePin;

//REGISTER ADDR: 0x60000001~0x6000000F

#define nCONFIG_HIGH()	PIO_Set(&FPGAnConfigPin) 
#define nCONFIG_LOW()	PIO_Clear(&FPGAnConfigPin) 

#define DATA_HIGH()		PIO_Set(&FPGAData0Pin)
#define DATA_LOW()		PIO_Clear(&FPGAData0Pin)		
#define DCLK_HIGH()		PIO_Set(&FPGADclkPin)
#define DCLK_LOW()		PIO_Clear(&FPGADclkPin)		

#define CONF_DONE_LOW	(PIO_Get(&FPGAConfDonePin) == 0) 
#define CONF_DONE_HIGH	(PIO_Get(&FPGAConfDonePin) != 0)
#define nSTATUS_LOW	(PIO_Get(&FPGAnStatusPin) == 0)
#define nSTATUS_HIGH	(PIO_Get(&FPGAnStatusPin) != 0)

#ifdef HEAD_EPSON_GEN5	
void FPGA_START_FLASH(INT16U interval_us, INT8U HeadMask);
void FPGA_START_FLASH_ALLWIN(INT16U interval_us, INT8U HeadMask);
void FPGA_STOP_FLASH_Safe();
void FPGA_STOP_FLASH();
#elif defined(HEAD_RICOH_G4)

#ifndef CONVERSION_BOARD
void FPGA_START_RICOH_AUTO_C();
#endif

#define FPGA_START_FLASH(interval_us, HeadMask) FPGA_START_FLASH_RICOH(HeadMask)
#define FPGA_START_FLASH_ALLWIN(interval_us, HeadMask) FPGA_START_FLASH_RICOH(HeadMask)
void FPGA_START_FLASH_RICOH(INT8U DataMask);

#define FPGA_STOP_FLASH_Safe FPGA_STOP_FLASH
void FPGA_STOP_FLASH();
#endif

#if defined(HEAD_EPSON_GEN5	) && (defined(SUPPORT_MOTOR_CONTROL) || defined(FPGA_MAINTAIN_COOR))
#define FPGA_BEGIN_BAND() SetSafeCmd(0x55AA)
#define FPGA_END_BAND()   SetSafeCmd(0x01FE)
#else
#define FPGA_BEGIN_BAND() (rFPGA_COMMAND = 0x55AA)
#define FPGA_END_BAND()   (rFPGA_COMMAND = 0x01FE)
#endif

#ifdef HEAD_EPSON_GEN5	
#define FPGA_SET_DATASEG(data)	(rFPGA_DATASEG = data)  //only for EPSON Gen5
#define FPGA_LAST_DATASEG	0x8000
#endif

#if defined(HEAD_RICOH_G4)
struct sShakeInfo_Ricoh
{
	INT8U Large_shake_1_unit_count;
	INT8U Large_shake_0_unit_count;
	INT16U Small_shake_0_shake_count;
};
extern struct sShakeInfo_Ricoh shakeInfo_Ricoh;
#endif

INT8U FPGA_RESET(void);
INT8U FPGA_Init(void);
INT8U FPGA_Cfg(void);
INT8U HugeBuf_Flush(INT32S size); // size == HUGEBUF_ALIGN_SIZE * N	
#if defined(HEAD_EPSON_GEN5)
INT8U FPGA_SendData(INT8U first, INT32U PreFireNum);
#else
INT8U FPGA_SendData(INT8U first);
#endif

#endif

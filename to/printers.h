#ifndef _PRINTERS_H_
#define _PRINTERS_H_

#include "os_includes.h"
#include    "os_cpu.h"

//MSB, Hardware version, version, sub-version, build, LSB
//#define BOARD_VERSION	   0x01010300 //Change voltage map
//#define BOARD_VERSION	   0x01010301 //Change control.c and control.h, fix pump bugs. 
//#define BOARD_VERSION	   0x01010400 //Add password control
//#define BOARD_VERSION	   0x01010401 //Change data transfer method to burst: SDRAM => FPGA
//#define BOARD_VERSION	   0x01010402 //Add some interface for flatbed printer
//#define BOARD_VERSION	   0x01020000 //Add some interface for UV flatbed printer, x/y/z/f motion, First relase in Human
//#define BOARD_VERSION	   0x01020100 //Add language protection 20081114
//#define BOARD_VERSION	   0x01020200 //Y Encoder adjust length, 步进立即生效，双向校准微调 20090327
//#define BOARD_VERSION	   0x01020300 //Support Konica new 512 head board
//#define BOARD_VERSION	   0x01020301 //Support auto flashing when begin job and printing....
//#define BOARD_VERSION	   0x01020302 //Support media sensor
//#define BOARD_VERSION	   0x01020303 //Support KM512 16 head version 2
//#define BOARD_VERSION	   0x01040404 //Special version to change the manufacturer ID, 仅给过COLORJET的临时版本.

//#define BOARD_VERSION	   0x01020304 //Support KM512LNX printhead 20091112
//#define BOARD_VERSION	   0x01020305 //Add one command to support Y-reverse printing
//#define BOARD_VERSION	   0x01020306 //Support spectra BYHX_HB_V4, support DSP private data, 20100106
//#define BOARD_VERSION	   0x01020307 //Support KM512LNX ALL in ONE, 可以支持旧头板
//#define BOARD_VERSION	   0x01020400 //增加状态级别0x00000200，于motion配合。使用老版本时（motion为新版本），这个级别的信息被忽略
//使用此版本，用老的motion没有影响。	20100128

//#define BOARD_VERSION	   0x01020500 //对GZ项目，增加软件版本限制
//#define BOARD_VERSION	   0x01020501 //KM512LN增加超频（7.5KHZ），要求头板版本为2.3（CPLD改动）
//#define BOARD_VERSION	   0x01020600 //20100702,  支持KM512LN打印6.6KZ，条件是头版版本大于等于x.2.1.1, motion版本大于等于x.x.3

//#define BOARD_VERSION	   0x01020601 //Support ink limited password。

//#define BOARD_VERSION	   0x02010101 //Cheney: the first version for updated board.	20100519
//#define BOARD_VERSION	   0x02020601 //Cheney: after merge code, update to the same version NO. 20100812

//#define BOARD_VERSION	   0x02030601 //Cheney: support Epson printhead
//#define BOARD_VERSION	   0x02000500 //Handy: Add command to switch KM1024 14PL and 42PL
//#define BOARD_VERSION	   0x02000600 //ALL Konica head KM256/KM512/KM1024 and 64M/128M memeory use same program.

#define BOARD_VERSION	   0x02030601 //Cheney: support Epson printhead

#define LIYU 			0x1 //Manufacture ID
#define INWEAR 			0x2
#define ALLWIN			0x3
#define KINCOLOR		0x4
#define HUMAN			0x5
#define MYJET			0x6
#define LOTUS			0x7
#define COLORJET		0x8
#define HAPOND			0x9
#define RODINJET		0xA
#define GZ				0xB
#define BRITEJET 		0xC
#define JHF      		0xD
#define VOXINDIA		0xE
#define WITCOLOR		0xF
#define MONOTECH		0x10
#define INWEAR_UV  0x13
#define MICOLOR 		0x14
#define HUMAN_STITCH	 0x15
#define TOPJET          0x16
#define DESIGN          0x17
#define DESIGN_UV          0x97
#define TATE            0x18
#define TATE_UV         0x98
#define INKWIN          0X1D
#define BEMAJET         0x1B
#define ALLWIN_WATER_4H 0x1E
#define RASTER          0x21


#define ALLWIN_WATER_2H			0x50
#define ALLWIN_4H			0x51

#define ALLWIN_4H_UV			0xD1

#define DYSS_RICOH 0x30


#define SHANGHAI_YAODI 0x31
#define TENGFENG 0x32
#define LONGKE 0X33
#define SAISHUN 0X34
#define SAISHUN_WATER 0X35
#define RIPSTAR_TEST 0X36
#define ALLWIN_2H_ROLL_UV 0X37
#define MARTIN 0X38
#define DGEN 0X39
#define FULGENCY 0X3A
#define ZHONGYE 0X3B
#define XINHANGJIDIAN_STITCH 0x3C
#define XUKE 0x3D
#define ORIC 0x3E
#define RUICAI 0x3F
#define LECAI_4H 0x40
#define RASTER_4H 0X41
#define LAILI 0X42
#define ALLWIN_EPSON    0x43
#define COLORFUL    0x44

#define DYSS_RICOH_FLAT_UV 0xB0
#define LONGKE_FLAT_UV 0XB3
#define RIPSTAR_UV_TEST 0XB6
#define FULGENCY_FLAT_UV 0XBA
#define ZHONGYE_FLAT_UV 0XBB
#define ORIC_ROLL_UV 0xBC
#define ORIC_FLAT_UV 0xBE
#define LECAI_FLAT_UV 0xC0

#define INWEAR_FLAT		0x82
#define ALLWIN_FLAT 	0x83
#define KINCOLOR_FLAT	0x84
#define HUMAN_FLAT		0x85
#define MYJET_FLAT		0x86
#define HAPOND_FLAT		0x89
#define RODINJET_FLAT	0x8A
#define GZ_UV			0x8B
#define JHF_UV     		0x8D
#define VOXINDIA_UV		0x8E
#define WITCOLOR_UV		0x8F
#define MONOTECH_UV		0x90
#define DOCAN_FLAT		0x92
#define INWEAR_FLAT_UV		0x93
#define MICOLOR_UV 		0x94
#define TOPJET_UV       0x96
#define INKWIN_UV          0X9D
#define RASTER_UV       0xA1


#define MICOLOR_EPSON   0x54

#ifdef BYHX_WTITE_BOARD_TOOL
#define MANUFACTURER_NAME 	0xFFFF
#define PRODUCT_ID			0xFFFF		
#else
#ifdef MANUFACTURER_LIYU
#define MANUFACTURER_NAME LIYU
#endif	

#ifdef MANUFACTURER_SAISHUN_WATER_EPSON
#define MANUFACTURER_NAME SAISHUN_WATER
#endif

#ifdef MANUFACTURER_XUKE
#define MANUFACTURER_NAME XUKE
#endif

#ifdef MANUFACTURER_ADDTOP_EPSON
#define MANUFACTURER_NAME RIPSTAR_TEST
#endif

#ifdef MANUFACTURER_RIPSTAR_TEST
#define MANUFACTURER_NAME RIPSTAR_TEST
#endif

#ifdef MANUFACTURER_MARTIN
#define MANUFACTURER_NAME MARTIN
#endif

#ifdef MANUFACTURER_DGEN
#define MANUFACTURER_NAME DGEN
#endif

#ifdef MANUFACTURER_FULGENCY_FLAT_UV
#define MANUFACTURER_NAME FULGENCY_FLAT_UV
#endif

#ifdef MANUFACTURER_SAIBO
#define MANUFACTURER_NAME ALLWIN_FLAT
#endif

#ifdef MANUFACTURER_SAIBO_ROLL_UV
#define MANUFACTURER_NAME RIPSTAR_UV_TEST
#endif

#ifdef MANUFACTURER_INWEAR
#define MANUFACTURER_NAME INWEAR
#endif	

#ifdef MANUFACTURER_DOCAN_UV
#define MANUFACTURER_NAME DOCAN_FLAT
#endif	

#ifdef MANUFACTURER_INWEAR_FLAT
#define MANUFACTURER_NAME INWEAR_FLAT
#endif

#ifdef MANUFACTURER_INWEAR_FLAT_UV
#define MANUFACTURER_NAME INWEAR_FLAT_UV
//#define MANUFACTURER_NAME ALLWIN
#endif

#ifdef MANUFACTURER_ALLWIN
#define MANUFACTURER_NAME ALLWIN
#endif

#ifdef MANUFACTURER_DYSS
#define MANUFACTURER_NAME DYSS_RICOH_FLAT_UV
//#define MANUFACTURER_NAME ALLWIN
#endif

#ifdef MANUFACTURER_DYSS_16H_FLAT
#define MANUFACTURER_NAME DYSS_RICOH_FLAT_UV
#endif

#ifdef MANUFACTURER_DYSS_8H
#define MANUFACTURER_NAME DYSS_RICOH_FLAT_UV
#endif

#ifdef MANUFACTURER_ALLWIN_FLAT
#define MANUFACTURER_NAME ALLWIN_FLAT
#endif

#ifdef MANUFACTURER_ALLWIN_2H_ROLL_UV
#define MANUFACTURER_NAME ALLWIN_FLAT
#endif

#ifdef MANUFACTURER_ALLWIN_4H_ROLL_UV
#define MANUFACTURER_NAME ALLWIN_4H_UV
#endif

#ifdef MANUFACTURER_ALLWIN_EPSON
#define MANUFACTURER_NAME ALLWIN
#endif

#ifdef MANUFACTURER_KINCOLOR_FLAT
#define MANUFACTURER_NAME KINCOLOR_FLAT
#endif

#ifdef MANUFACTURER_ALLWIN_EPSON_WATER_4H
#define MANUFACTURER_NAME ALLWIN_WATER_4H
#endif

#ifdef MANUFACTURER_ALLWIN_EPSON_WATER_2H
#define MANUFACTURER_NAME ALLWIN_WATER_2H
#endif

#ifdef MANUFACTURER_ALLWIN_EPSON_4H
#define MANUFACTURER_NAME ALLWIN_4H
#endif

#ifdef MANUFACTURER_LECAI_EPSON_4H
#define MANUFACTURER_NAME LECAI_4H
#endif


#ifdef MANUFACTURER_ALLWIN_EPSON_4H_UV
#define MANUFACTURER_NAME ALLWIN_4H_UV
#endif
//ALLWIN_EPSON_SAME
#ifdef MANUFACTURER_TATE_EPSON
#define MANUFACTURER_NAME TATE
//#define MANUFACTURER_NAME GZ_UV
#endif

#ifdef MANUFACTURER_ORIC
#define MANUFACTURER_NAME ORIC_ROLL_UV
#endif

#ifdef MANUFACTURER_ORIC_COMMON
#define MANUFACTURER_NAME ORIC
//#define MANUFACTURER_NAME ORIC_ROLL_UV
#endif

#ifdef MANUFACTURER_ORIC_FLAT
#define MANUFACTURER_NAME ORIC_FLAT_UV
#endif

#ifdef MANUFACTURER_TENGFENG_EPSON
#define MANUFACTURER_NAME TENGFENG
#endif

#ifdef MANUFACTURER_YAODI_EPSON
//#define MANUFACTURER_NAME SHANGHAI_YAODI
#define MANUFACTURER_NAME TATE
#endif

#ifdef MANUFACTURER_WANMENG_EPSON
#define MANUFACTURER_NAME TATE
#endif

#ifdef MANUFACTURER_TATE_EPSON_UV
#define MANUFACTURER_NAME TATE_UV
//#define MANUFACTURER_NAME GZ_UV
#endif

#ifdef MANUFACTURER_ZHONGYE_FLAT_UV
#define MANUFACTURER_NAME ZHONGYE_FLAT_UV
#endif
#ifdef MANUFACTURER_ZHONGYE_ROLL_UV
#define MANUFACTURER_NAME ZHONGYE_FLAT_UV
#endif

#ifdef MANUFACTURER_LONGKE_FLAT_UV_150
#define MANUFACTURER_NAME LONGKE_FLAT_UV
#endif

#ifdef MANUFACTURER_LONGKE_FLAT_UV_130
#define MANUFACTURER_NAME LONGKE_FLAT_UV
#endif

#ifdef MANUFACTURER_BEMAJET
#define MANUFACTURER_NAME BEMAJET
#endif

#ifdef MANUFACTURER_KINCOLOR
#define MANUFACTURER_NAME KINCOLOR
#endif

#ifdef MANUFACTURER_HUMAN
#define MANUFACTURER_NAME HUMAN
#endif

#ifdef MANUFACTURER_HUMAN_STITCH
#define MANUFACTURER_NAME HUMAN_STITCH	
#endif

#ifdef MANUFACTURER_XINHANGJIDIAN_STITCH
#define MANUFACTURER_NAME XINHANGJIDIAN_STITCH	
#endif

#ifdef MANUFACTURER_HUMAN_FLAT_EPSON
#define MANUFACTURER_NAME HUMAN_FLAT
#endif

#ifdef MANUFACTURER_HUMAN_EPSON
#define MANUFACTURER_NAME HUMAN
#endif

#ifdef MANUFACTURER_COLORJET
#define MANUFACTURER_NAME COLORJET
#endif

#ifdef MANUFACTURER_COLORJET_EPSON
#define MANUFACTURER_NAME COLORJET
//#define MANUFACTURER_NAME ALLWIN
#endif

#ifdef MANUFACTURER_HAPOND
#define MANUFACTURER_NAME HAPOND
#endif

#ifdef MANUFACTURER_RODINJET
#define MANUFACTURER_NAME RODINJET
#endif

#ifdef MANUFACTURER_KINCOLOR_FLAT
#define MANUFACTURER_NAME KINCOLOR_FLAT
#endif

#ifdef MANUFACTURER_HUMAN_FLAT
#define MANUFACTURER_NAME HUMAN_FLAT
#endif

#ifdef MANUFACTURER_MYJET
#define MANUFACTURER_NAME MYJET
#endif

#ifdef MANUFACTURER_MYJET_FLAT
#define MANUFACTURER_NAME MYJET_FLAT
#endif

#ifdef MANUFACTURER_LOTUS
#define MANUFACTURER_NAME LOTUS
#endif

#ifdef MANUFACTURER_RASTER
#define MANUFACTURER_NAME RASTER
#endif

#ifdef MANUFACTURER_RASTER_4H
#define MANUFACTURER_NAME RASTER_4H
#endif

#ifdef MANUFACTURER_HAPOND_FLAT
#define MANUFACTURER_NAME HAPOND_FLAT
#endif

#ifdef MANUFACTURER_RODINJET_FLAT
#define MANUFACTURER_NAME RODINJET_FLAT
#endif

#ifdef MANUFACTURER_GZ
#define MANUFACTURER_NAME GZ
//#define MANUFACTURER_NAME GZ_UV
#endif

#ifdef MANUFACTURER_GZ_UV
#define MANUFACTURER_NAME GZ_UV
#endif

#ifdef MANUFACTURER_BRITEJET
#define MANUFACTURER_NAME BRITEJET
#endif

#ifdef MANUFACTURER_JHF
#define MANUFACTURER_NAME JHF
#endif

#ifdef MANUFACTURER_INKWIN
#define MANUFACTURER_NAME INKWIN
#endif

#ifdef MANUFACTURER_VOXINDIA
#define MANUFACTURER_NAME VOXINDIA
#endif	

#ifdef MANUFACTURER_WITCOLOR
#define MANUFACTURER_NAME WITCOLOR
#endif	

#ifdef MANUFACTURER_WITCOLOR_UV
#define MANUFACTURER_NAME WITCOLOR_UV
#endif	

#ifdef MANUFACTURER_MONOTECH
#define MANUFACTURER_NAME MONOTECH
#endif	

#ifdef MANUFACTURER_MICOLOR_EPSON
//#define MANUFACTURER_NAME MICOLOR_EPSON
#define MANUFACTURER_NAME MICOLOR
//#define MANUFACTURER_NAME ALLWIN
#endif

#ifdef MANUFACTURER_COLORFUL
#define MANUFACTURER_NAME COLORFUL
#endif
#ifdef MANUFACTURER_TOPJET_EPSON
//#define MANUFACTURER_NAME ALLWIN
#define MANUFACTURER_NAME TOPJET
#endif

#ifdef MANUFACTURER_RUICAI_EPSON
#define MANUFACTURER_NAME RUICAI
//#define MANUFACTURER_NAME TOPJET
#endif

#ifdef MANUFACTURER_DESIGN_EPSON
#define MANUFACTURER_NAME DESIGN
#endif	

#ifdef MANUFACTURER_DESIGN_EPSON_UV
#define MANUFACTURER_NAME DESIGN_UV
#endif


#ifdef MANUFACTURER_RIPSTAR_TEST_UV
#define MANUFACTURER_NAME RIPSTAR_UV_TEST
#endif

#ifdef MANUFACTURER_LAILI
#define MANUFACTURER_NAME LAILI
#endif
#ifdef MANUFACTURER_LECAI_FLAT_UV_1H  //RIPSTAR_UV_TEST
#define MANUFACTURER_NAME LECAI_FLAT_UV
#endif

#if defined(MANUFACTURER_LECAI_FLAT_UV_2H) //RIPSTAR_UV_TEST
#define MANUFACTURER_NAME LECAI_FLAT_UV
#endif

#if defined(HEAD_KONICA)||defined(HEAD_XAAR128)||defined(HEAD_XAAR382)	//ALL KONICA Head
#define PRODUCT_ID			0x0100 
#define ENABLE_VOLTAGE_ADJUST
#endif	

/*#ifdef HEAD_XAAR128
#define PRODUCT_ID			0x0001 //XAAR-128, first 12-heads head-board
#endif

#ifdef HEAD_XAAR382
#define PRODUCT_ID			0x0002 //XAAR-128, first 12-heads head-board
#endif*/

#ifdef HEAD_SPECTRA_SKYWALKER
#define PRODUCT_ID			0x0200 //SPECTRA, 16 skywalker head head-board
#define ENABLE_PULSE_WIDTH_ADJUST
#define ENABLE_VOLTAGE_ADJUST	
#endif

#ifdef HEAD_SPECTRA_POLARIS
#define PRODUCT_ID			0x0300 //SPECTRA, POLARIS
#define ENABLE_PULSE_WIDTH_ADJUST
#define ENABLE_VOLTAGE_ADJUST	
#endif

#ifdef HEAD_EPSON_GEN5
#define PRODUCT_ID			0x0400 //EPSON, EPSON GEN5
#endif

#ifdef HEAD_RICOH_G4
#define PRODUCT_ID			0x0500 //Ricoh, Ricoh gen4
#endif

#ifdef VENDOR_PASSWORD_TOOL
#define PRODUCT_ID			0xFFFE
#endif

#endif	

#if (MANUFACTURER_NAME == LIYU)
#define RIGHT_ORIGIN
#endif

#define CHECK_BYHX_DATA

//====================================================================
#define MANUFACTURER_ID		MANUFACTURER_NAME
#define PHY_RESOLUTION	//Use 720 DPI

#ifdef PHY_RESOLUTION
#define POS_FAC	2
#else
#define POS_FAC	1
#endif

//#define PRINTER_DATA_WIDTH  16 //8,16,32 (1,2,4 bytes) //24, 3 bytes doesn't support now
//#define PRINTER_DATA_DEPTH	128 //64,128,256,512(no for 32 width)

#define PRINTER_POSITION_N_LIMIT (-5000*POS_FAC) //Nearest position
#define PRINTER_POSITION_F_LIMIT (530000*POS_FAC) //Farest position

//PRINTER_PLAT_SPACE must be greater than PRINTER_ACC_SPACE

#ifdef UV_PRINTER 
#ifdef MANUFACTURER_TATE_EPSON_UV
#if defined(HEAD_RICOH_G4)
#define PRINTER_PLAT_SPACE (2900*POS_FAC)  //Normall used value
#define PRINTER_ACC_SPACE  (2800*POS_FAC)
#else
#define PRINTER_PLAT_SPACE (2620*POS_FAC)  //Normall used value
#define PRINTER_ACC_SPACE  (2460*POS_FAC)
#endif
#elif defined(MANUFACTURER_ALLWIN_FLAT)||defined(MANUFACTURER_ALLWIN_EPSON_4H_UV)
#define PRINTER_PLAT_SPACE (3317*POS_FAC)  //-12cm
#define PRINTER_ACC_SPACE  (3177*POS_FAC)
#elif defined(MANUFACTURER_ALLWIN_2H_ROLL_UV)||defined(MANUFACTURER_ALLWIN_4H_ROLL_UV)
#define PRINTER_PLAT_SPACE (2600*POS_FAC)  //-12cm
#define PRINTER_ACC_SPACE  (1900*POS_FAC)
#elif defined(MANUFACTURER_DYSS)
#ifdef RICOH_3H
#define PRINTER_PLAT_SPACE (1511*POS_FAC + LCDMenuConfig_EX.X_Plat_Correction*X_BASE_RES)  //-12cm
#define PRINTER_ACC_SPACE  (1200*POS_FAC)
#else
#define PRINTER_PLAT_SPACE (1240*POS_FAC + LCDMenuConfig_EX.X_Plat_Correction*X_BASE_RES)  //-12cm
#define PRINTER_ACC_SPACE  (1000*POS_FAC)
#endif
#elif defined(MANUFACTURER_SAIBO)||defined(MANUFACTURER_SAIBO_ROLL_UV)
#define PRINTER_PLAT_SPACE (2600*POS_FAC)  //-12cm
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#elif defined(MANUFACTURER_KINCOLOR_FLAT)
#define PRINTER_PLAT_SPACE (2350*POS_FAC)  //-12cm
#define PRINTER_ACC_SPACE  (2300*POS_FAC)
#elif defined(MANUFACTURER_LONGKE_FLAT_UV_150)||defined(MANUFACTURER_LONGKE_FLAT_UV_130)
#define PRINTER_PLAT_SPACE (3600*POS_FAC)  //-5cm
#define PRINTER_ACC_SPACE  (3500*POS_FAC)
#elif defined(MANUFACTURER_LECAI_FLAT_UV_1H)
#define PRINTER_PLAT_SPACE (1965*POS_FAC)  //-10cm
#define PRINTER_ACC_SPACE  (1900*POS_FAC)
#elif defined(MANUFACTURER_LECAI_FLAT_UV_2H)

#ifdef LECAI_NEW_DCS810
#define PRINTER_PLAT_SPACE (4640*POS_FAC)  //-10cm
#define PRINTER_ACC_SPACE  (1900*POS_FAC)
#else
#define PRINTER_PLAT_SPACE (2400*POS_FAC)  //-10cm
#define PRINTER_ACC_SPACE  (1900*POS_FAC)
#endif

#elif defined(MANUFACTURER_INWEAR_FLAT)
#define PRINTER_PLAT_SPACE (2400*POS_FAC)  //-10cm
#define PRINTER_ACC_SPACE  (1900*POS_FAC)
#elif defined(MANUFACTURER_DESIGN_EPSON_UV)
#define PRINTER_PLAT_SPACE (3000*POS_FAC)  //Normall used value
#define PRINTER_ACC_SPACE  (2000*POS_FAC)
#elif defined(MANUFACTURER_ZHONGYE_FLAT_UV)||defined(MANUFACTURER_ZHONGYE_ROLL_UV)
#define PRINTER_PLAT_SPACE (1800*POS_FAC)  //Normall used value
#define PRINTER_ACC_SPACE  (1800*POS_FAC)
#elif defined(MANUFACTURER_ORIC)
#define PRINTER_PLAT_SPACE (2550*POS_FAC)  //Normall used value
#define PRINTER_ACC_SPACE  (1800*POS_FAC)
#elif defined(MANUFACTURER_ORIC_FLAT)
#define PRINTER_PLAT_SPACE (4806*POS_FAC)  //Normall used value
#define PRINTER_ACC_SPACE  (1800*POS_FAC)
#elif defined(MANUFACTURER_DYSS_16H_FLAT)
#define PRINTER_PLAT_SPACE (1800*POS_FAC)  //Normall used value
#define PRINTER_ACC_SPACE  (1800*POS_FAC)
#else
#define PRINTER_PLAT_SPACE (4300*POS_FAC)  //Normall used value
#define PRINTER_ACC_SPACE  (4200*POS_FAC)
#endif
//	#define PRINTER_PLAT_SPACE (2300*POS_FAC)  //For witcolor temp testing. 2010-08-05
//	#define PRINTER_ACC_SPACE  (2200*POS_FAC)

//	#define PRINTER_PLAT_SPACE (1360*POS_FAC)  //For INWEAR LED UV lamp
//	#define PRINTER_ACC_SPACE  (1200*POS_FAC)

//	#define PRINTER_PLAT_SPACE (5500*POS_FAC)  //For WIT-COLOR Polaris UV Testing 20100523
//	#define PRINTER_ACC_SPACE  (5400*POS_FAC)

//	#define PRINTER_PLAT_SPACE (2850*POS_FAC)  //For one Kincolor machine, special 20100114
//	#define PRINTER_ACC_SPACE  (2800*POS_FAC)

//	#define PRINTER_PLAT_SPACE (8000*POS_FAC)
//	#define PRINTER_ACC_SPACE  (7500*POS_FAC)

#define PRINTER_SAFE_SPACE	(610*POS_FAC)
#else
//	#define PRINTER_PLAT_SPACE (5100*POS_FAC)  //Only for new 512 test
//	#define PRINTER_ACC_SPACE  (5000*POS_FAC)  //Only for new 512 test
#ifdef ASBANNER
#define PRINTER_PLAT_SPACE (700*POS_FAC) //20100120
#define PRINTER_ACC_SPACE  (680*POS_FAC)
#elif defined(HEAD_EPSON_GEN5)
#if (MANUFACTURER_NAME == ALLWIN_EPSON)
#define PRINTER_PLAT_SPACE (1700*POS_FAC)
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#elif defined( MANUFACTURER_TOPJET_EPSON)
#define PRINTER_PLAT_SPACE (4020*POS_FAC)
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#elif defined(MANUFACTURER_RUICAI_EPSON)
#define PRINTER_PLAT_SPACE (2113*POS_FAC)
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#elif defined(MANUFACTURER_JHF)
#define PRINTER_PLAT_SPACE (2880*POS_FAC)   //add 2CM for JHF machine.
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#elif defined(MANUFACTURER_INKWIN)||defined(MANUFACTURER_ADDTOP_EPSON)||defined(MANUFACTURER_MARTIN)
#define PRINTER_PLAT_SPACE (2415*POS_FAC)   //sub 13mm for INKWIN machine.
#define PRINTER_ACC_SPACE  (1700*POS_FAC)				
#elif defined(MANUFACTURER_TATE_EPSON)  ||defined(MANUFACTURER_BEMAJET)||defined(MANUFACTURER_YAODI_EPSON)||defined(MANUFACTURER_TENGFENG_EPSON)||defined(MANUFACTURER_SAISHUN_WATER_EPSON)
#define PRINTER_PLAT_SPACE (2390*POS_FAC)   //sub 15mm for TATE machine.
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#elif defined(MANUFACTURER_ALLWIN_EPSON)
#define PRINTER_PLAT_SPACE (2742*POS_FAC)   //add 3CM for allwin machine.
#define PRINTER_ACC_SPACE  (1700*POS_FAC)	
#elif defined(MANUFACTURER_LECAI_EPSON_4H)
#define PRINTER_PLAT_SPACE (6100*POS_FAC)   //add 25CM for allwin machine.
#define PRINTER_ACC_SPACE  (2000*POS_FAC)	
#elif defined(MANUFACTURER_DGEN)
#define PRINTER_PLAT_SPACE (2915*POS_FAC)   //sub 13mm for INKWIN machine.
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#elif defined(HK_3M2)
#define PRINTER_PLAT_SPACE (3166*POS_FAC)   //add 50mm for HK 3M2  machine.
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#elif defined(MANUFACTURER_HUMAN_EPSON)&&defined(EPSON_4H)
#define PRINTER_PLAT_SPACE (2850*POS_FAC) 
#define PRINTER_ACC_SPACE  (2830*POS_FAC)
#else
#define PRINTER_PLAT_SPACE (2600*POS_FAC) 
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#endif	
#elif defined( HEAD_RICOH_G4)
#if (MANUFACTURER_NAME == ALLWIN)
#define PRINTER_PLAT_SPACE (1700*POS_FAC)
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#elif defined(MANUFACTURER_GZ)
#define PRINTER_PLAT_SPACE (1700*POS_FAC)   //add 3CM for allwin machine.
#define PRINTER_ACC_SPACE  (1600*POS_FAC)	
#elif defined(MANUFACTURER_TATE_EPSON)
#define PRINTER_PLAT_SPACE (2900*POS_FAC)   //add 3CM for allwin machine.
#define PRINTER_ACC_SPACE  (2800*POS_FAC)
#elif defined(MANUFACTURER_ORIC_COMMON)
#define PRINTER_PLAT_SPACE (3000*POS_FAC)  //Normall used value
#define PRINTER_ACC_SPACE  (1800*POS_FAC)
#else
#define PRINTER_PLAT_SPACE (1700*POS_FAC)
#define PRINTER_ACC_SPACE  (1700*POS_FAC)
#endif
#else
#define PRINTER_PLAT_SPACE (1360*POS_FAC)
#define PRINTER_ACC_SPACE  (1200*POS_FAC)
#endif	
#define PRINTER_SAFE_SPACE	(50*POS_FAC)
#endif


#define PRINTER_CLEAN_FAR  True

#if (MANUFACTURER_NAME == LIYU) //LIYU
#define PRINTER_AUTOCLEAN_START_POS	(PRINTER_POSITION_F_LIMIT-3200*POS_FAC)
#define PRINTER_AUTOCLEAN_END_POS PRINTER_POSITION_F_LIMIT
#elif ((MANUFACTURER_NAME == INWEAR)||(MANUFACTURER_NAME == HUMAN)||(MANUFACTURER_NAME == LOTUS)||(MANUFACTURER_NAME == COLORJET)||(MANUFACTURER_NAME == HAPOND)||(MANUFACTURER_NAME == RODINJET)||(MANUFACTURER_NAME == BRITEJET)||(MANUFACTURER_NAME == MONOTECH)||(MANUFACTURER_NAME == HUMAN_STITCH)||(MANUFACTURER_NAME == XINHANGJIDIAN_STITCH))
#define PRINTER_AUTOCLEAN_START_POS 100*POS_FAC
#define PRINTER_AUTOCLEAN_END_POS 	4170*POS_FAC
#elif ((MANUFACTURER_NAME == GZ) || (MANUFACTURER_NAME == GZ_UV))
#ifdef DCLK_12V //GZ NOVA
#define PRINTER_AUTOCLEAN_START_POS 100*POS_FAC	 //Changed by the request of GZ	
#define PRINTER_AUTOCLEAN_END_POS 	9600*POS_FAC
#else  //For GZ Polaris	
#define PRINTER_AUTOCLEAN_START_POS 2900*POS_FAC	 //Changed by the request of GZ			
#define PRINTER_AUTOCLEAN_END_POS 	8500*POS_FAC  //对工正的这个机器，printer_init中会更改这个值
#endif
#elif (MANUFACTURER_NAME == ALLWIN) || (MANUFACTURER_NAME == ALLWIN_WATER_4H) ||(MANUFACTURER_NAME == ALLWIN_WATER_2H) ||(MANUFACTURER_NAME == ALLWIN_4H)||(MANUFACTURER_NAME == ALLWIN_4H_UV) ||(MANUFACTURER_NAME == ALLWIN_EPSON) || (MANUFACTURER_NAME == DOCAN_FLAT) || (MANUFACTURER_NAME == TATE)|| (MANUFACTURER_NAME == TATE_UV)|| (MANUFACTURER_NAME == BEMAJET)|| (MANUFACTURER_NAME == SHANGHAI_YAODI)|| (MANUFACTURER_NAME == TENGFENG)|| (MANUFACTURER_NAME == SAISHUN_WATER)|| (MANUFACTURER_NAME == LONGKE_FLAT_UV)//Need adjust
#ifdef MACHINE_5M
#define PRINTER_AUTOCLEAN_START_POS 200*POS_FAC	
#define PRINTER_AUTOCLEAN_END_POS 5400*POS_FAC  //20100623 changed in ShangHai, but for released in Shanghai, the value is 100 and 5300, after release change it
#else
#define PRINTER_AUTOCLEAN_START_POS 100*POS_FAC	
#define PRINTER_AUTOCLEAN_END_POS 4170*POS_FAC
#endif	
#elif ((MANUFACTURER_NAME == MYJET) ||(MANUFACTURER_NAME == VOXINDIA))
#define PRINTER_AUTOCLEAN_START_POS 100*POS_FAC	
#define PRINTER_AUTOCLEAN_END_POS 4170*POS_FAC
#elif (MANUFACTURER_NAME == ALLWIN_FLAT)|| (MANUFACTURER_NAME == DYSS_RICOH_FLAT_UV)	
#define PRINTER_AUTOCLEAN_START_POS 1250*POS_FAC
#define PRINTER_AUTOCLEAN_END_POS 7250*POS_FAC
#elif (MANUFACTURER_NAME == KINCOLOR)|(MANUFACTURER_NAME == KINCOLOR_FLAT)|(MANUFACTURER_NAME == HUMAN_FLAT)|(MANUFACTURER_NAME == INWEAR_FLAT)|(MANUFACTURER_NAME == INWEAR_FLAT_UV)|(MANUFACTURER_NAME == MYJET_FLAT)|(MANUFACTURER_NAME == HAPOND_FLAT)|(MANUFACTURER_NAME == RODINJET_FLAT)|(MANUFACTURER_NAME == WITCOLOR)|(MANUFACTURER_NAME == WITCOLOR_UV) 
#define PRINTER_AUTOCLEAN_START_POS 0				
#define PRINTER_AUTOCLEAN_END_POS 0
#elif (MANUFACTURER_NAME == MICOLOR || MANUFACTURER_NAME == MICOLOR_EPSON ||MANUFACTURER_NAME == DESIGN||MANUFACTURER_NAME == DESIGN_UV)
#define PRINTER_AUTOCLEAN_START_POS 0				
#define PRINTER_AUTOCLEAN_END_POS 0
#elif (MANUFACTURER_NAME == TOPJET ||MANUFACTURER_NAME == ALLWIN_2H_ROLL_UV ||MANUFACTURER_NAME == MARTIN  ||MANUFACTURER_NAME == DGEN  ||MANUFACTURER_NAME == ORIC_ROLL_UV||MANUFACTURER_NAME == ORIC||MANUFACTURER_NAME == ORIC_FLAT_UV)
#define PRINTER_AUTOCLEAN_START_POS 0				
#define PRINTER_AUTOCLEAN_END_POS 0
#elif (MANUFACTURER_NAME == JHF)||(MANUFACTURER_NAME == INKWIN)||(MANUFACTURER_NAME == RIPSTAR_UV_TEST)||(MANUFACTURER_NAME == RIPSTAR_TEST)||(MANUFACTURER_NAME == FULGENCY_FLAT_UV)||(MANUFACTURER_NAME == ZHONGYE_FLAT_UV)||(MANUFACTURER_NAME == XUKE)
#define PRINTER_AUTOCLEAN_START_POS 0				
#define PRINTER_AUTOCLEAN_END_POS 0
#elif (MANUFACTURER_NAME == RASTER ||MANUFACTURER_NAME == RASTER_4H||MANUFACTURER_NAME == RUICAI ||MANUFACTURER_NAME == LECAI_FLAT_UV ||MANUFACTURER_NAME == LECAI_4H||MANUFACTURER_NAME == LAILI||MANUFACTURER_NAME == COLORFUL) 
#define PRINTER_AUTOCLEAN_START_POS 0				
#define PRINTER_AUTOCLEAN_END_POS 0
#elif (MANUFACTURER_NAME == 0xFFFF)
#define PRINTER_AUTOCLEAN_START_POS 0	
#define PRINTER_AUTOCLEAN_END_POS 0
#else
Compile_Error("No manufacturer name is defined")
#endif

//#define PRINTER_SINGLECLEAN_1_POS	PRINTER_POSITION_F_LIMIT-4500
#define PRINTER_SINGLECLEAN_SPACE	(510*POS_FAC)

#define PRINTER_MANUAL_CLEAN_POS 0	
#define  PRINTER_MOVE_SLOWEST 7
#ifdef HEAD_EPSON_GEN5
#if defined(MANUFACTURER_DOCAN_UV)||defined(MANUFACTURER_GZ)||defined (MANUFACTURER_INKWIN)
#define PRINTER_MOVE_SPEED 7
#elif defined(MANUFACTURER_JHF) ||defined(MANUFACTURER_HUMAN_STITCH) ||defined(MANUFACTURER_XINHANGJIDIAN_STITCH)
#define PRINTER_MOVE_SPEED 2
#elif defined(MANUFACTURER_KINCOLOR_FLAT)  ||defined(ALLWIN_EPSON_SAME)||defined(MANUFACTURER_SAIBO_ROLL_UV)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)
#define PRINTER_MOVE_SPEED 5
#elif defined(MANUFACTURER_LONGKE_FLAT_UV_150)||defined(MANUFACTURER_LONGKE_FLAT_UV_130)||defined(MANUFACTURER_TATE_EPSON_UV)||defined(MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_ADDTOP_EPSON)
#define PRINTER_MOVE_SPEED 6
#elif defined(MANUFACTURER_YAODI_EPSON)||defined(MANUFACTURER_TENGFENG_EPSON)||defined(MANUFACTURER_HUMAN_EPSON)||defined(MANUFACTURER_SAISHUN_WATER_EPSON) ||defined(MANUFACTURER_RIPSTAR_TEST_UV)
#define PRINTER_MOVE_SPEED 4
#else
#define PRINTER_MOVE_SPEED 5
#endif
#else
//#ifdef MANUFACTURER_DYSS
//#define PRINTER_MOVE_SPEED 7
//#else
#define PRINTER_MOVE_SPEED 5
//#endif
#endif
#if ((MANUFACTURER_NAME == GZ) || (MANUFACTURER_NAME == GZ_UV))
#define PRINTER_CLEAN_SPEED 4
#else
#define PRINTER_CLEAN_SPEED 7
#endif

#if (defined(SUPPORT_MOTOR_CONTROL)&& !defined(MOTION_Y_DSP))
#define PRINTER_MEDIAFEED_SPEED 0
#else
#if (defined(MANUFACTURER_GZ)&& defined(MOTION_Y_DSP))
#define PRINTER_MEDIAFEED_SPEED 3
#elif (defined(MANUFACTURER_MICOLOR_EPSON)&& defined(MOTION_Y_DSP))
#define PRINTER_MEDIAFEED_SPEED 3
#elif defined(MANUFACTURER_DYSS)
#define PRINTER_MEDIAFEED_SPEED 3
#else
#define PRINTER_MEDIAFEED_SPEED 3
#endif
#endif

#if (MANUFACTURER_NAME == HUMAN_FLAT)
#define PRINTER_ZAXIS_SPEED 3
#elif (MANUFACTURER_NAME == ALLWIN_FLAT)
#define PRINTER_ZAXIS_SPEED 6
#elif (MANUFACTURER_NAME == DYSS_RICOH_FLAT_UV)
#define PRINTER_ZAXIS_SPEED 6
#elif defined(MANUFACTURER_LECAI_FLAT_UV_1H)||defined(MANUFACTURER_LECAI_FLAT_UV_2H)
#define PRINTER_ZAXIS_SPEED 2
#else
#define PRINTER_ZAXIS_SPEED 6
#endif

#if defined(MANUFACTURER_LONGKE_FLAT_UV_150)||defined(MANUFACTURER_LONGKE_FLAT_UV_130)
#define PRINTER_ZAXIS_SPEED_FLAT 2
#endif

#define PRINTER_FLASH_NEAR_POS (-200*POS_FAC)

//#define PRINTER_FLASH_FAR_POS (2200*POS_FAC)
//#define PRINTER_FLASH_FAR_POS (100*POS_FAC) //ALL暂停后自动清洗会导致闪喷到台面上，改动此位置，不知是否会影响其他机器
#define PRINTER_FLASH_FAR_POS (1000*POS_FAC) //ALL暂停后自动清洗会导致闪喷到台面上，改动此位置，不知是否会影响其他机器

#define PRINTER_X_ORIGIN_PERMITTED_ERROR (10*POS_FAC)

//#define DATA_BYTECNTPERPIX (PRINTER_DATA_WIDTH*PRINTER_DATA_DEPTH/8) 

//#define HEADS_NUMBER		12
#define MAX_HEADS_CLEAN_NUMBER	6

#define MAX_PAPER_VOLUME 3
//One pix data size = DATA_WIDTH*DATA_DEPTH: 
//64, 128, 256, 512, 1024, 2048

typedef struct STRUCT_PRINTER
{
	//	INT8U data_width; //6 color Xaar128, 2 group: 16
	//	INT16U data_depth; //6 color Xaar128, 2 group: 128
	INT32S orgLimit; //The smallest movement position limitation, maybe less than 0
	INT32S org_oppLimit; //The largest movement position limitation
	INT32S platSpace; //The space between the papaer platform and the origin, use it to translate the logic system
	// to physical system. When host get	the origin point, sub this value from the physical position
	
	INT32S accSpace; //Acc distance of motor
	INT8U cleanPosOrgOpp; //清洗区域是否位于原点的相对方向. True: 位于相对方向, False:位于原点方向
	
	INT32S autoCleanStartPos; //StartPos <= EndPos
	INT32S autoCleanEndPos;
	INT32S headsCleanPos[MAX_HEADS_CLEAN_NUMBER];
	INT32S manualCleanPos;
	
	INT32S flashNearPos; //定义闪喷区域, 只在此区域内闪喷
	INT32S flashFarPos;
	
	INT8S  movSpeed;	
	INT8S  cleanSpeed;
	INT8S  feedSpeed;
	INT8S  xEncoder; ////!0 使用光栅， 0 使用司服编码   
}PrinterParaType;

//for epson allwin, only autoClean_passInterval is valid.
//for epson micolor, only longflash_passInterval is valid. 
typedef __packed struct CLEANPARA
{
	INT8U structsize;
	INT8U flash;    // 1: 是否自动闪喷，0/1=否/是
	INT8U pause_gohome; // 2: 暂停后回原点,0/1=否/是
	INT16U flash_interval;  // 3-4: 闪喷间隔，以毫秒为单位
	INT8U longflash_beforeprint;  // 5: 打印前回原点后是否猛喷一下
	INT8U autoClean_passInterval;     // 6: 每多少个 Pass 自动清洗一次喷头
	INT8U idleClean_intervalTime; 	//空闲清洗的时间间隔(小时)
	INT8U autoCleanTimes;     // 7: 每次清洗喷头时清洗几下，最小值2
	INT8U manualCleanTimes;  //8: 手动清洗次数 
	INT8U longflash_passInterval;     // 9: 每多少个 Pass 自动猛喷一次
	INT8U blowInk_passInterval;    // 10: 每多少个 Pass 自动压墨一次
	INT16U flashTimes;             //11,12 闪喷的次数；B 系统用这个, A不用
	
	INT16U pauseIntervalAfterClean;	   //13, 14清洗后小车停顿时间
	INT16U pauseIntervalAfterBlowInk;  //15, 16猛喷的时间
	INT8U paraFlag;  //17: 0 means the follwoing 2 parameters will use FW setting	
#if defined (EPSON_CLEAN_UPDOWN) || defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
	INT8U autoClean_way; //0~5:default/customized/strong/normal/weak/refill.  only for AllWIn epson
	INT8U reserve[64 - 19];   // 19-63: 保留
#else    
	INT8U reserve[64 - 18];   // 19-63: 保留
#endif
}CleanParaType;

#include "clean.h"

void Printer_Init(void);

#endif

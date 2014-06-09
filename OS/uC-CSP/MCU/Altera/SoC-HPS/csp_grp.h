/*
*********************************************************************************************************
*                                             uC/CSP
*                                        Chip Support Package
*
*                            (c) Copyright 2010; Micrium, Inc.; Weston, FL
*                         (c) Copyright 2003-2010; Micrium, Inc.; Weston, FL
*
*               All rights reserved. Protected by international copyright laws.
*
*               uC/CSP is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*
*                                       FAMILY DEFINITION & API
*
*                                          Altera Dev-5XS1
*
* Filename      : csp_grp.h
* Version       : V1.00
* Programmer(s) : NB
*********************************************************************************************************
* Note(s)       : 
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifndef  CSP_GRP_MODULE_PRESENT
#define  CSP_GRP_MODULE_PRESENT


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   CSP_GRP_MODULE
#define  CSP_GRP_EXT
#else
#define  CSP_GRP_EXT  extern
#endif


/*
*********************************************************************************************************
*                                        DEFAULT CONFIGURATION
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                         FAMILY IDENTIFIERS
*********************************************************************************************************
*/

#define  CSP_GRP_ID_DEV5XS1
#define  CSP_GRP_NAME                     "DEV5XS1"

/*
*********************************************************************************************************
*                                          REGISTER DEFINES
*********************************************************************************************************
*/

#define  SOCFPGA5XS1_CSP_RST_MNGR_BASE                       (CPU_INT32U )(0xFFD05000)

#define  SOCFPGA5XS1_CSP_RST_MNGR_MPU_MODULE_RST             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_RST_MNGR_BASE + 0x10))
#define  SOCFPGA5XS1_CSP_RST_MNGR_PERIPH_MODULE_RST          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_RST_MNGR_BASE + 0x14))
#define  SOCFPGA5XS1_CSP_RST_MNGR_PERIPH_BRGMDRST            (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_RST_MNGR_BASE + 0x1C))

#define  SOCFPGA5XS1_CSP_CLK_MNGR_BASE                       (CPU_INT32U)(0xFFD04000)
#define  SOCFPGA5XS1_CSP_CLK_MNGR_CTRL                       (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x00))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_BYPASS                     (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x04))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_INTER                      (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x08))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_INTREN                     (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x0C))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_DBCTRL                     (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x10))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_STAT                       (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x14))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_VCO                   (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x40))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_MISC                  (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x44))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_MPUCLK                (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x48))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_MAINCLK               (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x4C))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_DBGATCLK              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x50))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_MAINQSPICLK           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x54))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_MAINANDSDMMCCLK       (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x58))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_CFGS2FUSER0CLK        (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x5C))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_EN                    (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x60))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_MAINDIV               (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x64))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_DBGDIV                (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x68))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_TRACEDIV              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x6C))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_L4SRC                 (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x70))
#define  SOCFPGA5XS1_CSP_CLK_MNGR_MAIN_STAT                  (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_CLK_MNGR_BASE + 0x74))

#define  SOCFPGA5XS1_CSP_SYS_MNGR_BASE                       (CPU_INT32U)(0xFFD08000)

#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_VIOCTRL_0          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_BASE + 0x40))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_VIOCTRL_1          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_BASE + 0x44))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_VIOCTRL_2          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_BASE + 0x48))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_HIOCTRL            (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_BASE + 0x50))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_SRC                (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_BASE + 0x54))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_HWCTRL             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_BASE + 0x58))

#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE              (CPU_INT32U)(SOCFPGA5XS1_CSP_SYS_MNGR_BASE + 0x400)
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO0          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x50))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO1          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x54))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO2          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x58))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO3          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x5C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO4          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x60))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO5          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x64))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO6          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x68))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO7          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x6C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO8          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x70))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO9          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x74))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO10         (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x78))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO11         (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x7C))

#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX0           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1D4))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX1           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1D8))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX2           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1DC))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX3           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1E0))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX4           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1E4))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX5           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1E8))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX6           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1EC))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX7           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1F0))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX8           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1F4))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX9           (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1F8))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX10          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x1FC))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX11          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x200))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX12          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x204))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX13          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x208))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX14          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x20C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX15          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x210))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX16          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x214))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX17          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x218))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX18          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x21C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX19          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x220))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX20          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x224))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX21          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x228))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX22          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x22C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX23          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x230))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX24          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x234))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX25          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x238))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX26          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x23C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX27          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x240))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX28          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x244))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX29          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x248))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX30          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x24C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX31          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x250))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX32          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x254))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX33          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x258))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX34          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x25C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX35          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x260))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX36          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x264))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX37          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x268))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX38          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x26C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX39          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x270))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX40          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x274))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX41          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x278))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX42          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x27C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX43          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x280))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX44          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x284))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX45          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x288))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX46          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x28C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX47          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x290))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX48          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x294))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX49          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x298))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX50          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x29C))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX51          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2A0))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX52          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2A4))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX53          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2A8))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX54          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2AC))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX55          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2B0))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX56          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2B4))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX57          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2B8))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX58          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2BC))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX59          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2C0))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX60          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2C4))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX61          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2C8))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX62          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2CC))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX63          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2D0))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX64          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2D4))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX65          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2D8))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX66          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2DC))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX67          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2E0))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX68          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2E4))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX69          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2E8))
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX70          (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_BASE + 0x2EC))

#define  SOCFPGA5XS1_CSP_SCAN_MNGR_BASE                       (CPU_INT32U)(0xFFF02000)
#define  SOCFPGA5XS1_CSP_SCAN_MNGR_STAT                       (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SCAN_MNGR_BASE + 0x00))
#define  SOCFPGA5XS1_CSP_SCAN_MNGR_EN                         (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SCAN_MNGR_BASE + 0x04))
#define  SOCFPGA5XS1_CSP_SCAN_MNGR_FIFOSINGLEBYTE             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SCAN_MNGR_BASE + 0x10))
#define  SOCFPGA5XS1_CSP_SCAN_MNGR_FIFODOUBLEBYTE             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SCAN_MNGR_BASE + 0x14))
#define  SOCFPGA5XS1_CSP_SCAN_MNGR_FIFOTRIPLEBYTE             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SCAN_MNGR_BASE + 0x18))
#define  SOCFPGA5XS1_CSP_SCAN_MNGR_FIFOQUADBYTE               (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_SCAN_MNGR_BASE + 0x1C))

#define  SOCFPGA5XS1_CSP_GPIO0_BASE                           (CPU_INT32U)(0xFF708000)
#define  SOCFPGA5XS1_CSP_GPIO0_SWPORTA_DR                     (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_GPIO0_BASE + 0x0))
#define  SOCFPGA5XS1_CSP_GPIO0_SWPORTA_DDR                    (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_GPIO0_BASE + 0x4))

#define  SOCFPGA5XS1_CSP_GPIO1_BASE                           (CPU_INT32U)(0xFF709000)
#define  SOCFPGA5XS1_CSP_GPIO1_SWPORTA_DR                     (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_GPIO1_BASE + 0x0))
#define  SOCFPGA5XS1_CSP_GPIO1_SWPORTA_DDR                    (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_GPIO1_BASE + 0x4))

#define  SOCFPGA5XS1_CSP_GPIO2_BASE                           (CPU_INT32U)(0xFF70A000)
#define  SOCFPGA5XS1_CSP_GPIO2_SWPORTA_DR                     (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_GPIO2_BASE + 0x0))
#define  SOCFPGA5XS1_CSP_GPIO2_SWPORTA_DDR                    (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_GPIO2_BASE + 0x4))

#define  SOCFPGA5XS1_CSP_L3_REGS_BASE                         (CPU_INT32U)(0xFF800000)
#define  SOCFPGA5XS1_CSP_L3_REGS_REMAP                        (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_L3_REGS_BASE + 0x0))

#define  SOCFPGA5XS1_CSP_PVT_TMR_BASE                         (CPU_INT32U)(0xFFFEC600)

#define  SOCFPGA5XS1_CSP_PVT_TMR_LOAD                         (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_PVT_TMR_BASE + 0x00))
#define  SOCFPGA5XS1_CSP_PVT_TMR_COUNTER                      (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_PVT_TMR_BASE + 0x04))
#define  SOCFPGA5XS1_CSP_PVT_TMR_CONTROL                      (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_PVT_TMR_BASE + 0x08))
#define  SOCFPGA5XS1_CSP_PVT_TMR_INT_STATUS                   (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_PVT_TMR_BASE + 0x0C))

/*
*********************************************************************************************************
*                                        REGISTER BIT DEFINES
*********************************************************************************************************
*/

#define  SOCFPGA5XS1_CSP_L3_REMAP_MPUZERO                     0x00000001

#define  SOCFPGA5XS1_CSP_CLK_MNGR_CTRL_SAFEMODE               0x00000001
#define  SOCFPGA5XS1_CSP_CLK_MNGR_BYPASS_MAINPLL              0x00000001
#define  SOCFPGA5XS1_CSP_CLK_MNGR_DBCTRL_STAYOSC1             0x00000001
#define  SOCFPGA5XS1_CSP_CLK_MNGR_STAT_BUSY                   0x00000001

#define  SOCFPGA5XS1_CSP_RST_MNGR_PERIPH_MODULE_RST_GPIO0     0x02000000
#define  SOCFPGA5XS1_CSP_RST_MNGR_PERIPH_MODULE_RST_GPIO1     0x04000000
#define  SOCFPGA5XS1_CSP_RST_MNGR_PERIPH_MODULE_RST_GPIO2     0x08000000
#define  SOCFPGA5XS1_CSP_RST_MNGR_PERIPH_MODULE_RST_ALL       0x3FFFFFFF

#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_VIOCTRL_1_CFG       0x00000001
#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_VIOCTRL_1_BUSHOLD   0x00000002
#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_VIOCTRL_1_TRISTATE  0x00000004
#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_VIOCTRL_1_WKPULLUP  0x00000008
#define  SOCFPGA5XS1_CSP_SYS_MNGR_FRZCTRL_VIOCTRL_1_SLEW      0x00000010

#define  SOCFPGA5XS1_CSP_SCAN_MNGR_EN_IOSCANCHAIN1            0x00000002
#define  SOCFPGA5XS1_CSP_SCAN_MNGR_EN_ACTIVE                  0x80000000
#define  SOCFPGA5XS1_CSP_SCAN_MNGR_EN_WFIFOCNT                0x70000000

#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_FLASHIO_SEL       0x00000003
#define  SOCFPGA5XS1_CSP_SYS_MNGR_PINMUXGRP_GPLMUX_SEL        0x00000001


/*
*********************************************************************************************************
*                                 INTERRUPT CONTROLLER DEVICE NUMBERS
*********************************************************************************************************
*/

#define  CSP_INT_CTRL_NBR_MAIN                  (CSP_DEV_NBR)(0u)


/*
*********************************************************************************************************
*                                          INTERRUPT SOURCES
*
* Note(s) : (1) Interrupt controller sources #define's use the following standard:
*
*               #define  CSP_INT_SRC_NBR_<PER>_<EVENT>_<PER_NBR>
*
*               where 
*
*                   <PER>      Peripheral name (UART, ETHER, DMA, USB, etc)
*                   <PER_NBR>  Peripheral number. (00, 01, 02, etc)
*                   <EVENT>    Interrupt event (RX, TX, MATCH, ALERT, TO, etc)
*                              Empty for multiple events interrupts.
*
*              #define  CSP_INT_SRC_NBR_UART_RX_00    UART 00 receive interrupt.
*              #define  CSP_INT_SRC_NBR_UART_TX_00    UART 01 receive interrupt. 
*
*            (2) 'CSP_INT_SRC_NBR_MAX' defines maximum number of interrupts in the interrupt controller.
*                   
*            (3) A value of 32 is added to each IRQ source number in the source table for the global 
*                interrupt controller.
*********************************************************************************************************
*/
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_PVT_TMR					   (CSP_DEV_NBR)( 29)         /* private timer										   */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_CPU0_PARITY                   (CSP_DEV_NBR)( 32u)        /* CPU0 parity.                                          */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_CPU1_PARITY                   (CSP_DEV_NBR)( 48u)        /* CPU1 parity.                                          */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_SCU0_PARITY                   (CSP_DEV_NBR)( 64u)        /* SCU0 parity.                                          */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_SCU1_PARITY                   (CSP_DEV_NBR)( 65u)        /* SCU1 parity.                                          */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_L2_ECC_WRITE                  (CSP_DEV_NBR)( 67u)        /* L2 ECC write.                                         */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_L2_ECC_ERR                    (CSP_DEV_NBR)( 68u)        /* L2 ECC error corrected.                               */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_L2_ECC_UNCR_ERR               (CSP_DEV_NBR)( 69u)        /* L2 ECC error uncorrected.                             */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_L2                            (CSP_DEV_NBR)( 70u)        /* L2 combined interrupts.                               */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DDR_ECC_ERR                   (CSP_DEV_NBR)( 71u)        /* DDR ECC error.                                        */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DMA0                          (CSP_DEV_NBR)(136u)        /* DMA Channel 0.                                        */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DMA1                          (CSP_DEV_NBR)(137u)        /* DMA Channel 1.                                        */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DMA2                          (CSP_DEV_NBR)(138u)        /* DMA Channel 2.                                        */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DMA3                          (CSP_DEV_NBR)(139u)        /* DMA Channel 3.                                        */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DMA4                          (CSP_DEV_NBR)(140u)        /* DMA Channel 4.                                        */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DMA5                          (CSP_DEV_NBR)(141u)        /* DMA Channel 5.                                        */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DMA6                          (CSP_DEV_NBR)(142u)        /* DMA Channel 6.                                        */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DMA7                          (CSP_DEV_NBR)(143u)        /* DMA Channel 7.                                        */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_DMA_ABORT                     (CSP_DEV_NBR)(144u)        /* DMA abort.                                            */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_EMAC0                         (CSP_DEV_NBR)(147u)        /* Gb-Ethernet MAC0.                                     */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_EMAC1                         (CSP_DEV_NBR)(152u)        /* Gb-Ethernet MAC1.                                     */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_USB0                          (CSP_DEV_NBR)(157u)        /* USB0 generic.                                         */                                      */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_USB1                          (CSP_DEV_NBR)(160u)        /* USB1 generic.                                         */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_CAN0_STS                      (CSP_DEV_NBR)(163u)        /* CAN0 interrupt 0.                                     */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_CAN0_MO                       (CSP_DEV_NBR)(164u)        /* CAN0 interrupt 1.                                     */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_CAN1_STS                      (CSP_DEV_NBR)(167u)        /* CAN1 interrupt 0.                                     */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_CAN1_MO                       (CSP_DEV_NBR)(168u)        /* CAN1 interrupt 1.                                     */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_SDMMC                         (CSP_DEV_NBR)(171u)        /* SD/MMC.                                               */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_NAND                          (CSP_DEV_NBR)(176u)        /* NAND.                                                 */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_QSPI                          (CSP_DEV_NBR)(183u)        /* Quad-SPI.                                             */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_SPI0                          (CSP_DEV_NBR)(186u)        /* SPI 0.                                                */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_SPI1                          (CSP_DEV_NBR)(187u)        /* SPI 1.                                                */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_SPI2                          (CSP_DEV_NBR)(188u)        /* SPI 2.                                                */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_SPI3                          (CSP_DEV_NBR)(189u)        /* SPI 3.                                                */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_I2C0                          (CSP_DEV_NBR)(190u)        /* I2C 0.                                                */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_I2C1                          (CSP_DEV_NBR)(191u)        /* I2C 1.                                                */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_I2C2                          (CSP_DEV_NBR)(192u)        /* I2C 2.                                                */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_I2C3                          (CSP_DEV_NBR)(193u)        /* I2C 3.                                                */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_UART0                         (CSP_DEV_NBR)(194u)        /* UART 0.                                               */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_UART1                         (CSP_DEV_NBR)(195u)        /* UART 1.                                               */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_GPIO0                         (CSP_DEV_NBR)(196u)        /* GPIO 0.                                               */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_GPIO1                         (CSP_DEV_NBR)(197u)        /* GPIO 1.                                               */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_GPIO2                         (CSP_DEV_NBR)(198u)        /* GPIO 2.                                               */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_L4_TIMER0                     (CSP_DEV_NBR)(199u)        /* L4 timer 0.                                           */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_L4_TIMER1                     (CSP_DEV_NBR)(200u)        /* L4 timer 1.                                           */                                          */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_WD0                           (CSP_DEV_NBR)(203u)        /* Watchdog timer 0.                                     */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_WD1                           (CSP_DEV_NBR)(204u)        /* Watchdog timer 1.                                     */
#define  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_FPGA_MAN                      (CSP_DEV_NBR)(148u)        /* FPGA manager.                                         */

#define  CSP_INT_SRC_NBR_MAX                                           (CSP_DEV_NBR)(256u)        /* Maximum number of interrupts.                         */


/*
*********************************************************************************************************
*                                          INTERRUPT PRIORITIES
*********************************************************************************************************
*/

#define  CSP_INT_SRC_PRIO_HIGHEST                  (CSP_OPT)(16u)
#define  CSP_INT_SRC_PRIO_LOWEST                   (CSP_OPT)(CSP_INT_SRC_NBR_MAX - 1u)


/*
*********************************************************************************************************
*                                     PERIPHERALS CLOCKS DEFINES
*
* Note(s) : (1) The peripheral's clock #define's use the following standard.
*        
*                   #define  CSP_PM_PER_CLK_NBR_<PER>_<PER_NBR>
*
*                   where,
*
*                   <PER>      Peripheral clock name.
*                   <PER_NBR>  Peripheral clock number identifier.
*
*********************************************************************************************************
*/

                                                                                 /* --------------- PERIPHERAL CLOCK DEFINES ------------ */  
#define  CSP_PM_PER_CLK_NBR_EMAC0                     (CSP_DEV_NBR)( 0u)
#define  CSP_PM_PER_CLK_NBR_EMAC1                     (CSP_DEV_NBR)( 1u)
#define  CSP_PM_PER_CLK_NBR_USB0                      (CSP_DEV_NBR)( 2u)
#define  CSP_PM_PER_CLK_NBR_USB1                      (CSP_DEV_NBR)( 3u)
#define  CSP_PM_PER_CLK_NBR_L4_MP                     (CSP_DEV_NBR)( 4u)
#define  CSP_PM_PER_CLK_NBR_L4_SP                     (CSP_DEV_NBR)( 5u)
#define  CSP_PM_PER_CLK_NBR_SCAN_MANAGER              (CSP_DEV_NBR)( 6u)
#define  CSP_PM_PER_CLK_NBR_SPI_M                     (CSP_DEV_NBR)( 7u)
#define  CSP_PM_PER_CLK_NBR_CAN0                      (CSP_DEV_NBR)( 8u)
#define  CSP_PM_PER_CLK_NBR_CAN1                      (CSP_DEV_NBR)( 9u)
#define  CSP_PM_PER_CLK_NBR_GPIO                      (CSP_DEV_NBR)(10u)
#define  CSP_PM_PER_CLK_NBR_S2F_USER1                 (CSP_DEV_NBR)(11u)
#define  CSP_PM_PER_CLK_NBR_SDMMC                     (CSP_DEV_NBR)(12u)
#define  CSP_PM_PER_CLK_NBR_NAND                      (CSP_DEV_NBR)(13u)
#define  CSP_PM_PER_CLK_NBR_QSPI                      (CSP_DEV_NBR)(14u)

#define  CSP_PM_PER_CLK_NBR_MAX                       (CSP_DEV_NBR)(15u)         /* Maximum number of peripherals clocks.                 */



/*
*********************************************************************************************************
*                                        SYSTEM CLOCKS DEFINES
*
* Note(s) : (1) The system clock #define's use the following standard.
*        
*                   #define  CSP_PM_SYS_CLK_NBR_<SYS>_<SYS_NBR>
*
*                   where,
*
*                   <SYS>      System clock number name.
*                   <SYS_NBR>  System clock number identifier.
*
*********************************************************************************************************
*/

#define  CSP_PM_SYS_CLK_MPU_PERIPH                    (CSP_DEV_NBR)(0u)        
#define  CSP_PM_SYS_CLK_MPU_L2_RAM                    (CSP_DEV_NBR)(1u)
#define  CSP_PM_SYS_CLK_MPU                           (CSP_DEV_NBR)(2u)        
#define  CSP_PM_SYS_CLK_L4_MAIN                       (CSP_DEV_NBR)(3u)
#define  CSP_PM_SYS_CLK_L3_MAIN                       (CSP_DEV_NBR)(4u)
#define  CSP_PM_SYS_CLK_L3_MP                         (CSP_DEV_NBR)(5u)
#define  CSP_PM_SYS_CLK_L3_SP                         (CSP_DEV_NBR)(6u)
#define  CSP_PM_SYS_CLK_DBG_AT                        (CSP_DEV_NBR)(7u)
#define  CSP_PM_SYS_CLK_DBG                           (CSP_DEV_NBR)(8u)
#define  CSP_PM_SYS_CLK_DBG_TRACE                     (CSP_DEV_NBR)(9u)
#define  CSP_PM_SYS_CLK_DBG_TIMER                     (CSP_DEV_NBR)(10u)
#define  CSP_PM_SYS_CLK_CFG                           (CSP_DEV_NBR)(11u)
#define  CSP_PM_SYS_CLK_USER0                         (CSP_DEV_NBR)(12u)

#define  CSP_PM_SYS_CLK_NBR_MAX                       (CSP_DEV_NBR)(12u)



/*
*********************************************************************************************************
*                                        TIMER VALUE DATA TYPE
*********************************************************************************************************
*/

typedef  CPU_INT32U  CSP_TMR_VAL;                               /* Timers are 32-bits wide.                             */


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of CSP_GRP module include.                      */


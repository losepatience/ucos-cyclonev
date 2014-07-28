/*
 *******************************************************************************
 *                                   EXAMPLE CODE
 *
 *                  (c) Copyright 2011; Micrium, Inc.; Weston, FL
 *
 *    All rights reserved.  Protected by international copyright laws.
 *    Knowledge of the source code may NOT be used to develop a similar product.
 *    Please help us continue to provide the Embedded community with the finest
 *    software available.  Your honesty is greatly appreciated.
 *******************************************************************************
 */

/*
 *******************************************************************************
 *
 *                  APPLICATION CONFIGURATION
 *
 *     Altera Virtual DEV-5XS1 Cortex-A9 MPCore DEVELOPMENT KIT
 *
 * Filename      : app_cfg.h
 * Version       : V1.00
 * Programmer(s) : NB
 *******************************************************************************
 */

#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT

#include <cpu.h>

/*
 *******************************************************************************
 *                                            TASK PRIORITIES
 *******************************************************************************
 */

#define  OS_TASK_TMR_PRIO                        (OS_LOWEST_PRIO - 2)


/*
 *******************************************************************************
 *                                            TASK STACK SIZES
 *******************************************************************************
 */

//#define  APP_CFG_TASK_START_STK_SIZE                     u


/*
 *******************************************************************************
 *                                             uC/LIB CONFIGURATION
 *******************************************************************************
 */

#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN             DEF_DISABLED
#define  LIB_MEM_CFG_ARG_CHK_EXT_EN               DEF_ENABLED
#define  LIB_MEM_CFG_ALLOC_EN                     DEF_ENABLED
#define  LIB_MEM_CFG_HEAP_SIZE                          1024u

#endif

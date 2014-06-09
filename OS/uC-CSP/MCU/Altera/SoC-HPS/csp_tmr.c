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
*                                          TIMER MANAGEMENT
*
*                                          Altera Dev-5XS1
* 
* Filename      : csp_tmr.h
* Version       : V1.00
* Programmer(s) : NB
*********************************************************************************************************
* Note(s)       :
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define  CSP_TMR_MODULE
#include <csp.h>


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL MACRO's
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            CSP_TmrCfg()
*
* Description : Configure periodic or free running timer. 
*                   
* Argument(s) : tmr_nbr    Timer number identifier (see note #1).
*
*               freq       Periodic timer frequency. (see note #2)
*                          
* Return(s)   : DEF_OK,   If the timer was configured correctly,
 *              DEF_FAIL, otherwise.
*
* Caller(s)   : OS_CSP_TickInit()
*********************************************************************************************************
*/

CPU_BOOLEAN  CSP_TmrCfg (CSP_DEV_NBR  tmr_nbr,
                         CPU_INT32U   freq)
{
	CPU_INT32U  load_val;
    CPU_SR_ALLOC();  
  
#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (tmr_nbr > CSP_TMR_NBR_01) {
	    return (DEF_FAIL);  
	}
	
	if ((tmr_nbr == CSP_TMR_NBR_00) && (freq == 0u)) {
	    return (DEF_FAIL);
	}
#endif
	
	if (0) {//((SOCFPGA5XS1_CSP_CLK_MNGR_BYPASS & 0x00000001) == 0) {  /* Current implementation assumes main PLL is bypassed  */
		return (DEF_FAIL);
	}

    load_val = ((BSP_OSC_FREQ / 4) / freq) - 1u;
	
	switch (tmr_nbr) {
	    case CSP_TMR_NBR_00:
		    CPU_CRITICAL_ENTER();
			
	        SOCFPGA5XS1_CSP_PVT_TMR_CONTROL    = 0;             /* Disable timer                                        */
			                                                    /* Clear interrupt flag                                 */
			SOCFPGA5XS1_CSP_PVT_TMR_INT_STATUS = DEF_BIT_00;
			
			SOCFPGA5XS1_CSP_PVT_TMR_LOAD       = load_val;      /* Set the value to reload the counter register with.   */
                                                                /* Enable Timer IRQ, and Auto Reload.                   */
			DEF_BIT_SET_32(SOCFPGA5XS1_CSP_PVT_TMR_CONTROL, (DEF_BIT_01 | DEF_BIT_02));
			
			CPU_CRITICAL_EXIT();
            break;
			
	    case CSP_TMR_NBR_01:
	    default:
		    return (DEF_FAIL);  
	}
	
    return (DEF_OK);
}



/*
*********************************************************************************************************
*                                            CSP_TmrOutCmpCfg()
*
* Description : Configure a timer for compare mode. 
*                   
* Argument(s) : tmr_nbr    Timer number identifier (see 'CSP_TmrCfg' note #1).
*
*               pin_nbr    Pin number.
*
*               pin_action  Output compare pin action
*
*                              CSP_TMR_OPT_PIN_OUT_NONE      Do nothing.
*                              CSP_TMR_OPT_PIN_OUT_CLR       Clear  the correspoding  external pin for output compare.
*                              CSP_TMR_OPT_PIN_OUT_SET       Set    the correspondin  external pin for output compare.                                                       
*                              CSP_TMR_OPT_PIN_OUT_TOGGLE    Toggle the corresponding external pin for output compare.
*
* Return(s)   : DEF_OK,   If the timer was configured correctly in compare mode,
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  CSP_TmrOutCmpCfg (CSP_DEV_NBR  tmr_nbr,
                               CSP_DEV_NBR  pin,
                               CSP_OPT      pin_action,
                               CPU_INT32U   freq)
{
    (void)tmr_nbr;
    (void)pin;
    (void)pin_action;
    (void)freq;
        
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                           CSP_TmrIntClr()
*
* Description : Clear a periodic timer interrupt.
*                   
* Argument(s) : tmr_nbr    Timer number identifier (see 'CSP_TmrCfg' note #1).
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_TmrIntClr (CSP_DEV_NBR tmr_nbr)
{
#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (tmr_nbr > CSP_TMR_NBR_01) {
	    return;  
	}
#endif
	
	switch (tmr_nbr) {
	    case CSP_TMR_NBR_00:
 	        SOCFPGA5XS1_CSP_PVT_TMR_INT_STATUS = DEF_BIT_00;
			break;
			
	    case CSP_TMR_NBR_01:
	    default:    
		    return;  
	}
}


/*
*********************************************************************************************************
*                                            CSP_TmrRst()
*
* Description : Resets a timer.
*                   
* Argument(s) : tmr_nbr    tmr_nbr    Timer number identifier (see 'CSP_TmrCfg' note #1).
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_TmrRst (CSP_DEV_NBR  tmr_nbr)
{
#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)                         /* Do not modify the systick timer once initialized.      */
    if (tmr_nbr == CSP_TMR_NBR_00) {
	    return;  
	}
#endif
}


/*
*********************************************************************************************************
*                                             CSP_TmrRd()
*
* Description : Read the current value of a timer.
*                   
* Argument(s) : tmr_nbr    tmr_nbr    Timer number identifier (see 'CSP_TmrCfg' note #1).
*
* Return(s)   : The current value of the timer.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CSP_TMR_VAL  CSP_TmrRd  (CSP_DEV_NBR  tmr_nbr)
{
#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)                         /* Do not modify the systick timer once started.          */
    if (tmr_nbr == CSP_TMR_NBR_00) {
	    return (DEF_FAIL);  
	}
#endif

    return (CSP_TMR_VAL)0;
}


/*
*********************************************************************************************************
*                                     CSP_TmrStart()
*
* Description : Start a timer.
*                   
* Argument(s) : tmr_nbr    tmr_nbr    Timer number identifier (see 'CSP_TmrCfg' note #1).
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_TmrStart (CSP_DEV_NBR  tmr_nbr)
{
#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (tmr_nbr > CSP_TMR_NBR_01) {
	    return;  
	}
#endif

	switch (tmr_nbr) {
	    case CSP_TMR_NBR_00:
	        DEF_BIT_SET_32(SOCFPGA5XS1_CSP_PVT_TMR_CONTROL, DEF_BIT_00);
			break;
			
	    case CSP_TMR_NBR_01:
	    default:
		    return;
	}
}


/*
*********************************************************************************************************
*                                            CSP_TmrStop()
*
* Description : Stop a timer.
*                   
* Argument(s) : tmr_nbr    tmr_nbr    Timer number identifier (see 'CSP_TmrCfg' note #1).
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_TmrStop  (CSP_DEV_NBR  tmr_nbr)
{
#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)                         /* Do not modify the systick timer once started.          */
    if (tmr_nbr == CSP_TMR_NBR_00) {
	    return;  
	}
#endif
}


/*
*********************************************************************************************************
*                                             CSP_TmrWr()
*
* Description : Write a value to a timer.
*                   
* Argument(s) : tmr_nbr    tmr_nbr    Timer number identifier (see 'CSP_TmrCfg' note #1).
*
*               tmr_val    value to write.
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_TmrWr  (CSP_DEV_NBR  tmr_nbr,
                  CSP_TMR_VAL  tmr_val)
{
#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)                         /* Do not modify the systick timer once started.          */
    if (tmr_nbr == CSP_TMR_NBR_00) {
	    return;  
	}
#endif
}

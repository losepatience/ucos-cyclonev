/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                The Real-Time Kernel
*
*
*                                      (c) Copyright 2009, Micrium, Weston, FL
*                                                 All Rights Reserved
*
*                                            ARM Cortex A9 OS Chip Support
*                                            
*
* File      : os_csp.h
* Version   : V2.92
* By        : NB
************************************************************************************************************************
*/

#define  OS_CSP_MODULE
#include <ucos_ii.h>
#include <csp.h>
#include <os_csp.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


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
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  OS_CSP_TickISR_Handler (void *p_arg);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/



/*$PAGE*/
/*
************************************************************************************************************************
*                                                INITIALIZE TICKER INTERRUPT
*
* Description: Initialize the the tick interrupt.
*
* Arguments  : none.
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OS_CSP_TickInit (void)
{
    CPU_BOOLEAN  status;
    
    
    status = CSP_TmrCfg((CSP_DEV_NBR)CSP_TMR_NBR_00,
                        (CPU_INT32U )OS_TICKS_PER_SEC);

    if (status != DEF_OK) {
        return;
    }
    
    status = CSP_IntVectReg((CSP_DEV_NBR    )CSP_INT_CTRL_NBR_MAIN,
                            (CSP_DEV_NBR    )IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_PVT_TMR,
                            (CPU_FNCT_PTR   )OS_CSP_TickISR_Handler,
                            (void          *)0);
    if (status != DEF_OK) {
        return;
    }

    CSP_TmrStart(CSP_TMR_NBR_00);
    
    CSP_IntEn(CSP_INT_CTRL_NBR_MAIN,
    		  IRQ_SOCFPGA5XS1_CSP_INT_SRC_NBR_PVT_TMR);
}


/*$PAGE*/
/*
************************************************************************************************************************
*                                            INITIALIZE TICKER INTERRUPT HANDLER
*
* Description: Tick interrupt handler.
*
* Arguments  : 'p_arg'  Pointer to interrupt handler argument.
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OS_CSP_TickISR_Handler (void  *p_arg)
{
    (void)p_arg;
    
     CSP_TmrIntClr(CSP_TMR_NBR_00);
     OSTimeTick();     
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          ARM INTERRUPT/EXCEPTION HANDLER
*
* Description : Handle ARM exceptions.
*
* Argument(s) : src_id     ARM exception source identifier:
*
*                                  OS_CPU_ARM_EXCEPT_RESET             0x00
*                                  OS_CPU_ARM_EXCEPT_UNDEF_INSTR       0x01
*                                  OS_CPU_ARM_EXCEPT_SWI               0x02
*                                  OS_CPU_ARM_EXCEPT_ABORT_PREFETCH    0x03
*                                  OS_CPU_ARM_EXCEPT_ABORT_DATA        0x04
*                                  OS_CPU_ARM_EXCEPT_RSVD              0x05
*                                  OS_CPU_ARM_EXCEPT_IRQ               0x06
*                                  OS_CPU_ARM_EXCEPT_FIQ               0x07
*
* Return(s)   : none.
*
* Caller(s)   : OS_CPU_ARM_ExceptHndlr(), which is declared in os_cpu_a.s.
*
* Note(s)     : (1) Only OS_CPU_ARM_EXCEPT_FIQ and OS_CPU_ARM_EXCEPT_IRQ exceptions handler are implemented. 
*                   For the rest of the exception a infinite loop is implemented for debuging pruposes. This behavior
*                   should be replaced with another behavior (reboot, etc).
*********************************************************************************************************
*/

void  OS_CPU_IntHandler (CPU_INT32U  src_id)
{    
    switch (src_id) {
        case OS_CPU_ARM_EXCEPT_IRQ:
        case OS_CPU_ARM_EXCEPT_FIQ:
             CSP_IntHandler();
             break;
       
        case OS_CPU_ARM_EXCEPT_RST:
        case OS_CPU_ARM_EXCEPT_UND:
        case OS_CPU_ARM_EXCEPT_SWI:
        case OS_CPU_ARM_EXCEPT_ABORT_DATA:
        case OS_CPU_ARM_EXCEPT_ABORT_PREFETCH:
        case OS_CPU_ARM_EXCEPT_RSVD:        
        default:        
             OS_CSP_BSP_ExceptHandler((CPU_INT08U)src_id);
             break;
    }
}

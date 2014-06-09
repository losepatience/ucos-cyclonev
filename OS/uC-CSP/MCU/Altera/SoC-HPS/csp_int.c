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
*                                         INTERRUPT MANAGMENT
*
*                                          Altera Dev-5XS1
* 
* Filename      : csp_int.c
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

#define    CSP_INT_MODULE
#include   <csp.h>


/*
*********************************************************************************************************
*                                          DEFAULT CONFIGURATION
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE    (CPU_ADDR   )0xFFFEC100

#define  SOCFPGA5XS1_CSP_INT_REG_ICCICR              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE + 0x00))
#define  SOCFPGA5XS1_CSP_INT_REG_ICCPMR              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE + 0x04))
#define  SOCFPGA5XS1_CSP_INT_REG_ICCBPR              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE + 0x08))
#define  SOCFPGA5XS1_CSP_INT_REG_ICCIAR              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE + 0x0C))
#define  SOCFPGA5XS1_CSP_INT_REG_ICCEOIR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE + 0x10))
#define  SOCFPGA5XS1_CSP_INT_REG_ICCRPR              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE + 0x14))
#define  SOCFPGA5XS1_CSP_INT_REG_ICCHPIR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE + 0x18))
#define  SOCFPGA5XS1_CSP_INT_REG_ICCABPR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE + 0x1C))
#define  SOCFPGA5XS1_CSP_INT_REG_ICCIIDR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_CPU_INTF_REG_BASE + 0xFC))


#define  SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE         (CPU_ADDR   )0xFFFED000

#define  SOCFPGA5XS1_CSP_INT_REG_ICDDCR              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x000))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDICTR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x004))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDIIDR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x008))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDISR              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x080))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDISER             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x100))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDICER             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x180))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDISPR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x200))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDICPR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x280))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDABR              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x300))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDIPR              (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x400))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDIPTR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0x800))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDICFR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0xC00))
#define  SOCFPGA5XS1_CSP_INT_REG_ICDSGIR             (*(CPU_REG32 *)(SOCFPGA5XS1_CSP_INT_ADDR_GIC_REG_BASE + 0xF00))


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                           LOCAL DATA TYPES
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
*                                      INTERRUPT VECTOR TABLE
*
* Note(s) : (1) 'CSP_MainVectTbl[]' table is used by RTOS to dispath ISRs and measure interrupt 
*                excecution time.
*********************************************************************************************************
*/

static  CSP_INT_VECT  CSP_MainVectTbl[CSP_INT_SRC_NBR_MAX]; 


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
*                                            CSP_IntClr()
*
* Description : Clear an interrupt source on a specific interrupt controller.
*
* Argument(s) : int_ctrl   Interrupt controller number. (see note #1).
*
*               src_nbr    Interrupt source number. (see note #2). 
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Interrupt controllers numbers identifiers can be found in 'csp_grp.h' 
* 
*                       CSP_INT_CTRL_NBR_XX'  where 'XX" is the name of the interrupt controller.
*
*               (2) Interrupt source identifiers can be found in 'csp_grp.h' as CSP_INT
*
*                       CSP_INT_SRC_NBR_XX      where XX is the peripheral or interrupt source name.
*                                               Main interrupt controller only.
* 
*                       CSP_INT_XX_SRC_NBR_YY   where xx is the name of the interrupt controller or shared 
*                                               handler and YY is the peripheral or source name.
*********************************************************************************************************
*/

void  CSP_IntClr (CSP_DEV_NBR  int_ctrl,
                  CSP_DEV_NBR  src_nbr)
{
    SOCFPGA5XS1_CSP_INT_REG_ICCEOIR = src_nbr;
}


/*
*********************************************************************************************************
*                                          CSP_IntDis()
*
* Description : Disable an interrupt source on a specific interrupt controller.
*
* Argument(s) : int_ctrl   Interrupt controller number. (see 'CSP_IntClr()' note #1).
*
*               src_nbr    Interrupt source number. (see 'CSP_IntClr()' note #2).
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_IntDis (CSP_DEV_NBR  int_ctrl,
                  CSP_DEV_NBR  src_nbr)
{   
    CPU_INT32U  reg;  
    
    reg  =  (CPU_INT32U)&SOCFPGA5XS1_CSP_INT_REG_ICDICER;
	reg += ((src_nbr / 32u) * 4u);

    (*(CPU_REG32 *)reg) = (1u << (src_nbr & 0x1F));
}


/*
*********************************************************************************************************
*                                          CSP_IntDisAll()
*
* Description : Disable all interrupts on a specific interrupt controller.
*
* Argument(s) : int_ctrl   Interrupt controller number. (see 'CSP_IntClr()' note #1).
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_IntDisAll (CSP_DEV_NBR  int_ctrl)
{
    DEF_BIT_CLR_32(SOCFPGA5XS1_CSP_INT_REG_ICDDCR, DEF_BIT_00);
}


/*
*********************************************************************************************************
*                                          CSP_IntEn()
*
* Description : Enable an interrupt source on a specific interrupt controller.
*
* Argument(s) : int_ctrl   Interrupt controller number. (see 'CSP_IntClr()' note #1).
*
*               src_nbr    Interrupt source number. (see 'CSP_IntClr()' note #2).
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_IntEn (CSP_DEV_NBR  int_ctrl,
                 CSP_DEV_NBR  src_nbr)
{
    CPU_INT32U  reg;  
    
    reg  =  (CPU_INT32U)&SOCFPGA5XS1_CSP_INT_REG_ICDISER;
	reg += ((src_nbr / 32u) * 4u);

    (*(CPU_REG32 *)reg) = (1u << (src_nbr & 0x1F));
}


/*
*********************************************************************************************************
*                                          CSP_IntInit()
*
* Description : Initialize all interrupt controllers and shared handlers.
*                   (1) Disable all interrupts.
*                   (2) Clear all pending interrupts.
*                   (3) Initialize all vector interrupt tables.
*                   (4) Install all shared interrupt handlers. 
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_IntInit (void)
{
    CPU_INT32U     reg;
    CPU_INT32U     core_msk;
	CPU_INT08U     i;
    CPU_INT16U     src_nbr;
    CSP_INT_VECT  *p_int_vect;
    CPU_SR_ALLOC();	
	
	
    DEF_BIT_CLR_32(SOCFPGA5XS1_CSP_INT_REG_ICDDCR, DEF_BIT_00);    /* Disable the Global Interrupt Controller.             */
	
	for (i = 0u; i < (CSP_INT_SRC_NBR_MAX / 32u); i++) {        /* Disable all the GIC irq sources.                     */
	    reg  = (CPU_INT32U)&SOCFPGA5XS1_CSP_INT_REG_ICDICER;
 	    reg += (4u + (i * 4u));

        (*(CPU_REG32 *)reg) = 0xFFFFFFFF;
	  
 	    reg  = (CPU_INT32U)&SOCFPGA5XS1_CSP_INT_REG_ICDICPR;
 	    reg +=  i * 4u;

        (*(CPU_REG32 *)reg) = 0xFFFFFFFF;
	}
		 
	core_msk = (DEF_BIT_00 | DEF_BIT_08 | DEF_BIT_16 | DEF_BIT_24); 

    for (i = 0u; i < ((CSP_INT_SRC_NBR_MAX - 32u) / 4u); i++) {
	  reg  = (CPU_INT32U)&SOCFPGA5XS1_CSP_INT_REG_ICDIPTR;
 	    reg += (((32u / 4u) + i) * 4u);

        (*(CPU_REG32 *)reg) = core_msk;	
	}

                                                                /* Initialize the vector table.                         */
    for (src_nbr = 0u; src_nbr < CSP_INT_SRC_NBR_MAX; src_nbr++) {
        p_int_vect = &CSP_MainVectTbl[src_nbr];

        CPU_CRITICAL_ENTER();
        CSP_IntVectClr(p_int_vect);                             /* Initialize main vector table entry.                  */
        CPU_CRITICAL_EXIT();
    }
		 
    DEF_BIT_SET_32(SOCFPGA5XS1_CSP_INT_REG_ICCPMR, DEF_BIT_07);    /* Set priority mask.                                   */  

    
    DEF_BIT_SET_32(SOCFPGA5XS1_CSP_INT_REG_ICCICR, DEF_BIT_00);    /* Enable CPU interface.                                */
	DEF_BIT_SET_32(SOCFPGA5XS1_CSP_INT_REG_ICDDCR, DEF_BIT_00);    /* Enable the Global Interrupt Controller.              */ 
}
		 

/*
*********************************************************************************************************
*                                          CSP_IntSrcCfg()
*
* Description : Configure an interrupt source.
*
* Argument(s) : int_ctrl   Interrupt controller number. (see 'CSP_IntClr()' note #1).
*
*               src_nbr    Interrupt source number. (see 'CSP_IntClr()' note #2).
*
*               src_prio   Interrupt source priority.
*
*               src_pol    Interrupt source polarity:
*                              CSP_INT_POL_LEVEL_HIGH           Interrupt is activated on a high level.
*                              CSP_INT_POL_LEVEL_LOW            Interrupt is activated on a low level.
* 
*                              CSP_INT_POL_EDGE_RISING          Interrupt is activated on the rising edge.
*                              CSP_INT_POL_EDGE_FALLING         Interrupt is activated on the falling edge.
*                              CSP_INT_POL_EDGE_BOTH            Interrupt is activated on both edges.       
*
*
* Return(s)   : DEF_OK,    if the interrupt source was configured
*               DEF_FAIL,  otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Interrupt source priority is determined by the interrupt controller implementation.
*                   The maximum and minimum values are defined in the family definition file 'csp_grp.h'
*********************************************************************************************************
*/

CPU_BOOLEAN  CSP_IntSrcCfg (CSP_DEV_NBR  int_ctrl,
                            CSP_DEV_NBR  src_nbr,
                            CSP_OPT      src_prio,
                            CSP_OPT      src_pol)

{
    CPU_INT32U  reg;  


#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (int_ctrl != CSP_INT_CTRL_NBR_MAIN) {
        return (DEF_FAIL);
    }
    
    if (src_nbr > CSP_INT_SRC_NBR_MAX - 1u) {
        return (DEF_FAIL);
    }

    if ((src_prio > CSP_INT_SRC_PRIO_LOWEST) | 
		(src_prio < CSP_INT_SRC_PRIO_HIGHEST)) {
        return (DEF_FAIL);
    }
#endif
    
    reg  =  (CPU_INT32U)&SOCFPGA5XS1_CSP_INT_REG_ICDIPR;        /* Configure interrupt priority.                        */
	reg +=  ((src_nbr / 4u) * 4);

	(*(CPU_REG32 *)reg) &= ~(0xffu << ((src_nbr % 4u) * 8u));
	(*(CPU_REG32 *)reg) |= (src_prio << ((src_nbr % 4u) * 8u));
	 
    reg  =  (CPU_INT32U)&SOCFPGA5XS1_CSP_INT_REG_ICDICFR;                   
    reg +=  ((src_nbr / 16u) * 4);
	
	if (src_pol == CSP_INT_POL_LEVEL_HIGH || src_pol == CSP_INT_POL_LEVEL_LOW) { 
	    DEF_BIT_CLR_32((*(CPU_REG32 *)reg), (1u << (((src_nbr % 16u) * 2u) + 1u)));
	} else {
	    DEF_BIT_SET_32((*(CPU_REG32 *)reg), (1u << (((src_nbr % 16u) * 2u) + 1u))); 
	}
		 
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                          CSP_IntVectReg()
*
* Description : Register an interrupt service routine handler for a specific interrupt controller.
*
* Argument(s) : int_ctrl   Interrupt controller number. (see 'CSP_IntClr()' note #1).
*
*               src_nbr    Interrupt source number. (see 'CSP_IntClr()' note #2).
*
*               isr_fnct    Pointer to the interrupt service routine (ISR).
*
*               p_arg       Pointer to the argument of the interrupt service routine (ISR).
*
* Return(s)   : DEF_OK,    If the vector was registered successfully.
*               DEF_FAIL,  otherwise.
*               
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  CSP_IntVectReg (CSP_DEV_NBR    int_ctrl,
                             CSP_DEV_NBR    src_nbr,
                             CPU_FNCT_PTR   isr_fnct,
                             void          *p_arg)
{
    CSP_INT_VECT  *p_int_vect;
    CPU_SR_ALLOC();
	
	
#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (int_ctrl != CSP_INT_CTRL_NBR_MAIN) {
        return (DEF_FAIL);
    }
    
    if (src_nbr > CSP_INT_SRC_NBR_MAX - 1u) {
        return (DEF_FAIL);
    }
#endif

    p_int_vect = (CSP_INT_VECT  *)&CSP_MainVectTbl[src_nbr];
	
    CPU_CRITICAL_ENTER();

    CSP_IntVectSet((CSP_INT_VECT *)p_int_vect,
                   (CPU_FNCT_PTR  )isr_fnct,
                   (void *        )p_arg);

    CPU_CRITICAL_EXIT();
    
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                          CSP_IntVectUnreg()
*
* Description : Un-register an interrupt vector.
*
* Argument(s) : int_ctrl   Interrupt controller number. (see 'CSP_IntClr()' note #1).
*
*               src_nbr    Interrupt source number. (see 'CSP_IntClr()' note #2).
*
* Return(s)   : DEF_OK,     If the vector was unregistered successfully.
*               DEF_FAIL,   otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  CSP_IntVectUnreg (CSP_DEV_NBR  int_ctrl,
                               CSP_DEV_NBR  src_nbr)
{
    CSP_INT_VECT  *p_int_vect;
    CPU_SR_ALLOC();

#if (CSP_CFG_ARG_CHK_EN == DEF_ENABLED)
    if (int_ctrl != CSP_INT_CTRL_NBR_MAIN) {
        return (DEF_FAIL);
    }

    if (src_nbr > CSP_INT_SRC_NBR_MAX - 1u) {
        return (DEF_FAIL);
    }
#endif
    
    p_int_vect = (CSP_INT_VECT  *)&CSP_MainVectTbl[src_nbr];

    CPU_CRITICAL_ENTER();

    CSP_IntVectClr(p_int_vect);

    CPU_CRITICAL_EXIT();

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                       CSP_IntHandlerSrc()
*
* Description : Global interrupt handler.
*
* Argument(s) : src_nbr      Interrupt source number.
*
* Return(s)   : none
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_IntHandlerSrc (CSP_DEV_NBR  src_nbr)
{
}


/*
*********************************************************************************************************
*                                         CSP_IntHandler()
*
* Description : Global interrupt handler.
*
* Argument(s) : None.
*
* Return(s)   : none
*
* Caller(s)   : Application.
*
* Note(s)     : None.
*********************************************************************************************************
*/

void  CSP_IntHandler  (void)
{
    CPU_INT32U    int_id;
    CSP_INT_VECT  int_vect;

	
    int_id = SOCFPGA5XS1_CSP_INT_REG_ICCIAR;
    int_id = int_id & 0x000003FF;	
	
    int_vect = CSP_MainVectTbl[int_id];

	CSP_IntVectDeref(&int_vect);                  		        /* Call Dereferencing function                       	*/
	CSP_IntClr(CSP_INT_CTRL_NBR_MAIN, int_id);   		        /* Clear interrupt									   	*/	
}
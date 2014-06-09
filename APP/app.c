/*
 *********************************************************************************************************
 *                                              EXAMPLE CODE
 *
 *                             (c) Copyright 2011; Micrium, Inc.; Weston, FL
 *
 *               All rights reserved.  Protected by international copyright laws.
 *               Knowledge of the source code may NOT be used to develop a similar product.
 *               Please help us continue to provide the Embedded community with the finest
 *               software available.  Your honesty is greatly appreciated.
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *
 *                                            EXAMPLE CODE
 *
 *                          Altera Virtual DEV-5XS1 Cortex-A9 MPCore DEVELOPMENT KIT
 *
 * Filename      : app.c
 * Version       : V1.00
 * Programmer(s) : NB
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *                                             INCLUDE FILES
 *********************************************************************************************************
 */

#include <dwgpio.h>
#include <delay.h>
#include <ns16550.h>
#include <malloc.h>
#include <includes.h>
#include <stdio.h>
#include <stddef.h>
#include <list.h>
#include <spi_flash.h>
#include <fpga_uart.h>
#include <fpga_usb.h>
#include <old_apis.h>

/*
 *********************************************************************************************************
 *                                             LOCAL DEFINES
 *********************************************************************************************************
 */


/*
 *********************************************************************************************************
 *                                            LOCAL VARIABLES
 *********************************************************************************************************
 */

static  OS_STK         App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

/*
 *********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 *********************************************************************************************************
 */

static  void          App_TaskStart (void  *p_arg);

#define DEBUG_BASE 0x2000000
/* for debug purpose only */
/*void bufferit(unsigned int value)
  {
  static int MAX = 20;
  static int head = 0;
  unsigned int * buffer = DEBUG_BASE; //ARM7_UTXBUF;

 *(buffer + head) = value;
 if(head < MAX-1)
 head = head +1;
 else
 head = 0;

 }
 */
/*
 *********************************************************************************************************
 *                                                main()
 *
 * Description : This is the standard entry point for C code.  It is assumed that your code will call
 *               main() once you have performed all necessary initialization.
 *
 * Argument(s) : none
 *
 * Return(s)   : none
 *
 * Caller(s)   : Startup Code.
 *
 * Note(s)     : none.
 *********************************************************************************************************
 */

int  main (void)
{

	/* the following is for debug only
	   asm volatile ("mov r7,#21");
	   while(1)
	   {
	   ;}
	   bufferit(1);
	   */
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_ERR  cpu_err;
#endif
#if (OS_TASK_NAME_EN > 0u)
	CPU_INT08U  os_err;
#endif
	/*   bufferit(2); */
	//BSP_PreInit();
	//CSP_PM_Init();

	CPU_IntDis();
	CSP_IntInit();
	/*
	   bufferit(3); */
	CPU_Init();                                                 /* Initialize the uC/CPU services */
	/*   bufferit(4);       */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_NameSet((CPU_CHAR *)CSP_DEV_NAME,
			(CPU_ERR  *)&cpu_err);
#endif

	/*   bufferit(5); */
	CPU_IntDis();
	/*  bufferit(6); */
	OSInit();                                                   /* Init uC/OS-II.  */
	/*      bufferit(7);       */

	OSTaskCreateExt((void (*)(void *)) App_TaskStart,           /* Create the start task                                */
			(void           *) 0,
			(OS_STK         *)&App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
			(INT8U           ) APP_CFG_TASK_START_PRIO,
			(INT16U          ) APP_CFG_TASK_START_PRIO,
			(OS_STK         *)&App_TaskStartStk[0],
			(INT32U          ) APP_CFG_TASK_START_STK_SIZE,
			(void           *) 0,
			(INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

	/* bufferit(8); */
#if (OS_TASK_NAME_EN > 0u)
	OSTaskNameSet(APP_CFG_TASK_START_PRIO, (INT8U *)"Start", &os_err);
#endif
	/*    bufferit(9);
	*/
	OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */
	/*    bufferit(10); */
	while (DEF_TRUE) {                                          /* Should not get here                                  */
		;
	}
}


/*
 *********************************************************************************************************
 *                                          STARTUP TASK
 *
 * Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
 *               initialize the ticker only once multitasking has started.
 *
 * Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
 *
 * Returns     : none
 *
 * Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
 *                  used.  The compiler should not generate any code for this statement.
 *********************************************************************************************************
 */

int g_app_count = 0;
int loga = 0;
static  void  App_TaskStart (void *p_arg)
{

	//BSP_PostInit();                                             /* Initialize BSP functions   */
#include <clock.h>
	clock_init();
	OS_CSP_TickInit();                                          /* Initialize the Tick interrupt                        */
	unsigned int start_time = OSTimeGet();
	Mem_Init();                                                 /* Initialize memory managment module                   */
	Math_Init();                                                /* Initialize mathematical module                       */

#if (OS_TASK_STAT_EN > 0u)
	OSStatInit();                                               /* Determine CPU capacity                               */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
	CPU_IntDisMeasMaxCurReset();
#endif

#ifdef __STDC__
#  define STR1	"hello world\n"
#else
#  define STR1	"bye world\n"
#endif

	ns16550_init();

	printf("\n%s\n", STR1);


	mem_malloc_init(0x1000000, 0x1000000);

#if 0
	LIST_HEAD(list);
	LIST_HEAD(list2);
	list_add_tail(&list2, &list);
#endif

	timer_init();

#if 0
#include <dwgpio.h>
	PIO_InitializeInterrupts(1);
	Pin pin = {1 <<12, DW_BASE_PIOB, DW_ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT};
	PIO_Configure(&pin, 1);
#endif

#define	SPI_MODE_3	(SPI_CPOL|SPI_CPHA)

	char sfrbuf[12];
	char *sfwbuf = "good good study!";
	struct spi_flash *flash;
	flash = spi_flash_probe(0, 0, 50000000, SPI_MODE_3);
	if (!flash) {
		puts("SPI probe failed.\n");
	} else {
		spi_flash_erase(flash, 0, 64 * 1024);
		spi_flash_write(flash, 0, 0xF, (void *)sfwbuf);
		int i;
		for (i = 0; i < 1000; i++)
			spi_flash_read(flash, 0, 12, (void *)sfrbuf);
	}
	//int fpret = fpgamgr_program_fpga(0x0, 0x0);
	unsigned int end_time = OSTimeGet();
	printf("time %x\n", end_time - start_time);

	char msbuf[128];
	char sbbuf[128];
	int i;
	for (i = 0; i < 128; i++)
		msbuf[i] = i;

#if 0
	USB_Init();


	(void)OSTaskCreateExt(USB_Task,
			(void *)0,                                 /* No arguments passed to OS_TaskIdle() */
			&TaskUsbStk[USB_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
			USB_TASK_PRIO,                            
			USB_TASK_ID,
			&TaskUsbStk[0],                         /* Set Bottom-Of-Stack                  */
			USB_TASK_STK_SIZE,
			(void *)0,                                 /* No TCB extension                     */
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);/* Enable stack checking + clear stack  */
	OSTaskNameSet(USB_TASK_PRIO, (INT8U *)"USB", &err);
#endif
#if 0
	cy_uart_init();
	int retdd = cy_uart_tx(0, msbuf, 120);
	retdd = cy_uart_rx(0, sbbuf, 120);
#endif
#if 0
	lwip_entry(NULL);
	void lock(void *arg);
	request_irq(74, lock, NULL);
	int abc = 1;
#endif
	while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */

		//gpio_direction_output(41, abc); //led0
		printf("%x\n", loga);

		OSTimeDlyHMSM(0, 0, 1, 0);
	}
}

#if 0
void lock(void *arg)
{
	printf("wokao \n");
}
#endif

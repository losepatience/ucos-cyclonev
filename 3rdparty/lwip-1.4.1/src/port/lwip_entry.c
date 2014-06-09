#ifndef _TASK_H
#define _TASK_H

#define  NO_TASKS           15                  /* Number of identical tasks */

#define    	 USB_TASK_PRIO	2	//This should be the highest task to do real work;
#define     EMAC_TASK_PRIO	3
#define    START_TASK_PRIO	4
#define KEYBOARD_TASK_PRIO	6
#define   UPDATE_TASK_PRIO	7
#define    CLEAN_TASK_PRIO	8   //The priority of this task must be higher than paser task
#define   PARSER_TASK_PRIO	10
#define   STATUS_TASK_PRIO	11	//Can not do anything ocupy time
#define  CONTROL_TASK_PRIO	12
#if defined( HEAD_EPSON_GEN5)
#define     CALI_TASK_PRIO	14
#endif
#if defined(EPSON_BOTTOM_BOARD_V3) && defined(SUPPORT_MOTOR_CONTROL)
#define   MOTION_TASK_PRIO	5
#endif

#define    	 USB_TASK_ID	102
#define    	EMAC_TASK_ID	103
#define    START_TASK_ID	104
#define KEYBOARD_TASK_ID	105
#define   UPDATE_TASK_ID	106
#define    CLEAN_TASK_ID	108
#define   PARSER_TASK_ID	110
#define   STATUS_TASK_ID	111
#define  CONTROL_TASK_ID	112
#if defined( HEAD_EPSON_GEN5)
#define     CALI_TASK_ID	114
#endif
#if defined(EPSON_BOTTOM_BOARD_V3) && defined(SUPPORT_MOTOR_CONTROL)
#define   MOTION_TASK_ID	107
#endif

#define    START_TASK_STK_SIZE  2048    /* Size of each task's stacks (# of WORDs) Handy change from 256*/
#define 	EMAC_TASK_STK_SIZE	4096
#define 	 USB_TASK_STK_SIZE	512//256
#define   STATUS_TASK_STK_SIZE 	512//256
#define    CLEAN_TASK_STK_SIZE  256
#define   PARSER_TASK_STK_SIZE	2048 //512 //256 ///cheney: Flash module need large memory at stack.
#define  CONTROL_TASK_STK_SIZE	256
#define KEYBOARD_TASK_STK_SIZE  512 //256
#define   UPDATE_TASK_STK_SIZE  256
#if defined( HEAD_EPSON_GEN5)
#define     CALI_TASK_STK_SIZE	512
#endif
#if defined(EPSON_BOTTOM_BOARD_V3) && defined(SUPPORT_MOTOR_CONTROL)
#define   MOTION_TASK_STK_SIZE	512
#endif




#endif
#include "includes.h"
#include "stdio.h"
//#include "tasks.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/dhcp.h"

#define IP_ADDR_0 192
#define IP_ADDR_1 168
#define IP_ADDR_2 1
#define IP_ADDR_3 2

#define NET_MASK0 255
#define NET_MASK1 255
#define NET_MASK2 255
#define NET_MASK3 0

#define GATEWAY_ADDR_0 192
#define GATEWAY_ADDR_1 168
#define GATEWAY_ADDR_2 1
#define GATEWAY_ADDR_3 1


extern err_t ethernetif_init(struct netif * stNetif);

static struct netif emac_netif;
void set_lwip(void)
{
    struct ip_addr _ip_addr, _net_mask, _gateway;

    netif_init();

    IP4_ADDR(&_ip_addr, IP_ADDR_0, IP_ADDR_1, IP_ADDR_2, IP_ADDR_3);
    IP4_ADDR(&_net_mask, NET_MASK0, NET_MASK1, NET_MASK2, NET_MASK3);
    IP4_ADDR(&_gateway, GATEWAY_ADDR_0, GATEWAY_ADDR_1, GATEWAY_ADDR_2, GATEWAY_ADDR_3);

    netif_add(&emac_netif, &_ip_addr, &_net_mask, &_gateway, NULL, ethernetif_init, tcpip_input);

    netif_set_default(&emac_netif);

    netif_set_up(&emac_netif);

    /*
    Creates a new DHCP client for this interface on the first call.
    Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
    the predefined regular intervals after starting the client.

    You can peek in the netif->dhcp struct for the actual DHCP status.
    */
#if LWIP_DHCP
    dhcp_start(&emac_netif);
#endif

}

OS_STK 	TaskEmacStk[EMAC_TASK_STK_SIZE] __attribute__((aligned(8)));      /* EMAC Tasks stack */

struct testspeed_info
{
    u8_t b_start;
    u32_t start_time;
    u32_t end_time;

    u32_t data_len;

    u8_t b_report;
    u32_t report_len;
    u8_t report[16];
} tsi;

err_t testspeed_poll(void *arg, struct tcp_pcb *tpcb)
{
    if(tsi.b_report)
    {
        *((u32_t*)tsi.report) = tsi.start_time;
        *((u32_t*)tsi.report + 1) = tsi.end_time;
        *((u32_t*)tsi.report + 2) = tsi.data_len;

        err_t rc = tcp_write(tpcb, tsi.report, 3 * sizeof(u32_t), 0);
        if(rc == ERR_OK)
        {
          tcp_poll(tpcb, NULL, 0);
        }
    }
    return ERR_OK;
}

int start_data = 0;
volatile int start_data_f = 0;
int start_data_count_rec = 0;
int start_data_count_rec_err = 0;


err_t
testspeed_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  struct testspeed_info *es;
  err_t ret_err;

  start_data ++;
  start_data_f = 1;

  es = (struct testspeed_info *)arg;
  if (p == NULL)
  {
      tcp_arg(tpcb, NULL);
      tcp_sent(tpcb, NULL);
      tcp_recv(tpcb, NULL);
      tcp_err(tpcb, NULL);
      tcp_poll(tpcb, NULL, 0);

      tcp_close(tpcb);
    ret_err = ERR_OK;
  }
  else if(err != ERR_OK)
  {
    /* cleanup, for unkown reason */
    if (p != NULL)
    {
      pbuf_free(p);
      start_data_count_rec_err ++;
    }
    ret_err = err;
  }
  else
  {
      start_data_count_rec ++;
    if(tsi.b_start == 0)
    {
        tsi.b_start = 1;
        tsi.end_time = tsi.start_time = OSTimeGet();
        tsi.data_len = 0;
    }
    else
    {
        tsi.data_len += p->tot_len;
        tsi.end_time = OSTimeGet();
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    SYS_ARCH_DECL_PROTECT(lev);

	if (tsi.data_len >= 0x40000000)
		pr_info("good, complete\n");
    //int i;

    //for(i=0; i<0x100; i++);

//    SYS_ARCH_PROTECT(lev);
//    if(!tsi.b_report)
//    {
//        tsi.b_report = 1;
//        SYS_ARCH_UNPROTECT(lev);
//        *((u32_t*)tsi.report) = tsi.start_time;
//        *((u32_t*)tsi.report + 1) = tsi.end_time;
//        *((u32_t*)tsi.report + 2) = tsi.data_len;
//
//        err_t rc = tcp_write(tpcb, tsi.report, 3 * sizeof(u32_t), 0);
//        if(rc == ERR_MEM)
//        {
//            tcp_poll(tpcb, testspeed_poll, 4);
//  //abour 2 second
//        }
//    }
//    else
//        SYS_ARCH_UNPROTECT(lev);

    ret_err = ERR_OK;
  }
  return ret_err;
}

err_t testspeed_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    SYS_ARCH_DECL_PROTECT(lev);

    tcp_poll(tpcb, NULL, 0);

    SYS_ARCH_PROTECT(lev);
    tsi.b_report = 0;
    SYS_ARCH_UNPROTECT(lev);

    return ERR_OK;
}

void  testspeed_err(void *arg, err_t err)
{
    SYS_ARCH_DECL_PROTECT(lev);

    SYS_ARCH_PROTECT(lev);
    tsi.b_report = 0;
    SYS_ARCH_UNPROTECT(lev);
}

err_t lwip_testspeed_accept(void *arg, struct tcp_pcb *newpcb,
                                   err_t err)
{
    struct tcp_pcb_listen *lpcb = (struct tcp_pcb_listen*)arg;

	pr_info("ac\n");
    if(err != ERR_OK)
        return err;

    tcp_accepted(lpcb);

    tcp_setprio(newpcb, TCP_PRIO_NORMAL);


    tsi.b_start = 0;
    tsi.start_time = 0;
    tsi.end_time = 0;
    tsi.data_len = 0;
    tsi.b_report = 0;
    tsi.report_len = 0;

    tcp_arg(newpcb, &tsi);



    tcp_recv(newpcb, testspeed_recv);


    tcp_err(newpcb, testspeed_err);


    tcp_sent(newpcb, testspeed_sent);


    return ERR_OK;
}

void lwip_demo_task(void * pv_arg)
{
    struct tcp_pcb * rec_pcd = tcp_new();

    err_t err;
    while( (err = tcp_bind(rec_pcd, IP_ADDR_ANY, 35)) != ERR_OK)
        OSTimeDly(2000);

    struct tcp_pcb * rec_listen_pcd;
    while( (rec_listen_pcd = tcp_listen(rec_pcd)) == NULL)
        OSTimeDly(2000);

    tcp_arg(rec_listen_pcd, rec_listen_pcd);

    tcp_accept(rec_listen_pcd, lwip_testspeed_accept);

    for(;;)
    {
#if LWIP_DHCP
        u32_t i = 0;
        while (i < DHCP_COARSE_TIMER_MSECS)
        {
            OSTimeDly(DHCP_FINE_TIMER_MSECS);
            dhcp_fine_tmr();
            i += DHCP_FINE_TIMER_MSECS;
        }
        dhcp_coarse_tmr();
#else
        OSTimeDly(200);
#endif

    }

}

void lwip_entry(void * pv_arg)
{
    INT8U err;

    tcpip_init(NULL,NULL);

    set_lwip();

    (void)OSTaskCreateExt(lwip_demo_task,
                          (void *)0,                                 /* No arguments passed to OS_TaskIdle() */
                          &TaskEmacStk[EMAC_TASK_STK_SIZE - 1], /* Set Top-Of-Stack                     */
                          EMAC_TASK_PRIO,
                          EMAC_TASK_ID,
                          &TaskEmacStk[0],                         /* Set Bottom-Of-Stack                  */
                          EMAC_TASK_STK_SIZE,
                          (void *)0,                                 /* No TCB extension                     */
                          0);
    OSTaskNameSet(EMAC_TASK_PRIO, (INT8U *)"EMAC", &err);

    OSStart();
    while (1) {
        OSTimeDly(200);
    }
}

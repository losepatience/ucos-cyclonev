#include "consol.h"
#include "status.h"
#include "tasks.h"
#include "keylcd.h"
#include "global.h"
#include "string.h"
#include "control.h"
#include "display.h"
#include "miscellance.h"
#include "uv.h"

#define STACK_LEN	10
#define STATUS_CHANGE 		0x80000000	 //1 bit; Only set a flag that some flags are changed

INT8U uv_PrintScanErr(void);
static void CopyStatus(INT32U *stack, status_EP0BUF_TYPE *dst);

static OS_FLAGS status_Flags, status_Old_Flags;

static INT32U war_stack[STACK_LEN], err_stack[STACK_LEN],ini_stack[STACK_LEN],fta_stack[STACK_LEN],svc_stack[STACK_LEN],update_stack[STACK_LEN], err_stack_auto[STACK_LEN];
#ifdef FUNC_MAINTAIN
INT8U bStatus_Maintain = False;
#endif
const char *empty ="                ";
const char *status_message[20] = {
	"Ready           ",
	"Printing...     ",
	"Cleaning...     ",
	"Moving...       ",
	"Canceling...    ",
	"Pause           ",
};

const char * main_message[20] = {
	"Service call:   ",
	"Fatal error:    ",
	"Error:          ",
	"Initializing... ",
	"Warning:        ",	
};

const char * svc_message[] = {
	"USB startup fail",
	"FPGA cfg fail-1 ",	
	"FPGA cfg fail-2 ",
	"FPGA cfg fail-3 ",
	"FPGA cfg fail-4 ",
	"UPDATE Main Board Failed",
	"Board without BYHX initialization",
	"Check  SDRAM failed",
	"Cann't read FPGA from flash",
	"SDRAM is too small",
	"Operating encryption chip failure",
	"ARM Motion system initial error",
	"HEAD board operating encryption chip failure",
	"Power Off Mode,Please Close Printer",
	"",
	"",
	"Check BYHX_TOOL EEPROM failed"
};

const char *fta_message[] = {
	"USB unexpect reset",
	"INTERNAL ERROR 1",
	"I-head size NG  ",
	"I-Job start NG  ", //Has extra data
	"I-band data size",
	"I-COM Format    ",	
	"I-DMA0 working  ",
	"I-Prt pos wrong ",
	"I-Prt pos<OLimt ",
	"I-Prt pos>OPLimt",
	"DSP INIT S1 Fail",
	"DSP INIT S2 Fail",
	"Head INITS1 Fail",
	"Head INITS2 Fail",
	"Road HB->MB Fail",
	"Wrong Direction",
	"Updated failure at the main board writing stage",//0x10	
	"Read EEPROM failure",
	"Writing EEPROM failure",
	"Board without ex-factory initialization Settings",
	"Head board is restarted",
	"The first stage initialization failed",
	"The nozzle type  don't match",
	"Control system with FW manufacturers don't match",
	"More than limited time, please enter again password",
	"Internal error, blank band and y-distance is 0",
	"Connected to USB1.1 port, re-connecting",
	"The language you can not use",
	"PINCONF writing FPGA errors",
	"The color number written in factory is wrong",
	"The head board EEPROM written failure",
	"Nozzle heating over 55 degrees, all heating closed.",
	"Software error or version errors",//0x20
	"Firmware require special software version",
	"The current system setting does not support one head and two colors",
	"More than ink amount limitation, please enter again password",
	"Firmware requirements software version support ink password",
	"EPSON the print head error occurs",
	"EPSON head board FPGA error occurs",
	"Write board id error",
	" ",
	"Factory initialization error",
	" ",
	" ",
	" ",
	" ",
	" ",
	"Print data transport error or missing",
	"FACTORY_DATA data error",//0x30
	"Waste ink full ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0x40
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0x50
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0x60	
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0x70		
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0x80		
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0x90			
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0xA0	
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0xB0		
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0xC0		
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",//0xD0		
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	" ",
	"Move Internal Error 2 ",
	"Move Internal error 1 ",
	"Temperature overload protection ",
	"Current overload or voltage underload protection ",
	" ",//0xE0		
	"Read EEPROM error ",
	"Write EEPROM error ",
	"Write EEPROM 2 times ",
	"The time limit should run out of! ",
	"To limit the time there are 50 hours ",
	"Language and restrictions do not match ",
	"The contents of EEPROM error ",
	"Wrong password ",
	"Software has not dongle ",
	" ",
	" ",
	" ",
	" ",
	"Touch located sensor ",
	" ",
	" ",//0xF0			
	"Invalid COM command",
	"Invalid COM parameter",
	"Move again! ",
	"COM send time out",
	"COM data CRC error ",
	"Encoder Error! ",
	"Sensor error on measuring paper ",
	"No paper",
	"PaperJam in Horizontal ",
	"PaperJam in Vertical ",
	"Index error ",
	"Touch limit sensor ",
	"No found stepping encoder ",
	" ",
	" "//0xFF	
};

const char *err_message[] = {
	"EP0 setup overun",
	"USB1 connected  ",
	"DSP COM timeout ",
	"Head COM timeout",
	"I-Print data    ", //Not all print data is send out when print a band
	"Print data<<trig",
	"Print data>>trig",
	"FPGA  status error",
	"Internal Status",
	"FPGA received wrong command",
	"Move too fast or uneven",
	"Movement orders return time out",
	"AUTOCLEAN command should not happen",
	"AUTOCLEAN command should not happen"
		"",
		"",
		"",//0x10
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",//20
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",//30
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",//40
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"Debug status 1",//50
		"Debug status 2",
};

const char *ini_message[20] = {
	"Main board      ",
	"USB port        ",
	"FPGA            ",
	"Motion          ",
	"Head Board      ",
};

const char *war_message[50] = {
	"Unknown header  ",
	"EP0 cmd ignored ",
	"waring 2 ",
	"waring 3 ",
	"waring 4 ",
	"waring 5 ",
	"waring 6 ",
	"waring 7 ",
	"time password input except 0~f",
	"time password wrong",
	"time password and facturer ID not match",
	"time password and board ID not match",
	"first limited time waring",
	"second limited time waring",
	"last limited time waring",
	" ",
	"option password input except 0~f",
	"option password wrong",
	"option password and facturer ID not match",
	"option password and board ID not match",
	"disable pump sensor",
	"over speed not support",
	"ink password input except 0~f",
	"ink password wrong",
	"ink password and facturer ID not match",
	"ink password and board ID not match",
	"first limited inkwaring",
	"second limited ink waring",
	"last limited ink waring",
	"ink droplet value is wrong",
	"waring 30",
	"waring 31",
	"Print data is less than fire number or empty when trigger",
	"Print data is more than fire number",
	"Head temperature too low",
	"Head temperature too high",
	"Print head is not connected",
	"core board FPGA report data too more",
	"core board FPGA report data too less",
	"unkonw nozzle count",
	"EPSON Head Driver over heat",
	"XHOT error",
};

void PushStatus(INT32U * stack, INT32U number)
{
	INT8U i;
	for (i=0; i< STACK_LEN; i++)
	{
		if (stack[i] == number)
			return;
	}
	
	for (i=0; i< STACK_LEN; i++)
	{
		if (stack[i] == 0)
			break;
	}
	
	if (i >= STACK_LEN)
		CONSOL_Printf("!!!!!!!!!!!!!!Status stack overflow!!!!!!!!!!!!!!!!!!!!!!");	
	else
		stack[i] = number;	
}

INT8U PopStatus(INT32U * stack, INT32U number)
{
	INT8U i;
	for (i=0; i< STACK_LEN; i++)
	{
		if (stack[i] == number)
		{
			stack[i] = 0;
			break;
		}
	}
	//	if (i >= STACK_LEN)
	//		CONSOL_Printf("!!!Can not find status in stack!!!%x\r\n", number);	
	
	for (i=0; i< STACK_LEN; i++)
	{
		if (stack[i] != 0)
			return False; //Not empty
	}
	return True;
}

void FlushStack(INT32U * stack)
{
	INT8U i;
	for (i=0; i< STACK_LEN; i++)
		stack[i] =0;
}

void status_FlushAllErrStatus(void)
{
	FlushStack(err_stack);
}

//Do not call this function in interrupt toutine
void status_ReportStatus(INT32U number, INT8U op)
{
	INT8U err;
	
	INT32U *stack;
#ifdef CONVERSION_BOARD

	INT32U STATUS_MASK = 0;
	if(HEAD_ERROR_SET == op||HEAD_ERROR_CLEAR == op)
	{

		STATUS_MASK	 = BUF_STATUS_BITS;

		if(HEAD_ERROR_SET == op)
			op = STATUS_SET;
		else
			op = STATUS_CLR;
	}
	else
		STATUS_MASK	 = STATUS_BITS;
#endif

#ifdef CONVERSION_BOARD
	switch (number&STATUS_MASK)
#else
	switch (number&STATUS_BITS)
#endif
	{
	case STATUS_UPDATING:			
		if (op == STATUS_SET)
		{
			update_stack[0] = number;
			OSFlagPost(status_FLAG_GRP,STATUS_UPDATING|STATUS_CHANGE,OS_FLAG_SET,&err); 
#ifdef UV_PRINTER				
			uv_PrintScanErr();
#endif				
		}else
		{
			update_stack[0] = 0;
			OSFlagPost(status_FLAG_GRP,STATUS_UPDATING,OS_FLAG_CLR,&err);
			OSFlagPost(status_FLAG_GRP, STATUS_CHANGE,OS_FLAG_SET,&err); 
		}	
		return;
	case STATUS_WAR:
		stack = war_stack;
		break;
	case STATUS_ERR:
#ifdef UV_PRINTER
		if (op == STATUS_SET)				
			uv_PrintScanErr();
#endif
#if defined(SUPPORT_MOTOR_CONTROL)
		if (op == STATUS_SET)	
		{
			OSFlagPost (mix_FLAG_GRP, MOTION_URGENT_STOP_CMD, OS_FLAG_SET, &err);
#if defined(EPSON_BOTTOM_BOARD_V3)				
			Heater_Urgent_Stop();
#endif
		}
#endif
		stack = err_stack;
		break;
	case STATUS_ERR_AUTO:
#ifdef UV_PRINTER
		if (op == STATUS_SET)				
			uv_PrintScanErr();
#endif				
		stack = err_stack_auto;
		break;
	case STATUS_INI:
		stack = ini_stack;
		break;
	case STATUS_FTA:
#ifdef UV_PRINTER
		if (op == STATUS_SET)
			uv_PrintScanErr();
#endif
#if defined(SUPPORT_MOTOR_CONTROL)
		if (op == STATUS_SET && number != STATUS_FTA_USB1_USB1CONNECT)	
		{
			OSFlagPost (mix_FLAG_GRP, MOTION_URGENT_STOP_CMD, OS_FLAG_SET, &err);
#if defined(EPSON_BOTTOM_BOARD_V3)				
			Heater_Urgent_Stop();
#endif
		}
#endif
		stack = fta_stack;
		break;
	case STATUS_SVC:
#ifdef UV_PRINTER
		if (op == STATUS_SET)
			uv_PrintScanErr();
#endif
#if defined(SUPPORT_MOTOR_CONTROL)
		if (op == STATUS_SET)	
		{
			OSFlagPost (mix_FLAG_GRP, MOTION_URGENT_STOP_CMD, OS_FLAG_SET, &err);
#if defined(EPSON_BOTTOM_BOARD_V3)				
			Heater_Urgent_Stop();
#endif
		}
#endif
		stack = svc_stack;
		break;
	case STATUS_PRINT:
		FlushStack(war_stack); //Flush all waring status
		
		OSFlagPost(status_FLAG_GRP,STATUS_PAUSE|STATUS_WAR,OS_FLAG_CLR,&err);	//Clear warning, pause
		OSFlagPost(status_FLAG_GRP,STATUS_NO_CANCEL,OS_FLAG_SET,&err);	        //Cancel Flag (= Set No Cancel Flag)
		if (op == STATUS_SET)
			OSFlagPost(status_FLAG_GRP,(number&STATUS_BITS)|STATUS_CHANGE,OS_FLAG_SET,&err);
		else if (op == STATUS_CLR)
		{
			OSFlagPost(status_FLAG_GRP,number&STATUS_BITS,OS_FLAG_CLR,&err);
			OSFlagPost(status_FLAG_GRP,STATUS_CHANGE,OS_FLAG_SET,&err);
		}					
	default:
		if (op == STATUS_SET)
			OSFlagPost(status_FLAG_GRP,(number&STATUS_BITS)|STATUS_CHANGE,OS_FLAG_SET,&err);
		else if (op == STATUS_CLR)
		{
			OSFlagPost(status_FLAG_GRP,(number&STATUS_BITS),OS_FLAG_CLR,&err);
			OSFlagPost(status_FLAG_GRP,STATUS_CHANGE,OS_FLAG_SET,&err);
		}
		return;
	}
	
	if (op == STATUS_SET)
	{
		OSFlagPost(status_FLAG_GRP,(number&STATUS_BITS)|STATUS_CHANGE,OS_FLAG_SET,&err);
		PushStatus(stack, number);
		//		if (stack == war_stack)
		//			CONSOL_Printf("WARNING Status %x Set, total warnumber is %d", number, );		
	}else
	{
		if (PopStatus(stack, number)) //True: no same level status exist after pop, stack empty
			OSFlagPost(status_FLAG_GRP,(number&STATUS_BITS),OS_FLAG_CLR,&err); 
		OSFlagPost(status_FLAG_GRP, STATUS_CHANGE,OS_FLAG_SET,&err); 
	}
}
/*******************************************************************************************
*	Change the status of HOST, LCD, LED and BELL										   *
*******************************************************************************************/ 				
void status_ShowStatus(INT32U flag)
{   
#ifdef IIC_Key_Board_LCD		
	
#if (defined (EPSON_LCD))
	INT8U i, j=1, busy; 
	flag &= ~CMD_CANCEL_REQ;
	switch (flag & ~STATUS_WAR)
	{
	case STATUS_SVC:
		for (i = 0; i < STACK_LEN; i++)
		{
			if (svc_stack[i] != 0)
			{
#ifdef LCD_MODULE_ZX19264M1A
				LCD_SetDisStatus(E_ST_SVC, svc_stack[i]&0xFF, 0, (void *)svc_message[svc_stack[i]&0xFF]);
#else
				LCD_SetDisStatus(E_ST_SVC, svc_stack[i]&0xFF, 0, 0);
#endif
				break;
			}
		}
		break;
	case STATUS_FTA:
		for (i = 0; i < STACK_LEN; i++)
		{
			if (fta_stack[i] != 0)
			{
#ifdef LCD_MODULE_ZX19264M1A
				LCD_SetDisStatus(E_ST_FTA, fta_stack[i]&0xFF, 0, (void *)fta_message[fta_stack[i]&0xFF]);
#else
				LCD_SetDisStatus(E_ST_FTA, fta_stack[i]&0xFF, 0, 0);
#endif
				break;
			}
		}
		break;
	case STATUS_ERR:
		for (i = 0; i < STACK_LEN; i++)
		{
			if (err_stack[i] != 0)
			{
#ifdef LCD_MODULE_ZX19264M1A
				LCD_SetDisStatus(E_ST_ERR, err_stack[i]&0xFF, 0, (void *)err_message[err_stack[i]&0xFF]);
#else
				LCD_SetDisStatus(E_ST_ERR, err_stack[i]&0xFF, 0, 0);
#endif
				break;
			}
		}
		break;
	case STATUS_ERR_AUTO:
		for (i = 0; i < STACK_LEN; i++)
		{
			if (err_stack_auto[i] != 0)
			{
				LCD_SetDisStatus(E_ST_ERR_AUTO, err_stack_auto[i]&0xFF, 0, 0);
				break;
			}
		}
		break;
	case STATUS_INI:
		for (i = 0; i < STACK_LEN; i++)
		{
			if (ini_stack[i] != 0)
			{
				LCD_SetDisStatusEx(E_ST_INI, ini_stack[i]&0xFF, 0, 0);
				break;
			}
		}
		break;
	default:
		//case STATUS_WAR:
		if(flag & STATUS_WAR)
		{
			for (i = 0; i < STACK_LEN; i++)
			{
				if (war_stack[i] != 0)
				{
					LCD_SetDisStatusEx(E_ST_INI, war_stack[i]&0xFF, 0, 0);
					break;
				}
			}
		}
		break;
	case STATUS_RDY:
#ifdef FUNC_MAINTAIN
		if(bStatus_Maintain & 0x01)
		{
			busy = 0;
			LCD_SetMainInfoEx(E_IT_Busy, &busy);
			LCD_SetDisStatusEx(E_ST_MAINTAIN, 0, 0, 0);
			break;
		}
#endif
		busy = 0;
		LCD_SetMainInfoEx(E_IT_Busy, &busy);
		LCD_SetDisStatusEx(E_ST_RDY, 0, 0, 0);
		break;
	case STATUS_MOVING:
		busy = 1;
		LCD_SetMainInfoEx(E_IT_Busy, &busy);
		LCD_SetDisStatusEx(E_ST_MOVING, 0, 0, 0);
		break;
	case STATUS_PRINT:
		busy = 1;
		LCD_SetMainInfoEx(E_IT_Busy, &busy);
		LCD_SetDisStatusEx(E_ST_PRINT, 0, status_EP0Buf.percent, 0);
		break;
	case STATUS_PAUSE:
		busy = 0;
		LCD_SetMainInfoEx(E_IT_Busy, &busy);
		LCD_SetDisStatusEx(E_ST_PAUSE, 0, 0, 0);
		break;
	case STATUS_CLEANING:
		busy = 1;
		LCD_SetMainInfoEx(E_IT_Busy, &busy);
		LCD_SetDisStatusEx(E_ST_CLEANING, 0, 0, 0);
		break;
	case 0x10000000:
		busy = 1;
		LCD_SetMainInfoEx(E_IT_Busy, &busy);
		LCD_SetDisStatusEx(E_ST_CANCELING, 0, 0, 0);
		break;
	case STATUS_UPDATING:
		busy = 1;
		LCD_SetMainInfoEx(E_IT_Busy, &busy);
		LCD_SetDisStatusEx(E_ST_UPDATING, 0, status_EP0Buf.percent, 0);
		break;
	}
#else
	INT8U i,j=1; 
	flag &= ~CMD_CANCEL_REQ;
	switch (flag)
	{
	case STATUS_SVC:
		LCD_Text(0,0,main_message[0]);
		for (i = 0; i < STACK_LEN; i++)
		{
			if (svc_stack[i] != 0)
			{					
				LCD_Text(0,2*j,svc_message[svc_stack[i]&0xFF]);
				if (j++ > 3)
					break;
			}
		}
		for (;j<4;j++)
			LCD_Text(0,2*j,empty);
		break;
	case STATUS_FTA:
		LCD_Text(0,0,main_message[1]);
		for (i = 0; i < STACK_LEN; i++)
		{
			if (fta_stack[i] != 0)
			{
				LCD_Text(0,2*j,fta_message[fta_stack[i]&0xFF]);
				if (j++>3)
					break;
			}
		}
		for (;j<4;j++)
			LCD_Text(0,2*j,empty);
		break;
	case STATUS_ERR:
		LCD_Text(0,0,main_message[2]);
		for (i = 0; i < STACK_LEN; i++)
		{
			if (err_stack[i] != 0)
			{
				LCD_Text(0,2*j,err_message[err_stack[i]&0xFF]);
				if (j++>3)
					break;
			}
		}
		break;
	case STATUS_INI:
		LCD_Text(0,0,main_message[3]);
		for (i = 0; i < STACK_LEN; i++)
		{
			if (ini_stack[i] != 0)
			{
				LCD_Text(0,2*j,ini_message[ini_stack[i]&0xFF]);
				if (j++>3)
					break;
			}
		}
		for (;j<4;j++)
			LCD_Text(0,2*j,empty);
		break;
	case STATUS_RDY:
#ifdef FUNC_MAINTAIN
		if(bStatus_Maintain & 0x01)
		{
			LCD_Text(0,0,"Maintain...    ");
			for (j = 1; j<4;j++)
				LCD_Text(0,2*j,empty);
			break;
		}
#endif
		LCD_Text(0,0,"Ready           ");
		for (j = 1; j<4;j++)
			LCD_Text(0,2*j,empty);
		break;
	case STATUS_MOVING:
		LCD_Text(0,0,"Moving...       ");
		break;
	case STATUS_PRINT:
		LCD_Text(0,0,"Printing...     ");
		break;
	case STATUS_PAUSE:
		LCD_Text(0,0,"Pause           ");
		break;
	case STATUS_CLEANING:
		LCD_Text(0,0,"Cleaning...     ");
		break;			
	case 0x10000000:
		LCD_Text(0,0,"Canceling...    ");
		break;		
	}
#endif        
#endif	
	
#ifdef MANUFACTURER_DYSS
	if(flag&(STATUS_SVC|STATUS_FTA|STATUS_ERR|STATUS_ERR_AUTO|STATUS_WAR))
		changeLeddisplay(1);
#endif

#if (defined(MANUFACTURER_SAIBO)||defined(MANUFACTURER_SAIBO_ROLL_UV))&&defined(OLD_UV)
	if(flag&(STATUS_SVC|STATUS_FTA|STATUS_ERR|STATUS_ERR_AUTO|STATUS_WAR))
	{
		UV_CTR_ERR_STATUS_ON();
	}
	else
	{
		UV_CTR_ERR_STATUS_OFF();
	}
	
	if((flag &(STATUS_MOVING|STATUS_RDY|STATUS_PAUSE|STATUS_CLEANING|STATUS_NO_CANCEL)) ||
			STATUS_RDY == flag)
	{
		UV_CTR_UNPRT_STATUS_ON();
		UV_CTR_PRT_STATUS_OFF();
	}
	else if(flag & STATUS_PRINT)
	{
		UV_CTR_PRT_STATUS_ON();
		UV_CTR_UNPRT_STATUS_OFF();
	}

#endif
}

void status_Init(void)
{
	INT8U err;
	
	FlushStack(war_stack);
	FlushStack(err_stack);
	FlushStack(err_stack_auto);
	FlushStack(ini_stack);
	FlushStack(fta_stack);
	FlushStack(svc_stack);
	FlushStack(update_stack);
	status_Old_Flags = STATUS_CHANGE;
	PushStatus(ini_stack,STATUS_INI_DSP);
	PushStatus(ini_stack,STATUS_INI_HEADBOARD); 
	PushStatus(ini_stack,STATUS_INI_ARM); 
#ifdef SPECTRA_HVB	
	PushStatus(ini_stack,STATUS_INI_HVB); //For spectra
#endif	
	status_FLAG_GRP = OSFlagCreate(STATUS_INI|STATUS_NO_CANCEL|STATUS_CHANGE, &err);
	
	status_EP0Buf.flag = 0;
	status_EP0Buf.percent = 0;
	
	status_EP0Buf.status = STATUS_INI;				
	CopyStatus(ini_stack, &status_EP0Buf);
}

INT8U status_IsPrinting(void)
{
	INT8U err;
	OSFlagAccept(status_FLAG_GRP, STATUS_PRINT,OS_FLAG_WAIT_SET_ALL,&err);
	if (err == OS_NO_ERR)
		return True;
	else
		return False;
}

void CopyStatus(INT32U *stack, status_EP0BUF_TYPE *dst)
{
	INT8U i = 0;
	INT8U num = 0;
	//	OS_CPU_SR  cpu_sr;
	
	//	OS_ENTER_CRITICAL();	
#if defined(DEBUG_Y_ENCODER)
	for (i = 0; i < STACK_LEN - 5; i++)
#else
		for (i = 0; i < STACK_LEN; i++)
#endif	
		{
			if (stack[i] != 0)
				dst->statusCode[num++] = stack[i];
#if ((MANUFACTURER_NAME == GZ) || (MANUFACTURER_NAME == GZ_UV))			
			break; //20090917 in GZ fix report printing error bug, memory speed
#endif		
		}
	dst->statusCodeNum = num;
	//	OS_EXIT_CRITICAL();
}

void status_Task(void* data)
{
	INT8U err;
	OS_CPU_SR  cpu_sr;	
	
	status_ShowStatus(STATUS_INI);	
#if defined( IIC_Key_Board_LCD) && (defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4))
	LCD_SetMainInfoEx(E_IT_ManufactureName, factoryDataEx.ManufacturerName);
	LCD_SetMainInfoEx(E_IT_PrinterName, factoryDataEx.PrinterName);
#endif
	
	for (;;)
	{
		OSFlagPend(status_FLAG_GRP, STATUS_CHANGE,OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME,0,&err);
		//status_Flags = (OSFlagQuery(status_FLAG_GRP, &err) & STATUS_NO_PRINT_BITS);
		status_Flags = OSFlagQuery(status_FLAG_GRP, &err);
		
		status_Flags &= ~STATUS_CHANGE;
		
		if (status_Flags&STATUS_UPDATING)
		{
			OS_ENTER_CRITICAL();
			status_EP0Buf.status = STATUS_UPDATING;
			status_EP0Buf.statusCode[0] = update_stack[0]; 
			status_EP0Buf.statusCodeNum = 1;
			OS_EXIT_CRITICAL();							
			status_ShowStatus(STATUS_UPDATING);
		}				
		else if (status_Flags&STATUS_SVC)
		{
			OS_ENTER_CRITICAL();
			status_EP0Buf.status = STATUS_SVC;
			CopyStatus(svc_stack, &status_EP0Buf);
			OS_EXIT_CRITICAL();	
			status_ShowStatus(STATUS_SVC);
		}
		else if (status_Flags&STATUS_FTA)
		{
			OS_ENTER_CRITICAL();
			status_EP0Buf.status = STATUS_FTA;			
			CopyStatus(fta_stack, &status_EP0Buf);
			OS_EXIT_CRITICAL();
			status_ShowStatus(STATUS_FTA);
		}
		else if (status_Flags&STATUS_ERR)
		{
			OS_ENTER_CRITICAL();		
			status_EP0Buf.status = STATUS_ERR;			
			CopyStatus(err_stack, &status_EP0Buf);
			OS_EXIT_CRITICAL();			
			status_ShowStatus(STATUS_ERR);
		}
		else if (status_Flags&STATUS_ERR_AUTO)
		{
			OS_ENTER_CRITICAL();		
			status_EP0Buf.status = STATUS_ERR_AUTO;			
			CopyStatus(err_stack_auto, &status_EP0Buf);
			OS_EXIT_CRITICAL();			
			status_ShowStatus(STATUS_ERR_AUTO);
		}
		else if (status_Flags&STATUS_INI)
		{
			OS_ENTER_CRITICAL();	
			status_EP0Buf.status = STATUS_INI;				
			CopyStatus(ini_stack, &status_EP0Buf);
			OS_EXIT_CRITICAL();			
			status_ShowStatus(STATUS_INI);
		}
		else
		{
			OS_ENTER_CRITICAL();				
			if ( ((status_Flags & STATUS_NO_CANCEL) == 0) && (status_Flags&STATUS_PRINT) ) 
				status_EP0Buf.status = 0x10000000; //Canceling				
			else if (status_Flags & STATUS_CLEANING)
				status_EP0Buf.status = STATUS_CLEANING; 
			else if (status_Flags & STATUS_PAUSE)
			{
				if (status_Flags & STATUS_MOVING)  
					status_EP0Buf.status = STATUS_MOVING; 
				else
					status_EP0Buf.status = STATUS_PAUSE; 
			}
			else if (status_Flags & STATUS_PRINT)
				status_EP0Buf.status = STATUS_PRINT;
			else if (status_Flags & STATUS_MOVING)
				status_EP0Buf.status = STATUS_MOVING;
			else
				status_EP0Buf.status = STATUS_RDY;
#if 1				
			if (status_Flags&STATUS_WAR)
			{
				status_EP0Buf.status |= STATUS_WAR;
				CopyStatus(war_stack, &status_EP0Buf);
			}
			else
#endif			
				status_EP0Buf.statusCodeNum = 0;
			
			if (status_Flags&CMD_CANCEL_REQ) //Inform host to cancel job, send cancel command
				status_EP0Buf.status |= CMD_CANCEL_REQ;
			
			OS_EXIT_CRITICAL();						
			status_ShowStatus(status_EP0Buf.status);			
		}
	}
}	

#include <stdlib.h>
#include "os_includes.h"
#include "consol.h"
#include "usb.h"
#include "usb_descriptors.h"
#include "HugeBuf.h"
#include "status.h"
#include "miscellance.h"
#include "global.h"
#include "status.h"
#include <string.h>
#include "../LCD_GUI/resources.h"
#include "uv.h"

#ifdef FUNCTION_CLOSE_LCD
#include "../LCD_GUI/resources.h"
#endif

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#include "display.h"
#include "calibration.h"
#include "job.h"
#include "commpipe.h"

extern struct Struct_JobStart job_info;
#endif

#include <old_apis.h>
#include <stdio.h>

#define DEBUG_NOT_SET_PM_CALI

#include "onewire.h"
#include "motion.h"
#include "FPGA.h"
extern struct EPR_MOTION_PARAM motionParam;
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
struct USB_RPT_MainUI_Param mainUIPara;
struct USB_RPT_Media_Info mediaInfo;
struct USB_Print_Quality printQuality;
INT8U mainUI_dirty = False;
INT8U mediaInfo_dirty = False;
INT8U printQuality_dirty = False;
INT8U bFactoryDataDirty = False;
INT8U printratio_dirty = False;
INT8U MaxGroupNumber_is_group_num = True;
struct USB_RPT_MainUI_Param mainUIPara_ToPM;
struct USB_RPT_Media_Info mediaInfo_ToPM;
struct USB_Print_Quality printQuality_ToPM;
#endif

#ifdef DYSS_UV_CLEAN
float DYSS_MEASURE_HIGH_VALUE = 0;
INT8U MEASURE_HIGH_SUCCSE = 0;
#endif
#if defined(MANUFACTURER_DYSS)
extern INT32S LAST_PRT_Y_POSI;
#endif
INT8U FPGA_Init(void); //If FPGA configuration failed, please do not call FPGA Init
/* external variables */

extern const unsigned char desctbl[];//Descriptor
//extern UBYTE paper_w_type;
extern INT16U uart_waittime[2];
extern volatile ElapseTimeType elapsedTime;
extern INT8U nextband_autoClean;
extern INT8U nextband_autoSpray;

extern OS_EVENT *MenuSem; //Sem to lock LCD menu operate

extern INT8U TestHeaterEnable[] ;
extern float AverResistance[];

/* global variables */
//static INT8U setupBuff[8];		/* setup transaction buffer */
//static INT8U setupBuffBak[8];
static USBGenericRequest setupRequest_ForTask FAST_DATA_SECTION, setupRequest_ForRead FAST_DATA_SECTION;
static volatile INT8U Read_SX2reg_Value;  //

volatile INT8U valFlag_GetPos = 0;  //Value Flag
extern volatile INT8U valueFlag_FactoryData_Not_Init;
extern volatile INT8U valueFlag_BYHXData_Not_Init;
extern volatile INT8U motionInitS2;
extern INT8U MediaType;
volatile INT8U EP0_STOP = False;
volatile INT8U USB_INSERT = False;
static volatile INT8S readFlag = False;			/* shared between interrupt and register read routines */
static volatile INT8S setupDat = False;
static INT8U EP0INdata_ForINT[64] FAST_DATA_SECTION, EP0INdata_ForTask[64] FAST_DATA_SECTION;
static INT8U EP0OUTdata_ForINT[64] FAST_DATA_SECTION, 
	     EP0OUTdata_ForRead[64] FAST_DATA_SECTION, EP0OUTdata_ForTask[64] FAST_DATA_SECTION;

static CheckBoardType checkboarddata;
INT8U calierrtype = 0;

/// USB standard device driver.
//XXX USBDDriver usbdDriver FAST_DATA_SECTION;

//#define DEBUG

#define DMA1EP2OUT 1
#define DMA1EP6IN  0
char DMA1Usage = DMA1EP2OUT;

INT8U vol_CalHeadVoltage(INT8U headNum);
void security_getPassword(INT8U *strPWD, INT8U mID, INT16U bID, INT8U hType, INT16U period, INT16U reserveValue);
INT8U security_CheckSWUILanguageSetting(INT8U language, INT8U *str, INT8U isEP0);

INT8U vol_MapHeadNumber(INT8U hd_HeadNum, INT8U isTamparature, INT8U isHeater, INT8U isHeaterThermistorTemp, INT8U isPulseWidth);

void des(unsigned char *source,unsigned char * dest,unsigned char * inkey, int flg);

void USBDMA_EP2_DoneInterrupt(void) 
{	
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();	
	TickAfterLastComm = 0;
	HugeBuf_CompleteOneReceive();

	HugeBuf_StartOneReceive();
	OS_EXIT_CRITICAL();
}

#if 0
void USBDMA_EP6_DoneInterrupt(void) 
{	
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();	
	HugeBuf_CompleteOneSendEP6();

	HugeBuf_StartOneSendEP6();			
	OS_EXIT_CRITICAL();
}
#endif



/// VBus pin instance.
static const Pin pinVbus = {
	1 << 14,
	DW_BASE_PIOB,
	DW_ID_PIOB,
	PIO_INPUT,
	PIO_DEFAULT
};

//------------------------------------------------------------------------------
/// Handles interrupts coming from PIO controllers.
//------------------------------------------------------------------------------
static void ISR_Vbus(const Pin *pPin)
{
	// Check current level on VBus
	if (PIO_Get(&pinVbus)) {

		//XXX TRACE_INFO("VBUS conn\n\r");
		USBD_Connect();
	}
	else {

		//XXX TRACE_INFO("VBUS discon\n\r");
		TickAfterLastComm = MAX_TICK_INTO_OFFLINE;
		USBD_AbortDataWrite(USB_IN_EP);
		USBD_Disconnect();
	}
}

//------------------------------------------------------------------------------
/// Configures the VBus pin to trigger an interrupt when the level on that pin
/// changes.
//------------------------------------------------------------------------------
static void VBus_Configure( void )
{
	TRACE_INFO("VBus configuration\n\r");

	// Configure PIO
	PIO_Configure(&pinVbus, 1);
	PIO_ConfigureIt(&pinVbus, ISR_Vbus);
	PIO_EnableIt(&pinVbus);

	// Check current level on VBus
	if (PIO_Get(&pinVbus)) {

		// if VBUS present, force the connect
		TRACE_INFO("VBUS conn\n\r");
		USBD_Connect();
	}
	else {
		TRACE_INFO("VBUS discon\n\r"); 
		TickAfterLastComm = MAX_TICK_INTO_OFFLINE;
		USBD_AbortDataWrite(USB_IN_EP);
		USBD_Disconnect();
	}           
}

#define USB_SETUP_DAT_READ  1
//unsigned char InsideSetupDat = FALSE;
static INT8U IsSafeCommand(const USBGenericRequest *request);
static void HandleEP0Command(const USBGenericRequest *request, INT8U *  EP0INdata, INT8U * EP0OUTdata);
static INT8U EP0CommandProc(const USBGenericRequest *request, INT8U direction, INT8U *len, INT8U *  EP0INdata, INT8U * EP0OUTdata);

void USBReadTransferCallback(void *pArg,
		unsigned char status,
		unsigned int transferred,
		unsigned int remaining) 
{
	INT8U err;
	if((INT32U)pArg == USB_SETUP_DAT_READ)
	{
		if(status == USBD_STATUS_SUCCESS)
		{
			if(IsSafeCommand(&setupRequest_ForRead))
			{
				HandleEP0Command(&setupRequest_ForRead, EP0INdata_ForINT, EP0OUTdata_ForRead);
			}
			else if (OSFlagAccept(mix_FLAG_GRP, USB_SETUP_DAT, OS_FLAG_WAIT_SET_ANY,&err))
				USBD_Stall(0);
			else
			{
				memcpy(&setupRequest_ForTask, &setupRequest_ForRead, sizeof(setupRequest_ForRead));
				memcpy(EP0OUTdata_ForTask, EP0OUTdata_ForRead, USBGenericRequest_GetLength(&setupRequest_ForRead));
				OSFlagPost(mix_FLAG_GRP, USB_SETUP_DAT,OS_FLAG_SET, &err);
			}
		}
		else
			USBD_Stall(0);
		OSFlagPost(mix_FLAG_GRP, USB_READ_SETUP_DAT,OS_FLAG_CLR, &err);
	}
}

void HandleEP0Command(const USBGenericRequest *request, INT8U *  EP0INdata, INT8U * EP0OUTdata) 
{
	INT8U i = 0,err,IS_USB1 = False;
	INT8U setupdirection = USBGenericRequest_GetDirection(request);
	INT16U setuplength = USBGenericRequest_GetLength(request); 
	if(request->bRequest == 0x45 && USB_INSERT == True)
	{	
		if(EP0_STOP || USBD_IsHighSpeed() == 0)
		{
			return;
		}

		if(status_EP0Buf.status== STATUS_FTA)
		{
			i=0;
			while(status_EP0Buf.statusCode[i] !=STATUS_FTA_USB1_USB1CONNECT)
			{
				i++;
				if(i == 10)
					break;
			}
			if(i!=10)
			{
				IS_USB1 = True;
			}
			else
				IS_USB1 = False;
		}
		else
			IS_USB1 = False;

		if(USBD_IsHighSpeed() != 0 && IS_USB1 == True)
		{
			status_EP0Buf.status = 0;			
			status_EP0Buf.statusCodeNum = 0;
		}
	}
	if (EP0CommandProc(request, setupdirection, (INT8U *)&setuplength, EP0INdata, EP0OUTdata))
	{
		if(setupdirection == USBGenericRequest_OUT)
		{
			// Acknowledge the request
			USBD_Write(0, // Endpoint #0
					0, // No data buffer
					0, // No data buffer
					(TransferCallback) 0,
					(void *)  0);
		}
		else 
		{
			USBD_Write(0,
					EP0INdata, 
					setuplength, 
					(TransferCallback) 0,
					(void *)  0);
		}
	}
	else
	{
		if(USBGenericRequest_GetLength(request) ==0 || setupdirection == USBGenericRequest_OUT)
		{
			//for Compatible with the old version,
			//ack this request, else the host will report communicaction error.
			// Acknowledge the request
			USBD_Write(0, // Endpoint #0
					0, // No data buffer
					0, // No data buffer
					(TransferCallback) 0,
					(void *)  0);
		}
		else
			USBD_Stall(0);
	}

}

//------------------------------------------------------------------------------
/// Invoked whenever a SETUP request is received from the host. Forwards the
/// request to the standard handler.
//------------------------------------------------------------------------------
void USBDCallbacks_RequestReceived(const USBGenericRequest *request) 
{
	INT8U err;
	static INT8U configed = False;

	if(USBGenericRequest_GetType(request) == USBGenericRequest_STANDARD)
	{
		//USBDDriver_RequestHandler(&usbdDriver, request);
		if(USBD_GetState() >= USBD_STATE_CONFIGURED)
		{
			if(configed == False)
				OSFlagPost(mix_FLAG_GRP, USB_CONFIGURED,OS_FLAG_SET, &err);
			configed = True;
		}
		else
		{
			configed = False;
			OSFlagPost(mix_FLAG_GRP, USB_CONFIGURED,OS_FLAG_CLR, &err);
		}
	}
	else if(USBGenericRequest_GetType(request) == USBGenericRequest_VENDOR)
	{
		//cheney : we assert that Host can't send the second un-safe command before ARM handle over the first unsafe command.
		INT8U setupdirection = USBGenericRequest_GetDirection(request);
		INT16U setuplength = USBGenericRequest_GetLength(request); 

		TickAfterLastComm = 0;
		//    INT8U err;
		if(setupdirection == USBGenericRequest_OUT && setuplength != 0)
		{
			if (OSFlagAccept(mix_FLAG_GRP, USB_READ_SETUP_DAT, OS_FLAG_WAIT_SET_ANY,&err))
				USBD_Stall(0);
			else
			{
				memcpy(&setupRequest_ForRead, request, sizeof(setupRequest_ForRead));
				OSFlagPost(mix_FLAG_GRP, USB_READ_SETUP_DAT, OS_FLAG_SET, &err);
				USBD_Read(0, EP0OUTdata_ForRead, setuplength, USBReadTransferCallback, (void*)USB_SETUP_DAT_READ);
			}
		}
		else 
		{
			if(IsSafeCommand(request))
			{
				HandleEP0Command(request, EP0INdata_ForINT, EP0OUTdata_ForINT);
			}
			else
			{
				if (OSFlagAccept(mix_FLAG_GRP, USB_SETUP_DAT, OS_FLAG_WAIT_SET_ANY,&err))
					USBD_Stall(0);
				else
				{
					memcpy(&setupRequest_ForTask, request, sizeof(setupRequest_ForTask));
					OSFlagPost(mix_FLAG_GRP, USB_SETUP_DAT,OS_FLAG_SET, &err);
				}
			}
		}
	}
	else
	{
		USBD_Stall(0);
	}
}

#if 0
INT8U USB_Init(void)
{
	OS_CPU_SR  cpu_sr;
	//	INT8U err;

	readFlag 		= False;	/* false until register read */

	OS_ENTER_CRITICAL();

	// USB initialization, Disable Pull-up
	TRACE_INFO("USB initialization\n\r");
	USBDDriver_Initialize(&usbdDriver, &usbdDriverDescriptors, 0);
	USBD_Init();

	// Device connection
	TRACE_INFO("Connecting device\n\r");

	// Enable Vbus detect, connect if needed (vbus detected)
	VBus_Configure();

	OS_EXIT_CRITICAL();  


	return True;
}
#endif

//handle safe cmd in USB INT.
INT8U IsSafeCommand(const USBGenericRequest *request) 
{
	INT8U ret = False;
	INT8U reqCode = request->bRequest;

	INT16U index = request->wIndex;  
	INT16S value = request->wValue;
	INT8U direction = USBGenericRequest_GetDirection(request);

	switch (reqCode) 
	{
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	case 0x7F:
		if (direction) //All EP0 command must have ep0 buffer, 
		{
			if(value == 3 ||value == 15  ||value == 16||value == 28)
				ret = False;
			else
				ret = True;
			break;
		}
		else
		{
			if((index == 2 && value == 0) || value == 3||value == 15||value == 16 ||value == 17 ||value == 21||value == 24||value == 30||
					value == 31||value == 34||value == 35||value == 36||value == 38 || value == 40 || value == 41)
			{
				ret = False;
				break;
			}			
			else if(value == 0)
			{
				ret = True;
				break;
			}
			else if(value == 1)
			{
				ret = False;
				break;
			}
			else if(value == 2)
			{
				ret = False;
				break;
			}
			else if(value == 4 || value == 5)
			{
				ret = False;
				break;
			}
			else if(value == 6 || value == 7)
			{
				ret = False;
				break;
			}
		}
		break;
#endif
#if defined(SUPPORT_MOTOR_CONTROL)
	case 0x7E:
		if (!direction)
		{
#if defined(RECORD_MOTION_MOVEMENT)
			if(value == 3)
				ret = False;
			else
#endif
				ret = True;
		}
		else
			ret = True;
		break;
#endif
		//Process vendor request without EP0 data here
	case 0x20:
		switch (index) //Index
		{
		case 0x14: //SprayAuto
		case 0x15: //AutoSuckHead, auto clean
		case 0x16: //Enter single clean mode
		case 0x17: //Exit single clean mode
			ret = False;
			break;
		case 0x19: //Single Clean, [3] is index
			ret = True;
			break;
		case 0x20: //Start move, not used ?????????????????
		case 0x21: //Stop move
		case 0x22: //Go home
		case 0x25: //Go home. All axes, 1 X 2 Y 3 Z, can be bit or
		case 0x63: //Clear Updating Status.
			ret = False;
			break;
		case 0x65: //Set motion uart resend interval
			ret = True;
			break;
		case 0x66: //Clear Factory Data
		case 0x67: //Clear error, value is the error number
		case 0x30: //Pause
		case 0x31: //Resume
		case 0x32: //Cancel
		case 0x23: //Measure paper, support from 20090814
			ret = False;
			break;
		case 0x40: //Reset board
		case 0x18: //Close cleaner, don't support now
		default:
			ret = True;
			break;
		}
		break;
	case 0x42: //Report printer physical size
	case 0x43: //IN, Get the home position
	case 0x44:	//IN, get media edge
	case 0x45: //IN, Get status
		ret = True;
		break;
	case 0x46: //IN Get the current position
		ret = False;
		break;
	case 0x50: //IN, Get time password
	case 0x51: //IN, Get option password
		ret = True;
		break;
	case 0x73: //IN/OUT, Get/Set Ink password
		if(direction == 0) //Set,OUT
			return False;
		else
			return True;
	case 0x52:  //Get the elapse
	case 0x54: //IN, Get board info
	case 0x55: //IN, Get saved parameters
	case 0x56: //IN, get hand shake result
	case 0x58: //IN Get saved factory data
		ret = True;
		break;
	case 0x5A: //OUT, Set Factory data
		ret = False;
		break;
	case 0x5B: //IN, Get supported printer list.
		ret = True;
		break;
		// For Konica,  Index: 5 temperature,0: current voltage 1: Pulese, 2: Adjust Voltage 3: Base Voltage                6: Heat target temprature
	case 0x5C: 
		switch (index) 
		{
		case 0: //Get current voltage
			ret = True;
			break;
		case 1: //
		case 2://2: Manual Voltage, we change it to adjust voltage
		case 3:
			if (direction == 0) //Set, auto control,
				{
					ret = False;
				}
			else //Get
			{
				ret = True;
			}
			break;
		case 4:
		case 5: //temperature
			ret = True;
			break;

			//EPR_HEAT_TEMPRATURE_DATA_OFFSET
#ifdef SUPPORT_HEAD_HEATER				
		case 6: //heater target temperature
			if (direction == 0) //Set, if the value is 0xFF, printhead will use default value
			{
				ret = False;
			}
			else //Get
			{
				ret = True;
			}
			break;
		case 9: //heater target temperature
			if (direction == 0) //Set, if the value is 0xFF, printhead will use default value
			{
				ret = False;
			}
			else //Get
			{
				ret = True;
			}
			break;					
#endif	
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
		case 8: //epson head parameter percent
			if (direction == 0) //Set
			{
				ret = False;
			}
			else //Get
			{
				ret = True;
			}
			break;
#endif                    
		case 7: //temperature
		default:
			ret = True;
			break;
		}
		break;

		// 0x6E, For XAAR382, Index: 0: PWM(RO)   1: Vtrim(RW),  5 temperature(RO)
		// 2: DualBank(RW)     3: WVFM Select(WO)  4 HeadInfo(RO)
		// 对于2，3，4， value为喷头编号, 一次只读写一个
#ifdef HEAD_XAAR382		
	case 0x6E:
		switch (index) 
		{
		case 0: //Get the PWM values
			ret = True;
			break;
		case 1: //Vtrim
			if (direction == 0) //Set
			{
				ret = False;
			}
			else //Get
			{
				ret = True;
			}
			break;
		case 2://2:DualBank = 0
			if (direction == 0)
			{
				ret = False;
			}
			else //Get
			{
				ret = True;
			}
			break;
		case 3: //WVFM Select
			if (direction == 0) //Set, auto control,
				{
					ret = False;
				}
			else //Get
				ret = True;
			break;
		case 4: //HeadInfo
		case 5: //temperature
		default:
			ret = True;
			break;

		}
		break;
#endif			
	case 0x5D: //Get set BYHX data, this command is only for BYHX Tools
		if (direction) //Get
		{
			ret = True;
		}
#if defined( BYHX_WTITE_BOARD_TOOL ) || defined (ek_board_sim)
		else // == 0 Set
		{
			ret = False;
		}
#endif			
		break;

	case 0x5E: //Get head board parameter
		ret = True;
		break;
#ifdef VENDOR_PASSWORD_TOOL
	case 0x5F: //IN, Generate Time Limited password
		ret = False;
		break;
	case 0x6A: //IN, Generate Language password
		ret = False;
		break;
	case 0x74: //IN, Generate INK password
		ret = False;
		break;
#endif
	case 0x60: //OUT, 移动小车及进退纸
		ret = False;
		break;
#ifndef BYHX_WTITE_BOARD_TOOL			
	case 0x61://OUT, set time limited password
		ret = False;
		break;
	case 0x69://OUT, set option password: language
		if (direction == 0)
		{	
			ret = False;
		}
		else 
			ret = True;
		break;
#endif	
	case 0x62://OUT, handshake
		ret = False;
		break;
	case 0x63: 
	case 0x64: //OUT, Set clean para
		ret = False;
		break;
	case 0x65: //OUT, Save parameters, used for control panel, should saved to NAND Flash
		ret = False;
		break;
	case 0x66: //仅为调试，转发COM命令，value为端口号
		if (direction) //Get
		{
			ret = True;
		}	
		else
			ret = False;
		break;

	case 0x67: //OUT, 移动小车及进退纸到某一个位置
		ret = False;
		break;	
#ifdef UV_PRINTER			
	case 0x68: //设置/查询UV灯
		ret = False;
		break;
#endif
#ifndef BYHX_WTITE_BOARD_TOOL
	case 0x6B: //IN: 当前语言查询  Out 语言设置检查 value == language
		if (direction) //
		{
			ret = True;
		}else
		{
			ret = False;
		} 
		break;	
#endif	

		//#ifdef Y_ENCODER_ENABLE
	case 0x6C: //Get following Info (1) int32 bandIndex, (2)  unsigned int m_nBandY (3)unsigned int m_nBandResPosY (4)int32 nEncoderPos,
	case 0x6D:
		ret = True;
		break;	
	case 0x70: //Set standard voltage: float: H0 L0 H1 L1
		ret = False;
		break;	
	case 0x71: //Get DSP private data
		ret = True;
		break;	
	case 0x72: //None data setup
		ret = False;
		break;	
#if 1			
	case 0x76:
		ret = False;
		break;	
		break;
#endif	    	
		//Process vendor request with EP0 data here. Data stored in	EP0data[i]
	default: //We don't recognize the request
		ret = True;
		break;	
	}
	return ret;

}

#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_INTEGRATE_3)
static CleanParaType_EPSON_MICOLOR_USB cache;
#endif

#ifdef DYSS_UV_CLEAN
extern INT8U DYSS_CLEAN_FLASH;
#endif

INT8U EP0CommandProc(const USBGenericRequest *request, INT8U direction, INT8U *len, INT8U *  EP0INdata, INT8U * EP0OUTdata) //If return False, will stall EP0
{	
	INT8U err;
	INT8U reqCode = request->bRequest;

	INT16U index = request->wIndex;  
	INT16S value = request->wValue;

	INT16U setuplength = request->wLength;

	INT8U ret = True;
	INT16U i;
	INT8U tmp[32];
	INT8U epromlen;
	INT32U checksum = 0;
	INT32S tmpdata,tmp_index = 0;

	//INT8U headMapNum;

	switch (reqCode) 
	{
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	case 0x7F:
		if (direction) //All EP0 command must have ep0 buffer, 
		{
			if(value == 0)
			{
				i = headFPGADebugInfo[0]-2;
				if(i > sizeof(headFPGADebugInfo))
					i = sizeof(headFPGADebugInfo); 
				if( i > 64)
					i = 64; 
				memcpy(EP0INdata, (void*)(headFPGADebugInfo+2), i);
				bFPGADebugInfoUpdated = FALSE;
				*len = i;
			}
			else if(value == 1)
			{
				EP0INdata[0] = bReadHeadBoardParam;
				*len = 1;
			}
			else if(value == 2)
			{
				memcpy(EP0INdata, (void*)HBParamBuf, HBReadLen);
				*len = HBReadLen;
			}
#if defined (EPSON_CLEAN_UPDOWN)||defined (DYSS_UV_CLEAN)||defined (RICOH_CLEAN_PRESS)
			else if(value == 3 && (index &0xFF)== 0)
			{
				OutCleanParam_EPSON_Allwin((CleanParaType_EPSON_ALLWIN_USB *)EP0INdata,(index &0xFF00)>>8);
				*len = sizeof(CleanParaType_EPSON_ALLWIN_USB);
			}
#endif                
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
			else if(value == 3 && (index &0xFF) == 1)
			{
				OutCleanParam_EPSON_Micolor(&cache,(index &0xFF00)>>8);
				memcpy(EP0INdata, &cache, 64);
				*len = 64;
			}
			else if(value == 3 && (index &0xFF) == 2)
			{
				memcpy(EP0INdata, ((INT8U*)&cache + 64), sizeof(CleanParaType_EPSON_MICOLOR_USB)-64);
				*len = sizeof(CleanParaType_EPSON_MICOLOR_USB)-64;
			}
#endif                
			else if(value == 6) //IN, Get the saved factoryDataEx
			{
				if (valueFlag_FactoryData_Not_Init)
				{
					ret = False;
					break;
				}
				*len = factoryDataEx.len;
				if (*len > 64)
					*len = 64;
				for (i = 0; i < *len; i++) 
					EP0INdata[i] = ((__packed INT8U *)&factoryDataEx)[i];
				break;
			}
			else if(value == 7) //IN, Get the saved Calibration Config
			{
				*len = caliConfig.len;
				if (*len > 64)
					*len = 64;
				for (i = 0; i < *len; i++) 
					EP0INdata[i] = ((__packed INT8U *)&caliConfig)[i];
				break;
			}
			else if(value == 8) //IN, Get the saved Calibration setting with PM format
			{
				if (index >= sizeof(pmCaliSettingPMFormat))
					*len = 0;
				else if(index + setuplength > sizeof(pmCaliSettingPMFormat))
					*len = sizeof(pmCaliSettingPMFormat) - index;
				else
					*len = setuplength;
				if (*len > 64)
					*len = 64;
				//OSSemPend(PMCaliSettingSem, 0, &err);
				for (i = 0; i < *len; i++) 
					EP0INdata[i] = ((__packed INT8U *)&pmCaliSettingPMFormat + index)[i];
				//OSSemPost(PMCaliSettingSem);
				break;
			}
			else if(value == 9) //IN, Get Main UI items
			{
				memcpy(EP0INdata, &mainUIPara_ToPM, sizeof(mainUIPara_ToPM));
				*len = sizeof(mainUIPara_ToPM);
				break;
			}
			else if(value == 11) //In, Get media info items
			{
				memcpy(EP0INdata, &mediaInfo_ToPM, sizeof(mediaInfo_ToPM));
				*len = sizeof(mediaInfo_ToPM);
				break;
			}
			else if(value == 10) //IN, Get print quality items
			{
				memcpy(EP0INdata, &printQuality_ToPM, sizeof(printQuality_ToPM));
				*len = sizeof(printQuality_ToPM);
				break;
			}
			else if(value == 12) //IN, Get the saved Calibration setting with FW format
			{
				if (index >= sizeof(pmCaliSettingFWFormat))
					*len = 0;
				else if(index + setuplength > sizeof(pmCaliSettingFWFormat))
					*len = sizeof(pmCaliSettingFWFormat) - index;
				else
					*len = setuplength;
				if (*len > 64)
					*len = 64;
				//OSSemPend(PMCaliSettingSem, 0, &err);
				for (i = 0; i < *len; i++) 
					EP0INdata[i] = ((__packed INT8U *)&pmCaliSettingFWFormat + index)[i];
				//OSSemPost(PMCaliSettingSem);
				break;
			}
			else if(value == 13) //IN, Get the current language
			{
				EP0INdata[0] = LCDMenuConfig.LanguageIndex;
				*len = 1;
				break;
			}
			else if(value == 14) //IN, Get data channel map.
			{
				*len = GetDataChannelMap(EP0INdata,setuplength);
				break;
			}
			else if(value==15)
			{
				if(index == 0)
				{

					*len = GetCalibrationLength(EP0INdata);
					if(!(*len))
					{
						ret = False;
					}
					calierrtype=0;
					break;
				}
				else if(index == 2)
				{
					*(INT8U*)EP0INdata = calierrtype;
					*len = sizeof(INT8U);
					calierrtype = 0;
					break;
				}
			}

			else if(value == 16)//in,get epsonCaliSetting
			{
				if(calierrtype!=0)
				{
					break;
				}
				else
				{
					*len = InCaliData(EP0INdata,index,setuplength);	
					if(!(*len))
					{
						ret = False;
					}
					break;
				}
			}
			else if(value == 18)
			{
				*(FP32*)EP0INdata = LCDMenuConfig.MeasureSensorOffset;
				*len = sizeof(FP32);
				break;
			}
#ifdef COORD_NEW_UV		
			else if(value == 19)
			{
				memcpy(EP0INdata, &Ripstar_UV_Setting, sizeof(Ripstar_UV_Setting));
				*len = sizeof(Ripstar_UV_Setting);
				break;
			}
#endif	
			else if(value == 20)
			{
				EP0INdata[0] = 0x7f;
				EP0INdata[1] = 20;
				EP0INdata[2] = GetPhysicsHeadNum();
				*len = 3;
				break;	
			}
#ifdef DYSS_UV_CLEAN		
			else if(value == 21)
			{
				EP0INdata[0] = 0x7f;
				EP0INdata[1] = 21;
				EP0INdata[2] = MEASURE_HIGH_SUCCSE;
				*((__packed float*)&EP0INdata[3]) = DYSS_MEASURE_HIGH_VALUE;
				*len = 7;
				break;	
			}
			else if(value == 23)
			{
				EP0INdata[0] = 0x7f;
				EP0INdata[1] = 23;
				if(LCDMenuConfig.cfg_bit&DYSS_MEASURE_PROTECT_SWITCH)
					EP0INdata[2] = 1;
				else
					EP0INdata[2] = 0;
				*len = 3;
				break;	
			}		
#endif	
#ifdef WAVE_SET_FUN
			else if(value == 22)
			{
				if(index == 0)
					*(INT32U*)EP0INdata = DATA_LEN;
				else
				{	
					//WAVE_DATA_MAX_LEN
					memcpy((void*)EP0INdata, (void*)&WAVE_BUF[(index -1)*WAVE_DATA_MAX_LEN], WAVE_DATA_MAX_LEN);
				}
				break;
			}
#endif

#ifdef FUNCTION_CLOSE_LCD
			else if(value == 24)
			{
				memcpy((void *) EP0INdata, (void *)MenuStr_EM_4_ID1C8[index], MAX_HEAD_PARAM_NAME);
				break;
			}
#endif		
			else if(value == 25)
			{
				for(i = 0;i < 8;i++)
					EP0INdata[i] = g_headParameterPercent[i];
				break;
			}
#if defined (DGEN_AUTOFUCTION)
			else if(value == 26)
			{
				EP0INdata[0] =  LCDMenuConfig_EX.Color_Bar_Flash_On ;
				*((__packed float*)&EP0INdata[1]) = LCDMenuConfig_EX.Color_Bar_POSI; 
				break;
			}
#endif	
#ifdef MANUFACTURER_DYSS
			else if(value == 27)
			{
				*((__packed float*)&EP0INdata[0]) = LCDMenuConfig_EX.X_Plat_Correction;
				*((__packed float*)&EP0INdata[4]) = LCDMenuConfig_EX.Y_Plat_Correction;
				*len = 8;
				break;
			}
#endif

			else if(value == 28)
			{	
				*len = Read_Dsp_Data(EP0INdata,index,setuplength);
				break;
			}

			else if(value == 29)
			{	
				EP0INdata[0] = 0x7f;
				EP0INdata[1] = 29;
				EP0INdata[2] = Head_Board_Num;
				*len = 3;
				break;
			}
#ifdef FULGENCY_FUN
			else if(value == 30)
			{	
				EP0INdata[0] = 0x7f;
				EP0INdata[1] = 30;
				EP0INdata[2] = LCDMenuConfig_EX.Head_PUMP_Mask;
				*((__packed INT32U*)&EP0INdata[3]) = LCDMenuConfig_EX.Head_PUMP1_ON;
				*((__packed INT32U*)&EP0INdata[7]) = LCDMenuConfig_EX.Head_PUMP1_OFF;
				*((__packed INT32U*)&EP0INdata[11]) = LCDMenuConfig_EX.Head_PUMP2_ON;
				*((__packed INT32U*)&EP0INdata[15]) = LCDMenuConfig_EX.Head_PUMP2_OFF;
				*len = 19;
				break;
			}
#endif
#ifdef MANUFACTURER_DYSS
			else if(value == 31)
			{
				EP0INdata[0] = 0x7f;
				EP0INdata[1] = 31;
				*((__packed INT32U*)&EP0INdata[2])  = LCDMenuConfig_EX.b_Sundry;
				*len = 6;
				break;
			}
			else if(value == 32)
			{
				EP0INdata[0] = 0x7f;
				EP0INdata[1] = 32;
				*((__packed INT32S*)&EP0INdata[2])  = LAST_PRT_Y_POSI;
				*len = 6;
				break;
			}
#endif

#if (defined(TIMER_ON)&&defined(PUMP_INTERMITTENT))

			else if(value == 33)
			{
				EP0INdata[0] = 0x7f;
				EP0INdata[1] = 33;
				*((__packed INT16U*)&EP0INdata[2]) = LCDMenuConfig_EX.Interval_Pump_On_Time;
				*((__packed INT16U*)&EP0INdata[4]) = LCDMenuConfig_EX.Interval_Pump_Off_Time;
				*len = 6;
				break;
			}
#endif

#ifdef WAVE_SET_FUN
			else if(value == 34)		//读取波形名字命令
			{
				if(index == 0)
					*(INT32U*)EP0INdata = DATA_LEN;
				else
					memcpy((void*)EP0INdata, (void*)&WAVE_NAME_BUF[(index -1)*WAVE_DATA_MAX_LEN], WAVE_DATA_MAX_LEN);
				break;
			}
			else if(value == 35)		//读取通道名字
			{
				/*int dd = 0;
				  for(dd == 0; dd < MAX_HEAD_PARAM_NAME; dd++)
				  {
				  sprintf(MenuStr_EM_4_ID1C8[dd],"Head %c", 'A' + dd);
				  }*/
				if(index == 0)
					*(INT32U*)EP0INdata = MAX_HEAD_DRIVE_NUM;
				else
					memcpy((void*)EP0INdata, (void*)MenuStr_EM_4_ID1C8[index -1], MAX_HEAD_PARAM_NAME);
				break;
			}
			else if(value == 36)		//读取波形映射表
			{
				if(index == 0)
					*(INT32U*)EP0INdata = DATA_LEN;
				else
					memcpy((void*)EP0INdata, (void*)&WAVE_BUF[(index -1)*WAVE_DATA_MAX_LEN], WAVE_DATA_MAX_LEN);
			}

#endif
#ifdef FULGENCY_FUN
			else if(value == 37)
			{
				EP0INdata[0] = 0x7f;
				EP0INdata[1] = 37;
				*((__packed float*)&EP0INdata[2]) = LCDMenuConfig_EX.Z_Plat_Correction;
				*len = 6;
				break;
			}
#endif
		}
		else
		{
			if((index == 2 && value == 0) || value == 3)
			{
				StartCalibrationJob((struct CaliPrintSetting * )EP0OUTdata);
			}			
			else if(value == 0)
			{
				HBReadAddr = (EP0OUTdata[1]<<8) + EP0OUTdata[0];
				HBReadLen = (EP0OUTdata[3]<<8) + EP0OUTdata[2];
				if(HBReadLen > 22)
					HBReadLen = 22;
				bReadHeadBoardParam = 1;
			}
			else if(value == 1)
			{
				for (i = 0; i < setuplength; ) //send 16 byte each times.
				{
					INT8U j,k;
					INT16U addr = index;

					if(setuplength - i > 22)
						k = 22;
					else
						k = setuplength - i;
#ifdef CONVERSION_BOARD
					tmp[0] = k+6;
					tmp[k+5] = 1;
#else
					tmp[0] = k+5;
#endif
					tmp[1] = UART_CMD_EEPROM_WRITE;
					tmp[2] = addr>>8;
					tmp[3] = addr;
					tmp[4] = k;
					for (j = 0; j < k; j++)
					{
						tmp[5+j] = EP0OUTdata[i+j];
					}
					UART_SendCMD(UART_HEAD_CHANNEL, tmp);
					i+=k;
				}
			}
#if (defined (EPSON_CLEAN_UPDOWN))||defined (DYSS_UV_CLEAN)||defined (RICOH_CLEAN_PRESS)
			else if(value == 2 && (index &0xFF) == 0)
			{
				SetCleanParam_EPSON_Allwin((CleanParaType_EPSON_ALLWIN_USB *) EP0OUTdata,(index &0xFF00)>>8);
				ret = SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 0, sizeof(struct EPR_CleanParaEpsonAllwin));
				break;
			}
#endif
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
			else if(value == 2 && (index &0xFF) == 1)
			{
				memcpy(&cache, (CleanParaType_EPSON_MICOLOR_USB *) EP0OUTdata, 64);
				break;
			}
			else if(value == 2 && (index &0xFF) == 2)
			{
				memcpy( ((INT8U*)&cache + 64), EP0OUTdata, sizeof(CleanParaType_EPSON_MICOLOR_USB)-64);
				SetCleanParam_EPSON_Micolor(&cache,(index &0xFF00)>>8);
				ret = SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 0, sizeof(struct EPR_CleanParaEpsonMicolor));
				break;
			}
#endif
			else if(value == 4)
			{
				tmp[0] = 4;
				tmp[1] = UART_HEAD_EPSON_OUT_VOLTAGE;
				tmp[2] = EP0OUTdata[0];
				tmp[3] = EP0OUTdata[1];
				UART_SendCMD(UART_HEAD_CHANNEL, tmp);
			}
			else if(value == 5)
			{
				StartHWDynOffsetJob(EP0OUTdata[0], EP0OUTdata[1]);
			}
			else if(value == 6) //OUT, Set factoryDataEx
			{
				if( (((struct EPR_FactoryData_Ex*)EP0OUTdata)->len != 0x40 &&
							((struct EPR_FactoryData_Ex*)EP0OUTdata)->len != sizeof(EPR_FactoryDataExType)) ||
						(((struct EPR_FactoryData_Ex*)EP0OUTdata)->version != 1 &&
						 ((struct EPR_FactoryData_Ex*)EP0OUTdata)->version != 2))
				{
					ret = False;
					break;
				}

				epromlen = EP0OUTdata[0];
				if (epromlen > sizeof(EPR_FactoryDataExType))
					epromlen = sizeof(EPR_FactoryDataExType);

				memcpy(&factoryDataEx, EP0OUTdata, epromlen);

				for(i=0; i<factoryData.color_num; i++)
				{
					if(factoryDataEx.m_nColorOrder[i] >=0 && factoryDataEx.m_nColorOrder[i] <= 3)
						factoryDataEx.m_nColorOrder[i] += '0';
				}

				if(factoryDataEx.MaxGroupNumber == 0)
				{
					factoryDataEx.MaxGroupNumber = absv(factoryData.group_num);
					MaxGroupNumber_is_group_num = True;
				}
				else
				{
					MaxGroupNumber_is_group_num = False;
				}
#if defined( EPSON_CLEAN_UPDOWN)||defined( RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
				OSSemPend(CleaningParamSem, 0, &err);
				//if(factoryDataEx.MaxGroupNumber == 1)
				{
					cleanparam_EPSON_ALLWIN.Runtime.HeadMask = (1<<GetCleanPartCount()) - 1;
				}
				OSSemPost(CleaningParamSem);
				SaveCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN, 
						(INT8U*)&cleanparam_EPSON_ALLWIN.Runtime.HeadMask - (INT8U*)&cleanparam_EPSON_ALLWIN, 
						sizeof(cleanparam_EPSON_ALLWIN.Runtime.HeadMask));
#elif (defined(EPSON_CLEAN_INTEGRATE) ||defined(EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3) )
				OSSemPend(CleaningParamSem, 0, &err);
				//if(factoryDataEx.MaxGroupNumber == 1)
				{
					cleanparam_EPSON_MICOLOR.Runtime.HeadMask = (1<<GetCleanPartCount()) - 1;
				}
				OSSemPost(CleaningParamSem);						
				SaveCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR, 
						(INT8U*)&cleanparam_EPSON_MICOLOR.Runtime.HeadMask - (INT8U*)&cleanparam_EPSON_MICOLOR, 
						sizeof(cleanparam_EPSON_MICOLOR.Runtime.HeadMask));
#else
#ERROR
#endif
				factoryDataEx.len = sizeof(struct EPR_FactoryData_Ex);  //current len is sizeof(struct EPR_FactoryData_Ex)
				factoryDataEx.version = FACTORYDATA_EX_VER;  //current version is 0x02

				if(!SaveFactoryDataEx_EPSON(&factoryDataEx))
				{
					ret = False;
					break;
				}

				OSSemPend(LCDMenuConfigureSem, 0, &err);
				LCDMenuConfig.Active_HeadMask = (1<<EPSON_PERMIT_GROUPNUM) -1;
				OSSemPost(LCDMenuConfigureSem);

				if(!SaveLCDMenuConfig())
				{
					ret = False;
					break;
				}
				break;
			}
			else if(value == 7) //OUT, Set Calibration Config
			{
				if( (((CaliConfigType*)EP0OUTdata)->len != 0x40 &&
							((CaliConfigType*)EP0OUTdata)->len != sizeof(CaliConfigType)) ||
						((CaliConfigType*)EP0OUTdata)->version != CALI_CONFIG_VERSION )
				{
					ret = False;
					break;
				}

				epromlen = EP0OUTdata[0];
				if (epromlen > sizeof(CaliConfigType))
					epromlen = sizeof(CaliConfigType);

				memcpy(&caliConfig, EP0OUTdata, epromlen);
				caliConfig.len = sizeof(CaliConfigType);  
				caliConfig.version = CALI_CONFIG_VERSION;  

				ret = SaveEPSONCaliConfig();
				break;
			}
			else if(value == 9) //OUT, Set Main UI items
			{
				memcpy(&mainUIPara, EP0OUTdata, sizeof(mainUIPara));
				mainUI_dirty = True;
				break;
			}
			else if(value == 11) //OUT, Set media info items
			{
				memcpy(&mediaInfo, EP0OUTdata, sizeof(mediaInfo));
				mediaInfo_dirty = True;
				break;
			}
			else if(value == 10) //OUT, Set print quality items
			{
				memcpy(&printQuality, EP0OUTdata, sizeof(printQuality));
				printQuality_dirty = True;
				break;
			}
			else if(value == 12) //OUT, Set PM Calibration setting
			{ 
				if (index + setuplength > sizeof(pmCaliSettingFWFormat))
				{
					ret = False;
					break;
				}

#ifndef DEBUG_NOT_SET_PM_CALI
				//OSSemPend(PMCaliSettingSem, 0, &err);
				memcpy((char*)&pmCaliSettingFWFormat + index, EP0OUTdata, setuplength);
				//OSSemPost(PMCaliSettingSem);

				if(index + setuplength == sizeof(pmCaliSettingFWFormat))
				{
					bPMCaliSettingDirty = True;
				}
#endif
				break;
			}

			else if(value == 16)//out send epsoncaliseting
			{
				EPM_Head headout,headin;
				INT16U length;
				length = sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head);

#ifdef MANUFACTURER_DYSS
				length += EX_MAX_BYTE;
#endif

				if(index==0&&setuplength<=64&&setuplength>0)
				{
					calierrtype = 0;

					memcpy((char*)&headout, EP0OUTdata, sizeof(EPM_Head));
					if(!IIC_ReadEEPRom_Head(EPR_CALIBRATION_SETTING_OFFSET,&headin))
					{
						ret=False;
						break;
					}
					if(headout.flag!=headin.flag)
					{
						calierrtype = 4;//the flag is wrong
						ret=False;
						break;
					}
					else
					{
						if(headout.version<SUPPORT_CALI_UPDATE_BASE_VERSION)
						{
							calierrtype = 1;//the version of update low
							ret=False;
							break;
						}
						if(headout.version>EPR_EPSON_CALI_SETTING_VERSION)
						{
							calierrtype = 2;// the version of update too new ,not exist.
							ret=False;
							break;
						}
					}
				}
				if(calierrtype!=0)
				{
					break;
				}
				else
				{
					if(index+setuplength>length)
					{
						calierrtype = 3;//setuplength erro;
						ret = False;
						break;
					}
					if(!OutCaliData(EP0OUTdata,index,setuplength))
					{
						ret = False;
						break;
					}
				}
				break;
			}

			else if(value == 17)//定原点
			{
				OSSemPost(MenuSem);
				UIAction_SetBasePoint(0);
				OSSemPend(MenuSem, 0, &err);	
				break;
			}


			else if(value == 18)
			{
				memcpy((void *)&LCDMenuConfig.MeasureSensorOffset, EP0OUTdata, sizeof(float));
				SaveLCDMenuConfig();
				break;
			}
#ifdef RIPSTAR_FLAT_EX		
			else if(value == 19)//定原点
			{
				OSSemPost(MenuSem);
				UIAction_SetBasePoint_Y(0);
				OSSemPend(MenuSem, 0, &err);	
				break;
			}
			else if(value == 20)//定原点
			{
				OSSemPost(MenuSem);
				UIAction_SetBasePoint_Z(0);
				OSSemPend(MenuSem, 0, &err);	
				break;
			}
#endif	
#ifdef COORD_NEW_UV	
			else if(value == 21)
			{
				INT16U length = sizeof(Ripstar_UV_Setting);
				memcpy((char*)&Ripstar_UV_Setting, EP0OUTdata, sizeof(Ripstar_UV_Setting));
				if(!IIC_WriteEEPRom_Ex(EPR_NEW_UV_SETTING_OFFSET, (INT8U *)&Ripstar_UV_Setting, &length));
				{
					ret=False;
					break;
				}
			}
#endif

			else if(value == 22)
			{
#if defined(DYSS_UV_CLEAN)||defined(EPSON_CLEAN_UPDOWN)			
				DoFlash_EPSON_ALLWIN(0,0);
#else
				DoFlash_EPSON_MICOLOR(0,0);
#endif
				break;
			}
#ifdef DYSS_UV_CLEAN			
			else if(value == 23)
			{
				Menu_Move_Zpoint(0,0);
				break;
			}
			else if(value == 24)
			{
				status_ReportStatus(STATUS_MOVING, STATUS_SET);
				tmp[0] = 2;
				tmp[1] = UART_RPT_DSP_MEASURE_HIGH;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, tmp))
					OSTimeDly(1);
				break;
			}
			else if(value == 30)
			{
				status_ReportStatus(STATUS_MOVING, STATUS_SET);
				tmp[0] = 2;
				tmp[1] = UART_RPT_DSP_MEASURE_INIT;
				while (!UART_SendCMD(UART_MOTION_CHANNEL, tmp))
					OSTimeDly(1);
				break;
			}
			else if(value == 31)
			{
				if(EP0OUTdata[0] == 1)
					LCDMenuConfig.cfg_bit |= DYSS_MEASURE_PROTECT_SWITCH;
				else
					LCDMenuConfig.cfg_bit &= ~DYSS_MEASURE_PROTECT_SWITCH;

				bDyssMeasureDirty = True;

				SaveLCDMenuConfig();
				break;
			}				

#endif
#ifdef WAVE_SET_FUN
			else if(value == 25)
			{
				DATA_LEN = 0;
				tmp[0] = 4;
				tmp[1] = UART_HEAD_WAVE_READ;
				tmp[2] = index & 0xFF;
				tmp[3] = index >> 8;

				while (!UART_SendCMD(UART_HEAD_CHANNEL, tmp))
					OSTimeDly(1);
				break;
			}
			else if(value == 26||value == 27||value == 28||value == 29 || value == 43)
			{
				if(index == 0)
				{
					DATA_LEN = (INT16U)*(INT32U*)EP0OUTdata;
					TRANSFER_MODE = value - 25;
				}
				else
				{
					memcpy((void*)&WAVE_BUF[(index -1)*WAVE_DATA_MAX_LEN], (void*)EP0OUTdata, setuplength);
					if(value == 29)		//波形名字修改,备份波形名字
						memcpy((void*)&WAVE_NAME_BUF[(index -1)*WAVE_DATA_MAX_LEN], (void*)EP0OUTdata, setuplength);
					else if(value == 43)	//波形映射表修改，备份波形映射表
						memcpy((void*)&WAVE_MAP_TABLE[(index -1)*WAVE_DATA_MAX_LEN], (void*)EP0OUTdata, setuplength);
				}
				break;
			}
#endif

#ifdef FUNCTION_CLOSE_LCD
			else if(value == 32)
			{
				if(EP0OUTdata[0]&1)
					UI_Reset_Calibration(0);
				if(EP0OUTdata[0]&2)
					UI_Reset_CleanConfig(0);
				if(EP0OUTdata[0]&4)
					UI_Reset_MotionConfig(0);
				break;
			}
			else if(value == 33)
			{
				for(i = 0;i < 8;i++)
					g_headParameterPercent[i] = EP0OUTdata[i];
				break;
			}
#endif
#if defined (DGEN_AUTOFUCTION)
			else if(value == 34)
			{
				LCDMenuConfig_EX.Color_Bar_Flash_On = EP0OUTdata[0];
				LCDMenuConfig_EX.Color_Bar_POSI = *((__packed float*)&EP0OUTdata[1]);
				SaveLCDMenuConfig_EX();		
				break;
			}
#endif
#ifdef MANUFACTURER_DYSS
			else if(value == 35)
			{
				if(*((__packed float*)&EP0OUTdata[0]) >= 0 && *((__packed float*)&EP0OUTdata[0]) <= 30.0/2.54)
				{
					LCDMenuConfig_EX.X_Plat_Correction = *((__packed float*)&EP0OUTdata[0]);
					printer.platSpace = PRINTER_PLAT_SPACE;
					AdjustPlatSpace();
				}
				if(*((__packed float*)&EP0OUTdata[4]) >= -((float)(34188 - 2*epsonCaliSetting_runtime.stepPass.BaseStep)/DYSS_Y_PWM_RATIO) && *((__packed float*)&EP0OUTdata[4]) <= 42.0/2.54)
					LCDMenuConfig_EX.Y_Plat_Correction = *((__packed float*)&EP0OUTdata[4]);

				SaveLCDMenuConfig_EX();		
				break;
			}
#endif

			else if(value == 36)
			{
				Write_Dsp_Data(EP0OUTdata,index,setuplength);
				break;
			}

#ifdef FULGENCY_FUN
			else if(value == 37)
			{
				Y_GOHOME_Dirty = index;
				break;
			}
			else if(value == 38)
			{
				if(index == 0)
					FULGENCY_PLC_MASK |=START_PRT;
				else 
					FULGENCY_PLC_MASK |=Y_UNLOAD_FINISH;
				break;
			}
			else if(value == 39)
			{
				LCDMenuConfig_EX.Head_PUMP_Mask = EP0OUTdata[0];
				if(*((__packed INT32U*)&EP0OUTdata[1]) != 0)
					LCDMenuConfig_EX.Head_PUMP1_ON = *((__packed INT32U*)&EP0OUTdata[1]);
				if(*((__packed INT32U*)&EP0OUTdata[5]) != 0)
					LCDMenuConfig_EX.Head_PUMP1_OFF = *((__packed INT32U*)&EP0OUTdata[5]);
				if(*((__packed INT32U*)&EP0OUTdata[9]) != 0)
					LCDMenuConfig_EX.Head_PUMP2_ON = *((__packed INT32U*)&EP0OUTdata[9]);
				if(*((__packed INT32U*)&EP0OUTdata[13]) != 0)
					LCDMenuConfig_EX.Head_PUMP2_OFF = *((__packed INT32U*)&EP0OUTdata[13]);
				Head_Pump_Dirty = True;
				break;
			}
#endif

#if 0
			else if(value == 90)
			{
				if(index&0x10)
					status_ReportStatus(STATUS_WAR_EPSON_HEAD_FPGA_ERR|((index&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET); 
				else if(index&0x20)
					status_ReportStatus(STATUS_ERR_UART1_TXTIMEOUT|((index&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET); 
				else if(index&0x40)
					status_ReportStatus(STATUS_FTA_FACTORY_COLORNUM_WRONG|((index&HEAD_BIT_MASK)<<HEAD_BIT_STATUS), HEAD_ERROR_SET); 
				break;
			}
#endif
#ifdef MANUFACTURER_DYSS
			else if(value == 40)
			{
				LCDMenuConfig_EX.b_Sundry = *((__packed INT32U*)&EP0OUTdata[0]);
				SaveLCDMenuConfig_EX(&LCDMenuConfig_EX,(INT8U*)&LCDMenuConfig_EX.b_Sundry - (INT8U*)&LCDMenuConfig_EX, sizeof(LCDMenuConfig_EX.b_Sundry));		
				break;
			}
#endif
#if (defined(TIMER_ON)&&defined(PUMP_INTERMITTENT))

			else if(value == 41)
			{
				if(*((__packed INT16U*)&EP0OUTdata[0]) != 0)
					LCDMenuConfig_EX.Interval_Pump_On_Time = *((__packed INT16U*)&EP0OUTdata[0]);
				if(*((__packed INT16U*)&EP0OUTdata[2]) != 0)
					LCDMenuConfig_EX.Interval_Pump_Off_Time = *((__packed INT16U*)&EP0OUTdata[2]);
				SaveLCDMenuConfig_EX(&LCDMenuConfig_EX,(INT8U*)&LCDMenuConfig_EX.Interval_Pump_On_Time - (INT8U*)&LCDMenuConfig_EX, 
						sizeof(LCDMenuConfig_EX.Interval_Pump_On_Time) + sizeof(LCDMenuConfig_EX.Interval_Pump_Off_Time));		
				break;
			}
#endif

#ifdef WAVE_SET_FUN
			else if(value == 42)	//波形名字buf预读
			{
				DATA_LEN = 0;

				tmp[0] = 2;
				tmp[1] = UART_HEAD_WAVE_NAME_READ;

				while (!UART_SendCMD(UART_HEAD_CHANNEL, tmp))
					OSTimeDly(1);
				break;
			}
			else if(value == 44)	//波形映射表buf预读
			{
				DATA_LEN = 0;
				tmp[0] = 2;
				tmp[1] = UART_HEAD_WAVE_CHANNEL_READ ;

				while (!UART_SendCMD(UART_HEAD_CHANNEL, tmp))
					OSTimeDly(1);
				break;

			}
#endif
#ifdef FULGENCY_FUN
			else if(value == 45)
			{
				LCDMenuConfig_EX.Z_Plat_Correction = *((__packed float*)&EP0OUTdata[0]);
				SaveLCDMenuConfig_EX(&LCDMenuConfig_EX,(INT8U*)&LCDMenuConfig_EX.Z_Plat_Correction - (INT8U*)&LCDMenuConfig_EX, sizeof(LCDMenuConfig_EX.Z_Plat_Correction));		
				break;
			}
			else if(value == 46)
			{
				if(curPos.x < (cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos +10) && curPos.x > (cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos -10))
				{
					move_dir = 4;
					move_distance = Z_PLAT_LENGTH;//move_distance means target position
					move_type = 3;
				}
				else
				{
					move_dir = 1;
					move_distance = 0;//move_distance means target position
					move_type = 3;
				}

				status_ReportStatus(CMD_START_MOVE, STATUS_SET);
				break;
			}
#endif			
		}
		break;
#endif
#if defined(SUPPORT_MOTOR_CONTROL)
	case 0x7E:
		if (!direction)//out
		{
			switch(value)
			{
#if defined(RECORD_MOTION_MOVEMENT)
			case 1:
				StartMotionRecord(EP0OUTdata[0], EP0OUTdata[1]);
				break;
			case 2:
				EndMotionRecord();
				break;
			case 3:
				StartUploadRecord(*(INT32U*)EP0OUTdata, EP0OUTdata[sizeof(INT32U)]);
				break;
#endif                        
			case 6:
				memcpy((void *)&(motionParam.axis_param[AXIS_ID_X-1].transmisstion_ratio_open_loop), EP0OUTdata, sizeof(float));
				bMotionCfgDirty = TRUE;
				break;
			case 7:
				memcpy((void *)(motionParam.axis_param[AXIS_ID_X-1].Fixed_Position), EP0OUTdata, MAX_POSI_NUM*sizeof(INT32S));
				motionParam.axis_param[AXIS_ID_X-1].Fixed_MoveRegion_StartPoint = motionParam.axis_param[AXIS_ID_X-1].Fixed_Position[0];
				motionParam.axis_param[AXIS_ID_X-1].OriginOffset = motionParam.axis_param[AXIS_ID_X-1].Fixed_Position[1];
				motionParam.axis_param[AXIS_ID_X-1].Fixed_MoveRegion_EndPoint = motionParam.axis_param[AXIS_ID_X-1].Fixed_Position[4];
				bMotionCfgDirty = TRUE;
				break;
			case 8:
				if((tmp_index = (index)&0x00FF) > 20)
				{
					tmp_index += 5;
				}

				if(index >> 8)
				{
					memcpy((void *)(motionParam.axis_param[AXIS_ID_X-1].PID_Speed[tmp_index]), EP0OUTdata, 3*sizeof(float));
				}
				else
				{
					memcpy((void *)(motionParam.axis_param[AXIS_ID_Y-1].PID_Speed[tmp_index]), EP0OUTdata, 3*sizeof(float));
				}
				bMotionCfgDirty = TRUE;
				break;
			case 9:
				if((tmp_index = (index)&0x00FF) > 20)
				{
					tmp_index += 5;
				}
				if(tmp_index == 0)
					break;
				tmp_index--;
				if(index >> 8)
				{
					memcpy((void *)&(motionParam.axis_param[AXIS_ID_X-1].accParam[tmp_index]), EP0OUTdata, sizeof(struct AccParam));
				}
				else
				{
					memcpy((void *)&(motionParam.axis_param[AXIS_ID_Y-1].accParam[tmp_index]), EP0OUTdata, sizeof(struct AccParam));
				}
				bMotionCfgDirty = TRUE;
				break;
			}
		}
		else //in
		{
			switch(value)
			{
			case 4:
				//rFPGA_EPSON_RegAddr = EPSON_REGADDR_X_MOTOR_COOR;
#ifdef HEAD_EPSON_GEN5        
				tmpdata = ReadSafeEpsonRegInt(EPSON_REGADDR_X_MOTOR_COOR);
#elif defined(HEAD_RICOH_G4)
				tmpdata = rFPGA_RICOH_XMOTORCOOR_L;
				tmpdata |= ((rFPGA_RICOH_XMOTORCOOR_H&0xFF)<<16);
				tmpdata -= XORIGIN_OFFSET_MOTOR;
#endif
				memcpy(EP0INdata, &tmpdata, sizeof(INT32S));
				*len = sizeof(INT32S);
				break;
			case 5:
				//rFPGA_EPSON_RegAddr = EPSON_REGADDR_X_PRT_COOR;
#ifdef HEAD_EPSON_GEN5        
				tmpdata = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);
#elif defined(HEAD_RICOH_G4)
				tmpdata = rFPGA_RICOH_XPRTCOOR_L;
				tmpdata |= ((rFPGA_RICOH_XPRTCOOR_H&0xFF)<<16);
				tmpdata -= XORIGIN_OFFSET;
#endif
				memcpy(EP0INdata, &tmpdata, sizeof(INT32S));
				*len = sizeof(INT32S);
				break;
			case 6:
				memcpy(EP0INdata, (void *)&(motionParam.axis_param[AXIS_ID_X-1].transmisstion_ratio_open_loop), sizeof(float));
				*len = sizeof(float);
				break;
			case 7:
				memcpy(EP0INdata, (void *)(motionParam.axis_param[AXIS_ID_X-1].Fixed_Position), MAX_POSI_NUM*sizeof(INT32S));
				*len = MAX_POSI_NUM*sizeof(float);
				break;
			case 8:
				if((tmp_index = (index)&0x00FF) > 20)
				{
					tmp_index += 5;
				}

				if(index >> 8)
				{
					memcpy(EP0INdata, (void *)(motionParam.axis_param[AXIS_ID_X-1].PID_Speed[tmp_index]), 3*sizeof(float));
					*len = 3*sizeof(float);
				}
				else
				{
					memcpy(EP0INdata, (void *)(motionParam.axis_param[AXIS_ID_Y-1].PID_Speed[tmp_index]), 3*sizeof(float));
					*len = 3*sizeof(float);
				}
				break;
			case 9:
				if((tmp_index = (index)&0x00FF) > 20)
				{
					tmp_index += 5;
				}
				if(tmp_index == 0)
					break;
				tmp_index--;
				if(index >> 8)
				{
					memcpy(EP0INdata, (void *)&(motionParam.axis_param[AXIS_ID_X-1].accParam[tmp_index]), sizeof(struct AccParam));
					*len = sizeof(struct AccParam);
				}
				else
				{
					memcpy(EP0INdata, (void *)&(motionParam.axis_param[AXIS_ID_Y-1].accParam[tmp_index]), sizeof(struct AccParam));
					*len = sizeof(struct AccParam);
				}
				break;
			case 10:
				//rFPGA_EPSON_RegAddr = EPSON_REGADDR_Y_MOTOR_COOR;
#ifdef HEAD_EPSON_GEN5        
				tmpdata = ReadSafeEpsonRegInt(EPSON_REGADDR_Y_MOTOR_COOR);
#elif defined(HEAD_RICOH_G4)
				tmpdata = rFPGA_RICOH_YMOTORCOOR_L;
				tmpdata |= (rFPGA_RICOH_YMOTORCOOR_H<<16);
#endif
				memcpy(EP0INdata, &tmpdata, sizeof(INT32S));
				*len = sizeof(INT32S);
				break;
			}
		}
		break;
#endif                        
	case 0x7D:
		if (direction)
		{
			if(index == 0 && value == 0)
			{
				if(factoryDataEx.Only_Used_1head == 1)
				{	
					if((job_info.PassNumber -1)*2 < MAX_PASS_NUMBER_CALIBRATION)
						memcpy(EP0INdata, &(epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[(job_info.PassNumber -1)*2]), sizeof(signed short));
					else
						memcpy(EP0INdata, &(epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[3]), sizeof(signed short));
				}
				else
					memcpy(EP0INdata, &(epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[job_info.PassNumber -1]), sizeof(signed short));
				EP0INdata[sizeof(signed short)] = job_info.PassNumber;
				*len = sizeof(signed short) + 1;
			}
#if defined(EPSON_BOTTOM_BOARD_V3)||defined(EPSON_BOTTOM_BOARD_V2_1)
			else if(index == 1) //read Resistance_Offset
			{
				memcpy(EP0INdata, (void *)HeaterInfo.ResistanceOffset, sizeof(HeaterInfo.ResistanceOffset));
				*len = sizeof(HeaterInfo.ResistanceOffset) ;
			}
			else if(index == 3) 
			{
				if(TestHeaterEnable[value] ==  False)
					*(float *)EP0INdata =AverResistance[value];
				else
				{
					*(float *)EP0INdata = 500.0;
				}
				*len = sizeof(float) ;
			}
#endif
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
			else if(index == 2 )
			{
				if(bCleanSprayDirty == True)
					memcpy(EP0INdata, (void *)&USB_CleanSpray, sizeof(USB_CleanSpray));
				else
					memcpy(EP0INdata, (void *)&cleanparam_EPSON_MICOLOR.bAutoSpray, sizeof(USB_CleanSpray));
				*len = sizeof(USB_CleanSpray);
			}

#endif
		}
		else
		{
#if defined(EPSON_BOTTOM_BOARD_V3)||defined(EPSON_BOTTOM_BOARD_V2_1)
			if(index == 1) //write Resistance_Offset
			{
				memcpy((void *)HeaterInfo.ResistanceOffset, EP0OUTdata, sizeof(HeaterInfo.ResistanceOffset));
				bResistanceDirty = True;
			}	
			if(index == 4) //write Resistance_Offset
			{
				float Resistance = 0;
				memcpy((void *)&Resistance, EP0OUTdata, sizeof(float));
				HeaterInfo.ResistanceOffset[value] = Resistance - AverResistance[value];
				bResistanceDirty = True;
			}	
			if(index == 3 )
			{	    
				TestHeaterEnable[value] =  True;
			}
#endif
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
			if(index == 2 )
			{	    
				memcpy((void *)&USB_CleanSpray, EP0OUTdata, sizeof(USB_CleanSpray));
				bCleanSprayDirty = True;
			}

#endif
		}
		break;
		//Process vendor request without EP0 data here
	case 0x20:
		if (direction == 0) //All EP0 command must have ep0 buffer, 
		{
			ret = False;
			break;
		}else
		{
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x20;
			*len = 2;
		}

		switch (index) //Index
		{
		case 0x14: //SprayAuto
			//#if !(defined( HEAD_EPSON_GEN5)) || defined (EPSON_BOTTOM_BOARD_V3)
			nextband_autoSpray = True;
			status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
			//#endif
			break;
		case 0x15: //AutoSuckHead, auto clean
#if defined (EPSON_CLEAN_UPDOWN)
#ifdef FUNCTION_CLOSE_LCD
			DoKeyClean_EPSON_ALLWIN(0,value);
#else
			DoKeyClean_EPSON_ALLWIN(0,0);
#endif
#else
#ifdef DYSS_UV_CLEAN
			DYSS_CLEAN_FLASH = 2;
			ActiveCleanConfig = 2;
#endif
			nextband_autoClean = True;
			status_ReportStatus(CMD_CLEAN_AUTO, STATUS_SET);
#endif                    
			break;
		case 0x16: //Enter single clean mode
#ifndef HEAD_EPSON_GEN5
			clean_single_indexgl = -2;
			status_ReportStatus(CMD_CLEAN_SINGLE, STATUS_SET);
#endif                    
			break;
		case 0x17: //Exit single clean mode
#ifndef HEAD_EPSON_GEN5
			status_ReportStatus(CMD_CLEAN_SINGLE, STATUS_CLR);
#endif                    
			break;
		case 0x19: //Single Clean, [3] is index
			clean_single_indexgl = value%MAX_HEADS_CLEAN_NUMBER;
			break;
		case 0x20: //Start move, not used ?????????????????
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			UIAction_Move_Start((void*)1, value, move_distance);
#else
			move_dir = value;
			move_type = 0;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif                        
			break;
		case 0x21: //Stop move
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			UIAction_Move_End((void*)1);
#else
			status_ReportStatus(CMD_START_MOVE, STATUS_CLR);
#endif                        
			break;
		case 0x22: //Go home
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)

#if defined(FULGENCY_FUN)
			FIND_ORIGIN_Dirty = 1;
#else
			UIAction_X_GoHome((void*)1);
#endif

#else
			move_dir = 2;
			move_distance = 0;
			move_type = 0;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
			break;
		case 0x25: //Go home. All axes, 1 X 2 Y 3 Z, can be bit or
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)

#if defined(FULGENCY_FUN)

			if(value == 2)
				FIND_ORIGIN_Dirty = 3;
			else if(value == 4)
				FIND_ORIGIN_Dirty = 5;
#else
			UIAction_AllAxis_GoHome((void*)1, value);
#endif

#else
			move_dir = value;  //move_dir means axis.
			move_distance = 0;
			move_type = 1;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
			break;
		case 0x63: //Clear Updating Status.
			status_ReportStatus(STATUS_UPDATING, STATUS_CLR);
			break;
		case 0x65: //Set motion uart resend interval
			if (value < 0)
				uart_waittime[UART_MOTION_CHANNEL] = (INT16U)32767-value;
			else 
				uart_waittime[UART_MOTION_CHANNEL] = value;
			break;
		case 0x66: //Clear Factory Data
			OSSemPend(IICSem, 0, &err);
			epromlen = EPR_FACTORY_AUTHORITY_FLAG_SIZE;
			for ( i = 0; i < 8; i++)
				tmp[i] = 0;
			if (IIC_WriteEEPRom(EPR_FACTORY_AUTHORITY_FLAG_OFFSET, tmp, &epromlen) == False)
			{
				OSSemPost(IICSem);
				status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
				ret = False;
				break;
			}				
			OSSemPost(IICSem);
			break;
		case 0x67: //Clear error, value is the error number
			if (value < 256)
				status_ReportStatus(STATUS_ERR+value, STATUS_CLR);
			break;
		case 0x30: //Pause
			if(CMD_Epson_Operate_Type != CMD_EPSON_T_MACHIME_TEST)    
				status_ReportStatus(CMD_PAUSE, STATUS_SET);
			break;
		case 0x31: //Resume
#if defined(HEAD_RICOH_G4)    
			False_Idle_Off(); 
#endif
			status_ReportStatus(CMD_PAUSE, STATUS_CLR);
			status_ReportStatus(STATUS_PAUSE, STATUS_CLR);
			break;
		case 0x32: //Cancel
			if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			{
				status_ReportStatus(CMD_CANCEL_REQ, STATUS_CLR); 
				status_ReportStatus(STATUS_NO_CANCEL, STATUS_CLR);
				status_ReportStatus(STATUS_PAUSE, STATUS_CLR); //Clear pause
				status_ReportStatus(CMD_PAUSE, STATUS_CLR); //Clear pause
				status_FlushAllErrStatus();
				status_ReportStatus(STATUS_ERR, STATUS_CLR); //Clear error
			}
			break;
		case 0x23: //Measure paper, support from 20090814
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			UIAction_MeasureMedia((void*)1, value);
#else
			move_type = 4;
			measure_media_method = value;
			status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif
			break;	
		case 0x40: //Reset board
		case 0x18: //Close cleaner, don't support now
		default:
			ret = False;
		}
		break;
	case 0x42: //Report printer physical size
		*len = 18;
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x42;
		*((__packed INT32S *)&EP0INdata[2]) = printerPhysicalSize.x;	
		*((__packed INT32S *)&EP0INdata[6]) = printerPhysicalSize.y;	
		*((__packed INT32S *)&EP0INdata[10]) = printerPhysicalSize.z;
		*((__packed INT32S *)&EP0INdata[14]) = printerPhysicalSize.f;	
		break;
	case 0x43: //IN, Get the home position
		if (OSFlagAccept(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_SET_ANY,&err))
			ret = False; //Write_SX2reg(SX2_SETUP, 0xFF); //Stall the request
		else //If the motion stop, it will report the current position
		{	
			*len = 18;
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x43;
			*((__packed INT32S *)&EP0INdata[2]) = curPos.x - printer.platSpace;	
			*((__packed INT32S *)&EP0INdata[6]) = curPos.y;	
			*((__packed INT32S *)&EP0INdata[10]) = curPos.z;
			*((__packed INT32S *)&EP0INdata[14]) = curPos.f;	
			//				move_dir = 2;
			//				move_type = 0;
			//				status_ReportStatus(CMD_START_MOVE, STATUS_SET);			
		}
		break;
	case 0x44:	//IN, get media edge
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x44;

		*((__packed INT16S *)&EP0INdata[2]) = mediaVolNum;

		for (i = 0; i < mediaVolNum; i++)
		{
			*((__packed INT32S *)&EP0INdata[4+i*16])  = mediaEdge[i].paper_X_NearEdgePos - printer.platSpace;
			*((__packed INT32S *)&EP0INdata[8+i*16])  = mediaEdge[i].paper_X_FarEdgePos - printer.platSpace;
			*((__packed INT32S *)&EP0INdata[12+i*16]) = mediaEdge[i].paper_Y_NearEdgePos;
			*((__packed INT32S *)&EP0INdata[16+i*16]) = mediaEdge[i].paper_Y_FarEdgePos;
		}
		if (mediaVolNum > 0)
			*len = 4 + mediaVolNum*16;
		else
			*len = 4;
		break;
	case 0x45: //IN, Get status

		memcpy(EP0INdata, (void *)&status_EP0Buf, 64);				
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x45;
#ifdef FUNC_MAINTAIN
		if(bStatus_Maintain & 0x01)
		{
			status_EP0BUF_TYPE tmp = {0};
			memcpy((void *)&tmp,(void *)&status_EP0Buf, sizeof(status_EP0BUF_TYPE));
			tmp.status |= STATUS_MAINTAIN;	//表示维护状态
			memcpy(EP0INdata, (void *)&tmp, 64);		
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x45;
		}
#endif
#if defined(DEBUG_Y_ENCODER)
		*len = 64;
#else			
		*len = 8 + 4*status_EP0Buf.statusCodeNum;
#endif			
		break;
	case 0x46: //IN Get the current position
		valFlag_GetPos = 1; //Set the flag
		tmp[0] = 2;
		tmp[1] = UART_DSP_GET_POS_CMD;
		while (!UART_SendCMD(UART_MOTION_CHANNEL, tmp))
			OSTimeDly(1);
		//In interrupt routine, can not use OSFlagPend

		for (i = 0; i < 200; i++)
		{	
			if (valFlag_GetPos == 0)
				break;
			else
				OSTimeDly(1);
		}
		if (i >= 200)
			ret = False; 
		else
		{	
			*len = 6;
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x46;
			*((__packed INT32S *)&EP0INdata[2]) = curPos.x;	
		}
		break;
	case 0x50: //IN, Get time password
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x50;
		//memcpy(&EP0INdata[2], &password, sizeof(PasswordType));
		memcpy(&EP0INdata[4], password.m_sTimePwd, sizeof(password.m_sTimePwd));
		*len = sizeof(password.m_sTimePwd) + 4;
		break;
	case 0x51: //IN, Get option password
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x51;
		memcpy(&EP0INdata[4], password.m_sOptionPwd, sizeof(password.m_sOptionPwd));
		*len = sizeof(password.m_sOptionPwd) + 4;
		break;
	case 0x73: //IN/OUT, Get/Set Ink password
		if(direction == 0) //Set,OUT
		{
#ifndef BYHX_WTITE_BOARD_TOOL			
			INT8U oldPWStr[EPR_INK_PASSWORD_STR_SIZE];
			memcpy(oldPWStr, password.m_sInkPwd, EPR_INK_PASSWORD_STR_SIZE);
			memcpy(password.m_sInkPwd, &EP0OUTdata[2], EPR_INK_PASSWORD_STR_SIZE);

			if (security_CheckInkPassword()) //Password is OK
			{
				epromlen = EPR_INK_PASSWORD_STR_SIZE;

				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_INK_PASSWORD_STR_OFFSET, password.m_sInkPwd, &epromlen) == False)
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
				OSSemPost(IICSem);
			}else
			{
				memcpy(password.m_sInkPwd, oldPWStr, EPR_INK_PASSWORD_STR_SIZE);
			}
#endif					
		}
		else
		{
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x73;
			memcpy(&EP0INdata[4], password.m_sInkPwd, sizeof(password.m_sInkPwd));
			*len = sizeof(password.m_sInkPwd) + 4;
		}
		break;
	case 0x52:  //Get the elapse time, AND, for new version, Get the used ink volume and limited ink volume
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x52;
		*((__packed INT16U *)&EP0INdata[2]) = password.m_bTimeLimited;
		*((__packed INT32U *)&EP0INdata[4]) = elapsedTime.time_elapse/3600;
		if(setuplength > 8)
		{
			for(i=0; i<MAX_COLOR_NUMBER; i++)
			{
				*((__packed INT16U *)&EP0INdata[8+i*6]) = password.m_bInkLimited;
				*((__packed INT32U *)&EP0INdata[8+i*6+2]) = usedInkVolume.InkVolume[i][1]/INK_VOLUME_FACTOR_PER_LITER;
			}			
			*len = 8+6*MAX_COLOR_NUMBER;
		}
		else
			*len = 8;
		break;

	case 0x54: //IN, Get board info
		if (direction == 0)
		{
			ret = False;
			break;
		}
		memcpy(EP0INdata, &fwInfo, sizeof(FWInfoType));			
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x54;			
		*len = sizeof(FWInfoType);
		break;
	case 0x55: //IN, Get saved parameters
		if ((host_paras.dirty == True)|| (value > 60))
		{
			ret = False;
			break;
		}

		EP0INdata[0] = 0;
		EP0INdata[1] = 0x55;
		//			if (host_paras.length - index >= 60)
		//				EP0INdata[2] = 60; //64-4
		//			else
		//				EP0INdata[2] = host_paras.length - index;
		EP0INdata[2] = value;
		EP0INdata[3] = 0;	
		//			CONSOL_Printf("index = %d", index);
		memcpy(&EP0INdata[4], &host_paras.buf[index], EP0INdata[2]);
		break;
	case 0x56: //IN, get handshake result
		memcpy(&EP0INdata[0], &checkboarddata,sizeof(CheckBoardType));
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x56;			
		*len = sizeof(CheckBoardType);
		break;
	case 0x58: //IN Get saved factory data
		if (valueFlag_FactoryData_Not_Init)
		{
			ret = False;
			break;
		}
		if (factoryData.len > 62)
			factoryData.len = 62;
		for (i = 0; i < factoryData.len; i++) 
			EP0INdata[2+i] = ((__packed INT8U *)&factoryData)[i];
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x58;
		*len = factoryData.len + 2;
		break;

	case 0x5A: //OUT, Set Factory data
		epromlen = EP0OUTdata[0];
		if (epromlen > sizeof(EPR_FactoryDataType))
		{
			ret = False;
			break;
		}

		memcpy(&factoryData, EP0OUTdata, epromlen);

		for (i = 0; i<MAX_HEAD_LIST_LEN;i++)
		{
			if (factoryData.HeadType ==	byhxData.supportedHeadList[i])
				break;
		}
		if ( i >= MAX_HEAD_LIST_LEN) //Not supported print head type
		{
			ret = False;
			break;
		}

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
		{
			INT8U dirty = False;
			for(i=0; i<factoryData.color_num; i++)
			{
				if(factoryDataEx.m_nColorOrder[i] >=0 && factoryDataEx.m_nColorOrder[i] <= 3)
				{
					factoryDataEx.m_nColorOrder[i] += '0';
					dirty = True;
				}
			}
			if(dirty)
			{
				if(!SaveFactoryDataEx_EPSON(&factoryDataEx))
				{
					ret = False;
					break;
				}
			}
		}
#endif

		OSSemPend(IICSem, 0, &err);
		if (IIC_WriteEEPRom(EPR_FACTORY_DATA_OFFSET, (INT8U *)&factoryData, &epromlen) == False)
		{
			OSSemPost(IICSem);
			status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
			ret = False;
			break;
		}

		epromlen = EPR_FACTORY_AUTHORITY_FLAG_SIZE;
		if (IIC_WriteEEPRom(EPR_FACTORY_AUTHORITY_FLAG_OFFSET, factoryAuthorityStr, &epromlen) == False)
		{
			OSSemPost(IICSem);
			status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
			ret = False;
			break;
		}				
		OSSemPost(IICSem);
		bFactoryDataDirty = True;
		break;

	case 0x5B: //IN, Get supported printer list.
		for (i = 0; i<MAX_HEAD_LIST_LEN;i++)
			EP0INdata[i+2] = byhxData.supportedHeadList[i];
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x5B;
		*len = 18;
		break;

		// For Konica,  Index: 5 temperature,0: current voltage 1: Pulese, 2: Adjust Voltage 3: Base Voltage                6: Heat target temprature
	case 0x5C: 
		switch (index) 
		{
		case 0: //Get current voltage
			if (direction != 0)
			{
				for (i=0;i<MAX_VOLTAGENUMBER;i++)
					EP0INdata[i+2] = g_curHeadVoltage[i];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
				*len = MAX_VOLTAGENUMBER + 2;
			}
			break;
		case 1: //
			if (direction == 0) //Set, if the value is 0xFF, printhead will use default value
			{
				for (i=0;i<MAX_HEAD_NUMBER;i++)
					g_headFirePulseWData.data[i] = EP0OUTdata[i]; 
				g_headFirePulseWData.flag = EPR_HEAD_FIREPULSEW_FLAG;
				g_headFirePulseWData.headUpdated = False;

				epromlen = sizeof(FirePulseWParaType);
				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_FIREPULSEW_DATA_OFFSET, (INT8U *)&g_headFirePulseWData, &epromlen) == False)
				{
					OSSemPost(IICSem);
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
					ret = False;
					break;
				}
				OSSemPost(IICSem);
			}
			else //Get
			{
				for (i=0;i<MAX_HEAD_NUMBER;i++)
					EP0INdata[i+2] = g_headFirePulseWData.data[i];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
				*len = MAX_HEAD_NUMBER + 2;
			}
			break;
		case 2://2: Manual Voltage, we change it to adjust voltage
			if (direction == 0) //Set, if the value is 0xFF, printhead will use default value
			{
				g_headVoltageData.flag = EPR_HEAD_VOLTAGE_FLAG;
				for (i=0;i<MAX_VOLTAGENUMBER;i++)
					g_headVoltageData.manualData[i] = EP0OUTdata[i]; 

				//						g_headVoltageData.headUpdated = False; //!!!!!!!!Remove this line
				epromlen = sizeof(VoltageParaType);
				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_VOLTAGE_DATA_OFFSET, (INT8U *)&g_headVoltageData, &epromlen) == False)
				{
					OSSemPost(IICSem);
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
					ret = False;
					break;
				}
				OSSemPost(IICSem);
			}
			else //Get
			{
				for (i=0;i<MAX_VOLTAGENUMBER;i++)
					EP0INdata[i+2] = g_headVoltageData.manualData[i]; //g_curHeadVoltage[i];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
				*len = MAX_VOLTAGENUMBER + 2;
			}
			break;
		case 3:
			if (direction == 0) //Set, auto control,
				{
					for (i=0;i<MAX_VOLTAGENUMBER;i++)
						g_headVoltageData.autoBaseData[i] = EP0OUTdata[i]; 
					g_headVoltageData.flag = EPR_HEAD_VOLTAGE_FLAG;

					for (i = 0; i < MAX_VOLTAGENUMBER; i++)
						g_curHeadVoltage[i] = vol_CalHeadVoltage(i);

					epromlen = sizeof(VoltageParaType);
					OSSemPend(IICSem, 0, &err);
					if (IIC_WriteEEPRom(EPR_VOLTAGE_DATA_OFFSET, (INT8U *)&g_headVoltageData, &epromlen) == False)
					{
						OSSemPost(IICSem);
						status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
						ret = False;
						break;
					}
					OSSemPost(IICSem);
					g_headVoltageData.headUpdated = False;  //Must be change last						
				}
			else //Get
			{
				for (i=0;i<MAX_VOLTAGENUMBER;i++)
					EP0INdata[i+2] = g_headVoltageData.autoBaseData[i];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
				*len = MAX_VOLTAGENUMBER + 2;
			}
			break;
		case 4:
			if (direction == 0) //Only support get
				ret = False;
			else
			{
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
				EP0INdata[2] = g_headVoltageData.autoCon;
				*len = 3;					
			}						
			break;
		case 5: //temperature
			if (direction == 0) 
				ret = False;
			else //Get temeprature
			{
#ifdef CONVERSION_BOARD				
				for (i=0;i<MAX_HEAD_NUMBER*MAX_HB_NUM;i++)
#else
					for (i=0;i<MAX_EPSON_HEAD_CHANNEL;i++)
#endif
						EP0INdata[i+2] = g_headTemeprature[i];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
#ifdef CONVERSION_BOARD	
				*len = MAX_HEAD_NUMBER*MAX_HB_NUM + 2;	
#else
				*len = MAX_EPSON_HEAD_CHANNEL + 2;						
#endif
			}
			break;

			//EPR_HEAT_TEMPRATURE_DATA_OFFSET
#ifdef SUPPORT_HEAD_HEATER				
		case 6: //heater target temperature
			if (direction == 0) //Set, if the value is 0xFF, printhead will use default value
			{
#ifdef CONVERSION_BOARD
				for (i=0;i<MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM;i++)	
#else			
					for (i=0;i<MAX_HEAD_NUMBER;i++)
#endif					
						g_UVheatTemprature.data[i] = EP0OUTdata[i]; 
				//						for (i=0;i<8;i++)
				//							g_UVheatTemprature.data[i] = EP0OUTdata[7-i]; 

				g_UVheatTemprature.flag = EPR_HEAD_HEATTEMPRATURE_FLAG;
				g_UVheatTemprature.headUpdated = False;

				epromlen = sizeof(UVHeatTempratureType);
				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_HEAT_TEMPRATURE_DATA_OFFSET, (INT8U *)&g_UVheatTemprature, &epromlen) == False)
				{
					OSSemPost(IICSem);
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
					ret = False;
					break;
				}
				OSSemPost(IICSem);
			}
			else //Get
			{
#ifdef CONVERSION_BOARD
				for (i=0;i<MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM;i++)	
#else			
					for (i=0;i<MAX_HEAD_NUMBER;i++)
#endif					
						EP0INdata[i+2] = g_UVheatTemprature.data[i];
				//						for (i=0;i<8;i++)
				//							EP0INdata[i+2] = g_UVheatTemprature.data[7-i];

				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
#ifdef CONVERSION_BOARD
				*len = MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM + 2;
#else				
				*len = MAX_HEAD_NUMBER + 2;
#endif
			}
			break;
#endif	
		case 7: //temperature
			if (direction == 0) 
				ret = False;
			else //Get temeprature
			{
				for (i=0;i<MAX_COLOR_NUMBER;i++)
					EP0INdata[i+2] = g_headHeaterThermistorTemeprature[i];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
				*len = MAX_COLOR_NUMBER + 2;						
			}
			break;
#if 0
		case 8: //epson head parameter percent
#warning "need do mapping from PM UI to LCD menu"                    
			if (direction == 0)
			{
				for (i=0;i<MAX_EPSON_HEAD_CHANNEL;i++)
					g_headParameterPercent[i] = EP0OUTdata[EpsonChannelMap[i]]; 

				tmp[0] = MAX_EPSON_HEAD_CHANNEL+2;
				tmp[1] = UART_HEAD_EPSON_PARAM_PERCENT;
				for (i = 0; i < MAX_EPSON_HEAD_CHANNEL; i++)
				{
					tmp[2+i] = g_headParameterPercent[i];
				}
				UART_SendCMD(UART_HEAD_CHANNEL, tmp);
			}
			else //Get epson head parameter percent
			{
				for (i=0;i<MAX_EPSON_HEAD_CHANNEL;i++)
					EP0INdata[i+2] = g_headParameterPercent[EpsonChannelMap[i]];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
				*len = MAX_EPSON_HEAD_CHANNEL + 2;						
			}
			break;
#endif
#ifdef SUPPORT_HEAD_HEATER				
		case 9: //heater target temperature
			if (direction == 0) //Set, if the value is 0xFF, printhead will use default value
			{
				for (i=0;i<MAX_COLOR_NUMBER;i++)
#ifdef CONVERSION_BOARD					
					g_UVheatTemprature.data[i + MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM] = EP0OUTdata[i]; 
#else
				g_UVheatTemprature.data[i + MAX_HEAD_NUMBER] = EP0OUTdata[i]; 
#endif
				//						for (i=0;i<8;i++)
				//							g_UVheatTemprature.data[i] = EP0OUTdata[7-i]; 

				g_UVheatTemprature.flag = EPR_HEAD_HEATTEMPRATURE_FLAG;
				g_UVheatTemprature.headUpdated = False;

				epromlen = sizeof(UVHeatTempratureType);
				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_HEAT_TEMPRATURE_DATA_OFFSET, (INT8U *)&g_UVheatTemprature, &epromlen) == False)
				{
					OSSemPost(IICSem);
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
					ret = False;
					break;
				}
				OSSemPost(IICSem);
			}
			else //Get
			{
				for (i=0;i<MAX_COLOR_NUMBER;i++)
#ifdef CONVERSION_BOARD					
					EP0INdata[i+2] = g_UVheatTemprature.data[i + MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM];
#else
				EP0INdata[i+2] = g_UVheatTemprature.data[i + MAX_HEAD_NUMBER];
#endif
				//						for (i=0;i<8;i++)
				//							EP0INdata[i+2] = g_UVheatTemprature.data[7-i];

				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5C;
				*len = MAX_COLOR_NUMBER + 2;
			}
			break;	
#endif	
		default:
			ret = False;
			break;
		}
		break;

		// 0x6E, For XAAR382, Index: 0: PWM(RO)   1: Vtrim(RW),  5 temperature(RO)
		// 2: DualBank(RW)     3: WVFM Select(WO)  4 HeadInfo(RO)
		// 对于2，3，4， value为喷头编号, 一次只读写一个
#ifdef HEAD_XAAR382		
	case 0x6E:
		switch (index) 
		{
		case 0: //Get the PWM values
			if (direction != 0)
			{
				for (i=0; i<8;i++)
					EP0INdata[i+2] = PWM_values[i];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x6E;
				*len = 8 + 2;
			}
			break;

		case 1: //Vtrim
			if (direction == 0) //Set
			{
				tmp[0] = 8+2;
				tmp[1] = UART_HEADSET_VTRIM;
				for (i = 0; i < 8; i++)
				{
					headMapNum = vol_MapHeadNumber(i, False, False, False, False);
					tmp[2+i] = EP0OUTdata[headMapNum];
				}
				UART_SendCMD(UART_HEAD_CHANNEL, tmp);
				//						CONSOL_Printf("Vtrim com sent:\r\n");
				//						for (i = 0; i < 10; i++)
				//							CONSOL_Printf("%x ", tmp[i]);
			}
			else //Get
			{
				for (i=0;i<8;i++)
					EP0INdata[i+2] = Vtrim_values[i];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x6E;
				*len = 8 + 2;
			}
			break;
		case 2://2:DualBank = 0
			if (direction == 0)
			{
				tmp[0] = 4;
				tmp[1] = UART_HEADSET_SINGLEBANK_SINGLE;
				for (i = 0; i < 8; i++)
				{				
					if (value == vol_MapHeadNumber(i, False, False, False, False))
						break;
				}
				tmp[2] = i;
				tmp[3] = EP0OUTdata[0];
				UART_SendCMD(UART_HEAD_CHANNEL, tmp);					
			}
			else //Get
			{
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x6E;
				EP0INdata[2] = dualBank_values[value];
				*len = 3;
			}
			break;
		case 3: //WVFM Select
			if (direction == 0) //Set, auto control,
				{
					tmp[0] = 4;
					tmp[1] = UART_HEADSET_WFMSELECT_SINGLE;	
					for (i = 0; i < 8; i++)
					{				
						if (value == vol_MapHeadNumber(i, False, False, False, False))
							break;
					}
					tmp[2] = i;
					tmp[3] = EP0OUTdata[0];
					UART_SendCMD(UART_HEAD_CHANNEL, tmp);					
				}
			else //Get
				ret = False;
			break;
		case 4: //HeadInfo
			if (direction == 0) //Only support get
				ret = False;
			else  //这里Type内部结构都是字节型，跟HOST接口改为USHORT型******************
			{
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x6E;
				*((__packed INT32U *)&EP0INdata[2]) = headInfo[value].SerNo;
				*((__packed INT16U *)&EP0INdata[6]) = headInfo[value].FirmVer;
				*((__packed INT16U *)&EP0INdata[8]) = headInfo[value].Type;
				*((__packed INT16U *)&EP0INdata[10]) = headInfo[value].WfmID1;
				*((__packed INT16U *)&EP0INdata[12]) = headInfo[value].WfmID2;
				*((__packed INT16U *)&EP0INdata[14]) = headInfo[value].WfmID3;
				*((__packed INT16U *)&EP0INdata[16]) = headInfo[value].WfmID4;
				*len = 2 + sizeof(HeadInfoType);					
			}						
			break;
		case 5: //temperature
			if (direction == 0) 
				ret = False;
			else //Get temeprature
			{
				for (i=0;i<MAX_HEAD_NUMBER;i++)
					EP0INdata[i+2] = g_headTemeprature[i];
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x6E;
				*len = MAX_HEAD_NUMBER + 2;						
			}
			break;
		default:
			ret = False;
			break;

		}
		break;
#endif			
	case 0x5D: //Get set BYHX data, this command is only for BYHX Tools
		if (direction) //Get
		{
			if (valueFlag_BYHXData_Not_Init)
			{
				ret = False;
				break;
			}
			if (byhxData.len > 62)
				byhxData.len = 62;
			for (i = 0; i < byhxData.len; i++) 
				EP0INdata[2+i] = ((__packed INT8U *)&byhxData)[i];
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x5D;
			*len = byhxData.len + 2;
		}
#if defined(BYHX_WTITE_BOARD_TOOL) || defined (ek_board_sim)
		else // == 0 Set
		{
			epromlen = EP0OUTdata[0];
			if (epromlen > sizeof(EPR_BYHXDataType))
			{
				ret = False;
				break;
			}

			memcpy(&byhxData, EP0OUTdata, epromlen);

			if(!InitSecurityChip() || !WriteBoardAndManufacturerID(byhxData.boardID, byhxData.manufacturerID))
			{
				status_ReportStatus(STATUS_FTA_SECURITY_CHIP_ERR, STATUS_SET);
				ret = False;
				break;
			}
			byhxData.boardID = 0xFFFFFFFF;
			byhxData.manufacturerID = 0xFFFF;

			OSSemPend(IICSem, 0, &err);
			if (IIC_WriteEEPRom(EPR_BYHX_DATA_OFFSET, (INT8U *)&byhxData, &epromlen) == False)
			{
				OSSemPost(IICSem);
				status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
				ret = False;
				break;
			}

			epromlen = EPR_BYHX_AUTHORITY_FLAG_SIZE;
			if (IIC_WriteEEPRom(EPR_BYHX_AUTHORITY_FLAG_OFFSET, byhxAuthorityStr, &epromlen) == False)
			{
				OSSemPost(IICSem);
				status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
				ret = False;
				break;
			}				
			OSSemPost(IICSem);
		}
#endif			
		break;

	case 0x5E: //Get head board parameter
		*((__packed INT32U *)&EP0INdata[2]) = headboardInfo.headCategory; //Software maybe need not care this value.
		if (headboardInfo.headBoardType == HEAD_BOARD_TYPE_KM512_16HEAD_V2)
			*((__packed INT32U *)&EP0INdata[6]) = HEAD_BOARD_TYPE_KM512_16HEAD;  //Software think these 2 boards same.
		else
			*((__packed INT32U *)&EP0INdata[6]) = headboardInfo.headBoardType;
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x5E;
		*len = 10;
		break;
#ifdef VENDOR_PASSWORD_TOOL
	case 0x5F: //IN, Generate Time Limited password
		ret = False;
		if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_PRINT_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		{
			//index: time;		value: boardID
			if ((motionInitS2) && (value != 1) && (byhxData.boardID == 1))  //Password generator board ID == 1
				//if (motionInitS2)  //Supper
			{
				if (byhxData.manufacturerID == fwInfo.manufacturerID)
					security_getPassword(&EP0INdata[2], byhxData.manufacturerID, value, 0, index, 0x1234);
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x5F;
				ret = True;
			}
		}
		break;
	case 0x6A: //IN, Generate Language password
		ret = False;
		if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_PRINT_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		{
			//index[0~3]: language;		value: boardID
			if ((motionInitS2) && (value != 1) && (byhxData.boardID == 1))  //Password generator board ID == 1
				//				if (motionInitS2)  //Supper
			{
				index = index & 0x000F; //Language, 0/no password = chinese. 1 Traditional chinese, 3: English
				if (byhxData.manufacturerID == fwInfo.manufacturerID)
					security_getPassword(&EP0INdata[2], byhxData.manufacturerID, value, 0, index, 0x5678);
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x6A;
				ret = True;
			}
		}
		break;
	case 0x74: //IN, Generate INK password
		ret = False;
		if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_PRINT_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		{
			//index: permitted ink volume;		value: boardID
			if ((motionInitS2) && (value != 1) && (byhxData.boardID == 1))  //Password generator board ID == 1
				//if (motionInitS2)  //Supper
			{
				if (byhxData.manufacturerID == fwInfo.manufacturerID)
					security_getPassword(&EP0INdata[2], byhxData.manufacturerID, value, 0, index, 0x9ABC);
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x74;
				ret = True;
			}
		}
		break;
#endif
	case 0x60: //OUT, 移动小车及进退纸
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
		UIAction_Move_Start((void*)1, EP0OUTdata[0], *(__packed INT32S*)&EP0OUTdata[1]);
#else            
		move_dir = EP0OUTdata[0];
		move_distance = *(__packed INT32S*)&EP0OUTdata[1];
		move_type = 0;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif            
		break;	
#ifndef BYHX_WTITE_BOARD_TOOL
	case 0x61://OUT, set time limited password
		{	
			INT8U oldPWStr[EPR_TIME_PASSWORD_STR_SIZE];
			memcpy(oldPWStr, password.m_sTimePwd, EPR_TIME_PASSWORD_STR_SIZE);
			memcpy(password.m_sTimePwd, &EP0OUTdata[2], EPR_TIME_PASSWORD_STR_SIZE);

			if (security_CheckTimePassword()) //Password is OK
			{
				epromlen = EPR_TIME_PASSWORD_STR_SIZE;

				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_TIME_PASSWORD_STR_OFFSET, password.m_sTimePwd, &epromlen) == False)
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
				OSSemPost(IICSem);
			}else
				memcpy(password.m_sTimePwd, oldPWStr, EPR_TIME_PASSWORD_STR_SIZE);
		}
		break;
	case 0x69://OUT, set option password: language
		if (direction == 0)
		{	
			INT8U oldPwdStr[EPR_OPTION_PASSWORD_STR_SIZE];
			memcpy(oldPwdStr, password.m_sOptionPwd, EPR_OPTION_PASSWORD_STR_SIZE);
			memcpy(password.m_sOptionPwd, &EP0OUTdata[2], EPR_OPTION_PASSWORD_STR_SIZE);

			if (security_CheckOptionPassword()) //Password is OK
			{
				epromlen = EPR_OPTION_PASSWORD_STR_SIZE;

				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_OPTION_PASSWORD_STR_OFFSET, password.m_sOptionPwd, &epromlen) == False)
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
				OSSemPost(IICSem);
			}else
				memcpy(password.m_sOptionPwd, oldPwdStr, EPR_OPTION_PASSWORD_STR_SIZE);
		}else ret = False;
		break;
#endif	
	case 0x62://OUT, handshake
		memcpy(&checkboarddata,EP0OUTdata,64);
		if (checkboarddata.uiAlgorithm == 1)
		{
#ifdef GZ_KEY_USED
			if ((checkboarddata.swVersion & 0xFFFFFF00) >= (INT32U)0x00020200)
			{
				memcpy(tmp, "BYHX2010", 8);
				des(tmp, tmp, checkboarddata.uiInputValue, 1);

				if (memcmp(tmp, checkboarddata.desResult, 8)) //!=
				{
					g_shakehandPassed = False;
					status_ReportStatus(STATUS_FTA_SHAKEHAND_ERR, STATUS_SET);					
				}else
					g_shakehandPassed = True;
			}else
				status_ReportStatus(STATUS_FTA_SWVERSION_ERR, STATUS_SET);
#endif				
			*((INT32U *)&checkboarddata.uiInputValue[0]) ^= 0x42594858;  //"BYHX"
			*((INT32U *)&checkboarddata.uiInputValue[4]) ^= 0x42594858;  //"BYHX"
		}				
		break;

	case 0x63: 
		if (direction) //Get Random number
		{//使用第三个number
			srand(OSTimeGet());
			g_uiRandomNumber[0] = rand();
			g_uiRandomNumber[1] = rand();
			g_uiRandomNumber[2] = rand();
			g_uiRandomNumber[3] = rand();

			*((__packed INT32U *)&EP0INdata[2]) = g_uiRandomNumber[0];
			*((__packed INT32U *)&EP0INdata[6]) = g_uiRandomNumber[1];
			*((__packed INT32U *)&EP0INdata[10]) = g_uiRandomNumber[2];
			*((__packed INT32U *)&EP0INdata[14]) = g_uiRandomNumber[3];
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x63;
			*len = 25;
		}else
			ret = False;
		break;
	case 0x64: //OUT, Set clean para
#ifdef EPSON_CLEAN_UPDOWN

#ifdef FUNCTION_CLOSE_LCD
		if (direction) 
		{//读
			memcpy((void *) EP0INdata, (void *)&cleanPara, sizeof(CleanParaType));
			OutAutoClean_EPSON_Allwin((CleanParaType *) EP0INdata);
		}
		else
		{
			SetAutoClean_EPSON_Allwin((CleanParaType *) EP0OUTdata);
		}
#else
		SetAutoClean_EPSON_Allwin((CleanParaType *) EP0OUTdata);
		if (EP0OUTdata[0] <= 64)
		{
			cleanParaDirty = True;
		}
#endif		
#elif (defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3))&&!(defined(MANUFACTURER_MICOLOR_EPSON) || defined(MANUFACTURER_COLORFUL))
		if (EP0OUTdata[0] <= 64)
		{
			//memcpy(&cleanPara, EP0OUTdata, EP0OUTdata[0]);
			for (i = 0; i <EP0OUTdata[0];i++)
				((__packed INT8U *)&cleanPara)[i] = EP0OUTdata[i];
			cleanParaDirty = True;
		}
		//Cheney 20110623: Don't Permit PM set clean paramenter.
		//SetAutoClean_EPSON_Micolor((CleanParaType *) EP0OUTdata);
#endif
		break;
	case 0x65: //OUT, Save parameters, used for control panel, should saved to NAND Flash
		//unsigned short m_nResultFlag;
		//unsigned char m_nLen;  //长度
		//unsigned char m_nIndex; //流水号
		//unsigned char m_pData[64-4]; //参数数据			

		if (index +  EP0OUTdata[2] > EPR_HOST_PARA_MAXLEN)
			break;

		//			if (host_paras.saving == True)
		//				break;

		host_paras.saved = False;//index = offset, value = total size

		memcpy(&host_paras.buf[index], &EP0OUTdata[4], EP0OUTdata[2]);
		host_paras.length = value;

		if (host_paras.length > EPR_HOST_PARA_MAXLEN)
		{
			ret = False;
			break;
		}	

		if (index + EP0OUTdata[2] >= value)
		{
			host_paras.dirty = False;

			i = 0;
			host_paras.saved = True;
			while (i < host_paras.length + 4)
			{
				INT8U wrlen;
				if (host_paras.length + 4 - i > 64)  //Fix bugs
					wrlen = 64;
				else
					wrlen = host_paras.length +4 - i;					
				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_HOST_PARA_OFFSET+i, ((INT8U *)&host_paras)+i, &wrlen) == False)
				{
					CONSOL_Printf("Save EEPROM Failed!");
					OSSemPost(IICSem);
					break;
				}
				i += wrlen;	
				OSSemPost(IICSem);						 
			}
			if (i >= host_paras.length + 4)
				host_paras.saved = True; //Delete one line
			else
				host_paras.saved = False;
		}
		else
			host_paras.dirty = True;			
		break;
	case 0x66: //仅为调试，转发COM命令，value为端口号
		if (direction) //Get
		{
			for (i = 0; i < dspDebugInfo[0]; i++)
				EP0INdata[i+2] = dspDebugInfo[i];
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x66;
			*len = dspDebugInfo[0]+2;
		}	
		else
		{
#if defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP) && defined(EPSON_CLEAN_UPDOWN)        
			if(value ==UART_DSP_CHANNEL && EP0OUTdata[1] == 0x59)
			{
				status_ReportStatus(STATUS_MOVING, STATUS_SET);		              
				printratio_dirty = True;
				tmp[0] = 8; //Length
				tmp[1] = UART_AXISMOVETO_CMD; 
				tmp[2] = 4; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
				tmp[3] = TATE_CLEAN_CZ_SPEED_LEVEL;               
				*((__packed INT32S *)&tmp[4]) = 0; //move_distance means target position         
				while (!UART_SendCMD(UART_MOTION_CHANNEL, tmp)) 
					OSTimeDly(100);	
			}
			else		
#endif              
				UART_SendCMD(value, EP0OUTdata);
		}
		break;

	case 0x67: //OUT, 移动小车及进退纸到某一个位置
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
		UIAction_Move(0, EP0OUTdata[0], *(__packed INT32S*)&EP0OUTdata[1]);
#else
		move_dir = EP0OUTdata[0];
		move_distance = *(__packed INT32S*)&EP0OUTdata[1];  //move_distance means target position
		move_type = 3;
		status_ReportStatus(CMD_START_MOVE, STATUS_SET);
#endif            
		break;	
#ifdef UV_PRINTER
	case 0x68: //设置/查询UV灯
#ifdef COORD_NEW_UV      
		if (direction) //Get
		{
			*(__packed INT32U *)(&(EP0INdata[2])) = Ripstar_UV_Setting.UV_SETTING;
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x68;
			*len = 6;
		}
		else //control
		{
			Ripstar_UV_Setting.UV_SETTING = *(__packed INT32U *)(&(EP0OUTdata[0])); //EP0OUTdata[1] no use

			uv_UVMode = (Ripstar_UV_Setting.UV_SETTING >> 24);
			Ripstar_UV_Setting.UV_SETTING = (uv_UVMode<<16)|(Ripstar_UV_Setting.UV_SETTING&0xFF00FFFF);
#if defined(MANUFACTURER_DYSS)||defined(ORIC_FUNCTION)			
			uv_UVMode = (Ripstar_UV_Setting.UV_SETTING >> 24)/10;
#endif
			SaveFactoryUV_Setting();
			if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)	
			{
				Configure_Uv_Power(uv_UVMode);	
				if(EP0OUTdata[0] & (1<<4))
					Front_Light_on();
				else
					Front_Light_off();

				if(EP0OUTdata[0] & (1<<5))
					Back_Light_on();
				else
					Back_Light_off();	
			}
		}
#else
		if (direction) //Get
		{
			uv_GetStatus(&(EP0INdata[2]));
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x68;
			*len = 6;
		}
		else //control
		{
			uv_CtrUsrAction(EP0OUTdata[0]); //EP0OUTdata[1] no use
			uv_UVMode = EP0OUTdata[2];
		}
#endif
		break;
#endif
#ifndef BYHX_WTITE_BOARD_TOOL
	case 0x6B: //IN: 当前语言查询  Out 语言设置检查 value == language
		if (direction) //
		{
			if (password.m_bHaveOptionPwd == False)
				EP0INdata[2] = 0;
			else
				EP0INdata[2] = password.m_bLanguage;
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x6B;
			*len = 4;  //Should be 3????  20091110
		}else
		{
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			if((factoryData.m_nBitFlag & FDF_EPSON_LCD) == 0)
#endif
				if(!security_CheckSWUILanguageSetting((INT8U)(value&0xF), EP0OUTdata, True))
					status_ReportStatus(STATUS_FTA_UILANGUAGE_NOT_MATCH,STATUS_SET);
		} 
		break;	
#endif	

		//#ifdef Y_ENCODER_ENABLE
	case 0x6C: //Get following Info (1) int32 bandIndex, (2)  unsigned int m_nBandY (3)unsigned int m_nBandResPosY (4)int32 nEncoderPos,
		*((__packed INT32U *)&EP0INdata[2]) = g_yEncoderInf.checksum;
		*((__packed INT32S *)&EP0INdata[6]) = g_yEncoderInf.bandIndex;
		*((__packed INT32U *)&EP0INdata[10]) = g_yEncoderInf.encoderPos;
		*((__packed INT32U *)&EP0INdata[14]) = g_yEncoderInf.bandY;
		*((__packed INT32U *)&EP0INdata[18]) = g_yEncoderInf.nextBandY;
		*((__packed INT32U *)&EP0INdata[22]) = g_yEncoderInf.bandResY;
		*((__packed INT32U *)&EP0INdata[26]) = *((INT32U *)&(g_yEncoderInf.fStepTotal));
		//Debug
		//			if (g_yEncoderInf.encoderPos - ttttPos > 4000)
		//				status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG2, STATUS_SET);
		//			ttttPos = g_yEncoderInf.encoderPos;
		EP0INdata[0] = 0;
		EP0INdata[1] = 0x6C;
		*len = 30;
		break;
	case 0x6D:
		checksum ^= *(__packed INT32U*)&EP0OUTdata[0];
		checksum ^= *(__packed INT32U*)&EP0OUTdata[4];
		checksum ^= *(__packed INT32U*)&EP0OUTdata[8];
		checksum ^= *(__packed INT32U*)&EP0OUTdata[12];
		checksum ^= *(__packed INT32U*)&EP0OUTdata[16];

		if (checksum == 0)
		{
			g_adjustInf.checksum =  *(__packed INT32U*)&EP0OUTdata[0];
			g_adjustInf.passStepValue = *(__packed INT32S*)&EP0OUTdata[4];
			g_adjustInf.passAdvance = *(__packed INT32S*)&EP0OUTdata[8];
			g_adjustInf.feedBackY = *(__packed INT32S*)&EP0OUTdata[12];
			g_adjustInf.bidirectionValue = *(__packed INT32S*)&EP0OUTdata[16];
		}else
			ret = False;
		break;
		//#endif	
	case 0x70: //Set standard voltage: float: H0 L0 H1 L1
		for (i = 0; i < 4; i++) //64 / 4*2 * 2 = 16
		{
			INT8U j;
			INT16U addr = i*16;
#ifdef CONVERSION_BOARD
			tmp[0] = 22;
			tmp[21] = 1;
#else
			tmp[0] = 21;
#endif
			tmp[1] = UART_CMD_EEPROM_WRITE;
			tmp[2] = addr>>8;
			tmp[3] = addr;
			tmp[4] = 16;
			for (j = 0; j < 16; j++)
			{
				tmp[5+j] = EP0OUTdata[i*16+j];
			}
			UART_SendCMD(UART_HEAD_CHANNEL, tmp);
		}
		break;

	case 0x71: //Get DSP private data
		if (direction) //Get
		{
			for (i = 0; i < dspPrivateData[0]; i++)
				EP0INdata[i] = dspPrivateData[i];
			EP0INdata[0] = 0;
			EP0INdata[1] = 0x71;
			*len = dspPrivateData[0]+2;
		}	
		else
			ret = False;
		break;

	case 0x72: //None data setup
		//Write_SX2reg(SX2_EP0BC, 0); //Ack this request, ???
		//Set_DMA_OUT_EP2();
		HugBuf_InitReceiveSize();
		HugeBuf_StartOneReceive();  //Should find a place to start at the first time
		break;
#if 1			
	case 0x76:
		{
			INT8U flag = 0,CHAR_LEN;
			double g_dTotalPrintArea = 0;
			epromlen = sizeof(double);
			CHAR_LEN = sizeof(INT8U);
			OSSemPend(IICSem, 0, &err);
			if (IIC_ReadEEPRom(EPR_PRINT_AREA_OFFSET, (INT8U *)&flag, &CHAR_LEN) == False)
			{
				OSSemPost(IICSem);
				status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);
				ret = False;
				break;
			}
			if (IIC_ReadEEPRom(EPR_PRINT_AREA_OFFSET + 1, (INT8U *)&g_dTotalPrintArea, &epromlen) == False)
			{
				OSSemPost(IICSem);
				status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);
				ret = False;
				break;
			}
			OSSemPost(IICSem);
			if(flag != 0xCA)
			{
				flag = 0xCA;
				g_dTotalPrintArea = 0;
				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_PRINT_AREA_OFFSET, (INT8U *)&flag, &CHAR_LEN) == False)
				{
					OSSemPost(IICSem);
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
					ret = False;
					break;
				}
				if (IIC_WriteEEPRom(EPR_PRINT_AREA_OFFSET + 1, (INT8U *)&g_dTotalPrintArea, &epromlen) == False)
				{
					OSSemPost(IICSem);
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
					ret = False;
					break;
				}
				OSSemPost(IICSem);
			}
			if (direction) //Get
			{

				memcpy(&EP0INdata[2], &g_dTotalPrintArea, sizeof(double));
				EP0INdata[0] = 0;
				EP0INdata[1] = 0x76;
				*len = sizeof(double)+2;
			}
			else
			{
				double dTmp;
				memcpy(&dTmp, &EP0OUTdata[0], sizeof(double));
				g_dTotalPrintArea += dTmp;
				OSSemPend(IICSem, 0, &err);
				if (IIC_WriteEEPRom(EPR_PRINT_AREA_OFFSET + 1, (INT8U *)&g_dTotalPrintArea, &epromlen) == False)
				{
					OSSemPost(IICSem);
					status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
					ret = False;
					break;
				}
				OSSemPost(IICSem);
			}
			break;
		}
#endif			
		//Process vendor request with EP0 data here. Data stored in	EP0data[i]
	default: //We don't recognize the request
		ret = False; //Write_SX2reg(SX2_SETUP, 0xFF); //Stall the request
		break;
	}
	return ret;
}


void USB_Task(void* data)
{
	OS_CPU_SR cpu_sr;
	OS_FLAGS flags;

	INT8U err;
	INT8U Trytime = 0;

#if (INCLUDE_CONSOL == 1)
	//	CONSOL_Printf("Waiting USB Enumeration ...\r\n");
#endif
	OSFlagPend(mix_FLAG_GRP,USB_CONFIGURED,OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME,0,&err); 
#if (INCLUDE_CONSOL == 1)
	//	CONSOL_Printf("USB Enumeration OK!\r\n");
#endif

	//Adjust the USB speed
	while(USBD_IsHighSpeed() == 0)
	{
		TickAfterLastComm = MAX_TICK_INTO_OFFLINE;
		USBD_AbortDataWrite(USB_IN_EP);
		USBD_Disconnect();
		OSTimeDly(500);

		USBD_Connect();
		OSTimeDly(100);

		Trytime ++;

		if(Trytime >= 5)
		{            
			status_ReportStatus(STATUS_FTA_USB1_USB1CONNECT, STATUS_SET);
#if (INCLUDE_CONSOL == 1)
			CONSOL_Printf("USB is linked to a full speed host!\r\n");
#endif
			break;
		}
	}


#if 1
	HugeBuf_StartOneReceive();  //Should find a place to start at the first time
	//Begin SX2 FIFO2 data receive until huge buffer full
	//After each band is printed, should call this function one time
#endif

	for (;;)
	{
		flags = OSFlagPend(mix_FLAG_GRP, USB_SETUP_DAT|USB_CONFIGURED, OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME,0,&err); //Wait SX2 interrupt

		if(flags & USB_SETUP_DAT)
			HandleEP0Command(&setupRequest_ForTask, EP0INdata_ForTask, EP0OUTdata_ForTask);

		if(flags & USB_CONFIGURED)
		{
			if(!g_USBTransStoped)
			{
				USB_INSERT = True;
				if(USBD_IsHighSpeed() == 0) 
					EP0_STOP = True;
				USBD_AbortDataRead(USB_OUT_EP);	
				USBD_AbortDataWrite(USB_IN_EP);
				OSTimeDly(100);

				if(USBD_IsHighSpeed() == 0)
				{
					OSTimeDly(100);

					Trytime = 0;
					while(USBD_IsHighSpeed() == 0)
					{
						if(Trytime >= 0)    
						{            
							status_ReportStatus(STATUS_FTA_USB1_USB1CONNECT, STATUS_SET);
#if (INCLUDE_CONSOL == 1)
							CONSOL_Printf("USB is linked to a full speed host!\r\n");
#endif
							break;
						}

						TickAfterLastComm = MAX_TICK_INTO_OFFLINE;
						//USBD_AbortDataWrite(USB_IN_EP);
						USBD_Disconnect();
						OSTimeDly(500);

						USBD_Connect();
						OSTimeDly(300);

						Trytime ++;

					}
				}

				if(USBD_IsHighSpeed() != 0)
					status_ReportStatus(STATUS_FTA_USB1_USB1CONNECT, STATUS_CLR);

				OS_ENTER_CRITICAL();	
				HugBuf_InitReceiveSize();
				HugeBuf_StartOneReceive();  	
				CommPipe_StartOneSendEP6();
				OS_EXIT_CRITICAL(); 	

				if(USBD_IsHighSpeed() == 0)
				{
					while(USBD_IsHighSpeed() == 0)
					{
						status_ReportStatus(STATUS_FTA_USB1_USB1CONNECT, STATUS_SET);
						TickAfterLastComm = MAX_TICK_INTO_OFFLINE;
						USBD_Disconnect();
						OSTimeDly(500);

						USBD_Connect();
						OSTimeDly(500);
					}
					EP0_STOP = False;	
					status_ReportStatus(STATUS_FTA_USB1_USB1CONNECT, STATUS_CLR);				  
				}
				USB_INSERT = False;	
			}
		}
	}//Task body
}


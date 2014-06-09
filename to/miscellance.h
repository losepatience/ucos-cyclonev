#ifndef _MISCELLANCE_H_
#define _MISCELLANCE_H_

#include "global.h"
#include "../LCD_GUI/resources.h"
#if defined(MOTION_Y_DSP)||defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
#define UART_MOTION_CHANNEL	2
#define UART_DSP_CHANNEL	1
#else
#define UART_MOTION_CHANNEL 1
#endif
#define UART_HEAD_CHANNEL	0

//COM Command
#define UART_INIT_STAGE1_CMD	0x10
#define UART_INIT_STAGE2_CMD	0x11
#define UART_BEGIN_STAGE2_CMD	0x90
#define UART_PRINT_DIR_CMD		0x91 //To headboard

#define UART_SET_FACTORY_DATA_CMD	0x40

#define UART_DSP_SET_YDIR		0x46
#define UART_PRINTPARA_CMD		0xA5

#define UART_PRINT_CMD			0xE1
#define UART_PRINT_CMD_LEN		22//LEN + CMD + 20 parameters

#define UART_PRE_PRINT_CMD			0xE2
#define UART_PRE_PRINT_CMD_LEN		18 //LEN + CMD + 16 parameters

#define UART_MOVETO_CMD			0xD2
#define UART_STOPMOVE_CMD		0x3A //0xC3

#define UART_URGENTSTOP_CMD		0x39 //0xC3

#define UART_PAPERFEED_CMD		0x33
#define UART_STOPFEED_CMD		0x3C

/*  3 定长移动   0x31
Buf size = 1 + 1+ 4  

a) 一个字节表示轴 
enum JetDirection
{
JetDirection_Pos = 1,
JetDirection_Rev,
JetDirection_Forward,
JetDirection_Back,
JetDirection_Up_Z,
JetDirection_Down_Z
				  };
b)  一个字节表示速度
c） 一个int 表示 length */

#define UART_MOVEDISTANCE_CMD	0x31
#define UART_AXISMOVETO_CMD		0x41
#define UART_GOHOME_CMD			0x42
#define UART_MEASUREMEDIA_CMD	0x35
#define UART_STARTSWIPE_CMD		0x52

//DSP Checksum: The 3rd int(4 byte) in DSP file
#define UPDATE_DSP_CHKSUM_REQ	0x40

#define UART_UPDATE_DSP_BEGIN		0x3D
#define UART_UPDATE_DSP_DATA		0x3E
#define UART_UPDATE_DSP_END			0x3F
#define UART_UPDATE_DSP_INFO        0x40 //1 DATA:  

#define UART_DSP_RPT_PHYSICAL_SIZE	0x20  //DSP report x/y/z/f maxmum width

#define UART_DSP_RPT_STATUS			0x2A //5 DATA: 1 DATA(STATUS) + 4 DATA(X Postion)  			

#define UART_DSP_RPT_POSITION		0x22 //One couple get/set, use same value
#define UART_DSP_GET_POS_CMD		0x22

#define UART_DSP_RPT_DEBUG_INFO		0x2C //DSP report debug info, just translate to HOST
#define UART_DSP_RPT_PRIVATE_DATA	0x2D //DSP report private data, just translate to HOST

#define UART_DSP_RPT_PIPE_DATA	    0x2E //the pipe data that DSP report, just translate to HOST

#define UART_DSP_RPT_STOP			0xB4
#define UART_RPT_VERSION		0x15 //4 bytes version, 12 motion bytes date
#define UART_HEAD_RPT_OVERHEAT  0x16 //喷头加热超过了55度，所有加热被强行关闭。
#define UART_HEAD_RPT_FPGA_ERR  0x17 //头板带FPGA的，设置FPGA参数出错

#define UART_DSP_RPT_DESKEY		0x16
#define UART_DSP_RPT_MEDIA		0x29

#define UART_DSP_WRITE_EEPROM	0x47
#define	UART_DSP_REQ_READ_EEPROM 0x48

#define UART_RPT_DSP_EEPROM_CONTENT 0x49
#define UART_RPT_DSP_READ_ERR		0x4A
#define UART_RPT_DSP_WRITE_ERR		0x4B

#define UART_RPT_DSP_PASS_START		0x51

#define UART_RPT_DSP_SWIPE_COMPLETED	0x52

#define UART_RPT_DSP_NEEDLE	0x53

#define UART_DSP_STOP_PRINT	0x55

#define UART_RPT_DSP_MEASURE_HIGH	0x5D
#define UART_RPT_DSP_HIGH_VALUE	0x5E

#define UART_RPT_DSP_MEASURE_INIT	0x5F
#define UART_RPT_DSP_MEASURE_INIT_OVER	0x61
#define UART_RPT_DSP_MEASURE_TOUCH	0x62

#define UART_RPT_DSP_SENSOR_EX	0x63

#define UART_RPT_DSP_MEASURE_SENSOR_ONOFF	0x64

#define UART_DSP_MOVEDISTANCE_PERCENT 0x65
#define SciCmd_CMD_MoveCMDClose 0x66       // 移动命令关闭
#ifdef ALLWIN_EPSON_SAME
#define UART_DSP_PRINT_PAUSE 0x67
#endif

#define UART_DSP_Z_SAFE_POSI 0x68

#define UART_DSP_MANUAL_FIND_ORIGIN 0x69
#define UART_UPDATE_HB_BEGIN				0x3D
#define UART_UPDATE_HB_DATA				0x3E
#define UART_UPDATE_HB_END				0x3F
#define UART_UPDATE_HB_INFO				0x40
#define UART_HEAD_UPDATE_DATA_CMD	0x34
#define UART_HEAD_PUMP_BITS			0xA5
#define UART_HEAD_HV_POWERDOWN   	0x50  //Headboard report power down
#define UART_HEAD_VOLTAGE_SET		0x51
#define UART_HEAD_TEMEPRATUE_CMD	0x52  //Headboard report temperature
#define UART_HEAD_HEADTYPE_CMD		0x53  //Tell Headboard witch printhead is used, for Konica, 14/40PL logic
#define UART_HEAD_HEAT_TEMPERATURE_CMD		0x54  //加热的目标温度
#define UART_HEAD_DATA_TRANS_FREQUENCY_CMD  0x55  //设置对喷头的数据传输频率，对Spectra有效，对BYHX_HB_V4之后的板子有效
#define UART_HEAD_VOLTAGE_SET1				0x56  //对KM512 16头， 设置后16个电压
#define UART_HEAD_EPSON_JOBSTART			0x57  //对EPSON Gen5头，通知JOB开始。
//它带一个参数，用于指定是否为预定义的清洗JOB。为0，为普通JOB，否则为预定义的清洗JOB号。
//Add a byte to record job VSD type. same as  Struct_JobStart.VSDModel.
#define UART_HEAD_EPSON_PASSSTART			0x58  //对EPSON Gen5头，通知Pass开始。
#define UART_HEAD_EPSON_JOBEND				0x59  //对EPSON Gen5头，通知JOB结束。当针对普通JOB，只能在Pass结束后结束job。
#define UART_HEAD_EPSON_REQ_FPGA_STATUS		0x5A  //For epson. 
#define UART_HEAD_EPSON_REQ_HB_PARAM		0x5B  //For epson. 
#define UART_HEAD_EPSON_SET_RES				0x5C  //For epson. 
#define UART_HEAD_EPSON_OUT_VOLTAGE			0x5D  //For epson. Start a testing Model. output a target voltage.  
#define UART_HEAD_EPSON_OUT_WAVE			0x5E  //For epson. Start a testing Model. output a default wave.  
#define UART_HEAD_TEMEPRATUE_EX_CMD			0x5F  //For BYHX V5-8. 
#define UART_HEAD_EPSON_PARAM_PERCENT		0x63  //For epson. 
#define UART_HEAD_EPSON_SHEETCUT			0x64  //For epson. 

#define UART_HEAD_EPSON_SENSOR  	0x65  //For epson. only for large bottom board. Req/Report head sensor status. only report it when sensor status is changed.
#define UART_RESISTANCE_OFFSET     0x66  //For epson RESISTANCE_OFFSET.
#define UART_SET_RESISTANCE_OFFSET  0x67 
#define UART_HEAD_SET_HEATER_ENABLE  0x68
//head sensor status bit
#define HSSB_PSense0	0x0
#define HSSB_PSense1	0x1
#define HSSB_HVSense0	0x2
#define HSSB_HVSense1	0x3
#define HEAD_HEIGHT_LEVEL_SENSOR_BIT    HSSB_PSense0

#define UART_HEADRPT_VTRIM      0x41
#define UART_HEADRPT_PWM	    0x42
#define UART_HEADRPT_SINGLEBANK 0x43  
#define UART_HEADRPT_HEADINFO	0x44

#define UART_HEADSET_WFMSELECT  0x45  //开机设置，这个不能放在主界面上
#define UART_HEADSET_VTRIM		0x46
#define UART_HEADSET_SINGLEBANK 0x47  //开机设置

#define UART_HEAD_SENDCOM		0x48  //Reserved
#define UART_HEAD_COMRESPONSE   0x49  //Reserved
#define UART_HEAD_SET_CANGETSTATUS  0x4A //Reserved

#define UART_HEADSET_SINGLEBANK_SINGLE 0x4B
#define UART_HEADSET_WFMSELECT_SINGLE  0x4C

#define UART_CMD_EEPROM_WRITE	0x4D  //For spectra, write eeprom
#define UART_RPT_WRITE_EEPROM_RESULT 0x4E

#define UART_HEADRPT_EPSON_STATUS	0x4F  //For epson, report head status: the known status is Temperature is too high/low, head is bad, head is not connected. 
//	Add FPGA_DISABLE because of security chip.
//epson head status bit:
#define EHSB_ERROR					0x0
#define EHSB_TEMPERATURE_TOO_HIGH	0x1
#define EHSB_TEMPERATURE_TOO_LOW	0x2
#define EHSB_HEAD_IS_BAD			0x3
#define EHSB_HEAD_NOT_CONNECTED		0x4
#define EHSB_FPGA_ERROR				0x5
#define EHSB_FPGA_XHOT				0x6
#define EHSB_FPGA_DISABLE			0x7
#define EHS_WARNING_MASK			0x56
#define EHS_ERROR_MASK				0xA8

#define UART_HEADRPT_FPGA_STATUS	0x50  //For epson. 
//epson head FPGA bit:
#define EHFB_FPGA_ERROR				0x0
#define EHFB_FPGA_ENABLE	        0x10
#define EHFB_HB_DRIVER_OVERHEAT     0x40

#define UART_HEADRPT_HB_PARAM		0x51  //For epson. 

#define UART_HEADRPT_PIPE_DATA		0x60 

//SPECTRA HVB uart value must between 0x60~0x6F
#define UART_HEAD_SPECTRA_HVB_INITSTAGE1_OK			0x60
#define UART_HEAD_SPECTRA_HVB_INITSTAGE2_OK 		0x61
#define UART_HEAD_SPECTRA_HVB_VOLTAGE_REPORT		0x62

#define UART_HEAD_SPECTRA_HVB_BEGIN_INITSTAGE2_CMD 	0x68
#define UART_HEAD_SPECTRA_HVB_VOLTAGE_SET			0x69
#define UART_HEAD_PULSEWIDTH_SET		0x6A

#define UART_HEAD_WAVE_SET_FINISH  0x6B

#define UART_HEAD_SET_ENCRYPT  0x6C
#define UART_HEAD_SET_PUMP_MASK  0x6D
#ifdef WAVE_SET_FUN
#define WAVE_DATA_MAX_LEN  25
#define UART_HEAD_WAVE_READ  0x70
#define UART_HEAD_WAVE_TRANSFER  0x71
#define UART_HEAD_WAVE_SET  0x72
#define UART_HEAD_SWING_SET  0x73
#define UART_HEAD_POINT_SET  0x74
#define UART_HEAD_WAVE_READY  0x75
#define UART_HEAD_WAVE_NAME_READ  0x77
#define UART_HEAD_WAVE_NAME_TRANSFER  0x78
#define UART_HEAD_WAVE_NAME_SET  0x79
#define UART_HEAD_WAVE_CHANNEL_SET  0x7A
#define UART_HEAD_WAVE_CHANNEL_READ 0x7B
#define UART_HEAD_WAVE_CHANNEL_TRANSFER  0x7C
extern INT16U DATA_LEN;
extern INT8U TRANSFER_MODE;
extern INT8U WAVE_BUF[];
extern INT8U WAVE_NAME_BUF[];
extern INT8U WAVE_MAP_TABLE[MAX_HEAD_DRIVE_NUM * 2];
#endif
#if defined(FUNC_WAVE_MAP)&& !defined(WAVE_SET_FUN)
#define WAVE_DATA_MAX_LEN  25
#define UART_HEAD_WAVE_NAME_READ  0x77
#define UART_HEAD_WAVE_NAME_TRANSFER  0x78
#define UART_HEAD_WAVE_CHANNEL_SET  0x7A
#define UART_HEAD_WAVE_CHANNEL_READ 0x7B
#define UART_HEAD_WAVE_CHANNEL_TRANSFER  0x7C
extern INT16U DATA_LEN;
extern INT8U WAVE_NAME_BUF[];
extern INT8U TRANSFER_MODE;
extern INT8U WAVE_MAP_TABLE[MAX_HEAD_DRIVE_NUM * 2];
#endif

#define UART_UPDATE_AP_BEGIN 0X76 
//Limted time warning value:
#define TIME_LIMIT_WAR_FIRST		100	//100 hours
#define TIME_LIMIT_WAR_SECOND		50	//50 hours
#define TIME_LIMIT_WAR_LAST			1

#define TIME_LIMIT_EPR_WRT_UNIT		600  //Second

#define INK_LIMIT_WAR_FIRST		10	//10 liter
#define INK_LIMIT_WAR_SECOND		5	//5 Liter
#define INK_LIMIT_WAR_LAST			1

#define INK_VOLUME_FACTOR_PER_LITER 0x38D7
#define INK_LIMIT_EPR_WRT_UNIT		0xE //write e2prom per print 1 ml ink.
#define INK_DOT_CHECK_MASK			0x738989c2

#define	USB_READ_SETUP_DAT	  0x00000001
#define	USB_SETUP_DAT		  0x00000002
#define	USB_CONFIGURED		  0x00000004

#define MISC_FLAGS_UPDATE_HB	0x00000010
#define MISC_UPDATE_HB_ERROR	0x00000020

//IIC interruped
#define IIC_EEPROM_INT_FLAG  0x00000100
#define IIC_EEPROM_FAIL_FLAG 0x00000200
#define IIC_KEYBOARD_INT_FLAG  0x00000400
#define IIC_KEYBOARD_FAIL_FLAG 0x00000800
#define IIC_KEY_FLAG  0x00001000 //Key pressed or release

#define	MISC_FLAGS_BELLMENU  0x00000008 //for invalid menu command.
#define MISC_FLAGS_BELL		 0x00002000
#define MISC_FLAGS_BELLLVDS  0x00004000
#define MISC_FLAGS_UPDATEDSP 0x00008000  //For DSP updating

//EPSON calibration 
#define EPSON_CALI_RUN		0x00010000
#define EPSON_CALI_CANCEL	0x00020000

//Capping status
#define EPSON_DO_CAPPING	    0x00040000
#define EPSON_DO_PENDING_MOVE	0x00080000

//Micolor Epson cleaning motor movement state
#define EPSON_CLEAN_MOTOR_RUN   0x00100000

//USB EP6 pipe 
#define COMM_PIPE_COMPLETED     0x00200000

//send new CMD for ARM motion system.
#define MOTION_SEND_CMD         0x00400000
#ifdef SUPPORT_MOTOR_CONTROL_ONLY_STEP 
#define MOTION_MOVE_COMPLETED_Z 0x00800000
#define MOTION_MOVE_COMPLETED_C 0x01000000
#define MOTION_MOVE_COMPLETED_Z_DRIVER 0x02000000
#define MOTION_MOVE_COMPLETED_C_DRIVER 0x04000000
#else
#define MOTION_MOVE_COMPLETED_X 0x00800000
#define MOTION_MOVE_COMPLETED_Y 0x01000000
#define MOTION_MOVE_COMPLETED_Z 0x02000000
#define MOTION_MOVE_COMPLETED_C 0x04000000
#endif
#define MOTION_REPORT_CMD       0x08000000
#define MOTION_URGENT_STOP_CMD  0x10000000 //until now, it's NOT used.
#define LCD_BUSY_STATE  0x20000000 
#define Reset_Key_STATE  0x40000000 
#define CLEAN_FILL_STEP1_STATE 0x80000000
//MISCELLANCE Flags 
//Hardware configure

/*mix_FLAG_GRP_2*/
#define HEAD_WAVE_SET_FINISH 0x00000001
#define DYSS_UV_LIGHT_CAL 0x00000002

#define HARDWARE_CONFIGURE
#define HW_CNF_TAG		0x46435748
#define HW_CNF_CUR_VERSON	0x00010001
#define HW_CNF_BIT_SDRAM_SIZE  	0x00000003
#define HW_CNF_SDRAM_SIZE_64M	0x00000000
#define HW_CNF_SDRAM_SIZE_128M	0x00000001

#if defined(HEAD_RICOH_G4)&&defined(ONE_BIT_DATA)
extern volatile INT8U RIPSTAR_PRT_BIT;
#else
extern const INT8U RIPSTAR_PRT_BIT;
#endif
#ifdef HEAD_EPSON_GEN5
#define EPSON_DATA_PER_FIRE	((180*2*2*((GetPhysicsHeadNum()+2-1)/2))/RIPSTAR_PRT_BIT)
#define EPSON_MIN_DATA_UNIT 2
#define EPSON_PREPRINT_SHAKE_NUM 144
//extra add 2 fire, because that head board sends data is later than main board fire one clock. 
//And most of epson fire wave contains two fire. so I add two fire. and generate one empty fire data and one real fire data.
#define EPSON_PRE_EXTRA_FIRE_NUM (1+1+EPSON_PREPRINT_SHAKE_NUM+ EPSON_MIN_DATA_UNIT) 
#define EPSON_POST_EXTRA_FIRE_NUM (1+1) 
#elif defined(HEAD_RICOH_G4) 
#if defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
#define EPSON_DATA_PER_FIRE	((320*2*2*((GetPhysicsHeadNum()+4-1)/4))/RIPSTAR_PRT_BIT)
#else
#define EPSON_DATA_PER_FIRE	((192*2*2*((GetPhysicsHeadNum()+8-1)/8))/RIPSTAR_PRT_BIT)
#endif
#define EPSON_PRE_EXTRA_FIRE_NUM 4 
#define EPSON_POST_EXTRA_FIRE_NUM 1 
#endif
#define EPSON_TOTAL_EXTRA_FIRE_NUM (EPSON_PRE_EXTRA_FIRE_NUM + EPSON_POST_EXTRA_FIRE_NUM) 


void IIC_Init(void);
INT8U KB_Init(void);
INT8U KB_Init_Again(void);
INT8U IIC_GetKey(INT8U *key, INT8U *repeat, INT8U *funckey);  
INT8U IIC_LED_ON(INT8U sno);
INT8U IIC_LED_FLASH(INT8U sno);  
void Printer_Init(void);
INT8U UART_SendCMD(INT8U channel, INT8U *data);
INT8U UART_GetCMD(INT8U channel, INT8U *data);
void UART_SetCheckModel(INT8U channel, INT8U model);
void UART_Init(INT8U u1Inc);
#if defined(SUPPORT_MOTOR_CONTROL)
INT8U UART_MotionReportCMD(INT8U *data);
INT8U UART_MotionGetCMD(INT8U *data);
#endif

void timer4_init(void);

INT8U IIC_WriteEEPRom(INT16U subaddr, INT8U *buf, INT8U *len);
INT8U IIC_WriteEEPRom_Ex(INT16U addr, INT8U *buf, INT16U *len);
INT8U IIC_WriteEEPRom_Head(INT16U addr, EPM_Head *buf);
INT8U IIC_ReadEEPRom(INT16U subaddr, INT8U * buf, INT8U * len);
INT8U IIC_ReadEEPRom_Ex(INT16U addr, INT8U * buf, INT16U * len);
INT8U IIC_ReadEEPRom_Head(INT16U addr, EPM_Head *buf);
INT8U IIC_ReadEEPRom_Struct(INT16U addr, INT8U * buf, INT16U bufsize, EPM_Head * phead);
INT16U CalcCheckSum(INT8U * buf, EPM_Head * phead);

#endif

#include "os_includes.h"
#include "global.h"
#include "consol.h"
#include "status.h"
#include "keylcd.h"
#include "miscellance.h"
#include "FPGA.h"
#include "HugeBuf.h"

#include "pio/pio.h"
#include "pio/pio_it.h"
#include "tc/tc.h"
#include "pmc/pmc.h"
#include "irq/irq.h"
#include "clean.h"

#include "motion.h"
#include "CommPipe.h"
#include "usb.h"

#if defined(EPSON_PRT_FLASH)
#define FPGA_POSI_COUNT   1
INT8U bInited_FPGAPosiIT = False;
struct sFpgaPosiITInfo
{
    FPGAPosi_IntHandler Handler;
    void *Data;
    INT8U Enable;
    INT32U Position;
    //INT8U PosiID;
    INT8U Option;
    
    INT8U bPending;
}FpgaPosiITInfos[FPGA_POSI_COUNT] = {0};

struct sFpgaPosiITInfo FpgaPosiITInfos_pending[FPGA_POSI_COUNT] = {0};

struct PrintInfo
{
    INT32S lastEndCoor;
    
    INT32S PrintMove_Start;
    INT32S PrintMove_End;
    INT32S PrintFire_Start;
    INT32S PrintFire_End;
    INT32S PrintFlash_Start;
    INT32S PrintFlash_End;
    INT32S PrintFlash_Start_1;
    INT32S PrintFlash_End_1;
    INT32S Y_Step;
    
    INT32U Y_Step_WaitingTime;
    INT32U WaitCompletedFlag;
    
    INT16U baseDPI;
    
    INT8U encoder_div;
    INT8U YDir; //0: Normal printing; 1: reverse printing
    INT8U VSDModel;
    INT8U autoSpray;
    INT8U SprayPosType; //0, pre-spray, pos dir; 1, post-spray, pos dir; 2,  post-spray, neg dir; 3, pre-spray, neg dir.
    INT8U curSprayNo;
    INT32U PreFireNum;
    INT32U PostFireNum;
    INT8U b_first_pass;
    
    INT8U PrintMoveStat; // 0: idle, 1: print move, 2: return move. 3: Only Y step.
    
    INT8U YSpeed;
    
} printInfo = {0};

#endif
//#define STEPPER_ROTIO_SCALE (float)(0.25)
#ifdef MANUFACTURER_LECAI_EPSON_4H
#define STEPPER_ROTIO_SCALE 5
#else
#define STEPPER_ROTIO_SCALE 2
#endif

#ifdef SUPPORT_MOTOR_CONTROL_ONLY_STEP 
//#define T1828EX
//#define W1615EX
//#define SJ1645
//#define WJ1545
//#define WJ1345
//#define S1828EX
//#define W1815EX
//#define S2628EX  //feed-paper roll's D is 40.43, old is 28.2.
//#define R600
//#define LARGE_HB_MOTION_COMMON
//#define A1802
//#define TATE_BEMAJET
//#define ALLWIN_E180
extern INT8U MOTION_ZC_INIT_OK;

struct EPR_MOTION_PARAM motionParam SLOW_DATA_SECTION;
static struct EPR_MOTION_PARAM motionParam_default = 
#if 1
{  
	MOTION_PARAM_VERSION, sizeof(struct EPR_MOTION_PARAM), 4,
	{
		{//for Cleaner z stepper motor.
			MOTION_AXIS_PARAM_VERSION, sizeof(struct EPR_MOTION_AXIS_PARAM), 
			AXIS_ID_Z, MT_Stepper, CT_OPEN_LOOP, DT_PulseAndDir, SO_ONLY_INCREMENT_MOVEMENT,
			1.0f, {1.0f, 1.0f, 1.0f}, 
			{   0   },
			{   0   },
			0xFFFFFFFF,    //origin sensor offset, about 5.3mm
			0xFFFFFFFF,    //start point (for micolor, it is cleaning point), about 4mm
			0xFFFFFFFF,  //end point, about 135.5cm.
			{   0xFFFFFFFF, //start point (for micolor, it is cleaning point), about 4mm
			0xFFFFFFFF, //origin sensor offset, about 5.3mm
			0xFFFFFFFF,  //print region's start point, about 18cm
			0xFFFFFFFF, //print region's end point, about 130cm
			0xFFFFFFFF, //end point, about 135.5cm.
			0xFFFFFFFF, //end point of measure movement, about 131cm
			0xFFFFFFFF, // end point of cut sheet movement. about 135cm.
			0xFFFFFFFF
			},
			0, {PWMTYPE_DEFAULT, PWMTYPE_DEFAULT, PWMTYPE_DEFAULT}, 
		#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
			0x01, 0xFFFF, 
			{   4*32 + 14, 0xFF, 0*32 + 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
		#else
			0x01, 0xFFFF, 
			{   4*32 + 14, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
		#endif
			0xFB, {0*32+4, 0*32+5, 1*32+5, 0},
			MAX_Z_C_SPEED_LEVEL, 
			{    
				{AT_Trapezoid, 100, 400*STEPPER_ROTIO_SCALE, 22.5*400*STEPPER_ROTIO_SCALE, 0},
				{AT_Trapezoid, 100, 800*STEPPER_ROTIO_SCALE, 22.5*800*STEPPER_ROTIO_SCALE, 0}, 
				{AT_Trapezoid, 100, 2000*STEPPER_ROTIO_SCALE, 22.5*2000*STEPPER_ROTIO_SCALE, 0}, 
				{AT_Trapezoid, 100, 4000*STEPPER_ROTIO_SCALE, 22.5*4000*STEPPER_ROTIO_SCALE, 0},  
				{AT_Trapezoid, 100, 8000*STEPPER_ROTIO_SCALE, 22.5*8000*STEPPER_ROTIO_SCALE, 0}, 
				{AT_Trapezoid, 100, 10000*STEPPER_ROTIO_SCALE, 22.5*10000*STEPPER_ROTIO_SCALE, 0},  
				{AT_Trapezoid, 100, 20000*STEPPER_ROTIO_SCALE, 22.5*20000*STEPPER_ROTIO_SCALE, 0},  
				0
			}
		},
		{//for Cleaner WIPER stepper motor.
			MOTION_AXIS_PARAM_VERSION, sizeof(struct EPR_MOTION_AXIS_PARAM), 
			AXIS_ID_C, MT_Stepper, CT_OPEN_LOOP, DT_PulseAndDir, SO_ONLY_INCREMENT_MOVEMENT,
			1.0f, {1.0f, 1.0f, 1.0f}, 
			{   0   },
			{   0   },
			0xFFFFFFFF,    //origin sensor offset, about 5.3mm
			0xFFFFFFFF,    //start point (for micolor, it is cleaning point), about 4mm
			0xFFFFFFFF,  //end point, about 135.5cm.
			{   0xFFFFFFFF, //start point (for micolor, it is cleaning point), about 4mm
			0xFFFFFFFF, //origin sensor offset, about 5.3mm
			0xFFFFFFFF,  //print region's start point, about 18cm
			0xFFFFFFFF, //print region's end point, about 130cm
			0xFFFFFFFF, //end point, about 135.5cm.
			0xFFFFFFFF, //end point of measure movement, about 131cm
			0xFFFFFFFF, // end point of cut sheet movement. about 135cm.
			0xFFFFFFFF
			},
			1, {PWMTYPE_DEFAULT, PWMTYPE_DEFAULT, PWMTYPE_DEFAULT}, 
			0x01, 0xFFFF, 
			{  4*32+13, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
			0xFB, {0*32+1, 0*32+2, 1*32+4, 0 },
			MAX_Z_C_SPEED_LEVEL, 
			{    
				{AT_Trapezoid, 100, 400*STEPPER_ROTIO_SCALE, 22.5*400*STEPPER_ROTIO_SCALE, 0},
				{AT_Trapezoid, 100, 800*STEPPER_ROTIO_SCALE, 22.5*800*STEPPER_ROTIO_SCALE, 0}, 
				{AT_Trapezoid, 100, 2000*STEPPER_ROTIO_SCALE, 22.5*2000*STEPPER_ROTIO_SCALE, 0}, 
				{AT_Trapezoid, 100, 4000*STEPPER_ROTIO_SCALE, 22.5*4000*STEPPER_ROTIO_SCALE, 0},  
				{AT_Trapezoid, 100, 8000*STEPPER_ROTIO_SCALE, 22.5*8000*STEPPER_ROTIO_SCALE, 0}, 
				{AT_Trapezoid, 100, 10000*STEPPER_ROTIO_SCALE, 22.5*10000*STEPPER_ROTIO_SCALE, 0},  
				{AT_Trapezoid, 100, 20000*STEPPER_ROTIO_SCALE, 22.5*20000*STEPPER_ROTIO_SCALE, 0},  
				0
			}
		},
		{//for Cleaner z stepper motor DRIVER.
			MOTION_AXIS_PARAM_VERSION, sizeof(struct EPR_MOTION_AXIS_PARAM), 
			AXIS_ID_Z_DRIVER, MT_Stepper, CT_OPEN_LOOP, DT_PulseAndDir, SO_ONLY_INCREMENT_MOVEMENT,
			1.0f, {1.0f, 1.0f, 1.0f}, 
			{   0   },
			{   0   },
			0xFFFFFFFF,    //origin sensor offset, about 5.3mm
			0xFFFFFFFF,    //start point (for micolor, it is cleaning point), about 4mm
			0xFFFFFFFF,  //end point, about 135.5cm.
			{   0xFFFFFFFF, //start point (for micolor, it is cleaning point), about 4mm
			0xFFFFFFFF, //origin sensor offset, about 5.3mm
			0xFFFFFFFF,  //print region's start point, about 18cm
			0xFFFFFFFF, //print region's end point, about 130cm
			0xFFFFFFFF, //end point, about 135.5cm.
			0xFFFFFFFF, //end point of measure movement, about 131cm
			0xFFFFFFFF, // end point of cut sheet movement. about 135cm.
			0xFFFFFFFF
			},
			2, {PWMTYPE_DEFAULT, PWMTYPE_DEFAULT, PWMTYPE_DEFAULT}, 
		#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
			0x01, 0xFFFF, 
			{   4*32 + 14, 0xFF, 0*32 + 2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
		#else
			0x01, 0xFFFF, 
			{   4*32 + 14, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
		#endif
			0xFB, {0*32+4, 0*32+5, 1*32+5, 0},
			MAX_Z_C_SPEED_LEVEL, 
			{    
				{AT_Trapezoid, 100, 400, 22.5*400, 0},
				{AT_Trapezoid, 100, 800, 22.5*800, 0}, 
				{AT_Trapezoid, 100, 2000, 22.5*2000, 0}, 
				{AT_Trapezoid, 100, 4000, 22.5*4000, 0},  
				{AT_Trapezoid, 100, 8000, 22.5*8000, 0}, 
				{AT_Trapezoid, 100, 10000, 22.5*10000, 0},  
				{AT_Trapezoid, 100, 20000, 22.5*20000, 0},  
				0
			}
		},
		{//for Cleaner WIPER  stepper motor DRIVER.
			MOTION_AXIS_PARAM_VERSION, sizeof(struct EPR_MOTION_AXIS_PARAM), 
			AXIS_ID_C_DRIVER, MT_Stepper, CT_OPEN_LOOP, DT_PulseAndDir, SO_ONLY_INCREMENT_MOVEMENT,
			1.0f, {1.0f, 1.0f, 1.0f}, 
			{   0   },
			{   0   },
			0xFFFFFFFF,    //origin sensor offset, about 5.3mm
			0xFFFFFFFF,    //start point (for micolor, it is cleaning point), about 4mm
			0xFFFFFFFF,  //end point, about 135.5cm.
			{   0xFFFFFFFF, //start point (for micolor, it is cleaning point), about 4mm
			0xFFFFFFFF, //origin sensor offset, about 5.3mm
			0xFFFFFFFF,  //print region's start point, about 18cm
			0xFFFFFFFF, //print region's end point, about 130cm
			0xFFFFFFFF, //end point, about 135.5cm.
			0xFFFFFFFF, //end point of measure movement, about 131cm
			0xFFFFFFFF, // end point of cut sheet movement. about 135cm.
			0xFFFFFFFF
			},
			3, {PWMTYPE_DEFAULT, PWMTYPE_DEFAULT, PWMTYPE_DEFAULT}, 
			0x01, 0xFFFF, 
			{  4*32+13, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
			0xFB, {0*32+1, 0*32+2, 1*32+4, 0 },
			MAX_Z_C_SPEED_LEVEL, 
			{    
				{AT_Trapezoid, 100, 400, 22.5*400, 0},
				{AT_Trapezoid, 100, 800, 22.5*800, 0}, 
				{AT_Trapezoid, 100, 2000, 22.5*2000, 0}, 
				{AT_Trapezoid, 100, 4000, 22.5*4000, 0},  
				{AT_Trapezoid, 100, 8000, 22.5*8000, 0}, 
				{AT_Trapezoid, 100, 10000, 22.5*10000, 0},  
				{AT_Trapezoid, 100, 20000, 22.5*20000, 0},  
				0
			}
		} 
	}
};
#else
{
	0
};
#endif

enum MotionStatus
{
	MS_IDLE,
	MS_NORMAL_MOVE,
	IS_Z_LeaveOriginSensor,
	IS_Z_GoOriginSensor,
	IS_Z_GoHome,
	IS_C_LeaveOriginSensor,
	IS_C_GoOriginSensor,
	IS_C_GoHome,
	IS_C_OK
};

enum MotionStatus mainStatus FAST_DATA_SECTION;

INT8U MultiAxis = 0;

struct SensorInfo
{
	INT8U IsValid;
	INT8U IsFPGAExpandIO;
	INT8U ValidLevel;
	//for normal IO.
	Pin PinIO;
	//for FPGA expand sensor IO.
	INT16U FPGABit;
};


struct ControlIOInfo
{
	INT8U ValidLevel;
	Pin PinIO;
};

//memory alloc
// from HUGEBUF_END_ADDR
// the last 128KB is the buffer of sCurve point.
// the second last 64KB is the buffer of sCurve point.
// The first is all Curve, to DDR2 end, total (2M-192KB)
#define MAX_POINT_NUM   (16*1024+1)
#define POINT_POOL_SIZE     (MAX_POINT_NUM * sizeof(struct Point))
#define POINT_POOL   ((struct Point*)(AT91C_DDR2 + BOARD_DDRAM_SIZE - POINT_POOL_SIZE))
#define STEP_POOL_SIZE      (MAX_POINT_NUM * sizeof(float))
#define STEP_POOL   ((float*)(AT91C_DDR2 + BOARD_DDRAM_SIZE - POINT_POOL_SIZE - STEP_POOL_SIZE))

#define ACCTABLE_ADDR   HUGEBUF_END_ADDR

#define ACCTYPE_DISTANCE            0
#define ACCTYPE_CYCLETIME           1
#define ACCTBL_SHIFT    ((INT32U)(((struct ACCCurveInfo *)0)->IncTable))
#define GET_ACCTBL_HEAD(a)    ((struct ACCCurveInfo *)((INT32U)(a) - ACCTBL_SHIFT))
#define GET_ACCTBL_DISTBL(a)    ((INT32U*)((a)->IncTable + (a)->AccDisTblOffset))
#define ACC_DISTANCE_SHIFT_BIT   8
struct ACCCurveInfo
{
	INT32U AccType; //Increment distance, Cycle Time
	INT32U AccCurveLen;
	INT32U AccDistance; //based on the coor system of IncTable.
	INT32U AccDisTblOffset; 
	INT8U IncTable[4]; //the real length is AccTableLen.
};

struct ACCCurveIndexTable
{
	struct ACCCurveInfo * pAllCurveZ[MAX_Z_C_SPEED_LEVEL];
	struct ACCCurveInfo * pAllCurveC[MAX_Z_C_SPEED_LEVEL];
	struct ACCCurveInfo * pAllCurveZ_Driver[MAX_Z_C_SPEED_LEVEL];
	struct ACCCurveInfo * pAllCurveC_Driver[MAX_Z_C_SPEED_LEVEL];
} AccCurveIndexTbl;
INT32U currCurveBuf = 0; //ACCTABLE_ADDR;

struct Point
{
	float x,y;
};
struct BezierCurve
{
	struct Point p0,p1,p2,p3;
};

#define PID_SHIFT_BIT   8
#define PID_I_SHIFT_BIT   12

#define DIRTYPE_STANDBY 0
#define DIRTYPE_POS     1
#define DIRTYPE_NEG     2

#define MOVETYPE_CONTINUE   0
#define MOVETYPE_SEGMENT    1
#define MOVETYPE_CONTINUE_CHECKSENSOR   2

#define ACCSEGTYPE_CONSTANT_SPEED   0
#define ACCSEGTYPE_SPEED_UP         1
#define ACCSEGTYPE_SPEED_DOWN       2

//#define PWM_FREQ_MOVE   40000   //40khz
#define PWM_FREQ_MOVE   20000   //20khz
#define PWM_FREQ_STANDBY   (PWM_FREQ_MOVE/1)   //40khz
//#define PWM_FREQ_MOVE   10000   //40khz
//#define PWM_FREQ_STANDBY   (PWM_FREQ_MOVE/1)   //10khz

#define PWM_STANDBY_CYCLE   ((BOARD_MCK/PWM_FREQ_STANDBY))
#define PWM_MOVE_CYCLE   ((BOARD_MCK/PWM_FREQ_MOVE))

#define PWM_MAX_LOAD_NUM    4000
#define PWM_DEAD_TIME_STA6940M  (BOARD_MCK*3/1000000)  //3us
#define PWM_DEAD_TIME_8412      (BOARD_MCK*5/10000000)  //0.5us, ideal is 5.5ns!!!
#define PWM_DEAD_TIME PWM_DEAD_TIME_8412

#define PWM_X_PERMIT_ERROR  20
#define PWM_Y_PERMIT_ERROR  2

//the slowest freq = MCK/128/0xFFFF = 16HZ.
#define TC_DIVIDE_NUM   128
#define TC_TIMER_CLOCK_SRC  AT91C_TC_CLKS_TIMER_DIV4_CLOCK

//only for 
//  1. 带驱动器和没有编码器的Y向 (not use until now) 。
//  2. 用于Z和清洗的Stepper电机。
struct AxisParam_PulseDir_OpenLoop
{
	INT8U  AxisID;
	INT8U  ChannelNO;
	
	struct SensorInfo Origin;
	INT32S OriginSensorOffset;
	struct SensorInfo Limit;
	
	INT8U IsCheckTerm;
	INT32S Origin_Offset;
	INT32S Region_Start; //-1 means Unkown and NO-limitation
	INT32S Region_End; //-1 means Unkown and NO-limitation.
	
	//channel info.
	AT91PS_TC pTC_channel;    
	struct ControlIOInfo TC_DirPin;
	INT8U ChannelValidLevel[2];
	struct ControlIOInfo ControlIO;
	
	//usually, be 1.
	//Fix point number which frac bit is 24. 
	INT32U ratio_User2Control; 
	float ratio_move; 
	
	//the target coor when stop. the real coor can be different with ideal coor. the next movement will compensate the offset..
	INT32S CoorIdeal; 
	INT32S CurCoor;
	
	INT8U Dir; //0,standby, 1, positive, 2, negative.
	INT8U MoveType; //0, segment move; 1, continue move, and stop when receive stop cmd.
	INT8U AccSegment; //0: constant, 1: speed up, 2: speed down.
	
	INT32U speedDownCoor;
	INT32U speedDownIndex;
	
	INT16U * pAccTable; //the next cycle-time after a cycle. 
	INT32U AccTableLen;
	INT32U AccTableIndex;
	
	struct ACCCurveInfo ** pAllCurveTable;
	
	INT32U Min_Distance;
	
	struct EPR_MOTION_AXIS_PARAM * pAxisParam;
};

static struct AxisParam_PulseDir_OpenLoop ZParam FAST_DATA_SECTION;
static struct AxisParam_PulseDir_OpenLoop CParam FAST_DATA_SECTION;
static struct AxisParam_PulseDir_OpenLoop Z_DriverParam FAST_DATA_SECTION;
static struct AxisParam_PulseDir_OpenLoop C_DriverParam FAST_DATA_SECTION;

static struct AxisParam_PulseDir_OpenLoop *pTC_Driver1 FAST_DATA_SECTION=NULL;
static struct AxisParam_PulseDir_OpenLoop *pTC_Driver2 FAST_DATA_SECTION=NULL;
static struct AxisParam_PulseDir_OpenLoop *pTC_Driver3 FAST_DATA_SECTION=NULL;
static struct AxisParam_PulseDir_OpenLoop *pTC_Driver4 FAST_DATA_SECTION=NULL;

void TCHandler(void);
INT8U TC_Stop_Move(struct AxisParam_PulseDir_OpenLoop * pStepperTC);

#if defined(EPSON_PRT_FLASH)

#ifdef HEAD_EPSON_GEN5    

void SetEpsonRegShort(INT16U addr, INT16U data)
{
    do
    {
        rFPGA_EPSON_RegAddr	= addr;
        __asm("NOP");
        __asm("NOP");
        rFPGA_EPSON_RegPortL = data;
        __asm("NOP");
        __asm("NOP");
        if(rFPGA_EPSON_RegPortL != data)
        {
            __asm("NOP");
            __asm("NOP");
#ifdef DEBUG_FPGA_BUS
            status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG2, STATUS_SET);
#endif            
            continue;
        }
        else
            break;
    }
    while(True);
}

void SetEpsonRegInt(INT16U addr, INT32U data)
{
    do
    {
        rFPGA_EPSON_RegAddr	= addr;
        __asm("NOP");
        __asm("NOP");
        rFPGA_EPSON_RegPort = data;
        __asm("NOP");
        __asm("NOP");
        if(rFPGA_EPSON_RegPort != data)
        {
            __asm("NOP");
            __asm("NOP");
#ifdef DEBUG_FPGA_BUS
            status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG2, STATUS_SET);
#endif            
            continue;
        }
        else
            break;
    }
    while(True);
}
INT16U ReadEpsonRegShort(INT16U addr)
{
    rFPGA_EPSON_RegAddr	= addr;
    __asm("NOP");
    __asm("NOP");
    return rFPGA_EPSON_RegPortL;
}
INT32U ReadEpsonRegInt(INT16U addr)
{
#ifdef VM_PRINT
    if(EPSON_REGADDR_X_MOTOR_COOR == addr)
        return CURCOOR;
#endif
    rFPGA_EPSON_RegAddr	= addr;
    __asm("NOP");
    __asm("NOP");
    return rFPGA_EPSON_RegPort;
}
INT16U ReadSafeEpsonRegShort(INT16U addr)
{
    INT16U data;
    do{
        rFPGA_EPSON_RegAddr	= addr;
        __asm("NOP");
        __asm("NOP");
        data = rFPGA_EPSON_RegPortL;
        __asm("NOP");
        __asm("NOP");
        if(data != rFPGA_EPSON_RegPortL)
        {
            __asm("NOP");
            __asm("NOP");
#ifdef DEBUG_FPGA_BUS
            status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG2, STATUS_SET);
#endif            
            continue;
        }
        else
            break;
    }while(True);
    return data;
}
INT32U ReadSafeEpsonRegInt(INT16U addr)
{
    INT32U data;
    do{
        __asm("NOP");
        __asm("NOP");
        rFPGA_EPSON_RegAddr	= addr;
        __asm("NOP");
        __asm("NOP");
        data = rFPGA_EPSON_RegPort;
        __asm("NOP");
        __asm("NOP");
        if(data != rFPGA_EPSON_RegPort)
        {
            __asm("NOP");
            __asm("NOP");
#ifdef DEBUG_FPGA_BUS
            status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG2, STATUS_SET);
#endif            
            continue;
        }
        else
            break;
    }while(True);
    return data;
}

#define FPGA_IO_COUNT   12
INT8U bInited_FPGAExpandIoIT = False;
struct sFpgaIoITInfo
{
    FPGAExpandIo_IntHandler Handler;
    void *Data;
    INT8U Enable;
}FpgaIoITInfos[FPGA_IO_COUNT] = {0};

void FPGAIO_Irq(const Pin * pin) FAST_FUNC_SECTION
{
    INT16U io_stat;
    INT16U io_level;
    INT16U mask;
    struct sFpgaIoITInfo *pInfo, *pEnd;
    //OS_CPU_SR cpu_sr;	
    
    if (PIO_Get(pin)==1)
    {
        //OS_ENTER_CRITICAL();
        io_stat = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_STAT);
        ReadEpsonRegShort(EPSON_REGADDR_SENSOR_CTRL);
        io_level = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_IO);;
        
        mask = 1;
        pInfo = FpgaIoITInfos;
        pEnd = FpgaIoITInfos + FPGA_IO_COUNT;
        while(pInfo<pEnd)
        {
            if((io_stat & mask) && pInfo->Enable && pInfo->Handler != NULL)
            {
                if(io_level & mask)
                    pInfo->Handler(pInfo->Data, True);
                else
                    pInfo->Handler(pInfo->Data, False);
            }
            mask <<= 1;
            pInfo ++;
        }
        //OS_EXIT_CRITICAL();
    }
}

static Pin FpgaIoIrqPin;
INT8U InitFPGAExpandIoIT()
{
    OS_CPU_SR cpu_sr;	
    INT8U ret = True;
    Pin FpgaIoIrqPin_ = PIN_FPGA_EXPAND_IO_IRQ;
    
    memcpy(&FpgaIoIrqPin, &FpgaIoIrqPin_, sizeof(FpgaIoIrqPin_));
    
    OS_ENTER_CRITICAL();
    if(!bInited_FPGAExpandIoIT)
    {
        memset(FpgaIoITInfos, 0, sizeof(FpgaIoITInfos));
        
        PIO_ConfigureIt(&FpgaIoIrqPin, FPGAIO_Irq);
        PIO_EnableIt(&FpgaIoIrqPin);
        
        bInited_FPGAExpandIoIT = True;
    }
    else
        ret = False;
    OS_EXIT_CRITICAL();
    return ret;
}

INT8U ConfigFPGAExpandIoIT(FPGAExpandIo_IntHandler handler, INT8U bit, void * data)
{
    OS_CPU_SR cpu_sr;	
    
    if(bit >= FPGA_IO_COUNT)
        return False;
    
    OS_ENTER_CRITICAL();
    if(!bInited_FPGAExpandIoIT)
        InitFPGAExpandIoIT();
    
    FpgaIoITInfos[bit].Handler = handler;
    FpgaIoITInfos[bit].Data = data;
    FpgaIoITInfos[bit].Enable = True;
    
    ReadSafeEpsonRegShort(EPSON_REGADDR_SENSOR_STAT);
    SetEpsonRegShort(EPSON_REGADDR_SENSOR_CTRL, 
                     (1<<bit) | ReadSafeEpsonRegShort(EPSON_REGADDR_SENSOR_CTRL));
    
    OS_EXIT_CRITICAL();
    return True;
}
INT8U UnConfigFPGAExpandIoIT(INT8U bit)
{
    OS_CPU_SR cpu_sr;	
    INT8U i, empty = True;
    
    if(bit >= FPGA_IO_COUNT)
        return False;
    
    OS_ENTER_CRITICAL();
    SetEpsonRegShort(EPSON_REGADDR_SENSOR_CTRL, 
                     ~(1<<bit) & ReadSafeEpsonRegShort(EPSON_REGADDR_SENSOR_CTRL));
    
    FpgaIoITInfos[bit].Handler = NULL;
    FpgaIoITInfos[bit].Data = NULL;
    FpgaIoITInfos[bit].Enable = False;
    
    for(i=0; i<FPGA_IO_COUNT; i++)
    {
        if(FpgaIoITInfos[bit].Handler != NULL)
        {
            empty = False;
            break;
        }
    }
    if(empty)
    {
        PIO_DisableIt(&FpgaIoIrqPin);
        PIO_UnConfigureIt(&FpgaIoIrqPin);
        bInited_FPGAExpandIoIT = False;
    }
    
    OS_EXIT_CRITICAL();
    return True;
}

INT8U DisableFPGAExpandIoIT(INT8U bit)
{
    OS_CPU_SR cpu_sr;	
    
    if(bit >= FPGA_IO_COUNT)
        return False;
    
    OS_ENTER_CRITICAL();
    FpgaIoITInfos[bit].Enable = False;
    
    SetEpsonRegShort(EPSON_REGADDR_SENSOR_CTRL, 
                     ~(1<<bit) & ReadSafeEpsonRegShort(EPSON_REGADDR_SENSOR_CTRL));
    OS_EXIT_CRITICAL();
    return True;
}
INT8U EnableFPGAExpandIoIT(INT8U bit)
{
    OS_CPU_SR cpu_sr;	
    
    if(bit >= FPGA_IO_COUNT)
        return False;
    
    OS_ENTER_CRITICAL();
    FpgaIoITInfos[bit].Enable = True;
    
    ReadSafeEpsonRegShort(EPSON_REGADDR_SENSOR_STAT);
    SetEpsonRegShort(EPSON_REGADDR_SENSOR_CTRL, 
                     (1<<bit) | ReadSafeEpsonRegShort(EPSON_REGADDR_SENSOR_CTRL));
    OS_EXIT_CRITICAL();
    return True;
}

INT8U GetFPGAExpandIoStat(INT8U bit)
{
    OS_CPU_SR cpu_sr;
    INT16U io_level;
    
    if( bit >= FPGA_IO_COUNT)
        return False;
    
    OS_ENTER_CRITICAL();
    io_level = ReadEpsonRegShort(EPSON_REGADDR_SENSOR_IO);
    OS_EXIT_CRITICAL();
    
    if(io_level & (1<<bit))
        return True;
    else
        return True;
}
#endif

INT8U PushCurConfig()
{
    INT8U ret;
    OS_CPU_SR cpu_sr;	
    
    OS_ENTER_CRITICAL();
    ret = FpgaPosiITInfos_pending[0].Enable;
    FpgaPosiITInfos_pending[0] = FpgaPosiITInfos[0];
    OS_EXIT_CRITICAL();
    return !ret;
}
#if 0
INT8U PopCurConfig()
{
    OS_CPU_SR cpu_sr;	
    
    OS_ENTER_CRITICAL();
    if(FpgaPosiITInfos_cache[0].Enable)
    {
        SetTargetPosi(0, FpgaPosiITInfos_cache[0].Position, FpgaPosiITInfos_cache[0].Option);
        ConfigFPGAPosiIT(0, FpgaPosiITInfos_cache[0].Handler, FpgaPosiITInfos_cache[0].Data);
        EnableFPGAPosiIT(0);
        FpgaPosiITInfos_cache[0].Enable = False;
    }
    OS_EXIT_CRITICAL();
    return True;
}
#endif
INT8U GetPendingConfig(INT8U PosiID, INT32U *Position, INT8U *Option)
{
    *Position = FpgaPosiITInfos_pending[PosiID].Position;
    *Option = FpgaPosiITInfos_pending[PosiID].Option;
    return FpgaPosiITInfos[PosiID].bPending;
}

INT8U RegPendingConfig(INT8U PosiID, INT32U Position, INT8U Option,
                       FPGAPosi_IntHandler Handler, void * Data)
{
    OS_CPU_SR cpu_sr;	
    
    if(FpgaPosiITInfos_pending[PosiID].bPending != 0)
        return False;
    
    OS_ENTER_CRITICAL();
    FpgaPosiITInfos_pending[PosiID].Handler = Handler;
    FpgaPosiITInfos_pending[PosiID].Data = Data;
    FpgaPosiITInfos_pending[PosiID].Enable = True;
    
    FpgaPosiITInfos_pending[PosiID].Position = Position;
    FpgaPosiITInfos_pending[PosiID].Option = Option;
    
    FpgaPosiITInfos_pending[PosiID].bPending = True;
    FpgaPosiITInfos[PosiID].bPending = True;
    OS_EXIT_CRITICAL();
    
    return True;
}

INT8U PopPendingConfig()
{
    OS_CPU_SR cpu_sr;	
    
    OS_ENTER_CRITICAL();
    if(FpgaPosiITInfos_pending[0].bPending)
    {
        SetTargetPosi(0, FpgaPosiITInfos_pending[0].Position, FpgaPosiITInfos_pending[0].Option);
        ConfigFPGAPosiIT(0, FpgaPosiITInfos_pending[0].Handler, FpgaPosiITInfos_pending[0].Data);
        EnableFPGAPosiIT(0);
        FpgaPosiITInfos_pending[0].bPending = False;
        FpgaPosiITInfos[0].bPending = False;
    }
    OS_EXIT_CRITICAL();
    return True;
}

INT8U DelayPendingConfig()
{
    FpgaPosiITInfos[0].bPending = False;
    return True;
}

INT8U TrigPendingConfig()
{
    FpgaPosiITInfos[0].bPending = True;
    return True;
}
void FPGAPosi_Irq(const Pin * pin) FAST_FUNC_SECTION
{
    INT16U io_stat;
    INT16U mask;
    struct sFpgaPosiITInfo *pInfo, *pEnd;
    //OS_CPU_SR cpu_sr;	
    
    if (PIO_Get(pin)==1)
    {
        //OS_ENTER_CRITICAL();
#ifdef HEAD_EPSON_GEN5        
        io_stat = ReadEpsonRegShort(EPSON_REGADDR_POSI_STAT);
#elif defined(HEAD_RICOH_G4)
        io_stat = rFPGA_RICOH_XPOSI0_CS_H >> 12;
#endif
        
        mask = 1;
        pInfo = FpgaPosiITInfos;
        pEnd = FpgaPosiITInfos + FPGA_POSI_COUNT;
        while(pInfo<pEnd)
        {
            if((io_stat & mask) && pInfo->Enable && pInfo->Handler != NULL)
            {
                pInfo->Handler(pInfo->Data);
            }
            //PopPendingConfig();
            mask <<= 1;
            pInfo ++;
        }
        //OS_EXIT_CRITICAL();
    }
}

static Pin FpgaPosiIrqPin;
INT8U InitFPGAPosiIT()
{
    OS_CPU_SR cpu_sr;	
    INT8U ret = True;
    Pin FpgaPosiIrqPin_ = PIN_FPGA_POSITION_IRQ;
    
    memcpy(&FpgaPosiIrqPin, &FpgaPosiIrqPin_ ,sizeof(FpgaPosiIrqPin_));
    
    OS_ENTER_CRITICAL();
    if(!bInited_FPGAPosiIT)
    {
        memset(FpgaPosiITInfos, 0, sizeof(FpgaPosiITInfos));
        
        PIO_ConfigureIt(&FpgaPosiIrqPin, FPGAPosi_Irq);
        PIO_EnableIt(&FpgaPosiIrqPin);
        
        bInited_FPGAPosiIT = True;
    }
    else
        ret = False;
    OS_EXIT_CRITICAL();
    return ret;
}

INT8U SetTargetPosi(INT8U PosiID, INT32U Position, INT8U Option)
{
    OS_CPU_SR cpu_sr;	
    INT8U ret = True;
    INT16U mask, bit;
    
    if(PosiID >= FPGA_POSI_COUNT)
        return False;
    
    bit = ((INT16U)Option)<<(PosiID*4);
    mask = (0xF)<<(PosiID*4);
    
    OS_ENTER_CRITICAL();
    if(!bInited_FPGAPosiIT)
        InitFPGAPosiIT();
    
    FpgaPosiITInfos[PosiID].Position = Position;
    FpgaPosiITInfos[PosiID].Option = Option;
#ifdef   HEAD_EPSON_GEN5						
    if(PosiID == 0)
        SetEpsonRegInt(EPSON_REGADDR_X_POSI_0, Position);
    else
        SetEpsonRegInt(EPSON_REGADDR_X_POSI_1, Position);
    SetEpsonRegShort(EPSON_REGADDR_POSI_CTRL, 
                     bit | (~mask & ReadSafeEpsonRegShort(EPSON_REGADDR_POSI_CTRL)));
#elif defined(HEAD_RICOH_G4)
    if( PosiID == 0)
    {
        Position = (Position & 0xFFFFFF)| (((INT32U)(Option&0xF))<<24);
        rFPGA_RICOH_XPOSI0_L = (INT16U)Position;
        rFPGA_RICOH_XPOSI0_CS_H = (INT16U)(Position>>16);
    }
    else
        ret = False;
#endif
    OS_EXIT_CRITICAL();
    return ret;
}

INT8U ConfigFPGAPosiIT(INT8U PosiID, FPGAPosi_IntHandler Handler, void * Data)
{
    OS_CPU_SR cpu_sr;	
    
    if(PosiID >= FPGA_POSI_COUNT)
        return False;
    
    OS_ENTER_CRITICAL();
    if(!bInited_FPGAPosiIT)
        InitFPGAPosiIT();
    
    FpgaPosiITInfos[PosiID].Handler = Handler;
    FpgaPosiITInfos[PosiID].Data = Data;
    FpgaPosiITInfos[PosiID].Enable = True;
    
#ifdef HEAD_EPSON_GEN5        
    ReadSafeEpsonRegShort(EPSON_REGADDR_POSI_STAT);
#elif defined(HEAD_RICOH_G4)
    rFPGA_RICOH_XPOSI0_CS_H;
#endif
    OS_EXIT_CRITICAL();
    return True;
}

INT8U DisableFPGAPosiIT(INT8U PosiID)
{
    OS_CPU_SR cpu_sr;	
    
    if(PosiID >= FPGA_POSI_COUNT)
        return False;
    
    OS_ENTER_CRITICAL();
    FpgaPosiITInfos[PosiID].Enable = False;
#ifdef HEAD_EPSON_GEN5        
    ReadSafeEpsonRegShort(EPSON_REGADDR_POSI_STAT);
    
    SetEpsonRegShort(EPSON_REGADDR_POSI_CTRL, 
                     (~(INT16U)(1<<(PosiID*4))) & ReadSafeEpsonRegShort(EPSON_REGADDR_POSI_CTRL));
#elif defined(HEAD_RICOH_G4)
    {
        INT16U h;
        h = rFPGA_RICOH_XPOSI0_CS_H;
        h &= ~(1<<8);
        rFPGA_RICOH_XPOSI0_CS_H = h;
    }
#endif
    OS_EXIT_CRITICAL();
    return True;
}

INT8U EnableFPGAPosiIT(INT8U PosiID)
{
    OS_CPU_SR cpu_sr;	
    
    if(PosiID >= FPGA_POSI_COUNT)
        return False;
    
    OS_ENTER_CRITICAL();
    FpgaPosiITInfos[PosiID].Enable = True;
    
#ifdef HEAD_EPSON_GEN5        
    SetEpsonRegShort(EPSON_REGADDR_POSI_CTRL, 
                     (INT16U)(1<<(PosiID*4)) | ReadSafeEpsonRegShort(EPSON_REGADDR_POSI_CTRL));
#elif defined(HEAD_RICOH_G4)
    {
        INT16U h;
        h = rFPGA_RICOH_XPOSI0_CS_H;
        h |= (1<<8);
        rFPGA_RICOH_XPOSI0_CS_H = h;
    }
#endif
    OS_EXIT_CRITICAL();
    
    return True;
}

INT8U pos_index =0;
INT32U changePos[10] = {0};
#define AUTO_FLASH_FREQ     200 //us
#define FLASH_SAFE_DISTANCE 30
void FPGAPosi_EndFlash(void * data) FAST_FUNC_SECTION;
void FPGAPosi_BeginFPGABand(void * data) FAST_FUNC_SECTION;

void FPGAPosi_BeginFlash(void * data) FAST_FUNC_SECTION
{
#ifdef   HEAD_EPSON_GEN5 
    changePos[pos_index] = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);
#endif
    pos_index ++;
    if(pos_index == sizeof(changePos)/sizeof(int))
    {
        pos_index = 0;
    }
#ifdef   HEAD_EPSON_GEN5 
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)
       INT32U SprayInterval = cleanparam_EPSON_MICOLOR.SprayInterval; //unit: us.
#elif defined(EPSON_CLEAN_UPDOWN)
       INT32U SprayInterval = cleanparam_EPSON_ALLWIN.Flash_Prt_Freq; //unit: us.
#endif
    if(GetCleanPartCount() > 1)
    {
        if(printInfo.curSprayNo == 0)
        {
            if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
            {
                FPGA_START_FLASH(SprayInterval, 1);
                SetTargetPosi(0, printInfo.PrintFlash_End, 5);
            }
            else if(printInfo.PrintFlash_Start == printInfo.PrintFlash_Start_1)
            {
                FPGA_START_FLASH(SprayInterval, 3);
                SetTargetPosi(0, printInfo.PrintFlash_End, 5);
            }
            else
            {
                FPGA_START_FLASH(SprayInterval, 2);
                SetTargetPosi(0, printInfo.PrintFlash_End_1, 5);
            }
        }
        else if(printInfo.curSprayNo == 1)
        {
            FPGA_START_FLASH(SprayInterval, 0);
            if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
                SetTargetPosi(0, printInfo.PrintFlash_End + FLASH_SAFE_DISTANCE * 2, 5);
            else
                SetTargetPosi(0, printInfo.PrintFlash_End_1 + FLASH_SAFE_DISTANCE * 2, 5);
        }
        else if(printInfo.curSprayNo == 2)
        {
            if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
            {
                FPGA_START_FLASH(SprayInterval, 2);
                SetTargetPosi(0, printInfo.PrintFlash_End_1, 5);
            }
            else
            {
                FPGA_START_FLASH(SprayInterval, 1);
                SetTargetPosi(0, printInfo.PrintFlash_End, 5);
            }
        }
        else if(printInfo.curSprayNo == 3)
        {
            FPGA_START_FLASH(SprayInterval, 0);
            if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
                SetTargetPosi(0, printInfo.PrintFlash_End_1 + FLASH_SAFE_DISTANCE * 2, 5);
            else
                SetTargetPosi(0, printInfo.PrintFlash_End + FLASH_SAFE_DISTANCE * 2, 5);
        }
    }
    else
    {
        FPGA_START_FLASH(SprayInterval, 1);
        SetTargetPosi(0, printInfo.PrintFlash_End, 5);
    }
#elif defined(HEAD_RICOH_G4)
    //un-implement
#endif
    ConfigFPGAPosiIT(0, FPGAPosi_EndFlash, NULL);
}

void FPGAPosi_EndFlash(void * data) FAST_FUNC_SECTION
{
    INT8U nextFlash = False;
    
#ifdef   HEAD_EPSON_GEN5 
    changePos[pos_index] = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);
#endif
    pos_index ++;
    if(pos_index == sizeof(changePos)/sizeof(int))
    {
        pos_index = 0;
    }
    
    FPGA_STOP_FLASH();
#if defined(HEAD_EPSON_GEN5)
    if(GetCleanPartCount() > 1)
    {
        if(printInfo.curSprayNo == 1)
        {
            if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
            {
                if(printInfo.PrintFlash_End < printInfo.PrintFlash_Start_1 - FLASH_SAFE_DISTANCE)
                {
                    nextFlash = True;
                }
            }
            else
            {
                if(printInfo.PrintFlash_End_1 < printInfo.PrintFlash_Start - FLASH_SAFE_DISTANCE)
                {
                    nextFlash = True;
                }
            }
        }
        else if(printInfo.curSprayNo != 3)
            nextFlash = True;
    }
    
    if(nextFlash)
    {
        if(printInfo.curSprayNo == 0)
        {
            if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
                SetTargetPosi(0, printInfo.PrintFlash_End + FLASH_SAFE_DISTANCE, 5);
            else
                SetTargetPosi(0, printInfo.PrintFlash_End_1 + FLASH_SAFE_DISTANCE, 5);
        }
        else if(printInfo.curSprayNo == 1)
        {
            if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
                SetTargetPosi(0, printInfo.PrintFlash_Start_1, 5);
            else
                SetTargetPosi(0, printInfo.PrintFlash_Start, 5);
        }
        else //if(printInfo.curSprayNo == 2)
        {
            if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
                SetTargetPosi(0, printInfo.PrintFlash_End_1 + FLASH_SAFE_DISTANCE, 5);
            else
                SetTargetPosi(0, printInfo.PrintFlash_End + FLASH_SAFE_DISTANCE, 5);
        }
        ConfigFPGAPosiIT(0, FPGAPosi_BeginFlash, NULL);
        printInfo.curSprayNo ++;
    }
    else
    {
        
        if(GetCleanPartCount() > 1 && printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
            SetTargetPosi(0, printInfo.PrintFlash_End_1 + FLASH_SAFE_DISTANCE*3, 5);
        else
            SetTargetPosi(0, printInfo.PrintFlash_End + FLASH_SAFE_DISTANCE*3, 5);
        ConfigFPGAPosiIT(0, FPGAPosi_BeginFPGABand, NULL);
        
        FPGA_SendData(True, printInfo.PreFireNum); //Begin send data to FPGA FIFO
    }
#elif defined(HEAD_RICOH_G4)
    //un-implement
#endif		
}
#ifdef   HEAD_EPSON_GEN5 
void FPGAPosi_BeginFPGABand(void * data) FAST_FUNC_SECTION
{
    INT32U curPos = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);
    changePos[pos_index] = curPos; 
    pos_index ++;
    if(pos_index == sizeof(changePos)/sizeof(int))
    {
        pos_index = 0;
    }
    
    if(g_FPGADMA_sendPkg > 0)
    {
        SetFPGAReg(printInfo.PreFireNum, printInfo.PostFireNum);
        PushVSDCMD(printInfo.b_first_pass);                    
        FPGA_BEGIN_BAND();   //FPGA transfer first data to print head
    }
    else
    {
        SetTargetPosi(0, curPos + FLASH_SAFE_DISTANCE, 5);
    }
}
#endif  

INT8U IsDelaySendData()
{
    if(printInfo.autoSpray)
        return True;
    else
        return False;
}

void DelaySendData(INT32U PreFireNum, INT32U PostFireNum, INT8U b_first_pass)
{
    printInfo.PreFireNum = PreFireNum;
    printInfo.PostFireNum = PostFireNum;
    printInfo.b_first_pass = b_first_pass;
}

INT8U StartPrePrint(INT32S startFirePoint, INT32S endFirePoint, INT32S next_startFirePoint, INT32S next_endFirePoint)
{
    OS_CPU_SR cpu_sr;
    INT8U bEmptyPass[2] = {False,False},err;
    INT32S startPoint, next_startPoint;
    INT32S endPoint, next_endPoint;
    INT8U speedID;
    INT32S xAccDistance;
    INT32S startPoint_720;
    INT8U cmdbuf[32];
    static INT8U next_flash = False;
    
    if(startFirePoint == endFirePoint)
    {
        startPoint = startFirePoint;
        endPoint = endFirePoint;
        bEmptyPass[0] = True;
    }
    else
    {
        if(startFirePoint < endFirePoint)
        {
            startPoint = startFirePoint;
            endPoint = endFirePoint;
        }
        else
        {
            startPoint = endFirePoint;
            endPoint = startFirePoint;
        }
    }
    printInfo.PrintFire_Start = startPoint;
    printInfo.PrintFire_End = endPoint;
    
    if(next_startFirePoint == next_endFirePoint)
    {
        bEmptyPass[1] = True;
        next_startPoint = next_startFirePoint;
        next_endPoint = next_endFirePoint;
    }
    else
    {
        if(next_startFirePoint < next_endFirePoint)
        {
            next_startPoint = next_startFirePoint;
            next_endPoint = next_endFirePoint;
        }
        else
        {
            next_startPoint = next_endFirePoint;
            next_endPoint = next_startFirePoint;
        }
    }
    
    if(bEmptyPass[1] == False)
    {
        if(bEmptyPass[0] == True)
        {
            startPoint = next_startFirePoint;
            endPoint = next_startFirePoint;
        }
        else
        {
            if(startFirePoint < endFirePoint)
            {
                if(next_startFirePoint > next_endFirePoint)
                {
                    if(endFirePoint < next_startFirePoint)
                        endPoint = next_startFirePoint;
                }
            }
            else
            {
                if(next_startFirePoint < next_endFirePoint)
                {
                    if(endFirePoint > next_startFirePoint)
                        startPoint = next_startFirePoint;
                }
            }
        }
    }
    
    xAccDistance = printer.accSpace;
    
    startPoint -= xAccDistance;
    endPoint += xAccDistance;
    
    if(bEmptyPass[0] == True)
    {
        if(next_startFirePoint <= next_endFirePoint)
        {
            printInfo.PrintMove_Start = startPoint;
            printInfo.PrintMove_End = endPoint;
        }
        else
        {
            printInfo.PrintMove_Start = endPoint;
            printInfo.PrintMove_End = startPoint;
        }
    }
    else
    {
        if(startFirePoint <= endFirePoint)
        {
            printInfo.PrintMove_Start = startPoint;
            printInfo.PrintMove_End = endPoint;
        }
        else
        {
            printInfo.PrintMove_Start = endPoint;
            printInfo.PrintMove_End = startPoint;
        }
    }
    

    printInfo.lastEndCoor = curPos.x;

    
    printInfo.autoSpray = False;
    printInfo.PrintFlash_Start = 0;
    printInfo.PrintFlash_End = 0;
    printInfo.SprayPosType = 0;
    next_flash = False;
#if defined (EPSON_CLEAN_INTEGRATE) || defined (EPSON_CLEAN_INTEGRATE_1) || defined (EPSON_CLEAN_INTEGRATE_2)
    if( printInfo.PrintFire_Start != printInfo.PrintFire_End && cleanparam_EPSON_MICOLOR.bAutoSpray)
#elif defined(EPSON_CLEAN_UPDOWN)  
    if( printInfo.PrintFire_Start != printInfo.PrintFire_End && cleanparam_EPSON_ALLWIN.Flash_Prt_On )
#endif       
    {
        INT32U start, end, start1, end1;
#if defined (EPSON_CLEAN_INTEGRATE) || defined (EPSON_CLEAN_INTEGRATE_1) || defined (EPSON_CLEAN_INTEGRATE_2)		
        start = (cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_Start + 
                 cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_End - cleanparam_EPSON_MICOLOR.SprayWidth)/2;
        end = start + cleanparam_EPSON_MICOLOR.SprayWidth;
                
        start1 = (cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_1_Start + 
                  cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_1_End - cleanparam_EPSON_MICOLOR.SprayWidth)/2;
        end1 = start1 + cleanparam_EPSON_MICOLOR.SprayWidth;
#elif defined(EPSON_CLEAN_UPDOWN)  
        start = cleanparam_EPSON_ALLWIN.Flash_Prt_POsi - cleanparam_EPSON_ALLWIN.Flash_Width/2;
        end = cleanparam_EPSON_ALLWIN.Flash_Prt_POsi + cleanparam_EPSON_ALLWIN.Flash_Width/2;
                
        start1 = cleanparam_EPSON_ALLWIN.Flash_Prt_POsi_1 - cleanparam_EPSON_ALLWIN.Flash_Width/2;
        end1 = cleanparam_EPSON_ALLWIN.Flash_Prt_POsi_1 + cleanparam_EPSON_ALLWIN.Flash_Width/2;
#endif        		
        printInfo.PrintFlash_Start  = start + XORIGIN_OFFSET;
        printInfo.PrintFlash_End = end + XORIGIN_OFFSET;
        printInfo.PrintFlash_Start_1  = start1 + XORIGIN_OFFSET;
        printInfo.PrintFlash_End_1 = end1 + XORIGIN_OFFSET;
        printInfo.curSprayNo = 0;
        
        printInfo.autoSpray = True;
        if( printInfo.PrintMove_End > printInfo.PrintMove_Start)
        {
            if(printInfo.PrintFlash_End < printInfo.PrintFire_Start + XORIGIN_OFFSET)
                printInfo.SprayPosType = 0;
            else
                printInfo.SprayPosType = 1;
        }
        else
        {
            if(printInfo.PrintFlash_End < printInfo.PrintFire_Start + XORIGIN_OFFSET)
                printInfo.SprayPosType = 2;
            else
                printInfo.SprayPosType = 3;
        }

#if defined (EPSON_CLEAN_INTEGRATE) || defined (EPSON_CLEAN_INTEGRATE_1) || defined (EPSON_CLEAN_INTEGRATE_2)		
		INT8U Flash_PASS = cleanparam_EPSON_MICOLOR.passNum;
#elif defined(EPSON_CLEAN_UPDOWN)  
		INT8U Flash_PASS = cleanparam_EPSON_ALLWIN.Flash_Prt_Pass;
#endif
        if(!(printInfo.SprayPosType == 0 || printInfo.SprayPosType == 3))
        {//currently, only support pre-flash. because FPGA code can not handle shake between pass.
            printInfo.autoSpray = False;
        }
        else if(!(Flash_PASS != 0 && printedBands != 0 && (printedBands - 1) % Flash_PASS == 0))
        {
            printInfo.autoSpray = False;
        }
        if(Flash_PASS != 0 && printedBands != 0 &&( (printedBands - 1) % Flash_PASS ==0))
        {
            next_flash = True;
        }				
        if(printInfo.autoSpray == True)
        {
            if( printInfo.PrintMove_End > printInfo.PrintMove_Start)
            {
                if(printInfo.PrintMove_Start > start - MIN_FLASH_PRE_DISTANCE)
                    printInfo.PrintMove_Start = start - MIN_FLASH_PRE_DISTANCE;
                if(printInfo.PrintMove_Start > start1 - MIN_FLASH_PRE_DISTANCE)
                    printInfo.PrintMove_Start = start1 - MIN_FLASH_PRE_DISTANCE ;
            }
            else
            {
                if(printInfo.PrintMove_End > start - MIN_FLASH_PRE_DISTANCE)
                    printInfo.PrintMove_End = start - MIN_FLASH_PRE_DISTANCE;
                if(printInfo.PrintMove_End > start1 - MIN_FLASH_PRE_DISTANCE)
                    printInfo.PrintMove_End = start1 - MIN_FLASH_PRE_DISTANCE;
            }
        }
        else if(next_flash == True)
        {
            if( printInfo.PrintMove_End < printInfo.PrintMove_Start)
            {
                if(printInfo.PrintMove_End > start - MIN_FLASH_PRE_DISTANCE)
                    printInfo.PrintMove_End = start - MIN_FLASH_PRE_DISTANCE;
                if(printInfo.PrintMove_End > start1 - MIN_FLASH_PRE_DISTANCE)
                    printInfo.PrintMove_End = start1 - MIN_FLASH_PRE_DISTANCE;
            }
        }
    }
#if 0
    startPoint_720 = printInfo.PrintMove_Start;
    if( (printInfo.PrintMove_End > printInfo.PrintMove_Start && printInfo.lastEndCoor - MIN_PERMIT_X_PRINT_ERROR > startPoint_720) ||
       (printInfo.PrintMove_End <= printInfo.PrintMove_Start && printInfo.lastEndCoor + MIN_PERMIT_X_PRINT_ERROR < startPoint_720))
    	{
		status_ReportStatus(STATUS_MOVING, STATUS_SET);		
		cmdbuf[0] = 7; //Length
		cmdbuf[1] = UART_MOVETO_CMD;
		*((__packed INT32S *)&cmdbuf[2]) = startPoint_720;
		cmdbuf[6] = PRINTER_MOVE_SPEED;
		while (!UART_SendCMD(UART_MOTION_CHANNEL, cmdbuf)) 
		    OSTimeDly(100);			
		OSFlagPend(status_FLAG_GRP, STATUS_MOVING, OS_FLAG_WAIT_CLR_ALL, 0, &err); //Waiting moving stop
    	}
#endif	
if(printInfo.autoSpray)
    {
        if(GetCleanPartCount() > 1)
        {
            printInfo.curSprayNo = 0;
            if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
                SetTargetPosi(0, printInfo.PrintFlash_Start, 5);
            else
                SetTargetPosi(0, printInfo.PrintFlash_Start_1, 5);
        }
        else
            SetTargetPosi(0, printInfo.PrintFlash_Start, 5);
        ConfigFPGAPosiIT(0, FPGAPosi_BeginFlash, NULL);
    }
}
#endif
INT8U BuildDefaultMotionParam(struct EPR_MOTION_PARAM * pMotionParam)
{
	memcpy(pMotionParam, &motionParam_default, sizeof( struct EPR_MOTION_PARAM));
	return True;
}

INT8U SaveMotionParam(struct EPR_MOTION_PARAM * pMotionParam)
{
	INT8U ret = True;
	INT16U len = sizeof(struct EPR_MOTION_PARAM);
	
	EPM_Head head = 
	{
		MOTION_PARAM_FLAG,
		MOTION_PARAM_VERSION,
		sizeof(struct EPR_MOTION_PARAM) + sizeof(EPM_Head),
		0
	};
	
	CalcCheckSum((INT8U*)pMotionParam, &head);
	ret = IIC_WriteEEPRom_Head(EPR_MOTOR_PARAM_OFFSET, &head);
	
	ret &= IIC_WriteEEPRom_Ex(EPR_MOTOR_PARAM_OFFSET + sizeof(EPM_Head), (INT8U *)pMotionParam, &len);
	return ret;
}

INT8U ReadMotionEPRConfig()
{
	INT8U err;
	INT16U length;
	EPM_Head head;
	
	length = sizeof(struct EPR_MOTION_PARAM) + sizeof(EPM_Head);
	if(length > EPR_MOTOR_PARAM_SIZE)
		return False;
	
	if(!IIC_ReadEEPRom_Head(EPR_MOTOR_PARAM_OFFSET, &head))
	{
		return False;
	}
	
	memset(&motionParam, 0, sizeof(struct EPR_MOTION_PARAM));
	if( head.flag == 1 && head.version < EPR_MOTOR_PARAM_SIZE) //it is old structure before add the head.
	{
		length = head.version; //really, it is element len of old structure.
		if(length > sizeof(struct EPR_MOTION_PARAM))
			length = sizeof(struct EPR_MOTION_PARAM);
		if(!IIC_ReadEEPRom_Ex(EPR_MOTOR_PARAM_OFFSET, (INT8U * )&motionParam, &length))
		{
			return False;
		}
		//add new elememt initialization here.
		
		motionParam.len = sizeof(struct EPR_MOTION_PARAM);  
		motionParam.version = MOTION_PARAM_VERSION;  
		
		return SaveMotionParam(&motionParam);
	}
	else if(head.flag == MOTION_PARAM_FLAG)
	{
		if(!IIC_ReadEEPRom_Struct(EPR_MOTOR_PARAM_OFFSET + sizeof(EPM_Head), 
								  (INT8U * )&motionParam, sizeof(struct EPR_MOTION_PARAM), &head))
		{
			return False;
		}
		
		if( motionParam.len != sizeof(struct EPR_MOTION_PARAM) || 
		   motionParam.version != MOTION_PARAM_VERSION)
		{
			motionParam.len = sizeof(struct EPR_MOTION_PARAM);  
			motionParam.version = MOTION_PARAM_VERSION; 
			
			return SaveMotionParam(&motionParam);
		}
	}
	else
	{
		BuildDefaultMotionParam(&motionParam);        
		return SaveMotionParam(&motionParam);
	}
	
	return True;
}

INT8U UI_Reset_MotionConfig(void * pData)
{
	if(pData == (void *)-1)
	{
		return True;
	}
	BuildDefaultMotionParam(&motionParam);        
	return SaveMotionParam(&motionParam);
}
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
INT8U ConfigSensorPin(struct SensorInfo * pSensor, struct EPR_MOTION_AXIS_PARAM * pAxis_param, INT8U SensorIndex)
{
	if(pAxis_param->SensorExistMask & (1<<SensorIndex))
	{
		INT8U PortId, IoId;
		
		PortId = pAxis_param->SensorIOId[SensorIndex]/32;
		IoId = pAxis_param->SensorIOId[SensorIndex]%32;
		
		pSensor->IsFPGAExpandIO = False;
		pSensor->PinIO.type = PIO_INPUT;
		pSensor->PinIO.attribute = PIO_DEFAULT;
		pSensor->PinIO.mask = 1<<IoId;
		switch(PortId)
		{
		case 0:
			pSensor->PinIO.pio = AT91C_BASE_PIOA;
			pSensor->PinIO.id = AT91C_ID_PIOA;
			break;
		case 1:
			pSensor->PinIO.pio = AT91C_BASE_PIOB;
			pSensor->PinIO.id = AT91C_ID_PIOB;
			break;
		case 2:
			pSensor->PinIO.pio = AT91C_BASE_PIOC;
			pSensor->PinIO.id = AT91C_ID_PIOC;
			break;
		case 3:
			pSensor->PinIO.pio = AT91C_BASE_PIOD;
			pSensor->PinIO.id = AT91C_ID_PIOD_E;
			break;
		case 4:
			pSensor->PinIO.pio = AT91C_BASE_PIOE;
			pSensor->PinIO.id = AT91C_ID_PIOD_E;
			break;
		case 5:
			pSensor->IsFPGAExpandIO = True;
			pSensor->FPGABit = IoId;
			break;
		default:
			pSensor->IsValid = False;
			return False;
		}
		
		pSensor->IsValid = True;
		if(pAxis_param->SensorPolarityMask & (1<<SensorIndex))
			pSensor->ValidLevel = True;
		else
			pSensor->ValidLevel = False;
		
		return True;
	}
	else
	{
		pSensor->IsValid = False;
		return False;
	}
}

//return logic high
INT8U GetSensor(struct SensorInfo * pSensor)
{
	INT8U ret = False;
	if(pSensor->IsValid)
	{
		if(pSensor->IsFPGAExpandIO)
			ret = False;
		else
			ret = PIO_Get(&pSensor->PinIO);
		if(!pSensor->ValidLevel)
			ret = ! ret;
	}
	return ret;
}

INT8U ConfigureSensorIt(struct SensorInfo * pSensor, 
						FPGAExpandIo_IntHandler fpgahandler, void * data, void (*iohandler)(const Pin *))
{
	INT8U ret = False;
	if(pSensor->IsValid)
	{
		if(pSensor->IsFPGAExpandIO)
		{
			ret = False;
		}
		else
		{
			PIO_ConfigureIt(&pSensor->PinIO, iohandler);
			PIO_EnableIt(&pSensor->PinIO);
			ret = True;
		}
	}
	return ret;
}

INT8U UnConfigureSensorIt(struct SensorInfo * pSensor)
{
	INT8U ret = False;
	if(pSensor->IsValid)
	{
		if(pSensor->IsFPGAExpandIO)
		{
			//UnConfigFPGAExpandIoIT(pSensor->FPGABit);
		}
		else
		{
			OS_CPU_SR cpu_sr;
			
			OS_ENTER_CRITICAL();
			PIO_DisableIt(&pSensor->PinIO);
			PIO_UnConfigureIt(&pSensor->PinIO);
			OS_EXIT_CRITICAL();
			ret = True;
		}
	}
	return ret;
}
#endif

INT8U ConfigCtrlPin(struct ControlIOInfo * pCtrlIO, struct EPR_MOTION_AXIS_PARAM * pAxis_param, INT8U CtrlIOIndex)
{
	INT8U PortId, IoId;
	
	PortId = pAxis_param->ControlIOId[CtrlIOIndex]/32;
	IoId = pAxis_param->ControlIOId[CtrlIOIndex]%32;
	
	pCtrlIO->PinIO.type = PIO_OUTPUT_1;
	pCtrlIO->PinIO.attribute = PIO_DEFAULT;
	pCtrlIO->PinIO.mask = 1<<IoId;
	switch(PortId)
	{
	case 0:
		pCtrlIO->PinIO.pio = AT91C_BASE_PIOA;
		pCtrlIO->PinIO.id = AT91C_ID_PIOA;
		break;
	case 1:
		pCtrlIO->PinIO.pio = AT91C_BASE_PIOB;
		pCtrlIO->PinIO.id = AT91C_ID_PIOB;
		break;
	case 2:
		pCtrlIO->PinIO.pio = AT91C_BASE_PIOC;
		pCtrlIO->PinIO.id = AT91C_ID_PIOC;
		break;
	case 3:
		pCtrlIO->PinIO.pio = AT91C_BASE_PIOD;
		pCtrlIO->PinIO.id = AT91C_ID_PIOD_E;
		break;
	case 4:
		pCtrlIO->PinIO.pio = AT91C_BASE_PIOE;
		pCtrlIO->PinIO.id = AT91C_ID_PIOD_E;
		break;
	default:
		return False;
	}
	
	if(pAxis_param->ControlIOPolarityMask & (1<<CtrlIOIndex))
	{
		pCtrlIO->ValidLevel = True;
		pCtrlIO->PinIO.type = PIO_OUTPUT_1;
	}
	else
	{
		pCtrlIO->ValidLevel = False;
		pCtrlIO->PinIO.type = PIO_OUTPUT_0;
	}
	
	return True;
}



INT8U BuildPulseCurve(struct ACCCurveInfo * pCurveTable, const struct AxisParam_PulseDir_OpenLoop * pPulseDir, 
					  const struct EPR_MOTION_AXIS_PARAM * pAxisParam, INT16U DivideNum, INT8U SpeedId)
{
	INT32U i, PulseCount, accTime, tick;
	float startFreq, endFreq, K, freq;
	float ratio;
	INT16U *pCycle;
	
	pCurveTable->AccType = ACCTYPE_CYCLETIME;
	
	ratio = pAxisParam->transmisstion_ratio_open_loop;
	
	if( pAxisParam->accParam[SpeedId].type == AT_Trapezoid)
	{
		const struct AccParam *pTrapezoid = (struct AccParam *)&pAxisParam->accParam[SpeedId];
		
		endFreq = (float)(pTrapezoid->EndFreq * ratio);
		startFreq = (float)(pTrapezoid->startFreq * ratio);
		accTime = (INT32U)((float)pTrapezoid->AccTime*BOARD_MCK/1000000/DivideNum+0.5);
		K = (endFreq - startFreq)/accTime;
		
		pCycle = (INT16U*)(pCurveTable->IncTable); 
		PulseCount=0;
		for(i= 0; i<accTime;)
		{
			freq = startFreq + K*i;
			tick = (BOARD_MCK/(DivideNum*freq)+0.5);
			if(tick > 0xFFFF)
				tick = 0xFFFF;
			*pCycle = (INT16U)tick;
			i+=*pCycle;
			pCycle ++;
			if((INT32U)pCycle >= (INT32U)STEP_POOL)
				return False;
			PulseCount ++;
		}
		tick = (BOARD_MCK/(DivideNum*endFreq)+0.5);
		if(tick > 0xFFFF)
			tick = 0xFFFF;
		*pCycle = (INT16U)tick;
		PulseCount ++;
		pCurveTable->AccDistance = i + *pCycle;
		
		pCurveTable->AccCurveLen = PulseCount;
		pCurveTable->AccDisTblOffset = 0;
	}    
	else
		return False;
	
	return True;
}

INT8U BuildAllPulseCurve(struct AxisParam_PulseDir_OpenLoop * pPulseDir, 
						 const struct EPR_MOTION_AXIS_PARAM * pAxis_param) //PWMCycleTick based on MCK(133Mhz).
{
	INT32U  i;
	pPulseDir->pAllCurveTable = NULL;
	
	switch(pAxis_param->AxisID)
	{
	case AXIS_ID_Z:
		pPulseDir->pAllCurveTable = AccCurveIndexTbl.pAllCurveZ;
		if(pAxis_param->validSpeedLevel > MAX_Z_C_SPEED_LEVEL)
			return False;
		break;
	case AXIS_ID_C:
		pPulseDir->pAllCurveTable = AccCurveIndexTbl.pAllCurveC;
		if(pAxis_param->validSpeedLevel > MAX_Z_C_SPEED_LEVEL)
			return False;
		break;
	case AXIS_ID_Z_DRIVER:
		pPulseDir->pAllCurveTable = AccCurveIndexTbl.pAllCurveZ_Driver;
		if(pAxis_param->validSpeedLevel > MAX_Z_C_SPEED_LEVEL)
			return False;
		break;
	case AXIS_ID_C_DRIVER:
		pPulseDir->pAllCurveTable = AccCurveIndexTbl.pAllCurveC_Driver;
		if(pAxis_param->validSpeedLevel > MAX_Z_C_SPEED_LEVEL)
			return False;
		break;
	default:
		return False;
	}
	
	for(i = 0; i < pAxis_param->validSpeedLevel; i++)
	{
		pPulseDir->pAllCurveTable[i] = (struct ACCCurveInfo *)currCurveBuf;
		if( !BuildPulseCurve(pPulseDir->pAllCurveTable[i], pPulseDir, pAxis_param, TC_DIVIDE_NUM, i))
			return False;
		currCurveBuf = (INT32U)(pPulseDir->pAllCurveTable[i]->IncTable) + 
			(pPulseDir->pAllCurveTable[i]->AccCurveLen * sizeof(INT16U) + sizeof(INT32U) -1)/sizeof(INT32U)*sizeof(INT32U) ;
	}
	return True;
}

INT8U ConfigSensor(struct SensorInfo *pSensor)
{
	if(pSensor->IsValid)
	{
		if(!pSensor->IsFPGAExpandIO)
			PIO_Configure(&pSensor->PinIO, PIO_LISTSIZE(pSensor->PinIO));
		return True;
	}
	else 
		return False;
}

INT8U ConfigControlIO(struct ControlIOInfo *pControlIO, INT8U IsValid)
{
	if(IsValid == pControlIO->ValidLevel)
		pControlIO->PinIO.type = PIO_OUTPUT_1;
	else
		pControlIO->PinIO.type = PIO_OUTPUT_0;
	PIO_Configure(&pControlIO->PinIO, PIO_LISTSIZE(pControlIO->PinIO));
	return True;
}

typedef void (*pTCChannelHandle)(struct AxisParam_PulseDir_OpenLoop * pDCPWM);
void TCHandler_dimmy(struct AxisParam_PulseDir_OpenLoop * pDCPWM)
{
}
pTCChannelHandle pTC1Handle, pTC2Handle,pTC3Handle, pTC4Handle; 

INT8U InitTCChannel()
{
	Pin MotionTCPin_1_A = PIN_STEPPER1_PULSE;
	Pin MotionTCPin_1_A_Init = PIN_STEPPER1_PULSE_INIT;
	Pin MotionTCPin_1_B = PIN_STEPPER1_DIR;
	Pin MotionTCPin_2_A = PIN_STEPPER2_PULSE;
	Pin MotionTCPin_2_A_Init = PIN_STEPPER2_PULSE_INIT;
	Pin MotionTCPin_2_B = PIN_STEPPER2_DIR;
	
	Pin MotionTCPin_3_A = PIN_STEPPER3_PULSE;
	Pin MotionTCPin_3_B = PIN_STEPPER3_DIR;
	Pin MotionTCPin_4_A = PIN_STEPPER4_PULSE;
	Pin MotionTCPin_4_B = PIN_STEPPER4_DIR;
	INT8U ret = True;
	
	PMC_EnablePeripheral(AT91C_ID_TC);
	
	AT91C_BASE_TCB0->TCB_BMR &= ~AT91C_TCB_TC0XC0S;
	AT91C_BASE_TCB0->TCB_BMR |= AT91C_TCB_TC0XC0S_NONE;
	AT91C_BASE_TCB0->TCB_BMR &= ~AT91C_TCB_TC1XC1S;
	AT91C_BASE_TCB0->TCB_BMR |= AT91C_TCB_TC1XC1S_NONE;
	
	AT91C_BASE_TCB1->TCB_BMR &= ~AT91C_TCB_TC0XC0S;
	AT91C_BASE_TCB1->TCB_BMR |= AT91C_TCB_TC0XC0S_NONE;
	AT91C_BASE_TCB1->TCB_BMR &= ~AT91C_TCB_TC1XC1S;
	AT91C_BASE_TCB1->TCB_BMR |= AT91C_TCB_TC1XC1S_NONE;
	
	AT91C_BASE_TC0->TC_CCR = AT91C_TC_CLKDIS;
	AT91C_BASE_TC0->TC_IDR = AT91C_TC_COVFS | AT91C_TC_LOVRS | AT91C_TC_CPAS | AT91C_TC_CPBS |
		AT91C_TC_CPCS | AT91C_TC_LDRAS | AT91C_TC_LDRBS | AT91C_TC_ETRGS;
	AT91C_BASE_TC0->TC_SR;
	AT91C_BASE_TC0->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
		AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
			AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	
	AT91C_BASE_TC1->TC_CCR = AT91C_TC_CLKDIS;
	AT91C_BASE_TC1->TC_IDR = AT91C_TC_COVFS | AT91C_TC_LOVRS | AT91C_TC_CPAS | AT91C_TC_CPBS |
		AT91C_TC_CPCS | AT91C_TC_LDRAS | AT91C_TC_LDRBS | AT91C_TC_ETRGS;
	AT91C_BASE_TC1->TC_SR;
	AT91C_BASE_TC1->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
		AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
			AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	
	AT91C_BASE_TC2->TC_CCR = AT91C_TC_CLKDIS;
	AT91C_BASE_TC2->TC_IDR = AT91C_TC_COVFS | AT91C_TC_LOVRS | AT91C_TC_CPAS | AT91C_TC_CPBS |
		AT91C_TC_CPCS | AT91C_TC_LDRAS | AT91C_TC_LDRBS | AT91C_TC_ETRGS;
	AT91C_BASE_TC2->TC_SR;
	AT91C_BASE_TC2->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
		AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
			AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	
	AT91C_BASE_TC3->TC_CCR = AT91C_TC_CLKDIS;
	AT91C_BASE_TC3->TC_IDR = AT91C_TC_COVFS | AT91C_TC_LOVRS | AT91C_TC_CPAS | AT91C_TC_CPBS |
		AT91C_TC_CPCS | AT91C_TC_LDRAS | AT91C_TC_LDRBS | AT91C_TC_ETRGS;
	AT91C_BASE_TC3->TC_SR;
	AT91C_BASE_TC3->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
		AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
			AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	
	pTC1Handle = TCHandler_dimmy;
	pTC2Handle = TCHandler_dimmy; 
	pTC3Handle = TCHandler_dimmy;
	pTC4Handle = TCHandler_dimmy;
	
	if(pTC_Driver1 != NULL)
	{
		if(!pTC_Driver1->ChannelValidLevel[0])
			AT91C_BASE_TC0->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
				AT91C_TC_ACPA_CLEAR | AT91C_TC_ACPC_SET | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
					AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
		PIO_Configure(&MotionTCPin_1_A, PIO_LISTSIZE(MotionTCPin_1_A));
		if(!pTC_Driver1->ChannelValidLevel[1])
			MotionTCPin_1_B.type = PIO_OUTPUT_1;
		PIO_Configure(&MotionTCPin_1_B, PIO_LISTSIZE(MotionTCPin_1_B));
		ConfigControlIO(&pTC_Driver1->ControlIO, True); //keep stepper as reset
		pTC_Driver1->TC_DirPin.PinIO = MotionTCPin_1_B;
		pTC_Driver1->TC_DirPin.ValidLevel = pTC_Driver1->ChannelValidLevel[1];
	}
	else
	{
		PIO_Configure(&MotionTCPin_1_A_Init, PIO_LISTSIZE(MotionTCPin_1_A_Init));
		PIO_Configure(&MotionTCPin_1_B, PIO_LISTSIZE(MotionTCPin_1_B));
	}
	
	if(pTC_Driver2 != NULL)
	{
		if(!pTC_Driver2->ChannelValidLevel[0])
			AT91C_BASE_TC3->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
				AT91C_TC_ACPA_CLEAR | AT91C_TC_ACPC_SET | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
					AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
		PIO_Configure(&MotionTCPin_2_A, PIO_LISTSIZE(MotionTCPin_2_A));
		if(!pTC_Driver2->ChannelValidLevel[1])
			MotionTCPin_2_B.type = PIO_OUTPUT_1;
		PIO_Configure(&MotionTCPin_2_B, PIO_LISTSIZE(MotionTCPin_2_B));
		ConfigControlIO(&pTC_Driver2->ControlIO, True); //keep stepper as reset
		pTC_Driver2->TC_DirPin.PinIO = MotionTCPin_2_B;
		pTC_Driver2->TC_DirPin.ValidLevel = pTC_Driver2->ChannelValidLevel[1];
	}
	else
	{
		PIO_Configure(&MotionTCPin_2_A_Init, PIO_LISTSIZE(MotionTCPin_2_A_Init));
		PIO_Configure(&MotionTCPin_2_B, PIO_LISTSIZE(MotionTCPin_2_B));
	}
	
	if(pTC_Driver3 != NULL)
	{
		if(!pTC_Driver3->ChannelValidLevel[0])
			AT91C_BASE_TC1->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
				AT91C_TC_ACPA_CLEAR | AT91C_TC_ACPC_SET | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
					AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
		PIO_Configure(&MotionTCPin_3_A, PIO_LISTSIZE(MotionTCPin_3_A));
		if(!pTC_Driver3->ChannelValidLevel[1])
			MotionTCPin_3_B.type = PIO_OUTPUT_1;
		PIO_Configure(&MotionTCPin_3_B, PIO_LISTSIZE(MotionTCPin_3_B));
		ConfigControlIO(&pTC_Driver3->ControlIO, True); //keep stepper as reset
		pTC_Driver3->TC_DirPin.PinIO = MotionTCPin_3_B;
		pTC_Driver3->TC_DirPin.ValidLevel = pTC_Driver3->ChannelValidLevel[1];
	}
	
	if(pTC_Driver4 != NULL)
	{
		if(!pTC_Driver4->ChannelValidLevel[0])
			AT91C_BASE_TC2->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
				AT91C_TC_ACPA_CLEAR | AT91C_TC_ACPC_SET | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
					AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
		PIO_Configure(&MotionTCPin_4_A, PIO_LISTSIZE(MotionTCPin_4_A));
		if(!pTC_Driver4->ChannelValidLevel[1])
			MotionTCPin_4_B.type = PIO_OUTPUT_1;
		PIO_Configure(&MotionTCPin_4_B, PIO_LISTSIZE(MotionTCPin_4_B));
		ConfigControlIO(&pTC_Driver4->ControlIO, True); //keep stepper as reset
		pTC_Driver4->TC_DirPin.PinIO = MotionTCPin_4_B;
		pTC_Driver4->TC_DirPin.ValidLevel = pTC_Driver4->ChannelValidLevel[1];
	}
	
	IRQ_ConfigureIT(AT91C_ID_TC, IRQ_PRI_TC, TCHandler);
	IRQ_EnableIT(AT91C_ID_TC);
	
	if(pTC_Driver2 == NULL && pTC_Driver1 == NULL)
		return False;
	else
		return True;
}

INT8U Motion_Init()
{
	INT32U i;
	INT8U ret = True;
	
	currCurveBuf = ACCTABLE_ADDR;
	
	//read e2prom.
	if(!ReadMotionEPRConfig())
		return False;
	
	memset(&AccCurveIndexTbl, 0, sizeof(struct ACCCurveIndexTable));
	
	for(i = 0; i < motionParam.AxisCount; i++)
	{
		struct AxisParam_PulseDir_OpenLoop * pStepperTC = NULL;
		struct EPR_MOTION_AXIS_PARAM * pAxisParam = &motionParam.axis_param[i];
		float ratio;
		
		if(pAxisParam->AxisID == AXIS_ID_Z)
		{
			if( pAxisParam->motorType != MT_Stepper || 
			   pAxisParam->controlType != CT_OPEN_LOOP ||
				   pAxisParam->driveType != DT_PulseAndDir ||
					   (pAxisParam->PWM_ChannelID >= 4) )
				return False;
			else
				pStepperTC = &ZParam;
		}
		else if(pAxisParam->AxisID == AXIS_ID_C)
		{
			if( pAxisParam->motorType != MT_Stepper || 
			   pAxisParam->controlType != CT_OPEN_LOOP ||
				   pAxisParam->driveType != DT_PulseAndDir ||
					   (pAxisParam->PWM_ChannelID >= 4) )
				return False;
			else
				pStepperTC = &CParam;
		}
		else if(pAxisParam->AxisID == AXIS_ID_Z_DRIVER)
		{
			if( pAxisParam->motorType != MT_Stepper || 
			   pAxisParam->controlType != CT_OPEN_LOOP ||
				   pAxisParam->driveType != DT_PulseAndDir ||
					   (pAxisParam->PWM_ChannelID >= 4) )
				return False;
			else
				pStepperTC = &Z_DriverParam;
		}
		else if(pAxisParam->AxisID == AXIS_ID_C_DRIVER)
		{
			if( pAxisParam->motorType != MT_Stepper || 
			   pAxisParam->controlType != CT_OPEN_LOOP ||
				   pAxisParam->driveType != DT_PulseAndDir ||
					   (pAxisParam->PWM_ChannelID >= 4) )
				return False;
			else
				pStepperTC = &C_DriverParam;
		}
		else
			return False;
		
		if(pStepperTC != NULL)
		{   //open_loop, pulse_dir
			memset(pStepperTC, 0, sizeof(struct AxisParam_PulseDir_OpenLoop));
			pStepperTC->AxisID = pAxisParam->AxisID;
			pStepperTC->ChannelNO = pAxisParam->PWM_ChannelID;
			
			ratio = pAxisParam->transmisstion_ratio_open_loop;
			pStepperTC->ratio_move = ratio;
			
			if(!ConfigSensorPin(&(pStepperTC->Origin), pAxisParam, MSID_ORIGIN))
			{
				if(!(pAxisParam->option & SO_ONLY_INCREMENT_MOVEMENT))
					return False;
				pStepperTC->OriginSensorOffset = 0;
			}
			else
				pStepperTC->OriginSensorOffset = (INT32S)(ratio * pAxisParam->OriginOffset);
			ConfigSensorPin(&(pStepperTC->Limit), pAxisParam, MSID_LIMIT);
			
			pStepperTC->Origin_Offset = 0;
			if(!(pAxisParam->option & SO_FIXED_REGION) && pStepperTC->Limit.IsValid)
			{
				pStepperTC->IsCheckTerm = True;
				pStepperTC->Region_End = (INT32S)(0x7FFFFFFF);
			}
			else
			{
				pStepperTC->IsCheckTerm = False;
				pStepperTC->Region_End = (INT32S)(0x7FFFFFFF);
				if(pAxisParam->option & SO_FIXED_REGION)
				{
					pStepperTC->Region_End = (INT32S)(ratio * pAxisParam->Fixed_MoveRegion_EndPoint);
					pStepperTC->Region_End += pStepperTC->Origin_Offset;
				}
			}
			if(pAxisParam->Fixed_MoveRegion_StartPoint != 0xFFFFFFFF &&
			   pAxisParam->Fixed_MoveRegion_StartPoint != 0x80000000 &&
				   pAxisParam->Fixed_MoveRegion_StartPoint != 0x80000001 )
			{
				pStepperTC->Region_Start = (INT32S)(ratio * pAxisParam->Fixed_MoveRegion_StartPoint);
				pStepperTC->Region_Start += pStepperTC->Origin_Offset;
			}   
			else
				pStepperTC->Region_Start = (INT32S)(0x80000001);
			
			//channel info.
			if(pStepperTC->ChannelNO == 0)
			{
				pTC_Driver1 = pStepperTC;
				pStepperTC->pTC_channel = AT91C_BASE_TC0;
			}
			else if(pStepperTC->ChannelNO == 1)
			{
				pTC_Driver2 = pStepperTC;
				pStepperTC->pTC_channel = AT91C_BASE_TC3;
			}
			else if(pStepperTC->ChannelNO == 2)
			{
				pTC_Driver3 = pStepperTC;
				pStepperTC->pTC_channel = AT91C_BASE_TC1;
			}
			else
			{
				pTC_Driver4 = pStepperTC;
				pStepperTC->pTC_channel = AT91C_BASE_TC2;
			}
			
			if( pAxisParam->ControlIOPolarityMask & (1<<MCID_PWM1))
				pStepperTC->ChannelValidLevel[0] = True;
			else
				pStepperTC->ChannelValidLevel[0] = False;
			
			if( pAxisParam->ControlIOPolarityMask & (1<<MCID_PWM2))
				pStepperTC->ChannelValidLevel[1] = True;
			else
				pStepperTC->ChannelValidLevel[1] = False;
			
			if(!ConfigCtrlPin(&pStepperTC->ControlIO, pAxisParam, MCID_Ctrl0))
				return False;
			
			//usually, be 1.
			//Fix point number which frac bit is 24. 
			pStepperTC->ratio_User2Control = (INT32U)(ratio * (1<<24)) ; 
			pStepperTC->ratio_move = ratio;
			
			//the target coor when stop. the real coor can be different with ideal coor. the next movement will compensate the offset..
			pStepperTC->CoorIdeal = pStepperTC->Origin_Offset; 
			pStepperTC->CurCoor = pStepperTC->CoorIdeal; 
			
			pStepperTC->Dir = DIRTYPE_STANDBY;
			pStepperTC->MoveType = MOVETYPE_CONTINUE;
			pStepperTC->AccSegment = ACCSEGTYPE_SPEED_UP;
			
			pStepperTC->speedDownCoor = pStepperTC->Origin_Offset;
			pStepperTC->speedDownIndex = 0;
			
			pStepperTC->pAccTable = NULL; //the new cycle-time after a cycle. 
			pStepperTC->AccTableLen = 0;
			pStepperTC->AccTableIndex = 0;
			
			if(!BuildAllPulseCurve(pStepperTC, pAxisParam))
				status_ReportStatus(STATUS_FTA + SciError_Parameter, STATUS_SET);
			
			pStepperTC->Min_Distance = 0;
			pStepperTC->pAxisParam = pAxisParam;
		}
	}
	
	ret &= InitTCChannel();
	
	mainStatus = MS_IDLE;
	
	return ret;
}

INT8U reportMoveCompleted(INT8U ChannelNO)
{
	INT8U err;
	INT32U flag = (MOTION_MOVE_COMPLETED_Z << (ChannelNO-1));
	return flag == OSFlagPost(mix_FLAG_GRP, flag, OS_FLAG_SET, &err); 
}

INT8U ExitResetStatus()
{
	if(pTC_Driver1 != NULL)
	{
		if(!pTC_Driver1->ControlIO.ValidLevel)
			PIO_Set(&pTC_Driver1->ControlIO.PinIO);
		else
			PIO_Clear(&pTC_Driver1->ControlIO.PinIO);
	}
	if(pTC_Driver2 != NULL)
	{
		if(!pTC_Driver2->ControlIO.ValidLevel)
			PIO_Set(&pTC_Driver2->ControlIO.PinIO);
		else
			PIO_Clear(&pTC_Driver2->ControlIO.PinIO);
	}
	return True;
}

void TCHandler_PosDir_Move(struct AxisParam_PulseDir_OpenLoop * pStepperTC) FAST_FUNC_SECTION
{
	pStepperTC->CurCoor ++;
	
	if(pStepperTC->AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		if( (INT32S)pStepperTC->CurCoor >= (INT32S)pStepperTC->speedDownCoor )
		{
			pStepperTC->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pStepperTC->AccTableIndex = pStepperTC->speedDownIndex;
			pStepperTC->pTC_channel->TC_RC = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
			pStepperTC->pTC_channel->TC_RA = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
		}
	}
	else if(pStepperTC->AccSegment == ACCSEGTYPE_SPEED_UP)
	{
		pStepperTC->AccTableIndex ++;
		if( (INT32S)pStepperTC->CurCoor >= (INT32S)pStepperTC->speedDownCoor)
		{
			pStepperTC->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pStepperTC->AccTableIndex = pStepperTC->speedDownIndex;
			pStepperTC->pTC_channel->TC_RC = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
			pStepperTC->pTC_channel->TC_RA = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
		}
		else if(pStepperTC->AccTableIndex == pStepperTC->AccTableLen)
		{
			pStepperTC->AccSegment = ACCSEGTYPE_CONSTANT_SPEED;
		}
		else
		{
			pStepperTC->pTC_channel->TC_RC = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
			pStepperTC->pTC_channel->TC_RA = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
		}
	}
	else
	{
		if( pStepperTC->AccTableIndex == 0)
		{
			TC_Stop_Move(pStepperTC);
			reportMoveCompleted(pStepperTC->AxisID);
		}
		else
		{
			pStepperTC->AccTableIndex --;
			pStepperTC->pTC_channel->TC_RC = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
			pStepperTC->pTC_channel->TC_RA = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
		}
	}
}

void TCHandler_NegDir_Move(struct AxisParam_PulseDir_OpenLoop * pStepperTC) FAST_FUNC_SECTION
{
	INT16U A;
	pStepperTC->CurCoor --;
	
	if(pStepperTC->AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		if( (INT32S)pStepperTC->CurCoor <= (INT32S)pStepperTC->speedDownCoor )
		{
			pStepperTC->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pStepperTC->AccTableIndex = pStepperTC->speedDownIndex;
			pStepperTC->pTC_channel->TC_RC = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
			if(pStepperTC->pTC_channel->TC_RA > (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2)
				A = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
			else
				A = pStepperTC->pTC_channel->TC_RA;
			A -= 2;
			if(pStepperTC->pTC_channel->TC_CV < A)
				pStepperTC->pTC_channel->TC_RA = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
		}
	}
	else if(pStepperTC->AccSegment == ACCSEGTYPE_SPEED_UP)
	{
		pStepperTC->AccTableIndex ++;
		if( (INT32S)pStepperTC->CurCoor <= (INT32S)pStepperTC->speedDownCoor)
		{
			pStepperTC->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pStepperTC->AccTableIndex = pStepperTC->speedDownIndex;
			pStepperTC->pTC_channel->TC_RC = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
			if(pStepperTC->pTC_channel->TC_RA > (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2)
				A = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
			else
				A = pStepperTC->pTC_channel->TC_RA;
			A -= 2;
			if(pStepperTC->pTC_channel->TC_CV < A)
				pStepperTC->pTC_channel->TC_RA = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
		}
		else if(pStepperTC->AccTableIndex == pStepperTC->AccTableLen)
		{
			pStepperTC->AccSegment = ACCSEGTYPE_CONSTANT_SPEED;
		}
		else
		{
			pStepperTC->pTC_channel->TC_RC = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
			if(pStepperTC->pTC_channel->TC_RA > (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2)
				A = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
			else
				A = pStepperTC->pTC_channel->TC_RA;
			A -= 2;
			if(pStepperTC->pTC_channel->TC_CV < A)
				pStepperTC->pTC_channel->TC_RA = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
		}
	}
	else
	{
		if( pStepperTC->AccTableIndex == 0)
		{
			TC_Stop_Move(pStepperTC);
			reportMoveCompleted(pStepperTC->AxisID);
		}
		else
		{
			pStepperTC->AccTableIndex --;
			pStepperTC->pTC_channel->TC_RC = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
			if(pStepperTC->pTC_channel->TC_RA > (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2)
				A = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
			else
				A = pStepperTC->pTC_channel->TC_RA;
			A -= 2;
			if(pStepperTC->pTC_channel->TC_CV < A)
				pStepperTC->pTC_channel->TC_RA = (pStepperTC->pAccTable[pStepperTC->AccTableIndex])/2;
		}
	}
}

INT8U TC_Start_Standy(struct AxisParam_PulseDir_OpenLoop * pStepperTC)
{
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();
	pStepperTC->CoorIdeal = pStepperTC->CurCoor;
	
	pStepperTC->Dir = DIRTYPE_STANDBY; //0,standby, 1, positive, 2, negative.
	pStepperTC->MoveType = MOVETYPE_CONTINUE; //0, segment move; 1, continue move, and stop when receive stop cmd.
	pStepperTC->AccSegment = ACCSEGTYPE_CONSTANT_SPEED; //0: constant, 1: speed up, 2: speed down.
	
	pStepperTC->speedDownCoor = pStepperTC->CoorIdeal;
	pStepperTC->speedDownIndex = 0;
	
	pStepperTC->pAccTable = NULL; 
	pStepperTC->AccTableLen = 0;
	pStepperTC->AccTableIndex = 0;
	
	pStepperTC->pTC_channel->TC_CCR = AT91C_TC_CLKDIS;
	pStepperTC->pTC_channel->TC_IDR = AT91C_TC_COVFS | AT91C_TC_LOVRS | AT91C_TC_CPAS | AT91C_TC_CPBS |
		AT91C_TC_CPCS | AT91C_TC_LDRAS | AT91C_TC_LDRBS | AT91C_TC_ETRGS;
	pStepperTC->pTC_channel->TC_SR;
	if(pStepperTC->ChannelValidLevel[0])
		pStepperTC->pTC_channel->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
			AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
				AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	else
		pStepperTC->pTC_channel->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
			AT91C_TC_ACPA_CLEAR | AT91C_TC_ACPC_SET | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
				AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	if(pStepperTC->TC_DirPin.ValidLevel)
		PIO_Clear(&pStepperTC->TC_DirPin.PinIO);
	else
		PIO_Set(&pStepperTC->TC_DirPin.PinIO);
	
	if(pStepperTC->ChannelNO == 0)
		pTC1Handle = TCHandler_dimmy;
	else if(pStepperTC->ChannelNO == 1)
		pTC2Handle = TCHandler_dimmy;
	else if(pStepperTC->ChannelNO == 2)
		pTC3Handle = TCHandler_dimmy;
	else
		pTC4Handle = TCHandler_dimmy; 
	OS_EXIT_CRITICAL();
	return True;
}

INT8U TC_Start_Move(struct AxisParam_PulseDir_OpenLoop * pStepperTC, INT8U speedID, INT32U endCoor, INT8U moveType)
{
	OS_CPU_SR cpu_sr;
	INT32U distance;
	struct ACCCurveInfo * pAccTable;
	
	OS_ENTER_CRITICAL();
	
	pStepperTC->CoorIdeal = endCoor;
	
	if(pStepperTC->CoorIdeal > pStepperTC-> CurCoor)
	{
		distance = pStepperTC->CoorIdeal - pStepperTC-> CurCoor;
		pStepperTC->Dir = DIRTYPE_POS;
	}
	else
	{
		distance = pStepperTC-> CurCoor - pStepperTC->CoorIdeal;
		pStepperTC->Dir = DIRTYPE_NEG;
	}
	
	//Dir pin need 200ns setup time before Pulse pin.
	//I put it at the begin of this function.
	if((pStepperTC->TC_DirPin.ValidLevel && pStepperTC->Dir == DIRTYPE_POS) ||
	   (!pStepperTC->TC_DirPin.ValidLevel && pStepperTC->Dir == DIRTYPE_NEG) )
		PIO_Set(&pStepperTC->TC_DirPin.PinIO); // Positive rotate.
	else
		PIO_Clear(&pStepperTC->TC_DirPin.PinIO);
	
	pStepperTC->MoveType = moveType;
	
	pAccTable = pStepperTC->pAllCurveTable[speedID];
	pStepperTC->pAccTable = (INT16U*)pAccTable->IncTable; 
	pStepperTC->AccTableLen = pAccTable->AccCurveLen;
	if( distance < 2)
	{
		pStepperTC->AccSegment = ACCSEGTYPE_SPEED_DOWN; 
		pStepperTC->AccTableIndex = 0;
		pStepperTC->speedDownCoor = pStepperTC-> CurCoor;
		pStepperTC->speedDownIndex = 0;
	}
	else if(distance <= 2 * pStepperTC->AccTableLen)
	{
		pStepperTC->AccSegment = ACCSEGTYPE_SPEED_UP; 
		pStepperTC->AccTableIndex = 0;
		if( pStepperTC->Dir == DIRTYPE_POS)
			pStepperTC->speedDownCoor = pStepperTC->CoorIdeal - distance/2;
		else
			pStepperTC->speedDownCoor = pStepperTC->CoorIdeal + distance/2;
		pStepperTC->speedDownIndex = distance/2 - 1;
	}
	else
	{
		pStepperTC->AccSegment = ACCSEGTYPE_SPEED_UP; 
		pStepperTC->AccTableIndex = 0;
		if( pStepperTC->Dir == DIRTYPE_POS)
			pStepperTC->speedDownCoor = pStepperTC->CoorIdeal - pStepperTC->AccTableLen;
		else
			pStepperTC->speedDownCoor = pStepperTC->CoorIdeal + pStepperTC->AccTableLen;
		pStepperTC->speedDownIndex = pStepperTC->AccTableLen - 1;
	}
	
	if( pStepperTC->ChannelValidLevel[0])
		pStepperTC->pTC_channel->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
			AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
				AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	else
		pStepperTC->pTC_channel->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
			AT91C_TC_ACPA_CLEAR | AT91C_TC_ACPC_SET | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
				AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	
	if(pStepperTC->ChannelNO == 0)
	{
		if( pStepperTC->Dir == DIRTYPE_POS)
			pTC1Handle = TCHandler_PosDir_Move;
		else
			pTC1Handle = TCHandler_NegDir_Move;
	}
	else if(pStepperTC->ChannelNO == 1)
	{
		if( pStepperTC->Dir == DIRTYPE_POS)
			pTC2Handle = TCHandler_PosDir_Move;
		else
			pTC2Handle = TCHandler_NegDir_Move;
	}
	else if(pStepperTC->ChannelNO == 2)
	{
		if( pStepperTC->Dir == DIRTYPE_POS)
			pTC3Handle = TCHandler_PosDir_Move;
		else
			pTC3Handle = TCHandler_NegDir_Move;
	} 
	else
	{
		if( pStepperTC->Dir == DIRTYPE_POS)
			pTC4Handle = TCHandler_PosDir_Move;
		else
			pTC4Handle = TCHandler_NegDir_Move; 
	}
	
	pStepperTC->pTC_channel->TC_RA = pStepperTC->pAccTable[pStepperTC->AccTableIndex]/2;
	pStepperTC->pTC_channel->TC_RC = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
	pStepperTC->pTC_channel->TC_RB = pStepperTC->pAccTable[pStepperTC->AccTableIndex];
	
#ifndef VM_PRINT
	pStepperTC->pTC_channel->TC_IER = AT91C_TC_CPCS;
#endif
	pStepperTC->pTC_channel->TC_SR;
	pStepperTC->pTC_channel->TC_CCR = AT91C_TC_CLKEN|AT91C_TC_SWTRG;
	OS_EXIT_CRITICAL();
	return True;
}

INT8U TC_Stop_Move(struct AxisParam_PulseDir_OpenLoop * pStepperTC)
{
	OS_CPU_SR cpu_sr;
	INT32U isr;
	
	OS_ENTER_CRITICAL();
	isr = pStepperTC->pTC_channel->TC_SR;
	pStepperTC->pTC_channel->TC_CCR = AT91C_TC_CLKDIS;
	if((isr & AT91C_TC_CLKSTA) && (isr & AT91C_TC_CPCS))
	{//RC Compare already take place, but interrupt doesn't tirgger on time.
		if( pStepperTC->Dir == DIRTYPE_POS)
			pStepperTC->CurCoor ++;
		else
			pStepperTC->CurCoor --;
	}
	if((isr & AT91C_TC_CLKSTA) && (((isr & AT91C_TC_MTIOA) && pStepperTC->ChannelValidLevel[0]) || 
								   (!(isr & AT91C_TC_MTIOA) && !pStepperTC->ChannelValidLevel[0])) )
	{ //Pulse pin already send rise-edge jump. the stepper had received this signal.
		if( pStepperTC->Dir == DIRTYPE_POS)
			pStepperTC->CurCoor ++;
		else
			pStepperTC->CurCoor --;
	}
	if(pStepperTC->ChannelNO == 0)
		pTC1Handle = TCHandler_dimmy;
	else if(pStepperTC->ChannelNO == 1)
		pTC2Handle = TCHandler_dimmy;
	else if(pStepperTC->ChannelNO == 2)
		pTC3Handle = TCHandler_dimmy;
	else
		pTC4Handle = TCHandler_dimmy; 
	
	pStepperTC->Dir = DIRTYPE_STANDBY;
	
	OS_EXIT_CRITICAL();
	return True;
}

void TCHandler(void) FAST_FUNC_SECTION
{
	INT32U isr;
	
	if(pTC_Driver1 != NULL)
	{
		isr = AT91C_BASE_TC0->TC_SR & AT91C_BASE_TC0->TC_IMR;
		if(isr & AT91C_TC_CPCS)
			(*pTC1Handle)(pTC_Driver1);
	}
	if(pTC_Driver2 != NULL)
	{
		isr = AT91C_BASE_TC3->TC_SR & AT91C_BASE_TC3->TC_IMR;
		if(isr & AT91C_TC_CPCS)
			(*pTC2Handle)(pTC_Driver2);
	}
	if(pTC_Driver3 != NULL)
	{
		isr = AT91C_BASE_TC1->TC_SR & AT91C_BASE_TC1->TC_IMR;
		if(isr & AT91C_TC_CPCS)
			(*pTC3Handle)(pTC_Driver3);
	}
	if(pTC_Driver4 != NULL)
	{
		isr = AT91C_BASE_TC2->TC_SR & AT91C_BASE_TC2->TC_IMR;
		if(isr & AT91C_TC_CPCS)
			(*pTC4Handle)(pTC_Driver4);
	}
}

#define START_ERROR_NOT_STANDBY         ((INT8U)-1)
#define START_ERROR_CHECK_SAFE          ((INT8U)0)
#define START_ERROR_OK                  ((INT8U)True)
#define START_ERROR_DISTANCE_TOO_SHORT  ((INT8U)2)
#define START_ERROR_NOT_ROLLBACK        ((INT8U)3)
#define INIT_RETURN_SAFE_DISTANCE	(720*5)  //5 inch.
INT8U User_Start_Z_MOTION(INT8U speedID, INT8U moveType, INT8U coorType, INT32S endCoorOrDistance, INT8U dir)
{
	OS_CPU_SR cpu_sr;
	INT32S endCoor, distance;
	
	OS_ENTER_CRITICAL();
	if(ZParam.Dir != DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return START_ERROR_NOT_STANDBY;
	}
	
	if(moveType == MOVETYPE_CONTINUE || moveType == MOVETYPE_CONTINUE_CHECKSENSOR)
	{ 
		if(dir == DIRTYPE_POS)
			endCoor = ZParam.Region_End;
		else
			endCoor = ZParam.Region_Start;
	}
	else
	{
		endCoorOrDistance = (INT32S)(endCoorOrDistance * ZParam.ratio_move);
		if( coorType == 0) //absolute
			endCoor = ZParam.Origin_Offset + endCoorOrDistance;
		else
		{
			if(dir == DIRTYPE_POS) //Positive dir
				endCoor = ZParam.CoorIdeal + endCoorOrDistance;
			else
				endCoor = ZParam.CoorIdeal - endCoorOrDistance;
		}
	}
	
	if( endCoor > ZParam.CurCoor)
	{
		distance = endCoor - ZParam.CurCoor;
		dir = DIRTYPE_POS;
	}
	else
	{
		distance = ZParam.CurCoor - endCoor;
		dir = DIRTYPE_NEG;
	}
	
#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
	if((distance <= ZParam.Min_Distance) || ((GetSensor(&ZParam.Limit) == SENSOR_ZC_ORIGIN) && (dir == DIRTYPE_POS)) \
			|| ((GetSensor(&ZParam.Origin) == SENSOR_ZC_ORIGIN) && (dir == DIRTYPE_NEG)))
#else
	if( distance <= ZParam.Min_Distance)
#endif
	{
		OS_EXIT_CRITICAL();
		TC_Start_Standy(&ZParam);
		reportMoveCompleted(ZParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	
	OS_EXIT_CRITICAL();
	
	TC_Start_Move(&ZParam, speedID, endCoor, moveType);
	
	return START_ERROR_OK;
}

INT8U User_Start_Z_DRIVER(INT8U speedID, INT8U moveType, INT8U coorType, INT32S endCoorOrDistance, INT8U dir)
{
	OS_CPU_SR cpu_sr;
	INT32S endCoor, distance;
	
	OS_ENTER_CRITICAL();
	if(Z_DriverParam.Dir != DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return START_ERROR_NOT_STANDBY;
	}
	
	if(moveType == MOVETYPE_CONTINUE || moveType == MOVETYPE_CONTINUE_CHECKSENSOR)
	{ 
		if(dir == DIRTYPE_POS)
			endCoor = Z_DriverParam.Region_End;
		else
			endCoor = Z_DriverParam.Region_Start;
	}
	else
	{
		endCoorOrDistance = (INT32S)(endCoorOrDistance * Z_DriverParam.ratio_move);
		if( coorType == 0) //absolute
			endCoor = Z_DriverParam.Origin_Offset + endCoorOrDistance;
		else
		{
			if(dir == DIRTYPE_POS) //Positive dir
				endCoor = Z_DriverParam.CoorIdeal + endCoorOrDistance;
			else
				endCoor = Z_DriverParam.CoorIdeal - endCoorOrDistance;
		}
	}
	
	if( endCoor > Z_DriverParam.CurCoor)
	{
		distance = endCoor - Z_DriverParam.CurCoor;
		dir = DIRTYPE_POS;
	}
	else
	{
		distance = Z_DriverParam.CurCoor - endCoor;
		dir = DIRTYPE_NEG;
	}
#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
	if((distance <= Z_DriverParam.Min_Distance) || ((GetSensor(&Z_DriverParam.Limit) == SENSOR_ZC_ORIGIN) && (dir == DIRTYPE_POS)) \
		|| ((GetSensor(&Z_DriverParam.Origin) == SENSOR_ZC_ORIGIN) && (dir == DIRTYPE_NEG)))
#else
	if( distance <= Z_DriverParam.Min_Distance)
#endif
	{
		OS_EXIT_CRITICAL();
		TC_Start_Standy(&Z_DriverParam);
		reportMoveCompleted(Z_DriverParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	
	OS_EXIT_CRITICAL();
	
	TC_Start_Move(&Z_DriverParam, speedID, endCoor, moveType);
	
	return START_ERROR_OK;
}
INT8U User_Start_Z(INT8U speedID, INT8U moveType, INT8U coorType, INT32S endCoorOrDistance, INT8U dir)
{
	MultiAxis = 0x01|0x04;
	User_Start_Z_MOTION(speedID, moveType, coorType, endCoorOrDistance*STEPPER_ROTIO_SCALE, dir);
	User_Start_Z_DRIVER(speedID, moveType, coorType, endCoorOrDistance, dir);
	
	return START_ERROR_OK;
}

INT8U User_Start_C_MOTION(INT8U speedID, INT8U moveType, INT8U coorType, INT32S endCoorOrDistance, INT8U dir)
{
	OS_CPU_SR cpu_sr;
	INT32S endCoor, distance;
	
	OS_ENTER_CRITICAL();
	if(CParam.Dir != DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return START_ERROR_NOT_STANDBY;
	}
	
	if(moveType == MOVETYPE_CONTINUE || moveType == MOVETYPE_CONTINUE_CHECKSENSOR)
	{ 
		if(dir == DIRTYPE_POS)
			endCoor = CParam.Region_End;
		else
			endCoor = CParam.Region_Start;
	}
	else
	{
		endCoorOrDistance = (INT32S)(endCoorOrDistance * CParam.ratio_move);
		if( coorType == 0) //absolute
			endCoor = CParam.Origin_Offset + endCoorOrDistance;
		else
		{
			if(dir == DIRTYPE_POS) //Positive dir
				endCoor = CParam.CoorIdeal + endCoorOrDistance;
			else
				endCoor = CParam.CoorIdeal - endCoorOrDistance;
		}
	}
	
	if( endCoor > CParam.CurCoor)
	{
		distance = endCoor - CParam.CurCoor;
		dir = DIRTYPE_POS;
	}
	else
	{
		distance = CParam.CurCoor - endCoor;
		dir = DIRTYPE_NEG;
	}
	
	if( distance <= CParam.Min_Distance)
	{
		OS_EXIT_CRITICAL();
		TC_Start_Standy(&CParam);
		reportMoveCompleted(CParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	
	OS_EXIT_CRITICAL();
	
	TC_Start_Move(&CParam, speedID, endCoor, moveType);
	
	return START_ERROR_OK;
}

INT8U User_Start_C_DRIVER(INT8U speedID, INT8U moveType, INT8U coorType, INT32S endCoorOrDistance, INT8U dir)
{
	OS_CPU_SR cpu_sr;
	INT32S endCoor, distance;
	
	
	OS_ENTER_CRITICAL();
	if(C_DriverParam.Dir != DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return START_ERROR_NOT_STANDBY;
	}
	
	if(moveType == MOVETYPE_CONTINUE || moveType == MOVETYPE_CONTINUE_CHECKSENSOR)
	{ 
		if(dir == DIRTYPE_POS)
			endCoor = C_DriverParam.Region_End;
		else
			endCoor = C_DriverParam.Region_Start;
	}
	else
	{
		endCoorOrDistance = (INT32S)(endCoorOrDistance * C_DriverParam.ratio_move);
		if( coorType == 0) //absolute
			endCoor = C_DriverParam.Origin_Offset + endCoorOrDistance;
		else
		{
			if(dir == DIRTYPE_POS) //Positive dir
				endCoor = C_DriverParam.CoorIdeal + endCoorOrDistance;
			else
				endCoor = C_DriverParam.CoorIdeal - endCoorOrDistance;
		}
	}
	
	if( endCoor > C_DriverParam.CurCoor)
	{
		distance = endCoor - C_DriverParam.CurCoor;
		dir = DIRTYPE_POS;
	}
	else
	{
		distance = C_DriverParam.CurCoor - endCoor;
		dir = DIRTYPE_NEG;
	}
	
	if( distance <= C_DriverParam.Min_Distance)
	{
		OS_EXIT_CRITICAL();
		TC_Start_Standy(&C_DriverParam);
		reportMoveCompleted(C_DriverParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	
	OS_EXIT_CRITICAL();
	
	TC_Start_Move(&C_DriverParam, speedID, endCoor, moveType);
	
	return START_ERROR_OK;
}

INT8U User_Start_C(INT8U speedID, INT8U moveType, INT8U coorType, INT32S endCoorOrDistance, INT8U dir)
{
	MultiAxis = 0x02|0x08;
	User_Start_C_MOTION(speedID, moveType, coorType, endCoorOrDistance*STEPPER_ROTIO_SCALE, dir);
	User_Start_C_DRIVER(speedID, moveType, coorType, endCoorOrDistance, dir);
	
	return START_ERROR_OK;
}

INT8U User_Stop_Z_MOTION(INT8U bUrgent)
{
	OS_CPU_SR cpu_sr;
	INT32S distance;
	
	if(bUrgent)
	{
		return TC_Stop_Move(&ZParam);
	}
	
	OS_ENTER_CRITICAL();
	if(ZParam.Dir == DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		return False;
	}
	
	if(ZParam.AccSegment == ACCSEGTYPE_SPEED_UP)
	{
		ZParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(ZParam.Dir == DIRTYPE_POS)
			ZParam.speedDownCoor = ZParam.CurCoor + 1;
		else
			ZParam.speedDownCoor = ZParam.CurCoor - 1;
		if(ZParam.AccTableIndex < ZParam.AccTableLen)
			ZParam.AccTableIndex ++;
		ZParam.speedDownIndex = ZParam.AccTableIndex;
		distance = ZParam.AccTableIndex;
		if(ZParam.Dir == DIRTYPE_POS)
			ZParam.CoorIdeal = ZParam.speedDownCoor + distance;
		else
			ZParam.CoorIdeal = ZParam.speedDownCoor - distance;
	}
	else if(ZParam.AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		ZParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(ZParam.Dir == DIRTYPE_POS)
			ZParam.speedDownCoor = ZParam.CurCoor + 1;
		else
			ZParam.speedDownCoor = ZParam.CurCoor - 1;
		ZParam.AccTableIndex = ZParam.AccTableLen;
		ZParam.speedDownIndex = ZParam.AccTableIndex;
		distance = ZParam.AccTableIndex;
		if(ZParam.Dir == DIRTYPE_POS)
			ZParam.CoorIdeal = ZParam.speedDownCoor + distance;
		else
			ZParam.CoorIdeal = ZParam.speedDownCoor - distance;
	}
	
	OS_EXIT_CRITICAL();
	
	return True;
}

INT8U User_Stop_Z_DRIVER(INT8U bUrgent)
{
	OS_CPU_SR cpu_sr;
	INT32S distance;
	
	if(bUrgent)
	{
		return TC_Stop_Move(&Z_DriverParam);
	}
	
	OS_ENTER_CRITICAL();
	if(Z_DriverParam.Dir == DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		return False;
	}
	
	if(Z_DriverParam.AccSegment == ACCSEGTYPE_SPEED_UP)
	{
		Z_DriverParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(Z_DriverParam.Dir == DIRTYPE_POS)
			Z_DriverParam.speedDownCoor = Z_DriverParam.CurCoor + 1;
		else
			Z_DriverParam.speedDownCoor = Z_DriverParam.CurCoor - 1;
		if(Z_DriverParam.AccTableIndex < Z_DriverParam.AccTableLen)
			Z_DriverParam.AccTableIndex ++;
		Z_DriverParam.speedDownIndex = Z_DriverParam.AccTableIndex;
		distance = Z_DriverParam.AccTableIndex;
		if(Z_DriverParam.Dir == DIRTYPE_POS)
			Z_DriverParam.CoorIdeal = Z_DriverParam.speedDownCoor + distance;
		else
			Z_DriverParam.CoorIdeal = Z_DriverParam.speedDownCoor - distance;
	}
	else if(Z_DriverParam.AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		Z_DriverParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(Z_DriverParam.Dir == DIRTYPE_POS)
			Z_DriverParam.speedDownCoor = Z_DriverParam.CurCoor + 1;
		else
			Z_DriverParam.speedDownCoor = Z_DriverParam.CurCoor - 1;
		Z_DriverParam.AccTableIndex = Z_DriverParam.AccTableLen;
		Z_DriverParam.speedDownIndex = Z_DriverParam.AccTableIndex;
		distance = Z_DriverParam.AccTableIndex;
		if(Z_DriverParam.Dir == DIRTYPE_POS)
			Z_DriverParam.CoorIdeal = Z_DriverParam.speedDownCoor + distance;
		else
			Z_DriverParam.CoorIdeal = Z_DriverParam.speedDownCoor - distance;
	}
	
	OS_EXIT_CRITICAL();
	
	return True;
}

INT8U User_Stop_Z(INT8U bUrgent)
{
	User_Stop_Z_MOTION(bUrgent);
	User_Stop_Z_DRIVER(bUrgent);
	
	return True;
}

INT8U User_Stop_C_MOTION(INT8U bUrgent)
{
	OS_CPU_SR cpu_sr;
	INT32S distance;
	
	if(bUrgent)
	{
		return TC_Stop_Move(&CParam);
	}
	
	OS_ENTER_CRITICAL();
	if(CParam.Dir == DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		return False;
	}
	
	if(CParam.AccSegment == ACCSEGTYPE_SPEED_UP)
	{
		CParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(CParam.Dir == DIRTYPE_POS)
			CParam.speedDownCoor = CParam.CurCoor + 1;
		else
			CParam.speedDownCoor = CParam.CurCoor - 1;
		if(CParam.AccTableIndex < CParam.AccTableLen)
			CParam.AccTableIndex ++;
		CParam.speedDownIndex = CParam.AccTableIndex;
		distance = CParam.AccTableIndex;
		if(CParam.Dir == DIRTYPE_POS)
			CParam.CoorIdeal = CParam.speedDownCoor + distance;
		else
			CParam.CoorIdeal = CParam.speedDownCoor - distance;
	}
	else if(CParam.AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		CParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(CParam.Dir == DIRTYPE_POS)
			CParam.speedDownCoor = CParam.CurCoor + 1;
		else
			CParam.speedDownCoor = CParam.CurCoor - 1;
		CParam.AccTableIndex = CParam.AccTableLen;
		CParam.speedDownIndex = CParam.AccTableIndex;
		distance = CParam.AccTableIndex;
		if(CParam.Dir == DIRTYPE_POS)
			CParam.CoorIdeal = CParam.speedDownCoor + distance;
		else
			CParam.CoorIdeal = CParam.speedDownCoor - distance;
	}
	
	OS_EXIT_CRITICAL();
	
	return True;
}
INT8U User_Stop_C_DRIVER(INT8U bUrgent)
{
	OS_CPU_SR cpu_sr;
	INT32S distance;
	
	if(bUrgent)
	{
		return TC_Stop_Move(&C_DriverParam);
	}
	
	OS_ENTER_CRITICAL();
	if(C_DriverParam.Dir == DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		return False;
	}
	
	if(C_DriverParam.AccSegment == ACCSEGTYPE_SPEED_UP)
	{
		C_DriverParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(C_DriverParam.Dir == DIRTYPE_POS)
			C_DriverParam.speedDownCoor = C_DriverParam.CurCoor + 1;
		else
			C_DriverParam.speedDownCoor = C_DriverParam.CurCoor - 1;
		if(C_DriverParam.AccTableIndex < C_DriverParam.AccTableLen)
			C_DriverParam.AccTableIndex ++;
		C_DriverParam.speedDownIndex = C_DriverParam.AccTableIndex;
		distance = C_DriverParam.AccTableIndex;
		if(C_DriverParam.Dir == DIRTYPE_POS)
			C_DriverParam.CoorIdeal = C_DriverParam.speedDownCoor + distance;
		else
			C_DriverParam.CoorIdeal = C_DriverParam.speedDownCoor - distance;
	}
	else if(C_DriverParam.AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		C_DriverParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(C_DriverParam.Dir == DIRTYPE_POS)
			C_DriverParam.speedDownCoor = C_DriverParam.CurCoor + 1;
		else
			C_DriverParam.speedDownCoor = C_DriverParam.CurCoor - 1;
		C_DriverParam.AccTableIndex = C_DriverParam.AccTableLen;
		C_DriverParam.speedDownIndex = C_DriverParam.AccTableIndex;
		distance = C_DriverParam.AccTableIndex;
		if(C_DriverParam.Dir == DIRTYPE_POS)
			C_DriverParam.CoorIdeal = C_DriverParam.speedDownCoor + distance;
		else
			C_DriverParam.CoorIdeal = C_DriverParam.speedDownCoor - distance;
	}
	
	OS_EXIT_CRITICAL();
	
	return True;
}
INT8U User_Stop_C(INT8U bUrgent)
{
	User_Stop_C_MOTION(bUrgent);
	User_Stop_C_DRIVER(bUrgent);
	
	return True;
}

INT8U StartAllStandby()
{
	INT8U ret;
	if(pTC_Driver1 != NULL)
		ret &= TC_Start_Standy(pTC_Driver1);
	if(pTC_Driver2 != NULL)
		ret &= TC_Start_Standy(pTC_Driver2);
	if(pTC_Driver3 != NULL)
		ret &= TC_Start_Standy(pTC_Driver3);
	if(pTC_Driver4 != NULL)
		ret &= TC_Start_Standy(pTC_Driver4);
	return ret;
}

#if defined (EPSON_CLEAN_UPDOWN)||defined(RICOH_CLEAN_PRESS)
void ZOrigin_IO_Irq_Interrupt(const Pin *pPin) FAST_FUNC_SECTION
{
	if(mainStatus == IS_Z_GoOriginSensor)
	{
		if( GetSensor(&ZParam.Origin) == SENSOR_ZC_ORIGIN)
		{
			//OriginSensorPosi_X = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
			User_Stop_Z(False);
		}
	}
}
#endif

#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
void ZLimit_IO_Irq_Interrupt(const Pin *pPin) FAST_FUNC_SECTION
{
	if( (GetSensor(&ZParam.Limit) == SENSOR_ZC_ORIGIN) && (ZParam.Dir == DIRTYPE_POS) )
	{
		User_Stop_Z(False);
	}
}
#endif


void COrigin_IO_Irq_Interrupt(const Pin *pPin) FAST_FUNC_SECTION
{
	if(mainStatus == IS_C_GoOriginSensor)
	{
		if( GetSensor(&CParam.Origin) == SENSOR_ZC_ORIGIN)
		{
			//OriginSensorPosi_X = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
			User_Stop_C(False);
		}
	}
}


INT8U ResetTCCoor(struct AxisParam_PulseDir_OpenLoop * pStepperTC)
{
	pStepperTC->CoorIdeal = pStepperTC->CurCoor = pStepperTC->Origin_Offset;
	return False;
}
#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
INT8U ResetZTCCoor(void)
{
	ResetTCCoor(&ZParam);
	ResetTCCoor(&Z_DriverParam);

	return True;
}
#endif

INT8U MoveDistance(INT8U dir, INT8U speedid, INT32S distance)
{
	if(mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	if(speedid > 6)
		speedid = 6;	
	mainStatus = MS_NORMAL_MOVE;
	switch(dir)
	{
	case 5:
		return User_Start_Z(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_POS);
	case 6:
		return User_Start_Z(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_NEG);
	case 7:
		return User_Start_C(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_POS);
	case 8:
		return User_Start_C(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_NEG);
	default:
		mainStatus = MS_IDLE;
		status_ReportStatus(STATUS_FTA + SciError_Parameter, STATUS_SET);
		return False;
	}
}


INT8U AxisMoveTo(INT8U axis_no, INT8U speedid, INT32S targetPos)
{
	if( mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	if(speedid > 6)
		speedid = 6;	
	mainStatus = MS_NORMAL_MOVE;
	switch(axis_no)
	{
	case 4:
		return User_Start_Z(speedid, MOVETYPE_SEGMENT, 0, targetPos, DIRTYPE_POS);
	case 8:
		return User_Start_C(speedid, MOVETYPE_SEGMENT, 0, targetPos, DIRTYPE_NEG);
	default:
		mainStatus = MS_IDLE;
		status_ReportStatus(STATUS_FTA + SciError_Parameter, STATUS_SET);
		return False;
	}
}

INT8U StopMove()
{
	if(mainStatus != MS_NORMAL_MOVE)
		return False;
	
	User_Stop_Z(False);
	User_Stop_C(False);
	
	return True;
}
INT8U StartInitAction()
{
#if defined (EPSON_CLEAN_UPDOWN)||defined(RICOH_CLEAN_PRESS)
	#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
		ConfigureSensorIt(&ZParam.Limit, NULL, NULL, ZLimit_IO_Irq_Interrupt); 
	#endif
	if(GetSensor(&ZParam.Origin) == SENSOR_ZC_NO_ORIGIN)
	{
		mainStatus = IS_Z_GoOriginSensor;
		ConfigureSensorIt(&ZParam.Origin, NULL, NULL, ZOrigin_IO_Irq_Interrupt); 
		return User_Start_Z(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 1,0x0000FFFF, DIRTYPE_NEG);
	}
	else
	{
		mainStatus = IS_Z_LeaveOriginSensor;
	#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
		//ConfigureSensorIt(&ZParam.Origin, NULL, NULL, XOrigin_IO_Irq_Interrupt); 
		return User_Start_Z(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 1,0x000000FF, DIRTYPE_POS);
	#else
		return User_Start_Z(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 1,cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos, DIRTYPE_POS);

	#endif
	}
#else
	if(GetSensor(&CParam.Origin) == SENSOR_ZC_NO_ORIGIN)
	{
		mainStatus = IS_C_GoOriginSensor;
		ConfigureSensorIt(&CParam.Origin, NULL, NULL, COrigin_IO_Irq_Interrupt); 
		User_Start_C(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 1,0x0000FFFF, DIRTYPE_NEG);
	}
	else
	{
		mainStatus = IS_C_LeaveOriginSensor;
		User_Start_C(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 1,cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance, DIRTYPE_POS);
	}
	
#endif
}
void Motion_Task()
{
	INT8U err, ret;
	INT8U cmdbuf[32];
	OS_FLAGS flags;
	enum SciError reportErr = SciError_OK;
	
	ExitResetStatus();
	
	StartAllStandby();  
	
	StartInitAction();
	while(1)
	{
		
		flags = OSFlagPend(mix_FLAG_GRP, 
						   MOTION_URGENT_STOP_CMD|MOTION_SEND_CMD|MOTION_MOVE_COMPLETED_Z_DRIVER|MOTION_MOVE_COMPLETED_C_DRIVER|MOTION_MOVE_COMPLETED_Z|MOTION_MOVE_COMPLETED_C, 
						   OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME, 0, &err); 
		
		if(flags & MOTION_URGENT_STOP_CMD)
		{ 
			User_Stop_Z(True);
			User_Stop_C(True);
			continue;
		}
		
		if(flags & (MOTION_MOVE_COMPLETED_Z_DRIVER|MOTION_MOVE_COMPLETED_C_DRIVER|MOTION_MOVE_COMPLETED_Z|MOTION_MOVE_COMPLETED_C))
		{
			if(flags & MOTION_MOVE_COMPLETED_Z)
				MultiAxis &= ~(0x1);
			if(flags & MOTION_MOVE_COMPLETED_C)
				MultiAxis &= ~(0x2);
			if(flags & MOTION_MOVE_COMPLETED_Z_DRIVER)
				MultiAxis &= ~(0x4);
			if(flags & MOTION_MOVE_COMPLETED_C_DRIVER)
				MultiAxis &= ~(0x8);
			if(MultiAxis == 0)
			{
				StartAllStandby();
				switch(mainStatus)
				{
#if defined (EPSON_CLEAN_UPDOWN)||defined(RICOH_CLEAN_PRESS)
				case IS_Z_LeaveOriginSensor:
					if(GetSensor(&ZParam.Origin) == SENSOR_ZC_NO_ORIGIN)
					{
						mainStatus = IS_Z_GoOriginSensor;
						ConfigureSensorIt(&ZParam.Origin, NULL, NULL, ZOrigin_IO_Irq_Interrupt); 
						User_Start_Z(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 1,0x0000FFFF, DIRTYPE_NEG);
						break;
					}
				case IS_Z_GoOriginSensor:
					if(GetSensor(&CParam.Origin) == SENSOR_ZC_NO_ORIGIN)
					{
						mainStatus = IS_C_GoOriginSensor;
						ConfigureSensorIt(&CParam.Origin, NULL, NULL, COrigin_IO_Irq_Interrupt); 
						User_Start_C(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 1,0x0000FFFF, DIRTYPE_NEG);
					}
					else
					{
						mainStatus = IS_C_LeaveOriginSensor;
						User_Start_C(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 1,cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0], DIRTYPE_POS);
					}
					break;
#endif										
					//  case IS_Z_GoHome:
				case IS_C_LeaveOriginSensor:
					if(GetSensor(&CParam.Origin) == SENSOR_ZC_NO_ORIGIN)
					{
						mainStatus = IS_C_GoOriginSensor;
						ConfigureSensorIt(&CParam.Origin, NULL, NULL, COrigin_IO_Irq_Interrupt); 
						User_Start_C(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 1,0x000FFFFF, DIRTYPE_NEG);
						break;
					}
				case IS_C_GoOriginSensor:
#if defined(MANUFACTURER_ADDTOP_EPSON)&&defined(EPSON_CLEAN_INTEGRATE_1)
					mainStatus = IS_C_OK;
					ResetTCCoor(&CParam);
					ResetTCCoor(&ZParam);
					ResetTCCoor(&C_DriverParam);
					ResetTCCoor(&Z_DriverParam);
					User_Start_C(TATE_CLEAN_CZ_SPEED_LEVEL, MOVETYPE_SEGMENT, 0,cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance, DIRTYPE_POS);
#else
					mainStatus = MS_IDLE;
					ResetTCCoor(&CParam);
					ResetTCCoor(&ZParam);
					ResetTCCoor(&C_DriverParam);
					ResetTCCoor(&Z_DriverParam);
					
					MOTION_ZC_INIT_OK =True;
#endif						 
					break;
#if defined(MANUFACTURER_ADDTOP_EPSON)&&defined(EPSON_CLEAN_INTEGRATE_1)
				case IS_C_OK:
					mainStatus = MS_IDLE;										
					MOTION_ZC_INIT_OK =True;						 
					break;
#endif				
					
				case MS_NORMAL_MOVE:
					mainStatus = MS_IDLE;
					cmdbuf[0] = 2 + 4 * sizeof(INT32S);
					cmdbuf[1] = UART_DSP_RPT_STOP;
					*((__packed INT32S *)&cmdbuf[2]) = (INT32S)(curPos.x);  
					*((__packed INT32S *)&cmdbuf[6]) = (INT32S)(curPos.y); 
					*((__packed INT32S *)&cmdbuf[10]) = (INT32S)((Z_DriverParam.CoorIdeal - Z_DriverParam.Origin_Offset)/Z_DriverParam.ratio_move); 
					*((__packed INT32S *)&cmdbuf[14]) = (INT32S)((C_DriverParam.CoorIdeal - C_DriverParam.Origin_Offset)/C_DriverParam .ratio_move); 
					curPos.z_bak = (INT32S)((Z_DriverParam.CoorIdeal - Z_DriverParam.Origin_Offset)/Z_DriverParam.ratio_move);
					curPos.f_bak = (INT32S)((C_DriverParam.CoorIdeal - C_DriverParam.Origin_Offset)/C_DriverParam .ratio_move);
					UART_MotionReportCMD(cmdbuf);
					break;
				}
			}
		}
		
		while(UART_MotionGetCMD(cmdbuf))
		{
			switch(cmdbuf[1])
			{
				
			case UART_MOVEDISTANCE_CMD:
				MoveDistance(cmdbuf[2], cmdbuf[3], *(__packed INT32S *)(&cmdbuf[4])); 
				break;
			case UART_AXISMOVETO_CMD:
				AxisMoveTo(cmdbuf[2], cmdbuf[3], *(__packed INT32S *)(&cmdbuf[4]));
				break;
			case UART_STOPMOVE_CMD:					
				StopMove();
				break;
			case UART_GOHOME_CMD:
				AxisMoveTo(cmdbuf[2], TATE_CLEAN_CZ_SPEED_LEVEL, 0);
				break;
				
			}
			if(reportErr != SciError_OK)
				status_ReportStatus(STATUS_FTA + reportErr, STATUS_SET);
		}
	}
}

#endif

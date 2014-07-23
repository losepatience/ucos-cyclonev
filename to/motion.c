#include "os_includes.h"
#include "global.h"
#include "consol.h"
#include "status.h"
#include "keylcd.h"
#include "miscellance.h"
#include "FPGA.h"
#include "HugeBuf.h"

//#include "pio/pio.h"
//#include "pio/pio_it.h"
//#include "tc/tc.h"
//#include "pmc/pmc.h"
//#include "irq/irq.h"
#include "clean.h"

#include "motion.h"
#include "CommPipe.h"
#include "usb.h"

//#define RECORD_MOTION_MOVEMENT
//#define VM_PRINT
#define X_PRINT_ACC_SAFE_DISTANCE   (180)
#if defined(FPGA_MAINTAIN_COOR) || defined(SUPPORT_MOTOR_CONTROL)
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
#endif
#ifdef RECORD_MOTION_MOVEMENT
void PutMoveRecord(INT8U dir, INT8U SegType, INT8U MoveType, INT8U IsX, INT8U IsFeedBack, 
				   INT16S out, INT32S target_coor, INT32S real_coor);
static INT8U bAllowRecord = 0; //1, record X; 2 record Y; 3 record ALL
static INT8U bAllowRecord_Last = 0;
static INT8U bRecordType = 0; //0, auto loop; 1, lastest movement.
#define BEGIN_RECORD    StartMotionRecord(0, 3)
//#define BEGIN_RECORD
#else
#define PutMoveRecord(dir, SegType, MoveType, IsX, IsFeedBack, out, target_coor, real_coor)
#define BEGIN_RECORD
#endif

#ifdef STEPPER_8MICROSTEP
#ifdef MANUFACTURER_BEMAJET
#define STEPPER_ROTIO_SCALE 1
#elif defined(MANUFACTURER_GZ)
#define STEPPER_ROTIO_SCALE 6
#else
#define STEPPER_ROTIO_SCALE 8
#endif
#else
#define STEPPER_ROTIO_SCALE 1
#endif 

#if defined (EPSON_BOTTOM_BOARD_V3)&&((defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)) && !defined(COLORFUL_EPSON))
#error "V3 no support EPSON_CLEAN_INTEGRATE_1 and EPSON_CLEAN_INTEGRATE_2 "
#endif
#define MEASURE_ERROR_OFFSET (INT32S)(((float)(2/25.4))*720)
#ifdef SUPPORT_MOTOR_CONTROL 
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
//#define Ricoh_Test_Motion
//#define TATE_BEMAJET
//#define BEMAJET_2H
//#define ALLWIN_E180
#if defined(W1615EX) ||  defined(W1815EX)
#define OLD_Y_SPEED_SELECT_WAY //为了兼容老的算法。刁测试W1615EX时用的是老程序。
#endif

struct EPR_MOTION_PARAM motionParam SLOW_DATA_SECTION;
static struct EPR_MOTION_PARAM motionParam_default = 
#if defined (EPSON_BOTTOM_BOARD_V3)|| defined (EPSON_BOTTOM_BOARD_V2_1) || defined (EPSON_BOTTOM_BOARD_V2)
{  
	MOTION_PARAM_VERSION, sizeof(struct EPR_MOTION_PARAM), 4,
	{
		{//for X 
			MOTION_AXIS_PARAM_VERSION, sizeof(struct EPR_MOTION_AXIS_PARAM), 
			AXIS_ID_X, MT_DC, CT_CLOSE_LOOP_MOTOR, DT_PWM_Speed, SO_FIXED_REGION,
#if defined(WJ1545) || defined(WJ1345)            
5.0022f, //for micolor Testing machine at company.
#elif defined(S2628EX)
4.417f, 
#elif defined(R600)
4.942f,
#elif defined(LARGE_HB_MOTION_COMMON) 
4.751f,   
#elif defined(TATE_BEMAJET)||defined(BEMAJET_2H)
4.755f,             
#elif defined(A1802)
1.3333f,     
#elif defined(ALLWIN_E180)||defined(Ricoh_Test_Motion)
7.057f,               
#elif defined(DCM50202) || defined(DCM50202_W1615) 
1.990f,
#elif defined(COLORFUL_EPSON)
7.051f,
#else//for T1828EX/W1615EX/SJ1645/S1828EX
5.0827, //for micolor new machine with 皮带轮.
#endif
//2.0089f, //for roland machine.
{1.0f, (540.0f/720.0f), 2.0f}, 
{   {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}    },
{
#if defined(R600)
	{46.0f,0.0002f,0.0f}, //for standby
#elif defined(S2628EX)
{76.0f,0.01f,0.0f}, //for standby
#elif defined(LARGE_HB_MOTION_COMMON) || defined(A1802) || defined(TATE_BEMAJET)  ||defined(BEMAJET_2H)|| defined(ALLWIN_E180)|| defined(Ricoh_Test_Motion) 
{10.0f,0.0001f,0.0f}, //for standby                
#elif defined(DCM50202)
	{72.0f,0.002f,0.0f}, //for standby
#elif defined(DCM50202_W1615)
	{96.0f,0.01f,0.0f}, //for standby
#else
{96.0f,0.01f,0.0f}, //for standby
#endif
#if defined(W1615EX) || defined(W1815EX) || defined(S1828EX) || defined(DMN37HE)
{96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, //for 720
{90.0f,0.002f,0.0f}, {90.0f,0.002f,0.0f}, {90.0f,0.002f,0.0f}, {90.0f,0.002f,0.0f}, {90.0f,0.002f,0.0f}, //for 360
#elif defined(S2628EX)
{60.0f,0.002f,0.0f}, {50.0f,0.002f,0.0f}, {80.0f,0.002f,0.0f}, {80.0f,0.002f,0.0f}, {80.0f,0.002f,0.0f}, //for 720
{86.0f,0.002f,0.0f}, {86.0f,0.002f,0.0f}, {100.0f,0.01f,0.0f}, {86.0f,0.002f,0.0f}, {60.0f,0.002f,0.0f}, //for 360
#elif defined(T1828EX)
{50.0f,0.002f,0.0f}, {40.0f,0.002f,0.0f}, {80.0f,0.002f,0.0f}, {50.0f,0.002f,0.0f}, {50.0f,0.002f,0.0f}, //for 720
{96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.01f,0.0f}, {96.0f,0.002f,0.0f}, {70.0f,0.002f,0.0f}, //for 360
#elif defined(TATE_BEMAJET)||defined(BEMAJET_2H)
{25.0f,0.001f,0.0f}, {25.0f,0.001f,0.0f}, {30.0f,0.001f,0.0f}, {30.0f,0.001f,0.0f}, {25.0f,0.001f,30.0f}, //for 720
{35.0f,0.01f,0.0f}, {30.0f,0.01f,0.0f}, {35.0f,0.01f,0.0f}, {35.0f,0.01f,0.0f}, {30.0f,0.01f,30.0f}, //for 360    
#elif defined(LARGE_HB_MOTION_COMMON) || defined(ALLWIN_E180) || defined(Ricoh_Test_Motion)
{25.0f,0.001f,0.0f}, {25.0f,0.001f,0.0f}, {30.0f,0.001f,0.0f}, {30.0f,0.001f,0.0f}, {25.0f,0.001f,0.0f}, //for 720
{35.0f,0.01f,0.0f}, {30.0f,0.01f,0.0f}, {35.0f,0.01f,0.0f}, {35.0f,0.01f,0.0f}, {30.0f,0.01f,0.0f}, //for 360                
#elif defined(A1802)
{20.0f,0.0005f,30.0f}, {25.0f,0.001f,30.0f}, {8.0f,0.005f,30.0f}, {8.0f,0.005f,30.0f}, {5.0f,0.002f,30.0f}, //for 720
{20.0f,0.001f,30.0f}, {30.0f,0.002f,30.0f}, {30.0f,0.002f,30.0f}, {30.0f,0.002f,30.0f}, {20.0f,0.001f,30.0f}, //for 360                    
#elif defined(R600)
{20.0f,0.0005f,30.0f}, {25.0f,0.001f,30.0f}, {20.0f,0.0005f,30.0f}, {20.0f,0.0005f,30.0f}, {66.0f,0.001f,20.0f}, //for 720
{20.0f,0.001f,30.0f}, {30.0f,0.002f,30.0f}, {30.0f,0.002f,30.0f}, {30.0f,0.002f,30.0f}, {66.0f,0.001f,20.0f}, //for 360                  
#elif defined(DCM50202)
	{72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}, //for 720
	{72.0f,0.002f,0.0f}, {36.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}, //for 360
#elif defined(DCM50202_W1615)
	{96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, //for 720
	{90.0f,0.002f,0.0f}, {90.0f,0.002f,0.0f}, {90.0f,0.002f,0.0f}, {90.0f,0.002f,0.0f}, {90.0f,0.002f,0.0f}, //for 360
#else
{96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, //for 720
{96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, //for 360
#endif                
{96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, //for 540
{96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, //for 270
{96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}, //for 1440
#if defined(WJ1545) || defined(WJ1345)            
{24.0f,0.00004f,0.0f}, {48.0f,0.002f,0.0f}, {48.0f,0.002f,0.0f}  //for only move, for wj1545
#elif defined(W1615EX) || defined(W1815EX) || defined(SJ1645) 
{36.0f,0.002f,0.0f}, {48.0f,0.002f,0.0f}, {48.0f,0.002f,0.0f}  //for only move, for T1828EX
#elif defined(S2628EX)
{36.0f,0.002f,0.0f}, {38.0f,0.002f,0.0f}, {40.0f,0.002f,0.0f}  //for only move, for S2628EX
#elif defined(S1828EX)
{36.0f,0.002f,0.0f}, {60.0f,0.002f,0.0f}, {60.0f,0.002f,0.0f}  //for only move, for S1828EX
#elif defined(T1828EX)
{42.0f,0.002f,0.0f}, {60.0f,0.01f,0.0f}, {60.0f,0.01f,0.0f}  //for only move, for T1828EX
#elif defined(R600)
{36.0f,0.00004f,0.0f}, {42.0f,0.0005f,20.0f}, {42.0f,0.0005f,0.0f}  //for only move, for R600   
#elif defined(TATE_BEMAJET)||defined(BEMAJET_2H)
{10.0f,0.0005f,10.0f}, {30.0f,0.0015f,30.0f}, {30.0f,0.002f,30.0f}  //for only move, for R600    
#elif defined(LARGE_HB_MOTION_COMMON) || defined(ALLWIN_E180) || defined(Ricoh_Test_Motion)
{5.0f,0.0001f,0.0f}, {30.0f,0.0015f,0.0f}, {30.0f,0.002f,0.0f}  //for only move, for R600    
#elif defined(A1802)
{10.0f,0.0001f,30.0f}, {15.0f,0.0001f,30.0f}, {30.0f,0.002f,0.0f}  //for only move, for R600                     
#elif defined(DCM50202)
	{72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}  //for only move, for R600     
#elif defined(DCM50202_W1615)
	{72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}, {72.0f,0.002f,0.0f}  //for only move, for R600     
#elif defined(DMN37HE)
	{36.0f,0.002f,0.0f}, {48.0f,0.002f,0.0f}, {96.0f,0.002f,0.0f}  //for only move, for T1828EX
#else //for T1828EX
{36.0f,0.00004f,0.0f}, {60.0f,0.01f,0.0f}, {60.0f,0.01f,0.0f}  //for only move, for T1828EX
#endif
//for roland machine.
//{160.0f,0.12f,0.0f}, {160.0f,0.12f,0.0f}, {160.0f,0.12f,0.0f}, {160.0f,0.12f,0.0f}, {160.0f,0.12f,0.0f}, //for 720
//{180.0f,0.15f,0.0f}, {160.0f,0.12f,0.0f}, {180.0f,0.15f,0.0f}, {180.0f,0.15f,0.0f}, {180.0f,0.15f,0.0f}, //for 360
//{170.0f,0.12f,0.0f}, {170.0f,0.12f,0.0f}, {170.0f,0.12f,0.0f}, {170.0f,0.12f,0.0f}, {170.0f,0.12f,0.0f}, //for 540
//{180.0f,0.15f,0.0f}, {180.0f,0.15f,0.0f}, {180.0f,0.15f,0.0f}, {180.0f,0.15f,0.0f}, {180.0f,0.15f,0.0f}, //for 270
//{160.0f,0.12f,0.0f}, {160.0f,0.12f,0.0f}, {160.0f,0.12f,0.0f}, {160.0f,0.12f,0.0f}, {160.0f,0.12f,0.0f}, //for 1440
//{24.0f,0.00004f,0.0f}, {50.0f,0.01f,0.0f}, {160.0f,0.12f,0.0f}  //for only move
//{24.0f,0.001f,0.0f}, {24.0f,0.001f,0.0f}, {24.0f,0.001f,0.0f}  //for only move
},
//for Micolor WJ1554
#if defined(WJ1545) || defined(WJ1345) || defined(R600)//OriginOffset
-128,    //origin sensor offset, about 10.6mm, 
#elif defined(SJ1645) || defined(S2628EX)
-70, //for 160cm weak solvent
#elif defined(TATE_BEMAJET)||defined(BEMAJET_2H)
-20, //for 160cm weak solvent            
#elif defined(DCM50202) || defined(DCM50202_W1615)
	-420,
#else //for T1828EX W1615EX W1815EX S1828EX
-40,    //origin sensor offset, for T1828EX
#endif
0,    //start point (for micolor, it is cleaning point), about 4mm
#if defined(WJ1545) || defined(WJ1345)|| defined(R600)//Fixed_MoveRegion_EndPoint
53614,  //end point, about 135.5cm. for WJ1545
#elif defined(SJ1645)
55040, //for 160cm machine
#elif defined(W1615EX)            
64018,
#elif defined(W1815EX)            
72663,
#elif defined(S1828EX)            
71000,
#elif defined(S2628EX)
88786,
#elif defined(A1802)
67055,            
#elif defined(BEMAJET_2H)||defined(TATE_BEMAJET)
103189,//FOR 320CM
#elif defined(DCM50202) || defined(DCM50202_W1615)
	16900,
#else //for T1828EX
56014,    //for T1828EX
#endif
{   0, //start point (for micolor, it is cleaning point), about 4mm
//for Micolor WJ1554
#if defined(WJ1545) || defined(WJ1345)|| defined(R600)  //OriginOffset
-128,    //origin sensor offset, about 10.6mm, 
#elif defined(SJ1645) || defined(S2628EX)
-70, //for 160cm weak solvent
#elif defined(TATE_BEMAJET)||defined(BEMAJET_2H)
-20,                  
#elif defined(DCM50202) || defined(DCM50202_W1615)
	-420,
#else //for T1828EX W1615EX W1815EX S1828EX
-40,    //origin sensor offset, for T1828EX
#endif
#if defined(WJ1545) || defined(WJ1345) || defined(A1802)//print  start point
5700,  //print region's start point, about 19.3cm.for WJ1545
#elif defined(SJ1645)
5120,   //for 160cm machine.
#elif defined(W1615EX) ||defined(W1815EX)                
9570,
#elif defined(S2628EX)
8800,    
#elif defined(R600)
5900,
#elif defined(TATE_BEMAJET)||defined(BEMAJET_2H)
4460,                  
#elif defined(DCM50202) || defined(DCM50202_W1615)
	3700,
#elif defined(DMN37HE)
	4800,
#else //for T1828EX S1828EX
8080,    //for T1828EX
#endif
#if defined(WJ1545) || defined(WJ1345)|| defined(R600)//print end point
48786, //print region's end point, about 130cm
#elif defined(SJ1645)
50474, //160cm machine.
#elif defined(W1615EX)                
56000,
#elif defined(W1815EX)                
69000,
#elif defined(S1828EX)                
68000,
#elif defined(S2628EX) 
83614,   
#elif defined(BEMAJET_2H)||defined(TATE_BEMAJET)
100189,//FOR 320CM
#elif defined(A1802) 
61360,                 
#elif defined(DCM50202) || defined(DCM50202_W1615)	
	13200,
#elif defined(DMN37HE)
	55000,
#else //for T1828EX
8080 + 74*720,    //for T1828EX
#endif
#if defined(WJ1545) || defined(WJ1345)|| defined(R600)//move end point
53614,  //end point, about 135.5cm. for WJ1545
#elif defined(SJ1645)
55040, //for 160cm machine
#elif defined(W1615EX)                
64018,
#elif defined(W1815EX)            
72663,  
#elif defined(S1828EX)            
71000,
#elif defined(S2628EX)      
88786,
#elif defined(A1802)
67055,                 
#elif defined(BEMAJET_2H)||defined(TATE_BEMAJET)
103189,//FOR 320CM
#elif defined(DCM50202) || defined(DCM50202_W1615)
	16900,
#elif defined(DMN37HE)
	60600,
#else //for T1828EX
56014,    //for T1828EX
#endif
#ifdef A1802
#ifdef A1802_1H
3260,// 1head
#else
1786,// 2head
#endif
#else
1300, //measure sensor offset
#endif
#if defined(WJ1545) || defined(WJ1345)|| defined(R600)//end point of cut sheet movement
53614,  //end point, about 135.5cm. for WJ1545
#elif defined(SJ1645)
55040, //for 160cm machine
#elif defined(W1615EX)                
64018,
#elif defined(W1815EX)                
72663,
#elif defined(S1828EX)            
71000,                
#elif defined(S2628EX)
88786, 
#elif defined(A1802)
67055,                 
#elif defined(BEMAJET_2H)||defined(TATE_BEMAJET)
101189,//FOR 320CM
#else //for T1828EX
56014,    //for T1828EX
#endif
300 // start point of cut sheet movement.
},
//for Micolor WJ1304 testing
//            -300,    //origin sensor offset, about 10.6mm
//            -115,    //start point (for micolor, it is cleaning point), about 4mm
//            38400,  //end point, about 135.5cm.
//            {   -115, //start point (for micolor, it is cleaning point), about 4mm
//                -300, //origin sensor offset, about 10.6mm
//                5100,  //print region's start point, about 18cm
//                36850, //print region's end point, about 130cm
//                38350, //end point, about 135.5cm.
//                1130, //measure sensor offset
//                38300, // end point of cut sheet movement. about 135cm.
//                130 // start point of cut sheet movement.
//            },

0, {PWMTYPE_BI_POLAR, PWMTYPE_BI_POLAR, PWMTYPE_UNI_POLAR}, 
//0, {PWMTYPE_BI_POLAR, PWMTYPE_BI_POLAR}, 
#ifdef A1802
0xE1, 0xFF9B, //反转X轴原点信号
#else
	#if defined(INKWIN_SMALL_FLAT_MACHINE)
		0xE1, 0xFF9F, 
	#else
		0xE1, 0xFF9A, 
	#endif
#endif
//{   4*32+7, 0xFF, 4*32+8, 0xFF, 0xFF, 1*32+12, 1*32+5, 5*32+11, 0},
#ifdef EPSON_BOTTOM_BOARD_V2_1
{	4*32+7, 0xFF, 0xFF, 0xFF, 0xFF, 3*32+26, 3*32+27, 4*32+17, 0}, 	//V2_1
#else
{	4*32+7, 0xFF, 0xFF, 0xFF, 0xFF, 3*32+26, 3*32+27, 5*32+11, 0},	//V3
#endif
//0xFF, {3*32+24, 3*32+25, 3*32+26, 0},
0xFB, {3*32+24, 3*32+25, 1*32+13, 0},
MAX_X_SPEED_LEVEL, 
{   
#if defined(S2628EX)   
	{AT_SCurve, 400, 14367, 250000, 50000, (14367-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 69.6us, 14367Hz
	{AT_SCurve, 400, 8620,  200000, 40000, (8620-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 116us, 8620Hz
	{AT_SCurve, 400, 15847, 250000, 50000, (15847-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, 15847Hz
	{AT_SCurve, 400, 13888, 250000, 50000, (13888-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 72us, 13888Hz
	{AT_SCurve, 400, 28346, 400000, 80000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#elif defined(S1828EX)
{AT_SCurve, 400, 14367, 200000, 40000, (14367-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620,  150000, 30000, (8620-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15847, 200000, 40000, (15847-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888, 200000, 40000, (13888-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 28346, 400000, 80000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#elif defined(W1615EX) || defined(W1815EX) || defined(LARGE_HB_MOTION_COMMON) || defined(ALLWIN_E180) ||defined(A1802)     
{AT_SCurve, 400, 14367, 210000, 42000, (14367-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620,  150000, 30000, (8620-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15000, 250000, 50000, (15847-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888, 250000, 50000, (13888-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 28346, 400000, 80000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#elif defined(TATE_BEMAJET)||defined(BEMAJET_2H)
{AT_SCurve, 400, 14367, 210000, 42000, (14367-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620,  150000, 30000, (8620-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 116us, 8620Hz
#ifdef HEAD_EPSON_GEN7
{AT_SCurve, 400, 14800, 250000, 50000, (14800-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, change for BEMAJET
#else
{AT_SCurve, 400, 15000, 250000, 50000, (15000-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, change for BEMAJET
#endif        
{AT_SCurve, 400, 13888, 250000, 50000, (13888-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 28346, 400000, 80000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.                
#elif defined(T1828EX)
{AT_SCurve, 400, 14367, 250000, 50000, (14367-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620,  200000, 40000, (8620-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15847, 250000, 50000, (15847-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888, 250000, 50000, (13888-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 28346, 400000, 80000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#elif defined(Ricoh_Test_Motion)
{AT_SCurve, 400, 14000, 250000, 50000, (14000-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 71.43us, 14367Hz
{AT_SCurve, 400, 2461,  200000, 40000, (2461-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 406.4us, 8620Hz
{AT_SCurve, 400, 23998, 250000, 50000, (23998-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3,41.67us, 15847Hz
{AT_SCurve, 400, 20000, 250000, 50000, (20000-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 50us, 13888Hz
{AT_SCurve, 400, 28346, 400000, 80000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#elif defined(DCM50202)
	{AT_SCurve, 400, 14367, 210000, 52000, 1596, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 69.6us, 14367Hz
	{AT_SCurve, 400, 8620,  200000, 60000, 1222,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 116us, 8620Hz
	{AT_SCurve, 400, 15847, 250000, 70000, 1944, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, 15847Hz
	{AT_SCurve, 400, 13888, 250000, 50000, 1748, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 72us, 13888Hz
	{AT_SCurve, 400, 20000, 500000, 100000, 3194, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#elif defined(DCM50202_W1615)
#error
	{AT_SCurve, 400, 14367, 210000, 42000, (14367-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 69.6us, 14367Hz
	{AT_SCurve, 400, 8620,  150000, 30000, (8620-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 116us, 8620Hz
	{AT_SCurve, 400, 15847, 250000, 50000, (15847-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, 15847Hz
	{AT_SCurve, 400, 13888, 250000, 50000, (13888-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 72us, 13888Hz
	{AT_SCurve, 400, 20000, 500000, 100000, 3194, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#elif defined(DMN37HE)
	{AT_SCurve, 400, 14367, 210000, 42000, (14367-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 69.6us, 14367Hz
	{AT_SCurve, 400, 8620,  150000, 30000, (8620-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 116us, 8620Hz
	{AT_SCurve, 400, 15000, 150000, 30000, (15847-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, 15847Hz
	{AT_SCurve, 400, 13888, 200000, 40000, (13888-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 72us, 13888Hz
	{AT_SCurve, 400, 28346, 400000, 80000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#else
{AT_SCurve, 400, 14367, 150000, 30000, (14367-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620,  100000, 20000, (8620-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15847, 150000, 30000, (15847-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888, 150000, 30000, (13888-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 28346, 400000, 80000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#endif 
#if defined(S2628EX)  
{AT_SCurve, 400, 14367*2, 300000, 60000, (14367*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620*2,  250000, 50000, (8620*2-400)/10+400,  0, 0.5f, 0.5f}, //for 360DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15000*2, 300000, 60000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888*2, 300000, 60000, (13888*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 15000*2, 400000, 180000, (15000*2-400)*4/10+400, 0, 0.5f, 0.5f}, //for 360DPI return. about 1.05m/s.
#elif defined(S1828EX)
{AT_SCurve, 400, 14367*2, 250000, 50000, (14367*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620*2,  200000, 40000, (8620*2-400)/10+400,  0, 0.5f, 0.5f}, //for 360DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15000*2, 250000, 50000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888*2, 250000, 50000, (13888*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 15000*2, 400000, 80000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI return. about 1.05m/s.
#elif defined(W1615EX) || defined(W1815EX) || defined(LARGE_HB_MOTION_COMMON) || defined(A1802) || defined(ALLWIN_E180)  
{AT_SCurve, 400, 14367*2, 300000, 60000, (14367*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620*2,  230000, 46000, (8620*2-400)/10+400,  0, 0.5f, 0.5f}, //for 360DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15000*2, 300000, 60000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888*2, 300000, 60000, (13888*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 15000*2, 400000, 80000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI return. about 1.05m/s.
#elif defined(TATE_BEMAJET)||defined(BEMAJET_2H)
{AT_SCurve, 400, 14367*2, 250000, 50000, (14367*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620*2,  230000, 46000, (8620*2-400)/10+400,  0, 0.5f, 0.5f}, //for 360DPI VSD2, 116us, 8620Hz
#ifdef HEAD_EPSON_GEN7
{AT_SCurve, 400, 14800*2, 250000, 50000, (14800*2-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, change for BEMAJET
#else
{AT_SCurve, 400, 15000*2, 250000, 50000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3, 63.1us, change for BEMAJET
#endif                
{AT_SCurve, 400, 13888*2, 250000, 50000, (13888*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 15000*2, 400000, 80000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI return. about 1.05m/s.                
#elif defined(T1828EX)
{AT_SCurve, 400, 14367*2, 300000, 60000, (14367*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620*2,  250000, 50000, (8620*2-400)/10+400,  0, 0.5f, 0.5f}, //for 360DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15000*2, 300000, 60000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888*2, 300000, 60000, (13888*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 15000*2, 400000, 80000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI return. about 1.05m/s.
#elif defined(Ricoh_Test_Motion)
{AT_SCurve, 400, 14000*2, 250000, 50000, (14000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD1, 71.43us, 14367Hz
{AT_SCurve, 400, 2461*2,  200000, 40000, (2461*2-400)/10+400,  0, 0.5f, 0.5f}, //for 720DPI VSD2, 406.4us, 8620Hz
{AT_SCurve, 400, 23998*2, 250000, 50000, (23998*2-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD3,41.67us, 15847Hz
{AT_SCurve, 400, 20000*2, 250000, 50000, (20000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI VSD4, 50us, 13888Hz
{AT_SCurve, 400, 20000*2, 400000, 80000, (20000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI return. 1m/s.
#elif defined(DCM50202)
	{AT_SCurve, 400, 20000, 300000, 52000, 2084, 0, 0.5f, 0.5f}, //for 360DPI VSD1, 69.6us, 14367Hz
	{AT_SCurve, 400, 17240,  300000, 60000, 2084,  0, 0.5f, 0.5f}, //for 360DPI VSD2, 116us, 8620Hz
	{AT_SCurve, 200, 25000, 350000, 80000, 1222, 0, 0.5f, 0.5f}, //for 360DPI VSD3, 63.1us, 15847Hz
	{AT_SCurve, 400, 22000, 300000, 60000, 1222, 0, 0.5f, 0.5f}, //for 360DPI VSD4, 72us, 13888Hz
	{AT_SCurve, 200, 20000, 350000, 100000, 3360, 0, 0.5f, 0.5f}, //for 360DPI return. about 1.05m/s.
#elif defined(DCM50202_W1615)
	{AT_SCurve, 400, 14367*2, 300000, 60000, (14367*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD1, 69.6us, 14367Hz
	{AT_SCurve, 400, 8620*2,  230000, 46000, (8620*2-400)/10+400,  0, 0.5f, 0.5f}, //for 360DPI VSD2, 116us, 8620Hz
	{AT_SCurve, 400, 15000*2, 300000, 60000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD3, 63.1us, 15847Hz
	{AT_SCurve, 400, 13888*2, 300000, 60000, (13888*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD4, 72us, 13888Hz
	{AT_SCurve, 400, 15000*2, 400000, 80000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI return. about 1.05m/s.
#elif defined(DMN37HE)
	{AT_SCurve, 400, 14367*2, 300000, 70000, (14367*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD1, 69.6us, 14367Hz
	{AT_SCurve, 400, 8620*2,  200000, 40000, (8620*2-400)/10+400,  0, 0.5f, 0.5f}, //for 360DPI VSD2, 116us, 8620Hz
	{AT_SCurve, 400, 15000*2, 300000, 80000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD3, 63.1us, 15847Hz
	{AT_SCurve, 400, 13888*2, 300000, 60000, (13888*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD4, 72us, 13888Hz
	{AT_SCurve, 400, 15000*2, 400000, 80000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI return. about 1.05m/s.
#else
{AT_SCurve, 400, 14367*2, 190000, 40000, (14367*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620*2,  150000, 30000, (8620*2-400)/10+400,  0, 0.5f, 0.5f}, //for 360DPI VSD2, 116us, 8620Hz
//{AT_SCurve, 400, 15847*2, 300000, 60000, (15847*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD3, 63.1us, 15847Hz
//cheney: IMPORTANT, Micolor Current carriage max speed is only 30000Khz for 720DPI.
{AT_SCurve, 400, 15000*2, 190000, 40000, (15000*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888*2, 190000, 40000, (13888*2-400)/10+400, 0, 0.5f, 0.5f}, //for 360DPI VSD4, 72us, 13888Hz
//cheney: IMPORTANT, Current carriage max speed is only 30000Khz for 720DPI.
//  20110628: when print angle check, take place X paper jap, so decease speed.
{AT_SCurve, 400, 15000*2, 400000, 180000, (15000*2-400)*4/10+400, 0, 0.5f, 0.5f}, //for 360DPI return. about 1.05m/s.
#endif 
{AT_SCurve, 400, 14367*720/540, 190000, 40000, (14367*720/540-400)/10+400, 0, 0.5f, 0.5f}, //for 540DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620*720/540,  150000, 30000, (8620*720/540-400)/10+400,  0, 0.5f, 0.5f}, //for 540DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15847*720/540, 190000, 40000, (15847*720/540-400)/10+400, 0, 0.5f, 0.5f}, //for 540DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888*720/540, 190000, 40000, (13888*720/540-400)/10+400, 0, 0.5f, 0.5f}, //for 540DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 28346, 200000, 40000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 540DPI return. 1m/s.

//{AT_SCurve, 400, 14367*720/540*2, 400000, 80000, (14367*720/540*2-400)/10+400, 0, 0.5f, 0.5f}, //for 270DPI VSD1, 69.6us, 14367Hz
//cheney: IMPORTANT, Current carriage max speed is only 30000Khz for 720DPI.
{AT_SCurve, 400, 11250*720/540*2, 190000, 40000, (11250*720/540*2-400)/10+400, 0, 0.5f, 0.5f}, //for 270DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620*720/540*2,  190000, 40000, (8620*720/540*2-400)/10+400,  0, 0.5f, 0.5f}, //for 270DPI VSD2, 116us, 8620Hz
//{AT_SCurve, 400, 15847*720/540*2, 400000, 80000, (15847*720/540*2-400)/10+400, 0, 0.5f, 0.5f}, //for 270DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 11250*720/540*2, 190000, 40000, (11250*720/540*2-400)/10+400, 0, 0.5f, 0.5f}, //for 270DPI VSD3, 63.1us, 15847Hz
//{AT_SCurve, 400, 13888*720/540*2, 400000, 80000, (13888*720/540*2-400)/10+400, 0, 0.5f, 0.5f}, //for 270DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 11250*720/540*2, 190000, 40000, (11250*720/540*2-400)/10+400, 0, 0.5f, 0.5f}, //for 270DPI VSD4, 72us, 13888Hz
//{AT_SCurve, 400, 15847*720/540*2, 400000, 80000, (15847*720/540*2-400)/10+400, 0, 0.5f, 0.5f}, //for 270DPI return. about 1.49m/s.
{AT_SCurve, 400, 11250*720/540*2, 190000, 40000, (11250*720/540*2-400)/10+400, 0, 0.5f, 0.5f}, //for 270DPI return. about 1.49m/s.

{AT_SCurve, 400, 14367/2, 150000, 30000, (14367/2-400)/10+400, 0, 0.5f, 0.5f}, //for 1440DPI VSD1, 69.6us, 14367Hz
{AT_SCurve, 400, 8620/2,  150000, 30000, (8620/2-400)/10+400,  0, 0.5f, 0.5f}, //for 1440DPI VSD2, 116us, 8620Hz
{AT_SCurve, 400, 15847/2, 150000, 30000, (15847/2-400)/10+400, 0, 0.5f, 0.5f}, //for 1440DPI VSD3, 63.1us, 15847Hz
{AT_SCurve, 400, 13888/2, 150000, 30000, (13888/2-400)/10+400, 0, 0.5f, 0.5f}, //for 1440DPI VSD4, 72us, 13888Hz
{AT_SCurve, 400, 28346, 200000, 40000, (28346-400)/10+400, 0, 0.5f, 0.5f}, //for 1440DPI return. 1m/s.

//{AT_Trapezoid, 400, 28346/10, 300000, 0}, //0.1ms
//for micolor machine, the distance from the position found origin sensor to the max permitted position to right is 600.
//but the acc distance of {AT_SCurve, 400, 28346/10, 100000, 20000, (28346/10-400)/10+400, 0, 0.5f, 0.5f} is 800.
//so . i change it to, whose acc distance is 227.
#if defined(S2628EX)  
{AT_SCurve, 400, 28346/20, 100000, 45000, (28346/20-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI slow movement. 0.05m/s.
//Micolor hope that goto home is very smooth. I change S curve to match it.
{AT_SCurve, 400, 28346/4, 1000000, 450000, (28346/4-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI measure movement. 0.25m/s.
{AT_SCurve, 400, 28346/4, 100000, 20000, (28346/4-400)/10+400, 0, 0.5f, 0.5f} //for 720DPI measure movement. 0.25m/s
#elif defined(S1828EX)
{AT_SCurve, 400, 28346/20, 50000, 10000, (28346/20-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI slow movement. 0.05m/s.
//Micolor hope that goto home is very smooth. I change S curve to match it.
{AT_SCurve, 400, 28346/4, 1000000, 450000, (28346/4-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI measure movement. 0.25m/s.
{AT_SCurve, 400, 28346/4, 100000, 20000, (28346/4-400)/10+400, 0, 0.5f, 0.5f} //for 720DPI measure movement. 0.25m/s
#elif defined(W1615EX) || defined(W1815EX) || defined(LARGE_HB_MOTION_COMMON) || defined(ALLWIN_E180) || defined(Ricoh_Test_Motion)
{AT_SCurve, 400, 1500, 40000, 8000, (1500-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI slow movement. 0.05m/s.
//Micolor hope that goto home is very smooth. I change S curve to match it.
{AT_SCurve, 400, 28346/4, 1000000, 450000, (28346/4-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI measure movement. 0.25m/s.
{AT_SCurve, 400, 28346/4, 100000, 20000, (28346/4-400)/10+400, 0, 0.5f, 0.5f} //for 720DPI measure movement. 0.25m/s
#elif defined(A1802)|| defined(TATE_BEMAJET)||defined(BEMAJET_2H)    
{AT_SCurve, 400, 2500, 50000, 10000, (2500-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI slow movement. 0.05m/s.
{AT_SCurve, 400, 7086, 1000000, 450000, 3068, 0, 0.5f, 0.5f}, //for 720DPI measure movement. 0.25m/s.
{AT_SCurve, 400, 7086, 100000, 20000, (7086-400)/10+400, 0, 0.5f, 0.5f} //for 720DPI measure movement. 0.25m/s                
#elif defined(DCM50202)
	{AT_SCurve, 400, 1500, 40000, 8000, 510, 0, 0.5f, 0.5f}, //for 720DPI slow movement. 0.05m/s.
	{AT_SCurve, 200, 7086, 200000, 50000, 1068, 0, 0.5f, 0.5f}, //for 720DPI measure movement. 0.25m/s.
	{AT_SCurve, 400, 7086, 100000, 20000, 1068, 0, 0.5f, 0.5f} //for 720DPI measure movement. 0.25m/s   
#elif defined(DCM50202_W1615)
	{AT_SCurve, 400, 1500, 40000, 8000, 510, 0, 0.5f, 0.5f}, //for 720DPI slow movement. 0.05m/s.
	{AT_SCurve, 200, 7086, 200000, 50000, 1068, 0, 0.5f, 0.5f}, //for 720DPI measure movement. 0.25m/s.
	{AT_SCurve, 400, 7086, 100000, 20000, 1068, 0, 0.5f, 0.5f} //for 720DPI measure movement. 0.25m/s   
#elif defined(DMN37HE)
	{AT_SCurve, 100, 1500, 100000, 30000, (1500-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI slow movement. 0.05m/s.
	//Micolor hope that goto home is very smooth. I change S curve to match it.
	{AT_SCurve, 300, 12000, 200000, 60000, 3000, 0, 0.5f, 0.5f}, //for 720DPI measure movement. 0.25m/s.
	{AT_SCurve, 300, 12000, 150000, 40000, 3000, 0, 0.5f, 0.5f} //for 720DPI measure movement. 0.25m/s
#else
{AT_SCurve, 400, 28346/20, 50000, 10000, (28346/20-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI slow movement. 0.05m/s.
	#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)
//Micolor hope that goto home is very smooth. I change S curve to match it.
{AT_SCurve, 400, 28346/4, 1000000, 400000, (28346/4-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI measure movement. 0.25m/s.
{AT_SCurve, 400, 28346/4, 100000, 20000, (28346/4-400)/10+400, 0, 0.5f, 0.5f} //for 720DPI measure movement. 0.25m/s
#else
{AT_SCurve, 400, 28346/2, 300000, 60000, (28346/2-400)/10+400, 0, 0.5f, 0.5f}, //for 720DPI measure movement. 0.5m/s.
{AT_SCurve, 400, 28346, 300000, 60000, (28346-400)/10+400, 0, 0.5f, 0.5f} //for 720DPI normal movement. 1m/s.
#endif
#endif
}
		},
		{//for Y
			MOTION_AXIS_PARAM_VERSION, sizeof(struct EPR_MOTION_AXIS_PARAM), 
			AXIS_ID_Y, MT_DC, CT_CLOSE_LOOP_MOTOR, DT_PWM_Speed_Position, 0,
			1.0f, {1.0f, 1.0f, 1.0f}, 
			{   {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}    },
			{   
#if defined(WJ1545) || defined(WJ1345) 
				{10.0f,0.01f,0.0f}, //for standby, for WJ1545
				{10.0f,0.002f,0.0f}, {20.0f,0.005f,0.0f}, {20.0f,0.005f,0.0f}, {20.0f,0.005f,0.0f}, {20.0f,0.005f,0.0f},
#elif defined(WJ1645) || defined(SJ1645)
{10.0f,0.01f,0.0f}, //for standby, for WJ1545
{10.0f,0.002f,0.0f}, {20.0f,0.005f,0.0f}, {20.0f,0.005f,0.0f}, {20.0f,0.005f,0.0f}, {20.0f,0.005f,0.0f},
#elif defined(W1615EX) || defined(W1815EX)
{10.0f,0.01f,0.0f}, //for standby, for W1615EX
{10.0f,0.0005f,0.0f}, {30.0f,0.008f,0.0f}, {35.0f,0.008f,0.0f}, {35.0f,0.008f,0.0f}, {38.0f,0.008f,0.0f},
#elif defined(S2628EX)
//                {20.0f,0.028f,0.0f}, //for standby, for S2628EX   //increase 30% parameter
//                {13.0f,0.008f,28.0f}, {38.0f,0.01f,32.0f}, {50.0f,0.011f,32.0f}, {57.0f,0.011f,32.0f}, {65.0f,0.007f,32.0f},
{12.0f,0.016f,0.0f}, //for standby, for S2628EX   //decease 25% parameter
{8.0f,0.005f,16.0f}, {22.0f,0.005f,19.0f}, {30.0f,0.006f,19.0f}, {34.0f,0.006f,19.0f}, {38.0f,0.004f,19.0f},
//                {15.0f,0.021f,0.0f}, //for standby, for S2628EX   //old parameter
//                {10.0f,0.006f,20.0f}, {30.0f,0.0075f,25.0f}, {40.0f,0.008f,25.0f}, {45.0f,0.008f,25.0f}, {50.0f,0.005f,25.0f},
#elif defined(S1828EX)
{10.0f,0.01f,0.0f}, //for standby, for S1828EX
{10.0f,0.001f,50.0f}, {30.0f,0.003f,50.0f}, {35.0f,0.005f,50.0f}, {40.0f,0.005f,50.0f}, {50.0f,0.005f,50.0f},
#elif defined(T1828EX)
{1.0f,0.001f,0.0f}, //for standby, for 2head machine.
{5.0f,0.002f,0.0f}, {10.0f,0.003f,0.0f}, {12.0f,0.004f,0.0f}, {15.0f,0.004f,0.0f}, {20.0f,0.005f,0.0f},
#elif defined(LARGE_HB_MOTION_COMMON) || defined(ALLWIN_E180) || defined(Ricoh_Test_Motion)
{5.0f,0.02f,20.0f}, //for standby, for 2head machine.
{7.0f,0.02f,30.0f}, {10.0f,0.03f,30.0f}, {10.0f,0.03f,30.0f}, {10.0f,0.03f,30.0f}, {10.0f,0.03f,30.0f},   
#elif defined(TATE_BEMAJET)||defined(BEMAJET_2H) 
{5.0f,0.02f,0.0f}, //for standby, for 2head machine.
{7.0f,0.02f,25.0f}, {10.0f,0.03f,25.0f}, {10.0f,0.03f,25.0f}, {10.0f,0.03f,25.0f}, {10.0f,0.03f,25.0f},    
#elif defined(A1802)
{5.0f,0.02f,0.0f}, //for standby, for 2head machine.
{7.0f,0.02f,30.0f}, {15.0f,0.05f,30.0f}, {15.0f,0.05f,30.0f}, {15.0f,0.05f,30.0f}, {15.0f,0.05f,30.0f},                   
#elif defined(DCM50202) || defined(DCM50202_W1615)
	{5.0f,0.00004f,0.0f}, //for standby, for 2head machine.
	{5.0f,0.00004f,0.0f}, {5.0f,0.00004f,0.0f}, {5.0f,0.00004f,0.0f}, {5.0f,0.00004f,0.0f}, {5.0f,0.00004f,0.0f}, 
#elif defined(DMN37HE)
	{5.0f,0.00200f,0.0f}, //for standby, for 2head machine.
	{5.0f,0.00200f,0.0f}, {5.0f,0.00200f,0.0f}, {5.0f,0.00200f,0.0f}, {5.0f,0.00200f,0.0f}, {5.0f,0.00200f,0.0f}, 
#else 
{10.0f,0.01f,0.0f}, //for standby, for 2head machine.
{10.0f,0.002f,0.0f}, {20.0f,0.005f,0.0f}, {20.0f,0.005f,0.0f}, {20.0f,0.005f,0.0f}, {30.0f,0.005f,0.0f},
#endif
0   
			},
			0xFFFFFFFF,   
			0xFFFFFFFF,    
			0xFFFFFFFF,  
			{   0xFFFFFFFF, 
			0xFFFFFFFF, 
			0xFFFFFFFF,  
			0xFFFFFFFF, 
			0xFFFFFFFF, 
			0xFFFFFFFF, 
			0xFFFFFFFF, 
			0xFFFFFFFF
			},
			1, {PWMTYPE_BI_POLAR, PWMTYPE_BI_POLAR, PWMTYPE_UNI_POLAR}, 
		#if defined(INKWIN_SMALL_FLAT_MACHINE)
			0x65, 0xFF9B, 
		#else
			0x60, 0xFF9F, 
		#endif
		#if defined(INKWIN_SMALL_FLAT_MACHINE)
			{4*32+9, 0xFF, 3*32+21, 0xFF, 0xFF, 3*32+26, 3*32+27, 0xFF, 0},
		#else
			{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 3*32+26, 3*32+27, 0xFF, 0},
		#endif
			//0xFF, {4*32+31, 0*32+25, 3*32+27, 0},
			0xFB, {4*32+31, 0*32+25, 3*32+16, 0},
			MAX_Y_SPEED_LEVEL, 
			{  
#if defined(S2628EX)             
				{AT_SCurve, 1000, 4500,  100000, 45000, (4500-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for Y movement.
				{AT_SCurve, 500, 6500,  100000, 45000, (6500-500)*4/10+500, 0, 0.5f, 0.5f}, //for 6/8 pass
				{AT_SCurve, 500, 8600, 100000, 45000, (8600-500)*4/10+500, 0, 0.5f, 0.5f}, //for 3/4pass
				{AT_SCurve, 800, 11500, 100000, 45000, (11500-800)*4/10+800, 0, 0.5f, 0.5f}, //2 pass
				{AT_SCurve, 800, 14500, 100000, 45000, (14500-800)*4/10+800, 0, 0.5f, 0.5f}, //for 1pass.
				//                {AT_SCurve, 800, 3000,  200000, 90000, (3000-800)*4/10+800, 0, 0.5f, 0.5f}, //for Y movement.
				//                {AT_SCurve, 300, 4500,  200000, 90000, (4500-300)*4/10+300,  0, 0.5f, 0.5f}, //for 6/8 pass
				//                {AT_SCurve, 300, 6000,  200000, 90000, (6000-300)*4/10+300, 0, 0.5f, 0.5f}, //for 3/4pass
				//                {AT_SCurve, 500, 8000,  200000, 90000, (8000-500)*4/10+500, 0, 0.5f, 0.5f}, //2 pass
				//                {AT_SCurve, 500, 10000, 200000, 90000, (10000-500)*4/10+500, 0, 0.5f, 0.5f}, //for 1pass.
#elif defined(S1828EX)             
{AT_SCurve, 500, 6000,  100000, 45000, (6000-500)*4/10+500,  0, 0.5f, 0.5f}, //for Y movement.
{AT_SCurve, 500, 8000,  100000, 45000, (8000-500)*4/10+500,  0, 0.5f, 0.5f}, //for 6/8 pass
{AT_SCurve, 500, 12000, 100000, 45000, (12000-500)*4/10+500, 0, 0.5f, 0.5f}, //for 3/4pass
{AT_SCurve, 500, 16000, 100000, 45000, (16000-500)*4/10+500, 0, 0.5f, 0.5f}, //2 pass
{AT_SCurve, 500, 20000, 100000, 45000, (20000-500)*4/10+500, 0, 0.5f, 0.5f}, //for 1pass.
#elif defined(DMN37HE)
	{AT_SCurve, 10, 15000,  30000, 8000, 8000, 0, 0.5f, 0.5f}, //for Y movement.
	{AT_SCurve, 500, 20000,  30000, 8000, 10000, 0, 0.5f, 0.5f}, //for Y movement.
	{AT_SCurve, 500, 25000,  30000, 8000, 10000, 0, 0.5f, 0.5f}, //for Y movement.
	{AT_SCurve, 500, 28000,  30000, 8000, 10000, 0, 0.5f, 0.5f}, //for Y movement.
	{AT_SCurve, 500, 30000,  34000, 9000, 10000, 0, 0.5f, 0.5f}, //for Y movement.
#elif defined(W1615EX) || defined(W1815EX)
{AT_SCurve, 1000, 5000,  150000, 67500, (5000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for Y movement.
{AT_SCurve, 1000, 5000,  150000, 67500, (5000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for 6/8 pass
{AT_SCurve, 1000, 7000,  150000, 67500, (7000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for 3/4pass
{AT_SCurve, 1000, 10000, 150000, 67500, (10000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //2 pass
{AT_SCurve, 1000, 12000, 150000, 67500, (12000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for 1pass.
#elif defined(T1828EX)
{AT_SCurve, 1000, 6000,  100000, 45000, (6000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for Y movement.
{AT_SCurve, 1000, 8000,  100000, 45000, (7000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for 6/8 pass
{AT_SCurve, 1000, 11000, 100000, 45000, (11000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for 3/4pass
{AT_SCurve, 1000, 14000, 100000, 45000, (14000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //2 pass
{AT_SCurve, 1000, 17000, 100000, 45000, (17000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for 1pass.
#elif defined(BEMAJET_2H)
{AT_SCurve, 100, 8000,  100000, 45000, (8000-100)*4/10+100, 0, 0.5f, 0.5f}, //for Y movement.
{AT_SCurve, 100, 9000,  150000, 67500, (9000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 6/8 pass
{AT_SCurve, 100, 12000, 150000, 67500, (12000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 3/4pass
{AT_SCurve, 100, 16000, 150000, 67500, (16000-100)*4/10+100, 0, 0.5f, 0.5f}, //2 pass
{AT_SCurve, 100, 20000, 150000, 67500, (20000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 1pass.   
#elif defined(TATE_BEMAJET)
{AT_SCurve, 100, 8000,  100000, 45000, (8000-100)*4/10+100, 0, 0.5f, 0.5f}, //for Y movement.
{AT_SCurve, 100, 9000,  150000, 67500, (9000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 6/8 pass
{AT_SCurve, 100, 12000, 150000, 67500, (12000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 3/4pass
{AT_SCurve, 100, 16000, 150000, 67500, (16000-100)*4/10+100, 0, 0.5f, 0.5f}, //2 pass
{AT_SCurve, 100, 20000, 150000, 67500, (20000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 1pass.   
#elif defined(LARGE_HB_MOTION_COMMON) || defined(ALLWIN_E180) || defined(Ricoh_Test_Motion)
{AT_SCurve, 100, 6000,  100000, 45000, (6000-100)*4/10+100, 0, 0.5f, 0.5f}, //for Y movement.
{AT_SCurve, 100, 9000,  150000, 67500, (9000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 6/8 pass
{AT_SCurve, 100, 12000, 150000, 67500, (12000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 3/4pass
{AT_SCurve, 100, 16000, 150000, 67500, (16000-100)*4/10+100, 0, 0.5f, 0.5f}, //2 pass
{AT_SCurve, 100, 20000, 150000, 67500, (20000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 1pass.   
#elif defined(A1802)
{AT_SCurve, 100, 8500,  150000, 67500, (8500-100)*4/10+100, 0, 0.5f, 0.5f}, //for Y movement.
{AT_SCurve, 100, 9000,  150000, 67500, (9000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 6/8 pass
{AT_SCurve, 100, 12000, 150000, 67500, (12000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 3/4pass
{AT_SCurve, 100, 16000, 150000, 67500, (16000-100)*4/10+100, 0, 0.5f, 0.5f}, //2 pass
{AT_SCurve, 100, 20000, 150000, 67500, (20000-100)*4/10+100, 0, 0.5f, 0.5f}, //for 1pass.                 
#elif defined(DCM50202) || defined(DCM50202_W1615)
	{AT_SCurve, 400, 50000,  40000, 10000, 1000, 0, 0.5f, 0.5f}, //for Y movement.
	{AT_SCurve, 400, 30000,  40000, 10000, 1000, 0, 0.5f, 0.5f}, //for 6/8 pass
	{AT_SCurve, 400, 20000, 40000, 10000, 1000, 0, 0.5f, 0.5f}, //for 3/4pass
	{AT_SCurve, 400, 30000,  40000, 10000, 1000, 0, 0.5f, 0.5f}, //for 6/8 pass
	{AT_SCurve, 400, 20000, 40000, 10000, 1000, 0, 0.5f, 0.5f}, //for 3/4pass
#else
{AT_SCurve, 1000, 10000,  100000, 45000, (10000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for Y movement.
{AT_SCurve, 1000, 18000,  150000, 67500, (18000-1000)*4/10+1000,  0, 0.5f, 0.5f}, //for 6/8 pass
{AT_SCurve, 1000, 30000, 150000, 67500, (30000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for 3/4pass
{AT_SCurve, 1000, 35000, 200000, 90000, (35000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //2 pass
{AT_SCurve, 1000, 60000, 200000, 90000, (60000-1000)*4/10+1000, 0, 0.5f, 0.5f}, //for 1pass.
#endif
0
			}
		},
		{//for Cleaner 1 stepper motor.
			MOTION_AXIS_PARAM_VERSION, sizeof(struct EPR_MOTION_AXIS_PARAM), 
			AXIS_ID_Cleaner, MT_Stepper, CT_OPEN_LOOP, DT_PulseAndDir, SO_ONLY_INCREMENT_MOVEMENT,
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
#if defined (EPSON_CLEAN_UPDOWN)&&defined (EPSON_BOTTOM_BOARD_V2_1)
0x01, 0xFFFF, 
{  4*32+24, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
#elif defined(COLORFUL_EPSON)
	0x01, 0xFFFF, 
	{  4*32+24, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
#else
0, 0xFFFF, 
{   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
#endif
0xFB, {0*32+1, 0*32+2, 1*32+4, 0 },
MAX_Z_C_SPEED_LEVEL, 
{   //{AT_SCurve, 100, 400,  300000, 40000, (200-50)/10+50, 0, 0.5f, 0.5f}, 
#if defined(A1802) || defined(ALLWIN_E180)    
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 600 * STEPPER_ROTIO_SCALE, 40000, 0},
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 800 * STEPPER_ROTIO_SCALE, 40000, 0}, 
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1000 * STEPPER_ROTIO_SCALE, 40000, 0}, 
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1200 * STEPPER_ROTIO_SCALE, 40000, 0},  //micolor testing machine can not support 1800.
#elif defined (EPSON_CLEAN_UPDOWN)&&defined (EPSON_BOTTOM_BOARD_V2_1)
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 100 * STEPPER_ROTIO_SCALE, 400000, 0},
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 200 * STEPPER_ROTIO_SCALE, 400000, 0}, 
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 400 * STEPPER_ROTIO_SCALE, 400000, 0}, 
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 800 * STEPPER_ROTIO_SCALE, 400000, 0},  //micolor testing machine can not support 1800.
#elif defined(DMN37HE)
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 200 * STEPPER_ROTIO_SCALE, 6000, 0},
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 300 * STEPPER_ROTIO_SCALE, 8000, 0}, 
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 400 * STEPPER_ROTIO_SCALE, 10000, 0}, 
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 800 * STEPPER_ROTIO_SCALE, 12000, 0},  //micolor testing machine can not support 1800.
#else
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 400 * STEPPER_ROTIO_SCALE, 400000, 0},
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 600 * STEPPER_ROTIO_SCALE, 400000, 0}, 
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 800 * STEPPER_ROTIO_SCALE, 400000, 0}, 
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1200 * STEPPER_ROTIO_SCALE, 400000, 0},  //micolor testing machine can not support 1800.
#endif
//{AT_Trapezoid, 100, 1800, 300000, 0},
//{AT_Trapezoid, 100, 2000, 300000, 0}, //the fast speed is about 2000. 
0
}
		}, 
		{//for Cleaner 2 stepper motor.
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
			3, {PWMTYPE_DEFAULT, PWMTYPE_DEFAULT, PWMTYPE_DEFAULT}, 
#if defined (EPSON_CLEAN_UPDOWN)&&defined (EPSON_BOTTOM_BOARD_V2_1)
0x01, 0xFFFF, 
{   4*32 + 16, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
#else
0, 0xFFFF, 
{   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0},
#endif
0xFB, {0*32+4, 0*32+5, 1*32+5, 0},
MAX_Z_C_SPEED_LEVEL, 
{   //{AT_SCurve, 100, 400,  300000, 40000, (200-50)/10+50, 0, 0.5f, 0.5f}, 
#if defined(A1802) || defined(ALLWIN_E180) 
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1200 * STEPPER_ROTIO_SCALE, 400000, 0},
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1300 * STEPPER_ROTIO_SCALE, 400000, 0}, 
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1400 * STEPPER_ROTIO_SCALE, 400000, 0}, 
	{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1600 * STEPPER_ROTIO_SCALE, 400000, 0},  //micolor testing machine can not support 1800.
#elif defined(COLORFUL_EPSON)
    {AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 800 * STEPPER_ROTIO_SCALE, 400000, 0},
    {AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1000 * STEPPER_ROTIO_SCALE, 400000, 0}, 
    {AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1200 * STEPPER_ROTIO_SCALE, 400000, 0}, 
    {AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1400 * STEPPER_ROTIO_SCALE, 400000, 0},  //micolor testing machine can not support 1800.
#else
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 400 * STEPPER_ROTIO_SCALE, 400000, 0},
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 600 * STEPPER_ROTIO_SCALE, 400000, 0}, 
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 800 * STEPPER_ROTIO_SCALE, 400000, 0}, 
{AT_Trapezoid, 100 * STEPPER_ROTIO_SCALE, 1200 * STEPPER_ROTIO_SCALE, 400000, 0},  //micolor testing machine can not support 1800.
#endif
//{AT_Trapezoid, 100, 1800, 300000, 0},
//{AT_Trapezoid, 100, 2000, 300000, 0}, //the fast speed is about 2000. 
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
	MS_INIT,
	MS_IDLE,
	MS_PRINT,
	MS_CLEAN,
	MS_MEASURE,
	MS_NORMAL_MOVE,
	MS_MULTI_AXIS_MOVE_SERIAL,
	MS_MULTI_AXIS_MOVE_PARALLEL,
	MS_OTHERS
};

enum MotionStatus mainStatus FAST_DATA_SECTION;

enum CleanStatus
{
	Clean_IDLE,
	Clean_X_GO_START,
	Clean_X_GO_END,
	Clean_WIPE_GO_HOME,
	Clean_WIPE_GO_END,
	Clean_Z_GO_WIPE,
	Clean_Z_GO_HOME,
	Clean_COMPLETE
};

enum CleanStatus CleanStatus FAST_DATA_SECTION;
enum MotionError
{
	ME_OK,
	ME_DC_OVER_TMEP,
	ME_DC_OVER_CURRENT_OR_UNDER_VOL,
	ME_DC_OVER_BOTH,    //the merged case of the above.
	ME_DC_X_OVERLOAD,
	ME_DC_Y_OVERLOAD
};
enum MotionError MotionError_Main FAST_DATA_SECTION;


enum eInitStatus
{
	IS_IDLE = 0,
	IS_Send_INIT_STAGE1,
#if ((defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)) && \
	!defined(COLORFUL_EPSON))
IS_Micolor_Clean_GoHome,
#elif defined(EPSON_CLEAN_UPDOWN) || defined(COLORFUL_EPSON)
IS_Z_LeaveOriginSensor,
IS_Z_GoOriginSensor,
IS_Z_GoHome,
IS_C_LeaveOriginSensor,
IS_C_GoOriginSensor,
IS_C_GoHome,
#endif
IS_X_LeaveOriginSensor,
IS_X_GoOriginSensor,
IS_X_GoHome,
#ifdef EPSON_CLEAN_UPDOWN
IS_Allwin_Z_GoCap,  //Main task already do it.
#endif
IS_Send_INIT_STAGE2
#if defined(INWEAR_FLAT_FIND_Y_HOME)
,IS_Y_LeaveOriginSensor,
IS_Y_GoOriginSensor,
IS_Y_GoLimitSensor,
IS_Y_LeaveLimitSensor,
IS_Y_GoHome,
IS_X_GoSafePosition,
IS_Finished
#endif
};

enum eInitStatus InitStatus FAST_DATA_SECTION; //for different printer, it is different enum type.
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)
INT8U InitAxis = 0;
INT8U MultiAxis = 0;
#endif

enum eSerialMultiAxisMove
{
	SMAM_IDLE,
	SMAM_Z_MOVE,
	SMAM_C_MOVE,
	SMAM_X_MOVE,
	SMAM_Y_MOVE
}SerialMultiAxisMoveStat = SMAM_IDLE;

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

enum sMediaMeasureStat
{
	MMS_Init,
	MMS_GoStart,
	MMS_Measure,
	MMS_GoHome,
};

#define MAX_MEASURE_POINT   16
struct sMediaMeasureInfo
{
	struct SensorInfo media_sensor;
	INT32S moveRegion;
	INT32S sensor_offset;
	
	INT8U method; //not use. only support the simplest model.
	enum sMediaMeasureStat status;
	INT8U region_count;
	INT32S RegionPoint[MAX_MEASURE_POINT];
} MediaMeasureInfo FAST_DATA_SECTION;
#ifdef SET_ORIGIN_OFFSET
INT32S Media_sensor_offset = 0;
#endif
struct Motion_Wipe
{
	INT8U X_WIPE_SPEED;
	INT8U C_WIPE_TIMES;
	INT16U C_WipePos;
	INT16U Z_WipePos;
	INT16U X_WipePos_Start;
	INT16U X_WipePos_End;	
}MotionWipeInfo = {0};

struct ControlIOInfo
{
	INT8U ValidLevel;
	Pin PinIO;
};

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
#ifdef RECORD_MOTION_MOVEMENT
#define ACCTABLE_ADDR   (AT91C_DDR2 + BOARD_DDRAM_SIZE - RESERVED_MEMSIZE)
#else
#define ACCTABLE_ADDR   HUGEBUF_END_ADDR
#endif

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
	struct ACCCurveInfo * pAllCurveX[MAX_X_SPEED_LEVEL];
	struct ACCCurveInfo * pAllCurveY[MAX_Y_SPEED_LEVEL];
	struct ACCCurveInfo * pAllCurveZ[MAX_Z_C_SPEED_LEVEL];
	struct ACCCurveInfo * pAllCurveC[MAX_Z_C_SPEED_LEVEL];
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

//only for (not use until now) 
//  1. 带驱动器和光栅的X向。
//  2. 带驱动器和编码器的Y向。
//  3. 产生虚打信号。
struct AxisParam_PulseDir_Feedback
{
	INT8U  AxisID;
	INT8U  ChannelNO;
	INT16U CoorRegAddr;
	
	struct SensorInfo Origin;
	INT32S OriginSensorOffset;
	struct SensorInfo Limit;
	
	INT8U IsCheckTerm;
	INT32S Origin_Offset;
	INT32S Region_Start; //-1 means Unkown and NO-limitation
	INT32S Region_End; //-1 means Unkown and NO-limitation.
	
	//channel info.
	AT91PS_PWMC_CH pPWM_channel1;    
	AT91PS_PWMC_CH pPWM_channel2;    
	INT32U  Channel_Mask;
	INT8U ChannelValidLevel[2];
	
	//for X, 720DPI : pulse_number_from_motor_encoder.
	//for Y, pulse_number_from_feedback_encoder : pulse_number_from_motor_encoder
	//Fix point number which frac bit is 24. 
	INT32U ratio_motor; 
	//for X, 720DPI : pulse number based on print coor(720,540,1440).
	//for Y, always be 1.
	//Fix point number which frac bit is 24. 
	INT32U ratio_feedback;  
	
	float ratio_move; 
	float ratio_print[3]; 
	
	//the target coor when stop. the real coor can be different with ideal coor. the next movement will compensate the offset..
	INT32U CoorIdeal; 
	
	INT8U Dir; //0,standby, 1, positive, 2, negative.
	INT8U MoveType; //0, segment move; 1, continue move, and stop when receive stop cmd.
	INT8U AccSegment; //0: constant, 1: speed up, 2: speed down.
	
	INT32U curPWMTick;
	INT32U curPWMTickInAccSegment;
	
	INT32U speedDownCoor;
	INT32U speedDownIndex;
	
	INT16U * pAccTable; //the new cycle-time after a cycle. 
	INT32U AccTableLen;
	INT32U AccTableIndex;
	
	struct ACCCurveInfo ** pAllCurveTable;
	
	INT32U Min_Distance;
	
	struct EPR_MOTION_AXIS_PARAM * pAxisParam;
};

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
static struct AxisParam_PulseDir_OpenLoop *pTC_Driver1 FAST_DATA_SECTION=NULL;
static struct AxisParam_PulseDir_OpenLoop *pTC_Driver2 FAST_DATA_SECTION=NULL;

#define ERROR_HISTORY_LEN   100
//only for 
//  1. 不带驱动器和带光栅的DC驱动的X向。
//  2. 不带驱动器的DC驱动的Y向。包括没有编码器和有编码器两种情况。
struct AxisParam_DC_PWM
{
	INT8U  AxisID;
	INT8U  ChannelNO;
	INT16U CoorRegAddr;
	INT16U CoorRegAddr_DoubleFeedback;
	
	struct SensorInfo Origin;
	INT32S OriginSensorOffset;
	struct SensorInfo Limit;
	
	INT8U IsCheckTerm;
	INT32S Origin_Offset;
	INT32S Origin_Offset_Feedback;
	INT32S Region_Start; //0x7FFFFFFF means Unkown and NO-limitation
	INT32S Region_End; //0x7FFFFFFF means Unkown and NO-limitation.
	
	//channel info.
	AT91PS_PWMC_CH pPWM_channel1;    
	AT91PS_PWMC_CH pPWM_channel2;    
	INT32U Channel_Mask;
	INT8U ChannelValidLevel[2];
	struct ControlIOInfo ControlIO;
	struct SensorInfo Fault;
	struct SensorInfo OverTemp;
	
	INT16U BaseDPI; //720.
	INT16U CurrDPI; //720,360,540,270,1440.
	
	INT32U ratioTable_StepPerCycle[MAX_SPEED_LEVEL];
	struct ACCCurveInfo ** pAllCurveTable;
	INT32U PIDTable[MAX_SPEED_LEVEL+1][3]; //index 0 for standby.
	INT8U PWM_modelTable[3]; //index 0 for standby.
	
	float ratio_move; 
	float ratio_print[3]; 
	
	//the target coor when stop. the real coor can be different with ideal coor. the next movement will compensate the offset..
	INT32U CoorIdeal; 
	INT32U PermitStopError;
	INT32U Coor_NextCycle;
	INT32U Coor_NextCycle_Frac; //Fix point number which frac bit is 24. 
	INT32U ratio_StepPerCycle; //Fix point number which frac bit is 24. 
	
	INT8U Dir; //0,standby, 1, positive, 2, negative.
	INT8U MoveType; //0, segment move; 1, continue move, and stop when receive stop cmd.
	INT8U PWM_model; //based on driver CHIP.
	INT8U AccSegment; //0: constant, 1: speed up, 2: speed down.
	
	INT32U curPWMTick;
	INT32U curPWMTickInAccSegment;
	
	INT32U speedDownCoor;
	INT32U speedDownIndex;
	
	INT8U * pAccTable; //the coor increment after a PWM cycle. 
	INT32U AccTableLen;
	INT32U AccTableIndex;
	
	//PIDREG3 part
	int  e;			/* Input: Error  (Q14)  */
	int  e1;			/* Variable: Error  (Q14)  */
	int  e2;			/* Variable: Error  (Q14)  */
	int  Kp;			/* Parameter: Proportional gain (Q15) */
	int  Ki;			/* Parameter: Integral gain  (Q31-16bit) */
	int  Kd; 			/* Parameter: Derivative gain  (Q14)  */
	int  fixpoint_bit; /* for PID parameter , default is 16  */
	int  pid_out_max;		/* Parameter: Maximum output  (Q15)  */ //half of cycle time -50
	int  pid_out_min;		/* Parameter: Minimum output  (Q15)  */ //half of cycle time -50
	int  pid_out;   		/* Output: PID output  (Q15)  */
	int  pid_out_frac;   	
	int  pid_i_out_frac;   	
	int  e_history[ERROR_HISTORY_LEN];
	int  e_i;
	
	INT32U Min_Distance;
	INT16S PWM_MaxValue,PWM_MinValue; 
	INT16S PWM_CurValue;
	INT16U PWM_OverLoadNum;
	INT16U PWM_MaxOverLoadNum;//200
	INT16S PWM_CycleTime;
	INT16S PWM_DeadTime; // not use.
	
	//for Y coor overflow.
	INT32U lastCoor;
	//for Y coor, not rollback.
	INT32U Coor_nonRollback;
	
	struct EPR_MOTION_AXIS_PARAM * pAxisParam;
}   XParam FAST_DATA_SECTION = {0}, YParam FAST_DATA_SECTION = {0}, 
*pPWMC_Driver1 FAST_DATA_SECTION=NULL, *pPWMC_Driver2 FAST_DATA_SECTION=NULL;

void PWMHandler(void);
void TCHandler(void);
INT8U TC_Stop_Move(struct AxisParam_PulseDir_OpenLoop * pStepperTC);
#ifdef VM_PRINT
INT32U PWM_OFFSET_INDEX = 0;
INT8U CUR_SPEED_ID = 0;;
void VM_PWM_START(struct AxisParam_DC_PWM * pDCPWM)
{
	AT91C_BASE_PWMC_CH0->PWMC_CMR = AT91C_PWMC_CPRE_MCKA | AT91C_PWMC_CALG ;
	AT91C_BASE_PWMC_CH0->PWMC_CDTYR =(INT16U)(( (float)BOARD_MCK/(pDCPWM->pAxisParam->accParam[CUR_SPEED_ID].EndFreq*16))+0.5f);
	AT91C_BASE_PWMC_CH0->PWMC_CPRDR = AT91C_BASE_PWMC_CH0->PWMC_CDTYR*2;
	AT91C_BASE_PWMC_CH0->PWMC_CUPDR = AT91C_BASE_PWMC_CH0->PWMC_CDTYR;
	
	AT91C_BASE_PWMC_CH1->PWMC_CMR = AT91C_PWMC_CPRE_MCKA | AT91C_PWMC_CALG | AT91C_PWMC_CPOL;
	AT91C_BASE_PWMC_CH1->PWMC_CDTYR = (INT16U)(( (float)BOARD_MCK/(pDCPWM->pAxisParam->accParam[CUR_SPEED_ID].EndFreq*16))+0.5f);
	AT91C_BASE_PWMC_CH1->PWMC_CPRDR = AT91C_BASE_PWMC_CH1->PWMC_CDTYR*2;
	AT91C_BASE_PWMC_CH1->PWMC_CUPDR = AT91C_BASE_PWMC_CH1->PWMC_CDTYR;
	if(pDCPWM->Dir == DIRTYPE_POS)
	{
		AT91C_BASE_PWMC_CH2->PWMC_CMR = AT91C_PWMC_CPRE_MCKB | AT91C_PWMC_CPOL;
		AT91C_BASE_PWMC_CH2->PWMC_CDTYR = (INT16U)(( (float)BOARD_MCK/(pDCPWM->pAxisParam->accParam[CUR_SPEED_ID].EndFreq*16))+0.5f)*2;
		AT91C_BASE_PWMC_CH2->PWMC_CPRDR = AT91C_BASE_PWMC_CH2->PWMC_CDTYR*2;
		AT91C_BASE_PWMC_CH2->PWMC_CUPDR = AT91C_BASE_PWMC_CH2->PWMC_CDTYR;
		
		AT91C_BASE_PWMC_CH3->PWMC_CMR = AT91C_PWMC_CPRE_MCKB;
		AT91C_BASE_PWMC_CH3->PWMC_CDTYR = (INT16U)(( (float)BOARD_MCK/(pDCPWM->pAxisParam->accParam[CUR_SPEED_ID].EndFreq*16))+0.5f)*2;
		AT91C_BASE_PWMC_CH3->PWMC_CPRDR = AT91C_BASE_PWMC_CH3->PWMC_CDTYR*2;
		AT91C_BASE_PWMC_CH3->PWMC_CUPDR = AT91C_BASE_PWMC_CH3->PWMC_CDTYR;
	}
	else
	{
		AT91C_BASE_PWMC_CH2->PWMC_CMR = AT91C_PWMC_CPRE_MCKB;
		AT91C_BASE_PWMC_CH2->PWMC_CDTYR = (INT16U)(( (float)BOARD_MCK/(pDCPWM->pAxisParam->accParam[CUR_SPEED_ID].EndFreq*16))+0.5f)*2;
		AT91C_BASE_PWMC_CH2->PWMC_CPRDR = AT91C_BASE_PWMC_CH2->PWMC_CDTYR*2;
		AT91C_BASE_PWMC_CH2->PWMC_CUPDR = AT91C_BASE_PWMC_CH2->PWMC_CDTYR;
		
		AT91C_BASE_PWMC_CH3->PWMC_CMR = AT91C_PWMC_CPRE_MCKB | AT91C_PWMC_CPOL;
		AT91C_BASE_PWMC_CH3->PWMC_CDTYR = (INT16U)(( (float)BOARD_MCK/(pDCPWM->pAxisParam->accParam[CUR_SPEED_ID].EndFreq*16))+0.5f)*2;
		AT91C_BASE_PWMC_CH3->PWMC_CPRDR = AT91C_BASE_PWMC_CH3->PWMC_CDTYR*2;
		AT91C_BASE_PWMC_CH3->PWMC_CUPDR = AT91C_BASE_PWMC_CH3->PWMC_CDTYR;
	}
	AT91C_BASE_PWMC->PWMC_ENA = (AT91C_PWMC_CHID0|AT91C_PWMC_CHID1|AT91C_PWMC_CHID2|AT91C_PWMC_CHID3);
}

void VM_PWM_STOP(void)
{
	AT91C_BASE_PWMC->PWMC_DIS = (AT91C_PWMC_CHID0|AT91C_PWMC_CHID1|AT91C_PWMC_CHID2|AT91C_PWMC_CHID3);
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
#if defined(SET_ORIGIN_OFFSET)&&defined(MANUFACTURER_GZ)    
	if(EPSON_PERMIT_GROUPNUM == 1)
		motionParam.axis_param[AXIS_ID_X-1].Fixed_Position[MPID_MEASURE_END] = 3260;
	else
		motionParam.axis_param[AXIS_ID_X-1].Fixed_Position[MPID_MEASURE_END] = 1786;
#endif
	return SaveMotionParam(&motionParam);
}
#ifdef HEAD_EPSON_GEN5
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
			ret = GetFPGAExpandIoStat(pSensor->FPGABit);
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
			ret = ConfigFPGAExpandIoIT(fpgahandler, pSensor->FPGABit, data);
			EnableFPGAExpandIoIT(pSensor->FPGABit);
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
			UnConfigFPGAExpandIoIT(pSensor->FPGABit);
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
#elif defined(HEAD_RICOH_G4)
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

//SpeedID : 0~3: VSDModel 1-4; 4: return speed for uni-dir
INT8U XPrintSpeedMap(INT16U currDPI, INT8U SpeedID)
{
	INT8U i;
	if(SpeedID > 5)
		return (INT8U) -1;
	switch(currDPI)
	{
	case 720:   i = 0; break;
	case 360:   i = 1; break;
	case 540:   i = 2; break;
	case 270:   i = 3; break;
	case 1440:   i = 4; break;
	default:
		return (INT8U) -1;
	}
	return i*5 + SpeedID;
}

//MoveSpeedID : 0~2: slow/middle/high
INT8U XMoveSpeedMap(INT8U MoveSpeedID)
{
	if(MoveSpeedID >= 3)
		return (INT8U) -1;
	
	return 5*5 + MoveSpeedID;
}

INT8U GetPrintSpeedX(INT8U speedLevel, INT16U *currDPI, INT8U *SpeedID)
{
	const INT16U speedMap[5] = {720,360,540,270,1440};
	
	if(speedLevel >= 5*5)
		return False;
	
	*currDPI = speedMap[speedLevel/5];
	*SpeedID = speedLevel%5;
	
	return True;
}

INT16U GetBaseDPI(INT16U currDPI, INT8U *id)
{
	switch(currDPI)
	{
	default:
	case 720:   *id = 0;    return 720;
	case 360:   *id = 0;    return 720;
	case 540:   *id = 1;    return 540;
	case 270:   *id = 1;    return 540;
	case 1440:  *id = 2;    return 1440;
	}
}

INT8U BuildPWMRatioTable(INT32U *pRatioTable, const struct AxisParam_DC_PWM * pDCPWM, 
						 const struct EPR_MOTION_AXIS_PARAM * pAxisParam, INT32U PWMCycleTick, INT8U SpeedCount) //PWMCycleTick based on MCK(133Mhz).
{
	INT32U i;
	double freq,ratio;
	for(i=0; i<SpeedCount; i++)
	{
		switch(pAxisParam->accParam[i].type)
		{
		case AT_Trapezoid:
			freq = pAxisParam->accParam[i].EndFreq;
			break;
		case AT_SCurve:
			freq = pAxisParam->accParam[i].EndFreq;
			break;
		default:
			status_ReportStatus(STATUS_FTA + SciError_Parameter, STATUS_SET);
			return False;
		}
		if(pAxisParam->controlType == CT_CLOSE_LOOP)
		{
			INT8U index = 0;
			if(pAxisParam->AxisID == AXIS_ID_X)
			{
				INT16U currDPI;
				INT8U SpeedID;
				if(GetPrintSpeedX(i, &currDPI, &SpeedID))
				{   
					GetBaseDPI(currDPI, &index);
				}
			}
			freq *= pAxisParam->transmisstion_ratio_close_loop[index];
		}
		else
			freq *= pAxisParam->transmisstion_ratio_open_loop;
		ratio = freq * PWMCycleTick /BOARD_MCK;
		pRatioTable[i] = (INT32U)(ratio *(1<<24));
	}
	return True;
}

INT8U CalcBezierCtrlPoint(struct BezierCurve *p, INT32U startPointIndex, INT32U endPointIndex, struct Point *pPool)
{
	struct BezierCurve newP1,newP2;
	INT32U index = (endPointIndex + startPointIndex)/2;
	if(endPointIndex - startPointIndex == 2)
	{
		pPool[index].x = (p->p0.x + 3 * p->p1.x + 3 * p->p2.x + p->p3.x)/8;
		pPool[index].y = (p->p0.y + 3 * p->p1.y + 3 * p->p2.y + p->p3.y)/8;
		return True;
	}
	else
	{
		newP1.p0.x = p->p0.x;
		newP1.p0.y = p->p0.y;
		newP2.p3.x = p->p3.x;
		newP2.p3.y = p->p3.y;
		newP1.p1.x = (p->p0.x + p->p1.x)/2;
		newP1.p1.y = (p->p0.y + p->p1.y)/2;
		newP2.p2.x = (p->p2.x + p->p3.x)/2;
		newP2.p2.y = (p->p2.y + p->p3.y)/2;
		
		newP1.p2.x = (p->p1.x + p->p2.x)/2;
		newP1.p2.y = (p->p1.y + p->p2.y)/2;
		newP2.p1.x = (newP2.p2.x + newP1.p2.x)/2;
		newP2.p1.y = (newP2.p2.y + newP1.p2.y)/2;
		newP1.p2.x = (newP1.p2.x + newP1.p1.x)/2;
		newP1.p2.y = (newP1.p2.y + newP1.p1.y)/2;
		
		pPool[index].x = newP1.p3.x = newP2.p0.x = (newP1.p2.x + newP2.p1.x)/2;
		pPool[index].y = newP1.p3.y = newP2.p0.y = (newP1.p2.y + newP2.p1.y)/2;
		
		CalcBezierCtrlPoint(&newP1, startPointIndex, index, pPool);
		CalcBezierCtrlPoint(&newP2, index, endPointIndex, pPool);
		return False;
	}
}

INT8U BuildPWMCurve(struct ACCCurveInfo * pCurveTable, const struct AxisParam_DC_PWM * pDCPWM, 
					const struct EPR_MOTION_AXIS_PARAM * pAxisParam, INT32U PWMCycleTick, INT8U SpeedId) //PWMCycleTick based on MCK(133Mhz).
{
	INT32U i, stepCount, S0_int, S1_int, *pDis;
	float S1, t;
	double ratio;
	
	pCurveTable->AccType = ACCTYPE_DISTANCE;
	
	if(pAxisParam->controlType == CT_CLOSE_LOOP)
	{
		INT8U index = 0;
		if(pAxisParam->AxisID == AXIS_ID_X)
		{
			INT16U currDPI;
			INT8U SpeedID;
			if(GetPrintSpeedX(SpeedId, &currDPI, &SpeedID))
			{
				GetBaseDPI(currDPI, &index);
			}
		}
		ratio = pAxisParam->transmisstion_ratio_close_loop[index];
	}
	else
		ratio = pAxisParam->transmisstion_ratio_open_loop;
	
	if(pAxisParam->accParam[SpeedId].type == AT_Trapezoid)
	{
		const struct AccParam *pTrapezoid = (struct AccParam *)&pAxisParam->accParam[SpeedId];
		stepCount = (INT32U)((float)pTrapezoid->AccTime*BOARD_MCK/PWMCycleTick/1000000+0.5);
		if(stepCount == 0)
			stepCount = 1;
		
		pCurveTable->AccCurveLen = stepCount;
		pCurveTable->AccDisTblOffset = (stepCount + sizeof(INT32U) -1)/sizeof(INT32U)*sizeof(INT32U);
		pDis = (INT32U*)(pCurveTable->IncTable + pCurveTable->AccDisTblOffset);
		
		if((INT32U)pDis + (pCurveTable->AccCurveLen+1) * sizeof(INT32U) >= (INT32U)STEP_POOL)
			return False;
		
		S0_int = 0;
		pDis[0] = 0;
		for(i = 1; i <= stepCount ; i++)
		{
			t = (float)i*PWMCycleTick/BOARD_MCK;
			S1 = (float)ratio * ((float)i * (pTrapezoid->EndFreq  - pTrapezoid->startFreq ) / stepCount/2 + pTrapezoid->startFreq ) * t;
			
			pDis[i] = (INT32U)(S1*(1<<ACC_DISTANCE_SHIFT_BIT)+0.5);
			S1_int = (INT32U)(S1+0.5);
			pCurveTable->IncTable[i-1] = (INT8U)(S1_int - S0_int);
			S0_int = S1_int;
		}
		pCurveTable->AccDistance = S1_int;
	}
	else if(pAxisParam->accParam[SpeedId].type == AT_SCurve)
	{
		const struct AccParam * pSCurve = (const struct AccParam *)&pAxisParam->accParam[SpeedId];
		INT32U ACC_StepCount = (INT32U)((float)pSCurve->AccCurveTime*BOARD_MCK/PWMCycleTick/1000000+0.5);
		struct BezierCurve start;
		INT32U ACC_StepCount_align2power, tmp;
		struct Point *pPool = POINT_POOL;
		float accEndVec = (float)(pSCurve->AccCurveEndFreq - pSCurve->startFreq);
		float K, K_x, endVec = (float)(pSCurve->EndFreq - pSCurve->startFreq);
		float * pStepTool = STEP_POOL;
		float startVec = (float)pSCurve->startFreq;
		float V0, V1, S0;
		
		stepCount = (INT32U)((float)pSCurve->AccTime*BOARD_MCK/PWMCycleTick/1000000+0.5);
		pCurveTable->AccCurveLen = stepCount;
		pCurveTable->AccDisTblOffset = (stepCount + sizeof(INT32U) -1)/sizeof(INT32U)*sizeof(INT32U);
		pDis = (INT32U*)(pCurveTable->IncTable + pCurveTable->AccDisTblOffset);
		if((INT32U)pDis + (pCurveTable->AccCurveLen+1) * sizeof(INT32U) >= (INT32U)STEP_POOL)
			return False;
		
		if(ACC_StepCount >= pCurveTable->AccCurveLen/2)
			ACC_StepCount = pCurveTable->AccCurveLen/2 -1;
		
		if(ACC_StepCount == 0)
			ACC_StepCount = 1;
		else if(ACC_StepCount >= MAX_POINT_NUM)
			ACC_StepCount = MAX_POINT_NUM -1;
		ACC_StepCount_align2power = 1;
		tmp = ACC_StepCount;
		while(tmp != 0)
		{
			tmp >>= 1;
			ACC_StepCount_align2power <<= 1;
		}
		
		K = (endVec - 2*accEndVec)/(float)(stepCount - 2* ACC_StepCount);
		K_x = accEndVec /K;
		
		pPool[0].x = start.p0.x = 0.0;
		pPool[0].y = start.p0.y = 0.0;
		start.p1.x = ((float)ACC_StepCount - K_x) * pSCurve->CtrlPoint[0];
		start.p1.y = 0;
		start.p2.x = (float)ACC_StepCount - K_x * pSCurve->CtrlPoint[1];
		start.p2.y = accEndVec * (1.0 - pSCurve->CtrlPoint[1]);
		pPool[ACC_StepCount_align2power].x = start.p3.x = (float)ACC_StepCount;
		pPool[ACC_StepCount_align2power].y = start.p3.y = accEndVec;
		CalcBezierCtrlPoint(&start, 0, ACC_StepCount_align2power, pPool);
		
		tmp = 0;
		pStepTool[0] = startVec * ratio;
		for(i = 1; i<=ACC_StepCount; i++)
		{
			while(pPool[tmp].x < (float)i)
			{
				tmp ++;
				if(tmp >= ACC_StepCount_align2power)
				{
					tmp = ACC_StepCount_align2power;
					break;
				}
			}
			pStepTool[i] = pPool[tmp-1].y + (pPool[tmp].y - pPool[tmp-1].y)*((float)i - pPool[tmp-1].x)/(pPool[tmp].x - pPool[tmp-1].x);
			pStepTool[i] += startVec;
			
			pStepTool[i] *= ratio;
		}
		
		pDis[0] = 0;
		S1 = 0.0;
		t = (float)PWMCycleTick/BOARD_MCK;
		for(i = 1; i<=ACC_StepCount; i++)
		{
			S1 += (pStepTool[i]+pStepTool[i-1])/2*t; 
			pDis[i] = (INT32U)(S1 *(1<<ACC_DISTANCE_SHIFT_BIT));
		}
		V0 = pStepTool[ACC_StepCount];
		K *= ratio;
		S0 = S1;
		for(i= 1; i <= stepCount-2*ACC_StepCount; i++)
		{
			V1 = V0+K*i;
			S1 = S0 + (V0 + V1)/2*i*t; 
			pDis[i+ACC_StepCount] = (INT32U)(S1 *(1<<ACC_DISTANCE_SHIFT_BIT));
		}
		for(i = 1; i <= ACC_StepCount; i++)
		{
			S1 += (V0 - (pStepTool[ACC_StepCount - i+1]+pStepTool[ACC_StepCount - i])/2 + V1)*t; 
			pDis[i + stepCount - ACC_StepCount] = (INT32U)(S1 *(1<<ACC_DISTANCE_SHIFT_BIT));
		}
		
		tmp =0;
		for(i=0; i<pCurveTable->AccCurveLen; i++)
		{
			pCurveTable->IncTable[i] = ((pDis[i+1]+0x80)>>ACC_DISTANCE_SHIFT_BIT)-tmp;
			tmp += pCurveTable->IncTable[i];
		}
		pCurveTable->AccDistance = tmp;
	}
	else
		return False;
	
	return True;
}

INT8U BuildAllPWMCurve(struct AxisParam_DC_PWM * pDCPWM, 
					   const struct EPR_MOTION_AXIS_PARAM * pAxis_param, INT32U PWMCycleTick) //PWMCycleTick based on MCK(133Mhz).
{
	INT32U  i;
	pDCPWM->pAllCurveTable = NULL;
	
	switch(pAxis_param->AxisID)
	{
	case AXIS_ID_X:
		pDCPWM->pAllCurveTable = AccCurveIndexTbl.pAllCurveX;
		if(pAxis_param->validSpeedLevel > MAX_X_SPEED_LEVEL)
			return False;
		break;
	case AXIS_ID_Y:
		pDCPWM->pAllCurveTable = AccCurveIndexTbl.pAllCurveY;
		if(pAxis_param->validSpeedLevel > MAX_Y_SPEED_LEVEL)
			return False;
		break;
	default:
		return False;
	}
	
	for(i = 0; i < pAxis_param->validSpeedLevel; i++)
	{
		pDCPWM->pAllCurveTable[i] = (struct ACCCurveInfo *)currCurveBuf;
		if(!BuildPWMCurve(pDCPWM->pAllCurveTable[i], pDCPWM, pAxis_param, PWMCycleTick, i))
			return False;
		currCurveBuf = (INT32U)(pDCPWM->pAllCurveTable[i]->IncTable) + 
			pDCPWM->pAllCurveTable[i]->AccDisTblOffset + 
				(pDCPWM->pAllCurveTable[i]->AccCurveLen + 1) * sizeof(INT32U);
	}
	return True;
}

INT8U BuildAllPID(INT32U *pPIDTable, const struct AxisParam_DC_PWM * pDCPWM, 
				  const struct EPR_MOTION_AXIS_PARAM * pAxisParam)
{
	INT32U  i;
	double ratio;
	
	for(i = 0; i < pAxisParam->validSpeedLevel+1; i++) 
	{
		if(pAxisParam->controlType == CT_CLOSE_LOOP)
		{
			INT8U index = 0;
			if(pAxisParam->AxisID == AXIS_ID_X)
			{
				INT16U currDPI;
				INT8U SpeedID;
				if(i != 0 && GetPrintSpeedX(i-1, &currDPI, &SpeedID))
				{
					GetBaseDPI(currDPI, &index);
				}
			}
			ratio = pAxisParam->transmisstion_ratio_close_loop[index];
		}
		else
			ratio = pAxisParam->transmisstion_ratio_open_loop;
		
		*pPIDTable = (INT32U)(pAxisParam->PID_Speed[i][0] / (float)ratio * (1<<PID_SHIFT_BIT));
		pPIDTable++;
		*pPIDTable = (INT32U)(pAxisParam->PID_Speed[i][1] / (float)ratio * (1<<(PID_SHIFT_BIT+PID_I_SHIFT_BIT)));
		pPIDTable++;
		*pPIDTable = (INT32U)(INT32S)(pAxisParam->PID_Speed[i][2]/ERROR_HISTORY_LEN / (float)ratio * (1<<(PID_SHIFT_BIT+PID_I_SHIFT_BIT)));
		pPIDTable++;
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
	else if(pAxisParam->accParam[SpeedId].type == AT_SCurve)
	{
		const struct AccParam * pSCurve = (struct AccParam *)&pAxisParam->accParam[SpeedId];
		INT32U ACC_CurveTime = (INT32U)((float)pSCurve->AccCurveTime*BOARD_MCK/1000000/DivideNum+0.5);
		struct BezierCurve start;
		INT32U ACC_SplitCount_align2power=(1<<10);
		INT32S tmp;
		struct Point *pPool = POINT_POOL;
		float accEndFreq = (float)((pSCurve->AccCurveEndFreq - pSCurve->startFreq) * ratio); 
		float K_x;
		
		endFreq = (float)((pSCurve->EndFreq - pSCurve->startFreq) * ratio);
		startFreq = (float)(pSCurve->startFreq * ratio);
		
		accTime = (INT32U)((float)pSCurve->AccTime*BOARD_MCK/1000000/DivideNum+0.5);
		
		if(accTime <= 2 * ACC_CurveTime)
		{
			ACC_CurveTime = accTime/2 - 1;
		}
		
		K = (endFreq - 2*accEndFreq)/(float)(accTime - 2* ACC_CurveTime);
		K_x = accEndFreq /K;
		
		pPool[0].x = start.p0.x = 0.0;
		pPool[0].y = start.p0.y = 0.0;
		start.p1.x = ((float)ACC_CurveTime - K_x) * pSCurve->CtrlPoint[0];
		start.p1.y = 0;
		start.p2.x = (float)ACC_CurveTime - K_x * pSCurve->CtrlPoint[1];
		start.p2.y = accEndFreq * (1.0 - pSCurve->CtrlPoint[1]);
		pPool[ACC_SplitCount_align2power].x = start.p3.x = (float)ACC_CurveTime;
		pPool[ACC_SplitCount_align2power].y = start.p3.y = accEndFreq;
		CalcBezierCtrlPoint(&start, 0, ACC_SplitCount_align2power, pPool);
		
		pCycle = (INT16U*)(pCurveTable->IncTable); 
		PulseCount=0;
		tmp = 1;
		for(i= 0; i<ACC_CurveTime;)
		{
			while(pPool[tmp].x < (float)i)
			{
				tmp ++;
				if(tmp >= ACC_SplitCount_align2power)
				{
					tmp = ACC_SplitCount_align2power;
					break;
				}
			}
			freq = pPool[tmp-1].y + (pPool[tmp].y - pPool[tmp-1].y)*((float)i - pPool[tmp-1].x)/(pPool[tmp].x - pPool[tmp-1].x);
			freq += startFreq;
			
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
		
		for(; i<accTime - ACC_CurveTime;)
		{
			freq = K* (i - ACC_CurveTime) + accEndFreq;
			freq += startFreq;
			
			tick = (BOARD_MCK/(DivideNum*freq)+0.5);
			if(tick > 0xFFFF)
				tick = 0xFFFF;
			*pCycle = (INT16U)tick;
			i+=*pCycle;
			pCycle ++;
			if( (INT32U)pCycle >= (INT32U)STEP_POOL)
				return False;
			PulseCount ++;
		}
		
		tmp = ACC_SplitCount_align2power-1;
		for(; i < accTime; )
		{
			while(pPool[tmp].x > (float)(accTime- i))
			{
				tmp --;
				if( tmp < 0)
				{
					tmp = 0;
					break;
				}
			}
			freq = pPool[tmp].y + (pPool[tmp+1].y - pPool[tmp].y)*((float)(accTime- i) - pPool[tmp].x)/(pPool[tmp+1].x - pPool[tmp].x);
			freq = endFreq - freq + startFreq;
			
			tick = (BOARD_MCK/(DivideNum*freq)+0.5);
			if(tick > 0xFFFF)
				tick = 0xFFFF;
			*pCycle = (INT16U)tick;
			i += *pCycle;
			pCycle ++;
			if( (INT32U)pCycle >= (INT32U)STEP_POOL)
				return False;
			PulseCount ++;
		}
		tick = (BOARD_MCK/(DivideNum*(endFreq+startFreq))+0.5);
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
	case AXIS_ID_Cleaner:
		pPulseDir->pAllCurveTable = AccCurveIndexTbl.pAllCurveC;
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

typedef void (*pPWMChannelHandle)(struct AxisParam_DC_PWM * pDCPWM, INT32U curCoor);
void PWMHandler_dimmy(struct AxisParam_DC_PWM * pDCPWM, INT32U curCoor)
{
}
pPWMChannelHandle pPWM1Handle, pPWM2Handle; 

INT8U InitPWMChannel()
{
	Pin MotionPWMPins[] = {BOARD_PINS_MB_PWM};
	INT8U ret = True;
	
	PMC_EnablePeripheral(AT91C_ID_PWMC);
	
#ifdef VM_PRINT
	AT91C_BASE_PWMC->PWMC_MR = (1<<10|1)|(((1<<10|1)<<16));
#else
	AT91C_BASE_PWMC->PWMC_MR = 0;
#endif
	AT91C_BASE_PWMC->PWMC_DIS = (AT91C_PWMC_CHID0 | AT91C_PWMC_CHID1 | AT91C_PWMC_CHID2 | AT91C_PWMC_CHID3);
	AT91C_BASE_PWMC->PWMC_IDR = (AT91C_PWMC_CHID0 | AT91C_PWMC_CHID1 | AT91C_PWMC_CHID2 | AT91C_PWMC_CHID3);
	AT91C_BASE_PWMC->PWMC_ISR;
	
	AT91C_BASE_PWMC_CH0->PWMC_CMR = AT91C_PWMC_CALG;
	AT91C_BASE_PWMC_CH0->PWMC_CDTYR = PWM_STANDBY_CYCLE/2;
	AT91C_BASE_PWMC_CH0->PWMC_CPRDR = PWM_STANDBY_CYCLE/2;
	AT91C_BASE_PWMC_CH0->PWMC_CUPDR = PWM_STANDBY_CYCLE/2;
	
	AT91C_BASE_PWMC_CH1->PWMC_CMR = AT91C_PWMC_CALG;
	AT91C_BASE_PWMC_CH1->PWMC_CDTYR = PWM_STANDBY_CYCLE/2;
	AT91C_BASE_PWMC_CH1->PWMC_CPRDR = PWM_STANDBY_CYCLE/2;
	AT91C_BASE_PWMC_CH1->PWMC_CUPDR = PWM_STANDBY_CYCLE/2;
	
	AT91C_BASE_PWMC_CH2->PWMC_CMR = AT91C_PWMC_CALG;
	AT91C_BASE_PWMC_CH2->PWMC_CDTYR = PWM_STANDBY_CYCLE/2;
	AT91C_BASE_PWMC_CH2->PWMC_CPRDR = PWM_STANDBY_CYCLE/2;
	AT91C_BASE_PWMC_CH2->PWMC_CUPDR = PWM_STANDBY_CYCLE/2;
	
	AT91C_BASE_PWMC_CH3->PWMC_CMR = AT91C_PWMC_CALG;
	AT91C_BASE_PWMC_CH3->PWMC_CDTYR = PWM_STANDBY_CYCLE/2;
	AT91C_BASE_PWMC_CH3->PWMC_CPRDR = PWM_STANDBY_CYCLE/2;
	AT91C_BASE_PWMC_CH3->PWMC_CUPDR = PWM_STANDBY_CYCLE/2;
	
	PIO_Configure(MotionPWMPins, PIO_LISTSIZE(MotionPWMPins));
	
	pPWM1Handle = PWMHandler_dimmy;
	pPWM2Handle = PWMHandler_dimmy; 
	
	if(pPWMC_Driver1 != NULL)
	{
		ConfigSensor(&pPWMC_Driver1->Origin);
		ConfigSensor(&pPWMC_Driver1->Limit);
		ConfigControlIO(&pPWMC_Driver1->ControlIO, True);
		ConfigSensor(&pPWMC_Driver1->Fault);
		ConfigSensor(&pPWMC_Driver1->OverTemp);
		//	ConfigSensor(struct SensorInfo * pSensor)
		if(pPWMC_Driver1->ChannelValidLevel[0] == 0)
			pPWMC_Driver1->pPWM_channel1->PWMC_CMR ^= AT91C_PWMC_CPOL;
		if(pPWMC_Driver1->ChannelValidLevel[1] == 0)
			pPWMC_Driver1->pPWM_channel2->PWMC_CMR ^= AT91C_PWMC_CPOL;
	}
	else
		ret = False;
	if(pPWMC_Driver2 != NULL)
	{
		ConfigSensor(&pPWMC_Driver2->Origin);
		ConfigSensor(&pPWMC_Driver2->Limit);
		ConfigControlIO(&pPWMC_Driver2->ControlIO, True);
		ConfigSensor(&pPWMC_Driver2->Fault);
		ConfigSensor(&pPWMC_Driver2->OverTemp);
		if(pPWMC_Driver2->ChannelValidLevel[0] == 0)
			pPWMC_Driver2->pPWM_channel1->PWMC_CMR ^= AT91C_PWMC_CPOL;
		if(pPWMC_Driver2->ChannelValidLevel[1] == 0)
			pPWMC_Driver2->pPWM_channel2->PWMC_CMR ^= AT91C_PWMC_CPOL;
	}
	else
		ret = False;
	
#ifndef VM_PRINT
	IRQ_ConfigureIT(AT91C_ID_PWMC, IRQ_PRI_PWM, PWMHandler);
	IRQ_EnableIT(AT91C_ID_PWMC);
#endif
	
	return ret;
}

typedef void (*pTCChannelHandle)(struct AxisParam_PulseDir_OpenLoop * pDCPWM);
void TCHandler_dimmy(struct AxisParam_PulseDir_OpenLoop * pDCPWM)
{
}
pTCChannelHandle pTC1Handle, pTC2Handle; 

INT8U InitTCChannel()
{
	Pin MotionTCPin_1_A = PIN_STEPPER1_PULSE;
	Pin MotionTCPin_1_A_Init = PIN_STEPPER1_PULSE_INIT;
	Pin MotionTCPin_1_B = PIN_STEPPER1_DIR;
	Pin MotionTCPin_2_A = PIN_STEPPER2_PULSE;
	Pin MotionTCPin_2_A_Init = PIN_STEPPER2_PULSE_INIT;
	Pin MotionTCPin_2_B = PIN_STEPPER2_DIR;
	INT8U ret = True;
	
	PMC_EnablePeripheral(AT91C_ID_TC);
	
	AT91C_BASE_TCB1->TCB_BMR &= ~AT91C_TCB_TC0XC0S;
	AT91C_BASE_TCB1->TCB_BMR |= AT91C_TCB_TC0XC0S_NONE;
	AT91C_BASE_TCB1->TCB_BMR &= ~AT91C_TCB_TC1XC1S;
	AT91C_BASE_TCB1->TCB_BMR |= AT91C_TCB_TC1XC1S_NONE;
	
	AT91C_BASE_TC3->TC_CCR = AT91C_TC_CLKDIS;
	AT91C_BASE_TC3->TC_IDR = AT91C_TC_COVFS | AT91C_TC_LOVRS | AT91C_TC_CPAS | AT91C_TC_CPBS |
		AT91C_TC_CPCS | AT91C_TC_LDRAS | AT91C_TC_LDRBS | AT91C_TC_ETRGS;
	AT91C_BASE_TC3->TC_SR;
	AT91C_BASE_TC3->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
		AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
			AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	
	AT91C_BASE_TC4->TC_CCR = AT91C_TC_CLKDIS;
	AT91C_BASE_TC4->TC_IDR = AT91C_TC_COVFS | AT91C_TC_LOVRS | AT91C_TC_CPAS | AT91C_TC_CPBS |
		AT91C_TC_CPCS | AT91C_TC_LDRAS | AT91C_TC_LDRBS | AT91C_TC_ETRGS;
	AT91C_BASE_TC4->TC_SR;
	AT91C_BASE_TC4->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
		AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
			AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
#ifdef VM_PRINT
	AT91C_BASE_TC5->TC_CCR = AT91C_TC_CLKDIS;
	AT91C_BASE_TC5->TC_IDR = AT91C_TC_COVFS | AT91C_TC_LOVRS | AT91C_TC_CPAS | AT91C_TC_CPBS |
		AT91C_TC_CPCS | AT91C_TC_LDRAS | AT91C_TC_LDRBS | AT91C_TC_ETRGS;
	AT91C_BASE_TC5->TC_SR;
	AT91C_BASE_TC5->TC_CMR = AT91C_TC_CLKS_TIMER_DIV1_CLOCK | AT91C_TC_EEVT_XC0 | 
		AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_BCPB_NONE | AT91C_TC_BCPC_NONE |
			AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE;
	
	AT91C_BASE_TC5->TC_RA = ((BOARD_MCK/2)/(20*1000))/2 +0.5; //20KHz
	AT91C_BASE_TC5->TC_RC = AT91C_BASE_TC5->TC_RA*2;
	AT91C_BASE_TC5->TC_RB = AT91C_BASE_TC5->TC_RA*2;
#endif
	
	pTC1Handle = TCHandler_dimmy;
	pTC2Handle = TCHandler_dimmy; 
	
	if(pTC_Driver1 != NULL)
	{
		if(!pTC_Driver1->ChannelValidLevel[0])
			AT91C_BASE_TC3->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
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
#if defined (EPSON_BOTTOM_BOARD_V3)&&(defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)) 
	if(pTC_Driver2 != NULL && GetCleanPartCount() == 2)
#else   
		if(pTC_Driver2 != NULL)
#endif
		{
			if(!pTC_Driver2->ChannelValidLevel[0])
				AT91C_BASE_TC4->TC_CMR = TC_TIMER_CLOCK_SRC | AT91C_TC_EEVT_XC0 | 
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
	
#ifdef VM_PRINT
	IRQ_ConfigureIT(AT91C_ID_TC, IRQ_PRI_TC, PWMHandler);
	AT91C_BASE_TC5->TC_SR;
	AT91C_BASE_TC5->TC_CCR = AT91C_TC_CLKEN|AT91C_TC_SWTRG;
#else
	IRQ_ConfigureIT(AT91C_ID_TC, IRQ_PRI_TC, TCHandler);
#endif
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
		struct AxisParam_DC_PWM * pDCPWM = NULL;
		struct AxisParam_PulseDir_OpenLoop * pStepperTC = NULL;
		struct EPR_MOTION_AXIS_PARAM * pAxisParam = &motionParam.axis_param[i];
		float ratio;
		
		if(pAxisParam->AxisID == AXIS_ID_X)
		{
			if( pAxisParam->motorType != MT_DC || 
			   pAxisParam->controlType != CT_CLOSE_LOOP_MOTOR ||
				   pAxisParam->driveType != DT_PWM_Speed ||
					   pAxisParam->PWM_ChannelID >= 2 )
				return False;
			else
				pDCPWM = &XParam;
		}
		else if(pAxisParam->AxisID == AXIS_ID_Y)
		{
			if( pAxisParam->motorType != MT_DC || 
			   (pAxisParam->controlType != CT_CLOSE_LOOP && 
				pAxisParam->controlType != CT_CLOSE_LOOP_MOTOR 
					//&& pAxisParam->controlType != CT_DOUBLE_CLOSE_LOOP
					)||
				   pAxisParam->driveType != DT_PWM_Speed_Position ||
					   pAxisParam->PWM_ChannelID >= 2 )
				return False;
			else
				pDCPWM = &YParam;
		}
		else if(pAxisParam->AxisID == AXIS_ID_Z)
		{
#if defined (EPSON_BOTTOM_BOARD_V3)&&(defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)) 
			if(GetCleanPartCount() == 1)
				continue;
#endif 
			if( pAxisParam->motorType != MT_Stepper || 
			   pAxisParam->controlType != CT_OPEN_LOOP ||
				   pAxisParam->driveType != DT_PulseAndDir ||
					   (pAxisParam->PWM_ChannelID < 2 || pAxisParam->PWM_ChannelID >= 4) )
				return False;
			else
				pStepperTC = &ZParam;
		}
		else if(pAxisParam->AxisID == AXIS_ID_Cleaner)
		{
			if( pAxisParam->motorType != MT_Stepper || 
			   pAxisParam->controlType != CT_OPEN_LOOP ||
				   pAxisParam->driveType != DT_PulseAndDir ||
					   (pAxisParam->PWM_ChannelID < 2 || pAxisParam->PWM_ChannelID >= 4) )
				return False;
			else
				pStepperTC = &CParam;
		}
		else
			return False;
		
		if(pDCPWM != NULL)
		{
			memset(pDCPWM, 0, sizeof(struct AxisParam_DC_PWM));
			pDCPWM->AxisID = pAxisParam->AxisID;
			pDCPWM->ChannelNO = pAxisParam->PWM_ChannelID;
			
			if(pAxisParam->controlType == CT_CLOSE_LOOP_MOTOR)
			{
				if(pAxisParam->AxisID == AXIS_ID_X)
					pDCPWM->CoorRegAddr = EPSON_REGADDR_X_MOTOR_COOR;
				else
					pDCPWM->CoorRegAddr = EPSON_REGADDR_Y_MOTOR_COOR;
			}
			else if(pAxisParam->controlType == CT_CLOSE_LOOP)
			{
				if(pAxisParam->AxisID == AXIS_ID_X)
					pDCPWM->CoorRegAddr = EPSON_REGADDR_X_PRT_COOR;
				else
					//pDCPWM->CoorRegAddr = EPSON_REGADDR_Y_FEEDBACK_COOR;
					pDCPWM->CoorRegAddr = EPSON_REGADDR_Y_MOTOR_COOR;
			}
			else //CT_DOUBLE_CLOSE_LOOP
			{
				if(pAxisParam->AxisID == AXIS_ID_X)
				{
					pDCPWM->CoorRegAddr = EPSON_REGADDR_X_MOTOR_COOR;
					pDCPWM->CoorRegAddr_DoubleFeedback = EPSON_REGADDR_X_PRT_COOR;
				}
				else
				{ 
					//pDCPWM->CoorRegAddr = EPSON_REGADDR_Y_MOTOR_COOR;
					//pDCPWM->CoorRegAddr_DoubleFeedback = EPSON_REGADDR_Y_FEEDBACK_COOR;
					return False;
				}
			}
			
			if(!ConfigSensorPin(&(pDCPWM->Origin), pAxisParam, MSID_ORIGIN))
			{
				if(pAxisParam->AxisID == AXIS_ID_X)
					return False;
			}
			
			ConfigSensorPin(&(pDCPWM->Limit), pAxisParam, MSID_LIMIT);
			
			if(pAxisParam->controlType == CT_CLOSE_LOOP)
			{
				ratio = pAxisParam->transmisstion_ratio_close_loop[0];
				pDCPWM->ratio_move = ratio; 
				pDCPWM->ratio_print[0] = ratio; 
				pDCPWM->ratio_print[1] = pAxisParam->transmisstion_ratio_close_loop[1]; 
				pDCPWM->ratio_print[2] = pAxisParam->transmisstion_ratio_close_loop[2]; 
			}
			else
			{
				ratio = pAxisParam->transmisstion_ratio_open_loop;
				pDCPWM->ratio_move = ratio; 
				pDCPWM->ratio_print[0] = ratio; 
				pDCPWM->ratio_print[1] = ratio * pAxisParam->transmisstion_ratio_close_loop[0] / pAxisParam->transmisstion_ratio_close_loop[1]; 
				pDCPWM->ratio_print[2] = ratio * pAxisParam->transmisstion_ratio_close_loop[0] / pAxisParam->transmisstion_ratio_close_loop[2]; 
			}
			
			if(pAxisParam->AxisID == AXIS_ID_X)
			{
				pDCPWM->Origin_Offset = XORIGIN_OFFSET_MOTOR;
				pDCPWM->Origin_Offset_Feedback = XORIGIN_OFFSET;
			}
			else
			{
				pDCPWM->Origin_Offset_Feedback = pDCPWM->Origin_Offset = 0;
			}
			if(!(pAxisParam->option & SO_FIXED_REGION) && pDCPWM->Limit.IsValid)
			{
				pDCPWM->IsCheckTerm = True;
				pDCPWM->Region_End = (INT32S)(0x7FFFFFFF);
			}
			else
			{
				pDCPWM->IsCheckTerm = False;
				pDCPWM->Region_End = (INT32S)(0x7FFFFFFF);
				if( pAxisParam->option & SO_FIXED_REGION)
				{
					pDCPWM->Region_End = (INT32S)(ratio * pAxisParam->Fixed_MoveRegion_EndPoint);
					pDCPWM->Region_End += pDCPWM->Origin_Offset;
				}
			}
			pDCPWM->OriginSensorOffset = (INT32S)(ratio * pAxisParam->OriginOffset);
			if( pAxisParam->option & SO_FIXED_REGION)
			{
				pDCPWM->Region_Start = (INT32S)(ratio * pAxisParam->Fixed_MoveRegion_StartPoint);
				pDCPWM->Region_Start += pDCPWM->Origin_Offset;
			}
			else
			{
				if(pAxisParam->Fixed_MoveRegion_StartPoint != 0xFFFFFFFF &&
				   pAxisParam->Fixed_MoveRegion_StartPoint != 0x80000000 &&
					   pAxisParam->Fixed_MoveRegion_StartPoint != 0x80000001 )
				{
					pDCPWM->Region_Start = (INT32S)(ratio * pAxisParam->Fixed_MoveRegion_StartPoint);
					pDCPWM->Region_Start += pDCPWM->Origin_Offset;
				}   
				else
					pDCPWM->Region_Start = (INT32S)(0x80000001);
			}
			
			if( pDCPWM->ChannelNO == 0)
			{
				pPWMC_Driver1 = pDCPWM;
				pDCPWM->pPWM_channel1 = AT91C_BASE_PWMC_CH0;
				pDCPWM->pPWM_channel2 = AT91C_BASE_PWMC_CH1;
				pDCPWM->Channel_Mask = (AT91C_PWMC_CHID0|AT91C_PWMC_CHID1);
			}
			else
			{
				pPWMC_Driver2 = pDCPWM;
				pDCPWM->pPWM_channel1 = AT91C_BASE_PWMC_CH2;
				pDCPWM->pPWM_channel2 = AT91C_BASE_PWMC_CH3;
				pDCPWM->Channel_Mask = (AT91C_PWMC_CHID2|AT91C_PWMC_CHID3);
			}
			if( pAxisParam->ControlIOPolarityMask & (1<<MCID_PWM1))
				pDCPWM->ChannelValidLevel[0] = True;
			else
				pDCPWM->ChannelValidLevel[0] = False;
			if( pAxisParam->ControlIOPolarityMask & (1<<MCID_PWM2))
				pDCPWM->ChannelValidLevel[1] = True;
			else
				pDCPWM->ChannelValidLevel[1] = False;
			if( pAxisParam->ControlIOPolarityMask & (1<<MCID_Ctrl0))
				pDCPWM->ControlIO.ValidLevel = True;
			else
				pDCPWM->ControlIO.ValidLevel = False;
			if(!ConfigCtrlPin(&pDCPWM->ControlIO, pAxisParam, MCID_Ctrl0))
				return False;
			ConfigSensorPin(&(pDCPWM->Fault), pAxisParam, MSID_FAULT);
			ConfigSensorPin(&(pDCPWM->OverTemp), pAxisParam, MSID_OVERTEMP);
			
			if(pAxisParam->AxisID == AXIS_ID_X)
			{
				pDCPWM->BaseDPI = 720;
				pDCPWM->CurrDPI = 720;
			}
			else
			{
				pDCPWM->BaseDPI = 1;
				pDCPWM->CurrDPI = 1;
			}
			if(!BuildPWMRatioTable(pDCPWM->ratioTable_StepPerCycle, pDCPWM, 
								   pAxisParam, PWM_MOVE_CYCLE, pAxisParam->validSpeedLevel))
				return False;
			if(!BuildAllPWMCurve(pDCPWM, pAxisParam, PWM_MOVE_CYCLE)) //PWMCycleTick based on MCK(133Mhz).
			{
				status_ReportStatus(STATUS_FTA + SciError_Parameter, STATUS_SET);
				return False;
			}
			if(!BuildAllPID((INT32U*)pDCPWM->PIDTable, pDCPWM, pAxisParam))
				return False;
			pDCPWM->PWM_modelTable[0] = pAxisParam->PWM_Model[0];
			pDCPWM->PWM_modelTable[1] = pAxisParam->PWM_Model[1];
			pDCPWM->PWM_modelTable[2] = pAxisParam->PWM_Model[2];
			
			pDCPWM->CoorIdeal = pDCPWM->Origin_Offset;
			pDCPWM->Coor_nonRollback = pDCPWM->CoorIdeal;
			if(pAxisParam->AxisID == AXIS_ID_X)
				pDCPWM->PermitStopError = PWM_X_PERMIT_ERROR;
			else
				pDCPWM->PermitStopError = PWM_Y_PERMIT_ERROR;
			pDCPWM->Coor_NextCycle = pDCPWM->CoorIdeal;
			pDCPWM->Coor_NextCycle_Frac = 0;
			pDCPWM->ratio_StepPerCycle = 0;
			
			pDCPWM->Dir = DIRTYPE_STANDBY;
			pDCPWM->MoveType = MOVETYPE_CONTINUE; 
			pDCPWM->PWM_model = pDCPWM->PWM_modelTable[0] ; //based on driver CHIP.
			pDCPWM->AccSegment = ACCSEGTYPE_SPEED_UP; //0: constant, 1: speed up, 2: speed down.
			
			pDCPWM->curPWMTick = 0;
			pDCPWM->curPWMTickInAccSegment = 0;
			
			pDCPWM->speedDownCoor = pDCPWM->Origin_Offset;
			pDCPWM->speedDownIndex = 0;
			
			pDCPWM->pAccTable = NULL;
			pDCPWM->AccTableLen = 0;
			pDCPWM->AccTableIndex = 0;
			
			//PIDREG3 part
			pDCPWM->e = 0;
			pDCPWM->e1 = 0;
			pDCPWM->e2 = 0;
			pDCPWM->Kp = 0;
			pDCPWM->Ki = 0;
			pDCPWM->Kd = 0;
			pDCPWM->fixpoint_bit = 0;
			pDCPWM->pid_out_max = 0;
			pDCPWM->pid_out_min = 0;
			pDCPWM->pid_out = 0;
			pDCPWM->pid_out_frac = 0;
			pDCPWM->pid_i_out_frac = 0;
			memset(pDCPWM->e_history, 0, ERROR_HISTORY_LEN*sizeof(int));
			pDCPWM->e_i = 0;
			
			pDCPWM->PWM_CycleTime = PWM_STANDBY_CYCLE/2;
			pDCPWM->PWM_MaxValue = PWM_STANDBY_CYCLE/2 -50;
			pDCPWM->PWM_MinValue = 50; 
			pDCPWM->PWM_CurValue = 0;
			pDCPWM->PWM_OverLoadNum = 0;
			
			pDCPWM->PWM_MaxOverLoadNum = PWM_MAX_LOAD_NUM;//200
			pDCPWM->PWM_DeadTime = PWM_DEAD_TIME; 
			pDCPWM->Min_Distance = pDCPWM->PermitStopError;
			
			pDCPWM->lastCoor = 0;
			
			pDCPWM->pAxisParam = pAxisParam;
		}
		else
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
			if(pStepperTC->ChannelNO == 2)
			{
				pTC_Driver1 = pStepperTC;
				pStepperTC->pTC_channel = AT91C_BASE_TC3;
			}
			else
			{
				pTC_Driver2 = pStepperTC;
				pStepperTC->pTC_channel = AT91C_BASE_TC4;
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
	
	ret &= InitPWMChannel();
	ret &= InitTCChannel();
	
	mainStatus = MS_INIT;
	InitStatus = IS_IDLE;
	
	if(ret)
	{
		INT32S max_accSpace = 0;
		printer.platSpace = XParam.pAxisParam->Fixed_Position[MPID_FLATSTART];
		for(i = 0; i < MAX_X_SPEED_LEVEL - 3; i++) //only consider the acc distance of X print.
		{
			if( i%5 == 4)
				continue;
			if( max_accSpace < AccCurveIndexTbl.pAllCurveX[i]->AccDistance)
				max_accSpace = AccCurveIndexTbl.pAllCurveX[i]->AccDistance;
		}
		printer.accSpace = max_accSpace/XParam.ratio_move + X_PRINT_ACC_SAFE_DISTANCE;
		if(printer.accSpace > printer.platSpace)
			printer.accSpace = printer.platSpace;
		printer.orgLimit = XParam.pAxisParam->Fixed_Position[MPID_START];
		printer.org_oppLimit = XParam.pAxisParam->Fixed_Position[MPID_MOVEEND];
	}
	
	return ret;
}

INT8U reportMoveCompleted(INT8U ChannelNO)
{
	INT8U err;
	INT32U flag = (MOTION_MOVE_COMPLETED_X << (ChannelNO-1));
	return flag == OSFlagPost(mix_FLAG_GRP, flag, OS_FLAG_SET, &err); 
}

INT8U reportMotionError(struct AxisParam_DC_PWM * pDCPWM, enum MotionError error)
{
	switch(error)
	{
	case ME_DC_X_OVERLOAD:
		status_ReportStatus(STATUS_FTA + SciError_PaperJamX, STATUS_SET);
		break;
	case ME_DC_Y_OVERLOAD:
#ifndef MOTION_Y_DSP			
		status_ReportStatus(STATUS_FTA + SciError_PaperJamY, STATUS_SET);
#endif
		break;
	case ME_DC_OVER_CURRENT_OR_UNDER_VOL:
	case ME_DC_OVER_BOTH:
		status_ReportStatus(STATUS_FTA + SciError_OverCurrent_Protect, STATUS_SET);
		break;
	case ME_DC_OVER_TMEP:
		status_ReportStatus(STATUS_FTA + SciError_OverTemp_Protect, STATUS_SET);
		break;
	default:
		return False;
	}
	return True;
}

INT8U PWM_SafeCheck(struct AxisParam_DC_PWM * pDCPWM)
{
	INT8U overTemp = GetSensor(&pDCPWM->OverTemp);
	INT8U overCurr = GetSensor(&pDCPWM->Fault);
	enum MotionError cur = ME_OK;
	
	if(overCurr)
	{
		if(overTemp)
			cur = ME_DC_OVER_BOTH;
		else
			cur = ME_DC_OVER_CURRENT_OR_UNDER_VOL;
	}
	else if(overTemp)
	{
		cur = ME_DC_OVER_TMEP;
	}
	
	if(cur == ME_OK)
		return True;
	else
	{
		OS_CPU_SR cpu_sr;
		if(MotionError_Main == ME_OK)
			MotionError_Main = cur;
		OS_ENTER_CRITICAL();
		if( pDCPWM->ControlIO.ValidLevel)
			PIO_Set(&pDCPWM->ControlIO.PinIO);
		else
			PIO_Clear(&pDCPWM->ControlIO.PinIO);
		OS_EXIT_CRITICAL();
		
		reportMotionError(pDCPWM, MotionError_Main);
		return False;
	}
}

inline void PWMHandler_GetCoor(INT32U * curCh1Coor, INT32U * curCh2Coor) FAST_FUNC_SECTION
{
	OS_CPU_SR cpu_sr;
	INT32U LAST_COOR[2] = {0};
	
	//  OS_ENTER_CRITICAL();
	if(pPWMC_Driver1 != NULL)
	{
#if defined(HEAD_EPSON_GEN5)  
		do
		{
			rFPGA_EPSON_RegAddr = pPWMC_Driver1->CoorRegAddr;
		}
		while(rFPGA_EPSON_RegAddr != pPWMC_Driver1->CoorRegAddr);
#endif
		if(pPWMC_Driver1->AxisID == AXIS_ID_Y)
		{
			INT32U curr = 0;
#if defined(HEAD_EPSON_GEN5)       
			do
			{
				LAST_COOR[0] = (INT32U)rFPGA_EPSON_RegPortL;
				curr = (INT32U)rFPGA_EPSON_RegPortL;
			}
			while(curr != LAST_COOR[0]);
			if( (INT16U)pPWMC_Driver1->lastCoor < 0x4000 && curr > 0xC000)
				pPWMC_Driver1->lastCoor -= 0x10000;
			else if((INT16U)pPWMC_Driver1->lastCoor > 0xC000 && curr < 0x4000)
				pPWMC_Driver1->lastCoor += 0x10000;
			pPWMC_Driver1->lastCoor &= 0xFFFF0000;
			pPWMC_Driver1->lastCoor |= curr;
			*curCh1Coor = pPWMC_Driver1->lastCoor;
#elif defined(HEAD_RICOH_G4) 
			curr =(INT32U)(rFPGA_RICOH_YMOTORCOOR_L | (rFPGA_RICOH_YMOTORCOOR_H)<<16);
			*curCh1Coor = curr;
#endif
		}
		else
		{
#if defined(HEAD_EPSON_GEN5)       
			do
			{
				LAST_COOR[0] = (INT32U)rFPGA_EPSON_RegPort;
				*curCh1Coor = (INT32U)rFPGA_EPSON_RegPort;
			}
			while(*curCh1Coor != LAST_COOR[0]);
#elif defined(HEAD_RICOH_G4)
			if(pPWMC_Driver1->CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
			{
				*curCh1Coor =(INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
			}
			else if(pPWMC_Driver1->CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
			{
				*curCh1Coor = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
			}
#endif
		}
	}
	else
		*curCh1Coor = 0;
	if(pPWMC_Driver2 != NULL)
	{
#if defined(HEAD_EPSON_GEN5)
		do{
			rFPGA_EPSON_RegAddr = pPWMC_Driver2->CoorRegAddr;
		}while(rFPGA_EPSON_RegAddr != pPWMC_Driver2->CoorRegAddr);
#endif
		if(pPWMC_Driver2->AxisID == AXIS_ID_Y)
		{
#if defined(HEAD_EPSON_GEN5)      
			INT32U curr = 0;
			do
			{
				LAST_COOR[1] = (INT32U)rFPGA_EPSON_RegPortL;
				curr = (INT32U)rFPGA_EPSON_RegPortL;
			}
			while(curr != LAST_COOR[1]);
			if( (INT16U)pPWMC_Driver2->lastCoor < 0x4000 && curr > 0xC000)
				pPWMC_Driver2->lastCoor -= 0x10000;
			else if((INT16U)pPWMC_Driver2->lastCoor > 0xC000 && curr < 0x4000)
				pPWMC_Driver2->lastCoor += 0x10000;
			pPWMC_Driver2->lastCoor &= 0xFFFF0000;
			pPWMC_Driver2->lastCoor |= curr;
			*curCh2Coor = pPWMC_Driver2->lastCoor;
#elif defined(HEAD_RICOH_G4) 
			INT32U curr =(INT32U)(rFPGA_RICOH_YMOTORCOOR_L | (rFPGA_RICOH_YMOTORCOOR_H)<<16);
			*curCh2Coor = curr;
#endif
		}
		else
		{
#if defined(HEAD_EPSON_GEN5)       
			do
			{
				LAST_COOR[1] = rFPGA_EPSON_RegPort;
				*curCh2Coor = rFPGA_EPSON_RegPort;
			}
			while(*curCh2Coor != LAST_COOR[1]);
#elif defined(HEAD_RICOH_G4)
			if(pPWMC_Driver2->CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
			{
				*curCh2Coor =(INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
			}
			else if(pPWMC_Driver2->CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
			{
				*curCh2Coor = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
			}
#endif
		}
	}
	else
		*curCh2Coor = 0;
	//OS_EXIT_CRITICAL();
}

//for not record case,  it will spend 0x500~0x600 clock for standby adjustment. 1/4 loading of CPU time.
//for record case. it will spend 0x800~0x900 clock for standby adjustment. 1/3 loading of CPU time .
// NOTE: if PWM freq is 20Khz, the PWM cycle is 6666 clock, about 0x1A0A.
INT32U LAST_COOR[2] = {0xFFFFFFFF, 0xFFFFFFFF};
void PWMHandler(void) FAST_FUNC_SECTION
{ 
	INT32U curCh1Coor, curCh2Coor;
	INT32U isr = AT91C_BASE_PWMC->PWMC_ISR & AT91C_BASE_PWMC->PWMC_IMR & AT91C_BASE_PWMC->PWMC_SR;
	
	PWMHandler_GetCoor(&curCh1Coor, &curCh2Coor);
	
	if(LAST_COOR[0]!=0xFFFFFFFF && (absv(LAST_COOR[0] - curCh1Coor) > 1000))
		curCh1Coor = pPWMC_Driver1->Coor_NextCycle - pPWMC_Driver1->e;
	else
		LAST_COOR[0] = curCh1Coor;
	
	if(LAST_COOR[1]!=0xFFFFFFFF && (absv(LAST_COOR[1] - curCh2Coor) > 1000))
		curCh2Coor = pPWMC_Driver2->Coor_NextCycle - pPWMC_Driver2->e;
	else
		LAST_COOR[1] = curCh2Coor;	 
	
#ifdef VM_PRINT
	isr = AT91C_BASE_TC5->TC_SR & AT91C_BASE_TC5->TC_IMR;
	(*pPWM1Handle)(pPWMC_Driver1, curCh1Coor);
#else
	if(isr & (AT91C_PWMC_CHID0|AT91C_PWMC_CHID1))
		(*pPWM1Handle)(pPWMC_Driver1, curCh1Coor);
	if(isr & (AT91C_PWMC_CHID2|AT91C_PWMC_CHID3))
		(*pPWM2Handle)(pPWMC_Driver2, curCh2Coor);
#endif
}

INT8U ExitResetStatus()
{
	if(pPWMC_Driver1 != NULL)
	{
		if(!pPWMC_Driver1->ControlIO.ValidLevel)
			PIO_Set(&pPWMC_Driver1->ControlIO.PinIO);
		else
			PIO_Clear(&pPWMC_Driver1->ControlIO.PinIO);
	}
	if(pPWMC_Driver2 != NULL)
	{
		if(!pPWMC_Driver2->ControlIO.ValidLevel)
			PIO_Set(&pPWMC_Driver2->ControlIO.PinIO);
		else
			PIO_Clear(&pPWMC_Driver2->ControlIO.PinIO);
	}
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

INT8U PWM_Urgent_Stop(struct AxisParam_DC_PWM * pDCPWM)
{
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();
	if( pDCPWM->ControlIO.ValidLevel)
		PIO_Set(&pDCPWM->ControlIO.PinIO);
	else
		PIO_Clear(&pDCPWM->ControlIO.PinIO);
	
	pDCPWM->PWM_CycleTime = PWM_STANDBY_CYCLE/2;
	
#ifndef VM_PRINT
	pDCPWM->pPWM_channel1->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
	if(pDCPWM->ChannelValidLevel[0] == 0)
		pDCPWM->pPWM_channel1->PWMC_CMR ^= AT91C_PWMC_CPOL;
	pDCPWM->pPWM_channel2->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
	if(pDCPWM->ChannelValidLevel[1] == 0)
		pDCPWM->pPWM_channel2->PWMC_CMR ^= AT91C_PWMC_CPOL;
	pDCPWM->pPWM_channel1->PWMC_CPRDR = pDCPWM->PWM_CycleTime;
	pDCPWM->pPWM_channel2->PWMC_CPRDR = pDCPWM->PWM_CycleTime;
	pDCPWM->pPWM_channel1->PWMC_CDTYR = pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
	pDCPWM->pPWM_channel2->PWMC_CDTYR = pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
#endif
	if( pDCPWM->ChannelNO == 0)
		pPWM1Handle = PWMHandler_dimmy;
	else
		pPWM2Handle = PWMHandler_dimmy;
#ifdef VM_PRINT
	VM_PWM_STOP();
	AT91C_BASE_TC5->TC_IDR = AT91C_TC_CPCS;
#else
	AT91C_BASE_PWMC->PWMC_IDR = pDCPWM->Channel_Mask;
	AT91C_BASE_PWMC->PWMC_DIS = pDCPWM->Channel_Mask;
#endif
	
#ifdef   HEAD_EPSON_GEN5
	SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, ER_CoorCtrl_SMOOTH_DIVIDER_720 | ER_CoorCtrl_SMOOTH_MULTI_720);
#else
	rFPGA_RICOH_COORCTRL_L = ER_CoorCtrl_SMOOTH_DIVIDER_720 |ER_CoorCtrl_SMOOTH_MULTI_720;
#endif
	pDCPWM->BaseDPI = 720;
	pDCPWM->CurrDPI = 720;
	
	//pDCPWM->CoorIdeal
	pDCPWM->Coor_NextCycle = pDCPWM->CoorIdeal;
	pDCPWM->Coor_NextCycle_Frac = 0;
	pDCPWM->ratio_StepPerCycle = 0;
	
	pDCPWM->Dir = DIRTYPE_STANDBY;
	pDCPWM->MoveType = MOVETYPE_CONTINUE;
	pDCPWM->AccSegment = ACCSEGTYPE_CONSTANT_SPEED;
	
	pDCPWM->curPWMTick = 0;
	pDCPWM->curPWMTickInAccSegment = 0;
	
	pDCPWM->speedDownCoor = 0;
	pDCPWM->speedDownIndex = 0;
	
	pDCPWM->pAccTable = NULL;
	pDCPWM->AccTableLen = 0;
	pDCPWM->AccTableIndex = 0;
	
	pDCPWM->PWM_MaxValue = PWM_STANDBY_CYCLE/2 -50;
	pDCPWM->PWM_MinValue = 50; 
	pDCPWM->PWM_CurValue = 0;
	pDCPWM->PWM_OverLoadNum = 0;
	
	pDCPWM->e = 0;
	pDCPWM->e1 = 0;
	pDCPWM->e2 = 0;
	pDCPWM->Kp = pDCPWM->PIDTable[0][0];
	pDCPWM->Ki = pDCPWM->PIDTable[0][1];
	pDCPWM->Kd = pDCPWM->PIDTable[0][2];
	pDCPWM->fixpoint_bit = PID_SHIFT_BIT; 
	pDCPWM->pid_out_max = pDCPWM->PWM_CycleTime/2;
	pDCPWM->pid_out_min = -(pDCPWM->PWM_CycleTime/2);
	pDCPWM->pid_out = 0;
	pDCPWM->pid_out_frac = 0;
	pDCPWM->pid_i_out_frac = 0;
	memset(pDCPWM->e_history, 0, ERROR_HISTORY_LEN*sizeof(int));
	pDCPWM->e_i = 0;
	
	OS_EXIT_CRITICAL();
	return True;
}


INT8U calcPID(struct AxisParam_DC_PWM * pDCPWM) FAST_FUNC_SECTION
{
#define v  pDCPWM
	int out, out_frac;
	int e_speed = (v->e - v->e1);
	
	int center_i = v->e_i - ERROR_HISTORY_LEN/2;
	if(center_i< 0 )
		center_i  += ERROR_HISTORY_LEN;
	
	out = v->Ki * v->e + v->Kd * (v->e + v->e_history[v->e_i] - 2*v->e_history[center_i]) + v->pid_i_out_frac;
	v->pid_i_out_frac = out & ((1<< PID_I_SHIFT_BIT)-1);
	out >>= PID_I_SHIFT_BIT;
	
	out += v->Kp * e_speed + v->pid_out_frac;
	out_frac = out;
	out >>= v->fixpoint_bit;
	out_frac = out_frac - (out << (v->fixpoint_bit));
	
	if (out > v->pid_out_max)
	{
		out =  v->pid_out_max;
		out_frac = 0;
	}
	else if (out < v->pid_out_min)
	{
		out =  v->pid_out_min;
		out_frac = 0;
	}
	
	v->pid_out = out;
	v->pid_out_frac = out_frac;
	
	v->e2 = e_speed;
	v->e1 = v->e;
	
	v->e_history[v->e_i] = v->e;
	v->e_i ++;
	if(v->e_i >= ERROR_HISTORY_LEN)
		v->e_i = 0;
	
#undef v
	
	pDCPWM->PWM_CurValue = (INT32S)pDCPWM->PWM_CurValue + pDCPWM->pid_out;
	
	if(pDCPWM->PWM_CurValue > pDCPWM->PWM_MaxValue || pDCPWM->PWM_CurValue < pDCPWM->PWM_MinValue)
	{
		if( pDCPWM->PWM_CurValue > pDCPWM->PWM_MaxValue)
		{
			pDCPWM->PWM_CurValue = pDCPWM->PWM_MaxValue;
			pDCPWM->PWM_OverLoadNum ++;
		}
		else
		{
			pDCPWM->PWM_CurValue = pDCPWM->PWM_MinValue;
			if(pDCPWM->PWM_model == PWMTYPE_BI_POLAR)
				pDCPWM->PWM_OverLoadNum ++;
			else
				pDCPWM->PWM_OverLoadNum = 0;
		}
		if( pDCPWM->PWM_OverLoadNum > pDCPWM->PWM_MaxOverLoadNum)
		{
			PWM_Urgent_Stop(pDCPWM);
			reportMotionError(pDCPWM, pDCPWM->ChannelNO== 0 ? ME_DC_X_OVERLOAD : ME_DC_Y_OVERLOAD);
			return False;
		}
	}
	else 
		pDCPWM->PWM_OverLoadNum = 0;
	return True;
}

void PWMHandler_Standby_UNI_POLAR(struct AxisParam_DC_PWM * pDCPWM, INT32U curCoor) FAST_FUNC_SECTION
{
#ifdef VM_PRINT
	curCoor = pDCPWM->Coor_NextCycle;
#else
	pDCPWM->e = (INT32S)(pDCPWM->Coor_NextCycle - curCoor);
	if( calcPID(pDCPWM))
	{
		//start status: CPOL = 0, CUPDR = 100%
		if( pDCPWM->PWM_CurValue >= 0)
		{
			pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime - pDCPWM->PWM_CurValue;
			pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
		}
		else
		{
			pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
			pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime + pDCPWM->PWM_CurValue;
		}
	}
#endif
	pDCPWM->curPWMTick ++;
	PutMoveRecord(pDCPWM->Dir, pDCPWM->AccSegment, pDCPWM->MoveType, (pDCPWM->AxisID == AXIS_ID_X), 
				  False, pDCPWM->PWM_CurValue, pDCPWM->Coor_NextCycle, curCoor);
}

void PWMHandler_Standby_BI_POLAR(struct AxisParam_DC_PWM * pDCPWM, INT32U curCoor) FAST_FUNC_SECTION
{
#ifdef VM_PRINT
	curCoor = pDCPWM->Coor_NextCycle;
#else
	pDCPWM->e = (INT32S)(pDCPWM->Coor_NextCycle - curCoor);
	if( calcPID(pDCPWM))
	{
		//start status: X1: CPOL = 0, CUPDR = (50% - deadtime); X1: CPOL = 1, CUPDR = (50% + deadtime).
		pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime - pDCPWM->PWM_CurValue;
		pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime - (pDCPWM->PWM_CurValue + pDCPWM->PWM_DeadTime);
	}
#endif
	pDCPWM->curPWMTick ++;
	PutMoveRecord(pDCPWM->Dir, pDCPWM->AccSegment, pDCPWM->MoveType, (pDCPWM->AxisID == AXIS_ID_X), 
				  False, pDCPWM->PWM_CurValue, pDCPWM->Coor_NextCycle, curCoor);
}

void PWM_Start_Standy(struct AxisParam_DC_PWM * pDCPWM, INT32U standbyPos, INT8U bKeepPIDErr, INT8U LastPWMModel)
{
	pDCPWM->CoorIdeal = standbyPos;
	pDCPWM->Coor_NextCycle = standbyPos;
	pDCPWM->Coor_NextCycle_Frac = 0;
	pDCPWM->ratio_StepPerCycle = 0;
	
	pDCPWM->Dir = DIRTYPE_STANDBY;
	pDCPWM->MoveType = MOVETYPE_CONTINUE;
	pDCPWM->AccSegment = ACCSEGTYPE_CONSTANT_SPEED;
	
	pDCPWM->curPWMTick = 0;
	pDCPWM->curPWMTickInAccSegment = 0;
	
	pDCPWM->speedDownCoor = 0;
	pDCPWM->speedDownIndex = 0;
	
	pDCPWM->pAccTable = NULL;
	pDCPWM->AccTableLen = 0;
	pDCPWM->AccTableIndex = 0;
	
	pDCPWM->PWM_CycleTime = PWM_STANDBY_CYCLE/2;
	pDCPWM->PWM_OverLoadNum = 0;
	switch(pDCPWM->PWM_model)
	{
	case PWMTYPE_UNI_POLAR:
		pDCPWM->PWM_MaxValue = (pDCPWM->PWM_CycleTime - 50);
		pDCPWM->PWM_MinValue = -(pDCPWM->PWM_CycleTime - 50); 
		if(bKeepPIDErr)
		{
			if(LastPWMModel == PWMTYPE_BI_POLAR)
			{
				pDCPWM->PWM_CurValue = (pDCPWM->PWM_CurValue - (pDCPWM->PWM_CycleTime - pDCPWM->PWM_DeadTime)/2) * 2;
				if(pDCPWM->PWM_CurValue > pDCPWM->PWM_MaxValue)
					pDCPWM->PWM_CurValue = pDCPWM->PWM_MaxValue;
				else if(pDCPWM->PWM_CurValue < pDCPWM->PWM_MinValue)
					pDCPWM->PWM_CurValue = pDCPWM->PWM_MinValue;
			}
		}
		else
			pDCPWM->PWM_CurValue = 0;
		break;
	default:
	case PWMTYPE_BI_POLAR:
		pDCPWM->PWM_MaxValue = (pDCPWM->PWM_CycleTime - 50);
		pDCPWM->PWM_MinValue = 50; 
		if(bKeepPIDErr)
		{
			if(LastPWMModel == PWMTYPE_UNI_POLAR)
			{
				pDCPWM->PWM_CurValue = pDCPWM->PWM_CurValue/2 + (pDCPWM->PWM_CycleTime - pDCPWM->PWM_DeadTime)/2;
				if(pDCPWM->PWM_CurValue > pDCPWM->PWM_MaxValue)
					pDCPWM->PWM_CurValue = pDCPWM->PWM_MaxValue;
				else if(pDCPWM->PWM_CurValue < pDCPWM->PWM_MinValue)
					pDCPWM->PWM_CurValue = pDCPWM->PWM_MinValue;
			}
		}
		else
			pDCPWM->PWM_CurValue = (pDCPWM->PWM_CycleTime - pDCPWM->PWM_DeadTime)/2;
		break;
	}
	
	//PIDREG3 part
	if(!bKeepPIDErr)
	{
		pDCPWM->e = 0;
		pDCPWM->e1 = 0;
		pDCPWM->e2 = 0;
		memset(pDCPWM->e_history, 0, ERROR_HISTORY_LEN*sizeof(int));
		pDCPWM->e_i = 0;
	}
	pDCPWM->Kp = pDCPWM->PIDTable[0][0];
	pDCPWM->Ki = pDCPWM->PIDTable[0][1];
	pDCPWM->Kd = pDCPWM->PIDTable[0][2];
	pDCPWM->fixpoint_bit = PID_SHIFT_BIT; 
	pDCPWM->pid_out_max = pDCPWM->PWM_CycleTime/2;
	pDCPWM->pid_out_min = -(pDCPWM->PWM_CycleTime/2);
	pDCPWM->pid_out = 0;
	pDCPWM->pid_out_frac = 0;
	
	//AT91C_BASE_PWMC->PWMC_MR = 0;
#ifndef VM_PRINT
	pDCPWM->pPWM_channel1->PWMC_CPRDR = pDCPWM->PWM_CycleTime;
	pDCPWM->pPWM_channel2->PWMC_CPRDR = pDCPWM->PWM_CycleTime;
	switch(pDCPWM->PWM_model)
	{
	case PWMTYPE_UNI_POLAR:
		pDCPWM->pPWM_channel1->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
		if(pDCPWM->ChannelValidLevel[0] == 0)
			pDCPWM->pPWM_channel1->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel2->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
		if(pDCPWM->ChannelValidLevel[1] == 0)
			pDCPWM->pPWM_channel2->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel1->PWMC_CDTYR = pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
		pDCPWM->pPWM_channel2->PWMC_CDTYR = pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
		if( pDCPWM->ChannelNO == 0)
			pPWM1Handle = PWMHandler_Standby_UNI_POLAR;
		else
			pPWM2Handle = PWMHandler_Standby_UNI_POLAR;
		AT91C_BASE_PWMC->PWMC_IER = pDCPWM->Channel_Mask;
		AT91C_BASE_PWMC->PWMC_ENA = pDCPWM->Channel_Mask;
		break;
	case PWMTYPE_BI_POLAR:
		pDCPWM->pPWM_channel1->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
		if(pDCPWM->ChannelValidLevel[0] == 0)
			pDCPWM->pPWM_channel1->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel2->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG | AT91C_PWMC_CPOL;
		if(pDCPWM->ChannelValidLevel[1] == 0)
			pDCPWM->pPWM_channel2->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel1->PWMC_CDTYR = pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime - pDCPWM->PWM_CurValue;
		pDCPWM->pPWM_channel2->PWMC_CDTYR = pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime - (pDCPWM->PWM_CurValue + pDCPWM->PWM_DeadTime);;
		if( pDCPWM->ChannelNO == 0)
			pPWM1Handle = PWMHandler_Standby_BI_POLAR;
		else
			pPWM2Handle = PWMHandler_Standby_BI_POLAR;
		AT91C_BASE_PWMC->PWMC_IER = pDCPWM->Channel_Mask;
		AT91C_BASE_PWMC->PWMC_ENA = pDCPWM->Channel_Mask;
		break;
	case PWMTYPE_STANDBY_LOWLOOP:
		pDCPWM->pPWM_channel1->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
		if(pDCPWM->ChannelValidLevel[0] == 0)
			pDCPWM->pPWM_channel1->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel2->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
		if(pDCPWM->ChannelValidLevel[1] == 0)
			pDCPWM->pPWM_channel2->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel1->PWMC_CDTYR = pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
		pDCPWM->pPWM_channel2->PWMC_CDTYR = pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
		if( pDCPWM->ChannelNO == 0)
			pPWM1Handle = PWMHandler_dimmy;
		else
			pPWM2Handle = PWMHandler_dimmy;
		AT91C_BASE_PWMC->PWMC_IDR = pDCPWM->Channel_Mask;
		AT91C_BASE_PWMC->PWMC_DIS = pDCPWM->Channel_Mask;
		break;
	}
#endif
}

INT8U PWM_Start_Standy_X(struct AxisParam_DC_PWM * pDCPWM, INT32U standbyPos, INT8U bKeepPIDErr)
{
	OS_CPU_SR cpu_sr;
	INT8U LastPWMModel;
	
	if(!PWM_SafeCheck(pDCPWM))
		return False;
	
	OS_ENTER_CRITICAL();
#ifdef   HEAD_EPSON_GEN5
	SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, ER_CoorCtrl_SMOOTH_DIVIDER_720 | ER_CoorCtrl_SMOOTH_MULTI_720);
#else
	rFPGA_RICOH_COORCTRL_L = ER_CoorCtrl_SMOOTH_DIVIDER_720 |ER_CoorCtrl_SMOOTH_MULTI_720;
#endif
	pDCPWM->BaseDPI = 720;
	pDCPWM->CurrDPI = 720;
	
	LastPWMModel = pDCPWM->PWM_model;
	pDCPWM->PWM_model = pDCPWM->PWM_modelTable[0] ;
	
	if( pDCPWM->PWM_model != PWMTYPE_UNI_POLAR &&
	   pDCPWM->PWM_model != PWMTYPE_BI_POLAR &&
		   pDCPWM->PWM_model != PWMTYPE_STANDBY_LOWLOOP 
			   //&& pDCPWM->PWM_model != PWMTYPE_STANDBY_HIGHLOOP
			   )
		pDCPWM->PWM_model = PWMTYPE_BI_POLAR;
	
	PWM_Start_Standy(pDCPWM, standbyPos, bKeepPIDErr, LastPWMModel);
	
	OS_EXIT_CRITICAL();
	return True;
}

INT8U PWM_Start_Standy_Y(struct AxisParam_DC_PWM * pDCPWM, INT32U standbyPos, INT8U bKeepPIDErr)
{
	OS_CPU_SR cpu_sr;
	INT8U LastPWMModel;
	
	if(!PWM_SafeCheck(pDCPWM))
		return False;
	
	OS_ENTER_CRITICAL();
	
	pDCPWM->BaseDPI = 1;
	pDCPWM->CurrDPI = 1;
	
	LastPWMModel = pDCPWM->PWM_model;
	pDCPWM->PWM_model = pDCPWM->PWM_modelTable[0] ;
	
	if( pDCPWM->PWM_model != PWMTYPE_UNI_POLAR &&
	   pDCPWM->PWM_model != PWMTYPE_BI_POLAR &&
		   pDCPWM->PWM_model != PWMTYPE_STANDBY_LOWLOOP 
			   //&& pDCPWM->PWM_model != PWMTYPE_STANDBY_HIGHLOOP
			   )
		pDCPWM->PWM_model = PWMTYPE_UNI_POLAR;
	
	PWM_Start_Standy(pDCPWM, standbyPos, bKeepPIDErr, LastPWMModel);
	
	OS_EXIT_CRITICAL();
	return True;
}

#define PWM_Stop_X(a)    PWM_Stop(a)
#define PWM_Stop_Y(a)    PWM_Stop(a)
INT8U PWM_Stop(struct AxisParam_DC_PWM * pDCPWM)
{
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();
	
	AT91C_BASE_PWMC->PWMC_IDR = pDCPWM->Channel_Mask;
#ifdef VM_PRINT
	AT91C_BASE_TC5->TC_IDR = AT91C_TC_CPCS;
#else
	pDCPWM->pPWM_channel1->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
	if(pDCPWM->ChannelValidLevel[0] == 0)
		pDCPWM->pPWM_channel1->PWMC_CMR ^= AT91C_PWMC_CPOL;
	pDCPWM->pPWM_channel2->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
	if(pDCPWM->ChannelValidLevel[1] == 0)
		pDCPWM->pPWM_channel2->PWMC_CMR ^= AT91C_PWMC_CPOL;
	AT91C_BASE_PWMC->PWMC_DIS = pDCPWM->Channel_Mask;
#endif
	
	if(pDCPWM->ChannelNO == 0)
		pPWM1Handle = PWMHandler_dimmy;
	else
		pPWM2Handle = PWMHandler_dimmy; 
	
	OS_EXIT_CRITICAL();
	return True;
}

#ifdef VM_PRINT
INT32U CURCOOR = 0x200000;
INT32U PWM_NUM = 0;
INT8U BEGIN_PWM =False;
INT8U IS_PRINT =False;
#endif
void PWMHandler_PosDir_UNI_POLAR(struct AxisParam_DC_PWM * pDCPWM, INT32U curCoor) FAST_FUNC_SECTION
{
#ifdef VM_PRINT
	static INT8U first1 =True, first2 =True;
	INT32U cur_print_coor;
	curCoor = pDCPWM->Coor_NextCycle;
	if(BEGIN_PWM == True)
	{
		if(PWM_NUM > 0)
		{
			if(IS_PRINT == True)
				PWM_NUM = 0;
			else
				PWM_NUM--;
			if(first2 ==True)
			{
				VM_PWM_START(pDCPWM);
				first2 =False;
			}
		}
		else
		{
			if(IS_PRINT == True)
			{
#ifdef   HEAD_EPSON_GEN5  			
				cur_print_coor = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);	
#elif defined(HEAD_RICOH_G4)
				cur_print_coor = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
#endif
				if(pDCPWM->Dir == DIRTYPE_POS)
				{
					if((cur_print_coor - 0x40000)>= (printInfo.PrintMove_Start>printInfo.PrintMove_End?printInfo.PrintMove_Start:printInfo.PrintMove_End))
					{
						VM_PWM_STOP();
						BEGIN_PWM = False;
						first1 = False;
					}
				}
				else
				{
					if((cur_print_coor - 0x40000)<= (printInfo.PrintMove_Start<printInfo.PrintMove_End?printInfo.PrintMove_Start:printInfo.PrintMove_End))
					{
						VM_PWM_STOP();
						BEGIN_PWM = False;
						first1 = False;
					}
				}
			}
			else
			{
				VM_PWM_STOP();
				BEGIN_PWM = False;
				first1 = False;
			}
		}
	}
#else
	pDCPWM->e = (INT32S)(pDCPWM->Coor_NextCycle - curCoor);
	if(calcPID(pDCPWM))
	{
		//start status: CPOL = 0, CUPDR = 100%
		pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime - pDCPWM->PWM_CurValue;
		pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
	}
	PutMoveRecord(pDCPWM->Dir, pDCPWM->AccSegment, pDCPWM->MoveType, (pDCPWM->AxisID == AXIS_ID_X), 
				  False, pDCPWM->PWM_CurValue, pDCPWM->Coor_NextCycle, curCoor);
#endif
	
	pDCPWM->curPWMTick ++;
	
	if(pDCPWM->AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		pDCPWM->Coor_NextCycle_Frac += pDCPWM->ratio_StepPerCycle;
		pDCPWM->Coor_NextCycle += (pDCPWM->Coor_NextCycle_Frac >> 24);
		pDCPWM->Coor_NextCycle_Frac &= 0xFFFFFF;
		if( (INT32S)pDCPWM->Coor_NextCycle >= (INT32S)pDCPWM->speedDownCoor )
		{
			pDCPWM->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pDCPWM->curPWMTickInAccSegment = 0;
			pDCPWM->AccTableIndex = pDCPWM->speedDownIndex;
			
			pDCPWM->Coor_NextCycle = pDCPWM->speedDownCoor;
		}
		else
			pDCPWM->curPWMTickInAccSegment ++;
	}
	else if(pDCPWM->AccSegment == ACCSEGTYPE_SPEED_UP)
	{
#ifdef VM_PRINT
		if((PWM_OFFSET_INDEX > pDCPWM->AccTableLen -pDCPWM->AccTableIndex)&&(pDCPWM->ChannelNO == 0)&&(first1 == True))
		{
			first1 = False;
			BEGIN_PWM = True;
		}
#endif
		pDCPWM->Coor_NextCycle += pDCPWM->pAccTable[pDCPWM->AccTableIndex];
		pDCPWM->AccTableIndex ++;
		pDCPWM->curPWMTickInAccSegment ++;
		if( (INT32S)pDCPWM->Coor_NextCycle >= (INT32S)pDCPWM->speedDownCoor)
		{
			pDCPWM->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pDCPWM->curPWMTickInAccSegment = 0;
			pDCPWM->AccTableIndex = pDCPWM->speedDownIndex;
			
			pDCPWM->Coor_NextCycle = pDCPWM->speedDownCoor;
		}
		else if(pDCPWM->AccTableIndex == pDCPWM->AccTableLen)
		{
			pDCPWM->AccSegment = ACCSEGTYPE_CONSTANT_SPEED;
			pDCPWM->curPWMTickInAccSegment = 0;
		}
	}
	else
	{
		pDCPWM->curPWMTickInAccSegment ++;
		if( pDCPWM->AccTableIndex == (INT32U)-1)
		{
			if( (INT32S)curCoor >= (INT32S)(pDCPWM->Coor_NextCycle - pDCPWM->PermitStopError))
			{
				if(pDCPWM->AxisID == AXIS_ID_X)
				{
					PWM_Stop_X(pDCPWM);
					PWM_Start_Standy_X(pDCPWM, pDCPWM->CoorIdeal, True);
#ifdef VM_PRINT
					first1 =True;first2=True;
					CURCOOR=pDCPWM->CoorIdeal;
#endif
				}
				else
				{
					PWM_Stop_Y(pDCPWM);
					PWM_Start_Standy_Y(pDCPWM, pDCPWM->CoorIdeal, True);
				}
				reportMoveCompleted(pDCPWM->AxisID);
			}
		}
		else
		{
			pDCPWM->Coor_NextCycle += pDCPWM->pAccTable[pDCPWM->AccTableIndex];
			pDCPWM->AccTableIndex --;
		}
	}
}

void PWMHandler_NegDir_UNI_POLAR(struct AxisParam_DC_PWM * pDCPWM, INT32U curCoor) FAST_FUNC_SECTION
{
#ifdef VM_PRINT
	static INT8U first1 =True, first2 =True;
	INT32U cur_print_coor;
	curCoor = pDCPWM->Coor_NextCycle;
	if(BEGIN_PWM == True)
	{
		if(PWM_NUM > 0)
		{
			if(IS_PRINT == True)
				PWM_NUM = 0;
			else
				PWM_NUM--;
			if(first2 ==True)
			{
				VM_PWM_START(pDCPWM);
				first2 =False;
			}
		}
		else
		{
			if(IS_PRINT == True)
			{
#ifdef   HEAD_EPSON_GEN5  			
				cur_print_coor = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);	
#elif defined(HEAD_RICOH_G4)
				cur_print_coor = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
#endif
				if(pDCPWM->Dir == DIRTYPE_POS)
				{
					if((cur_print_coor - 0x40000)>= (printInfo.PrintMove_Start>printInfo.PrintMove_End?printInfo.PrintMove_Start:printInfo.PrintMove_End))
					{
						VM_PWM_STOP();
						BEGIN_PWM = False;
						first1 = False;
					}
				}
				else
				{
					if((cur_print_coor - 0x40000)<= (printInfo.PrintMove_Start<printInfo.PrintMove_End?printInfo.PrintMove_Start:printInfo.PrintMove_End))
					{
						VM_PWM_STOP();
						BEGIN_PWM = False;
						first1 = False;
					}
				}
			}
			else
			{
				VM_PWM_STOP();
				BEGIN_PWM = False;
				first1 = False;
			}
		}
	}
#else
	pDCPWM->e = (INT32S)(curCoor - pDCPWM->Coor_NextCycle);
	if(calcPID(pDCPWM))
	{
		//start status: CPOL = 0, CUPDR = 100%
		pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
		pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime - pDCPWM->PWM_CurValue;
	}
	PutMoveRecord(pDCPWM->Dir, pDCPWM->AccSegment, pDCPWM->MoveType, (pDCPWM->AxisID == AXIS_ID_X), 
				  False, pDCPWM->PWM_CurValue, pDCPWM->Coor_NextCycle, curCoor);
#endif
	
	pDCPWM->curPWMTick ++;
	
	if(pDCPWM->AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		pDCPWM->Coor_NextCycle_Frac += pDCPWM->ratio_StepPerCycle;
		pDCPWM->Coor_NextCycle -= (pDCPWM->Coor_NextCycle_Frac >> 24);
		pDCPWM->Coor_NextCycle_Frac &= 0xFFFFFF;
		if( (INT32S)pDCPWM->Coor_NextCycle <= (INT32S)pDCPWM->speedDownCoor )
		{
			pDCPWM->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pDCPWM->curPWMTickInAccSegment = 0;
			pDCPWM->AccTableIndex = pDCPWM->speedDownIndex;
			
			pDCPWM->Coor_NextCycle = pDCPWM->speedDownCoor;
		}
		else
			pDCPWM->curPWMTickInAccSegment ++;
	}
	else if(pDCPWM->AccSegment == ACCSEGTYPE_SPEED_UP)
	{
#ifdef VM_PRINT
		if((PWM_OFFSET_INDEX > pDCPWM->AccTableLen -pDCPWM->AccTableIndex)&&(pDCPWM->ChannelNO == 0)&&(first1 == True))
		{
			first1 = False;
			BEGIN_PWM = True;
		}
#endif
		pDCPWM->Coor_NextCycle -= pDCPWM->pAccTable[pDCPWM->AccTableIndex];
		pDCPWM->AccTableIndex ++;
		pDCPWM->curPWMTickInAccSegment ++;
		if( (INT32S)pDCPWM->Coor_NextCycle <= (INT32S)pDCPWM->speedDownCoor)
		{
			pDCPWM->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pDCPWM->curPWMTickInAccSegment = 0;
			pDCPWM->AccTableIndex = pDCPWM->speedDownIndex;
			
			pDCPWM->Coor_NextCycle = pDCPWM->speedDownCoor;
		}
		else if(pDCPWM->AccTableIndex == pDCPWM->AccTableLen)
		{
			pDCPWM->AccSegment = ACCSEGTYPE_CONSTANT_SPEED;
			pDCPWM->curPWMTickInAccSegment = 0;
		}
	}
	else
	{
		pDCPWM->curPWMTickInAccSegment ++;
		if( pDCPWM->AccTableIndex == (INT32U)-1)
		{
			if( (INT32S)curCoor <= (INT32S)(pDCPWM->Coor_NextCycle + pDCPWM->PermitStopError))
			{
				pDCPWM->e = - pDCPWM->e;
				pDCPWM->e1 = - pDCPWM->e1;
				pDCPWM->e2 = - pDCPWM->e2;
				int index=0;
				for(index =0; index <ERROR_HISTORY_LEN ; index ++)
				{
					pDCPWM->e_history[index] = - pDCPWM->e_history[index];
				}
				if(pDCPWM->AxisID == AXIS_ID_X)
				{
					PWM_Stop_X(pDCPWM);
					PWM_Start_Standy_X(pDCPWM, pDCPWM->CoorIdeal, True);
#ifdef VM_PRINT
					first1 =True;first2=True;
					CURCOOR=pDCPWM->CoorIdeal;
#endif
				}
				else
				{
					PWM_Stop_Y(pDCPWM);
					PWM_Start_Standy_Y(pDCPWM, pDCPWM->CoorIdeal, True);
				}
				reportMoveCompleted(pDCPWM->AxisID);
			}
		}
		else
		{
			pDCPWM->Coor_NextCycle -= pDCPWM->pAccTable[pDCPWM->AccTableIndex];
			pDCPWM->AccTableIndex --;
		}
	}
}

void PWMHandler_PosDir_BI_POLAR(struct AxisParam_DC_PWM * pDCPWM, INT32U curCoor) FAST_FUNC_SECTION
{
#ifdef VM_PRINT
	static INT8U first1 =True, first2 =True;
	INT32U cur_print_coor;
	curCoor = pDCPWM->Coor_NextCycle;
	if(BEGIN_PWM == True)
	{
		if(PWM_NUM > 0)
		{
			if(IS_PRINT == True)
				PWM_NUM = 0;
			else
				PWM_NUM--;
			if(first2 ==True)
			{
				VM_PWM_START(pDCPWM);
				first2 =False;
			}
		}
		else
		{
			if(IS_PRINT == True)
			{
#ifdef   HEAD_EPSON_GEN5  			
				cur_print_coor = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);	
#elif defined(HEAD_RICOH_G4)
				cur_print_coor = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
#endif	
				if(pDCPWM->Dir == DIRTYPE_POS)
				{
					if((cur_print_coor - 0x40000)>= (printInfo.PrintMove_Start>printInfo.PrintMove_End?printInfo.PrintMove_Start:printInfo.PrintMove_End))
					{
						VM_PWM_STOP();
						BEGIN_PWM = False;
						first1 = False;
					}
				}
				else
				{
					if((cur_print_coor - 0x40000)<= (printInfo.PrintMove_Start<printInfo.PrintMove_End?printInfo.PrintMove_Start:printInfo.PrintMove_End))
					{
						VM_PWM_STOP();
						BEGIN_PWM = False;
						first1 = False;
					}
				}
			}
			else
			{
				VM_PWM_STOP();
				BEGIN_PWM = False;
				first1 = False;
			}
		}
	}
#else
	pDCPWM->e = (INT32S)(INT16S)(INT16U)(pDCPWM->Coor_NextCycle - curCoor);
	if(calcPID(pDCPWM))
	{
		//start status: X1: CPOL = 0, CUPDR = 50% - deadtime/2; X1: CPOL = 1, CUPDR = 50% - deadtime/2.
		pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime - pDCPWM->PWM_CurValue;
		pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime - (pDCPWM->PWM_CurValue + pDCPWM->PWM_DeadTime);
	}
	PutMoveRecord(pDCPWM->Dir, pDCPWM->AccSegment, pDCPWM->MoveType, (pDCPWM->AxisID == AXIS_ID_X), 
				  False, pDCPWM->PWM_CurValue, pDCPWM->Coor_NextCycle, curCoor);
#endif
	
	pDCPWM->curPWMTick ++;
	
	if(pDCPWM->AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		pDCPWM->Coor_NextCycle_Frac += pDCPWM->ratio_StepPerCycle;
		pDCPWM->Coor_NextCycle += (pDCPWM->Coor_NextCycle_Frac >> 24);
		pDCPWM->Coor_NextCycle_Frac &= 0xFFFFFF;
		if( (INT32S)pDCPWM->Coor_NextCycle >= (INT32S)pDCPWM->speedDownCoor )
		{
			pDCPWM->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pDCPWM->curPWMTickInAccSegment = 0;
			pDCPWM->AccTableIndex = pDCPWM->speedDownIndex;
			
			pDCPWM->Coor_NextCycle = pDCPWM->speedDownCoor;
		}
		else
			pDCPWM->curPWMTickInAccSegment ++;
	}
	else if(pDCPWM->AccSegment == ACCSEGTYPE_SPEED_UP)
	{
#ifdef VM_PRINT
		if((PWM_OFFSET_INDEX > pDCPWM->AccTableLen -pDCPWM->AccTableIndex)&&(pDCPWM->ChannelNO == 0)&&(first1 == True))
		{
			first1 = False;
			BEGIN_PWM = True;
		}
#endif
		pDCPWM->Coor_NextCycle += pDCPWM->pAccTable[pDCPWM->AccTableIndex];
		pDCPWM->AccTableIndex ++;
		pDCPWM->curPWMTickInAccSegment ++;
		if( (INT32S)pDCPWM->Coor_NextCycle >= (INT32S)pDCPWM->speedDownCoor)
		{
			pDCPWM->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pDCPWM->curPWMTickInAccSegment = 0;
			pDCPWM->AccTableIndex = pDCPWM->speedDownIndex;
			
			pDCPWM->Coor_NextCycle = pDCPWM->speedDownCoor;
		}
		else if(pDCPWM->AccTableIndex == pDCPWM->AccTableLen)
		{
			pDCPWM->AccSegment = ACCSEGTYPE_CONSTANT_SPEED;
			pDCPWM->curPWMTickInAccSegment = 0;
		}
	}
	else
	{
		pDCPWM->curPWMTickInAccSegment ++;
		if( pDCPWM->AccTableIndex == (INT32U)-1)
		{
			if( (INT32S)curCoor >= (INT32S)(pDCPWM->Coor_NextCycle - pDCPWM->PermitStopError))
			{
				if(pDCPWM->AxisID == AXIS_ID_X)
				{
					PWM_Stop_X(pDCPWM);
					PWM_Start_Standy_X(pDCPWM, pDCPWM->CoorIdeal, True);
#ifdef VM_PRINT
					first1 =True;first2=True;
					CURCOOR=pDCPWM->CoorIdeal;
#endif
				}
				else
				{
					PWM_Stop_Y(pDCPWM);
					PWM_Start_Standy_Y(pDCPWM, pDCPWM->CoorIdeal, True);
				}
				reportMoveCompleted(pDCPWM->AxisID);
			}
		}
		else
		{
			pDCPWM->Coor_NextCycle += pDCPWM->pAccTable[pDCPWM->AccTableIndex];
			pDCPWM->AccTableIndex --;
		}
	}
}

void PWMHandler_NegDir_BI_POLAR(struct AxisParam_DC_PWM * pDCPWM, INT32U curCoor) FAST_FUNC_SECTION
{
#ifdef VM_PRINT
	static INT8U first1 =True, first2 =True;
	INT32U cur_print_coor;
	curCoor = pDCPWM->Coor_NextCycle;
	if(BEGIN_PWM == True)
	{
		if(PWM_NUM > 0)
		{
			if(IS_PRINT == True)
				PWM_NUM = 0;
			else
				PWM_NUM--;
			if(first2 ==True)
			{
				VM_PWM_START(pDCPWM);
				first2 =False;
			}
		}
		else
		{
			if(IS_PRINT == True)
			{
#ifdef   HEAD_EPSON_GEN5  			
				cur_print_coor = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);	
#elif defined(HEAD_RICOH_G4)
				cur_print_coor = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
#endif	
				if(pDCPWM->Dir == DIRTYPE_POS)
				{
					if((cur_print_coor - 0x40000)>= (printInfo.PrintMove_Start>printInfo.PrintMove_End?printInfo.PrintMove_Start:printInfo.PrintMove_End))
					{
						VM_PWM_STOP();
						BEGIN_PWM = False;
						first1 = False;
					}
				}
				else
				{
					if((cur_print_coor - 0x40000)<= (printInfo.PrintMove_Start<printInfo.PrintMove_End?printInfo.PrintMove_Start:printInfo.PrintMove_End))
					{
						VM_PWM_STOP();
						BEGIN_PWM = False;
						first1 = False;
					}
				}
			}
			else
			{
				VM_PWM_STOP();
				BEGIN_PWM = False;
				first1 = False;
			}
		}
	}
#else
	pDCPWM->e = (INT32S)(INT16S)(INT16U)(pDCPWM->Coor_NextCycle - curCoor);
	if(calcPID(pDCPWM))
	{
		//start status: X1: CPOL = 0, CUPDR = 50% - deadtime/2; X1: CPOL = 1, CUPDR = 50% - deadtime/2.
		pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime - pDCPWM->PWM_CurValue;
		pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime - (pDCPWM->PWM_CurValue + pDCPWM->PWM_DeadTime);
	}
	PutMoveRecord(pDCPWM->Dir, pDCPWM->AccSegment, pDCPWM->MoveType, (pDCPWM->AxisID == AXIS_ID_X), 
				  False, pDCPWM->PWM_CurValue, pDCPWM->Coor_NextCycle, curCoor);
#endif
	
	pDCPWM->curPWMTick ++;
	
	if(pDCPWM->AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		pDCPWM->Coor_NextCycle_Frac += pDCPWM->ratio_StepPerCycle;
		pDCPWM->Coor_NextCycle -= (pDCPWM->Coor_NextCycle_Frac >> 24);
		pDCPWM->Coor_NextCycle_Frac &= 0xFFFFFF;
		if( (INT32S)pDCPWM->Coor_NextCycle <= (INT32S)pDCPWM->speedDownCoor )
		{
			pDCPWM->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pDCPWM->curPWMTickInAccSegment = 0;
			pDCPWM->AccTableIndex = pDCPWM->speedDownIndex;
			
			pDCPWM->Coor_NextCycle = pDCPWM->speedDownCoor;
		}
		else
			pDCPWM->curPWMTickInAccSegment ++;
	}
	else if(pDCPWM->AccSegment == ACCSEGTYPE_SPEED_UP)
	{
#ifdef VM_PRINT
		if((PWM_OFFSET_INDEX > pDCPWM->AccTableLen -pDCPWM->AccTableIndex)&&(pDCPWM->ChannelNO == 0)&&(first1 == True))
		{
			first1 = False;
			BEGIN_PWM = True;
		}
#endif
		pDCPWM->Coor_NextCycle -= pDCPWM->pAccTable[pDCPWM->AccTableIndex];
		pDCPWM->AccTableIndex ++;
		pDCPWM->curPWMTickInAccSegment ++;
		if( (INT32S)pDCPWM->Coor_NextCycle <= (INT32S)pDCPWM->speedDownCoor)
		{
			pDCPWM->AccSegment = ACCSEGTYPE_SPEED_DOWN;
			pDCPWM->curPWMTickInAccSegment = 0;
			pDCPWM->AccTableIndex = pDCPWM->speedDownIndex;
			
			pDCPWM->Coor_NextCycle = pDCPWM->speedDownCoor;
		}
		else if(pDCPWM->AccTableIndex == pDCPWM->AccTableLen)
		{
			pDCPWM->AccSegment = ACCSEGTYPE_CONSTANT_SPEED;
			pDCPWM->curPWMTickInAccSegment = 0;
		}
	}
	else
	{
		pDCPWM->curPWMTickInAccSegment ++;
		if( pDCPWM->AccTableIndex == (INT32U)-1)
		{
			if( (INT32S)curCoor <= (INT32S)(pDCPWM->Coor_NextCycle + pDCPWM->PermitStopError))
			{
				if(pDCPWM->AxisID == AXIS_ID_X)
				{
					PWM_Stop_X(pDCPWM);
					PWM_Start_Standy_X(pDCPWM, pDCPWM->CoorIdeal, True);
#ifdef VM_PRINT
					first1 =True;first2=True;
					CURCOOR=pDCPWM->CoorIdeal;
#endif
				}
				else
				{
					PWM_Stop_Y(pDCPWM);
					PWM_Start_Standy_Y(pDCPWM, pDCPWM->CoorIdeal, True);
				}
				reportMoveCompleted(pDCPWM->AxisID);
			}
		}
		else
		{
			pDCPWM->Coor_NextCycle -= pDCPWM->pAccTable[pDCPWM->AccTableIndex];
			pDCPWM->AccTableIndex --;
		}
	}
}

INT8U PWM_Start_Movement(struct AxisParam_DC_PWM * pDCPWM, INT8U speedID, 
						 INT32U endCoor, INT8U moveType, INT32U distance, INT32U currCoor)
{
	struct ACCCurveInfo * pAccTable;
	INT32U i;
	
	//doesn't consider coor value overflow. it is the job of caller.
	pDCPWM->CoorIdeal = endCoor;
	pDCPWM->Coor_NextCycle = currCoor;
	pDCPWM->Coor_NextCycle_Frac = 0;
	pDCPWM->ratio_StepPerCycle = pDCPWM->ratioTable_StepPerCycle[speedID];
	
	pDCPWM->MoveType = moveType;
	pDCPWM->AccSegment =  ACCSEGTYPE_SPEED_UP;
	
	pDCPWM->curPWMTick = 0;
	pDCPWM->curPWMTickInAccSegment = 0;
	
	pAccTable = pDCPWM->pAllCurveTable[speedID];
	pDCPWM->pAccTable = pAccTable->IncTable;
	pDCPWM->AccTableLen = pAccTable->AccCurveLen;
	pDCPWM->AccTableIndex = 0;
#ifdef VM_PRINT
	CUR_SPEED_ID = speedID;
#endif
	
	if(distance < (INT32U)pAccTable->IncTable[0]*2)
		return False;
	
	if(distance >= pAccTable->AccDistance *2)
	{
		if(pDCPWM->Dir == DIRTYPE_POS)
			pDCPWM->speedDownCoor = endCoor - pAccTable->AccDistance;
		else
			pDCPWM->speedDownCoor = endCoor + pAccTable->AccDistance;
		pDCPWM->speedDownIndex = pAccTable->AccCurveLen - 1;
	}
	else
	{
		INT32U * pAccDisTable = (INT32U *)(pAccTable->IncTable + pAccTable->AccDisTblOffset) + 1;
		INT32U start,end;
		
		start = 0;
		end = pAccTable->AccCurveLen-1;
		distance <<= ACC_DISTANCE_SHIFT_BIT;
		while(end - start > 1)
		{
			i = (start + end)/2;
			if(pAccDisTable[i] *2 > distance)
			{
				end = i;
			}
			else if(pAccDisTable[i] *2 < distance)
			{
				start = i;
			}
			else
			{
				start = i;
				break;
			}
		}
		if(pDCPWM->Dir == DIRTYPE_POS)
			pDCPWM->speedDownCoor = endCoor - (pAccDisTable[start]>>ACC_DISTANCE_SHIFT_BIT);
		else
			pDCPWM->speedDownCoor = endCoor + (pAccDisTable[start]>>ACC_DISTANCE_SHIFT_BIT);
		pDCPWM->speedDownIndex = start-1;
#ifdef VM_PRINT
		BEGIN_PWM = True;
#endif
	}
#ifdef VM_PRINT
	if(pDCPWM->Dir == DIRTYPE_POS)
		PWM_NUM = ( ((float)(endCoor - currCoor)*0x1000000)/pDCPWM->ratio_StepPerCycle);
	else
		PWM_NUM = ( ((float)(currCoor - endCoor)*0x1000000)/pDCPWM->ratio_StepPerCycle);
#endif  
	pDCPWM->PWM_CycleTime = PWM_MOVE_CYCLE/2;
	pDCPWM->PWM_OverLoadNum = 0;
	pDCPWM->PWM_MaxValue = (pDCPWM->PWM_CycleTime - 50);
	switch(pDCPWM->PWM_model)
	{
	case PWMTYPE_UNI_POLAR:
		pDCPWM->PWM_MinValue = 0; 
		//pDCPWM->PWM_CurValue = (pDCPWM->PWM_CycleTime)/2; //350
		//pDCPWM->PWM_CurValue = 350; //for 28346/10 Hz;
		//pDCPWM->PWM_CurValue = pDCPWM->PWM_MaxValue; //350
		pDCPWM->PWM_CurValue = pDCPWM->PWM_MinValue; //cheney: the start value is too large.
		break;
	case PWMTYPE_BI_POLAR:
		pDCPWM->PWM_MinValue = 50; 
		pDCPWM->PWM_CurValue = (pDCPWM->PWM_CycleTime - pDCPWM->PWM_DeadTime)/2;
		break;
	}
	
	//PIDREG3 part
	pDCPWM->e = 0;
	pDCPWM->e1 = 0;
	pDCPWM->e2 = 0;
	pDCPWM->Kp = pDCPWM->PIDTable[speedID+1][0];
	pDCPWM->Ki = pDCPWM->PIDTable[speedID+1][1];
	pDCPWM->Kd = pDCPWM->PIDTable[speedID+1][2];
	pDCPWM->fixpoint_bit = PID_SHIFT_BIT; 
	pDCPWM->pid_out_max = pDCPWM->PWM_CycleTime/2;
	pDCPWM->pid_out_min = -(pDCPWM->PWM_CycleTime/2);
	pDCPWM->pid_out = 0;
	pDCPWM->pid_out_frac = 0;
	memset(pDCPWM->e_history, 0, ERROR_HISTORY_LEN*sizeof(int));
	pDCPWM->e_i = 0;
	
	
	//AT91C_BASE_PWMC->PWMC_MR = 0;
#ifndef VM_PRINT
	pDCPWM->pPWM_channel1->PWMC_CPRDR = pDCPWM->PWM_CycleTime;
	pDCPWM->pPWM_channel2->PWMC_CPRDR = pDCPWM->PWM_CycleTime;
#endif
	switch(pDCPWM->PWM_model)
	{
	case PWMTYPE_UNI_POLAR:
#ifndef VM_PRINT
		pDCPWM->pPWM_channel1->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
		if(pDCPWM->ChannelValidLevel[0] == 0)
			pDCPWM->pPWM_channel1->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel2->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
		if(pDCPWM->ChannelValidLevel[1] == 0)
			pDCPWM->pPWM_channel2->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel1->PWMC_CDTYR = pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
		pDCPWM->pPWM_channel2->PWMC_CDTYR = pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime;
#endif
		if( pDCPWM->ChannelNO == 0)
		{
#ifdef VM_PRINT
			PWM_OFFSET_INDEX =(( (float)(pAccTable->AccDistance)*0x1000000)/pDCPWM->ratio_StepPerCycle);
#endif
			if(pDCPWM->Dir == DIRTYPE_POS)
				pPWM1Handle = PWMHandler_PosDir_UNI_POLAR;
			else
				pPWM1Handle = PWMHandler_NegDir_UNI_POLAR;
		}
		else
		{
			if(pDCPWM->Dir == DIRTYPE_POS)
				pPWM2Handle = PWMHandler_PosDir_UNI_POLAR;
			else
				pPWM2Handle = PWMHandler_NegDir_UNI_POLAR;
		}
#ifdef VM_PRINT
		AT91C_BASE_TC5->TC_IER = AT91C_TC_CPCS;
#else
		AT91C_BASE_PWMC->PWMC_IER = pDCPWM->Channel_Mask;
		AT91C_BASE_PWMC->PWMC_ENA = pDCPWM->Channel_Mask;
#endif
		break;
	case PWMTYPE_BI_POLAR:
#ifndef VM_PRINT
		pDCPWM->pPWM_channel1->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG;
		if(pDCPWM->ChannelValidLevel[0] == 0)
			pDCPWM->pPWM_channel1->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel2->PWMC_CMR = AT91C_PWMC_CPRE_MCK | AT91C_PWMC_CALG | AT91C_PWMC_CPOL;
		if(pDCPWM->ChannelValidLevel[1] == 0)
			pDCPWM->pPWM_channel2->PWMC_CMR ^= AT91C_PWMC_CPOL;
		pDCPWM->pPWM_channel1->PWMC_CDTYR = pDCPWM->pPWM_channel1->PWMC_CUPDR = pDCPWM->PWM_CycleTime - pDCPWM->PWM_CurValue;
		pDCPWM->pPWM_channel2->PWMC_CDTYR = pDCPWM->pPWM_channel2->PWMC_CUPDR = pDCPWM->PWM_CycleTime - (pDCPWM->PWM_CurValue + pDCPWM->PWM_DeadTime);;
#endif
		if( pDCPWM->ChannelNO == 0)
		{
#ifdef VM_PRINT
			PWM_OFFSET_INDEX =(( (float)(pAccTable->AccDistance)*0x1000000)/pDCPWM->ratio_StepPerCycle);
#endif
			if(pDCPWM->Dir == DIRTYPE_POS)
				pPWM1Handle = PWMHandler_PosDir_BI_POLAR;
			else
				pPWM1Handle = PWMHandler_NegDir_BI_POLAR;
		}
		else
		{
			if(pDCPWM->Dir == DIRTYPE_POS)
				pPWM2Handle = PWMHandler_PosDir_BI_POLAR;
			else
				pPWM2Handle = PWMHandler_NegDir_BI_POLAR;
		}
#ifdef VM_PRINT
		AT91C_BASE_TC5->TC_IER = AT91C_TC_CPCS;
#else
		AT91C_BASE_PWMC->PWMC_IER = pDCPWM->Channel_Mask;
		AT91C_BASE_PWMC->PWMC_ENA = pDCPWM->Channel_Mask;
#endif
		break;
	}
	return True;
}

#define DISTANCE_TOO_SHORT   2
INT8U PWM_Start_X(struct AxisParam_DC_PWM * pDCPWM, INT8U speedID, INT16U baseDPI, 
				  INT16U currDPI, INT32U endCoor, INT8U moveType, INT8U IsPrint)
{
	OS_CPU_SR cpu_sr;
	INT32U currCoor, distance, tmp;
	INT8U ret;
	
	if(!PWM_SafeCheck(pDCPWM))
		return False;
	
	OS_ENTER_CRITICAL();
	if(IsPrint)
	{
		//another field of FPGA CoorCtrl must be set by caller.
#ifdef   HEAD_EPSON_GEN5 
		tmp = ReadSafeEpsonRegInt(EPSON_REGADDR_COOR_CTRL);
#elif defined(HEAD_RICOH_G4)
		tmp = (INT32U)(rFPGA_RICOH_COORCTRL_L|( (INT32U)rFPGA_RICOH_COORCTRLSTAT_H << 16));
#endif
		switch(baseDPI)
		{
		case 1440:
			tmp &= ~(ER_CoorCtrl_SMOOTH_DIVIDER | ER_CoorCtrl_SMOOTH_MULTI);
			tmp |= (ER_CoorCtrl_SMOOTH_DIVIDER_1440 | ER_CoorCtrl_SMOOTH_MULTI_1440);
			break;
		case 720:
			tmp &= ~(ER_CoorCtrl_SMOOTH_DIVIDER | ER_CoorCtrl_SMOOTH_MULTI);
			tmp |= (ER_CoorCtrl_SMOOTH_DIVIDER_720 | ER_CoorCtrl_SMOOTH_MULTI_720);
			break;
		case 540:
			tmp &= ~(ER_CoorCtrl_SMOOTH_DIVIDER | ER_CoorCtrl_SMOOTH_MULTI);
			tmp |= (ER_CoorCtrl_SMOOTH_DIVIDER_540 | ER_CoorCtrl_SMOOTH_MULTI_540);
			break;
		}
		switch(currDPI)
		{
		case 1440:
		case 720:
		case 540:
			tmp &= ~ ER_CoorCtrl_PRINT_DIVIDER;
			tmp |= ER_CoorCtrl_PRINT_DIVIDER_NONE;
			break;
		case 360:
		case 270:
			tmp &= ~ ER_CoorCtrl_PRINT_DIVIDER;
			tmp |= ER_CoorCtrl_PRINT_DIVIDER_HALF;
			break;
		}
	}
	else
	{
		//FPGA has a Bug when non-print movement. 
		//FPGA will use the setting VSD min-cycle to check all coor interval time.
		//but normal movement 's speed can be very fast.
		// So I set the fast VSD and max divider.
		//another field of FPGA CoorCtrl must be set by caller.
		tmp = ER_CoorCtrl_SMOOTH_DIVIDER_720 | ER_CoorCtrl_SMOOTH_MULTI_720 | 
			ER_CoorCtrl_PRINT_DIVIDER_QUAD | ER_CoorCtrl_PRINT_PHASE_0 | 
				ER_CoorCtrl_EN_DIR_NONE | ER_CoorCtrl_VSDMODEL_VSD3;
	}
#ifdef   HEAD_EPSON_GEN5		
	SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, tmp);
	currCoor = ReadEpsonRegInt(pDCPWM->CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
	tmp &= ~ER_CoorCtrl_FLASH;
	rFPGA_RICOH_COORCTRL_L = (INT16U)tmp;
	rFPGA_RICOH_COORCTRLSTAT_H = (INT16U)(tmp >> 16);
	if(pDCPWM->CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
	{
		currCoor = (INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
	}
	else if(pDCPWM->CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
	{
		currCoor = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
	}
#endif
	if(endCoor > currCoor)
		distance = endCoor - currCoor;
	else
		distance = currCoor - endCoor;
	
	//the caller must check if it ought to move or not. 
	//  In another word, the distance between endcoor and currCoor must >  Min_Distance.
	if(endCoor > currCoor)
		pDCPWM->Dir = DIRTYPE_POS;
	else
		pDCPWM->Dir = DIRTYPE_NEG;
	
	pDCPWM->BaseDPI = baseDPI;
	pDCPWM->CurrDPI = currDPI;
	
	if(IsPrint)
		pDCPWM->PWM_model = pDCPWM->PWM_modelTable[2];
	else
		pDCPWM->PWM_model = pDCPWM->PWM_modelTable[1];
	if( pDCPWM->PWM_model != PWMTYPE_UNI_POLAR && pDCPWM->PWM_model != PWMTYPE_BI_POLAR)
		pDCPWM->PWM_model = PWMTYPE_UNI_POLAR;
	
	if(!PWM_Start_Movement(pDCPWM, speedID, endCoor, moveType, distance, currCoor))
		ret = DISTANCE_TOO_SHORT;
	else
		ret = True;
	
	OS_EXIT_CRITICAL();
	return ret;
}

INT8U PWM_Start_Y(struct AxisParam_DC_PWM * pDCPWM, INT8U speedID, INT8U dir, INT32U endCoor, INT8U moveType, INT8U IsPrint)
{
	OS_CPU_SR cpu_sr;
	INT32U currCoor, distance;
	INT8U ret;
	
	if(!PWM_SafeCheck(pDCPWM))
		return False;
	
	OS_ENTER_CRITICAL();
	{//Y coor only has 16bit. must avoid overflow.
#ifdef HEAD_EPSON_GEN5
		INT32U curr = (INT32U)ReadEpsonRegShort(pDCPWM->CoorRegAddr);
		if( (INT16U)pDCPWM->lastCoor < 0x4000 && curr > 0xC000)
			pDCPWM->lastCoor -= 0x10000;
		else if((INT16U)pDCPWM->lastCoor > 0xC000 && curr < 0x4000)
			pDCPWM->lastCoor += 0x10000;
		pDCPWM->lastCoor &= 0xFFFF0000;
		pDCPWM->lastCoor |= curr;
		currCoor = pDCPWM->lastCoor;
#elif defined(HEAD_RICOH_G4)
		INT32U curr = (INT32U)(rFPGA_RICOH_YMOTORCOOR_L | (rFPGA_RICOH_YMOTORCOOR_H &0xFF) << 16);
		currCoor = YParam.lastCoor = curr;
#endif
	}
	pDCPWM->Dir = dir;
	if(dir == DIRTYPE_POS)
		distance = endCoor - currCoor;
	else
		distance = currCoor - endCoor;
	//the caller must check if it ought to move or not. 
	//  In another word, the distance between endcoor and currCoor must >  Min_Distance.
	
	pDCPWM->BaseDPI = 1;
	pDCPWM->CurrDPI = 1;
	
	if(IsPrint)
		pDCPWM->PWM_model = pDCPWM->PWM_modelTable[2];
	else
		pDCPWM->PWM_model = pDCPWM->PWM_modelTable[1];
	if( pDCPWM->PWM_model != PWMTYPE_UNI_POLAR && pDCPWM->PWM_model != PWMTYPE_BI_POLAR)
		pDCPWM->PWM_model = PWMTYPE_UNI_POLAR;
	
#ifdef VM_PRINT
	reportMoveCompleted(pDCPWM->AxisID);
	PWM_Start_Standy_Y(pDCPWM, pDCPWM->CoorIdeal, True);
	ret = True;
#else
	if(!PWM_Start_Movement(pDCPWM, speedID, endCoor, moveType, distance, currCoor))
		ret = DISTANCE_TOO_SHORT;
	else
		ret = True;
#endif
	OS_EXIT_CRITICAL();
	return ret;
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
	
	if(pStepperTC->ChannelNO == 2)
		pTC1Handle = TCHandler_dimmy;
	else
		pTC2Handle = TCHandler_dimmy; 
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
		distance = pStepperTC->CoorIdeal - pStepperTC-> CurCoor;
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
	
	if(pStepperTC->ChannelNO == 2)
	{
		if( pStepperTC->Dir == DIRTYPE_POS)
			pTC1Handle = TCHandler_PosDir_Move;
		else
			pTC1Handle = TCHandler_NegDir_Move;
	}
	else
	{
		if( pStepperTC->Dir == DIRTYPE_POS)
			pTC2Handle = TCHandler_PosDir_Move;
		else
			pTC2Handle = TCHandler_NegDir_Move; 
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
	if(pStepperTC->ChannelNO == 2)
		pTC1Handle = TCHandler_dimmy;
	else
		pTC2Handle = TCHandler_dimmy; 
	
	pStepperTC->Dir = DIRTYPE_STANDBY;
	
	OS_EXIT_CRITICAL();
	return True;
}

void TCHandler(void) FAST_FUNC_SECTION
{
	INT32U isr;
	
	if(pTC_Driver1 != NULL)
	{
		isr = AT91C_BASE_TC3->TC_SR & AT91C_BASE_TC3->TC_IMR;
		if(isr & AT91C_TC_CPCS)
			(*pTC1Handle)(pTC_Driver1);
	}
	if(pTC_Driver2 != NULL)
	{
		isr = AT91C_BASE_TC4->TC_SR & AT91C_BASE_TC4->TC_IMR;
		if(isr & AT91C_TC_CPCS)
			(*pTC2Handle)(pTC_Driver2);
	}
}

INT8U MapXUserSpeedToXMotorSpeed(INT8U speedID, INT16U currDPI, INT8U bIsPrint)
{
	if(bIsPrint)
		return XPrintSpeedMap(currDPI, speedID);
	else
		return XMoveSpeedMap(speedID);
}

#define START_ERROR_NOT_STANDBY         ((INT8U)-1)
#define START_ERROR_CHECK_SAFE          ((INT8U)0)
#define START_ERROR_OK                  ((INT8U)True)
#define START_ERROR_DISTANCE_TOO_SHORT  ((INT8U)2)
#define START_ERROR_NOT_ROLLBACK        ((INT8U)3)
#define INIT_RETURN_SAFE_DISTANCE	(720*5)  //5 inch.
INT8U IS_XOrigin_Irq = False;
INT8U User_Start_X(INT8U speedID, INT16U currDPI, INT8U bIsPrint, INT8U moveType, 
				   INT8U coorType, INT32S endCoorOrDistance, INT8U dir)
{
	OS_CPU_SR cpu_sr;
	INT32S endCoor, max_width, currCoor, distance;
	INT8U ret, baseDPIid;
	INT16U baseDPI;
	float ratio;
	INT8U isRealPrint;
#ifdef VM_PRINT
	IS_PRINT = bIsPrint;
#endif
	if(bIsPrint && speedID != 4)
		isRealPrint = True;
	else
		isRealPrint = False;
	
	speedID = MapXUserSpeedToXMotorSpeed(speedID, currDPI, bIsPrint);
	baseDPI = GetBaseDPI(currDPI, &baseDPIid);
	
#ifdef ALLWIN_EPSON_SAME
#define CARRIAGE_WIDTH 2 //30cm
#elif defined(MANUFACTURER_MICOLOR_EPSON)
#define CARRIAGE_WIDTH 2 //30cm
#else
	//for default carriage width
#define CARRIAGE_WIDTH 2 //20cm
#endif
	max_width = ((PAPER_MEDIA_WIDTH_MM*10 + CARRIAGE_WIDTH * 2 * 1000)  * currDPI) / 254;
	
	OS_ENTER_CRITICAL();
	if(XParam.Dir != DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return START_ERROR_NOT_STANDBY;
	}
	
	if(!bIsPrint)
		ratio = XParam.ratio_move;
	else
		ratio = XParam.ratio_print[baseDPIid];
	
	if(moveType == MOVETYPE_CONTINUE)
	{ //currDPI must be 720.
		if(dir == DIRTYPE_POS)
			endCoor = XParam.Region_End;
		else
			endCoor = XParam.Region_Start;
	}
	else if(moveType == MOVETYPE_CONTINUE_CHECKSENSOR)
	{ //doesn't know the real start and end.
		//currDPI must be 720.
		if( XParam.Region_End == (INT32S)(0x7FFFFFFF))
		{
			max_width = (INT32S)(max_width * ratio);
			
			if(dir == DIRTYPE_POS)
				endCoor = XParam.Origin_Offset + max_width;
			else
				endCoor = XParam.Origin_Offset - max_width;
		}
		else
		{
			if(dir == DIRTYPE_POS)
				endCoor = XParam.Region_Start + INIT_RETURN_SAFE_DISTANCE *ratio;
			else
				endCoor = XParam.Origin_Offset - (XParam.Region_End - XParam.Region_Start);
		}
	}
	else
	{
		if(coorType == 2)
			endCoor = endCoorOrDistance; //raw coor system.
		else
		{
			endCoorOrDistance = (INT32S)(endCoorOrDistance * ratio);
			if( coorType == 0) //absolute, and support varied printing DPI.
				endCoor = endCoorOrDistance + XParam.Origin_Offset;
			else
			{ //currDPI must be 720.
				if(dir == DIRTYPE_POS) 
					endCoor = XParam.CoorIdeal + endCoorOrDistance;
				else
					endCoor = XParam.CoorIdeal - endCoorOrDistance;
			}
			if(endCoor > XParam.Region_End)
			{
				endCoor = XParam.Region_End;
			}
			else if(endCoor < XParam.Region_Start)
			{
				endCoor = XParam.Region_Start;
			}
		}
	}
#ifdef   HEAD_EPSON_GEN5
	currCoor = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
	if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
	{
		currCoor = (INT32S)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
	}
	else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
	{
		currCoor = (INT32S)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
	}
#endif
	if(endCoor > currCoor)
		distance = endCoor - currCoor;
	else
		distance = currCoor - endCoor;
	
	if(distance <= XParam.Min_Distance)
	{
		OS_EXIT_CRITICAL();
		PWM_Start_Standy_X(&XParam, endCoor, True);
		reportMoveCompleted(XParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	
	OS_EXIT_CRITICAL();
	
	ret = PWM_Start_X(&XParam, speedID, baseDPI, currDPI, endCoor, moveType, isRealPrint);
	
	if( ret == False)
		return START_ERROR_CHECK_SAFE;
	else if(ret == DISTANCE_TOO_SHORT)
	{
		PWM_Start_Standy_X(&XParam, endCoor, True);
		reportMoveCompleted(XParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	else 
		return START_ERROR_OK;
}

INT8U User_Start_Y(INT8U speedID, INT8U moveType, INT8U coorType, INT32S endCoorOrDistance, INT8U dir, INT8U bUniDir, INT8U bIsPrint)
{
	OS_CPU_SR cpu_sr;
	INT32S endCoor, currCoor, distance;
	INT8U ret;
#ifdef MOTION_Y_DSP
	INT8U buf[20] = {0};
	if(moveType == MOVETYPE_CONTINUE)
	{
		if(dir == DIRTYPE_POS)
		{
			buf[0] = 8; //Length
			buf[1] = UART_PAPERFEED_CMD; 
			buf[2] = 1;
			buf[3] = printer.feedSpeed;
			*((__packed INT32S *)&buf[4]) = 0;
			
			while (!UART_SendCMD(UART_DSP_CHANNEL, buf)) 
				OSTimeDly(10);	
		}
		else
		{
			buf[0] = 8; //Length
			buf[1] = UART_PAPERFEED_CMD; 
			buf[2] = 0;
			buf[3] = printer.feedSpeed;
			*((__packed INT32S *)&buf[4]) = 0;
			
			while (!UART_SendCMD(UART_DSP_CHANNEL, buf)) 
				OSTimeDly(10);	
		}
	}
	else
	{
		if(coorType == 1)
		{
			if(endCoorOrDistance == 0)
			{
				INT8U err;
				OSFlagPost(mix_FLAG_GRP, MOTION_MOVE_COMPLETED_Y, OS_FLAG_SET, &err); 
				return START_ERROR_OK;
			}
			if(dir == DIRTYPE_POS)
			{
				buf[0] = 8; //Length
				buf[1] = UART_MOVEDISTANCE_CMD; 
				buf[2] = 3; //1,2 X, 3,4 Y, 5,6 Z
				buf[3] = printer.feedSpeed; 
				*((__packed INT32S *)&buf[4]) = endCoorOrDistance; 
				
				while (!UART_SendCMD(UART_DSP_CHANNEL, buf)) 
					OSTimeDly(10);		
			}
			else
			{
				buf[0] = 8; //Length
				buf[1] = UART_MOVEDISTANCE_CMD; 
				buf[2] = 4; //1,2 X, 3,4 Y, 5,6 Z
				buf[3] = printer.feedSpeed; 
				*((__packed INT32S *)&buf[4]) = endCoorOrDistance; 
				
				while (!UART_SendCMD(UART_DSP_CHANNEL, buf)) 
					OSTimeDly(10);		
			}
		}
		else
		{
			buf[0] = 8; //Length						
			buf[1] = UART_AXISMOVETO_CMD; 
			buf[2] = 2; //AXIS:1:x; 2:y; 4:z; 8:wiper Y.
			buf[3] = printer.feedSpeed; 
			*((__packed INT32S *)&buf[4]) = endCoorOrDistance; //move_distance means target position
			
			while (!UART_SendCMD(UART_DSP_CHANNEL, buf)) 
				OSTimeDly(10);	
		}	
	}
	return START_ERROR_OK;
#endif
	//speedID ++;
	if(speedID >= MAX_Y_SPEED_LEVEL)
		speedID = MAX_Y_SPEED_LEVEL - 1;
	
	OS_ENTER_CRITICAL();
	if(YParam.Dir != DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return START_ERROR_NOT_STANDBY;
	}
	
	if(moveType == MOVETYPE_CONTINUE || moveType == MOVETYPE_CONTINUE_CHECKSENSOR)
	{ 
		if(dir == DIRTYPE_POS)
			endCoor = YParam.Region_End;
		else
			endCoor = YParam.Region_Start;
	}
	else
	{
		endCoorOrDistance = (INT32S)(endCoorOrDistance * YParam.ratio_move);
		if( coorType == 0) //absolute
			endCoor = YParam.Origin_Offset + endCoorOrDistance;
		else
		{
			if(dir == DIRTYPE_POS) //Positive dir
				endCoor = YParam.Coor_nonRollback + endCoorOrDistance;
			else
				endCoor = YParam.Coor_nonRollback - endCoorOrDistance;
		}
	}
	
	{//Y coor only has 16bit. must avoid overflow.
#ifdef HEAD_EPSON_GEN5
		INT32U curr = (INT32U)ReadEpsonRegShort(YParam.CoorRegAddr);
		if( (INT16U)YParam.lastCoor < 0x4000 && curr > 0xC000)
			YParam.lastCoor -= 0x10000;
		else if((INT16U)YParam.lastCoor > 0xC000 && curr < 0x4000)
			YParam.lastCoor += 0x10000;
		YParam.lastCoor &= 0xFFFF0000;
		YParam.lastCoor |= curr;
		currCoor = YParam.lastCoor;
#elif defined(HEAD_RICOH_G4)
		INT32U curr = (INT32U)(rFPGA_RICOH_YMOTORCOOR_L | (rFPGA_RICOH_YMOTORCOOR_H &0xFF) << 16);
		currCoor = YParam.lastCoor = curr;
#endif
	}
	
	YParam.Coor_nonRollback = endCoor;
	
	if( endCoor > currCoor)
		distance = endCoor - currCoor;
	else
		distance = currCoor - endCoor;
	if( distance <= YParam.Min_Distance)
	{
		OS_EXIT_CRITICAL();
		PWM_Start_Standy_Y(&YParam, endCoor, True); //???, It can't assure to NO-rollback if PWM_model is BI_Polar.
		reportMoveCompleted(YParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	
	if( endCoor > currCoor)
	{
		distance = endCoor - currCoor;
		if( moveType == MOVETYPE_SEGMENT && coorType == 0)
			dir = DIRTYPE_POS;
		else
		{
			if( bUniDir && dir != DIRTYPE_POS)
			{
				OS_EXIT_CRITICAL();
				PWM_Start_Standy_Y(&YParam, currCoor, True);
				reportMoveCompleted(YParam.AxisID);
				return START_ERROR_NOT_ROLLBACK;
			}
			else
			{
				dir = DIRTYPE_POS;
			}
		}
	}
	else
	{
		if( moveType == MOVETYPE_SEGMENT && coorType == 0)
			dir = DIRTYPE_NEG;
		else
		{
			if( bUniDir && dir != DIRTYPE_NEG)
			{
				OS_EXIT_CRITICAL();
				PWM_Start_Standy_Y(&YParam, currCoor, True);
				reportMoveCompleted(YParam.AxisID);
				return START_ERROR_NOT_ROLLBACK;
			}
			else
			{
				dir = DIRTYPE_NEG;
			}
		}
	}
	
	OS_EXIT_CRITICAL();
	
	ret = PWM_Start_Y(&YParam, speedID, dir, endCoor, moveType, bIsPrint);
	
	if( ret == False)
		return START_ERROR_CHECK_SAFE;
	else if(ret == DISTANCE_TOO_SHORT)
	{
		PWM_Start_Standy_Y(&YParam, endCoor, True);
		reportMoveCompleted(YParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	else 
		return START_ERROR_OK;
}

INT8U User_Start_Z(INT8U speedID, INT8U moveType, INT8U coorType, INT32S endCoorOrDistance, INT8U dir)
{
	OS_CPU_SR cpu_sr;
	INT32S endCoor, distance;
	
	endCoorOrDistance *= STEPPER_ROTIO_SCALE;
	
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
	
	if( distance <= ZParam.Min_Distance)
	{
		OS_EXIT_CRITICAL();
		TC_Start_Standy(&ZParam);
		reportMoveCompleted(ZParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	
	OS_EXIT_CRITICAL();
	
#ifdef VM_PRINT
	reportMoveCompleted(ZParam.AxisID);
#else
	TC_Start_Move(&ZParam, speedID, endCoor, moveType);
#endif
	
	return START_ERROR_OK;
}

INT8U User_Start_C(INT8U speedID, INT8U moveType, INT8U coorType, INT32S endCoorOrDistance, INT8U dir)
{
	OS_CPU_SR cpu_sr;
	INT32S endCoor, distance;
#if defined(EPSON_CLEAN_INTEGRATE_3_CAP)
	INT8U bmove_c_axle = (endCoorOrDistance == 0xFFFFFFF) ? 0:1;
#endif
	
	endCoorOrDistance *= STEPPER_ROTIO_SCALE;
	
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
	
#if defined(EPSON_CLEAN_INTEGRATE_3_CAP)
	if(( distance <= CParam.Min_Distance) || (bmove_c_axle == 0))
#else
	if( distance <= CParam.Min_Distance)
#endif
	{
		OS_EXIT_CRITICAL();
		TC_Start_Standy(&CParam);
		reportMoveCompleted(CParam.AxisID);
		return START_ERROR_DISTANCE_TOO_SHORT;
	}
	
	OS_EXIT_CRITICAL();
	
#ifdef VM_PRINT
	reportMoveCompleted(CParam.AxisID);
#else
	TC_Start_Move(&CParam, speedID, endCoor, moveType);
#endif
	
	return START_ERROR_OK;
}
extern INT8U slow_down_at_once;
INT32U Find_index_for_down_curve(INT32U last_index,struct ACCCurveInfo * current_Curve,struct ACCCurveInfo * last_Curve)
{		
	INT32U index ;
	INT32U rate;
	float f1,f2;
	
	INT32U * CurrentDisTable = (INT32U *)(current_Curve->IncTable + current_Curve->AccDisTblOffset) + 1;
	INT32U * LastDisTable = (INT32U *)(last_Curve->IncTable + last_Curve->AccDisTblOffset) + 1;
	rate = (LastDisTable[last_index+1]-LastDisTable[last_index-1])/2; 
	
	f1= (float)(current_Curve->AccCurveLen);
	f2= (float)(last_Curve->AccCurveLen);
	index = (INT32U)(last_index*(f1/f2));
	
	if((index==0)||(index==current_Curve->AccCurveLen))
	{
		return index;
	}
	if(((CurrentDisTable[index+1]-CurrentDisTable[index-1])/2) > rate)
	{
		while(((CurrentDisTable[index+1]-CurrentDisTable[index-1])/2) > rate)
		{
			index--;
			if(index==0)
			{
				break;
			}
		}
	}
	else if(((CurrentDisTable[index+1]-CurrentDisTable[index-1])/2) < rate)
	{
		while(((CurrentDisTable[index+1]-CurrentDisTable[index-1])/2) < rate)
		{
			index++;
			if(index==current_Curve->AccCurveLen)
			{
				break;
			}
		}
	}
	
	return index;
}

INT8U User_Stop_X(INT8U bUrgent, INT8U bBaseOnRealCoor)
{
	OS_CPU_SR cpu_sr;
	INT32S distance;
	struct ACCCurveInfo *pAccCurve;
	struct ACCCurveInfo *last_pAccCurve;
	INT32U current_index;
	if(bUrgent)
	{
		return PWM_Urgent_Stop(&XParam);
	}
	if(slow_down_at_once)
	{
		pAccCurve = XParam.pAllCurveTable[MAX_X_SPEED_LEVEL - 1];
		last_pAccCurve = GET_ACCTBL_HEAD(XParam.pAccTable);
		current_index = Find_index_for_down_curve(XParam.AccTableIndex,pAccCurve,last_pAccCurve);
	}
	OS_ENTER_CRITICAL();
	if(XParam.Dir == DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		return False;
	}
	if(slow_down_at_once)
	{
		XParam.pAccTable = pAccCurve->IncTable;
		XParam.AccTableLen = pAccCurve->AccCurveLen;
	}
	else
	{
		pAccCurve = GET_ACCTBL_HEAD(XParam.pAccTable);
	}
	if(XParam.AccSegment == ACCSEGTYPE_SPEED_UP)
	{
		XParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(bBaseOnRealCoor)
		{
#ifdef   HEAD_EPSON_GEN5       
			XParam.speedDownCoor = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
			if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
			{
				XParam.speedDownCoor = (INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
			}
			else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
			{
				XParam.speedDownCoor = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
			}
#endif
			XParam.Coor_NextCycle = XParam.speedDownCoor;
			XParam.Coor_NextCycle_Frac = 0;
			XParam.e2 = XParam.e1 = XParam.e = 0;
			memset(XParam.e_history, 0, ERROR_HISTORY_LEN*sizeof(int));
			XParam.e_i = 0;
		}
		else
			XParam.speedDownCoor = XParam.Coor_NextCycle;
		if(slow_down_at_once)
		{
			XParam.AccTableIndex = current_index;
		}
		if(XParam.AccTableIndex > 0)
			XParam.AccTableIndex --;
		XParam.speedDownIndex = XParam.AccTableIndex;
		distance = GET_ACCTBL_DISTBL(pAccCurve)[XParam.AccTableIndex+1];
		if(XParam.Dir == DIRTYPE_POS)
			XParam.CoorIdeal = XParam.speedDownCoor + (distance >>ACC_DISTANCE_SHIFT_BIT);
		else
			XParam.CoorIdeal = XParam.speedDownCoor - (distance >>ACC_DISTANCE_SHIFT_BIT);
	}
	else if(XParam.AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		XParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(bBaseOnRealCoor)
		{
#ifdef   HEAD_EPSON_GEN5       
			XParam.speedDownCoor = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
			if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
			{
				XParam.speedDownCoor = (INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
			}
			else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
			{
				XParam.speedDownCoor = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
			}
#endif
			XParam.Coor_NextCycle = XParam.speedDownCoor;
			XParam.Coor_NextCycle_Frac = 0;
			XParam.e1 = XParam.e = 0;
		}
		else
			XParam.speedDownCoor = XParam.Coor_NextCycle;
		XParam.AccTableIndex = XParam.AccTableLen - 1;
		XParam.speedDownIndex = XParam.AccTableIndex;
		distance = pAccCurve->AccDistance;
		if( XParam.Dir == DIRTYPE_POS)
			XParam.CoorIdeal = XParam.speedDownCoor + distance;
		else
			XParam.CoorIdeal = XParam.speedDownCoor - distance;
	}
	else
	{
		XParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		if(slow_down_at_once)
		{
			XParam.speedDownCoor = XParam.Coor_NextCycle;
			XParam.AccTableIndex = current_index;
			if(XParam.AccTableIndex > 0)
				XParam.AccTableIndex --;
			
			XParam.speedDownIndex = XParam.AccTableIndex;
			distance = GET_ACCTBL_DISTBL(pAccCurve)[XParam.AccTableIndex+1];
			if(XParam.Dir == DIRTYPE_POS)
				XParam.CoorIdeal = XParam.speedDownCoor + (distance >>ACC_DISTANCE_SHIFT_BIT);
			else
				XParam.CoorIdeal = XParam.speedDownCoor - (distance >>ACC_DISTANCE_SHIFT_BIT);
		}
	}
	slow_down_at_once = FALSE;
	OS_EXIT_CRITICAL();
	
	return True;
}
INT8U User_Stop_Y(INT8U bUrgent)
{
	OS_CPU_SR cpu_sr;
	INT32S distance;
	struct ACCCurveInfo *pAccCurve;
	
	if(bUrgent)
	{
		return PWM_Urgent_Stop(&YParam);
	}
	
	OS_ENTER_CRITICAL();
	if(YParam.Dir == DIRTYPE_STANDBY)
	{
		OS_EXIT_CRITICAL();
		return False;
	}
	
	pAccCurve = GET_ACCTBL_HEAD(YParam.pAccTable);
	if(YParam.AccSegment == ACCSEGTYPE_SPEED_UP)
	{
		YParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		YParam.speedDownCoor = YParam.Coor_NextCycle;
		if( YParam.AccTableIndex > 0)
			YParam.AccTableIndex --;
		YParam.speedDownIndex = YParam.AccTableIndex;
		distance = GET_ACCTBL_DISTBL(pAccCurve)[YParam.AccTableIndex+1];
		if( YParam.Dir == DIRTYPE_POS)
			YParam.CoorIdeal = YParam.speedDownCoor + (distance >>ACC_DISTANCE_SHIFT_BIT);
		else
			YParam.CoorIdeal = YParam.speedDownCoor - (distance >>ACC_DISTANCE_SHIFT_BIT);
	}
	else if(YParam.AccSegment == ACCSEGTYPE_CONSTANT_SPEED)
	{
		YParam.AccSegment = ACCSEGTYPE_SPEED_DOWN;
		YParam.speedDownCoor = YParam.Coor_NextCycle;
		YParam.AccTableIndex = YParam.AccTableLen - 1;
		YParam.speedDownIndex = YParam.AccTableIndex;
		distance = pAccCurve->AccDistance;
		if( YParam.Dir == DIRTYPE_POS)
			YParam.CoorIdeal = YParam.speedDownCoor + distance;
		else
			YParam.CoorIdeal = YParam.speedDownCoor - distance;
	}
	YParam.Coor_nonRollback = YParam.CoorIdeal;
	
	OS_EXIT_CRITICAL();
	
	return True;
}
INT8U User_Stop_Z(INT8U bUrgent)
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

INT8U User_Stop_C(INT8U bUrgent)
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

INT8U StartAllStandby()
{
	INT8U ret;
	ret = PWM_Start_Standy_X(&XParam, XParam.CoorIdeal, False);
#ifndef MOTION_Y_DSP		
	ret &= PWM_Start_Standy_Y(&YParam, YParam.CoorIdeal, False);
#endif
	if(pTC_Driver1 != NULL)
		ret &= TC_Start_Standy(pTC_Driver1);
	if(pTC_Driver2 != NULL)
		ret &= TC_Start_Standy(pTC_Driver2);
	return ret;
}

INT8U UpdateSpeedInfoBaseonHeadType(INT8U HeadType)
{
	if(mainStatus != MS_INIT || InitStatus > IS_Send_INIT_STAGE1)
		return False;
	if(HeadType != HeadNo_Epson_Gen5 && HeadType != HeadNo_Ricoh_Gen4_64Pin && 
	   HeadType !=HeadNo_Ricoh_Gen4_G4 && HeadType != HeadNo_Ricoh_Gen4_G4L &&
		   HeadType !=HeadNo_Ricoh_Gen4_G4 )
		return False;
	else
		return True;
}

INT8U UpdateMoveRegionBaseonMediaWidth(INT8U Width) //unit : dm
{
	INT32S width_dot = (Width * 1000 * 720) / 254;
	INT32S width_offset;
	
	//    if((XParam.pAxisParam->Fixed_Position[MPID_FLATSTART] != 0x80000000 &&
	//        XParam.pAxisParam->Fixed_Position[MPID_FLATSTART] != 0xFFFFFFFF) &&
	//        (XParam.pAxisParam->Fixed_Position[MPID_FLATEND] != 0x80000000 &&
	//        XParam.pAxisParam->Fixed_Position[MPID_FLATEND] != 0xFFFFFFFF))
	//    {
	//        width_offset = width_dot - (XParam.pAxisParam->Fixed_Position[MPID_FLATEND] - 
	//            XParam.pAxisParam->Fixed_Position[MPID_FLATSTART]);
	//        XParam.Region_End += (INT32S)(width_offset * XParam.ratio_move);
	//        return True;
	//    }
	//    else
	//        return False;
	
	return True;
}
#if defined (EPSON_CLEAN_UPDOWN) || defined(COLORFUL_EPSON)
void ZOrigin_IO_Irq_Interrupt(const Pin *pPin) FAST_FUNC_SECTION
{
	if(mainStatus == MS_INIT)
	{
		if(InitStatus == IS_Z_GoOriginSensor)
		{
			if( GetSensor(&ZParam.Origin) == SENSOR_ZC_ORIGIN)
			{
				//OriginSensorPosi_X = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
				User_Stop_Z(False);
			}
		}
	}
}

void COrigin_IO_Irq_Interrupt(const Pin *pPin) FAST_FUNC_SECTION
{
	if(mainStatus == MS_INIT)
	{
		if(InitStatus == IS_C_GoOriginSensor)
		{
			if( GetSensor(&CParam.Origin) == SENSOR_ZC_ORIGIN)
			{
				//OriginSensorPosi_X = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
				User_Stop_C(False);
			}
		}
	}
}
#endif
INT8U StartInitAction()
{
#if ((defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)) && \
	!defined(COLORFUL_EPSON))
	INT8U speedID = 2;
	INT8U i, tmp;
	enum MicolorCleanType type;
	
	InitStatus = IS_Micolor_Clean_GoHome;
	//micolor hope that the speed of roll-back is the slowest, else maybe make head print dirty.
	//    tmp = cleanparam_EPSON_MICOLOR.Runtime.ActivedConfig;
	//    for(i=0; i<cleanparam_EPSON_MICOLOR.Config[tmp].sectionCount; i++)
	//    {
	//        type = cleanparam_EPSON_MICOLOR.Config[tmp].sections[i].type;
	//        if(type == SuckOnly || type == SuckAndWipe || type == SuckWipeAndFlash)
	//        {
	//            speedID = cleanparam_EPSON_MICOLOR.Config[tmp].sections[i].MoveSpeed;
	//            break;
	//        }
	//    }
	if(GetCleanPartCount() == 2)
		InitAxis = 3;
	else
		InitAxis = 1;
	
	//roll back the swipe.
	i = User_Start_C(speedID, 
					 //MOVETYPE_CONTINUE, 
					 MOVETYPE_SEGMENT, 
					 1,
					 cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance, 
					 cleanparam_EPSON_MICOLOR.factory.ElectricRotateDir ? DIRTYPE_NEG : DIRTYPE_POS
						 );
	
	if(GetCleanPartCount() == 2)
	{
		i &= User_Start_Z(speedID, 
						  //MOVETYPE_CONTINUE, 
						  MOVETYPE_SEGMENT, 
						  1,
						  cleanparam_EPSON_MICOLOR.factory.Wiper_Y_SuckToHideDistance, 
						  cleanparam_EPSON_MICOLOR.factory.ElectricRotateDir ? DIRTYPE_NEG : DIRTYPE_POS
							  );
	}
	return i;
	
#elif defined(COLORFUL_EPSON)
    if(GetSensor(&CParam.Origin) == SENSOR_ZC_NO_ORIGIN)
    {
        InitStatus = IS_C_GoOriginSensor;
        ConfigureSensorIt(&CParam.Origin, NULL, NULL, COrigin_IO_Irq_Interrupt); 
        User_Start_C(0, MOVETYPE_SEGMENT, 1,0x000FFFFF, DIRTYPE_NEG);
    }
    else
    {
        InitStatus = IS_C_LeaveOriginSensor;
        User_Start_C(0, MOVETYPE_SEGMENT, 1,cleanparam_EPSON_MICOLOR.factory.Wiper_Y_HideToWipeDistance, DIRTYPE_POS);
    }
#elif defined(EPSON_CLEAN_UPDOWN)
	if(GetSensor(&ZParam.Origin) == SENSOR_ZC_NO_ORIGIN)
	{
		InitStatus = IS_Z_GoOriginSensor;
		ConfigureSensorIt(&ZParam.Origin, NULL, NULL, ZOrigin_IO_Irq_Interrupt); 
		return User_Start_Z(0, MOVETYPE_SEGMENT, 1,0x000FFFFF, DIRTYPE_NEG);
	}
	else
	{
		InitStatus = IS_Z_LeaveOriginSensor;
		//ConfigureSensorIt(&ZParam.Origin, NULL, NULL, XOrigin_IO_Irq_Interrupt); 
		return User_Start_Z(0, MOVETYPE_SEGMENT, 1,cleanparam_EPSON_ALLWIN.factory.HeadBox_Z_SuckPos, DIRTYPE_POS);
	}
#else
	return False;
#endif
}

INT32S OriginSensorPosi_X = 0;

void XOrigin_IO_Irq_Interrupt(const Pin *pPin) FAST_FUNC_SECTION
{
	if(mainStatus == MS_INIT)
	{
		if(InitStatus == IS_X_GoOriginSensor)
		{
			if( GetSensor(&XParam.Origin) == True)
			{
#ifdef   HEAD_EPSON_GEN5       
				OriginSensorPosi_X = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
				if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
				{
					OriginSensorPosi_X = (INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
				}
				else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
				{
					OriginSensorPosi_X = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
				}
#endif
				User_Stop_X(False, True);
			}
		}
		else if(InitStatus == IS_X_LeaveOriginSensor)
		{
			if( GetSensor(&XParam.Origin) == False)
			{
				IS_XOrigin_Irq = True;
				User_Stop_X(False, True);
			}
		}
	}
}

void XOrigin_FPGA_Irq_Interrupt(void * data, INT8U IsHigh) FAST_FUNC_SECTION
{
	if(mainStatus == MS_INIT)
	{
		if(InitStatus == IS_X_GoOriginSensor)
		{
			if( IsHigh)
			{
#ifdef   HEAD_EPSON_GEN5       
				OriginSensorPosi_X = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
				if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
				{
					OriginSensorPosi_X = (INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
				}
				else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
				{
					OriginSensorPosi_X = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
				}
#endif
				User_Stop_X(False, True);
			}
		}
		else if(InitStatus == IS_X_LeaveOriginSensor)
		{
			if( !IsHigh)
			{
				IS_XOrigin_Irq = True;
				User_Stop_X(False, True);
			}
		}
	}
}

#if defined(INWEAR_FLAT_FIND_Y_HOME)
INT32S OriginSensorPosi_Y = 0;
INT32S LimitSensorPosi_Y = 0;

void YOrigin_IO_Irq_Interrupt(const Pin *pPin) FAST_FUNC_SECTION
{
	//if(mainStatus == MS_INIT)
	{
		//if(InitStatus == IS_Y_GoOriginSensor)
		if (YParam.Dir == DIRTYPE_NEG)
		{
			if( GetSensor(&YParam.Origin) == True)
			{
#ifdef   HEAD_EPSON_GEN5       
				OriginSensorPosi_Y = ReadSafeEpsonRegInt(YParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
				if(YParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
				{
					OriginSensorPosi_Y = (INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
				}
				else if(YParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
				{
					OriginSensorPosi_Y = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
				}
#endif
				User_Stop_Y(False);
			}
		}
		//else if(InitStatus == IS_Y_LeaveOriginSensor)
		{
			//if( GetSensor(&YParam.Origin) == False)
			{
				//User_Stop_Y(False);	
			}
		}
	}
}

void YLimit_IO_Irq_Interrupt(const Pin *pPin) FAST_FUNC_SECTION
{
	//if(mainStatus == MS_INIT)
	{
		//if(InitStatus == IS_Y_GoLimitSensor)
		if (YParam.Dir == DIRTYPE_POS)
		{
			if(GetSensor(&YParam.Limit) == True)
			{  
	#ifdef   HEAD_EPSON_GEN5       
				LimitSensorPosi_Y = ReadSafeEpsonRegInt(YParam.CoorRegAddr);
	#elif defined(HEAD_RICOH_G4)
				if(YParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
				{
					LimitSensorPosi_Y = (INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
				}
				else if(YParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
				{
					LimitSensorPosi_Y = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
				}
	#endif
				User_Stop_Y(False);
			}
		}
		//else if (InitStatus == IS_Y_LeaveLimitSensor)
		{
			//if(GetSensor(&YParam.Limit) == False)
			{  
				//User_Stop_Y(False);
			}
		}
	}
}

void YOrigin_FPGA_Irq_Interrupt(void * data, INT8U IsHigh) FAST_FUNC_SECTION
{
	if(mainStatus == MS_INIT)
	{
		if(InitStatus == IS_Y_GoOriginSensor)
		{
			//if( IsHigh)
			{
#ifdef   HEAD_EPSON_GEN5       
				OriginSensorPosi_Y = ReadSafeEpsonRegInt(YParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
				if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
				{
					OriginSensorPosi_Y = (INT32U)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
				}
				else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
				{
					OriginSensorPosi_Y = (INT32U)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
				}
#endif
				User_Stop_Y(False);
			}
		}
	}
}

INT8U ResetYRegion()
{
    INT32U currCoor = 0;
	INT32U curr = (INT32U)ReadEpsonRegShort(YParam.CoorRegAddr);
	if( (INT16U)YParam.lastCoor < 0x4000 && curr > 0xC000)
		YParam.lastCoor -= 0x10000;
	else if((INT16U)YParam.lastCoor > 0xC000 && curr < 0x4000)
		YParam.lastCoor += 0x10000;
	YParam.lastCoor &= 0xFFFF0000;
	YParam.lastCoor |= curr;
	currCoor = YParam.lastCoor;

	//YParam.Region_Start += YParam.Origin_Offset;
	YParam.Region_Start = YParam.Origin_Offset;
	YParam.Region_End = currCoor;
	YParam.Region_End += YParam.Origin_Offset;
}

#endif

INT8U StartFindXHome()
{
	if(GetSensor(&XParam.Origin) == False)
	{
		InitStatus = IS_X_GoOriginSensor;
		ConfigureSensorIt(&XParam.Origin, XOrigin_FPGA_Irq_Interrupt, NULL, XOrigin_IO_Irq_Interrupt); 
		return User_Start_X(0, 720, False, MOVETYPE_CONTINUE_CHECKSENSOR, 0, 0, DIRTYPE_NEG);
	}
	else
	{
		InitStatus = IS_X_LeaveOriginSensor;
		ConfigureSensorIt(&XParam.Origin, XOrigin_FPGA_Irq_Interrupt, NULL, XOrigin_IO_Irq_Interrupt); 
		return User_Start_X(0, 720, False, MOVETYPE_CONTINUE_CHECKSENSOR, 0, 0, DIRTYPE_POS);
	}
}
#if defined(INWEAR_FLAT_FIND_Y_HOME)
INT8U StartFindYHome()
{
	//ConfigureSensorIt(&YParam.Limit, NULL, NULL, YLimit_IO_Irq_Interrupt); 
	if(GetSensor(&YParam.Origin) == False)
	{
		InitStatus = IS_Y_GoOriginSensor;
		ConfigureSensorIt(&YParam.Origin, YOrigin_FPGA_Irq_Interrupt, NULL, YOrigin_IO_Irq_Interrupt); 
		return User_Start_Y(1, MOVETYPE_CONTINUE, 0, 0, DIRTYPE_NEG, False, False);
	}
	else
	{
		InitStatus = IS_Y_LeaveOriginSensor;
		//ConfigureSensorIt(&YParam.Origin, YOrigin_FPGA_Irq_Interrupt, NULL, YOrigin_IO_Irq_Interrupt); 
		//return User_Start_Y(0, MOVETYPE_CONTINUE, 0, 0, DIRTYPE_POS, False, False);
		return User_Start_Y(1, MOVETYPE_SEGMENT, 1, 200000, DIRTYPE_POS, False, False);
	}
}

INT8U StartFindYLimit()
{
	if(GetSensor(&YParam.Limit) == False)
	{
		InitStatus = IS_Y_GoLimitSensor;
		ConfigureSensorIt(&YParam.Limit, NULL, NULL, YLimit_IO_Irq_Interrupt); 
		return User_Start_Y(0, MOVETYPE_CONTINUE, 0, 0, DIRTYPE_POS, False, False);
	}
	else
	{
		InitStatus = IS_Y_LeaveLimitSensor;
		//ConfigureSensorIt(&YParam.Limit, NULL, NULL, YLimit_IO_Irq_Interrupt); 
		//return User_Start_Y(0, MOVETYPE_CONTINUE, 0, 0, DIRTYPE_NEG, False, False);
		return User_Start_Y(0, MOVETYPE_SEGMENT, 1, 20000, DIRTYPE_NEG, False, False);
	}
}

INT8U StartGoXSafePostion()
{

		InitStatus = IS_X_GoSafePosition;
		return User_Start_X(0, 720, False, MOVETYPE_SEGMENT, 0, 4000, DIRTYPE_POS);
}

INT8U FirstGoYHome()
{
	InitStatus = IS_Y_GoHome;
	//return User_Start_Y(0, MOVETYPE_SEGMENT, 1, 20000, DIRTYPE_NEG, False, False);
	//return User_Start_Y(0, MOVETYPE_SEGMENT, 1, 20000, DIRTYPE_POS, False, False);
	//return User_Start_X(0, 720, False, MOVETYPE_SEGMENT, 0, 0, DIRTYPE_NEG);
	return User_Start_Y(0, MOVETYPE_SEGMENT, 0, 0, DIRTYPE_NEG, False, False);
}
#endif

INT8U FirstGoXHome()
{
	UnConfigureSensorIt(&XParam.Origin);
	InitStatus = IS_X_GoHome;
	return User_Start_X(0, 720, False, MOVETYPE_SEGMENT, 2, OriginSensorPosi_X - XParam.OriginSensorOffset, DIRTYPE_POS);
}

static INT8U bHomeWhenPowerOn = True;

INT8U ResetXCoorSystem()
{
	OS_CPU_SR cpu_sr;
	INT32S X_posi = 0;
	INT32S X_print_posi = 0;
	
	OS_ENTER_CRITICAL();
#ifdef   HEAD_EPSON_GEN5		
	X_posi = ReadSafeEpsonRegInt(XParam.CoorRegAddr);
	X_print_posi = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR) - XORIGIN_OFFSET;
	SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, 
				   ER_CoorCtrl_RESET_X | ReadSafeEpsonRegInt(EPSON_REGADDR_COOR_CTRL));
	SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, 
				   ~ER_CoorCtrl_RESET_X & ReadSafeEpsonRegInt(EPSON_REGADDR_COOR_CTRL));
#elif defined(HEAD_RICOH_G4)
	if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
	{
		X_posi = (INT32S)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
	}
	else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
	{
		X_posi = (INT32S)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
	}
	X_print_posi = (INT32S)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16) - XORIGIN_OFFSET;
	rFPGA_RICOH_COORCTRL_L = ER_CoorCtrl_RESET_X | rFPGA_RICOH_COORCTRL_L;
	rFPGA_RICOH_COORCTRL_L = ~ER_CoorCtrl_RESET_X & rFPGA_RICOH_COORCTRL_L;
#endif
	XParam.CoorIdeal -= X_posi - XParam.Origin_Offset; 
	XParam.Coor_NextCycle -= X_posi - XParam.Origin_Offset;
	if(pPWMC_Driver1->AxisID == AXIS_ID_X)
		LAST_COOR[0]  = 0xFFFFFFFF;
	else if(pPWMC_Driver2->AxisID == AXIS_ID_X)
		LAST_COOR[1]  = 0xFFFFFFFF;
#if defined(HEAD_EPSON_GEN5)
	if((fwInfo.hd_version  & 0xFFFFFFFF) >= 0x03030101 && !Already_Reset_FPGA)
	{
		FPGA_RESETAGAIN();
		Already_Reset_FPGA = True;
	}
#endif	 
	OS_EXIT_CRITICAL();
	
#ifdef MICOLOR_AUTOFUCTION
	if( (- X_print_posi) > cleanparam_EPSON_MICOLOR.factory.Carriage_X_ReleasePos)
		bHomeWhenPowerOn = False;
	else
		bHomeWhenPowerOn = True;
#endif
	return True;
}

INT8U IsHomeWhenPowerOn()
{
	return bHomeWhenPowerOn;
}

INT8U ResetYCoorSystem()
{
	OS_CPU_SR cpu_sr;
	INT32S X_posi = 0;
	
	OS_ENTER_CRITICAL();
#ifdef   HEAD_EPSON_GEN5		
	SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, 
				   ER_CoorCtrl_RESET_Y | ReadSafeEpsonRegInt(EPSON_REGADDR_COOR_CTRL));
	SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, 
				   ~ER_CoorCtrl_RESET_Y & ReadSafeEpsonRegInt(EPSON_REGADDR_COOR_CTRL));
#elif defined(HEAD_RICOH_G4)
	rFPGA_RICOH_COORCTRL_L = ER_CoorCtrl_RESET_Y | rFPGA_RICOH_COORCTRL_L;
	rFPGA_RICOH_COORCTRL_L = ~ER_CoorCtrl_RESET_Y & rFPGA_RICOH_COORCTRL_L;
#endif		
	//YParam.Origin_Offset = 0;
	YParam.CoorIdeal = YParam.Origin_Offset;
	YParam.Coor_nonRollback = YParam.CoorIdeal;
	YParam.Coor_NextCycle = YParam.CoorIdeal;
	YParam.Coor_NextCycle_Frac = 0;
	YParam.ratio_StepPerCycle = 0;
	YParam.lastCoor = 0;

	if(pPWMC_Driver1->AxisID == AXIS_ID_Y)
		LAST_COOR[0]  = 0xFFFFFFFF;
	else if(pPWMC_Driver2->AxisID == AXIS_ID_Y)
		LAST_COOR[1]  = 0xFFFFFFFF;
	OS_EXIT_CRITICAL();
	
	return True;
}

INT8U EndInitAction()
{
	InitStatus = IS_Send_INIT_STAGE2;
	INT8U cmdbuf[32];
	
	cmdbuf[0] = 6;
	cmdbuf[1] = UART_INIT_STAGE2_CMD;
	*((__packed INT32U *)&cmdbuf[2]) = (INT32U)((XParam.Region_End - XParam.Origin_Offset)/XParam.ratio_move); // the end position of X move region.
	UART_MotionReportCMD(cmdbuf);
	
	cmdbuf[0] = 2 + 4 * sizeof(INT32U);
	cmdbuf[1] = UART_DSP_RPT_PHYSICAL_SIZE;
	*((__packed INT32U *)&cmdbuf[2]) = (INT32U)((XParam.Region_End - XParam.Origin_Offset)/XParam.ratio_move); 
	*((__packed INT32U *)&cmdbuf[6]) = (INT32U)((YParam.Region_End - YParam.Origin_Offset)/YParam.ratio_move); 
	*((__packed INT32U *)&cmdbuf[10]) = (INT32U)((ZParam.Region_End - ZParam.Origin_Offset)/ZParam.ratio_move); 
	*((__packed INT32U *)&cmdbuf[14]) = (INT32U)((CParam.Region_End - CParam.Origin_Offset)/CParam.ratio_move); 
	UART_MotionReportCMD(cmdbuf);
	
	cmdbuf[0] = 2 + 4;
	cmdbuf[1] = UART_DSP_RPT_PRIVATE_DATA;
	cmdbuf[2] = 0;
	cmdbuf[3] = 0;
	cmdbuf[4] = 0;
	cmdbuf[5] = 0;
	UART_MotionReportCMD(cmdbuf);
	
	mainStatus = MS_IDLE;
	return True;
}

INT8U ResetTCCoor(struct AxisParam_PulseDir_OpenLoop * pStepperTC)
{
	pStepperTC->CoorIdeal = pStepperTC->CurCoor = pStepperTC->Origin_Offset;
	return False;
}

#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)
INT8U MultiAxisMoveDistance(INT8U dir, INT8U speedid, INT32S distance)
{
	INT8U i;
	if((dir & 0x3F) != 0xC)
	{
		status_ReportStatus(STATUS_FTA + SciError_Parameter, STATUS_SET);
		return False;
	}
	
	mainStatus = MS_MULTI_AXIS_MOVE_PARALLEL;
	MultiAxis = 3;
	if(dir & 0x40)
	{
		i = User_Start_Z(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_POS);
		i &= User_Start_C(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_POS);
	}
	else
	{
		i = User_Start_Z(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_NEG);
		i &= User_Start_C(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_NEG);
	}
	return i;
}
#endif

INT8U MoveDistance(INT8U dir, INT8U speedid, INT32S distance)
{
	if(mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)
	if(dir & 0x80)
	{
		return MultiAxisMoveDistance(dir, speedid, distance);
	}
#endif
	
	if(dir == 1 || dir == 2)
	{
		switch(speedid)
		{
		default:
		case 2:
			speedid = 0;
			break;
		case 0:
		case 1:
			speedid = 1;
			break;
		case 3:
			speedid = 2;
			break;
		}
	}
	mainStatus = MS_NORMAL_MOVE;
	switch(dir)
	{
	case 1:
		return User_Start_X(speedid, 720, False, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_POS);
	case 2:
		return User_Start_X(speedid, 720, False, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_NEG);
	case 3:
		return User_Start_Y(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_POS, False, False);
	case 4:
		return User_Start_Y(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_NEG, False, False);
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

INT8U XMoveTo(INT8U speedid, INT32S targetPos)
{
	if(mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	switch(speedid)
	{
	default:
	case 2:
		speedid = 0;
		break;
	case 0:
	case 1:
		speedid = 1;
		break;
	case 3:
		speedid = 2;
		break;
	}
	mainStatus = MS_NORMAL_MOVE;
	return User_Start_X(speedid, 720, False, MOVETYPE_SEGMENT, 0, targetPos, 0);
}

INT8U AxisMoveTo(INT8U axis_no, INT8U speedid, INT32S targetPos)
{
	if( mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	
	if(axis_no == 1)
	{
		switch(speedid)
		{
		default:
		case 2:
			speedid = 0;
			break;
		case 0:
		case 1:
			speedid = 1;
			break;
		case 3:
			speedid = 2;
			break;
		}
	}
	mainStatus = MS_NORMAL_MOVE;
	switch(axis_no)
	{
	case 1:
		return User_Start_X(speedid, 720, False, MOVETYPE_SEGMENT, 0, targetPos, 0);
	case 2:
		return User_Start_Y(speedid, MOVETYPE_SEGMENT, 0, targetPos, 0, False, False);
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

INT8U YMoveDistance(INT8U dir, INT8U speedid, INT32S distance)
{
	if(mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	
	mainStatus = MS_NORMAL_MOVE;
	if(distance != 0)
	{
		if(dir == 1)
			return User_Start_Y(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_POS, False, False);
		else
			return User_Start_Y(speedid, MOVETYPE_SEGMENT, 1, distance, DIRTYPE_NEG, False, False);
	}
	else
	{
		if(dir == 1)
			return User_Start_Y(speedid, MOVETYPE_CONTINUE, 0, 0, DIRTYPE_POS, False, False);
		else
			return User_Start_Y(speedid, MOVETYPE_CONTINUE, 0, 0, DIRTYPE_NEG, False, False);
	}
}

INT8U StopMove()
{
	if((mainStatus != MS_NORMAL_MOVE) && (mainStatus != MS_MULTI_AXIS_MOVE_PARALLEL))
		return False;
	
	User_Stop_X(False, False);
	User_Stop_Y(False);
	User_Stop_Z(False);
	User_Stop_C(False);
	
	return True;
}

#define DEFAULT_SPEED_ID    1
#define MEASURE_SPEED_ID    1

INT8U GoHome_AxisNO = 0;
INT8U StartGoHome(INT8U axis_no)
{
	if( mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	mainStatus = MS_MULTI_AXIS_MOVE_SERIAL;
	SerialMultiAxisMoveStat = SMAM_IDLE;
	
	GoHome_AxisNO = axis_no;
	
	if(GoHome_AxisNO & 4)
	{
		SerialMultiAxisMoveStat = SMAM_Z_MOVE;
		return User_Start_Z(DEFAULT_SPEED_ID, MOVETYPE_SEGMENT, 0, 0, 0); 
	}
	else if(GoHome_AxisNO & 8)
	{
		SerialMultiAxisMoveStat = SMAM_C_MOVE;
		return User_Start_C(DEFAULT_SPEED_ID, MOVETYPE_SEGMENT, 0, 0, 0);
	}
	else if(GoHome_AxisNO & 1)
	{
		SerialMultiAxisMoveStat = SMAM_X_MOVE;
		return User_Start_X(DEFAULT_SPEED_ID, 720, False, MOVETYPE_SEGMENT, 0, 0, 0);
	}
	else if(GoHome_AxisNO & 2)
	{
		SerialMultiAxisMoveStat = SMAM_Y_MOVE;
		return User_Start_Y(DEFAULT_SPEED_ID, MOVETYPE_SEGMENT, 0, 0, 0, False, False);
	}
	else
	{
		mainStatus = MS_IDLE;
		SerialMultiAxisMoveStat = SMAM_IDLE;
		status_ReportStatus(STATUS_FTA + SciError_Parameter, STATUS_SET);
		return False;
	}
}

INT8U ContinueGoHome()
{
	INT8U cmdbuf[32];
	
	if( mainStatus != MS_MULTI_AXIS_MOVE_SERIAL)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	
	if((GoHome_AxisNO & 8) && SerialMultiAxisMoveStat < SMAM_C_MOVE)
	{
		SerialMultiAxisMoveStat = SMAM_C_MOVE;
		return User_Start_C(DEFAULT_SPEED_ID, MOVETYPE_SEGMENT, 0, 0, 0);
	}
	else if((GoHome_AxisNO & 1) && SerialMultiAxisMoveStat < SMAM_X_MOVE)
	{
		SerialMultiAxisMoveStat = SMAM_X_MOVE;
		return User_Start_X(DEFAULT_SPEED_ID, 720, False, MOVETYPE_SEGMENT, 0, 0, 0);
	}
	else if((GoHome_AxisNO & 2) && SerialMultiAxisMoveStat < SMAM_Y_MOVE)
	{
		SerialMultiAxisMoveStat = SMAM_Y_MOVE;
		return User_Start_Y(DEFAULT_SPEED_ID, MOVETYPE_SEGMENT, 0, 0, 0, False, False);
	}
	else
	{
		mainStatus = MS_IDLE;
		SerialMultiAxisMoveStat = SMAM_IDLE;
		
		cmdbuf[0] = 2 + 4 * sizeof(INT32S);
		cmdbuf[1] = UART_DSP_RPT_STOP;
		*((__packed INT32S *)&cmdbuf[2]) = (INT32S)((XParam.CoorIdeal - XParam.Origin_Offset)/XParam.ratio_move); 
		*((__packed INT32S *)&cmdbuf[6]) = (INT32S)((YParam.CoorIdeal - YParam.Origin_Offset)/YParam.ratio_move); 
		*((__packed INT32S *)&cmdbuf[10]) = (INT32S)((ZParam.CoorIdeal - ZParam.Origin_Offset)/ZParam.ratio_move); 
		*((__packed INT32S *)&cmdbuf[14]) = (INT32S)((CParam.CoorIdeal - CParam.Origin_Offset)/CParam.ratio_move); 
		UART_MotionReportCMD(cmdbuf);
		return True;
	}
}

void InitMeasureInfo()
{
	memset(&MediaMeasureInfo, 0, sizeof(struct sMediaMeasureInfo));
	
	if( XParam.pAxisParam->Fixed_Position[MPID_MEASURE_END] != 0x80000000 &&
	   XParam.pAxisParam->Fixed_Position[MPID_MEASURE_END] != 0xFFFFFFFF)
	{
		MediaMeasureInfo.sensor_offset = XParam.pAxisParam->Fixed_Position[MPID_MEASURE_END];
		MediaMeasureInfo.moveRegion = (XParam.pAxisParam->Fixed_Position[MPID_FLATEND] - 
									   MediaMeasureInfo.sensor_offset + 360) ;
	}
	else
	{
		MediaMeasureInfo.moveRegion = XParam.Region_End/XParam.ratio_move;
		MediaMeasureInfo.sensor_offset = 0;
	}
#ifdef SET_ORIGIN_OFFSET
	Media_sensor_offset = MediaMeasureInfo.sensor_offset;
#endif
	MediaMeasureInfo.status = MMS_Init;
	ConfigSensorPin(&MediaMeasureInfo.media_sensor, XParam.pAxisParam, MSID_MEASURE);
	ConfigSensor(&MediaMeasureInfo.media_sensor);
}
#define MIN_MEDIA_WIDTH 720 //1 inch.
void Measure_IO_Irq_Interrupt(const Pin *pPin) FAST_FUNC_SECTION
{
	if(mainStatus == MS_MEASURE && MediaMeasureInfo.status == MMS_Measure && 
	   MediaMeasureInfo.region_count < MAX_MEASURE_POINT)
	{
		if(MediaMeasureInfo.region_count %2 == 0)
		{
			if(GetSensor(&MediaMeasureInfo.media_sensor) == True)
			{
#ifdef   HEAD_EPSON_GEN5       
				MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = 
					ReadSafeEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
				if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
				{
					MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = (INT32S)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
				}
				else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
				{
					MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = (INT32S)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
				}
#endif								
			}
			MediaMeasureInfo.region_count++;
		}
		else
		{
			if(GetSensor(&MediaMeasureInfo.media_sensor) == False)
			{
#ifdef   HEAD_EPSON_GEN5       
				MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = 
					ReadSafeEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
				if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
				{
					MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = (INT32S)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
				}
				else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
				{
					MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = (INT32S)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
				}
#endif
				if(MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] - 
				   MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count -1] <= MIN_MEDIA_WIDTH)
				{
					MediaMeasureInfo.region_count--;
				}
				else
					MediaMeasureInfo.region_count++;
			}
		}
	}
}

void Measure_FPGA_Irq_Interrupt(void * data, INT8U IsHigh) FAST_FUNC_SECTION
{
	if(mainStatus == MS_MEASURE && MediaMeasureInfo.status == MMS_Measure && 
	   MediaMeasureInfo.region_count < MAX_MEASURE_POINT)
	{
		if(MediaMeasureInfo.region_count %2 == 0)
		{
			if(IsHigh == MediaMeasureInfo.media_sensor.ValidLevel)
			{
#ifdef   HEAD_EPSON_GEN5       
				MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = 
					ReadSafeEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
				if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
				{
					MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = (INT32S)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
				}
				else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
				{
					MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = (INT32S)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
				}
#endif
				MediaMeasureInfo.region_count++;
			}
			
		}
		else
		{
			if(IsHigh != MediaMeasureInfo.media_sensor.ValidLevel)
			{
#ifdef   HEAD_EPSON_GEN5       
				MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = 
					ReadSafeEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
				if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
				{
					MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = (INT32S)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
				}
				else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
				{
					MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] = (INT32S)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
				}
#endif
				if( MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count] - 
				   MediaMeasureInfo.RegionPoint[MediaMeasureInfo.region_count -1] <= MIN_MEDIA_WIDTH)
				{
					MediaMeasureInfo.region_count--;
				}
				else
					MediaMeasureInfo.region_count++;
			}
		}
	}
}

INT8U StartMeasure(INT8U method)
{
	if( mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	mainStatus = MS_MEASURE;
	MediaMeasureInfo.status = MMS_GoStart;
	MediaMeasureInfo.method = method;
	return User_Start_X(DEFAULT_SPEED_ID, 720, False, MOVETYPE_SEGMENT, 0, 0, 0);
}

INT8U ContinueMeasure()
{
	if( mainStatus != MS_MEASURE || (MediaMeasureInfo.status != MMS_GoStart && 
									 MediaMeasureInfo.status != MMS_Measure && MediaMeasureInfo.status != MMS_GoHome ))
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	switch (MediaMeasureInfo.status)
	{
	case MMS_GoStart:
		MediaMeasureInfo.status = MMS_Measure;
		ConfigureSensorIt(&MediaMeasureInfo.media_sensor, Measure_FPGA_Irq_Interrupt, NULL, Measure_IO_Irq_Interrupt); 
		return User_Start_X(MEASURE_SPEED_ID, 720, False, MOVETYPE_SEGMENT, 0, MediaMeasureInfo.moveRegion, 0);
	case MMS_Measure:
		MediaMeasureInfo.status = MMS_GoHome;
		UnConfigureSensorIt(&MediaMeasureInfo.media_sensor); 
		return User_Start_X(DEFAULT_SPEED_ID, 720, False, MOVETYPE_SEGMENT, 0, 0, 0);
	case MMS_GoHome:
		MediaMeasureInfo.status = MMS_Init;
		mainStatus = MS_IDLE;
		
		{
			INT32S max_width = 0, cur_width;
			INT8U i, index = 0;
			INT8U cmdbuf[32];
			
			cmdbuf[0] = 2 + 4 * sizeof(INT32S);
			cmdbuf[1] = UART_DSP_RPT_STOP;
			*((__packed INT32S *)&cmdbuf[2]) = (INT32S)((XParam.CoorIdeal - XParam.Origin_Offset)/XParam.ratio_move);  
			*((__packed INT32S *)&cmdbuf[6]) = (INT32S)((YParam.CoorIdeal - YParam.Origin_Offset)/YParam.ratio_move); 
			*((__packed INT32S *)&cmdbuf[10]) = (INT32S)((ZParam.CoorIdeal - ZParam.Origin_Offset)/ZParam.ratio_move); 
			*((__packed INT32S *)&cmdbuf[14]) = (INT32S)((CParam.CoorIdeal - CParam.Origin_Offset)/CParam.ratio_move); 
			UART_MotionReportCMD(cmdbuf);
			
			for(i=0; i<MediaMeasureInfo.region_count/2; i++)
			{
				cur_width = MediaMeasureInfo.RegionPoint[i*2+1] - MediaMeasureInfo.RegionPoint[i*2];
				if( cur_width > max_width)
				{
					max_width = cur_width; 
					index = i;
				}
			}
			MediaMeasureInfo.region_count = 0;
			cmdbuf[0] = 2 + 2 + 4 * sizeof(INT32S);
			cmdbuf[1] = UART_DSP_RPT_MEDIA;
			if(max_width > 0)
			{
				cmdbuf[2] = 1; //volume number
				cmdbuf[3] = 0; //volume index.
				*((__packed INT32S *)&cmdbuf[4]) = (INT32S)((MediaMeasureInfo.RegionPoint[index*2] - XParam.Origin_Offset)/XParam.ratio_move) + 
					MediaMeasureInfo.sensor_offset - MEASURE_ERROR_OFFSET;
				*((__packed INT32S *)&cmdbuf[8]) = (INT32S)((MediaMeasureInfo.RegionPoint[index * 2 + 1] - XParam.Origin_Offset)/XParam.ratio_move) + 
					MediaMeasureInfo.sensor_offset + MEASURE_ERROR_OFFSET;
				*((__packed INT32S *)&cmdbuf[12]) = 0;
				*((__packed INT32S *)&cmdbuf[16]) = 0;
			}
			else
				cmdbuf[2] = 0;
			return UART_MotionReportCMD(cmdbuf);
		}
	default:
		return False;
	}
}

#define Y_PRINT_SPEED 2
INT8U SetPrintParam(INT8U baseDPI, INT8U encoder_div, INT8U vsd_model)
{
	OS_CPU_SR cpu_sr;
	
	switch(baseDPI)
	{
	case 0:
		printInfo.baseDPI = 720;
		break;
	case 1:
		printInfo.baseDPI = 540;
		break;
	case 2:
		printInfo.baseDPI = 1440;
		break;
	default:
		return False;
	}
	
	if(encoder_div >= 1 && encoder_div <= 4)
		printInfo.encoder_div = encoder_div;
	else
		return False;
	
	if(vsd_model >= 1 && vsd_model <= 4)
		printInfo.VSDModel = vsd_model - 1;
	else
		return False;
	
	printInfo.WaitCompletedFlag = 0;
	printInfo.YSpeed = Y_PRINT_SPEED;
	return True;
}

INT8U SetPrintYDir(INT8U YDir)
{
	if( YDir == 0 || YDir == 1)
	{
		printInfo.YDir = YDir;
		return True;
	}
	else
		return False;
}

void FPGAPosi_OutOfPrintRegion(void * data) FAST_FUNC_SECTION
{
	INT32U Position;
	INT8U Option;
	if(printInfo.YDir == 0)
		User_Start_Y(printInfo.YSpeed, MOVETYPE_SEGMENT, 1, printInfo.Y_Step, DIRTYPE_POS, True, True);
	else
		User_Start_Y(printInfo.YSpeed, MOVETYPE_SEGMENT, 1, printInfo.Y_Step, DIRTYPE_NEG, True, True);
	
	if(GetPendingConfig(0, &Position, &Option))
	{
		PopPendingConfig();
	}
}

void CalcYStepSpeed(INT32S Y_Step)
{
	INT32S i;
	INT8U speedID = MapXUserSpeedToXMotorSpeed(printInfo.VSDModel, printInfo.baseDPI/printInfo.encoder_div, True);
	INT32S xAccTick = XParam.pAllCurveTable[speedID]->AccCurveLen;
	
	INT32S xDis = X_PRINT_ACC_SAFE_DISTANCE;
	xDis -= MIN_PERMIT_X_PRINT_ERROR;
	xDis *= XParam.ratio_move;
	xAccTick += (INT32S)(xDis / ((float)XParam.ratioTable_StepPerCycle[speedID]/(1<<24)));
	
	if( Y_Step < 0)
		Y_Step = - Y_Step;
	
	for(i= 1; i<MAX_Y_SPEED_LEVEL; i++)
	{
		INT32S yAccTick = YParam.pAllCurveTable[i]->AccCurveLen * 2;
		INT32S yAccDistance = YParam.pAllCurveTable[i]->AccDistance * 2;
		
#ifndef OLD_Y_SPEED_SELECT_WAY  
		if( Y_Step > yAccDistance)
		{
			yAccDistance = (INT32S)((Y_Step - yAccDistance)/YParam.ratio_move);
			yAccTick += (INT32S)(yAccDistance / ((float)YParam.ratioTable_StepPerCycle[i]/(1<<24)));;
			if(yAccTick <= xAccTick)
			{
				if(i>1)
					printInfo.YSpeed = i -1;
				else
					printInfo.YSpeed = 1;
				return;
			}
		}
		else
		{
			//            yAccTick = yAccTick*yAccTick/yAccDistance;
			//            yAccTick *= Y_Step;
			//            if(yAccTick <= xAccTick*xAccTick)
			{
				if(i>1)
					printInfo.YSpeed = i -1;
				else
					printInfo.YSpeed = 1;
				return;
			}
			
		}
#else
		if( Y_Step > yAccDistance)
		{
			yAccDistance = (INT32S)((Y_Step - yAccDistance)/YParam.ratio_move);
			yAccTick += (INT32S)(yAccDistance / ((float)YParam.ratioTable_StepPerCycle[i]/(1<<24)));;
		}
		
		if(yAccTick <= xAccTick)
		{
			//printInfo.WaitCompletedFlag = MOTION_MOVE_COMPLETED_X | MOTION_MOVE_COMPLETED_Y;
			if(i>1)
				printInfo.YSpeed = i -1;
			else
				printInfo.YSpeed = 1;
			return;
		}
#endif        
		
	}    
	//printInfo.WaitCompletedFlag = MOTION_MOVE_COMPLETED_X | MOTION_MOVE_COMPLETED_Y;
	printInfo.YSpeed = i - 1;
}


INT8U pos_index =0;
INT32U changePos[10] = {0};
#define AUTO_FLASH_FREQ     200 //us
#define FLASH_SAFE_DISTANCE 30
void FPGAPosi_EndFlash(void * data) FAST_FUNC_SECTION;
void FPGAPosi_BeginFPGABand(void * data) FAST_FUNC_SECTION;

void FPGAPosi_BeginFlash(void * data) FAST_FUNC_SECTION
{
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
#ifdef   HEAD_EPSON_GEN5 
	changePos[pos_index] = ReadSafeEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);
#endif
	pos_index ++;
	if(pos_index == sizeof(changePos)/sizeof(int))
	{
		pos_index = 0;
	}
#ifdef   HEAD_EPSON_GEN5    
	if(GetCleanPartCount() > 1)
	{
		if(printInfo.curSprayNo == 0)
		{
			if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
			{
				FPGA_START_FLASH(cleanparam_EPSON_MICOLOR.SprayInterval, 1);
				SetTargetPosi(0, printInfo.PrintFlash_End, 5);
			}
			else if(printInfo.PrintFlash_Start == printInfo.PrintFlash_Start_1)
			{
				FPGA_START_FLASH(cleanparam_EPSON_MICOLOR.SprayInterval, 3);
				SetTargetPosi(0, printInfo.PrintFlash_End, 5);
			}
			else
			{
				FPGA_START_FLASH(cleanparam_EPSON_MICOLOR.SprayInterval, 2);
				SetTargetPosi(0, printInfo.PrintFlash_End_1, 5);
			}
		}
		else if(printInfo.curSprayNo == 1)
		{
			FPGA_START_FLASH(cleanparam_EPSON_MICOLOR.SprayInterval, 0);
			if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
				SetTargetPosi(0, printInfo.PrintFlash_End + FLASH_SAFE_DISTANCE * 2, 5);
			else
				SetTargetPosi(0, printInfo.PrintFlash_End_1 + FLASH_SAFE_DISTANCE * 2, 5);
		}
		else if(printInfo.curSprayNo == 2)
		{
			if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
			{
				FPGA_START_FLASH(cleanparam_EPSON_MICOLOR.SprayInterval, 2);
				SetTargetPosi(0, printInfo.PrintFlash_End_1, 5);
			}
			else
			{
				FPGA_START_FLASH(cleanparam_EPSON_MICOLOR.SprayInterval, 1);
				SetTargetPosi(0, printInfo.PrintFlash_End, 5);
			}
		}
		else if(printInfo.curSprayNo == 3)
		{
			FPGA_START_FLASH(cleanparam_EPSON_MICOLOR.SprayInterval, 0);
			if(printInfo.PrintFlash_Start < printInfo.PrintFlash_Start_1)
				SetTargetPosi(0, printInfo.PrintFlash_End_1 + FLASH_SAFE_DISTANCE * 2, 5);
			else
				SetTargetPosi(0, printInfo.PrintFlash_End + FLASH_SAFE_DISTANCE * 2, 5);
		}
	}
	else
	{
		FPGA_START_FLASH(cleanparam_EPSON_MICOLOR.SprayInterval, 1);
		SetTargetPosi(0, printInfo.PrintFlash_End, 5);
	}
#elif defined(HEAD_RICOH_G4)
	//un-implement
#endif
	ConfigFPGAPosiIT(0, FPGAPosi_EndFlash, NULL);
#endif
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
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
		ConfigFPGAPosiIT(0, FPGAPosi_BeginFlash, NULL);
		printInfo.curSprayNo ++;
#endif
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
		
		if(FpgaPosiITInfos_pending[0].bPending == True)
		{
			if(printInfo.PrintFire_End + XParam.Origin_Offset_Feedback > FpgaPosiITInfos_pending[0].Position)
			{
				PopPendingConfig();
				RegPendingConfig(0, printInfo.PrintFire_End + XParam.Origin_Offset_Feedback, 5, FPGAPosi_OutOfPrintRegion, 0);
			}
			else
			{
				SetTargetPosi(0, printInfo.PrintFire_End + XParam.Origin_Offset_Feedback, 5);
				ConfigFPGAPosiIT(0, FPGAPosi_OutOfPrintRegion, NULL);	
			}
		}
		else
		{
			SetTargetPosi(0, printInfo.PrintFire_End + XParam.Origin_Offset_Feedback, 5);
			ConfigFPGAPosiIT(0, FPGAPosi_OutOfPrintRegion, NULL);		 
		}
	}
	else
	{
		SetTargetPosi(0, curPos + FLASH_SAFE_DISTANCE, 5);
	}
}
#endif  
void ConfigPrintReg()
{
	OS_CPU_SR cpu_sr;
	
	INT32S startFirePoint, endFirePoint, startPoint, endPoint;
	INT32U reg_ctrl, i;
	INT8U cmdbuf[32];
	
	startFirePoint = printInfo.PrintFire_Start;
	endFirePoint = printInfo.PrintFire_End;
	startPoint = printInfo.PrintMove_Start;
	endPoint = printInfo.PrintMove_End;
	
	printInfo.PrintMoveStat = 1;
	endFirePoint -= printInfo.encoder_div;
	
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
	if(printInfo.autoSpray)
	{
#ifdef   HEAD_EPSON_GEN5
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
#elif defined(HEAD_RICOH_G4)
		//unimplement.
#endif
	}
	else
#endif
	{
#ifdef   HEAD_EPSON_GEN5    
		if(endPoint > startPoint)
			SetTargetPosi(0, endFirePoint + XParam.Origin_Offset_Feedback, 5);
		else
			SetTargetPosi(0, startFirePoint + XParam.Origin_Offset_Feedback, 9);
		ConfigFPGAPosiIT(0, FPGAPosi_OutOfPrintRegion, NULL);
#elif defined(HEAD_RICOH_G4)
		if(FpgaPosiITInfos[0].Enable == True)
		{
			if(endPoint > startPoint)
			{
				if(endFirePoint + XParam.Origin_Offset_Feedback < FpgaPosiITInfos[0].Position)
				{
					FpgaPosiITInfos[0].bPending == True;
					PushCurConfig();
					DisableFPGAPosiIT(0);
					SetTargetPosi(0, endFirePoint + XParam.Origin_Offset_Feedback, 5);
					ConfigFPGAPosiIT(0, FPGAPosi_OutOfPrintRegion, NULL);
				}
				else
				{
					RegPendingConfig(0, endFirePoint + XParam.Origin_Offset_Feedback, 5, FPGAPosi_OutOfPrintRegion, 0);
				}
			}
			else
			{
				if(startFirePoint + XParam.Origin_Offset_Feedback > FpgaPosiITInfos[0].Position)
				{
					FpgaPosiITInfos[0].bPending == True;
					PushCurConfig();
					DisableFPGAPosiIT(0);
					SetTargetPosi(0, startFirePoint + XParam.Origin_Offset_Feedback, 9);
					ConfigFPGAPosiIT(0, FPGAPosi_OutOfPrintRegion, NULL);
				}
				else
				{
					RegPendingConfig(0, startFirePoint + XParam.Origin_Offset_Feedback, 9, FPGAPosi_OutOfPrintRegion, 0);
				}
			}
		}
		else
		{
			if(endPoint > startPoint)
				SetTargetPosi(0, endFirePoint + XParam.Origin_Offset_Feedback, 5);
			else
				SetTargetPosi(0, startFirePoint + XParam.Origin_Offset_Feedback, 9);
			ConfigFPGAPosiIT(0, FPGAPosi_OutOfPrintRegion, NULL);
		}
		EnableFPGAPosiIT(0);
#endif
	}
	
	switch(printInfo.encoder_div)
	{
	case 1:
		reg_ctrl = ER_CoorCtrl_PRINT_DIVIDER_NONE;
		break;
	case 2:
		reg_ctrl = ER_CoorCtrl_PRINT_DIVIDER_HALF;
		break;
	case 3:
		reg_ctrl = ER_CoorCtrl_PRINT_DIVIDER_THREE;
		break;
	case 4:
		reg_ctrl = ER_CoorCtrl_PRINT_DIVIDER_QUAD;
		break;
	}
	
	switch(startFirePoint % printInfo.encoder_div)
	{
	case 0:
		reg_ctrl |= ER_CoorCtrl_PRINT_PHASE_0;
		break;
	case 1:
		reg_ctrl |= ER_CoorCtrl_PRINT_PHASE_1;
		break;
	case 2:
		reg_ctrl |= ER_CoorCtrl_PRINT_PHASE_2;
		break;
	case 3:
		reg_ctrl |= ER_CoorCtrl_PRINT_PHASE_3;
		break;
	}
	if(endPoint > startPoint)
		reg_ctrl |= ER_CoorCtrl_EN_DIR_POS;
	else
		reg_ctrl |= ER_CoorCtrl_EN_DIR_NEG;
	switch(printInfo.VSDModel)
	{
	case 0:
		reg_ctrl |= ER_CoorCtrl_VSDMODEL_VSD1;
		break;
	case 1:
		reg_ctrl |= ER_CoorCtrl_VSDMODEL_VSD2;
		break;
	case 2:
		reg_ctrl |= ER_CoorCtrl_VSDMODEL_VSD3;
		break;
	case 3:
		reg_ctrl |= ER_CoorCtrl_VSDMODEL_VSD4;
		break;
	}
	OS_ENTER_CRITICAL();
#ifdef   HEAD_EPSON_GEN5		
	i = ReadSafeEpsonRegInt(EPSON_REGADDR_COOR_CTRL);
#elif defined(HEAD_RICOH_G4)
	i = (INT32U)(rFPGA_RICOH_COORCTRL_L |rFPGA_RICOH_COORCTRLSTAT_H << 16);
#endif
	i &= (ER_CoorCtrl_SMOOTH_DIVIDER | ER_CoorCtrl_SMOOTH_MULTI | ER_CoorCtrl_FLASH);
	i |= (reg_ctrl | ER_CoorCtrl_FIRE);
#ifdef   HEAD_EPSON_GEN5		
	SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, i);
	SetEpsonRegInt(EPSON_REGADDR_X_FIRE_START, startFirePoint + XParam.Origin_Offset_Feedback);
	SetEpsonRegInt(EPSON_REGADDR_X_FIRE_END, endFirePoint + XParam.Origin_Offset_Feedback);
#elif defined(HEAD_RICOH_G4)
	i &= ~ER_CoorCtrl_FLASH;
	rFPGA_RICOH_COORCTRL_L =(INT16U)i;
	rFPGA_RICOH_COORCTRLSTAT_H =(INT16U)(i >> 16);
	rFPGA_RICOH_FIRESTART_L = (INT16U)(startFirePoint + XParam.Origin_Offset_Feedback);
	rFPGA_RICOH_FIREEND_L = (INT16U)(endFirePoint + XParam.Origin_Offset_Feedback);
	rFPGA_RICOH_FIRESTART_END_H = (INT16U)((((startFirePoint + XParam.Origin_Offset_Feedback) >> 16)&0xFF) |(((endFirePoint + XParam.Origin_Offset_Feedback)>>8)&0xFF00));
#endif
	OS_EXIT_CRITICAL();
#ifndef HEAD_RICOH_G4
	cmdbuf[0] = 6;
	cmdbuf[1] = UART_HEAD_EPSON_PASSSTART;
	cmdbuf[2] = (INT8U)FireCount ;
	cmdbuf[3] = (INT8U)(FireCount>>8);
	cmdbuf[4] = (INT8U)FireCountEx;
	cmdbuf[5] = (INT8U)(FireCountEx>>8);
	UART_SendCMD(UART_HEAD_CHANNEL, cmdbuf);
#endif
}

//When print width is too short, the movement is NOT smooth, specially if print speed is 360 DPI.
// So I add a min print distance to avoid this case.
#define MIN_PRINT_DISTANCE  (12*720) 

INT8U StartPrePrint(INT32S startFirePoint, INT32S endFirePoint, INT32S next_startFirePoint, INT32S next_endFirePoint)
{
	OS_CPU_SR cpu_sr;
	INT8U bEmptyPass[2] = {False,False};
	INT32S startPoint, next_startPoint;
	INT32S endPoint, next_endPoint;
	INT8U speedID;
	INT32S xAccDistance;
	INT32S startPoint_720;
	INT8U cmdbuf[32];
	static INT8U next_flash = False;
	if( mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	
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
	
	if(bEmptyPass[0] == True && bEmptyPass[1] == True)
	{
		cmdbuf[0] = 2 + 4 * sizeof(INT32S);
		cmdbuf[1] = UART_DSP_RPT_STOP;
		*((__packed INT32S *)&cmdbuf[2]) = (INT32S)((XParam.CoorIdeal - XParam.Origin_Offset)/XParam.ratio_move);  
		*((__packed INT32S *)&cmdbuf[6]) = (INT32S)((YParam.CoorIdeal - YParam.Origin_Offset)/YParam.ratio_move); 
		*((__packed INT32S *)&cmdbuf[10]) = (INT32S)((ZParam.CoorIdeal - ZParam.Origin_Offset)/ZParam.ratio_move); 
		*((__packed INT32S *)&cmdbuf[14]) = (INT32S)((CParam.CoorIdeal - CParam.Origin_Offset)/CParam.ratio_move); 
		return UART_MotionReportCMD(cmdbuf);
	}
	
	if(bEmptyPass[0] == False)
	{
		if(endPoint - startPoint < MIN_PRINT_DISTANCE)
		{
			endPoint = startPoint + MIN_PRINT_DISTANCE;
			if(endPoint > XParam.pAxisParam->Fixed_Position[MPID_FLATEND])
			{
				endPoint = XParam.pAxisParam->Fixed_Position[MPID_FLATEND];
				startPoint = endPoint - MIN_PRINT_DISTANCE;
			}
		}
	}
	if(bEmptyPass[1] == False)
	{
		if(next_endPoint - next_startPoint < MIN_PRINT_DISTANCE)
		{
			next_endPoint = next_startPoint + MIN_PRINT_DISTANCE;
			if(next_endPoint > XParam.pAxisParam->Fixed_Position[MPID_FLATEND])
			{
				next_endPoint = XParam.pAxisParam->Fixed_Position[MPID_FLATEND];
				next_startPoint = endPoint - MIN_PRINT_DISTANCE;
			}
			if(next_startFirePoint < next_endFirePoint)
			{
				next_startFirePoint = next_startPoint;
				next_endFirePoint = next_endPoint;
			}
			else
			{
				next_startFirePoint = next_endPoint;
				next_endFirePoint = next_startPoint;
			}
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
	
	speedID = MapXUserSpeedToXMotorSpeed(printInfo.VSDModel, printInfo.baseDPI/printInfo.encoder_div, True);
	xAccDistance = XParam.pAllCurveTable[speedID]->AccDistance + X_PRINT_ACC_SAFE_DISTANCE * XParam.ratio_move;
	
	switch(printInfo.baseDPI)
	{
	default:
	case 720:
		xAccDistance /= XParam.ratio_print[0];
		break;
	case 540:
		xAccDistance /= XParam.ratio_print[1];
		break;
	case 1440:
		xAccDistance /= XParam.ratio_print[2];
		break;
	}
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
	
	OS_ENTER_CRITICAL();
	printInfo.lastEndCoor = XParam.CoorIdeal - XParam.Origin_Offset;
	OS_EXIT_CRITICAL();
	
	printInfo.autoSpray = False;
	printInfo.PrintFlash_Start = 0;
	printInfo.PrintFlash_End = 0;
	printInfo.SprayPosType = 0;
	next_flash = False;
#if defined (EPSON_CLEAN_INTEGRATE) || defined (EPSON_CLEAN_INTEGRATE_1) || defined (EPSON_CLEAN_INTEGRATE_2)|| defined (EPSON_CLEAN_INTEGRATE_3)
	if( printInfo.PrintFire_Start != printInfo.PrintFire_End && cleanparam_EPSON_MICOLOR.bAutoSpray 
	   //&& printInfo.PrintMove_End > printInfo.PrintMove_Start   //only spray when positive direction
	   )
	{
		INT32U start, end, start1, end1;
		start = (cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_Start + 
				 cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_End - cleanparam_EPSON_MICOLOR.SprayWidth)/2;
		end = start + cleanparam_EPSON_MICOLOR.SprayWidth;
		
		start = start * printInfo.baseDPI/720;
		printInfo.PrintFlash_Start  = start + XParam.Origin_Offset_Feedback;
		printInfo.PrintFlash_End = end * printInfo.baseDPI/720 + XParam.Origin_Offset_Feedback;
		
		start1 = (cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_1_Start + 
				  cleanparam_EPSON_MICOLOR.factory.Carriage_X_SprayPos_1_End - cleanparam_EPSON_MICOLOR.SprayWidth)/2;
		end1 = start1 + cleanparam_EPSON_MICOLOR.SprayWidth;
		
		start1 = start1 * printInfo.baseDPI/720;
		printInfo.PrintFlash_Start_1  = start1 + XParam.Origin_Offset_Feedback;
		printInfo.PrintFlash_End_1 = end1 * printInfo.baseDPI/720 + XParam.Origin_Offset_Feedback;
		printInfo.curSprayNo = 0;
		
		printInfo.autoSpray = True;
		if( printInfo.PrintMove_End > printInfo.PrintMove_Start)
		{
			if(printInfo.PrintFlash_End < printInfo.PrintFire_Start + XParam.Origin_Offset_Feedback)
				printInfo.SprayPosType = 0;
			else
				printInfo.SprayPosType = 1;
		}
		else
		{
			if(printInfo.PrintFlash_End < printInfo.PrintFire_Start + XParam.Origin_Offset_Feedback)
				printInfo.SprayPosType = 2;
			else
				printInfo.SprayPosType = 3;
		}
		
		if(!(printInfo.SprayPosType == 0 || printInfo.SprayPosType == 3))
		{//currently, only support pre-flash. because FPGA code can not handle shake between pass.
			printInfo.autoSpray = False;
		}
		else if(!(cleanparam_EPSON_MICOLOR.passNum != 0 && printedBands != 0 && (printedBands - 1) % cleanparam_EPSON_MICOLOR.passNum == 0))
		{
			printInfo.autoSpray = False;
		}
		if(cleanparam_EPSON_MICOLOR.passNum != 0 && printedBands != 0 &&( (printedBands - 1) % cleanparam_EPSON_MICOLOR.passNum ==0))
		{
			next_flash = True;
		}				
		if(printInfo.autoSpray == True)
		{
			if( printInfo.PrintMove_End > printInfo.PrintMove_Start)
			{
				if(printInfo.PrintMove_Start > start - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720)
					printInfo.PrintMove_Start = start - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720 ;
				if(printInfo.PrintMove_Start > start1 - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720)
					printInfo.PrintMove_Start = start1 - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720 ;
			}
			else
			{
				if(printInfo.PrintMove_End > start - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720)
					printInfo.PrintMove_End = start - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720 ;
				if(printInfo.PrintMove_End > start1 - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720)
					printInfo.PrintMove_End = start1 - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720 ;
			}
		}
		else if(next_flash == True)
		{
			if( printInfo.PrintMove_End < printInfo.PrintMove_Start)
			{
				if(printInfo.PrintMove_End > start - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720)
					printInfo.PrintMove_End = start - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720 ;
				if(printInfo.PrintMove_End > start1 - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720)
					printInfo.PrintMove_End = start1 - MIN_FLASH_PRE_DISTANCE* printInfo.baseDPI/720 ;
			}
		}
	}
	//printInfo.autoSpray = False;
#endif
	startPoint_720 = printInfo.PrintMove_Start * 720 / printInfo.baseDPI;
	
	if( (printInfo.PrintMove_End > printInfo.PrintMove_Start && printInfo.lastEndCoor/XParam.ratio_move - MIN_PERMIT_X_PRINT_ERROR > startPoint_720) ||
	   (printInfo.PrintMove_End <= printInfo.PrintMove_Start && printInfo.lastEndCoor/XParam.ratio_move + MIN_PERMIT_X_PRINT_ERROR < startPoint_720))
	{
		mainStatus = MS_NORMAL_MOVE;
		return User_Start_X(4, printInfo.baseDPI/printInfo.encoder_div, True, MOVETYPE_SEGMENT, 0, printInfo.PrintMove_Start, 0);
	}
	else
	{
		cmdbuf[0] = 2 + 4 * sizeof(INT32S);
		cmdbuf[1] = UART_DSP_RPT_STOP;
		*((__packed INT32S *)&cmdbuf[2]) = (INT32S)((XParam.CoorIdeal - XParam.Origin_Offset)/XParam.ratio_move);  
		*((__packed INT32S *)&cmdbuf[6]) = (INT32S)((YParam.CoorIdeal - YParam.Origin_Offset)/YParam.ratio_move); 
		*((__packed INT32S *)&cmdbuf[10]) = (INT32S)((ZParam.CoorIdeal - ZParam.Origin_Offset)/ZParam.ratio_move); 
		*((__packed INT32S *)&cmdbuf[14]) = (INT32S)((CParam.CoorIdeal - CParam.Origin_Offset)/CParam.ratio_move); 
		return UART_MotionReportCMD(cmdbuf);
	}
}

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

INT8U StartPrint(INT32S cur_step_distance)
{
	OS_CPU_SR cpu_sr;
	
	if( mainStatus != MS_IDLE)
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	
	mainStatus = MS_PRINT;
	
	printInfo.Y_Step = cur_step_distance;
	if( printInfo.PrintFire_Start == printInfo.PrintFire_End)
	{
		printInfo.PrintMoveStat = 3;
		//CalcYStepSpeed(cur_step_distance); 
		//for skip-space case, the cur_step_distance can be very large, it will be too fast.
		printInfo.YSpeed = Y_PRINT_SPEED;
		printInfo.WaitCompletedFlag = MOTION_MOVE_COMPLETED_Y;
		FPGAPosi_OutOfPrintRegion(NULL);
		return True;
	}
	else
	{
		CalcYStepSpeed(cur_step_distance);
		printInfo.WaitCompletedFlag = MOTION_MOVE_COMPLETED_X | MOTION_MOVE_COMPLETED_Y;
		
		ConfigPrintReg();
		return User_Start_X(printInfo.VSDModel, printInfo.baseDPI/printInfo.encoder_div, True, MOVETYPE_SEGMENT, 0, printInfo.PrintMove_End, 0);
	}
}

INT8U ContinuePrint(INT32U flags)
{
	INT32U reg_ctrl, i;
	OS_CPU_SR cpu_sr;
	INT8U cmdbuf[32];
	
	if( mainStatus != MS_PRINT || (printInfo.PrintMoveStat != 1 && printInfo.PrintMoveStat != 3))
	{
		status_ReportStatus(STATUS_FTA + SciError_MoveAgain, STATUS_SET);
		return False;
	}
	
	if(flags & MOTION_MOVE_COMPLETED_X)
		printInfo.WaitCompletedFlag &= ~MOTION_MOVE_COMPLETED_X;
	if(flags & MOTION_MOVE_COMPLETED_Y)
		printInfo.WaitCompletedFlag &= ~MOTION_MOVE_COMPLETED_Y;
	
	if(printInfo.WaitCompletedFlag == 0)
	{
		DisableFPGAPosiIT(0);
		
		OS_ENTER_CRITICAL();
#ifdef   HEAD_EPSON_GEN5						
		SetEpsonRegInt(EPSON_REGADDR_COOR_CTRL, ~(ER_CoorCtrl_FIRE | ER_CoorCtrl_FLASH) & ReadEpsonRegInt(EPSON_REGADDR_COOR_CTRL));
#elif defined(HEAD_RICOH_G4)
		rFPGA_RICOH_COORCTRL_L = ~(ER_CoorCtrl_FIRE | ER_CoorCtrl_FLASH) & rFPGA_RICOH_COORCTRL_L;
#endif
		OS_EXIT_CRITICAL();
		
		printInfo.PrintMoveStat = 0;
		mainStatus = MS_IDLE;
		
		cmdbuf[0] = 2 + 4 * sizeof(INT32S);
		cmdbuf[1] = UART_DSP_RPT_STOP;
		*((__packed INT32S *)&cmdbuf[2]) = (INT32S)((XParam.CoorIdeal - XParam.Origin_Offset)/XParam.ratio_move);  
		*((__packed INT32S *)&cmdbuf[6]) = (INT32S)((YParam.CoorIdeal - YParam.Origin_Offset)/YParam.ratio_move); 
		*((__packed INT32S *)&cmdbuf[10]) = (INT32S)((ZParam.CoorIdeal - ZParam.Origin_Offset)/ZParam.ratio_move); 
		*((__packed INT32S *)&cmdbuf[14]) = (INT32S)((CParam.CoorIdeal - CParam.Origin_Offset)/CParam.ratio_move); 
		return UART_MotionReportCMD(cmdbuf);
	}
	
	return True;
}

void Motion_Task()
{
	INT8U err, ret;
	INT8U cmdbuf[32];
	OS_FLAGS flags;
#ifdef MOTION_Y_DSP		
	INT32U i = 0 ;
	INT8U len;
	INT16U addr;
	INT8U UartCMD[64];
#endif
	enum SciError reportErr = SciError_OK;
	
	ExitResetStatus();
	
	mainStatus = MS_INIT;
	InitStatus = IS_IDLE;
	SerialMultiAxisMoveStat = SMAM_IDLE;
	InitMeasureInfo();
	
	//sometime, when power on, Y will rotate some distances if motion control pin's status is not right.
	//  to reset Y coor system, will make Y-standy-process don't roll-back it. 
	//  Else, if this distance is large, it will cause paper-jap error.
	ResetYCoorSystem();
	StartAllStandby();  
	
	//    for(ret =0; ret <250; ret++)
	//        OSTimeDly(65000);
	//#ifdef MOTION_Y_DSP		
#if 0
	while(!UART_GetCMD(UART_DSP_CHANNEL, UartCMD))
	{
		i++;
		if(UartCMD[1] == UART_INIT_STAGE1_CMD)
			break;
		if(i>1000)
			break;
		OSTimeDly(10);
	}
	if(i < 1000)
		UART_MotionReportCMD(UartCMD);
	else
		status_ReportStatus(STATUS_FTA + SciError_InternalErr_1, STATUS_SET);
#else
	cmdbuf[0] = 6;
	cmdbuf[1] = UART_INIT_STAGE1_CMD;
	*((__packed INT32U *)&cmdbuf[2]) = 0; // The check sum of the code
	UART_MotionReportCMD(cmdbuf);
#endif
	InitStatus = IS_Send_INIT_STAGE1;
	
	CleanStatus = Clean_IDLE;
	
	/* ignored report cmd.
	//case UART_DSP_RPT_DESKEY:
case UART_DSP_RPT_DEBUG_INFO: //Motion report position
	//trig by USB cmd. then motion return UART_DSP_RPT_DEBUG_INFO cmd.
case UART_DSP_RPT_PIPE_DATA:
case UART_UPDATE_DSP_INFO:
case UART_DSP_RPT_STATUS: //abandon
case UART_DSP_REQ_READ_EEPROM:
case UART_DSP_WRITE_EEPROM:
	*/
	
	//    case UART_RPT_VERSION: ///int + string description. after init-stage_1
	//    case UART_DSP_RPT_PHYSICAL_SIZE: //after init_stage_2. 
	//    case UART_DSP_RPT_PRIVATE_DATA: //report motion config, security/encoder. after init_stage_2. Currently, report all zero.
	
	while(1)
	{
		if(mainStatus == MS_INIT && InitStatus == IS_Send_INIT_STAGE1)
		{ 
			flags = OSFlagPend(mix_FLAG_GRP,MOTION_SEND_CMD,OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME,10000,&err);
			
			if(flags == 0 && err == OS_TIMEOUT)
			{
				//resend again.
				cmdbuf[0] = 6;
				cmdbuf[1] = UART_INIT_STAGE1_CMD;
				*((__packed INT32U *)&cmdbuf[2]) = 0; // The check sum of the code
				UART_MotionReportCMD(cmdbuf);
				continue;
			}
		}
		else
		{
#ifdef MOTION_Y_DSP		        
			flags = OSFlagPend(mix_FLAG_GRP, 
							   MOTION_URGENT_STOP_CMD|MOTION_SEND_CMD|MOTION_MOVE_COMPLETED_X|MOTION_MOVE_COMPLETED_Y|MOTION_MOVE_COMPLETED_Z|MOTION_MOVE_COMPLETED_C, 
							   OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME, 5, &err);
#else
			flags = OSFlagPend(mix_FLAG_GRP, 
							   MOTION_URGENT_STOP_CMD|MOTION_SEND_CMD|MOTION_MOVE_COMPLETED_X|MOTION_MOVE_COMPLETED_Y|MOTION_MOVE_COMPLETED_Z|MOTION_MOVE_COMPLETED_C, 
							   OS_FLAG_WAIT_SET_ANY|OS_FLAG_CONSUME, 0, &err);
#endif
		}
		
		if(flags & MOTION_URGENT_STOP_CMD)
		{
			User_Stop_X(True, True);
			User_Stop_Y(True);
			User_Stop_Z(True);
			User_Stop_C(True);
			continue;
		}
		
		if(flags & (MOTION_MOVE_COMPLETED_X|MOTION_MOVE_COMPLETED_Y|MOTION_MOVE_COMPLETED_Z|MOTION_MOVE_COMPLETED_C))
		{
			switch(mainStatus)
			{
			case MS_INIT:
				switch(InitStatus)
				{
				case IS_IDLE:
				case IS_Send_INIT_STAGE1:
					status_ReportStatus(STATUS_FTA + SciError_InternalErr_1, STATUS_SET);//
					break;
#if ((defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)) && \
	!defined(COLORFUL_EPSON))

				case IS_Micolor_Clean_GoHome:
					if(flags & MOTION_MOVE_COMPLETED_C)
						InitAxis &= ~(0x1);
					if(flags & MOTION_MOVE_COMPLETED_Z)
						InitAxis &= ~(0x2);
					if(InitAxis == 0)
						StartFindXHome();
					break;
#elif defined(COLORFUL_EPSON)
				case IS_C_LeaveOriginSensor:
					if(GetSensor(&CParam.Origin) == SENSOR_ZC_NO_ORIGIN)
					{
						InitStatus = IS_C_GoOriginSensor;
						ConfigureSensorIt(&CParam.Origin, NULL, NULL, COrigin_IO_Irq_Interrupt); 
						User_Start_C(0, MOVETYPE_SEGMENT, 1,0x000FFFFF, DIRTYPE_NEG);
						break;
					}
				case IS_C_GoOriginSensor:
					ResetTCCoor(&CParam);
					StartFindXHome();
					break;
#elif defined(EPSON_CLEAN_UPDOWN)
				case IS_Z_LeaveOriginSensor:
					if(GetSensor(&ZParam.Origin) == SENSOR_ZC_NO_ORIGIN)
					{
						InitStatus = IS_Z_GoOriginSensor;
						ConfigureSensorIt(&ZParam.Origin, NULL, NULL, ZOrigin_IO_Irq_Interrupt); 
						User_Start_Z(0, MOVETYPE_SEGMENT, 1,0x000FFFFF, DIRTYPE_NEG);
						break;
					}
				case IS_Z_GoOriginSensor:
#ifndef EPSON_CLEAN_UPDOWN_WIPER_EX										
					if(GetSensor(&CParam.Origin) == SENSOR_ZC_NO_ORIGIN)
					{
						InitStatus = IS_C_GoOriginSensor;
						ConfigureSensorIt(&CParam.Origin, NULL, NULL, COrigin_IO_Irq_Interrupt); 
						User_Start_C(0, MOVETYPE_SEGMENT, 1,0x000FFFFF, DIRTYPE_NEG);
					}
					else
					{
						InitStatus = IS_C_LeaveOriginSensor;
						User_Start_C(0, MOVETYPE_SEGMENT, 1,cleanparam_EPSON_ALLWIN.factory.WiperPos_Y[0], DIRTYPE_POS);
					}
					break;
					//  case IS_Z_GoHome:
				case IS_C_LeaveOriginSensor:
					if(GetSensor(&CParam.Origin) == SENSOR_ZC_NO_ORIGIN)
					{
						InitStatus = IS_C_GoOriginSensor;
						ConfigureSensorIt(&CParam.Origin, NULL, NULL, COrigin_IO_Irq_Interrupt); 
						User_Start_C(0, MOVETYPE_SEGMENT, 1,0x000FFFFF, DIRTYPE_NEG);
						break;
					}
				case IS_C_GoOriginSensor:
#endif											
					ResetTCCoor(&CParam);
					ResetTCCoor(&ZParam);
					StartFindXHome();
					//  case IS_C_GoHome:
					//no implement.
					break;
#endif
				case IS_X_LeaveOriginSensor:
					if(IS_XOrigin_Irq == True)
					{
						InitStatus = IS_X_GoOriginSensor;
						User_Start_X(0, 720, False, MOVETYPE_CONTINUE_CHECKSENSOR, 0, 0, DIRTYPE_NEG);
					}						
					break;
				case IS_X_GoOriginSensor:
					FirstGoXHome();
					break;
				case IS_X_GoHome:
#if defined(INWEAR_FLAT_FIND_Y_HOME)
                  			PWM_Stop(&XParam);
					ResetXCoorSystem();
                    		PWM_Start_Standy_X(&XParam, XParam.CoorIdeal, False);
					StartGoXSafePostion();
					break;
				case IS_X_GoSafePosition:
					StartFindYHome();
					break;
				case IS_Y_LeaveOriginSensor:
					InitStatus = IS_Y_GoOriginSensor;
					ConfigureSensorIt(&YParam.Origin, NULL, NULL, YOrigin_IO_Irq_Interrupt); 
					User_Start_Y(0, MOVETYPE_CONTINUE, 0, 0, DIRTYPE_NEG, False, False);						
					break;
				case IS_Y_GoOriginSensor:
					PWM_Stop(&YParam);
					ResetYCoorSystem();
					PWM_Start_Standy_Y(&YParam, YParam.CoorIdeal, False);
                                                               
					StartFindYLimit(); 
					break;
				case IS_Y_LeaveLimitSensor:
					InitStatus = IS_Y_GoLimitSensor;
					ConfigureSensorIt(&YParam.Limit, NULL, NULL, YLimit_IO_Irq_Interrupt); 
					User_Start_Y(0, MOVETYPE_CONTINUE, 0, 0, DIRTYPE_POS, False, False);
					break;
				case IS_Y_GoLimitSensor:
					ResetYRegion();  //error if not fined Y limit
					FirstGoYHome();
					break;
				case IS_Y_GoHome:
					//ResetXCoorSystem();
					//ResetYCoorSystem();
					//PWM_Start_Standy_X(&XParam, XParam.CoorIdeal, False);
					//PWM_Start_Standy_Y(&YParam, YParam.CoorIdeal, False);

					//StartAllStandby(); 
					//PWM_Start_Standy_Y(&YParam, YParam.CoorIdeal, False);
					InitStatus = IS_Finished;
					//ResetYRegion();  //error if not fined Y limit
					//ConfigureSensorIt(&YParam.Origin, NULL, NULL, YOrigin_IO_Irq_Interrupt); 
					//User_Start_Y(0, MOVETYPE_CONTINUE, 0, 0, DIRTYPE_NEG, False, False);	
					User_Start_X(0, 720, False, MOVETYPE_SEGMENT, 0, 0, DIRTYPE_NEG);
					//User_Start_Y(0, MOVETYPE_SEGMENT, 1, 0, DIRTYPE_NEG, False, False);
					break;
	#if defined (EPSON_CLEAN_INTEGRATE) ||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)|| defined(EPSON_CLEAN_UPDOWN)|| defined(RICOH_CLEAN_PRESS)
 				case IS_Finished:
					EndInitAction();
	#endif
					BEGIN_RECORD;
					break;
#else
					ResetXCoorSystem();
					ResetYCoorSystem();
					StartAllStandby();  
#if defined (EPSON_CLEAN_INTEGRATE) ||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)|| defined(EPSON_CLEAN_UPDOWN)|| defined(RICOH_CLEAN_PRESS)
					EndInitAction();
#endif
					BEGIN_RECORD;
					break;
#endif
#if defined(EPSON_CLEAN_UPDOWN)
				case IS_Allwin_Z_GoCap:
#endif
				case IS_Send_INIT_STAGE2:
					break;
				}
				break;
			case MS_NORMAL_MOVE:
				mainStatus = MS_IDLE;
				if(CleanStatus != Clean_IDLE)
				{
					break;
				}
				cmdbuf[0] = 2 + 4 * sizeof(INT32S);
				cmdbuf[1] = UART_DSP_RPT_STOP;
				*((__packed INT32S *)&cmdbuf[2]) = (INT32S)((XParam.CoorIdeal - XParam.Origin_Offset)/XParam.ratio_move);  
				*((__packed INT32S *)&cmdbuf[6]) = (INT32S)((YParam.CoorIdeal - YParam.Origin_Offset)/YParam.ratio_move); 
				*((__packed INT32S *)&cmdbuf[10]) = (INT32S)((ZParam.CoorIdeal - ZParam.Origin_Offset)/ZParam.ratio_move); 
				*((__packed INT32S *)&cmdbuf[14]) = (INT32S)((CParam.CoorIdeal - CParam.Origin_Offset)/CParam.ratio_move); 
				UART_MotionReportCMD(cmdbuf);
				break;
			case MS_MULTI_AXIS_MOVE_SERIAL:
				ContinueGoHome();
				break;
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1)||defined (EPSON_CLEAN_INTEGRATE_2)||defined (EPSON_CLEAN_INTEGRATE_3)
			case MS_MULTI_AXIS_MOVE_PARALLEL:
				if(flags & MOTION_MOVE_COMPLETED_C)
					MultiAxis &= ~(0x1);
				if(flags & MOTION_MOVE_COMPLETED_Z)
					MultiAxis &= ~(0x2);
				if(MultiAxis == 0)
				{
					mainStatus = MS_IDLE;
					cmdbuf[0] = 2 + 4 * sizeof(INT32S);
					cmdbuf[1] = UART_DSP_RPT_STOP;
					*((__packed INT32S *)&cmdbuf[2]) = (INT32S)((XParam.CoorIdeal - XParam.Origin_Offset)/XParam.ratio_move);  
					*((__packed INT32S *)&cmdbuf[6]) = (INT32S)((YParam.CoorIdeal - YParam.Origin_Offset)/YParam.ratio_move); 
					*((__packed INT32S *)&cmdbuf[10]) = (INT32S)((ZParam.CoorIdeal - ZParam.Origin_Offset)/ZParam.ratio_move); 
					*((__packed INT32S *)&cmdbuf[14]) = (INT32S)((CParam.CoorIdeal - CParam.Origin_Offset)/CParam.ratio_move); 
					UART_MotionReportCMD(cmdbuf);
				}
				break;
#endif
			case MS_MEASURE:
				ContinueMeasure();
				break;
			case MS_PRINT:
				ContinuePrint(flags);
				break;
			}
			if(mainStatus == MS_IDLE)
			{
				switch(CleanStatus)
				{
				case Clean_WIPE_GO_HOME:
					CleanStatus = Clean_X_GO_START;
					AxisMoveTo(8, 0, 0);
					break;
				case Clean_X_GO_START:
					if(MotionWipeInfo.C_WIPE_TIMES-- > 0)
					{	
						CleanStatus = Clean_Z_GO_WIPE;
						XMoveTo(MotionWipeInfo.X_WIPE_SPEED, MotionWipeInfo.X_WipePos_Start);
					}
					else
					{
						CleanStatus = Clean_IDLE;
#ifdef EPSON_CLEAN_UPDOWN                        
						XMoveTo(MotionWipeInfo.X_WIPE_SPEED, cleanparam_EPSON_ALLWIN.factory.Carriage_X_SuckPos);
#else
						XMoveTo(MotionWipeInfo.X_WIPE_SPEED, 0);
#endif
					}
					break;
				case Clean_WIPE_GO_END:
					CleanStatus = Clean_X_GO_END;
					AxisMoveTo(8, 0, MotionWipeInfo.C_WipePos);
					break;
				case Clean_Z_GO_WIPE:
					CleanStatus = Clean_WIPE_GO_END;
					AxisMoveTo(4,0,MotionWipeInfo.Z_WipePos);
					break;
				case Clean_Z_GO_HOME:
					CleanStatus = Clean_WIPE_GO_HOME;
					AxisMoveTo(4, 0, 0);
					break;
				case Clean_X_GO_END:
					CleanStatus = Clean_Z_GO_HOME;
					XMoveTo(MotionWipeInfo.X_WIPE_SPEED, MotionWipeInfo.X_WipePos_End);
					break;
				}
			}
		}
		
		while(UART_MotionGetCMD(cmdbuf))
		{
			switch(cmdbuf[1])
			{
			case UART_SET_FACTORY_DATA_CMD:
#ifdef MOTION_Y_DSP
				while (!UART_SendCMD(UART_DSP_CHANNEL, cmdbuf))
					OSTimeDly(10);
#endif								
				if(!UpdateSpeedInfoBaseonHeadType(cmdbuf[2]))
					reportErr = SciError_Parameter;
				break;
			case UART_BEGIN_STAGE2_CMD:
#ifdef MOTION_Y_DSP
				while (!UART_SendCMD(UART_DSP_CHANNEL, cmdbuf))
					OSTimeDly(10);
#endif								
				if(!UpdateMoveRegionBaseonMediaWidth(cmdbuf[3]))
					reportErr = SciError_ReadEEPROM;
				else if(cmdbuf[2]==0)
					reportErr = SciError_Parameter;
				else
				{
#ifndef MOTION_Y_DSP                
					memset(cmdbuf, 0, sizeof(cmdbuf));
					cmdbuf[0] = 2 + sizeof(INT32U) + 12;
					cmdbuf[1] = UART_RPT_VERSION;
					*((__packed INT32U *)&cmdbuf[2]) = 0x02010001;
					strcpy((char*)&cmdbuf[6], "Embedded.");
					UART_MotionReportCMD(cmdbuf);
#endif
#if !defined(RICOH_CLEAN_PRESS)
					StartInitAction();
#else
					StartFindXHome();
#endif
				}
				break;
			case UART_MOVEDISTANCE_CMD:
				MoveDistance(cmdbuf[2], cmdbuf[3], *(__packed INT32S *)(&cmdbuf[4])); 
				break;
			case UART_MOVETO_CMD:
				XMoveTo(cmdbuf[6], *(__packed INT32S *)(&cmdbuf[2])); 
				break;
			case UART_AXISMOVETO_CMD:
				AxisMoveTo(cmdbuf[2], cmdbuf[3], *(__packed INT32S *)(&cmdbuf[4]));
				break;
#if defined(EPSON_CLEAN_UPDOWN)
			case UART_STARTSWIPE_CMD:
				MotionWipeInfo.X_WIPE_SPEED = cmdbuf[2];
				MotionWipeInfo.C_WIPE_TIMES = cmdbuf[3];
				MotionWipeInfo.C_WipePos = *((__packed INT32S *)&cmdbuf[4]);
				MotionWipeInfo.Z_WipePos = *((__packed INT32S *)&cmdbuf[8]);
				MotionWipeInfo.X_WipePos_Start = *((__packed INT32S *)&cmdbuf[12]);
				MotionWipeInfo.X_WipePos_End = MotionWipeInfo.X_WipePos_Start + *((__packed INT32S *)&cmdbuf[16]);
				CleanStatus = Clean_WIPE_GO_HOME;				
				AxisMoveTo(4,0,0);
				break;
#endif                
			case UART_PAPERFEED_CMD:
				YMoveDistance(cmdbuf[2], cmdbuf[3], *(__packed INT32S *)(&cmdbuf[4])); 
				break;
			case UART_STOPMOVE_CMD:
#ifdef MOTION_Y_DSP
				while (!UART_SendCMD(UART_DSP_CHANNEL, cmdbuf))
					OSTimeDly(10);
#endif							
				StopMove();
				break;
			case UART_GOHOME_CMD:
				StartGoHome(cmdbuf[2]);
				break;
			case UART_MEASUREMEDIA_CMD:
				StartMeasure(cmdbuf[2]);
				//case UART_DSP_RPT_MEDIA:
				break;
			case UART_RPT_DSP_READ_ERR:
			case UART_RPT_DSP_EEPROM_CONTENT:
			case UART_RPT_DSP_WRITE_ERR:
				//ignore.
				break;
			case UART_PRINTPARA_CMD:
#ifdef MOTION_Y_DSP
				while (!UART_SendCMD(UART_DSP_CHANNEL, cmdbuf))
					OSTimeDly(10);
#endif							
				SetPrintParam(cmdbuf[10], *(__packed INT32U *)(&cmdbuf[2]), *(__packed INT32U *)(&cmdbuf[6]));
				break;
			case UART_DSP_SET_YDIR:
#ifdef MOTION_Y_DSP
				while (!UART_SendCMD(UART_DSP_CHANNEL, cmdbuf))
					OSTimeDly(10);
#else							
				SetPrintYDir(cmdbuf[2]);
#endif
				break;
			case UART_PRE_PRINT_CMD:
				StartPrePrint(*(__packed INT32S *)(&cmdbuf[2]), *(__packed INT32S *)(&cmdbuf[6]), 
							  *(__packed INT32S *)(&cmdbuf[10]), *(__packed INT32S *)(&cmdbuf[14]));
				break;
			case UART_PRINT_CMD:
				StartPrint(*(__packed INT32S *)(&cmdbuf[18]));
				break;
			case UART_UPDATE_DSP_BEGIN:
			case UART_UPDATE_DSP_DATA:
			case UART_UPDATE_DSP_END:
#ifdef MOTION_Y_DSP
				while (!UART_SendCMD(UART_DSP_CHANNEL, cmdbuf))
					OSTimeDly(10);
#else
				cmdbuf[0] = 4;
				cmdbuf[1] = UART_UPDATE_DSP_INFO;
				cmdbuf[2] = 0;
				cmdbuf[3] = 0;
				UART_MotionReportCMD(cmdbuf);
#endif								
				break;
			case UART_DSP_GET_POS_CMD:
				cmdbuf[0] = 2 + sizeof(INT32S);
				cmdbuf[1] = UART_DSP_RPT_POSITION;
				*((__packed INT32U *)&cmdbuf[2]) = (INT32U)((XParam.CoorIdeal - XParam.Origin_Offset)/XParam.ratio_move);
				UART_MotionReportCMD(cmdbuf);
				break;
			}
			if(reportErr != SciError_OK)
				status_ReportStatus(STATUS_FTA + reportErr, STATUS_SET);
		}
		
#ifdef MOTION_Y_DSP				
		while(UART_GetCMD(UART_DSP_CHANNEL, UartCMD))
		{
			switch (UartCMD[1])
			{				
			case UART_INIT_STAGE2_CMD:
				
				break;
#ifdef	VENDOR_PASSWORD_TOOL
			case UART_DSP_RPT_DESKEY:
				UART_MotionReportCMD(UartCMD);
				break;
#endif						
			case UART_DSP_RPT_STOP: //Motion stop. Parameters: 4 bytes 当前位置 X: INT32S 类型: 4 字节, Little Endian
				OSFlagPost(mix_FLAG_GRP, MOTION_MOVE_COMPLETED_Y, OS_FLAG_SET, &err); 
				YParam.CoorIdeal = *(__packed INT32S *)(&UartCMD[6]);            
				break;
			case UART_DSP_RPT_POSITION: //Motion report position
				break;
				
			case UART_DSP_RPT_DEBUG_INFO: //Motion report position
				
				for (i = 0; i < UartCMD[0]; i++)
					dspDebugInfo[i] = UartCMD[i];
				
				break;
				
			case UART_DSP_RPT_PRIVATE_DATA: //Motion report position
				
				for (i = 0; i < UartCMD[0]; i++)
					dspPrivateData[i] = UartCMD[i];
				break;					
				
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			case UART_DSP_RPT_PIPE_DATA:
				UART_MotionReportCMD(UartCMD);
				break;  
#endif
			case UART_UPDATE_DSP_INFO:
				UART_MotionReportCMD(UartCMD);
				break;
			case UART_DSP_RPT_STATUS:
#if defined(HEAD_EPSON_GEN5)
				UART_MotionReportCMD(UartCMD);
#endif
				break;							
			case UART_RPT_VERSION:
				UART_MotionReportCMD(UartCMD);
				break;
			case UART_DSP_RPT_PHYSICAL_SIZE:
				//						printerPhysicalSize.x = *(__packed INT32S *)(&UartCMD[2]);
				*((__packed INT32U *)&UartCMD[2]) = (INT32U)((XParam.Region_End - XParam.Origin_Offset)/XParam.ratio_move); 
				*((__packed INT32U *)&UartCMD[10]) = (INT32U)((ZParam.Region_End - ZParam.Origin_Offset)/ZParam.ratio_move); 
				*((__packed INT32U *)&UartCMD[14]) = (INT32U)((CParam.Region_End - CParam.Origin_Offset)/CParam.ratio_move); 
				UART_MotionReportCMD(UartCMD);
				break;
				
			case UART_DSP_RPT_MEDIA:
				
				break;
			case UART_DSP_REQ_READ_EEPROM:
				addr = (UartCMD[2] << 8)|UartCMD[3];
				len  = UartCMD[4];
				if ((len > 23) || (addr + len >= EPR_DSP_DISK_SIZE))
				{
					UartCMD[0] = 3;
					UartCMD[1] = UART_RPT_DSP_READ_ERR;
					UartCMD[2] = 1;
					UART_SendCMD(UART_DSP_CHANNEL, UartCMD);
				}
				else
				{
					OSSemPend(IICSem, 0, &err);
					if (IIC_ReadEEPRom(EPR_DSP_DISK_OFFSET + addr, &UartCMD[5], &len))
					{
						UartCMD[0] = len + 5;
						UartCMD[1] = UART_RPT_DSP_EEPROM_CONTENT;
						UartCMD[4] = len;
						UART_SendCMD(UART_DSP_CHANNEL, UartCMD);
						UartCMD[0] = 3;
						UartCMD[1] = UART_RPT_DSP_READ_ERR;
						UartCMD[2] = 0;
						UART_SendCMD(UART_DSP_CHANNEL, UartCMD); 						
					}else
					{
						UartCMD[0] = 3;
						UartCMD[1] = UART_RPT_DSP_READ_ERR;
						UartCMD[2] = 2; //Read failed.
						UART_SendCMD(UART_DSP_CHANNEL, UartCMD);
					}
					OSSemPost(IICSem);
				}			
				break;
			case UART_DSP_WRITE_EEPROM:
				addr = (UartCMD[2] << 8)|UartCMD[3];
				len  = UartCMD[4];
				if ((len > 23)|| (addr + len >= EPR_DSP_DISK_SIZE))
				{
					UartCMD[0] = 3;
					UartCMD[1] = UART_RPT_DSP_WRITE_ERR;
					UartCMD[2] = 1;
					UART_SendCMD(UART_DSP_CHANNEL, UartCMD);
				}
				else
				{ //
					OSSemPend(IICSem, 0, &err);
					if (IIC_WriteEEPRom(EPR_DSP_DISK_OFFSET+addr, &UartCMD[5], &len))
					{
						UartCMD[0] = 3;
						UartCMD[1] = UART_RPT_DSP_WRITE_ERR;
						UartCMD[2] = 0;
						UART_SendCMD(UART_DSP_CHANNEL, UartCMD);	
					}else
					{
						UartCMD[0] = 3;
						UartCMD[1] = UART_RPT_DSP_WRITE_ERR;
						UartCMD[2] = 2; //Write.
						UART_SendCMD(UART_DSP_CHANNEL, UartCMD);
					}
					OSSemPost(IICSem);
				}			
				break;
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
			case UART_RPT_DSP_PASS_START:
				UART_MotionReportCMD(UartCMD);
				break;
#endif						
#if defined (EPSON_CLEAN_UPDOWN)
			case UART_RPT_DSP_SWIPE_COMPLETED:
				
				break;
#endif
			}				
		}	
#endif
	}
}

INT32S GetCutStartPos()
{
	return XParam.pAxisParam->Fixed_Position[MPID_CUT_START];
}

INT32S GetCutEndPos()
{
	return XParam.pAxisParam->Fixed_Position[MPID_CUT_END];
}

#endif

#if defined(FPGA_MAINTAIN_COOR) || defined(SUPPORT_MOTOR_CONTROL)

#ifdef HEAD_EPSON_GEN5        
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

#endif


#if defined(HEAD_EPSON_GEN5) && ( defined(SUPPORT_MOTOR_CONTROL) || defined(FPGA_MAINTAIN_COOR) || defined(EPSON_BOTTOM_BOARD_V3) ||defined (EPSON_BOTTOM_BOARD_V2_1))
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

#endif

void SetSafeCmd(INT16U data)
{
	do
	{
		rFPGA_COMMAND = data;
		__asm("NOP");
		__asm("NOP");
		if(rFPGA_COMMAND != data)
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

#ifdef RECORD_MOTION_MOVEMENT
struct MOVE_STAT_RECORD
{
	INT8U stat; //bit0-1, dir; bit2-3, segment type; bit4, x/y; bit 5, coor type(motor/feedback); bit6-7, move type;  
	INT8U speedid;
	//    INT16S out_delta_frac;
	//    INT16S out_delta;
	INT16S out;
	INT32S target_coor;
	INT32S real_coor;
	INT32S real_prt_coor;
};

static struct MOVE_STAT_RECORD * curRecord[2] = {NULL,NULL};
static struct MOVE_STAT_RECORD * lastRecord[2] = {NULL,NULL};
static INT8U curSpeed[2] = {0,0};
static INT32U count[2] = {0,0};
INT32U cur_real_coor[2] = {0,0};
struct MOVE_STAT_RECORD * Record_beginAcc[2] = {NULL,NULL};
struct MOVE_STAT_RECORD * Record_EndAcc[2] = {NULL,NULL};
struct MOVE_STAT_RECORD * Record_beginConst[2] = {NULL,NULL};
struct MOVE_STAT_RECORD * Record_EndConst[2] = {NULL,NULL};
struct MOVE_STAT_RECORD * Record_beginDec[2] = {NULL,NULL};
struct MOVE_STAT_RECORD * Record_EndDec[2] = {NULL,NULL};
INT32S target_coor_cur = 0, real_coor_cur = 0;

void StartMotionRecord(INT8U recordType, INT8U IsXorY)
{
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();
	bRecordType = recordType;
	bAllowRecord = IsXorY;
	bAllowRecord_Last = 0;
	curRecord[0] = (struct MOVE_STAT_RECORD *)(HUGEBUF_END_ADDR);
	curRecord[1] = (struct MOVE_STAT_RECORD *)(HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4));
	Record_beginAcc[1] = Record_beginAcc[0] = 0;
	Record_EndAcc[1] = Record_EndAcc[0] = 0;
	Record_beginConst[1] = Record_beginConst[0] = 0;
	Record_EndConst[1] = Record_EndConst[0] = 0;
	Record_beginDec[1] = Record_beginDec[0] = 0;
	Record_EndDec[1] = Record_EndDec[0] = 0;
	OS_EXIT_CRITICAL();
}

void EndMotionRecord()
{
	OS_CPU_SR cpu_sr;
	
	OS_ENTER_CRITICAL();
	bAllowRecord = 0;
	OS_EXIT_CRITICAL();
}


static INT8U CacheBuf[0x200];
void StartUploadRecord(INT32U recordCount, INT8U IsXorY)
{
	INT32U record_start, record_end, totallen, len, curlen;
	OS_CPU_SR cpu_sr;
	INT8U *pBuf, *pTarget;
	
	OS_ENTER_CRITICAL();
	if( bRecordType == 0 || (Record_beginAcc[0] == NULL && (IsXorY & 1)) || 
	   (Record_beginAcc[1] == NULL && (IsXorY & 2)))
	{
		if((IsXorY & 1))
		{
			record_start = (INT32U)curRecord[0] - recordCount*sizeof(struct MOVE_STAT_RECORD);
			if( record_start < HUGEBUF_END_ADDR)
				record_start += (BOARD_DDRAM_SIZE/4);
			record_end = (INT32U)curRecord[0] - sizeof(struct MOVE_STAT_RECORD);
			if( record_end < HUGEBUF_END_ADDR)
				record_end += (BOARD_DDRAM_SIZE/4);
		}
		else if(IsXorY & 2)
		{
			record_start = (INT32U)curRecord[1] - recordCount*sizeof(struct MOVE_STAT_RECORD);
			if( record_start < (HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4)))
				record_start += (BOARD_DDRAM_SIZE/4);
			record_end = (INT32U)curRecord[1] - sizeof(struct MOVE_STAT_RECORD);
			if( record_end < (HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4)))
				record_end += (BOARD_DDRAM_SIZE/4);
		}
	}
	else
	{
		if( (IsXorY & 1))
		{
			record_start = (INT32U)Record_beginAcc[0] - 0x10000;
			if( record_start < HUGEBUF_END_ADDR)
				record_start += (BOARD_DDRAM_SIZE/4);
			record_end = (INT32U)Record_EndDec[0] + 0x10000;
			if( record_end >= HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4))
				record_end -= (BOARD_DDRAM_SIZE/4);
		}
		else if(IsXorY & 2)
		{
			record_start = (INT32U)Record_beginAcc[1] - 0x10000;
			if( record_start < (HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4)))
				record_start += (BOARD_DDRAM_SIZE/4);
			record_end = (INT32U)Record_EndDec[1] + 0x10000;
			if( record_end >= HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/2))
				record_end -= (BOARD_DDRAM_SIZE/4);
		}
	}
	OS_EXIT_CRITICAL();
	
	if(record_end > record_start)
		len = record_end - record_start;
	else
		len = (record_end - record_start) + (BOARD_DDRAM_SIZE/4);
	
	pBuf = (INT8U* )record_start;
	totallen = len;
	while(len > 0)
	{
		if(len > 512-32)
			curlen = 512-32;
		else
			curlen = len;
		
		pTarget = CacheBuf;
		*(INT16U*)pTarget = EP6_CMD_T_MOTION_RECORD;
		pTarget += sizeof(INT16U);
		*(__packed INT32U*)pTarget = totallen;
		pTarget += sizeof(INT32U);
		*(__packed INT32U*)pTarget = totallen - len;
		pTarget += sizeof(INT32U);
		*(__packed INT32U*)pTarget = curlen;
		pTarget += sizeof(INT32U);
		
		if(IsXorY & 1)
		{
			if( (INT32U)pBuf + curlen >= (INT32U)HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4))
			{
				memcpy(pTarget, (void*)pBuf,  HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4) - (INT32U)pBuf);
				memcpy(pTarget + (HUGEBUF_END_ADDR + (BOARD_DDRAM_SIZE/4) - (INT32U)pBuf), 
					   (void*)HUGEBUF_END_ADDR, curlen - (HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4) - (INT32U)pBuf));
				pBuf += curlen;
				pBuf -= (BOARD_DDRAM_SIZE/4);
			}
			else
			{
				memcpy(pTarget, (void*)pBuf,  curlen);
				pBuf += curlen;
			}
		}
		else
		{
			if( (INT32U)pBuf + curlen >= (INT32U)HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/2))
			{
				memcpy(pTarget, (void*)pBuf,  HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/2) - (INT32U)pBuf);
				memcpy(pTarget + (HUGEBUF_END_ADDR + (BOARD_DDRAM_SIZE/2) - (INT32U)pBuf), 
					   (void*)(HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4)), 
					   curlen - (HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/2) - (INT32U)pBuf));
				pBuf += curlen;
				pBuf -= (BOARD_DDRAM_SIZE/4);
			}
			else
			{
				memcpy(pTarget, (void*)pBuf,  curlen);
				pBuf += curlen;
			}
		}
		PushCommPipeData(COMMPIPE_ARM_CHANNEL_ID, CacheBuf, curlen + (pTarget - CacheBuf), True);
		len -= curlen;
	}
}

void PutMoveRecord(INT8U dir, INT8U SegType, INT8U MoveType, INT8U IsX, INT8U IsFeedBack, 
				   INT16S out, INT32S target_coor, INT32S real_coor)
{
	OS_CPU_SR cpu_sr;
	
	//    {
	//        INT16U reg;
	//        rFPGA_EPSON_RegAddr = EPSON_REGADDR_COOR_STAT;
	//        reg = rFPGA_EPSON_RegPort;
	//        if(ER_CoorStat_OverFreq & reg)
	//        {
	//            status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG2, STATUS_SET);
	//        }
	//    }
	
	if(!bAllowRecord)
		return;
	if(IsX && (bAllowRecord & 1 ))
	{
		INT8U bRecord = True;
		if(bRecordType == 1)
		{
			if(Record_beginAcc[0]!= NULL && Record_EndDec[0] != NULL)
			{
				INT32U p = (INT32U)Record_EndDec[0];
				p += 0x10000;
				if(p >= (HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4)))
					p -= (BOARD_DDRAM_SIZE/4);
				if((INT32U)curRecord[0] == p)
					bRecord = False;
			}
		}
		if(bRecord)
		{
			//OS_ENTER_CRITICAL();
#ifdef   HEAD_EPSON_GEN5            
			curRecord[0]->real_prt_coor = ReadEpsonRegInt(EPSON_REGADDR_X_PRT_COOR);
#elif defined(HEAD_RICOH_G4)
			curRecord[0]->real_prt_coor = (INT32S)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
#endif
			//OS_EXIT_CRITICAL();
			if(bAllowRecord == bAllowRecord_Last)
			{
				//                if( lastRecord[0]->target_coor > target_coor + 1000 || lastRecord[0]->target_coor < target_coor - 1000 ||
				//                    lastRecord[0]->real_coor > real_coor + 1000 || lastRecord[0]->real_coor < real_coor - 1000 )
				//                {
				//                    target_coor_cur = target_coor;
				//                    real_coor_cur = real_coor;
				//                    OS_ENTER_CRITICAL();
				//                    cur_real_coor[0] = ReadEpsonRegInt(XParam.CoorRegAddr);
				//                    OS_EXIT_CRITICAL();
				//                    PWM_Stop(&XParam);
				//                    status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG2, STATUS_SET);
				//                }
				if( SegType != ((lastRecord[0]->stat >>2) & 0x3))
				{
					if( SegType == ACCSEGTYPE_CONSTANT_SPEED && Record_beginConst[0] == NULL)
						Record_beginConst[0] = curRecord[0];
					else if(SegType == ACCSEGTYPE_SPEED_UP && Record_beginAcc[0] == NULL)
						Record_beginAcc[0] = curRecord[0];
					else if(SegType == ACCSEGTYPE_SPEED_DOWN && Record_beginDec[0] == NULL)
						Record_beginDec[0] = curRecord[0];
					if(((lastRecord[0]->stat >>2) & 0x3) == ACCSEGTYPE_CONSTANT_SPEED && Record_EndConst[0] == NULL)
						Record_EndConst[0] = lastRecord[0];
					else if(((lastRecord[0]->stat >>2) & 0x3) == ACCSEGTYPE_SPEED_UP && Record_EndAcc[0] == NULL)
						Record_EndAcc[0] = lastRecord[0];
					else if(((lastRecord[0]->stat >>2) & 0x3) == ACCSEGTYPE_SPEED_DOWN && Record_EndDec[0] == NULL)
						Record_EndDec[0] = lastRecord[0];
				}
			}
			
			curRecord[0]->stat = dir | (SegType<<2) | (MoveType << 6) | (IsX<<4) | (IsFeedBack<<5);
			//          curRecord[0]->speedid = curSpeed[0];
			//          curRecord[0]->out_delta_frac = out_delta_frac;
			//          curRecord[0]->out_delta = out_delta;
			curRecord[0]->out = out;
			curRecord[0]->target_coor = target_coor;
			curRecord[0]->real_coor = real_coor;
			
			lastRecord[0] = curRecord[0];
			curRecord[0] ++;
			if((INT32U)curRecord[0] >= HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4))
				curRecord[0] = (struct MOVE_STAT_RECORD *)(HUGEBUF_END_ADDR);
			//count[0] ++;
			
			bAllowRecord_Last &= 0xFE;
			bAllowRecord_Last |= bAllowRecord & 1;
		}
	}
	if(!IsX && (bAllowRecord & 2 ))
	{
		INT8U bRecord = True;
		if(bRecordType == 1)
		{
			if(Record_beginAcc[1]!= NULL && Record_EndDec[1] != NULL)
			{
				INT32U p = (INT32U)Record_EndDec[1];
				p += 0x10000;
				if(p >= (HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/2)))
					p -= (BOARD_DDRAM_SIZE/4);
				if((INT32U)curRecord[0] == p)
					bRecord = False;
			}
		}
		if(bRecord)
		{
			//OS_ENTER_CRITICAL();
#ifdef   HEAD_EPSON_GEN5               
			curRecord[1]->real_prt_coor = ReadEpsonRegInt(XParam.CoorRegAddr);
#elif defined(HEAD_RICOH_G4)
			if(XParam.CoorRegAddr == EPSON_REGADDR_X_MOTOR_COOR)
			{
				curRecord[1]->real_prt_coor = (INT32S)(rFPGA_RICOH_XMOTORCOOR_L |(rFPGA_RICOH_XMOTORCOOR_H&0xFF) << 16);
			}
			else if(XParam.CoorRegAddr == EPSON_REGADDR_X_PRT_COOR)
			{
				curRecord[1]->real_prt_coor = (INT32S)(rFPGA_RICOH_XPRTCOOR_L |(rFPGA_RICOH_XPRTCOOR_H&0xFF) << 16);
			}
#endif						
			//OS_EXIT_CRITICAL();
			if(bAllowRecord == bAllowRecord_Last)
			{
				//                if( lastRecord[1]->target_coor > target_coor + 1000 || lastRecord[1]->target_coor < target_coor - 1000 ||
				//                    lastRecord[1]->real_coor > real_coor + 1000 || lastRecord[1]->real_coor < real_coor - 1000 )
				//                {
				//                    target_coor_cur = target_coor;
				//                    real_coor_cur = real_coor;
				//                    OS_ENTER_CRITICAL();
				//                    cur_real_coor[1] = ReadEpsonRegInt(YParam.CoorRegAddr);
				//                    OS_EXIT_CRITICAL();
				//                    PWM_Stop(&YParam);
				//                    status_ReportStatus(STATUS_ERR_INTERNAL_DEBUG1, STATUS_SET);
				//                }
				if( SegType != ((lastRecord[1]->stat >>2) & 0x3))
				{
					if(SegType == ACCSEGTYPE_CONSTANT_SPEED)
						Record_beginConst[1] = curRecord[1];
					else if(SegType == ACCSEGTYPE_SPEED_UP)
						Record_beginAcc[1] = curRecord[1];
					else if(SegType == ACCSEGTYPE_SPEED_DOWN)
						Record_beginDec[1] = curRecord[1];
					if(((lastRecord[1]->stat >>2) & 0x3) == ACCSEGTYPE_CONSTANT_SPEED)
						Record_EndConst[1] = lastRecord[1];
					else if(((lastRecord[1]->stat >>2) & 0x3) == ACCSEGTYPE_SPEED_UP)
						Record_EndAcc[1] = lastRecord[1];
					else if(((lastRecord[1]->stat >>2) & 0x3) == ACCSEGTYPE_SPEED_DOWN)
						Record_EndDec[1] = lastRecord[1];
				}
			}
			curRecord[1]->stat = dir | (SegType<<2) | (MoveType << 6) | (IsX<<4) | (IsFeedBack<<5);
			//curRecord[1]->speedid = curSpeed[1];
			//        curRecord[1]->out_delta_frac = out_delta_frac;
			//        curRecord[1]->out_delta = out_delta;
			curRecord[1]->out = out;
			curRecord[1]->target_coor = target_coor;
			curRecord[1]->real_coor = real_coor;
			
			lastRecord[1] = curRecord[1];
			curRecord[1] ++;
			if((INT32U)curRecord[1] >= HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/2))
				curRecord[1] = (struct MOVE_STAT_RECORD *)(HUGEBUF_END_ADDR+(BOARD_DDRAM_SIZE/4));
			//count[1] ++;
			bAllowRecord_Last &= 0xFD;
			bAllowRecord_Last |= bAllowRecord & 2;
		}
	}
}

#endif

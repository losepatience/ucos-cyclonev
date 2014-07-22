#ifndef _MOTION_H_

#define _MOTION_H_

#ifdef HEAD_EPSON_GEN5
#define XORIGIN_OFFSET_MOTOR  0x200000
#define XORIGIN_OFFSET  0x40000
#elif defined(HEAD_RICOH_G4)
#define XORIGIN_OFFSET_MOTOR  0x800000
#define XORIGIN_OFFSET  0x800000
#endif
#define MIN_PERMIT_X_PRINT_ERROR (50*720/254)   //5mm

extern INT8U Already_Reset_FPGA;
#if defined(SUPPORT_MOTOR_CONTROL)|| defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)

INT8U Motion_Init();
void Motion_Task();

#define MAX_AXIS 4
#define MAX_POSI_NUM 8
#define MAX_SPEED_LEVEL ((4+1)*5+3) //(4 VSD model + return_speed_for_uni_dir) * 5 resolution + 3 move speed
#define MAX_SENSOR_NUM 16
#define MAX_CONTROL_NUM 8
//#define MAX_PID_NUM 6

#ifdef HEAD_EPSON_GEN5
#define MAX_X_SPEED_LEVEL   MAX_SPEED_LEVEL
#define MAX_Y_SPEED_LEVEL   5
#if defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
#define MAX_Z_C_SPEED_LEVEL 7
#else
#define MAX_Z_C_SPEED_LEVEL 4
#endif
#elif defined(HEAD_RICOH_G4)
#define MAX_X_SPEED_LEVEL   MAX_SPEED_LEVEL
#define MAX_Y_SPEED_LEVEL   5
#if defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
#define MAX_Z_C_SPEED_LEVEL 7
#else
#define MAX_Z_C_SPEED_LEVEL 4
#endif
#define RICOH_G4_FLASH_HEAD_SPACE ((INT32S)(factoryData.m_fHeadXColorSpace * (INT32S)factoryDataEx.m_nXEncoderDPI))
#endif

enum AccType
{
	AT_Trapezoid, AT_SCurve
};
enum DriveType
{
	DT_PWM_Speed, DT_PWM_Speed_Position, DT_PWM_Current_Speed_Position, DT_PulseAndDir
};
enum ControlType //开环控制，闭环控制
{   //for DriveType is DT_PWM, CT_CLOSE_LOOP/CT_CLOSE_LOOP_MOTOR/CT_DOUBLE_CLOSE_LOOP is valid.
	//for DriveType is DT_PulseAndDir, CT_OPEN_LOOP/CT_CLOSE_LOOP is valid.
	CT_OPEN_LOOP, CT_CLOSE_LOOP, CT_CLOSE_LOOP_MOTOR, CT_DOUBLE_CLOSE_LOOP
};
enum MotorType
{
	MT_Stepper, MT_DC, MT_AC
};
enum SpecialOption
{
	SO_ONLY_INCREMENT_MOVEMENT = 1, //only for Micolor EPSON cleaning system.
	SO_SINGLE_DIR = 2,
	SO_FIXED_REGION = 4 //,
		//SO_PERMITTED_NEG_POSITION = 8,
		//SO_FEEDBACK_FROM_LASTEST_END = 16
};


struct AccParam
{
	enum AccType type;
	
	//for Trapezoid and Scurve.
	INT32U startFreq; //unit: Hz.
	INT32U EndFreq; //unit: HZ.
	INT32U AccTime; //Unit: us.
	
	//only for Scurve.
	INT32U AccCurveTime; //Unit: us.
	//INT32U DecCurveTime; //Unit: us.
	INT32U AccCurveEndFreq; //unit: HZ.
	//INT32U DecCurveStartFreq; //unit: HZ.
	INT16U CurveType; //Curve type. default value is 0, Bezier curve.
	float CtrlPoint[2]; //the ratio of control point(p1,p2) of Bezier curve.
};

#define MOTION_AXIS_PARAM_VERSION 0x1
#if defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
#define AXIS_ID_Z   1
#define AXIS_ID_C   2
#define AXIS_ID_Z_DRIVER   3
#define AXIS_ID_C_DRIVER   4
#else
#define AXIS_ID_X   1
#define AXIS_ID_Y   2
#define AXIS_ID_Z   3
#define AXIS_ID_Cleaner   4
#endif
//motion position id
#define MPID_START      0x0     
#define MPID_ORGSENSOR  0x1
#define MPID_FLATSTART  0x2
#define MPID_FLATEND    0x3
#define MPID_MOVEEND    0x4
#define MPID_MEASURE_END 0x5
#define MPID_CUT_START  0x6
#define MPID_CUT_END    0x7
//motion sensor id
#define MSID_ORIGIN     0x0     
#define MSID_LIMIT      0x2
#define MSID_FAULT      0x5
#define MSID_OVERTEMP   0x6
#define MSID_MEASURE    0x7
//motion control io id
#define MCID_PWM1       0x0     
#define MCID_PWM2       0x1     
#define MCID_Ctrl0      0x2     
#define MCID_Ctrl1      0x3     
#define MCID_Ctrl2      0x4     
#define MCID_Ctrl3      0x5     

#define PWMTYPE_DEFAULT                         0
#define PWMTYPE_UNI_POLAR                       1
#define PWMTYPE_BI_POLAR                        2
#define PWMTYPE_STANDBY_LOWLOOP                 3   //slow decay model, current:(ground->left low diode->motor->right low FET)   
//#define PWMTYPE_STANDBY_HIGHLOOP                4   //slow decay model, current:(powerSupply->left high FET->motor->right high diode)   
#ifdef A1802 //反转Z C轴原点信号
#define SENSOR_ZC_ORIGIN                         1
#define SENSOR_ZC_NO_ORIGIN                      0
#else
#define SENSOR_ZC_ORIGIN                         0
#define SENSOR_ZC_NO_ORIGIN                      1
#endif
//copy from DSP code.
//only SciError_SensorPause is normal Error. the other is fatal error.
struct EPR_MOTION_AXIS_PARAM
{
	INT16U version;
	INT16U len;
	
	//AxisID: 1,X; 2,Y; 3, Z; 4/5/6..., other axis;
	INT8U AxisID;
	enum MotorType motorType;
	enum ControlType controlType;
	enum DriveType driveType;
	//enum SpecialOption option;
	INT32U option;
	
	//for DriveType is DT_PulseAndDir: the pulse number from caller : the pulse number to motion.
	//  for X, 720DPI : the pulse number to motion.
	//  for Y close_loop, pulse_number_from_feedback_encoder : the pulse number to motion
	//  for Y open_loop, always be 1.
	//  for Stepper, usually be 1. the pulse number to motion (based on default divider number) : the pulse number to motion
	//for DriveType is DT_PWM: the pulse number from caller : the pulse number for motion's encoder.
	//  for X, 720DPI : pulse_number_from_motor_encoder.
	//  for Y close_loop, pulse_number_from_feedback_encoder : the pulse number to motion
	//  for Y open_loop, always be 1.
	float transmisstion_ratio_open_loop; 
	//the pulse number from caller : the pulse number from feedback encoder.
	//for x, 720 : 720/540/1440. 
	//for y. only index 0 is valid, and always be 1.
	float transmisstion_ratio_close_loop[3]; 
	float PID_CSP[3][3]; //first dim is for type, second dim is for PID parameter.
	// PID parameter index 0 is P, index 1 is I, index 2 is D.
	//usually, type index 0 is current, type index 1 is speed, type index 2 is position.
	float PID_Speed[MAX_SPEED_LEVEL+1][3]; //first dim is for type, second dim is for PID parameter.
	// PID parameter index 0 is P, index 1 is I, index 2 is D.
	//usually, type index 0 is standby, type index 1 is for speed Level 1, type index 2 is for speed Level 2......
	
	//the following unit is the pulse number from caller.
	INT32S OriginOffset; //the offset of the coordinate system origin to the origin sensor.
	INT32S Fixed_MoveRegion_StartPoint;
	INT32S Fixed_MoveRegion_EndPoint;
	INT32S Fixed_Position[MAX_POSI_NUM]; //index 0 is startPoint, index 1 is origin sensor offset, 
	//index 2 is flatform(print region)'s start point, index 3 is flatform(print region)'s end point.
	//index 4 is move region's end point.
	//the left is the customized position.
	//  for micolor, the index 5 is the measure sensor offset, the index 6 is end point of cut sheet movement.
	//      the index 7 is start point of cut sheet movement.
	//if a value is 0x80000000/0xFFFFFFFF, it means invalid.
	
	INT8U PWM_ChannelID; //can not change.
	INT8U PWM_Model[3]; //index 0 is for standby, index 1 is for movement, index 2 is for print.
	//Bit0: origin, Bit1: start sensor, Bit2: end sensor, Bit3: start limit sensor, Bit4: end limit sensor, 
	//  Bit5: Fault Warning, usually Overcurrent or UnderVoltage, Bit6: OverTemp warning.
	//  Bit7: Media Measure sensor.
	//current valid is Bit0/2/5/6/7.
	INT16U SensorExistMask; 
	INT16U SensorPolarityMask; 
	INT8U SensorIOId[MAX_SENSOR_NUM]; // PIO_PORT * 32 + PIO_PIN. if PIO_PORT == 5, it means FPGA expand sensor IO
	//internal use. based on driver chip. usually, bit 0~7 is PWM0, PWM1, ControlIO_0(INH/brake/reset/IN_C_11), ControlIO_1(IN_C_12),
	//  ControlIO_2(IN_C_21), ControlIO_3(IN_C_22)......
	INT8U ControlIOPolarityMask; 
	//The first two item is invalid because it must be PWM IO.
	INT8U ControlIOId[MAX_CONTROL_NUM]; // PIO_PORT * 32 + PIO_PIN. if PIO_PORT == 5, it means FPGA expand Control IO
	
	INT8U validSpeedLevel;
	struct AccParam accParam[MAX_SPEED_LEVEL];
};
#if defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
#define MOTION_PARAM_VERSION 0x2
#define MOTION_PARAM_FLAG    0x5982
#else
#define MOTION_PARAM_VERSION 0x1
#define MOTION_PARAM_FLAG    0x4982
#endif
struct EPR_MOTION_PARAM
{
	INT16U version;
	INT16U len;
	
	INT8U AxisCount;
	
	struct EPR_MOTION_AXIS_PARAM axis_param[MAX_AXIS];
};

INT8U ConfigMoveParam(INT8U Axis, INT8U SpeedLevel);
INT8U StartMovement(INT8U Axis, INT8U SpeedLevel, INT8U dir, INT32U distance);

INT8U IsDelaySendData();
void DelaySendData(INT32U PreFireNum, INT32U PostFireNum, INT8U b_first_pass);

INT32S GetCutStartPos();
INT32S GetCutEndPos();

#define MIN_FLASH_PRE_DISTANCE  (720/2)   //0.5 inch.

INT8U IsHomeWhenPowerOn();
INT8U SaveMotionParam(struct EPR_MOTION_PARAM * pMotionParam);

#endif //SUPPORT_MOTOR_CONTROL

#if defined(FPGA_MAINTAIN_COOR) || defined(SUPPORT_MOTOR_CONTROL)|| defined(SUPPORT_MOTOR_CONTROL_ONLY_STEP)
typedef void (*FPGAExpandIo_IntHandler)(void * data, INT8U IsHigh);
typedef void (*FPGAPosi_IntHandler)(void * data);
#ifdef HEAD_EPSON_GEN5        
//typedef void (*FPGAExpandIo_IntHandler)(void * data, INT8U IsHigh);
INT8U InitFPGAExpandIoIT();
INT8U ConfigFPGAExpandIoIT(FPGAExpandIo_IntHandler handler, INT8U bit, void * data);
INT8U UnConfigFPGAExpandIoIT(INT8U bit);
INT8U DisableFPGAExpandIoIT(INT8U bit);
INT8U EnableFPGAExpandIoIT(INT8U bit);
INT8U GetFPGAExpandIoStat(INT8U bit);
#endif
INT8U InitFPGAPosiIT();
INT8U SetTargetPosi(INT8U PosiID, INT32U Position, INT8U Option);
INT8U ConfigFPGAPosiIT(INT8U PosiID, FPGAPosi_IntHandler Handler, void * Data);
INT8U DisableFPGAPosiIT(INT8U PosiID);
INT8U EnableFPGAPosiIT(INT8U PosiID);

//INT8U GetCurConfig(INT8U PosiID, INT32U *Position, INT8U *Option);
//if want to insert a position interrupt before current one.
INT8U PushCurConfig();
//if the inserted interrupt fucntion want to continue old position.
INT8U PopCurConfig();

INT8U GetPendingConfig(INT8U PosiID, INT32U *Position, INT8U *Option);
//if want to patch a position interrupt after current one.
INT8U RegPendingConfig(INT8U PosiID, INT32U Position, INT8U Option,
					   FPGAPosi_IntHandler Handler, void * Data);
INT8U PopPendingConfig();
//if current one want to continue itself position interrupt.
INT8U DelayPendingConfig();
//if current one want to continue the pending one.
INT8U TrigPendingConfig();

#endif

#if defined(RECORD_MOTION_MOVEMENT) && defined(SUPPORT_MOTOR_CONTROL) 
void StartMotionRecord(INT8U recordType, INT8U IsXorY);
void EndMotionRecord();
void StartUploadRecord(INT32U recordCount, INT8U IsXorY);
#endif

#if defined(LECAI_EPSON_4H_Y_ROOLENCODER)
INT8U ResetZTCCoor(void);
INT8U User_Stop_Z(INT8U bUrgent);
#endif

void SetFPGAReg(INT32U PreFireNum, INT32U PostFireNum);
void PushVSDCMD(INT8U b_first_pass);


#endif

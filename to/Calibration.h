#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_
#include "global.h"
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)

#if defined(RIPSTAR_FLAT_EX)&&defined(MANUFACTURER_DYSS)
//#define DYSS_MAX_X_PRINT_WIDTH  ((float)(30.0f/2.54f))
#define DYSS_MAX_X_PLAT_WIDTH ((float)(24.0f/2.54f) + PAPER_MEDIA_WIDTH_INCH)
#endif

#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
#define MAX_SPEED_LEVEL_EACH_VSD 3
#else
#define MAX_SPEED_LEVEL_EACH_VSD 1
#endif

#ifdef CLOSE_NOZZLE
#define CLOSE_NOZZLE_NUM 4
#else
#define CLOSE_NOZZLE_NUM 0
#endif

#if defined( MANUFACTURER_TATE_EPSON_UV)||defined( RICOH_VSD2)||defined(ONE_BIT_DATA)
#define CALI_VSD_DEFAULT 1
#else
//#define CALI_VSD_DEFAULT 3
#if defined(HEAD_RICOH_G4) && defined(UV_PRINTER)
#define CALI_VSD_DEFAULT 1
#else
#define CALI_VSD_DEFAULT 3
#endif
#endif
#ifdef FULGENCY_FUN
#define DEFAULT_DPI2_TO 1
#else
#define DEFAULT_DPI2_TO 2
#endif
#ifdef DYSS_1270
#define CHANGE_DPI_TO ((Info.VSDModel == 1)? 2:1)
#endif
//INT8U CALI_VSD_DEFAULT = 0;
//#define BI_TEST
//#define NEW_DOT_CHECK
//#define TEST_DOUBLE_CHANNEL

#ifdef HEAD_EPSON_GEN5
#define NOZZLE_LINE_COUNT_PER_HEAD	8
#define NOZZLE_COUNT_PER_LINE       180
#define NOZZLE_DPI                  180
#ifdef EPSON_4H
#define MAX_HEAD_NUMBER_CALIBRATION	4
#else
#define MAX_HEAD_NUMBER_CALIBRATION	2
#endif
#define MAX_GROUP_NUM               MAX_HEAD_NUMBER_CALIBRATION

#define X_NORMAL_RES                  (720)

#define X_BASE_RES                  (factoryDataEx.m_nXEncoderDPI)
#define INTERLEAVE_CALIBRATION	    2
#define NOZZLE_BIT_DEEP             1
#define NOZZLE_BIT_WIDTH            2
#elif defined(RICOH_G5_3H)
#define NOZZLE_LINE_COUNT_PER_HEAD	4
#define NOZZLE_COUNT_PER_LINE       320
#define NOZZLE_DPI                  150
#define MAX_HEAD_NUMBER_CALIBRATION 4
#define MAX_GROUP_NUM               4
#define X_BASE_RES                  (factoryDataEx.m_nXEncoderDPI)
#define X_NORMAL_RES                  (600)
#define INTERLEAVE_CALIBRATION	    4
#define NOZZLE_BIT_DEEP             1  //ricoh use the same data format with epson
#define NOZZLE_BIT_WIDTH            2
#elif defined(RICOH_G5_4H) /*XXX*/
#define NOZZLE_LINE_COUNT_PER_HEAD	4
#define NOZZLE_COUNT_PER_LINE       320
#define NOZZLE_DPI                  150
#define MAX_HEAD_NUMBER_CALIBRATION 16/*XXX*/
#define MAX_GROUP_NUM               4
#define X_BASE_RES                  (factoryDataEx.m_nXEncoderDPI)
#define X_NORMAL_RES                  (600)
#define INTERLEAVE_CALIBRATION	    4
#define NOZZLE_BIT_DEEP             1  //ricoh use the same data format with epson
#define NOZZLE_BIT_WIDTH            2
#else //for HEAD_RICOH_G4
#define NOZZLE_LINE_COUNT_PER_HEAD	2
#define NOZZLE_COUNT_PER_LINE       192
#define NOZZLE_DPI                  150
#ifdef RICOH_G4_16H
#define MAX_HEAD_NUMBER_CALIBRATION 16
#else
#define MAX_HEAD_NUMBER_CALIBRATION 8
#endif
#define MAX_GROUP_NUM               4

#define X_NORMAL_RES                  (600)


#define X_BASE_RES                  (factoryDataEx.m_nXEncoderDPI)
#define INTERLEAVE_CALIBRATION	    2
#define NOZZLE_BIT_DEEP             1  //ricoh use the same data format with epson
#define NOZZLE_BIT_WIDTH            2
#endif

#define X_WIDTH_MULTIPLE 	(X_BASE_RES/X_NORMAL_RES)

#define NOZZLE_DPI_G4L              75

#define MAX_COLOR_NUMBER_CALIBRATION	8
#define COLOR_NUMBER_CALIBRATION	(factoryData.color_num + factoryData.spot_color1_headnum + factoryData.spot_color2_headnum)
#define MIN_COLOR_NUMBER_CALIBRATION	4
#define MAX_PASS_NUMBER_CALIBRATION	24
#ifdef MANUFACTURER_DYSS
#define MAX_MEDIA_TYPE_CALIBRATION	 7
#else
#define MAX_MEDIA_TYPE_CALIBRATION	7
#endif
#define MAX_INTERLEAVE_CALIBRATION	4
#ifdef RICOH_G4_16H
#define MAX_HEIGHT_LEVEL    1   //Max Height Level: high/low.
#else
#define MAX_HEIGHT_LEVEL    2   //Max Height Level: high/low.
#endif
#ifdef HEAD_EPSON_GEN5
#ifdef EPSON_4H
#define MAX_X_RES_NUM	2	//720,360  //to save memory.
#else
#define MAX_X_RES_NUM	5	//720,360,540,270,1440
#endif
#else
#define MAX_X_RES_NUM	3	//600,300,200 or 720,360,240
#endif
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
#define MAX_VSDMODEL_NUM	(2*MAX_SPEED_LEVEL_EACH_VSD)	//VSD1,VSD2
#else
#define MAX_VSDMODEL_NUM	4	//VSD1,VSD2,VSD3,VSD4
#endif

#define BUFF_BLOCK_SIZE	(1024) 
#ifdef HEAD_EPSON_GEN5
#define FIRE_BLOCK_SIZE	(((180*2*2*4/2) + BUFF_BLOCK_SIZE -1)/BUFF_BLOCK_SIZE*BUFF_BLOCK_SIZE) //current version only allocates the buffer length of 4 heads.
#elif defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
#define FIRE_BLOCK_SIZE	(((320*2*2*8/4) + BUFF_BLOCK_SIZE -1)/BUFF_BLOCK_SIZE*BUFF_BLOCK_SIZE) //current version only allocates the buffer length of 8 heads.
#else
#define FIRE_BLOCK_SIZE	(((192*2*2*16/8) + BUFF_BLOCK_SIZE -1)/BUFF_BLOCK_SIZE*BUFF_BLOCK_SIZE) //current version only allocates the buffer length of 16 heads.
#endif

#define REAL float

#define MEDIA_ID_PP     0
#define MEDIA_ID_GLOSS  1

#define COLORCODE_K	'K'
#define COLORCODE_C	'C'
#define COLORCODE_M	'M'
#define COLORCODE_Y	'Y'

#define COLORCODE_Lc 'c'
#define COLORCODE_Lm 'm'
#define COLORCODE_Or 'O'
#define COLORCODE_Gr 'G'
#define COLORCODE_R  'R'
#define COLORCODE_B  'B'
#define COLORCODE_W  'W'
#define COLORCODE_V  'V'
#define COLORCODE_SPOTCOLOR0  '0'
#define COLORCODE_SPOTCOLOR1  '1'
#define COLORCODE_SPOTCOLOR2  '2'
#define COLORCODE_SPOTCOLOR3  '3'

#define COLORCODE_UNDEF 'U'

#define TITLE_Y_SPACE   (NOZZLE_COUNT_PER_LINE/4)
#define TITLE_CHAR_WIDTH        (32*X_WIDTH_MULTIPLE)
#define TITLE_STARTNOZZLE (NOZZLE_COUNT_PER_LINE/2+NOZZLE_COUNT_PER_LINE/4)

#define PATTERN_BASE_DPI                X_BASE_RES
#if defined(MANUFACTURER_DYSS)&&defined(UV_PRINTER)
#define DOTCHECK_LINECOUNT_PERPASS      1
#else
#define DOTCHECK_LINECOUNT_PERPASS      4
#endif
#define DOTCHECK_FIXDOT_LINECOUNT_PERPASS   32

#ifdef MANUFACTURER_DYSS
#define CHECKNOZZLE_NOZZLE_LINEWIDTH (100*X_WIDTH_MULTIPLE)  //0.42cm
#define CHECKNOZZLE_MAX_STEP 4
#else
#define CHECKNOZZLE_STEPNOZZLE          10
#define CHECKNOZZLE_NOZZLE_LINEWIDTH    (72*X_WIDTH_MULTIPLE)  //unit: X_BASE DPI
#endif

#define STEPCHECK_UNITCOUNT         11
#define STEPCHECK_UNITLINEWIDTH     (108*X_WIDTH_MULTIPLE)  //unit: X_BASE DPI
#define STEPCHECK_UNITLINE_OVERLAP  (STEPCHECK_UNITLINEWIDTH/3)   //unit: X_BASE DPI
#define STEPCHECK_UNITLINE_INTERVAL STEPCHECK_UNITLINEWIDTH   //unit: X_BASE DPI
#define STEPCHECK_COLORINTERVAL     (2*STEPCHECK_UNITLINE_INTERVAL)   //unit: X_BASE DPI
#define STEPCHECK_COLORNUM          2   //only print with K and M.
#define STEPCHECK_CHAR_WIDTH        (32*X_WIDTH_MULTIPLE)
#define STEPCHECK_DIGITAL_STARTNOZZLE   (NOZZLE_COUNT_PER_LINE/2)
#define STEPCHECK_TITLE_STARTNOZZLE (NOZZLE_COUNT_PER_LINE/2+NOZZLE_COUNT_PER_LINE/4)
#define STEPCHECK_BASEPASSNUM       4
#define STEPCHECK_STARTNOZZLE       24
#define STEPCHECK_NOZZLECOUNT       6
#define STEPCHECK_NOZZLESTEP        8
#define STEPCHECK_REPEATCOUNT       35  //about 10 circle of axis.
#define STEPCHECK_BAR_WIDTH         STEPCHECK_UNITLINEWIDTH  //unit: X_BASE DPI

#define STEPCHECK_MICRO_UNITCOUNT   17

//for angle check
#define VERTICALCHECK_LINECOUNT     36
#define VERTICALCHECK_LINEINTERVAL  36
#define VERTICALCHECK_COLORINTERVAL 144
#define VERTICALCHECK_HEADINTERVAL  (144*2)
#define VERTICALCHECK_MAX_COLORNUM  8
#define VERTICALCHECK_COLORNUM      COLOR_NUMBER_CALIBRATION
#define VERTICALCHECK_TITLE_STARTNOZZLE     (NOZZLE_COUNT_PER_LINE/2+NOZZLE_COUNT_PER_LINE/4)

#define INTERLEAVECHECK_NOZZLECOUNT         3
#define INTERLEAVECHECK_NOZZLESTEP          9
#define INTERLEAVECHECK_TITLE_STARTNOZZLE   (NOZZLE_COUNT_PER_LINE/2+NOZZLE_COUNT_PER_LINE/4)
#define INTERLEAVECHECK_UNITLINEWIDTH       (X_BASE_RES*2/5)  //unit: X_BASE DPI
#define INTERLEAVECHECK_UNITLINE_OVERLAP    (INTERLEAVECHECK_UNITLINEWIDTH/2)  //unit: X_BASE DPI
#define INTERLEAVECHECK_COLORINTERVAL       (INTERLEAVECHECK_UNITLINEWIDTH/2)  
#define INTERLEAVECHECK_COLORNUM            COLOR_NUMBER_CALIBRATION
#ifdef HEAD_RICOH_G4
#define INTERLEAVECHECK_UNITCOUNT           1
#else
#define INTERLEAVECHECK_UNITCOUNT           4
#endif
#define INTERLEAVECHECK_UNITINTERVAL        (INTERLEAVECHECK_UNITLINEWIDTH*2)

#define LEFTCHECK_UNITCOUNT         21
#define LEFTCHECK_LINECOUNT         (24*X_WIDTH_MULTIPLE)
#define LEFTCHECK_LINEINTERVAL      (24*X_WIDTH_MULTIPLE)
#define LEFTCHECK_COLORINTERVAL     (192*X_WIDTH_MULTIPLE)

#if defined(RICOH_G5_3H)||defined(RICOH_G5_4H) 
#define LEFTCHECK_STARTNOZZLE       24
#define LEFTCHECK_ENDNOZZLE         264
#else
#define LEFTCHECK_STARTNOZZLE       24
#define LEFTCHECK_ENDNOZZLE         144
#endif
#define LEFTCHECK_SPACENOZZLE       ((LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE)/3)
#define LEFTCHECK_TITLE_STARTNOZZLE (NOZZLE_COUNT_PER_LINE - 32)
#define LEFTCHECK_INFO_MAX_CHAR_NUM 7
#define LEFTCHECK_CHAR_WIDTH        (32*X_WIDTH_MULTIPLE)

#define YINTERVALCHECK_LINECOUNT    90
#define YINTERVALCHECK_DOTCOUNT_STEP    4
#define YINTERVALCHECK_DOTCOUNT_START   4
#define YINTERVALCHECK_BLOCKSHIFT   36
#define YINTERVALCHECK_COLORNUM     COLOR_NUMBER_CALIBRATION
#define YINTERVALCHECK_UNITINTERVAL (X_BASE_RES/2)
#define YINTERVALCHECK_TITLE_STARTNOZZLE   (NOZZLE_COUNT_PER_LINE/2+NOZZLE_COUNT_PER_LINE/4)

#define YINTERVALCHECK_COLOR_BLANK1 5
#define YINTERVALCHECK_COLOR_BLANK2 6
#define YINTERVALCHECK_COLOR_HOR_LEN ((float)1.0/2.54*720)
#define YINTERVALCHECK_COLOR_HOR_DPI 180

//for left and right angle check
#define ANGLECHECK_UNITCOUNT        7
#define ANGLECHECK_LINECOUNT        36
#define ANGLECHECK_LINEINTERVAL     36
#define ANGLECHECK_UNITINTERVAL     72
#define ANGLECHECK_OVERLAP_NOZZLE   10
#define ANGLECHECK_TITLE_STARTNOZZLE    (NOZZLE_COUNT_PER_LINE/2+NOZZLE_COUNT_PER_LINE/4)
#define ANGLECHECK_INFO_MAX_CHAR_NUM    2

//for vertical calibration
#define VERTICAL_CALI_UNITCOUNT         11
#define VERTICAL_CALI_NOZZLECOUNT       6
#define VERTICAL_CALI_NOZZLESTEP        8
#define VERTICAL_CALI_STARTNOZZLE       24
#define VERTICAL_CALI_UNITLINEWIDTH     (108*X_WIDTH_MULTIPLE)  //unit: X_BASE DPI
#define VERTICAL_CALI_UNITLINE_OVERLAP  (VERTICAL_CALI_UNITLINEWIDTH/3)   //unit: X_BASE DPI
#define VERTICAL_CALI_UNITLINE_INTERVAL VERTICAL_CALI_UNITLINEWIDTH   //unit: X_BASE DPI

//for vertical alignment check.
#define VERTICAL_ALIGN_UNITLINEWIDTH     (360*X_WIDTH_MULTIPLE)  //unit: X_BASE DPI
#define VERTICAL_ALIGN_UNITLINE_OVERLAP  (VERTICAL_ALIGN_UNITLINEWIDTH/3)   //unit: X_BASE DPI
#define VERTICAL_ALIGN_UNITLINE_INTERVAL VERTICAL_ALIGN_UNITLINEWIDTH   //unit: X_BASE DPI

#define HWTEST_DYN_PASSNUM          1000
#define HWTEST_DYN_WIDTH            (10000*X_WIDTH_MULTIPLE)

extern INT8U ASC_Bmp_Tab[];
#define ASCII_WIDTH     8
#define ASCII_HEIGHT    16
#ifdef HEAD_RICOH_G4
extern INT8U Datamap_to_Hardmap[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP];
#endif
struct PrinterSetting_Cali
{
	INT16U DPI_X, DPI_Y;
	INT32U PrintRegionWidth;
	
	INT8U bIntegratedHead;
	
	INT8U HeadCount;
	INT8U GroupCount;
	INT8U GroupCount_YContinue;
	INT8U GroupCount_YInterleave;
	INT8U ColorCount;
	INT8U YInterleaveNum_Head; //head itself interleave number.
	INT8U YInterleaveNum;   //total interleave number, contains multi-head merged
	INT8U YInterleaveNum_InHead; //interleave number inside a head. 
	INT8U BusWidth;
	
	INT8U bOriginIsRight;
	INT8U bHeadDirIsPositive;
	INT8U bHeadElectricDir; 
	
	INT16U NozzleCount;
	INT16U BytesPerFire;
	
	INT8U head_group;
	//for 8 color layout & 2 head & 2 head is Y continue, we can't calibrate Vertical offset with head nozzle shift. 
	//  Only stepping Y to align nozzle, BUT stepping Y is NOT stable. 
	//  So we don't do vertical calibrate, only do HeadYIntervalCheck to check this special case.
	INT8U bVerticalValid;
	INT16S verticalNozzleShift;
	INT8U bYIntervalReplaceVertical; 
	
	INT8U BaseColor_ColorCode;
	INT8U BaseColor_Index;
	
	INT8U BaseColor_HeadIndex; //用于打印title。对于大部分校正图案，也用于calibration的base。
	INT8U BaseColor_LineIndex[INTERLEAVE_CALIBRATION]; //仅仅包含一个头内部的nozzle line.
	INT8U YStepColor_Index[STEPCHECK_COLORNUM]; //it is lineIndex extension = headIndex * NOZZLE_LINE_COUNT_PER_HEAD + lineIndex.
	
	INT8U VerticalColor_Index[VERTICALCHECK_MAX_COLORNUM]; //same as YStepColor_Index. not used.
};

struct WCV_POSITION
{
  INT8S  colornum;   
  INT8S  colorindex;  
};

struct ColorMap_Cali
{
	INT8U BaseHeadNo[MAX_COLOR_NUMBER_CALIBRATION]; //用于水平校正时的base.
	INT8U BaseLineNo[MAX_COLOR_NUMBER_CALIBRATION]; //用于水平校正时的base.
	
	//here, LineIndex is base on printhead coor system itself.
	//factoryDataEx.m_nColorOrder[colorIndex] is based on user coor system (from right to left)(printhead maybe rotated).
	INT8U ColorCode[MAX_HEAD_NUMBER_CALIBRATION][NOZZLE_LINE_COUNT_PER_HEAD]; 
	INT8U ColorIndex[MAX_HEAD_NUMBER_CALIBRATION][NOZZLE_LINE_COUNT_PER_HEAD];
	//logic channel map from head/line to channel NO. 
	//  however, it already contains IndexShiftMap.
	//  IndexShiftMap will consider Head swap, head disable, and Hardware-map.
	INT8U ColorIndex2DataChannelIndex[MAX_HEAD_NUMBER_CALIBRATION][NOZZLE_LINE_COUNT_PER_HEAD];
};

struct Head_SinglePos_Cali
{//based on printhead coor system itself
	float XOffset[NOZZLE_LINE_COUNT_PER_HEAD];
	float YOffset[NOZZLE_LINE_COUNT_PER_HEAD];
};

struct Head_GroupPos_Cali
{ //for ricoh, it is headPos, not group position.
	float XOffset[MAX_HEAD_NUMBER_CALIBRATION];
	float YOffset[MAX_HEAD_NUMBER_CALIBRATION];
};

struct Head_X_Offset_Cali
{   //For BaseColor Index 0, it is the base of all head /all line/ all color.
	//  After completed 8 colors feature,  K is the base of all head /all line/ all color in calibration picture. 
	//  however, BaseColor Index 0 is the origin of all XOffset.
	//For BaseLine of one color, it is the base of all head/line of one color.
	
	//here, LineIndex is base on printhead coor system itself.
	signed char XOffset[MAX_HEAD_NUMBER_CALIBRATION][NOZZLE_LINE_COUNT_PER_HEAD];
};

struct Head_Y_Offset_Cali
{
	//always be 喷头的最大分辨率。即NOZZLE_DPI * INTERLEAVE_CALIBRATION。
	//目前实际有效精度是NOZZLE_DPI。等待改进（20120717）。
	unsigned short nDPI; 
	//仅仅记录偏移量，这意味着不同Group的值是一样的。
	signed char YOffset[MAX_HEAD_NUMBER_CALIBRATION][NOZZLE_LINE_COUNT_PER_HEAD];
};

struct Group_X_Offset_Cali
{
	unsigned short nDPI;
	signed short XOffset[MAX_VSDMODEL_NUM][MAX_HEAD_NUMBER_CALIBRATION];
};

struct Head_Step_Cali
{
	unsigned int BaseStep;
	signed short PassStep[MAX_PASS_NUMBER_CALIBRATION];
};

struct EPSON_X_Cali
{
	unsigned short nDPI;
	struct Head_X_Offset_Cali xPosDirOffset;
	struct Head_X_Offset_Cali xNegDirOffset;
};

struct EPSON_Angle_X_Cali
{   //assert all nozzle line in one head is same.
	//based on the first nozzle, the X shift of the last nozzle.
	unsigned short nDPI;
	signed char xPosDirOffset[MAX_HEAD_NUMBER_CALIBRATION]; 
	signed char xNegDirOffset[MAX_HEAD_NUMBER_CALIBRATION];
};

//#define EPR_EPSON_CALI_SETTING_VERSION  0x101 //initial version
//20110612, add height level selection for xOffset, and  change MAX_HEAD_NUMBER_CALIBRATION from 4 to 2 because the content is too large.
//#define EPR_EPSON_CALI_SETTING_VERSION  0x102 
//20110723, add headOffset for 2-head system.
//#define EPR_EPSON_CALI_SETTING_VERSION  0x103 
//20120130, support 8 color and ricoh.
//#define EPR_EPSON_CALI_SETTING_VERSION  0x104
//20120703,support eprCaliSetting update

#ifdef HEAD_EPSON_GEN5
#ifdef EPSON_4H
#define EPR_EPSON_CALI_SETTING_FLAG     0x9EA2
#define EPR_EPSON_CALI_SETTING_FLAG_1     EPR_EPSON_CALI_SETTING_FLAG
#else
#define EPR_EPSON_CALI_SETTING_FLAG     0x9EE2
#define EPR_EPSON_CALI_SETTING_FLAG_1     0x9EE3
#endif
#else
#define EPR_EPSON_CALI_SETTING_FLAG     0x9E52
#define EPR_EPSON_CALI_SETTING_FLAG_1     0x9E53
#endif
#define EPR_EPSON_CALI_SETTING_VERSION  0x106
#define SUPPORT_CALI_UPDATE_BASE_VERSION  0x105


struct EPR_EPSON_CaliSetting
{
	INT16U len;
	INT16U version;
	
	struct EPSON_X_Cali xOffset[MAX_HEIGHT_LEVEL][MAX_X_RES_NUM][MAX_VSDMODEL_NUM];
#if defined(HEAD_EPSON_GEN5) && !defined(EPSON_4H) //to save eeprom space for EPSON4H & RICOH.
	struct EPSON_Angle_X_Cali xAngleOffset[MAX_HEIGHT_LEVEL][MAX_X_RES_NUM][MAX_VSDMODEL_NUM]; 
#endif
	struct Head_Y_Offset_Cali yOffset;
	struct Head_Step_Cali stepPass[MAX_MEDIA_TYPE_CALIBRATION];
	//完整的水平偏移是有两部分叠加而成的。一部分是headOffset。一部分是xOffset。
	//  xOffset仅仅记录头内部的偏移(在不同速度下)。
	//  实际情况更复杂。为了保证叠加后的水平偏移反应实际情况，
	//  （叠加后的水平偏移应该随着速度发生变化，headOffset仅仅记录了头之间的物理偏移，它是以最慢的速度测量的），
	//  xOffset是在叠加后的水平偏移上减去一个固定的物理偏移，xOffset事实上包含头之间的物理偏移在不同速度下的变化量。
	struct Group_X_Offset_Cali headOffset;
};

struct EPSONCaliSetting
{
	struct PrinterSetting_Cali printerSetting;
	struct ColorMap_Cali colorMap;
	struct Head_SinglePos_Cali singleSetting;
	struct Head_GroupPos_Cali groupSetting;
	struct EPR_EPSON_CaliSetting eprCaliSetting;
};

struct Head_X_Offset_Cali_Runtime
{
	//here, LineIndex is base on printhead coor system itself.
	INT32S XOffset[MAX_HEAD_NUMBER_CALIBRATION][NOZZLE_LINE_COUNT_PER_HEAD];
};

struct Head_Y_Offset_Cali_Runtime
{
	INT32S YOffset[MAX_HEAD_NUMBER_CALIBRATION][NOZZLE_LINE_COUNT_PER_HEAD];
};

struct EPSON_X_Cali_Runtime
{
	INT32S PosDirOriginOffset;
	INT32U PosDirAllHeadWidth;
	struct Head_X_Offset_Cali_Runtime xPosDirOffset;
	INT32S NegDirOriginOffset; //based on PosDirOriginOffset.
	INT32U NegDirAllHeadWidth;
	struct Head_X_Offset_Cali_Runtime xNegDirOffset;
};

struct EPSON_Angle_X_Cali_Runtime
{
	INT8U bEnable;
	INT32U PosDirWidthEx; //based on EPSON_X_Cali_Runtime.PosDirAllHeadWidth
	INT32S PosDirOriginOffset; //based on EPSON_X_Cali_Runtime.PosDirOriginOffset
	INT8S xPosDirOffset[MAX_HEAD_NUMBER_CALIBRATION];
	INT32U NegDirWidthEx; //based on EPSON_X_Cali_Runtime.NegDirAllHeadWidth
	INT32S NegDirOriginOffset; //based on EPSON_X_Cali_Runtime.NegDirOriginOffset
	INT8S xNegDirOffset[MAX_HEAD_NUMBER_CALIBRATION];
};

struct EPSONCaliSetting_Runtime
{
	struct EPSON_X_Cali_Runtime xOffset;
	struct EPSON_Angle_X_Cali_Runtime angleXOffset;
	struct Head_Y_Offset_Cali_Runtime yOffset;
	struct Head_Step_Cali stepPass;
};

#if defined(RIPSTAR_FLAT_EX)  
extern struct EPSONCaliSetting_Runtime epsonCaliSetting_runtime;
#endif
struct CaliPrintParam
{
	INT8U VSDModel;
	INT8U DotSetting;
	INT16U DPI;
	INT32U StartPosX;
	
	//for Y Step calibration.
	INT8U MediaType;
	INT8U PassNum;
	
	INT8U Option; //for most case, bit 0: 1 is engineer model, 0 is user model. 
	//bit 1~2: if consider X divide effection or not. if yes, the real X DPI of calibration is CaliPrintParam.DPI*divide number. 
	//  divide number = ((option & 0x6) >> 1) + 1. usually, set divide number as 1 to ignore it.
	//bit 3: if consider angle or not. if to consider angle, some calibration pattern is changed.
};

struct CaliPatternRuntimeParam
{
	struct EPSON_X_Cali_Runtime* pCurXCali;
	INT32S curBandY;
	INT32U curpassNO;
	INT8U curDir;
	INT32S PassBuffSize;
	INT8U bCacheBuff;
	INT8U * pTargetBuf;
	INT8U FireBuf[FIRE_BLOCK_SIZE];
	INT32S AllHeadWidth;
	INT32S LineOffsetTable[MAX_HEAD_NUMBER_CALIBRATION*NOZZLE_LINE_COUNT_PER_HEAD]; 
	INT32S PatternShift;
};

extern struct EPSONCaliSetting epsonCaliSetting;

//IndexShiftMap 包括所有的index map，包括head disable引起的, HEAD_2_SWAP引起的（以及类似原因：接线方便），
//  以及头板硬件映射关系引起的。
extern INT8U IndexShiftMap[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP];


//initial all calibration setting. 
//  contains to read EPR_EPSON_CaliSetting.
//  contains to initial EPSONCaliSetting.
//  contains to initial pmCaliSetting and bPMCaliSettingDirty.
//  contains to initial Default CaliPrintParam.
//  contains to initial runtime Cali setting.
INT8U InitCalibration(void); 
//INT8U StartPrintCali(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam);
void Cali_Task(void* data);

INT8U StartCalibrationJob(struct CaliPrintSetting * pInfo);
INT8U StartHWDynOffsetJob(INT8U VsdModel, INT8U Loading);

INT8U ConvertCaliSettingToPMWithPMFormat(struct EPSONCaliSetting * pCaliSetting, SPMCaliSettingType * pPMCaliSetting);
INT8U ConvertCaliSettingToFWWithPMFormat(SPMCaliSettingType * pPMCaliSetting, struct EPSONCaliSetting * pCaliSetting);
INT8U ConvertCaliSettingToFWWithFWFormat(SPMCaliSettingType * pPMCaliSetting, struct EPSONCaliSetting * pCaliSetting);
INT8U ConvertCaliSettingToPMWithFWFormat(struct EPSONCaliSetting * pCaliSetting, SPMCaliSettingType * pPMCaliSetting);

INT8U SaveEPSONCaliSetting();
INT8U ReadEPSONCaliSetting();
INT8U SaveEPSONCaliConfig();
INT8U ReadEPSONCaliConfig();

INT8U ResetCalibration();
INT8U ResetMenuConfig();

INT32U GetStepPerHead();
//INT32U GetSprayRegion(INT32S sprayPos, INT32U sprayWidth);
//INT8U FillSprayRegion(INT8U * pBuff, INT32U sprayWidth, 

INT8U GetDataChannelMap(INT8U *pBuf,INT8U num);
INT8S GetNozzleLineIndex(INT8U colorCode, INT8U interleaveNO);
INT8S GetHeadIndexWithInterleave(INT8U groupNo_YContinue, INT8U colorCode, INT8U interleaveNO);
//INT8S GetHeadIndex(INT8U groupNo, INT8U colorCode);
INT8U GetCalibrationLength(INT8U *EP0INdata);
INT8U InCaliData(INT8U *EP0INdata,INT16U index,INT16U setuplength);
INT8U OutCaliData(INT8U *EP0OUTdata,INT16U index,INT16U setuplength);
#endif

#endif

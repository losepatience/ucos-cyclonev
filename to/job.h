#ifndef _JOB_H_
#define _JOB_H_

#include "global.h"

#define JOB_START_HEAD	0x1
#define BAND_HEAD		0x2
#define JOB_END_HEAD	0x3
#define UPDATER_HEAD	0x4

#define UPDATEHB_WAITTIME	5000
typedef struct _tag_ARM7_FW_HEADER
{
	INT32U jumper[8];	// 中断跳转指令区
	INT32U vector[8];	// 中断向量区
	INT32U FwFlag;		// FW标识，'HBV2', 'FUNC'
	INT8U  FwID;		// Fw标识，即头板类型
	INT8U  FwCategory;	// 头板Category
	INT8U  reserved1;
	INT8U  reserved2;
	INT32U version;		// 头板版本
	INT32U reserved99[13];	// 未使用区域
} ARM7FwHeader_t;

#define MAKE_4CHAR_CONST(chA,chB,chC,chD)	(INT32U)(chD | (chC << 8) | (chB << 16) | (chA << 24))
#define MAKE_4CHAR_CONST_BE(chA,chB,chC,chD)	(INT32U)(chA | (chB << 8) | (chC << 16) | (chD << 24))

struct Struct_JobStart{
	INT32U encoder_div;
	INT32S dir_space;  //bidirection position difference
	INT32U jobHeight;  //Total height
	INT32U speed; 	   //Print speed.	
	INT32U  blankBandDryTimeMS; 
	INT32U  language;  //UI language setting
	INT8U   languageStr[16];
	INT32U  YDir; //0: Normal printing; 1: reverse printing
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	INT8U	dataBitDeep;
	INT8U	printBitDeep;
	INT8U   mappedDataFor1Bit;
	INT8U   VSDModel; //0: None, 1: VSD1, 2: VSD2, 3: VSD3, 4:VSD4.
	INT8U   baseDPI; //0: 720, 1: 540, 2: 1440.
	INT8U   PassNumber;
	INT16U  displayResolution;
#endif	
};

struct Struct_Band
{
	INT32U	dir;		//1:正向(远离原点) 0:反向
	INT32U	band_width;			
	INT32U 	band_X;				
	INT32U 	band_Y;			//Cur band Y position	
	INT32U	step_distance;	//打完后,步进量	
	
	INT32U	next_dir;
	INT32U	next_band_width;
	INT32U  next_band_X;
	INT32U  compressMode; //1: 12 channal data 0: 16 channal data, FW not use so far
	INT32U  stepTimeMS;   //FW not use
	
	// New ADD for Y encoder
	INT32U  nextBandY;    
	INT32S	shiftY;
	INT32U  bandResY;
	INT32S  bandIndex;
	INT32U  compress_data_bytescnt;	
	INT32U  dotCount[MAX_COLOR_NUMBER];
	INT32U  dotCheckSum;
	INT32U  GapStartPos; //new added. must be even number, because 2 fire share a trigger signal.	
};

/*struct Struct_JobEnd  //No use
{
}*/

struct Struct_Updater
{
	INT32U	data_bytescnt; //True size, not aliagned
	INT32U	offset;
	INT32U  mask;
	INT32U	checksum;
	
	INT32U  Fileflag;     //'BYHX'
	INT32U  CheckSum;     //
	INT32U  FileVersion;  //need add HW version , FW version , Manufactory ID?????????
	INT32U  FWNum;        // Used number
	INT32U  SubAddressTable[8]; // 0 means 
	INT32U  SubSizeTable[8];
	INT32U  FWId[8];     //20 FWMain  21 FWMotion 22 FWHead
	INT32U  Reserve[8];
	
};

typedef struct Struct_PackageHead{
	INT32U header_flag; //"BYHX"
	INT32U header_size; //Must be HUGEBUF_ALIGN_SIZE alianed
	INT32U header_type;//Job info, bandinfo, endjob, update
	INT32U data_bytescnt; //Not include header, transfer count, HUGEBUF_ALIGN_SIZE aligned
	union {
		struct Struct_JobStart 	job_inf;
		struct Struct_Band 		band_inf;
		//		struct Struct_JobEnd	job_end;
		struct Struct_Updater	updater_inf;
	}Inf;
}DataHeaderType;

#endif

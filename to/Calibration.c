#include "os_includes.h"
#include "consol.h"
#include "flash.h"
#include "tasks.h"
#include "FPGA.h"
#include "miscellance.h"
#include "status.h"
#include "hugebuf.h"
#include "keylcd.h"
#include "global.h"
#include "control.h"
#ifdef UV_PRINTER
#include "uv.h"
#endif

#include "calibration.h"
#if 0
#include "usb/device/core/usbd.h"
#endif

#include "irq/irq.h"

#include "display.h"

#ifdef HEAD_RICOH_G4
extern Pin Pin_FPGA_nHalfEmpty;
#endif

#if (defined( MANUFACTURER_TATE_EPSON )||defined( A1802 )||defined(MANUFACTURER_HUMAN_EPSON))&&!defined( HEAD_RICOH_G4 )
#define VSD2TOVSD3
#endif 

#ifdef HEAD_RICOH_G4
INT8U Datamap_to_Hardmap[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={12,13,8,9,4,5,0,1,14,15,10,11,6,7,2,3,} ;
INT8U Datamap_to_Hardmap_NEG[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={12,13,8,9,4,5,0,1,14,15,10,11,6,7,2,3,} ;
//INT8U Datamap_to_Hardmap_NEG[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={0,1,4,5,8,9,12,13,2,3,6,7,10,11,14,15,} ;need test
#ifdef RICOH_G5_3H 
//need to test.
INT8U Datamap_to_Hardmap_3H[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={8,10,9,11,4,6,5,7,0,2,1,3,12,14,13,15,} ;
//INT8U Datamap_to_Hardmap_3H[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={11,10,9,8,7,6,5,4,3,2,1,0,15,14,13,12,} ;
#elif defined(RICOH_G5_4H)
INT8U Datamap_to_Hardmap_G5_4H_4C[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={0,2,1,3,12,14,13,15,4,6,5,7,8,10,9,11} ;
INT8U Datamap_to_Hardmap_G5_4H_8C[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={0,2,1,3,4,6,5,7,12,14,13,15,8,10,9,11} ;
#else
INT8U Datamap_to_Hardmap_3H[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] ={4,5,2,3,0,1,6,7,8,9,10,11,12,13,14,15,} ;
#endif                
#endif

#ifdef HEAD_EPSON_GEN7
INT8U EPSON_GEN5_TO_GEN7[] = {4,1,2,7,6,3,0,5};
#elif defined(HEAD_EPSON_GEN5S)
INT8U EPSON_GEN5_TO_GEN7[] = {3,5,4,6,2,1,0,7};
#endif

#ifdef EPSON_4H
INT8U EPSON_4H_Head_HardMap[] = {3,2,0,1};
#endif

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)

//For Head layout coor( it is independ!!), X is from right to left.
//Y is from far to near when viewed from Printer front.
//  For Y, the first fire head is head 0.
//  For Y, the first nozzle is the first fire nozzle also.

//motion X movement coor is from origin to terminal.

struct EPSONCaliSetting epsonCaliSetting SLOW_DATA_SECTION;
struct EPSONCaliSetting_Runtime epsonCaliSetting_runtime SLOW_DATA_SECTION;
struct CaliPatternRuntimeParam runtimeParam SLOW_DATA_SECTION;
struct CaliPrintParam caliParam;

static INT16U  ALLLEN;
extern INT8U calierrtype;


extern volatile INT8U valueFlag_FactoryData_Not_Init;
extern volatile INT8U valueFlag_BYHXData_Not_Init;

//IndexShiftMap 包括所有的index map，包括head disable引起的, HEAD_2_SWAP引起的（以及类似原因：接线方便），
//  以及头板硬件映射关系引起的。
INT8U IndexShiftMap[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] = {0};

INT8U CalcRuntimeParam(void);
void InitDataChannelMap();
INT8U GetNoBasePassNO_ForHorizatalLikeCheck(INT8U divideNum, INT8U colorNO, INT8U bAllBase,
											INT8U headNO_in_group, INT8U * baseGroupNO,
											INT16S * StepNozzleBefNobase, INT16S * StepNozzleAftNobase,INT8U *order);

INT8U GetColorMap()
{
	INT8U i,j,k;
	INT8U lineIndex;
	INT8U headIndex;
	INT8U head_max,head_group;
	INT8U colorIndex;
	
	head_group = epsonCaliSetting.printerSetting.head_group;
	head_max = epsonCaliSetting.printerSetting.GroupCount * head_group;
	
	for(i = 0; i<MAX_HEAD_NUMBER_CALIBRATION; i++)
	{
		headIndex = i;
		
		INT8U groupNO_YInterleave = (i/head_group) / epsonCaliSetting.printerSetting.GroupCount_YContinue;
		INT8U groupNo_YContinue = (i/head_group) % epsonCaliSetting.printerSetting.GroupCount_YContinue;
		
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++) //物理位置。当喷头包括若干个y向的拼插组，在组里，以Y向拼插为序（对于ricoh gen5，有意义）
		{
			if( epsonCaliSetting.printerSetting.bHeadDirIsPositive)
				lineIndex = j;
			else
				lineIndex = NOZZLE_LINE_COUNT_PER_HEAD - 1 - j;
			
			if( epsonCaliSetting.printerSetting.YInterleaveNum >=2 && ((factoryDataEx.LayoutType & 0x2) != 0) &&
			   epsonCaliSetting.printerSetting.YInterleaveNum % 2 == 0) 
			{   //对称色序.
				//对于ricoh gen5 h3, 暂时不支持对称色序。
				if( (factoryDataEx.LayoutType & 0x4) == 0 && 
				   (epsonCaliSetting.printerSetting.GroupCount_YInterleave >= 2) &&
					   (epsonCaliSetting.printerSetting.GroupCount_YInterleave % 2 == 0))
				{   //不同喷头（组）间是对称的。
					colorIndex = ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j)/epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
					if( groupNO_YInterleave % 2 != 0)
						colorIndex = head_group*NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead - 1 - colorIndex;
				}
				else
				{   //喷头（组）内部是对称的。
					if( ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j) < head_group*NOZZLE_LINE_COUNT_PER_HEAD/2)
					{
						colorIndex = ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j) *2 /(epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
					}
					else
					{
						colorIndex = (head_group*NOZZLE_LINE_COUNT_PER_HEAD - 1 - ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j))*2/(epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
					}
				}
			}
			else
			{
				INT8U Unit_NO = ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j)/epsonCaliSetting.printerSetting.YInterleaveNum_Head;
				INT8U InUnit_NO = ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j)%epsonCaliSetting.printerSetting.YInterleaveNum_Head;
				
				colorIndex = Unit_NO * (epsonCaliSetting.printerSetting.YInterleaveNum_Head/epsonCaliSetting.printerSetting.YInterleaveNum_InHead) +
					InUnit_NO % (epsonCaliSetting.printerSetting.YInterleaveNum_Head/epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
			}
			
			if( headIndex >= head_max || colorIndex >= epsonCaliSetting.printerSetting.ColorCount)
			{
				epsonCaliSetting.colorMap.ColorCode[headIndex][lineIndex] = COLORCODE_UNDEF;
				epsonCaliSetting.colorMap.ColorIndex[headIndex][lineIndex] = -1;
			}
			else
			{
				epsonCaliSetting.colorMap.ColorCode[headIndex][lineIndex] = factoryDataEx.m_nColorOrder[colorIndex];
				epsonCaliSetting.colorMap.ColorIndex[headIndex][lineIndex] = colorIndex;
			}
#if 0					
#if defined( HEAD_EPSON_GEN7)
			if( epsonCaliSetting.printerSetting.bIntegratedHead)
				epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headIndex][lineIndex] = 
					headIndex * NOZZLE_LINE_COUNT_PER_HEAD + EPSON_GEN5_TO_GEN7[lineIndex];
#else
			if( epsonCaliSetting.printerSetting.bIntegratedHead)
				epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headIndex][lineIndex] = 
					headIndex * NOZZLE_LINE_COUNT_PER_HEAD + lineIndex;
#endif	
			else
				epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headIndex][lineIndex] = 
					headIndex * epsonCaliSetting.printerSetting.YInterleaveNum_Head + lineIndex;
#endif	
		}
	}
	return True;
}

#define DEFAULT_NOZZLELINE -1
//interleaveNO, sorted by physical space. 
//  For the case of epson 2H/4C and interleave to 720DPI, 
//  interleaveNO 0 means H0.K0, 
//  interleaveNO 1 means H1.K0, 
//  interleaveNO 2 means H0.K1, 
//  interleaveNO 3 means H1.K1.
INT8S GetNozzleLineIndex(INT8U colorCode, INT8U interleaveNO)
{
	INT8S i,j;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	INT8U groupNO_YInterleave = interleaveNO % epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	INT8U interleaveNO_InHead = interleaveNO / epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	
	INT8U real_interleaveNO_InHead;
	if(epsonCaliSetting.printerSetting.bHeadDirIsPositive)
		real_interleaveNO_InHead = interleaveNO_InHead;
	else
		real_interleaveNO_InHead = epsonCaliSetting.printerSetting.YInterleaveNum_Head - 1 - interleaveNO_InHead;
	real_interleaveNO_InHead %= epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
	
	//考虑到对称色序。起始喷头序号可以满足对称色序（不同组间是对称的）的要求。
	//  interleaveNO的检查，可以满足对称色序（组内部是对称的）的要求。
	for ( i = head_group*epsonCaliSetting.printerSetting.GroupCount_YContinue*groupNO_YInterleave; 
		  i < head_group*epsonCaliSetting.printerSetting.GroupCount_YContinue*groupNO_YInterleave + head_group ; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			INT8U Unit_NO = ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j)/epsonCaliSetting.printerSetting.YInterleaveNum_Head;
			INT8U InUnit_NO = ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j)%epsonCaliSetting.printerSetting.YInterleaveNum_Head;
			INT8U Cur_interleaveNO_InHead = InUnit_NO / (epsonCaliSetting.printerSetting.YInterleaveNum_Head/epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
			
			if( Cur_interleaveNO_InHead == real_interleaveNO_InHead &&
			   colorCode == epsonCaliSetting.colorMap.ColorCode[i][j])
				return j;
		}
	}
	
	return DEFAULT_NOZZLELINE;
}

#define DEFAULT_HEADLINE -1
INT8S GetHeadIndexWithInterleave(INT8U groupNo_YContinue, INT8U colorCode, INT8U interleaveNO)
{
	INT8S i,j;
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	INT8U groupNO_YInterleave = interleaveNO % epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	INT8U interleaveNO_InHead = interleaveNO / epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	
	INT8U real_interleaveNO_InHead;
	if(epsonCaliSetting.printerSetting.bHeadDirIsPositive)
		real_interleaveNO_InHead = interleaveNO_InHead;
	else
		real_interleaveNO_InHead = epsonCaliSetting.printerSetting.YInterleaveNum_Head - 1 - interleaveNO_InHead;
	real_interleaveNO_InHead %= epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
	
	for ( i = head_group*epsonCaliSetting.printerSetting.GroupCount_YContinue*groupNO_YInterleave; 
		  i < head_group*epsonCaliSetting.printerSetting.GroupCount_YContinue*groupNO_YInterleave + head_group ; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			INT8U Unit_NO = ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j)/epsonCaliSetting.printerSetting.YInterleaveNum_Head;
			INT8U InUnit_NO = ((i % head_group) * NOZZLE_LINE_COUNT_PER_HEAD  + j)%epsonCaliSetting.printerSetting.YInterleaveNum_Head;
			INT8U Cur_interleaveNO_InHead = InUnit_NO / (epsonCaliSetting.printerSetting.YInterleaveNum_Head/epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
			
			if( Cur_interleaveNO_InHead == real_interleaveNO_InHead &&
			   colorCode == epsonCaliSetting.colorMap.ColorCode[i][j])
				return i + head_group * groupNo_YContinue;
		}
	}
	return DEFAULT_HEADLINE;
}

//We assert that interleaveNO is 0. For most case, it is ok. 
// but, for 对称色序（组内部是对称的）并且 (!bIntegratedHead)，it will be error.
// 建议尽量使用 GetHeadIndexWithInterleave
INT8S GetHeadIndex(INT8U groupNo, INT8U colorCode)
{
	INT8S i,j;
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	INT8U groupNO_YInterleave = groupNo / epsonCaliSetting.printerSetting.GroupCount_YContinue;
	INT8U groupNo_YContinue = groupNo % epsonCaliSetting.printerSetting.GroupCount_YContinue;
	
	return GetHeadIndexWithInterleave(groupNo_YContinue, colorCode, 
									  groupNO_YInterleave * epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
	
	/*
	for ( i = head_group*epsonCaliSetting.printerSetting.GroupCount_YContinue*groupNO_YInterleave; 
	i < head_group*epsonCaliSetting.printerSetting.GroupCount_YContinue*groupNO_YInterleave + head_group ; i++)
	{
	for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
	{
	if( colorCode == epsonCaliSetting.colorMap.ColorCode[i][j])
	return i + head_group * groupNo_YContinue;
}
}
	return DEFAULT_HEADLINE;
	*/    
}

void InitIndexShiftMap()
{
	int index_shift = 0, i,j;
	INT8U head_group = epsonCaliSetting.printerSetting.head_group;
	
	//for epson 2H, keep the old order. HEAD_2_SWAP will swap the order of this 2 head.
	//for epson 4H, always swap head order to get the shortest connect cable.
	//for ricoh. keep the native map. 
	
	//IndexShiftMap 包括所有的index map，包括head disable引起的, HEAD_2_SWAP引起的（以及类似原因：接线方便），
	//  以及头板硬件映射关系引起的。
	//IndexShiftMap[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] = {0};
#ifdef HEAD_RICOH_G4    
	if(factoryData.m_fHeadXColorSpace < 0)
		memcpy(Datamap_to_Hardmap, Datamap_to_Hardmap_NEG, MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP);

#ifdef RICOH_G4_16H
	for(i = 0;i<16;i++)
		Datamap_to_Hardmap[16+i] = Datamap_to_Hardmap[i] + 16;
#endif

#endif    
	INT8U posi_head_map[MAX_HEAD_NUMBER_CALIBRATION]= {0}; //only for EPSON 4H. to get the shortest connection.
	if( epsonCaliSetting.printerSetting.bIntegratedHead && (head_group * EPSON_PERMIT_GROUPNUM == 4))
	{   
		//For epson 4H, always swap head order to get the shortest connection.  
		//得到所有喷头的X offset，然后排序，可以得到最佳喷头接线。
		//  头板的座是从右到左排列，喷头的X offset也按照右到左排列，这样一一对应，可以得到最佳喷头接线。
		//  一个特例是，喷头的X offset一样。这时，喷头序号小的接右边的座。
		float XOffset[MAX_HEAD_NUMBER_CALIBRATION];
		for(i=0; i<EPSON_PERMIT_GROUPNUM; i++)
		{
			INT8U GroupCount_YContinue = EPSON_PERMIT_GROUPNUM / epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			
			INT8U groupNO_YInterleave = i / GroupCount_YContinue;
			INT8U groupNo_YContinue = i % GroupCount_YContinue;
			
			for(j=0; j<head_group; j++)
			{
				posi_head_map[i*head_group + j]= i*head_group + j;
				
				//                if( epsonCaliSetting.printerSetting.GroupCount == 1 &&
				//                    epsonCaliSetting.printerSetting.ColorCount == 8 &&
				//                        epsonCaliSetting.printerSetting.YInterleaveNum == 2) //Special case: PM use m_fHeadXGroupSpace as m_fHeadXColorSpace.
				//                {
				//                    XOffset[i*head_group + j] = j * factoryData.m_fHeadXGroupSpace;
				//                }
				//                else 
				if((factoryDataEx.LayoutType & 0x8) == 1)
				{
					if( (factoryDataEx.LayoutType & 0x1) == 0)
					{
						XOffset[i*head_group + j] = i * factoryData.m_fHeadXGroupSpace + j * factoryData.m_fHeadXColorSpace;
					}
					else
					{
						XOffset[i*head_group + j] = (groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave + 
													 groupNO_YInterleave) * factoryData.m_fHeadXGroupSpace;
						XOffset[i*head_group + j] += j * factoryData.m_fHeadXColorSpace;
					}
				}
				else
				{
					groupNo_YContinue = groupNo_YContinue % 2; //for rollback.
					if( (factoryDataEx.LayoutType & 0x1) == 0)
					{
						XOffset[i*head_group + j] = (groupNO_YInterleave * GroupCount_YContinue + 
													 groupNo_YContinue) * factoryData.m_fHeadXGroupSpace;
						XOffset[i*head_group + j] += j * factoryData.m_fHeadXColorSpace;
					}
					else
					{
						XOffset[i*head_group + j] = (groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave + 
													 groupNO_YInterleave) * factoryData.m_fHeadXGroupSpace;
						XOffset[i*head_group + j] += j * factoryData.m_fHeadXColorSpace;
					}
				}
			}
		}
		
		for(i = 0; i < EPSON_PERMIT_GROUPNUM*head_group; i++)
		{
			for(j = i+1; j < EPSON_PERMIT_GROUPNUM*head_group; j++)
			{
				if(XOffset[i] > XOffset[j] + 0.001 || 
				   (XOffset[i] > XOffset[j] - 0.001 && posi_head_map[i] > posi_head_map[j])) // 0.001 仅仅为了可靠。
				{
					float ft = XOffset[i];
					XOffset[i] = XOffset[j];
					XOffset[j] = ft;
					
					INT8U u8t = posi_head_map[i];
					posi_head_map[i] = posi_head_map[j];
					posi_head_map[j] = u8t;
				}
			}
		}
	}
	
	INT8U head_shift = 0;
	INT8U active_groupNum = EPSON_ACTIVE_GROUPNUM;
	if(EPSON_PERMIT_GROUPNUM == EPSON_ACTIVE_GROUPNUM)
	{
		head_shift = 0;
		active_groupNum = EPSON_ACTIVE_GROUPNUM;
	}
	else if(EPSON_PERMIT_GROUPNUM > EPSON_ACTIVE_GROUPNUM)
	{
		//Head-disable function is only consider YContinue group.
		active_groupNum = EPSON_ACTIVE_GROUPNUM;
		if(active_groupNum % epsonCaliSetting.printerSetting.GroupCount_YInterleave != 0)
		{
			//something is wrong.
			active_groupNum /= epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			if(active_groupNum == 0)
				active_groupNum ++;
			active_groupNum *= epsonCaliSetting.printerSetting.GroupCount_YInterleave;
		}
		
		INT8U permit_groupNum = EPSON_PERMIT_GROUPNUM/epsonCaliSetting.printerSetting.GroupCount_YInterleave;
		active_groupNum /= epsonCaliSetting.printerSetting.GroupCount_YInterleave;
		
		if( permit_groupNum == 2)
		{
			if( ( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0x2) &&
			   permit_groupNum != active_groupNum)
			{
				head_shift = head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			}
		}
#if  defined(HEAD_RICOH_G4)
		else if(permit_groupNum == 3)
		{
			if( ( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0x2 ) ||
			   ( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0x6 ))
			{
				head_shift = head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			}
			else if( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0x4 )
			{
				head_shift = 2 * head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			}
		}
#endif
		else if(permit_groupNum == 4)
		{
			if( ( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0x2 ) ||
			   ( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0x6 ) ||
				   ( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0xE ))
			{
				head_shift = head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			}
			else if( ( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0x4 ) ||
					( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0xC ))
			{
				head_shift = 2 * head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			}
			else if( (LCDMenuConfig.Active_HeadMask & ((1<<permit_groupNum)-1)) == 0x8 )
			{
				head_shift = 3 * head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			}
		}
		active_groupNum *= epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	}
	
	if( epsonCaliSetting.printerSetting.bIntegratedHead && (head_group * EPSON_PERMIT_GROUPNUM == 4))
	{
		for(i=0; i<head_group * active_groupNum; i++)
		{
			for(j=0; j<head_group * EPSON_PERMIT_GROUPNUM; j++)
			{
				if(posi_head_map[j] == i+head_shift)
				{
					index_shift = j;
					break;
				}
			}
#ifdef EPSON_4H
			index_shift = EPSON_4H_Head_HardMap[index_shift];
#endif
			for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
			{
#if defined( HEAD_EPSON_GEN7)||defined( HEAD_EPSON_GEN5S)
				IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = index_shift * NOZZLE_LINE_COUNT_PER_HEAD + EPSON_GEN5_TO_GEN7[j];
#else                
				IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = index_shift * NOZZLE_LINE_COUNT_PER_HEAD + j;
#endif
			}
		}
	}
	else
	{
		for(i=0; i<head_group * active_groupNum; i++)
		{
			index_shift = i+head_shift;
#ifdef HEAD_2_SWAP
			if(head_group * active_groupNum == 1)		
			{
				if( (LCDMenuConfig.Active_HeadMask & ((1<<EPSON_PERMIT_GROUPNUM)-1)) == 0x2)
					index_shift = 0;
				else
					index_shift = 1;
			}
			else
			{
				if(i==0)
					index_shift = 1;
				else if(i==1)
					index_shift = 0;
			}
#endif                    
			for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
			{
#ifdef TEST_DOUBLE_CHANNEL                
#if defined( RICOH_3H) || defined(RICOH_G5_3H) || defined(RICOH_G5_4H)
#else 
				if(j==0)
					IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = (index_shift + 4) * NOZZLE_LINE_COUNT_PER_HEAD + j;
				else
					IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = index_shift * NOZZLE_LINE_COUNT_PER_HEAD + j;
#endif
#else
#if defined( HEAD_EPSON_GEN7)||defined( HEAD_EPSON_GEN5S)
				IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = index_shift * NOZZLE_LINE_COUNT_PER_HEAD + EPSON_GEN5_TO_GEN7[j];
#else
				IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = index_shift * NOZZLE_LINE_COUNT_PER_HEAD + j;
#endif
#endif
				
#ifdef HEAD_RICOH_G4
#if defined( RICOH_3H) || defined(RICOH_G5_3H)
				IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = Datamap_to_Hardmap_3H[IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j]];
#elif defined(RICOH_G5_4H)
				if(COLOR_NUMBER_CALIBRATION > 4)
					IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = Datamap_to_Hardmap_G5_4H_8C[IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j]];
				else
					IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = Datamap_to_Hardmap_G5_4H_4C[IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j]];
#else
				IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j] = Datamap_to_Hardmap[IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j]];
#endif
#endif
			}
		}
	}
}

//only for safe
INT8U SetDefaultCalibrationConfig()
{
	INT8U i,j,k;
	
	epsonCaliSetting.printerSetting.PrintRegionWidth = (INT32U)(((float)18 *10) / 2.54 * X_BASE_RES);
#if defined( HEAD_EPSON_GEN5)
	epsonCaliSetting.printerSetting.bIntegratedHead = True;
	epsonCaliSetting.printerSetting.HeadCount = 1;
	epsonCaliSetting.printerSetting.head_group = 1;
#else //defined(HEAD_RICOH_G4)
	epsonCaliSetting.printerSetting.bIntegratedHead = False;
	epsonCaliSetting.printerSetting.HeadCount = 4;
	epsonCaliSetting.printerSetting.head_group = 4;
#endif
	
	epsonCaliSetting.printerSetting.GroupCount = 1;
	epsonCaliSetting.printerSetting.GroupCount_YContinue = 1;
	epsonCaliSetting.printerSetting.GroupCount_YInterleave = 1;
	epsonCaliSetting.printerSetting.ColorCount = 4;
	epsonCaliSetting.printerSetting.bOriginIsRight = True;
	epsonCaliSetting.printerSetting.bHeadDirIsPositive = True;
	epsonCaliSetting.printerSetting.bHeadElectricDir = False;
	
	epsonCaliSetting.printerSetting.DPI_X = X_BASE_RES;
	
	epsonCaliSetting.printerSetting.YInterleaveNum_Head = INTERLEAVE_CALIBRATION;
#if defined( HEAD_EPSON_GEN5)
	epsonCaliSetting.printerSetting.YInterleaveNum = NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.ColorCount;
#else   //defined(HEAD_RICOH_G4)
	epsonCaliSetting.printerSetting.YInterleaveNum = NOZZLE_LINE_COUNT_PER_HEAD;
#endif
	if(epsonCaliSetting.printerSetting.YInterleaveNum > epsonCaliSetting.printerSetting.YInterleaveNum_Head) 
		epsonCaliSetting.printerSetting.YInterleaveNum_InHead = epsonCaliSetting.printerSetting.YInterleaveNum_Head;
	else
		epsonCaliSetting.printerSetting.YInterleaveNum_InHead = epsonCaliSetting.printerSetting.YInterleaveNum;
	
	if(factoryData.HeadType == HeadNo_Epson_Gen5)
	{
		if( epsonCaliSetting.printerSetting.HeadCount == 1)
			epsonCaliSetting.printerSetting.BusWidth = 8;
		else if(epsonCaliSetting.printerSetting.HeadCount == 2)
			epsonCaliSetting.printerSetting.BusWidth = 16;
		else
			epsonCaliSetting.printerSetting.BusWidth = 32;
	}
	else
	{
		if( GetPhysicsHeadNum() * epsonCaliSetting.printerSetting.YInterleaveNum_Head <= 8)
			epsonCaliSetting.printerSetting.BusWidth = 8;
		else if(GetPhysicsHeadNum() * epsonCaliSetting.printerSetting.YInterleaveNum_Head <= 16)
			epsonCaliSetting.printerSetting.BusWidth = 16;
		else
			epsonCaliSetting.printerSetting.BusWidth = 32;
	}
	epsonCaliSetting.printerSetting.DPI_Y = epsonCaliSetting.printerSetting.YInterleaveNum * NOZZLE_DPI;
	
	epsonCaliSetting.printerSetting.NozzleCount = NOZZLE_COUNT_PER_LINE;
	epsonCaliSetting.printerSetting.BytesPerFire = EPSON_DATA_PER_FIRE;
	
	epsonCaliSetting.printerSetting.verticalNozzleShift = 0;
	epsonCaliSetting.printerSetting.bVerticalValid = False;
	epsonCaliSetting.printerSetting.bYIntervalReplaceVertical = False;
	
	epsonCaliSetting.printerSetting.BaseColor_ColorCode = COLORCODE_K;
	epsonCaliSetting.printerSetting.BaseColor_Index = 0;
	
#if defined( HEAD_EPSON_GEN5)
	epsonCaliSetting.printerSetting.BaseColor_HeadIndex = epsonCaliSetting.printerSetting.GroupCount - 1;
#else   //defined(HEAD_RICOH_G4)
	epsonCaliSetting.printerSetting.BaseColor_HeadIndex =
		(epsonCaliSetting.printerSetting.GroupCount - 1) * epsonCaliSetting.printerSetting.ColorCount + 0;
#endif
	
	for(i=0; i<MAX_INTERLEAVE_CALIBRATION; i++)
		epsonCaliSetting.printerSetting.BaseColor_LineIndex[i] = i;
	
#if defined( HEAD_EPSON_GEN5)
	for(i=0; i<epsonCaliSetting.printerSetting.ColorCount; i++)
	{
		epsonCaliSetting.colorMap.BaseHeadNo[i] = 0;
		epsonCaliSetting.colorMap.BaseLineNo[i] = i*epsonCaliSetting.printerSetting.YInterleaveNum;
	}
#elif defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
	for(i=0; i<epsonCaliSetting.printerSetting.ColorCount; i++)
	{
		epsonCaliSetting.colorMap.BaseHeadNo[i] = i/2;
		epsonCaliSetting.colorMap.BaseLineNo[i] = i%2*2;
	}
#else   //defined(HEAD_RICOH_G4)
	for(i=0; i<epsonCaliSetting.printerSetting.ColorCount; i++)
	{
		epsonCaliSetting.colorMap.BaseHeadNo[i] = i;
		epsonCaliSetting.colorMap.BaseLineNo[i] = 0;
	}
#endif
	
	for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
	{
#if defined( HEAD_EPSON_GEN5)
		epsonCaliSetting.colorMap.ColorCode[i][0] = COLORCODE_Y;
		epsonCaliSetting.colorMap.ColorCode[i][1] = COLORCODE_Y;
		epsonCaliSetting.colorMap.ColorCode[i][2] = COLORCODE_M;
		epsonCaliSetting.colorMap.ColorCode[i][3] = COLORCODE_M;
		epsonCaliSetting.colorMap.ColorCode[i][4] = COLORCODE_C;
		epsonCaliSetting.colorMap.ColorCode[i][5] = COLORCODE_C;
		epsonCaliSetting.colorMap.ColorCode[i][6] = COLORCODE_K;
		epsonCaliSetting.colorMap.ColorCode[i][7] = COLORCODE_K;
		epsonCaliSetting.colorMap.ColorIndex[i][0] = epsonCaliSetting.colorMap.ColorIndex[i][1] = 0;
		epsonCaliSetting.colorMap.ColorIndex[i][2] = epsonCaliSetting.colorMap.ColorIndex[i][3] = 1;
		epsonCaliSetting.colorMap.ColorIndex[i][4] = epsonCaliSetting.colorMap.ColorIndex[i][5] = 2;
		epsonCaliSetting.colorMap.ColorIndex[i][6] = epsonCaliSetting.colorMap.ColorIndex[i][7] = 3;
#elif defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
		j= i%epsonCaliSetting.printerSetting.ColorCount;
		switch (j)
		{
		case 0:
			epsonCaliSetting.colorMap.ColorCode[i][0] = COLORCODE_K;
			epsonCaliSetting.colorMap.ColorCode[i][1] = COLORCODE_K;
			epsonCaliSetting.colorMap.ColorIndex[i][0] = epsonCaliSetting.colorMap.ColorIndex[i][1] = 0;
			epsonCaliSetting.colorMap.ColorCode[i][2] = COLORCODE_C;
			epsonCaliSetting.colorMap.ColorCode[i][3] = COLORCODE_C;
			epsonCaliSetting.colorMap.ColorIndex[i][2] = epsonCaliSetting.colorMap.ColorIndex[i][3] = 1;
			break;
		case 1:
			epsonCaliSetting.colorMap.ColorCode[i][0] = COLORCODE_M;
			epsonCaliSetting.colorMap.ColorCode[i][1] = COLORCODE_M;
			epsonCaliSetting.colorMap.ColorIndex[i][0] = epsonCaliSetting.colorMap.ColorIndex[i][1] = 2;
			epsonCaliSetting.colorMap.ColorCode[i][2] = COLORCODE_Y;
			epsonCaliSetting.colorMap.ColorCode[i][3] = COLORCODE_Y;
			epsonCaliSetting.colorMap.ColorIndex[i][2] = epsonCaliSetting.colorMap.ColorIndex[i][3] = 3;
			break;
		case 2:
			epsonCaliSetting.colorMap.ColorCode[i][0] = COLORCODE_Lc;
			epsonCaliSetting.colorMap.ColorCode[i][1] = COLORCODE_Lc;
			epsonCaliSetting.colorMap.ColorIndex[i][0] = epsonCaliSetting.colorMap.ColorIndex[i][1] = 4;
			epsonCaliSetting.colorMap.ColorCode[i][2] = COLORCODE_Lm;
			epsonCaliSetting.colorMap.ColorCode[i][3] = COLORCODE_Lm;
			epsonCaliSetting.colorMap.ColorIndex[i][2] = epsonCaliSetting.colorMap.ColorIndex[i][3] = 5;
			break;
		}
#else   //defined(HEAD_RICOH_G4)
		j= i%epsonCaliSetting.printerSetting.ColorCount;
		switch (j)
		{
		case 0:
			epsonCaliSetting.colorMap.ColorCode[i][0] = COLORCODE_K;
			epsonCaliSetting.colorMap.ColorCode[i][1] = COLORCODE_K;
			epsonCaliSetting.colorMap.ColorIndex[i][0] = epsonCaliSetting.colorMap.ColorIndex[i][1] = 0;
			break;
		case 1:
			epsonCaliSetting.colorMap.ColorCode[i][0] = COLORCODE_C;
			epsonCaliSetting.colorMap.ColorCode[i][1] = COLORCODE_C;
			epsonCaliSetting.colorMap.ColorIndex[i][0] = epsonCaliSetting.colorMap.ColorIndex[i][1] = 1;
			break;
		case 2:
			epsonCaliSetting.colorMap.ColorCode[i][0] = COLORCODE_M;
			epsonCaliSetting.colorMap.ColorCode[i][1] = COLORCODE_M;
			epsonCaliSetting.colorMap.ColorIndex[i][0] = epsonCaliSetting.colorMap.ColorIndex[i][1] = 2;
			break;
		case 3:
			epsonCaliSetting.colorMap.ColorCode[i][0] = COLORCODE_Y;
			epsonCaliSetting.colorMap.ColorCode[i][1] = COLORCODE_Y;
			epsonCaliSetting.colorMap.ColorIndex[i][0] = epsonCaliSetting.colorMap.ColorIndex[i][1] = 3;
			break;
		}
#endif
	}
	
#if defined( HEAD_EPSON_GEN5)
	epsonCaliSetting.printerSetting.YStepColor_Index[0] = 0;
	epsonCaliSetting.printerSetting.YStepColor_Index[1] = 4;
	
	epsonCaliSetting.printerSetting.VerticalColor_Index[0] = 0;
	epsonCaliSetting.printerSetting.VerticalColor_Index[1] = 2;
	epsonCaliSetting.printerSetting.VerticalColor_Index[2] = 4;
	epsonCaliSetting.printerSetting.VerticalColor_Index[3] = 6;
	
	for(i=0; i<NOZZLE_LINE_COUNT_PER_HEAD; i++)
	{
#ifdef HEAD_EPSON_GEN5S   
		epsonCaliSetting.singleSetting.XOffset[i] = (float)(0 + (i/2)*(186+64) + (i%2)*64)/720;
#else
		epsonCaliSetting.singleSetting.XOffset[i] = (float)(0 + (i/2)*(216+64) + (i%2)*64)/720;
#endif
		epsonCaliSetting.singleSetting.YOffset[i] = (float)(0 + (i%2)*1)/(NOZZLE_DPI*INTERLEAVE_CALIBRATION);
	}
#else
	epsonCaliSetting.printerSetting.YStepColor_Index[0] = 0;
	epsonCaliSetting.printerSetting.YStepColor_Index[1] = 2;
	
	epsonCaliSetting.printerSetting.VerticalColor_Index[0] = 0;
	epsonCaliSetting.printerSetting.VerticalColor_Index[1] = 1;
	epsonCaliSetting.printerSetting.VerticalColor_Index[2] = 2;
	epsonCaliSetting.printerSetting.VerticalColor_Index[3] = 3;
	
	for(i=0; i<NOZZLE_LINE_COUNT_PER_HEAD; i++)
	{
		epsonCaliSetting.singleSetting.XOffset[i] = (float)(0 + (i/2)*(216+64) + (i%2)*64)/720;
		epsonCaliSetting.singleSetting.YOffset[i] = (float)(0 + (i%2)*1)/(NOZZLE_DPI*INTERLEAVE_CALIBRATION);
	}
#endif
	
	for(i=0; i<epsonCaliSetting.printerSetting.GroupCount; i++)
	{
#if defined( HEAD_EPSON_GEN5)
		epsonCaliSetting.groupSetting.XOffset[i] = i * 4;
#else
		epsonCaliSetting.groupSetting.XOffset[i] = i * 1;
#endif
		epsonCaliSetting.groupSetting.YOffset[i] = i * (float)NOZZLE_COUNT_PER_LINE / NOZZLE_DPI;
	}
	
	memset(&epsonCaliSetting.eprCaliSetting, 0,  sizeof(struct EPR_EPSON_CaliSetting));
	
	epsonCaliSetting.eprCaliSetting.len = sizeof(struct EPR_EPSON_CaliSetting);
	epsonCaliSetting.eprCaliSetting.version = EPR_EPSON_CALI_SETTING_VERSION;
	
#if defined( HEAD_EPSON_GEN5)
	for(k=0; k<MAX_HEIGHT_LEVEL; k++)
	{
		for(i=0; i<MAX_X_RES_NUM; i++)
		{
			for(j=0; j<MAX_VSDMODEL_NUM; j++)
			{
				switch (i)
				{
				case 0:
				default:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = X_BASE_RES;
#if !defined(EPSON_4H)										
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = X_BASE_RES;
#endif
					break;
				case 1:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = X_BASE_RES/2;
#if !defined(EPSON_4H)										
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = X_BASE_RES/2;
#endif
					break;
				case 2:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = X_BASE_RES/4*3;
#if !defined(EPSON_4H)										
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = X_BASE_RES/4*3;
#endif
					break;
				case 3:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = X_BASE_RES/4*3/2;
#if !defined(EPSON_4H)	
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = X_BASE_RES/4*3/2;
#endif
					break;
				case 4:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = X_BASE_RES * 2;
#if !defined(EPSON_4H)	
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = X_BASE_RES * 2;
#endif
					break;
				}
			}
		}
	}
#endif
	for(i=0; i< MAX_MEDIA_TYPE_CALIBRATION; i++)
		epsonCaliSetting.eprCaliSetting.stepPass[i].BaseStep = 9660; //2362719; //10000;
	epsonCaliSetting.eprCaliSetting.yOffset.nDPI = NOZZLE_DPI * INTERLEAVE_CALIBRATION;
	
	return True;
}

INT8U InitCalibrationConfig()
{
	INT8U i,j;
	INT8U head_group;
	INT8U head_group_pure_color;
	INT8U head_group_white;
	INT8U head_group_coat;
	
	if (valueFlag_FactoryData_Not_Init ||
		(factoryData.HeadType != HeadNo_Epson_Gen5 &&
		 factoryData.HeadType != HeadNo_Ricoh_Gen4_64Pin &&
			 factoryData.HeadType != HeadNo_Ricoh_Gen4_G4 &&
				 factoryData.HeadType != HeadNo_Ricoh_Gen4_G4L &&
					 factoryData.HeadType != HeadNo_Ricoh_Gen5_2C_100Pin 
						 ))
		return False;
	
	//factoryDataEx.LayoutType bit0 == 0, 在X方向的坐标，先Ycontinue，再Yinterleave。
	//factoryDataEx.LayoutType bit0 == 1, 在X方向的坐标，先Yinterleave，再Ycontinue。
	//  内部喷头编号总是，先Ycontinue，再Yinterleave。
	//factoryDataEx.LayoutType bit1 == 1, 对称色序。Y interleave 必须是2的倍数。2012-7-2: still not support fully.
	//factoryDataEx.LayoutType bit2，对称色序的对称方式。当允许多种对称方式的时候（如，EPSON Y 拼插成720DPI)，
	//  0，以最外层为对称单位（在不同的头上颜色是对称的）。1，以最内层为对称单位（在一个头内部颜色是对称的）。
	//  方式0，在消除因覆盖关系导致的色差，效果应该更好。方式1，更容易拼插，两个颜色之间的距离比较窄。
	//factoryDataEx.LayoutType bit3 == 0, Ycontinue group 在X方向是回绕。通常，回绕更节省空间。
	//  例如，EPSON4H/4C，在X向位置只有两个位置，13在一个位置，24在一个位置。34的位置回绕了。
	if((factoryDataEx.LayoutType & 0x80) != 0 
	   // || (factoryDataEx.LayoutType & 0x6) != 0 //currently, NOT support 对称色序。
	   )
		return False;
	
	//memset(&epsonCaliSetting, 0,  sizeof(epsonCaliSetting));
	memset(&epsonCaliSetting, 0,  sizeof(epsonCaliSetting)-sizeof(epsonCaliSetting.eprCaliSetting));
	
	epsonCaliSetting.printerSetting.DPI_X = factoryDataEx.m_nXEncoderDPI;
	
	epsonCaliSetting.printerSetting.PrintRegionWidth = (INT32U)((LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX) * epsonCaliSetting.printerSetting.DPI_X);
	if(factoryData.HeadType == HeadNo_Epson_Gen5)
		epsonCaliSetting.printerSetting.bIntegratedHead = True;
	else
		epsonCaliSetting.printerSetting.bIntegratedHead = False;
	
	epsonCaliSetting.printerSetting.YInterleaveNum_Head = INTERLEAVE_CALIBRATION;
	epsonCaliSetting.printerSetting.YInterleaveNum = factoryDataEx.YInterleaveNum;
	if( epsonCaliSetting.printerSetting.YInterleaveNum > epsonCaliSetting.printerSetting.YInterleaveNum_Head) 
		epsonCaliSetting.printerSetting.YInterleaveNum_InHead = epsonCaliSetting.printerSetting.YInterleaveNum_Head;
	else
		epsonCaliSetting.printerSetting.YInterleaveNum_InHead = epsonCaliSetting.printerSetting.YInterleaveNum;
	
	head_group = (COLOR_NUMBER_CALIBRATION * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
				  NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	head_group_pure_color = (factoryData.color_num * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
							 NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	//here, we assert white and vanish is use 
	head_group_white  = (factoryData.spot_color1_headnum * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
						 NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	if(head_group_white ==0 && factoryData.spot_color1_headnum  > 0)
		head_group_white = 1;
	head_group_coat  = (factoryData.spot_color2_headnum * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
						NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	if(head_group_coat ==0 && factoryData.spot_color2_headnum  > 0)
		head_group_coat = 1;
	
	epsonCaliSetting.printerSetting.head_group = head_group;
	epsonCaliSetting.printerSetting.HeadCount = EPSON_ACTIVE_GROUPNUM *head_group;
	epsonCaliSetting.printerSetting.GroupCount = EPSON_ACTIVE_GROUPNUM;
	epsonCaliSetting.printerSetting.GroupCount_YInterleave = 
		(epsonCaliSetting.printerSetting.YInterleaveNum + epsonCaliSetting.printerSetting.YInterleaveNum_InHead - 1)/
			epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
	epsonCaliSetting.printerSetting.YInterleaveNum = epsonCaliSetting.printerSetting.YInterleaveNum_InHead * 
		epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	epsonCaliSetting.printerSetting.GroupCount_YContinue = epsonCaliSetting.printerSetting.GroupCount/
		epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	
	epsonCaliSetting.printerSetting.ColorCount = COLOR_NUMBER_CALIBRATION;
	
	if(factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L)
		epsonCaliSetting.printerSetting.DPI_Y = NOZZLE_DPI_G4L * epsonCaliSetting.printerSetting.YInterleaveNum;
	else
		epsonCaliSetting.printerSetting.DPI_Y = NOZZLE_DPI * epsonCaliSetting.printerSetting.YInterleaveNum;
	
	if((EPSON_PERMIT_GROUPNUM *head_group) * NOZZLE_LINE_COUNT_PER_HEAD <= 8)
		epsonCaliSetting.printerSetting.BusWidth = 8;
	else if((EPSON_PERMIT_GROUPNUM *head_group) * NOZZLE_LINE_COUNT_PER_HEAD <= 16)
		epsonCaliSetting.printerSetting.BusWidth = 16;
	else
		epsonCaliSetting.printerSetting.BusWidth = 32;
	
	if(factoryData.m_nBitFlag & 0x1)
		epsonCaliSetting.printerSetting.bOriginIsRight = True;
	else
		epsonCaliSetting.printerSetting.bOriginIsRight = False;
	if(factoryDataEx.m_nBitFlagEx & 0x1)
		epsonCaliSetting.printerSetting.bHeadDirIsPositive = True;
	else
		epsonCaliSetting.printerSetting.bHeadDirIsPositive = False;
	if(factoryData.HeadType == HeadNo_Epson_Gen5)
	{
		if(epsonCaliSetting.printerSetting.bHeadDirIsPositive)
			epsonCaliSetting.printerSetting.bHeadElectricDir = False;
		else
			epsonCaliSetting.printerSetting.bHeadElectricDir = True;
	}
	else
	{
		if(epsonCaliSetting.printerSetting.bHeadDirIsPositive)
			epsonCaliSetting.printerSetting.bHeadElectricDir = False;
		else
			epsonCaliSetting.printerSetting.bHeadElectricDir = True;
	}
	
	GetColorMap(); //fill epsonCaliSetting.colorMap.ColorCode.
	
	epsonCaliSetting.printerSetting.BaseColor_ColorCode = caliConfig.BaseColor;
	for(i=0; i<epsonCaliSetting.printerSetting.ColorCount; i++)
	{
		if( caliConfig.BaseColor == factoryDataEx.m_nColorOrder[i])
		{
			epsonCaliSetting.printerSetting.BaseColor_Index = i;
			break;
		}
	}
	epsonCaliSetting.printerSetting.BaseColor_HeadIndex =
		GetHeadIndexWithInterleave(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1,
								   epsonCaliSetting.printerSetting.BaseColor_ColorCode, 0);
	
	for(i= 0; i<epsonCaliSetting.printerSetting.YInterleaveNum_InHead; i++)
	{
		epsonCaliSetting.printerSetting.BaseColor_LineIndex[i] = 
			GetNozzleLineIndex(caliConfig.BaseColor, i * epsonCaliSetting.printerSetting.GroupCount_YInterleave);
	}
	
	for(i=0; i<STEPCHECK_COLORNUM && i<2; i++)
		epsonCaliSetting.printerSetting.YStepColor_Index[i] = GetNozzleLineIndex(caliConfig.YStepColor[i], 0) + 
			GetHeadIndexWithInterleave(0, caliConfig.YStepColor[i], 0) * NOZZLE_LINE_COUNT_PER_HEAD;
	
	for(i=0; i<epsonCaliSetting.printerSetting.ColorCount; i++)
	{
		epsonCaliSetting.printerSetting.VerticalColor_Index[i] =
			GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[i], 0) * NOZZLE_LINE_COUNT_PER_HEAD +
				GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], 0);
	}
	
	epsonCaliSetting.printerSetting.NozzleCount = NOZZLE_COUNT_PER_LINE;
	epsonCaliSetting.printerSetting.BytesPerFire = EPSON_DATA_PER_FIRE;
	
	for(i=0; i<epsonCaliSetting.printerSetting.ColorCount; i++)
	{
		epsonCaliSetting.colorMap.BaseHeadNo[i] = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[i],0);
		epsonCaliSetting.colorMap.BaseLineNo[i] = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], 0);
	}
	
	for(i=0; i<NOZZLE_LINE_COUNT_PER_HEAD; i++)
	{
#if defined( HEAD_EPSON_GEN5)
#ifdef HEAD_EPSON_GEN5S   
		epsonCaliSetting.singleSetting.XOffset[i] = (float)(0 + (i/2)*(186+64) + (i%2)*64)/720;
#else
		epsonCaliSetting.singleSetting.XOffset[i] = (float)(0 + (i/2)*(216+64) + (i%2)*64)/720;
#endif
		epsonCaliSetting.singleSetting.YOffset[i] = (float)(0 + (i%2)*1)/(NOZZLE_DPI*INTERLEAVE_CALIBRATION);
#else   //defined(HEAD_RICOH_G4)
		switch(factoryData.HeadType)
		{
		case HeadNo_Ricoh_Gen4_G4:
			epsonCaliSetting.singleSetting.XOffset[i] = (float)(0.5292/25.4) * i;
			epsonCaliSetting.singleSetting.YOffset[i] = (float)(0 + (i%2)*1)/(NOZZLE_DPI*INTERLEAVE_CALIBRATION);
			break;
		case HeadNo_Ricoh_Gen4_G4L:
			epsonCaliSetting.singleSetting.XOffset[i] = (float)(0.9948/25.4) * i;
			epsonCaliSetting.singleSetting.YOffset[i] = (float)(0 + (i%2)*1)/(NOZZLE_DPI_G4L*INTERLEAVE_CALIBRATION);
			break;
		case HeadNo_Ricoh_Gen4_64Pin: //need testing.
			epsonCaliSetting.singleSetting.XOffset[i] = (float)(0.85/25.4) * i;
			epsonCaliSetting.singleSetting.YOffset[i] = (float)(0 + (i%2)*1)/(NOZZLE_DPI*INTERLEAVE_CALIBRATION);
			break;
		case HeadNo_Ricoh_Gen5_2C_100Pin: 
			if(epsonCaliSetting.printerSetting.bHeadDirIsPositive)
			{  
				switch(i)
				{
				case 0:
				default:
					epsonCaliSetting.singleSetting.XOffset[i] = (float)0;
					break;
				case 1:
					epsonCaliSetting.singleSetting.XOffset[i] = (float)(11.811/25.4);
					break;
				case 2:
					epsonCaliSetting.singleSetting.XOffset[i] = (float)(0.5503/25.4);
					break;
				case 3:
					epsonCaliSetting.singleSetting.XOffset[i] = (float)(12.3613/25.4);
					break;
				}
			}
			else
			{
				switch(i)
				{
				case 0:
					
					epsonCaliSetting.singleSetting.XOffset[i] = (float)(12.3613/25.4);
					
					break;
				case 1:
					epsonCaliSetting.singleSetting.XOffset[i] = (float)(0.5503/25.4);
					
					break;
				case 2:
					epsonCaliSetting.singleSetting.XOffset[i] = (float)(11.811/25.4);
					break;
				case 3:
				default:
					epsonCaliSetting.singleSetting.XOffset[i] = (float)0;
					break;
				}
			}  
			epsonCaliSetting.singleSetting.YOffset[i] = (float)(0 + (i%4)*1)/(NOZZLE_DPI*INTERLEAVE_CALIBRATION);
			break;
		}
#endif
	}
	
#if defined( HEAD_EPSON_GEN5)
	for(i=0; i<epsonCaliSetting.printerSetting.GroupCount; i++)
	{
		INT8U groupNO_YInterleave = i / epsonCaliSetting.printerSetting.GroupCount_YContinue;
		INT8U groupNo_YContinue = i % epsonCaliSetting.printerSetting.GroupCount_YContinue;
		
		for(j=0; j<head_group; j++)
		{
			if( epsonCaliSetting.printerSetting.GroupCount == 1 && 
			   epsonCaliSetting.printerSetting.ColorCount == 8 &&
				   epsonCaliSetting.printerSetting.YInterleaveNum == 2 && 
					   factoryDataEx.MaxGroupNumber == 1 ) //Special case: PM use m_fHeadXGroupSpace as m_fHeadXColorSpace.
			{
				epsonCaliSetting.groupSetting.XOffset[i*head_group + j] = j * factoryData.m_fHeadXGroupSpace;
			}
			else if((factoryDataEx.LayoutType & 0x8) == 0x8)
			{
				if( (factoryDataEx.LayoutType & 0x1) == 0)
				{
					epsonCaliSetting.groupSetting.XOffset[i*head_group + j] = i * factoryData.m_fHeadXGroupSpace + j * factoryData.m_fHeadXColorSpace;
				}
				else
				{
					epsonCaliSetting.groupSetting.XOffset[i*head_group + j] = (groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave + 
																			   groupNO_YInterleave) * factoryData.m_fHeadXGroupSpace;
					epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += j * factoryData.m_fHeadXColorSpace;
				}
			}
			else
			{
				groupNo_YContinue = groupNo_YContinue % 2; //for rollback.
				if( (factoryDataEx.LayoutType & 0x1) == 0)
				{
					epsonCaliSetting.groupSetting.XOffset[i*head_group + j] = (groupNO_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue + 
																			   groupNo_YContinue) * factoryData.m_fHeadXGroupSpace;
					epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += j * factoryData.m_fHeadXColorSpace;
				}
				else
				{
					epsonCaliSetting.groupSetting.XOffset[i*head_group + j] = (groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave + 
																			   groupNO_YInterleave) * factoryData.m_fHeadXGroupSpace;
					epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += j * factoryData.m_fHeadXColorSpace;
				}
			}
			epsonCaliSetting.groupSetting.YOffset[i*head_group + j] = groupNo_YContinue * ((float)NOZZLE_COUNT_PER_LINE/NOZZLE_DPI) + j * factoryData.m_fHeadYSpace;
			epsonCaliSetting.groupSetting.YOffset[i*head_group + j] += groupNO_YInterleave / (NOZZLE_DPI * epsonCaliSetting.printerSetting.YInterleaveNum);
		}
	}
	
#else   //defined(HEAD_RICOH_G4)
	for(i=0; i<epsonCaliSetting.printerSetting.GroupCount; i++)
	{
            INT8U groupNO_YInterleave = i / epsonCaliSetting.printerSetting.GroupCount_YContinue;
            INT8U groupNo_YContinue = i % epsonCaliSetting.printerSetting.GroupCount_YContinue;
            
            for(j=0; j<head_group; j++)
            {
                if(factoryDataEx.LayoutType & 0x8)
                {
                    if((factoryDataEx.LayoutType & 0x1) == 0)
                    {
                        if( j < head_group_pure_color)
                        {
                          epsonCaliSetting.groupSetting.XOffset[i*head_group + j] =
                          i * factoryData.m_fHeadXGroupSpace + j * factoryData.m_fHeadXColorSpace;
                        }
                        else if(j < head_group_pure_color+ head_group_white)
                        {
                            epsonCaliSetting.groupSetting.XOffset[i*head_group + j] =
                            i * factoryData.m_fHeadXGroupSpace + (j- head_group_pure_color)* factoryData.m_fHeadXColorSpace;
                        }
                        else
                        {
                            switch((factoryDataEx.LayoutType>>4)&0x7)
                            {
                                case 0:
                                case 1:
                                {
                                    epsonCaliSetting.groupSetting.XOffset[i*head_group + j] =
                                                          i * factoryData.m_fHeadXGroupSpace + (j- head_group_pure_color-head_group_white)* factoryData.m_fHeadXColorSpace;
                                    break;
                                }
                                case 2:
                                case 3:
                                {
                                    epsonCaliSetting.groupSetting.XOffset[i*head_group + j] =
                                        i * factoryData.m_fHeadXGroupSpace + (j- head_group_pure_color)* factoryData.m_fHeadXColorSpace;
                                    break;
                                }
                                default:
                                break;
                            }
                        }
                    }
                    else
                    {
                            epsonCaliSetting.groupSetting.XOffset[i*head_group + j] = (groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave + 
                                     groupNO_YInterleave) * factoryData.m_fHeadXGroupSpace;
                            epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += j * factoryData.m_fHeadXColorSpace;
                    }
              }
              else
              {
                    groupNo_YContinue = groupNo_YContinue % 2; //for rollback.
                    if( (factoryDataEx.LayoutType & 0x1) == 0)
                    {
                        epsonCaliSetting.groupSetting.XOffset[i*head_group + j] = (groupNO_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue + 
                                 groupNo_YContinue) * factoryData.m_fHeadXGroupSpace;

                        if( j < head_group_pure_color+head_group_white)
                        {
                            epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += j * factoryData.m_fHeadXColorSpace;
                        }
                        else
                        {
                            switch((factoryDataEx.LayoutType>>4)&0x7)
                            {
                                case 0:
                                case 1:
                                {
                                    epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += (j-head_group_white)* factoryData.m_fHeadXColorSpace;
                                    break;
                                }
                                case 2:
                                case 3:
                                {
                                    epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += j* factoryData.m_fHeadXColorSpace;
                                    break;
                                }
                                default:
                                break;
                            }
    			}
                  }
                  else
                  {

                      epsonCaliSetting.groupSetting.XOffset[i*head_group + j] = (groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave + 
                                       groupNO_YInterleave) * factoryData.m_fHeadXGroupSpace;
                      epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += j * factoryData.m_fHeadXColorSpace;
                  }
              }
#ifdef SHAIBO_WHITE_SHIFT
              //赛博ricoh8头头板的白墨间距做小了大约6mm，这个底板是临时版本，正式版本会做成一样。
              if(j>=head_group_pure_color && j<head_group_pure_color +head_group_white)
              {
                      if(factoryData.m_fHeadXColorSpace < 0)
                              epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += 5/25.4;
                      else
                              epsonCaliSetting.groupSetting.XOffset[i*head_group + j] -= 5/25.4;
              }
#endif            
#ifdef XULI_WHITE_SHIFT
              //Xuli ricoh8?????????6X2???????????????????,????????
              if(j>=head_group_pure_color && j<head_group_pure_color +head_group_white)
              {
                      if(factoryData.m_fHeadXColorSpace < 0)
                              epsonCaliSetting.groupSetting.XOffset[i*head_group + j] += factoryData.m_fHeadXColorSpace * (head_group_white + 0.5);
                      else
                              epsonCaliSetting.groupSetting.XOffset[i*head_group + j] -= factoryData.m_fHeadXColorSpace * (head_group_white - 0.5);
              }
#endif            
              float cache[4];
              short index[4] ={0};
              for(int i =0 ;i<4 ;i++)
              {
                      cache[i]= ((float)groupNo_YContinue * NOZZLE_COUNT_PER_LINE)/NOZZLE_DPI + groupNO_YInterleave / (NOZZLE_DPI * epsonCaliSetting.printerSetting.YInterleaveNum)+
                        i*factoryData.m_fHeadYSpace;
              }			
              switch(factoryData.HeadType)
              {
                case HeadNo_Ricoh_Gen5_2C_100Pin:
                case HeadNo_Ricoh_Gen4_G4:

                case HeadNo_Ricoh_Gen4_64Pin:
                    if(factoryData.spot_color1_headnum + factoryData.spot_color2_headnum == 0)
                    {
                            epsonCaliSetting.groupSetting.YOffset[i*head_group + j] =
                                    ((float)groupNo_YContinue * NOZZLE_COUNT_PER_LINE)/NOZZLE_DPI + j * factoryData.m_fHeadYSpace;
                            epsonCaliSetting.groupSetting.YOffset[i*head_group + j] +=
                                    groupNO_YInterleave / (NOZZLE_DPI * epsonCaliSetting.printerSetting.YInterleaveNum);
                    }
                    else if(factoryData.spot_color1_headnum + factoryData.spot_color2_headnum != 0)
                    {
                        switch((factoryDataEx.LayoutType>>4)&0x7)
                        {
                            case 0:
                            {
                                index[0]=1;
                                index[1]=0;
                                index[2]=2;
                                break;
                            }
                            case 1:
                            {
                                index[0]=1;
                                index[1]=2;
                                index[2]=0;
                                break;
                            }
                            case 2:
                            {
                                index[0]=1;
                                index[1]=0;
                                index[2]=0;
                                break;
                            }
                            case 3:
                            {
                                index[0]=0;
                                index[1]=1;
                                index[2]=1;
                                break;
                            }
                            default:
                            break;
                        }
                      if( j < head_group_pure_color)
                      {
                          epsonCaliSetting.groupSetting.YOffset[i*head_group + j] =cache[index[0]];
                      }
                      else if( j< head_group_pure_color + head_group_white)
                      {
                          epsonCaliSetting.groupSetting.YOffset[i*head_group + j] =cache[index[1]];

                      }
                      else
                      {
                          epsonCaliSetting.groupSetting.YOffset[i*head_group + j] =cache[index[2]];
                      }
                  }
                  else
                  {
                  //not implement...
                  }
                  break;
              case HeadNo_Ricoh_Gen4_G4L:
                      epsonCaliSetting.groupSetting.YOffset[i*head_group + j] =
                              ((float)groupNo_YContinue * NOZZLE_COUNT_PER_LINE)/NOZZLE_DPI_G4L + j * factoryData.m_fHeadYSpace;
                      epsonCaliSetting.groupSetting.YOffset[i*head_group + j] +=
                              groupNO_YInterleave / (NOZZLE_DPI_G4L * epsonCaliSetting.printerSetting.YInterleaveNum);
                      break;
              }
          }
		
	}
#endif
	
	epsonCaliSetting.printerSetting.bVerticalValid = False;
	epsonCaliSetting.printerSetting.verticalNozzleShift = 0;
	epsonCaliSetting.printerSetting.bYIntervalReplaceVertical = False;
	if(head_group > 1)
	{
		epsonCaliSetting.printerSetting.bVerticalValid = True;
		
		float space = factoryData.m_fHeadYSpace;
		if(space <0)
			space = -space;
		INT32S nozzleNum = (INT32S)(space * NOZZLE_DPI * epsonCaliSetting.printerSetting.YInterleaveNum + 0.5);
		if(factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L)
			nozzleNum = (INT32S)(space * NOZZLE_DPI_G4L * epsonCaliSetting.printerSetting.YInterleaveNum + 0.5);
		if( nozzleNum/epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.ColorCount >=
		   NOZZLE_COUNT_PER_LINE - (VERTICAL_CALI_NOZZLECOUNT * VERTICAL_CALI_NOZZLESTEP + VERTICAL_CALI_UNITCOUNT/2) )
			epsonCaliSetting.printerSetting.bVerticalValid = False;
		
		epsonCaliSetting.printerSetting.verticalNozzleShift = nozzleNum;
			if(factoryData.m_fHeadYSpace < 0)
				epsonCaliSetting.printerSetting.verticalNozzleShift = -nozzleNum;
		if((NOZZLE_COUNT_PER_LINE > nozzleNum/epsonCaliSetting.printerSetting.YInterleaveNum  - 5)
		   &&(NOZZLE_COUNT_PER_LINE < nozzleNum/epsonCaliSetting.printerSetting.YInterleaveNum + 5))
			epsonCaliSetting.printerSetting.bYIntervalReplaceVertical = True;
	}
	
	InitIndexShiftMap();
	for(i = 0; i<MAX_HEAD_NUMBER_CALIBRATION; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[i][j] = IndexShiftMap[i * NOZZLE_LINE_COUNT_PER_HEAD + j];
		}
	}
	InitDataChannelMap();
	return True;
}

INT8U SetDefaultCaliPrintParam()
{
	caliParam.VSDModel = 3;
	caliParam.DotSetting = 3;
	caliParam.DPI = X_BASE_RES;
	caliParam.StartPosX = 0;
	caliParam.MediaType = 0;
	caliParam.PassNum = 4;
	caliParam.Option = 0;
	return True;
}

INT8U GetResIndex(INT16U res)
{
	if(res == epsonCaliSetting.printerSetting.DPI_X)
		return 0;
	else if(res == epsonCaliSetting.printerSetting.DPI_X/2)
		return 1;
#if defined( HEAD_EPSON_GEN5)
#ifndef EPSON_4H
	else if(res == epsonCaliSetting.printerSetting.DPI_X/4*3)
		return 2;
	else if(res == epsonCaliSetting.printerSetting.DPI_X/4*3/2)
		return 3;
	else if(res == epsonCaliSetting.printerSetting.DPI_X * 2)
		return 4;
#endif
#elif defined(HEAD_RICOH_G4)
#ifdef DYSS_1270
	else if(res == epsonCaliSetting.printerSetting.DPI_X/4)
		return 2;
#else
	else if(res == epsonCaliSetting.printerSetting.DPI_X/3)
		return 2;
#endif
#else
#error
#endif
	else
		return 0;
}

static INT16U GetRes(INT8U resID)
{
	switch (resID)
	{
	default:
	case 0:
		return epsonCaliSetting.printerSetting.DPI_X;
	case 1:
		return epsonCaliSetting.printerSetting.DPI_X/2;
#if defined( HEAD_EPSON_GEN5)
#ifndef EPSON_4H
	case 2:
		return epsonCaliSetting.printerSetting.DPI_X/4*3;
	case 3:
		return epsonCaliSetting.printerSetting.DPI_X/4*3/2;
	case 4:
		return epsonCaliSetting.printerSetting.DPI_X * 2;
#endif
#elif defined(HEAD_RICOH_G4)
#ifdef DYSS_1270
	case 2:
		return epsonCaliSetting.printerSetting.DPI_X/4;
#else
	case 2:
		return epsonCaliSetting.printerSetting.DPI_X/3;
#endif
#else
#error
#endif
	}
}


INT8U GetVSDModelIndex(INT8U VSDModel)
{
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	return LCDMenuConfig_EX.Calc_X_Speed + (VSDModel-1)*MAX_SPEED_LEVEL_EACH_VSD;
#else
	return VSDModel-1;
#endif
}

INT8U GetColorOrderOfPMFormat(INT8U FwOrder)
{
	return FwOrder;
}

INT8U GetColorOrderOfFwFormat(INT8U PMOrder)
{
	return PMOrder;
}

#if defined( HEAD_EPSON_GEN7)
static INT8U DataChannelMap[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] = {4,2,6,0,1,7,3,5,0};
#else
static INT8U DataChannelMap[MAX_HEAD_NUMBER_CALIBRATION * NOZZLE_LINE_COUNT_PER_HEAD * NOZZLE_BIT_DEEP] = {0};
#endif
INT8U GetDataChannelMap(INT8U *pBuf,INT8U num)
{
	memcpy(pBuf, DataChannelMap, num);
	return num;
}

void InitDataChannelMap()
{
	INT8U i,j,k, colorNumber, interleaveNo, groupNumber;
	
	colorNumber = epsonCaliSetting.printerSetting.ColorCount;
	interleaveNo = epsonCaliSetting.printerSetting.YInterleaveNum;
	groupNumber = epsonCaliSetting.printerSetting.GroupCount_YContinue;
	
	for(i=0; i<interleaveNo; i++)
	{
		for(j=0; j<groupNumber; j++)
		{
			for(k=0; k<colorNumber; k++)
			{
				INT8U index = i*groupNumber*colorNumber + j*colorNumber + k;
				DataChannelMap[index] = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[
					GetHeadIndexWithInterleave(j,factoryDataEx.m_nColorOrder[k],i)]
						[GetNozzleLineIndex(factoryDataEx.m_nColorOrder[k], i)];
			}
		}
	}
}
#ifdef MANUFACTURER_DYSS
void DYSS_CHANGE_200DPI150(SCaliHorSettingType *psrc, SCaliHorSettingType *dest,INT8U num)
{
	INT8U i = 0;
	float tmp;
	for(i = 0;i < PM_MAX_HEAD_NUM;i++)
	{
		tmp = (float)((INT8S)dest->XLeftArray[i]*2)/num;
		if(tmp > 0)
			tmp+=0.5;
		else
			tmp-=0.5;
		psrc->XLeftArray[i] = tmp;
		
		tmp = (float)((INT8S)dest->XRightArray[i]*2)/num;
		if(tmp > 0)
			tmp+=0.5;
		else
			tmp-=0.5;
		psrc->XRightArray[i] = tmp;
	}
	tmp = (float)((INT8S)dest->nBidirRevise*2)/num;
	if(tmp > 0)
		tmp+=0.5;
	else
		tmp-=0.5;
	psrc->nBidirRevise = tmp;
}

#endif
INT8U ConvertCaliSettingToPMWithPMFormat(struct EPSONCaliSetting * pCaliSetting, SPMCaliSettingType * pPMCaliSetting)
{
	INT8U err;
	INT8U i,j,k,l,m,n;
	struct EPSON_Angle_X_Cali * pAngleX;
	struct EPSON_X_Cali curXOffset;
	struct EPSON_X_Cali * pXOffset = &curXOffset;
	SCaliHorSettingType * pPMHor;
	INT8U index, lineindex, headindex, lineindex_base,headindex_base,lineindex_allbase,headindex_allbase;
	const INT16U angleDPI = 720;
	const INT8U x_divide_scale = 1;
	
	OSSemPend(CalibrationParamSem, 0, &err);
	/*XXX*///IRQ_DisableIT(AT91C_ID_UDPHS);
	
	//x_divide_scale = ((caliParam.Option & 0x6) >> 1) + 1;
	memset(pPMCaliSetting, 0, sizeof(SPMCaliSettingType));
	
	pPMCaliSetting->len = sizeof(SPMCaliSettingType);
	pPMCaliSetting->version = 0x101;
	
	pPMCaliSetting->nStepPerHead = pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].BaseStep / pCaliSetting->printerSetting.GroupCount_YContinue;
	for(i=0; i<PM_MAX_PASS_NUM; i++)
	{
		if( i<MAX_PASS_NUMBER_CALIBRATION)
		{
			if(factoryDataEx.Only_Used_1head == 1)
			{
				if(i*2 < MAX_PASS_NUMBER_CALIBRATION)
					pPMCaliSetting->nPassStepArray[i] = pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].PassStep[i*2];
				else
					pPMCaliSetting->nPassStepArray[i] = pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].PassStep[3];
			}
			else			
				pPMCaliSetting->nPassStepArray[i] = pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].PassStep[i];
		}
		else
			pPMCaliSetting->nPassStepArray[i] = 0;
	}
	
	for(i=0; i<PM_MAX_HEAD_NUM; i++)
	{
		if(i < pCaliSetting->printerSetting.ColorCount)
		{
			headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[i], 0);
			lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], 0);
			
			//currently, PM use NOZZLE_DPI * pCaliSetting->printerSetting.YInterleaveNum.
			if(factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L)
				pPMCaliSetting->nVerticalArray[GetColorOrderOfPMFormat(i)] =
					pCaliSetting->eprCaliSetting.yOffset.YOffset[headindex][lineindex] *
						(NOZZLE_DPI_G4L * pCaliSetting->printerSetting.YInterleaveNum) / pCaliSetting->eprCaliSetting.yOffset.nDPI;
			else
				pPMCaliSetting->nVerticalArray[GetColorOrderOfPMFormat(i)] =
					pCaliSetting->eprCaliSetting.yOffset.YOffset[headindex][lineindex] *
						(NOZZLE_DPI * pCaliSetting->printerSetting.YInterleaveNum) / pCaliSetting->eprCaliSetting.yOffset.nDPI;
		}
		else
			pPMCaliSetting->nVerticalArray[i] = 0;
	}
	
#if defined(HEAD_EPSON_GEN5) && !defined(EPSON_4H)
	pAngleX = &(pCaliSetting->eprCaliSetting.xAngleOffset[LCDMenuConfig.Head_Height_Level][GetResIndex(angleDPI)][GetVSDModelIndex(caliParam.VSDModel)]);
	if(pCaliSetting->printerSetting.bOriginIsRight)
	{
		pPMCaliSetting->nLeftAngle = pAngleX->xPosDirOffset[0] * (angleDPI * x_divide_scale) / pAngleX->nDPI;
		pPMCaliSetting->nRightAngle = pAngleX->xNegDirOffset[0] * (angleDPI *x_divide_scale) / pAngleX->nDPI;
	}
	else
	{
		pPMCaliSetting->nLeftAngle = pAngleX->xNegDirOffset[0] * (angleDPI * x_divide_scale) / pAngleX->nDPI;
		pPMCaliSetting->nRightAngle = pAngleX->xPosDirOffset[0] * (angleDPI * x_divide_scale) / pAngleX->nDPI;
	}
#else
	pPMCaliSetting->nLeftAngle = 0;
	pPMCaliSetting->nRightAngle = 0;
#endif
	
	n= LCDMenuConfig.Head_Height_Level;
	for(i=0; i<PM_MAX_RESLIST_NUM; i++)
	{
		
		if( i >= MAX_X_RES_NUM)
			continue;		

		INT16U res = GetRes(i);
		for(j=0; j<PM_MAX_SPEED_NUM; j++)
		{
			//pXOffset = &(pCaliSetting->eprCaliSetting.xOffset[n][i][j]);
			memcpy(&curXOffset, &(pCaliSetting->eprCaliSetting.xOffset[n][i][j]), sizeof(struct EPSON_X_Cali));
			
			//FW's Order is 720,360,540,270.
			//PM's Order is 720,540,360,270.
			//for ricoh, all order is  600,300,200 or 720,360,240.
			switch(i)
			{
#if defined( HEAD_EPSON_GEN5)
			case 1:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[2][j]);
				break;
			case 2:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[1][j]);
				break;
#endif
			default:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[i][j]);
				break;
			}
			
			headindex_allbase = pCaliSetting->colorMap.BaseHeadNo[pCaliSetting->printerSetting.BaseColor_Index];
			lineindex_allbase = pCaliSetting->colorMap.BaseLineNo[pCaliSetting->printerSetting.BaseColor_Index];
			
#if 0
			for(l=0; l<pCaliSetting->printerSetting.HeadCount; l++)
			{
				for(m=0; m<NOZZLE_LINE_COUNT_PER_HEAD; m++)
				{
					curXOffset.xPosDirOffset.XOffset[l][m] +=
						(pCaliSetting->eprCaliSetting.headOffset.XOffset[j][l] - pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex_allbase])*
							pXOffset->nDPI / pCaliSetting->eprCaliSetting.headOffset.nDPI;
					curXOffset.xNegDirOffset.XOffset[l][m] +=
						(pCaliSetting->eprCaliSetting.headOffset.XOffset[j][l] - pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex_allbase])*
							pXOffset->nDPI / pCaliSetting->eprCaliSetting.headOffset.nDPI;
				}
			}
#endif
			for(m=0; m<pCaliSetting->printerSetting.ColorCount; m++)
			{
				headindex_base = pCaliSetting->colorMap.BaseHeadNo[m];
				lineindex_base = pCaliSetting->colorMap.BaseLineNo[m];
				for(l=0; l<pCaliSetting->printerSetting.GroupCount_YContinue; l++)
				{
					for(k=0; k<pCaliSetting->printerSetting.YInterleaveNum; k++)
					{
						headindex = GetHeadIndexWithInterleave(l, factoryDataEx.m_nColorOrder[m],k);
						lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[m],k);
						index = (k*pCaliSetting->printerSetting.GroupCount_YContinue+l)*pCaliSetting->printerSetting.ColorCount + GetColorOrderOfPMFormat(m);
						
						//                        if(headindex == headindex_base && lineindex == lineindex_base)
						{
							
							curXOffset.xPosDirOffset.XOffset[headindex][lineindex] +=
								(pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex] - pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex_allbase])*
									pXOffset->nDPI / pCaliSetting->eprCaliSetting.headOffset.nDPI;
							curXOffset.xNegDirOffset.XOffset[headindex][lineindex] +=
								(pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex] - pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex_allbase])*
									pXOffset->nDPI / pCaliSetting->eprCaliSetting.headOffset.nDPI;
						}
						//                        else
						//                        {
						//                            curXOffset.xPosDirOffset.XOffset[headindex][lineindex] +=
						//                                (pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex] - pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex_base])*
						//                                    pXOffset->nDPI / pCaliSetting->eprCaliSetting.headOffset.nDPI;
						//                            curXOffset.xNegDirOffset.XOffset[headindex][lineindex] +=
						//                                (pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex] - pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex_base])*
						//                                    pXOffset->nDPI / pCaliSetting->eprCaliSetting.headOffset.nDPI;
						//                        }
					}
				}
			}
			
			
			if(pCaliSetting->printerSetting.bOriginIsRight)
				pPMHor->nBidirRevise = pXOffset->xNegDirOffset.XOffset[headindex_allbase][lineindex_allbase] * (x_divide_scale*res) / pXOffset->nDPI;
			else
				pPMHor->nBidirRevise = - pXOffset->xNegDirOffset.XOffset[headindex_allbase][lineindex_allbase] * (x_divide_scale*res) / pXOffset->nDPI;
			for(m=0; m<pCaliSetting->printerSetting.ColorCount; m++)
			{
				headindex_base = pCaliSetting->colorMap.BaseHeadNo[m];
				lineindex_base = pCaliSetting->colorMap.BaseLineNo[m];
				for(l=0; l<pCaliSetting->printerSetting.GroupCount_YContinue; l++)
				{
					for(k=0; k<pCaliSetting->printerSetting.YInterleaveNum; k++)
					{
						headindex = GetHeadIndexWithInterleave(l, factoryDataEx.m_nColorOrder[m],k);
						lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[m],k);
						index = (k*pCaliSetting->printerSetting.GroupCount_YContinue+l)*pCaliSetting->printerSetting.ColorCount + GetColorOrderOfPMFormat(m);
						
						//if(headindex == headindex_base && lineindex == lineindex_base)
						{
							if(m == pCaliSetting->printerSetting.BaseColor_Index && (headindex == headindex_base && lineindex == lineindex_base))
							{
								pPMHor->XLeftArray[index] = 0;
								pPMHor->XRightArray[index] = 0;
							}
							else
							{
								if(pCaliSetting->printerSetting.bOriginIsRight)
								{
									pPMHor->XLeftArray[index] = pXOffset->xPosDirOffset.XOffset[headindex][lineindex] * (x_divide_scale*res) / pXOffset->nDPI;
									pPMHor->XRightArray[index] = (//pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
																  pXOffset->xNegDirOffset.XOffset[headindex][lineindex] - pPMHor->nBidirRevise) * (x_divide_scale*res) / pXOffset->nDPI;
								}
								else
								{
									pPMHor->XLeftArray[index] = (//pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
																 pXOffset->xNegDirOffset.XOffset[headindex][lineindex] + pPMHor->nBidirRevise) * (x_divide_scale*res) / pXOffset->nDPI;
									pPMHor->XRightArray[index] = pXOffset->xPosDirOffset.XOffset[headindex][lineindex] * (x_divide_scale*res) / pXOffset->nDPI;
								}
							}
						}
						//                        else
						//                        {
						//                            if(pCaliSetting->printerSetting.bOriginIsRight)
						//                            {
						//                                pPMHor->XLeftArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
						//                                                             pXOffset->xPosDirOffset.XOffset[headindex][lineindex]) * (x_divide_scale*res) / pXOffset->nDPI;
						//                                pPMHor->XRightArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
						//                                                              pXOffset->xNegDirOffset.XOffset[headindex][lineindex] - pPMHor->nBidirRevise) * (x_divide_scale*res) / pXOffset->nDPI;
						//                            }
						//                            else
						//                            {
						//                                pPMHor->XLeftArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
						//                                                             pXOffset->xNegDirOffset.XOffset[headindex][lineindex] + pPMHor->nBidirRevise) * (x_divide_scale*res) / pXOffset->nDPI;
						//                                pPMHor->XRightArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
						//                                                              pXOffset->xPosDirOffset.XOffset[headindex][lineindex]) * (x_divide_scale*res) / pXOffset->nDPI;
						//                            }
						//                        }
					}
				}
			}
		}
	}
	
#ifdef MANUFACTURER_DYSS
	for(j=0; j<PM_MAX_SPEED_NUM; j++)
	{
		DYSS_CHANGE_200DPI150(&(pPMCaliSetting->sCalibrationHorizonArray[2][j]),&(pPMCaliSetting->sCalibrationHorizonArray[1][j]),3);
		DYSS_CHANGE_200DPI150(&(pPMCaliSetting->sCalibrationHorizonArray[3][j]),&(pPMCaliSetting->sCalibrationHorizonArray[1][j]),4);
	}
#endif
	/*XXX*///IRQ_EnableIT(AT91C_ID_UDPHS);
	OSSemPost(CalibrationParamSem);
	
	return True;
}

INT8U ConvertCaliSettingToFWWithPMFormat(SPMCaliSettingType * pPMCaliSetting, struct EPSONCaliSetting * pCaliSetting)
{
	INT8U err;
	INT8U i,j,k,l,m,n;
	struct EPSON_Angle_X_Cali * pAngleX;
	struct EPSON_X_Cali curXOffset;
	struct EPSON_X_Cali * pXOffset = &curXOffset;
	SCaliHorSettingType * pPMHor;
	INT8U index, lineindex, headindex, lineindex_base,headindex_base,headindex_allbase;
	INT16U angleDPI = pCaliSetting->printerSetting.DPI_X;
	const INT8U x_divide_scale = 1;
	
	//    OS_CPU_SR cpu_sr;
	//    OS_ENTER_CRITICAL();
	
	OSSemPend(CalibrationParamSem, 0, &err);
	//OSSemPend(PMCaliSettingSem, 0, &err);
	/*XXX*///IRQ_DisableIT(AT91C_ID_UDPHS);
	
	if(pPMCaliSetting->version != 0x101 || pPMCaliSetting->len != sizeof(SPMCaliSettingType))
	{
		//    OS_EXIT_CRITICAL();		
		
		//    OSSemPost(PMCaliSettingSem);
		/*XXX*///IRQ_EnableIT(AT91C_ID_UDPHS);
		OSSemPost(CalibrationParamSem);
		return False;
	}
	
	//memset(&(pCaliSetting->eprCaliSetting), 0, sizeof(struct EPR_EPSON_CaliSetting)-sizeof(epsonCaliSetting.eprCaliSetting.stepPass));
	
	pCaliSetting->eprCaliSetting.len = sizeof(struct EPR_EPSON_CaliSetting);
	pCaliSetting->eprCaliSetting.version = EPR_EPSON_CALI_SETTING_VERSION;
	
	pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].BaseStep = pPMCaliSetting->nStepPerHead * pCaliSetting->printerSetting.GroupCount_YContinue;
	
	for(i=0; i<MAX_PASS_NUMBER_CALIBRATION; i++)
		pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].PassStep[i] = pPMCaliSetting->nPassStepArray[i];
	
	pCaliSetting->eprCaliSetting.yOffset.nDPI =
		pCaliSetting->printerSetting.DPI_Y * pCaliSetting->printerSetting.YInterleaveNum_Head/
			pCaliSetting->printerSetting.YInterleaveNum;
	for(i=0; i<pCaliSetting->printerSetting.ColorCount; i++)
	{
		for(j=0; j<pCaliSetting->printerSetting.YInterleaveNum; j++)
		{
			for(k=0; k<pCaliSetting->printerSetting.GroupCount_YContinue; k++)
			{
				headindex = GetHeadIndexWithInterleave(k, factoryDataEx.m_nColorOrder[i],j);
				lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], j);
				
				//currently, PM use NOZZLE_DPI * pCaliSetting->printerSetting.YInterleaveNum.
				if(factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L)
					pCaliSetting->eprCaliSetting.yOffset.YOffset[headindex][lineindex] =
						pPMCaliSetting->nVerticalArray[GetColorOrderOfPMFormat(i)] *
							pCaliSetting->eprCaliSetting.yOffset.nDPI/
								(NOZZLE_DPI_G4L * pCaliSetting->printerSetting.YInterleaveNum);
				else
					pCaliSetting->eprCaliSetting.yOffset.YOffset[headindex][lineindex] =
						pPMCaliSetting->nVerticalArray[GetColorOrderOfPMFormat(i)] *
							pCaliSetting->eprCaliSetting.yOffset.nDPI/
								(NOZZLE_DPI * pCaliSetting->printerSetting.YInterleaveNum);
			}
		}
	}
	
#if defined(HEAD_EPSON_GEN5) && !defined(EPSON_4H)
	pAngleX = &(pCaliSetting->eprCaliSetting.xAngleOffset[LCDMenuConfig.Head_Height_Level][GetResIndex(angleDPI)][GetVSDModelIndex(caliParam.VSDModel)]);
	memset(pAngleX, 0, sizeof(struct EPSON_Angle_X_Cali));
	pAngleX->nDPI = (x_divide_scale*angleDPI);
	for(k=0; k<pCaliSetting->printerSetting.HeadCount; k++)
	{
		if(pCaliSetting->printerSetting.bOriginIsRight)
		{
			pAngleX->xPosDirOffset[k] = x_divide_scale*pPMCaliSetting->nLeftAngle;
			pAngleX->xNegDirOffset[k] = x_divide_scale*pPMCaliSetting->nRightAngle;
		}
		else
		{
			pAngleX->xPosDirOffset[k] = x_divide_scale*pPMCaliSetting->nRightAngle;
			pAngleX->xNegDirOffset[k] = x_divide_scale*pPMCaliSetting->nLeftAngle;
		}
	}
#endif
	
	n= LCDMenuConfig.Head_Height_Level;
	for(i=0; i<MAX_X_RES_NUM; i++)
	{
		for(j=0; j<MAX_VSDMODEL_NUM; j++)
			memset(&(pCaliSetting->eprCaliSetting.xOffset[n][i][j]), 0, sizeof(struct EPSON_X_Cali));
	}
	for(i=0; i<PM_MAX_RESLIST_NUM; i++)
	{
		INT16U res = GetRes(i);
		for(j=0; j<PM_MAX_SPEED_NUM; j++)
		{
			//pXOffset = &(pCaliSetting->eprCaliSetting.xOffset[n][i][j]);
			memset(&curXOffset, 0, sizeof(struct EPSON_X_Cali));
			
			//pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[i][j]);
			
			//FW's Order is 720,360,540,270.
			//PM's Order is 720,540,360,270.
			//for ricoh, all order is  600,300,200
			switch(i)
			{
#if defined( HEAD_EPSON_GEN5)
			case 1:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[2][j]);
				break;
			case 2:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[1][j]);
				break;
#endif
			default:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[i][j]);
				break;
			}
			
			pXOffset->nDPI = x_divide_scale*res;
			
#if defined(HEAD_EPSON_GEN5) && defined(EPSON_4H)
			//FW's Order is 720,360.
			if( i >= 2)
				continue;
#endif
			
			headindex_allbase = pCaliSetting->colorMap.BaseHeadNo[pCaliSetting->printerSetting.BaseColor_Index];
			//lineindex_allbase = pCaliSetting->colorMap.BaseLineNo[pCaliSetting->printerSetting.BaseColor_Index];
			
			for(m=0; m<pCaliSetting->printerSetting.ColorCount; m++)
			{
				headindex_base = pCaliSetting->colorMap.BaseHeadNo[m];
				lineindex_base = pCaliSetting->colorMap.BaseLineNo[m];
				for(l=0; l<pCaliSetting->printerSetting.GroupCount_YContinue; l++)
				{
					for(k=0; k<pCaliSetting->printerSetting.YInterleaveNum; k++)
					{
						headindex = GetHeadIndexWithInterleave(l, factoryDataEx.m_nColorOrder[m],k);
						lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[m],k);
						
						if(headindex == headindex_base && lineindex == lineindex_base)
						{
							index = (k*pCaliSetting->printerSetting.GroupCount_YContinue+l)*pCaliSetting->printerSetting.ColorCount + GetColorOrderOfPMFormat(m);
							if(m == pCaliSetting->printerSetting.BaseColor_Index)
							{
								if(pCaliSetting->printerSetting.bOriginIsRight)
								{
									pXOffset->xPosDirOffset.XOffset[headindex][lineindex] = 0;
									pXOffset->xNegDirOffset.XOffset[headindex][lineindex] = x_divide_scale*pPMHor->nBidirRevise;
								}
								else
								{
									pXOffset->xPosDirOffset.XOffset[headindex][lineindex] = 0;
									pXOffset->xNegDirOffset.XOffset[headindex][lineindex] = - x_divide_scale*pPMHor->nBidirRevise;
								}
							}
							else
							{
								if(pCaliSetting->printerSetting.bOriginIsRight)
								{
									pXOffset->xPosDirOffset.XOffset[headindex][lineindex] = x_divide_scale*pPMHor->XLeftArray[index];
									pXOffset->xNegDirOffset.XOffset[headindex][lineindex] = x_divide_scale*((pPMHor->XRightArray[index] + pPMHor->nBidirRevise) - pPMHor->XLeftArray[index]);
								}
								else
								{
									pXOffset->xPosDirOffset.XOffset[headindex][lineindex] = x_divide_scale*pPMHor->XRightArray[index];
									pXOffset->xNegDirOffset.XOffset[headindex][lineindex] = x_divide_scale*(pPMHor->XLeftArray[index] - (pPMHor->XRightArray[index] + pPMHor->nBidirRevise));
								}
							}
						}
					}
				}
				
				for(l=0; l<pCaliSetting->printerSetting.GroupCount_YContinue; l++)
				{
					for(k=0; k<pCaliSetting->printerSetting.YInterleaveNum; k++)
					{
						headindex = GetHeadIndexWithInterleave(l, factoryDataEx.m_nColorOrder[m],k);
						lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[m],k);
						
						if(!(headindex == headindex_base && lineindex == lineindex_base))
						{
							index = (k*pCaliSetting->printerSetting.GroupCount_YContinue+l)*pCaliSetting->printerSetting.ColorCount+m;
							
							if(pCaliSetting->printerSetting.bOriginIsRight)
							{
								pXOffset->xPosDirOffset.XOffset[headindex][lineindex] = pPMHor->XLeftArray[index] -
									pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base];
								pXOffset->xNegDirOffset.XOffset[headindex][lineindex] = pPMHor->XRightArray[index] + pPMHor->nBidirRevise -
									pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base];
							}
							else
							{
								pXOffset->xNegDirOffset.XOffset[headindex][lineindex] = pPMHor->XLeftArray[index] - pPMHor->nBidirRevise -
									pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base];
								pXOffset->xPosDirOffset.XOffset[headindex][lineindex] = pPMHor->XRightArray[index] -
									pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base];
							}
							pXOffset->xPosDirOffset.XOffset[headindex][lineindex] *= x_divide_scale;
							pXOffset->xNegDirOffset.XOffset[headindex][lineindex] *= x_divide_scale;
						}
					}
				}
			}
			
			for(l=0; l<pCaliSetting->printerSetting.HeadCount; l++)
			{
				for(m=0; m<NOZZLE_LINE_COUNT_PER_HEAD; m++)
				{
					curXOffset.xPosDirOffset.XOffset[l][m] -=
						(pCaliSetting->eprCaliSetting.headOffset.XOffset[j][l] - pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex_allbase])*
							pXOffset->nDPI / pCaliSetting->eprCaliSetting.headOffset.nDPI;
					curXOffset.xNegDirOffset.XOffset[l][m] -=
						(pCaliSetting->eprCaliSetting.headOffset.XOffset[j][l] - pCaliSetting->eprCaliSetting.headOffset.XOffset[j][headindex_allbase])*
							pXOffset->nDPI / pCaliSetting->eprCaliSetting.headOffset.nDPI;
				}
			}
			memcpy(&(pCaliSetting->eprCaliSetting.xOffset[n][i][j]), &curXOffset, sizeof(struct EPSON_X_Cali));
		}
	}
	
	//    OS_EXIT_CRITICAL();		
	
	//    OSSemPost(PMCaliSettingSem);
	/*XXX*///IRQ_EnableIT(AT91C_ID_UDPHS);
	OSSemPost(CalibrationParamSem);
	
	return True;
}

INT8U ConvertCaliSettingToFWWithFWFormat(SPMCaliSettingType * pPMCaliSetting, struct EPSONCaliSetting * pCaliSetting)
{
	INT8U err;
	INT8U i,j,k,l,m,n;
	struct EPSON_Angle_X_Cali * pAngleX;
	struct EPSON_X_Cali * pXOffset;
	SCaliHorSettingType * pPMHor;
	INT8U index, lineindex, headindex,lineindex_base,headindex_base;
	const INT16U angleDPI = 720;
	const INT8U x_divide_scale = 1;
	
	//    OS_CPU_SR cpu_sr;
	//    OS_ENTER_CRITICAL();
	
	OSSemPend(CalibrationParamSem, 0, &err);
	//OSSemPend(PMCaliSettingSem, 0, &err);
	/*XXX*///IRQ_DisableIT(AT91C_ID_UDPHS);
	
	if(pPMCaliSetting->version != 0x101 || pPMCaliSetting->len != sizeof(SPMCaliSettingType))
	{
		//    OS_EXIT_CRITICAL();		
		
		//    OSSemPost(PMCaliSettingSem);
		/*XXX*///IRQ_EnableIT(AT91C_ID_UDPHS);
		OSSemPost(CalibrationParamSem);
		return False;
	}
	
	//memset(&(pCaliSetting->eprCaliSetting), 0, sizeof(struct EPR_EPSON_CaliSetting)-sizeof(epsonCaliSetting.eprCaliSetting.stepPass));
	
	pCaliSetting->eprCaliSetting.len = sizeof(struct EPR_EPSON_CaliSetting);
	pCaliSetting->eprCaliSetting.version = EPR_EPSON_CALI_SETTING_VERSION;
	
	pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].BaseStep = pPMCaliSetting->nStepPerHead;
	
	for(i=0; i<MAX_PASS_NUMBER_CALIBRATION; i++)
		pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].PassStep[i] = pPMCaliSetting->nPassStepArray[i];
	
	pCaliSetting->eprCaliSetting.yOffset.nDPI =
		pCaliSetting->printerSetting.DPI_Y * pCaliSetting->printerSetting.YInterleaveNum_Head/
			pCaliSetting->printerSetting.YInterleaveNum;
	for(i=0; i<pCaliSetting->printerSetting.ColorCount; i++)
	{
		for(j=0; j<pCaliSetting->printerSetting.YInterleaveNum; j++)
		{
			for(k=0; k<pCaliSetting->printerSetting.GroupCount_YContinue; k++)
			{
				headindex = GetHeadIndexWithInterleave(k, factoryDataEx.m_nColorOrder[i], j);
				lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], j);
				
				pCaliSetting->eprCaliSetting.yOffset.YOffset[headindex][lineindex] =
					pPMCaliSetting->nVerticalArray[GetColorOrderOfPMFormat(i)];
			}
		}
	}
	
#if defined(HEAD_EPSON_GEN5) && !defined(EPSON_4H)
	pAngleX = &(pCaliSetting->eprCaliSetting.xAngleOffset[LCDMenuConfig.Head_Height_Level][GetResIndex(angleDPI)][GetVSDModelIndex(caliParam.VSDModel)]);
	memset(pAngleX, 0, sizeof(struct EPSON_Angle_X_Cali));
	pAngleX->nDPI = (x_divide_scale*angleDPI);
	for(k=0; k<pCaliSetting->printerSetting.HeadCount; k++)
	{
		if(pCaliSetting->printerSetting.bOriginIsRight)
		{
			pAngleX->xPosDirOffset[k] = x_divide_scale*pPMCaliSetting->nLeftAngle;
			pAngleX->xNegDirOffset[k] = x_divide_scale*pPMCaliSetting->nRightAngle;
		}
		else
		{
			pAngleX->xPosDirOffset[k] = x_divide_scale*pPMCaliSetting->nRightAngle;
			pAngleX->xNegDirOffset[k] = x_divide_scale*pPMCaliSetting->nLeftAngle;
		}
	}
#endif
	
	n= LCDMenuConfig.Head_Height_Level;
	for(i=0; i<MAX_X_RES_NUM; i++)
	{
		for(j=0; j<MAX_VSDMODEL_NUM; j++)
			memset(&(pCaliSetting->eprCaliSetting.xOffset[n][i][j]), 0, sizeof(struct EPSON_X_Cali));
	}
	for(i=0; i<PM_MAX_RESLIST_NUM; i++)
	{
		INT16U res = GetRes(i);
		for(j=0; j<PM_MAX_SPEED_NUM; j++)
		{
			pXOffset = &(pCaliSetting->eprCaliSetting.xOffset[n][i][j]);
			
			//pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[i][j]);
			//FW's Order is 720,360,540,270.
			//PM's Order is 720,540,360,270.
			//for ricoh, all order is  600,300,200
			switch(i)
			{
#if defined( HEAD_EPSON_GEN5)
			case 1:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[2][j]);
				break;
			case 2:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[1][j]);
				break;
#endif
			default:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[i][j]);
				break;
			}
			
			pXOffset->nDPI = x_divide_scale*res;
			
#if defined(HEAD_EPSON_GEN5) && defined(EPSON_4H)
			//FW's Order is 720,360.
			if( i >= 2)
				continue;
#endif
			//headindex_allbase = pCaliSetting->colorMap.BaseHeadNo[pCaliSetting->printerSetting.BaseColor_Index];
			//lineindex_allbase = pCaliSetting->colorMap.BaseLineNo[pCaliSetting->printerSetting.BaseColor_Index];
			
			for(m=0; m<pCaliSetting->printerSetting.ColorCount; m++)
			{
				headindex_base = pCaliSetting->colorMap.BaseHeadNo[m];
				lineindex_base = pCaliSetting->colorMap.BaseLineNo[m];
				for(l=0; l<pCaliSetting->printerSetting.GroupCount_YContinue; l++)
				{
					for(k=0; k<pCaliSetting->printerSetting.YInterleaveNum; k++)
					{
						headindex = GetHeadIndexWithInterleave(l, factoryDataEx.m_nColorOrder[m],k);
						lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[m],k);
						
						index = (k*pCaliSetting->printerSetting.GroupCount_YContinue+l)*pCaliSetting->printerSetting.ColorCount + GetColorOrderOfPMFormat(m);
						
						if(pCaliSetting->printerSetting.bOriginIsRight)
						{
							pXOffset->xPosDirOffset.XOffset[headindex][lineindex] = pPMHor->XLeftArray[index] -
								pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base];
							pXOffset->xNegDirOffset.XOffset[headindex][lineindex] = pPMHor->XRightArray[index] + pPMHor->nBidirRevise -
								pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base];
						}
						else
						{
							pXOffset->xNegDirOffset.XOffset[headindex][lineindex] = pPMHor->XLeftArray[index] - pPMHor->nBidirRevise -
								pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base];
							pXOffset->xPosDirOffset.XOffset[headindex][lineindex] = pPMHor->XRightArray[index] -
								pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base];
						}
						pXOffset->xPosDirOffset.XOffset[headindex][lineindex] *= x_divide_scale;
						pXOffset->xNegDirOffset.XOffset[headindex][lineindex] *= x_divide_scale;
					}
				}
			}
		}
	}
	
	//    OS_EXIT_CRITICAL();		
	
	//    OSSemPost(PMCaliSettingSem);
	/*XXX*///IRQ_EnableIT(AT91C_ID_UDPHS);
	OSSemPost(CalibrationParamSem);
	
	return True;
}

INT8U ConvertCaliSettingToPMWithFWFormat(struct EPSONCaliSetting * pCaliSetting, SPMCaliSettingType * pPMCaliSetting)
{
	INT8U err;
	INT8U i,j,k,l,m,n;
	struct EPSON_Angle_X_Cali * pAngleX;
	struct EPSON_X_Cali * pXOffset;
	SCaliHorSettingType * pPMHor;
	INT8U index, lineindex,headindex,lineindex_base,headindex_base,lineindex_allbase,headindex_allbase;
	const INT16U angleDPI = 720;
	const INT8U x_divide_scale = 1;
	
	//    OS_CPU_SR cpu_sr;
	//    OS_ENTER_CRITICAL();
	
	OSSemPend(CalibrationParamSem, 0, &err);
	//OSSemPend(PMCaliSettingSem, 0, &err);
	/*XXX*///IRQ_DisableIT(AT91C_ID_UDPHS);
	
	//x_divide_scale = ((caliParam.Option & 0x6) >> 1) + 1;
	memset(pPMCaliSetting, 0, sizeof(SPMCaliSettingType));
	
	pPMCaliSetting->len = sizeof(SPMCaliSettingType);
	pPMCaliSetting->version = 0x101;
	
	pPMCaliSetting->nStepPerHead = pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].BaseStep;
	for(i=0; i<PM_MAX_PASS_NUM; i++)
	{
		if(i<MAX_PASS_NUMBER_CALIBRATION)
			pPMCaliSetting->nPassStepArray[i] = pCaliSetting->eprCaliSetting.stepPass[LCDMenuConfig.SheetType].PassStep[i];
		else
			pPMCaliSetting->nPassStepArray[i] = 0;
	}
	
	for(i=0; i<PM_MAX_HEAD_NUM; i++)
	{
		if(i < pCaliSetting->printerSetting.ColorCount)
		{
			headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[i], 0);
			lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], 0);
			
			pPMCaliSetting->nVerticalArray[GetColorOrderOfPMFormat(i)] =
				pCaliSetting->eprCaliSetting.yOffset.YOffset[headindex][lineindex];
		}
		else
			pPMCaliSetting->nVerticalArray[i] = 0;
	}
	
#if defined(HEAD_EPSON_GEN5) && !defined(EPSON_4H)
	pAngleX = &(pCaliSetting->eprCaliSetting.xAngleOffset[LCDMenuConfig.Head_Height_Level][GetResIndex(angleDPI)][GetVSDModelIndex(caliParam.VSDModel)]);
	if(pCaliSetting->printerSetting.bOriginIsRight)
	{
		pPMCaliSetting->nLeftAngle = pAngleX->xPosDirOffset[0] * (angleDPI * x_divide_scale) / pAngleX->nDPI;
		pPMCaliSetting->nRightAngle = pAngleX->xNegDirOffset[0] * (angleDPI *x_divide_scale) / pAngleX->nDPI;
	}
	else
	{
		pPMCaliSetting->nLeftAngle = pAngleX->xNegDirOffset[0] * (angleDPI * x_divide_scale) / pAngleX->nDPI;
		pPMCaliSetting->nRightAngle = pAngleX->xPosDirOffset[0] * (angleDPI * x_divide_scale) / pAngleX->nDPI;
	}
#else
	pPMCaliSetting->nLeftAngle = 0;
	pPMCaliSetting->nRightAngle = 0;
#endif
	
	n= LCDMenuConfig.Head_Height_Level;
	for(i=0; i<PM_MAX_RESLIST_NUM; i++)
	{

		if( i >= MAX_X_RES_NUM)
			continue;	
		
		INT16U res = GetRes(i);
		for(j=0; j<PM_MAX_SPEED_NUM; j++)
		{
			pXOffset = &(pCaliSetting->eprCaliSetting.xOffset[n][i][j]);
			//pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[i][j]);
			//FW's Order is 720,360,540,270.
			//PM's Order is 720,540,360,270.
			//for ricoh, all order is  600,300,200
			switch(i)
			{
#if defined( HEAD_EPSON_GEN5)
			case 1:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[2][j]);
				break;
			case 2:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[1][j]);
				break;
#endif
			default:
				pPMHor = &(pPMCaliSetting->sCalibrationHorizonArray[i][j]);
				break;
			}
			
			headindex_allbase = pCaliSetting->colorMap.BaseHeadNo[pCaliSetting->printerSetting.BaseColor_Index];
			lineindex_allbase = pCaliSetting->colorMap.BaseLineNo[pCaliSetting->printerSetting.BaseColor_Index];
			if(pCaliSetting->printerSetting.bOriginIsRight)
				pPMHor->nBidirRevise = pXOffset->xNegDirOffset.XOffset[headindex_allbase][lineindex_allbase] * (x_divide_scale*res) / pXOffset->nDPI;
			else
				pPMHor->nBidirRevise = - pXOffset->xNegDirOffset.XOffset[headindex_allbase][lineindex_allbase] * (x_divide_scale*res) / pXOffset->nDPI;
			for(m=0; m<pCaliSetting->printerSetting.ColorCount; m++)
			{
				headindex_base = pCaliSetting->colorMap.BaseHeadNo[m];
				lineindex_base = pCaliSetting->colorMap.BaseLineNo[m];
				for(l=0; l<pCaliSetting->printerSetting.GroupCount_YContinue; l++)
				{
					for(k=0; k<pCaliSetting->printerSetting.YInterleaveNum; k++)
					{
						headindex = GetHeadIndexWithInterleave(l, factoryDataEx.m_nColorOrder[m], k);
						lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[m],k);
						index = (k*pCaliSetting->printerSetting.GroupCount_YContinue+l)*pCaliSetting->printerSetting.ColorCount +
							GetColorOrderOfPMFormat(m);
						
						if(headindex == headindex_base && lineindex == lineindex_base)
						{
							if(m == pCaliSetting->printerSetting.BaseColor_Index)
							{
								pPMHor->XLeftArray[index] = 0;
								pPMHor->XRightArray[index] = 0;
							}
							else
							{
								if(pCaliSetting->printerSetting.bOriginIsRight)
								{
									pPMHor->XLeftArray[index] = pXOffset->xPosDirOffset.XOffset[headindex][lineindex] * (x_divide_scale*res) / pXOffset->nDPI;
									pPMHor->XRightArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
																  pXOffset->xNegDirOffset.XOffset[headindex][lineindex] - pPMHor->nBidirRevise) * (x_divide_scale*res) / pXOffset->nDPI;
								}
								else
								{
									pPMHor->XLeftArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
																 pXOffset->xNegDirOffset.XOffset[headindex][lineindex] + pPMHor->nBidirRevise) * (x_divide_scale*res) / pXOffset->nDPI;
									pPMHor->XRightArray[index] = pXOffset->xPosDirOffset.XOffset[headindex][lineindex] * (x_divide_scale*res) / pXOffset->nDPI;
								}
							}
						}
						else
						{
							if(pCaliSetting->printerSetting.bOriginIsRight)
							{
								pPMHor->XLeftArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
															 pXOffset->xPosDirOffset.XOffset[headindex][lineindex]) * (x_divide_scale*res) / pXOffset->nDPI;
								pPMHor->XRightArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
															  pXOffset->xNegDirOffset.XOffset[headindex][lineindex] - pPMHor->nBidirRevise) * (x_divide_scale*res) / pXOffset->nDPI;
							}
							else
							{
								pPMHor->XLeftArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
															 pXOffset->xNegDirOffset.XOffset[headindex][lineindex] + pPMHor->nBidirRevise) * (x_divide_scale*res) / pXOffset->nDPI;
								pPMHor->XRightArray[index] = (pXOffset->xPosDirOffset.XOffset[headindex_base][lineindex_base] +
															  pXOffset->xPosDirOffset.XOffset[headindex][lineindex]) * (x_divide_scale*res) / pXOffset->nDPI;
							}
						}
					}
				}
			}
		}
	}
	
	//    OS_EXIT_CRITICAL();		
	
	//    OSSemPost(PMCaliSettingSem);
	/*XXX*///IRQ_EnableIT(AT91C_ID_UDPHS);
	OSSemPost(CalibrationParamSem);
	
	return True;
}

INT8U InitCalibration()
{
	INT8U ret = True;
	
	//OSSemPend(CalibrationParamSem, 0, &err);
	
	if(!InitCalibrationConfig())
	{
		SetDefaultCalibrationConfig();
		ret = False;
	}
	
	SetDefaultCaliPrintParam();
	ConvertCaliSettingToPMWithPMFormat(&epsonCaliSetting, &pmCaliSettingPMFormat);
	ConvertCaliSettingToPMWithFWFormat(&epsonCaliSetting, &pmCaliSettingFWFormat);
	
	CalcRuntimeParam();
	
	//OSSemPost(CalibrationParamSem);
	
	return ret;
}

INT8U CalcRuntimeParam()
{
	INT8U i,j;
	float r,r1;
	INT32S R,R1;
	INT8U colorindex;
	INT32S PosiOrigin, NegOrigin;
	INT32S PosiTerm, NegTerm;
	INT8S base_offset, angle_offset_PosDir, angle_offset_NegDir;
	
	INT8U x_divide_scale = ((caliParam.Option & 0x6) >> 1) + 1;
	
	struct EPSON_X_Cali * pXOffset = &(epsonCaliSetting.eprCaliSetting.xOffset[LCDMenuConfig.Head_Height_Level][GetResIndex(caliParam.DPI)][GetVSDModelIndex(caliParam.VSDModel)]);
#if defined(HEAD_EPSON_GEN5) && !defined(EPSON_4H)
	struct EPSON_Angle_X_Cali * pAngleX = &(epsonCaliSetting.eprCaliSetting.xAngleOffset[LCDMenuConfig.Head_Height_Level][GetResIndex(caliParam.DPI)][GetVSDModelIndex(caliParam.VSDModel)]);
#endif
	
	for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			colorindex = epsonCaliSetting.colorMap.ColorIndex[i][j];
			R1 = (epsonCaliSetting.groupSetting.XOffset[i] +
					  epsonCaliSetting.singleSetting.XOffset[j]) * (x_divide_scale * caliParam.DPI);
			R1 += (float)epsonCaliSetting.eprCaliSetting.headOffset.XOffset[GetVSDModelIndex(caliParam.VSDModel)][i]* (x_divide_scale * caliParam.DPI)/epsonCaliSetting.eprCaliSetting.headOffset.nDPI;
			R = R1;
			base_offset = 0;
			//            base_offset = pXOffset->xPosDirOffset.XOffset[epsonCaliSetting.colorMap.BaseHeadNo[colorindex]][epsonCaliSetting.colorMap.BaseLineNo[colorindex]];
			//            if(!(i == epsonCaliSetting.colorMap.BaseHeadNo[colorindex] && j == epsonCaliSetting.colorMap.BaseLineNo[colorindex]))
			//            {
			//                r += (float)base_offset * (x_divide_scale * caliParam.DPI)/ pXOffset->nDPI;
			//                if( r >= 0 )
			//                    r += 0.5;
			//                else
			//                    r -= 0.5;
			//                r = (float)(INT32S)r;
			//            }
			R += pXOffset->xPosDirOffset.XOffset[i][j]* (x_divide_scale * caliParam.DPI)/ pXOffset->nDPI;
			epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j] = (INT32S)R;			
			R = R1;
			R += (float)base_offset * (x_divide_scale * caliParam.DPI)/ pXOffset->nDPI;
			R = (float)(INT32S)R;
			R += pXOffset -> xNegDirOffset.XOffset[i][j]* (x_divide_scale * caliParam.DPI)/ pXOffset->nDPI;
			epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j] = (INT32S)R;
		}
	}
	PosiTerm = PosiOrigin = epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[0][0];
	NegTerm = NegOrigin = epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[0][0];
	for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			if(PosiOrigin > epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j])
				PosiOrigin = epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j];
			if(PosiTerm < epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j])
				PosiTerm = epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j];
			if(NegOrigin > epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j])
				NegOrigin = epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j];
			if(NegTerm < epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j])
				NegTerm = epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j];
		}
	}
	epsonCaliSetting_runtime.xOffset.PosDirOriginOffset = PosiOrigin;
	epsonCaliSetting_runtime.xOffset.PosDirAllHeadWidth = PosiTerm - PosiOrigin;
	epsonCaliSetting_runtime.xOffset.NegDirOriginOffset = NegOrigin - PosiOrigin;
	epsonCaliSetting_runtime.xOffset.NegDirAllHeadWidth = NegTerm - NegOrigin;
	for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j] -= PosiOrigin;
			epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j] -= NegOrigin;
		}
	}
	
	//yOffset
	if(epsonCaliSetting.printerSetting.bHeadDirIsPositive)
	{
		for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
		{
			for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
			{
				r = (epsonCaliSetting.groupSetting.YOffset[i] + epsonCaliSetting.singleSetting.YOffset[j])*
					epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
				r += epsonCaliSetting.eprCaliSetting.yOffset.YOffset[i][j];
				if( r >= 0 )
					r += 0.5;
				else
					r -= 0.5;
				epsonCaliSetting_runtime.yOffset.YOffset[i][j] = (INT32S)r;
			}
		}
	}
	else
	{
		r1 = 0.0;
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			if(r1<epsonCaliSetting.singleSetting.YOffset[j])
				r1 = epsonCaliSetting.singleSetting.YOffset[j];
		}
		
		for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
		{
			for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
			{
				r = (epsonCaliSetting.groupSetting.YOffset[i] + (r1- epsonCaliSetting.singleSetting.YOffset[j]))*
					epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
				r += epsonCaliSetting.eprCaliSetting.yOffset.YOffset[i][j];
				if( r >= 0 )
					r += 0.5;
				else
					r -= 0.5;
				epsonCaliSetting_runtime.yOffset.YOffset[i][j] = (INT32S)r;
			}
		}
	}
	PosiOrigin = epsonCaliSetting_runtime.yOffset.YOffset[0][0];
	for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			if(PosiOrigin > epsonCaliSetting_runtime.yOffset.YOffset[i][j])
				PosiOrigin = epsonCaliSetting_runtime.yOffset.YOffset[i][j];
		}
	}
	for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			epsonCaliSetting_runtime.yOffset.YOffset[i][j] -= PosiOrigin;
		}
	}
	
	//angle.
	memset(&(epsonCaliSetting_runtime.angleXOffset), 0, sizeof(struct EPSON_Angle_X_Cali_Runtime));
	epsonCaliSetting_runtime.angleXOffset.bEnable = False;
#if defined(HEAD_EPSON_GEN5) && !defined(EPSON_4H)
	if((caliParam.Option & 0x8))
	{
		for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
		{
			if(pAngleX->xPosDirOffset[i] != 0)
			{
				epsonCaliSetting_runtime.angleXOffset.bEnable = True;
				break;
			}
		}
		
		if(epsonCaliSetting_runtime.angleXOffset.bEnable)
		{
			PosiOrigin = NegOrigin = 0;
			PosiTerm = epsonCaliSetting_runtime.xOffset.PosDirAllHeadWidth;
			NegTerm = epsonCaliSetting_runtime.xOffset.NegDirAllHeadWidth;
			
			for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
			{
				angle_offset_PosDir = pAngleX->xPosDirOffset[i] * (x_divide_scale * caliParam.DPI)/pAngleX->nDPI;
				
				angle_offset_NegDir = pAngleX->xNegDirOffset[i] * (x_divide_scale * caliParam.DPI)/pAngleX->nDPI;
				
				epsonCaliSetting_runtime.angleXOffset.xPosDirOffset[i] = angle_offset_PosDir;
				epsonCaliSetting_runtime.angleXOffset.xNegDirOffset[i] = angle_offset_NegDir;
				
				for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
				{
					if(epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j] + angle_offset_PosDir < PosiOrigin)
						PosiOrigin = epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j] + angle_offset_PosDir;
					if(epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j] + angle_offset_PosDir > PosiTerm)
						PosiTerm = epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j] + angle_offset_PosDir;
					if(epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j] + angle_offset_NegDir < PosiTerm)
						PosiTerm = epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j] + angle_offset_NegDir;
					if(epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j] + angle_offset_NegDir > NegTerm)
						NegTerm = epsonCaliSetting_runtime.xOffset.xNegDirOffset.XOffset[i][j] + angle_offset_NegDir;
				}
			}
			epsonCaliSetting_runtime.angleXOffset.PosDirWidthEx = PosiTerm - PosiOrigin - epsonCaliSetting_runtime.xOffset.PosDirAllHeadWidth;
			epsonCaliSetting_runtime.angleXOffset.PosDirOriginOffset = PosiOrigin;
			epsonCaliSetting_runtime.angleXOffset.NegDirWidthEx = NegTerm - NegOrigin - epsonCaliSetting_runtime.xOffset.NegDirAllHeadWidth;
			epsonCaliSetting_runtime.angleXOffset.NegDirOriginOffset = NegOrigin;
		}
	}
#endif
	
	epsonCaliSetting_runtime.stepPass.BaseStep = epsonCaliSetting.eprCaliSetting.stepPass[caliParam.MediaType].BaseStep;
	for(i=0; i<MAX_PASS_NUMBER_CALIBRATION; i++)
		epsonCaliSetting_runtime.stepPass.PassStep[i] = (INT16S)epsonCaliSetting.eprCaliSetting.stepPass[caliParam.MediaType].PassStep[i];
	
	return True;
}

INT16U GetPassNum(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam)
{
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	switch (type)
	{
	case DotCheck:
		if(pPrintParam->VSDModel !=2)
		{
#ifdef NEW_DOT_CHECK        
			//each pass print 4 nozzle row for variable dot.
			//for epson, it need 2 pass each head in head_group. for ricoh 4 color, it need 2 pass; for ricoh 8Head 8color, it need 4 pass.
			return (head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave * NOZZLE_LINE_COUNT_PER_HEAD + 
					DOTCHECK_LINECOUNT_PERPASS -1)/DOTCHECK_LINECOUNT_PERPASS + 1;
#else
			return (head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave * NOZZLE_LINE_COUNT_PER_HEAD + 
					DOTCHECK_LINECOUNT_PERPASS -1)/DOTCHECK_LINECOUNT_PERPASS;
#endif
		}
		else
		{
			//each pass print 32 nozzle row for fixed dot.
			return (head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave * NOZZLE_LINE_COUNT_PER_HEAD + 
					DOTCHECK_FIXDOT_LINECOUNT_PERPASS -1)/DOTCHECK_FIXDOT_LINECOUNT_PERPASS;
		}
	case NozzleCheck:
		return 1;
	case StepCheck:
		if((pPrintParam->Option & 0x1) == 0)
			return 2;
		else
			return 1 + pPrintParam->PassNum * STEPCHECK_REPEATCOUNT;
	case StepCheck_Micro:
		return STEPCHECK_MICRO_UNITCOUNT+1;
	case VerticalCheck:
		return 1 + epsonCaliSetting.printerSetting.ColorCount;
	case AngleCheck:
		return 5;
	case InterleaveCheck:
		if((pPrintParam->Option & 0x10) == 0)
		{
			if( epsonCaliSetting.printerSetting.bIntegratedHead)
			{
				//每个喷头都有自己的interleave检查，同时如果存在YInterleaveGroup，会有检查总体拼插的Pattern
				return 1 + epsonCaliSetting.printerSetting.head_group*epsonCaliSetting.printerSetting.GroupCount_YInterleave + 
					(epsonCaliSetting.printerSetting.GroupCount_YInterleave>1? epsonCaliSetting.printerSetting.head_group : 0);
			}
			else
			{
				//和上面类似，只不过单位变成了组。
				return 1 + epsonCaliSetting.printerSetting.GroupCount_YInterleave + 
					(epsonCaliSetting.printerSetting.GroupCount_YInterleave>1? 1 : 0);
			}
		}
		else //not completed
			return 1;
	case GroupOffsetCheck:
		{
			if( !epsonCaliSetting.printerSetting.bIntegratedHead)
			{
				return 1 + (epsonCaliSetting.printerSetting.GroupCount_YContinue + 1) * 
					epsonCaliSetting.printerSetting.head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			}
			else
			{
				if(epsonCaliSetting.printerSetting.HeadCount == 2) //keep the pattern same as the old one.
					return 1 + 1 + 1;
				else
					return 1 + (epsonCaliSetting.printerSetting.GroupCount_YContinue + 1) * 
						epsonCaliSetting.printerSetting.head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
			}
		}
	case LeftCheck:
		return 1 + (epsonCaliSetting.printerSetting.GroupCount_YContinue + (((pPrintParam->Option & 0x6) >> 1) + 1) ) *
			epsonCaliSetting.printerSetting.ColorCount * epsonCaliSetting.printerSetting.YInterleaveNum;
	case BiDirCheck:
#ifdef  BI_TEST  
		return 1 + 6;
#else
		return 1 + (1 + (((pPrintParam->Option & 0x6) >> 1) + 1) );
#endif
	case RightCheck:
		return 1 + (epsonCaliSetting.printerSetting.GroupCount_YContinue + (((pPrintParam->Option & 0x6) >> 1) + 1) ) *
			epsonCaliSetting.printerSetting.ColorCount * epsonCaliSetting.printerSetting.YInterleaveNum;
	case HeadYIntervalCheck:
		if(epsonCaliSetting.printerSetting.bYIntervalReplaceVertical && epsonCaliSetting.printerSetting.HeadCount == 2)
			return 2; //assert only 2head & EPSON & Head_group == 2.
		else
			return 1 + epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	case VerticalAlignCheck:
		return 2;
	case AngleLeftCheck:
		return 1 + (2 * (((pPrintParam->Option & 0x6) >> 1) + 1) );
	case AngleRightCheck:
		return 1 + (2 * (((pPrintParam->Option & 0x6) >> 1) + 1) );
	case HWDynOffsetTest:
		return HWTEST_DYN_PASSNUM;
	default:
		return 0;
	}
}

INT8U GetUnitCount(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam)
{
	INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
	INT8U SameDir_Scale;
	if(type == AngleLeftCheck || type == AngleRightCheck)
	{
		return ((ANGLECHECK_UNITCOUNT-1) * divideNum +1);
	}
	else
	{
		if((type == LeftCheck && epsonCaliSetting.printerSetting.bOriginIsRight) ||
		   (type == RightCheck && !epsonCaliSetting.printerSetting.bOriginIsRight) )
			SameDir_Scale = 2;
		else
			SameDir_Scale = 1;
		return ((LEFTCHECK_UNITCOUNT-1)/SameDir_Scale * divideNum +1);
	}
}

INT32U GetPatternWidth(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam, INT16U PassNO)
{
	switch (type)
	{
	case DotCheck:
		{
			INT8S cur_linenum;
			INT8S passnumber = GetPassNum(DotCheck, pPrintParam);
			cur_linenum = (epsonCaliSetting.printerSetting.head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave * 
						   NOZZLE_LINE_COUNT_PER_HEAD + passnumber - 1) / passnumber;
			
			if(pPrintParam->VSDModel !=2)
				return cur_linenum*((6+3*6)*3+(36+1+36+36+36+1+36)*3+(90+36)*(3+3*6)) +
					(epsonCaliSetting.printerSetting.GroupCount-1)*72;
			else
				return cur_linenum*((36+1+36+36+36+1+36) + (90+36)) +
					(epsonCaliSetting.printerSetting.GroupCount-1)*72;
		}
	case NozzleCheck:
#ifdef MANUFACTURER_DYSS
		return ((float)30.0/2.54)*PATTERN_BASE_DPI;
#else		
		//相邻的Y continue group，在X方向是分开的，以便分辨断墨喷嘴是哪个喷头的。
#ifndef RICOH_G4_16H
		if(epsonCaliSetting.printerSetting.GroupCount_YContinue <= 2)
#endif			
		{
			//when Group number > 2, maybe, pattern width is too large. However, I keep on that checknozzle must be printed at a pass.
			return epsonCaliSetting.printerSetting.HeadCount * NOZZLE_LINE_COUNT_PER_HEAD *
				(CHECKNOZZLE_STEPNOZZLE + 1) * ( pPrintParam->DPI * CHECKNOZZLE_NOZZLE_LINEWIDTH/ PATTERN_BASE_DPI);
		}
#ifndef RICOH_G4_16H
		else
		{
			//when Group number > 2, maybe, pattern width is too large. we can shift X position of Y continue group to save X space.
			INT8U shiftUnit = (epsonCaliSetting.printerSetting.GroupCount_YContinue+1)/2;
			return ((epsonCaliSetting.printerSetting.HeadCount + shiftUnit - 1)/ shiftUnit) * 
				NOZZLE_LINE_COUNT_PER_HEAD * (CHECKNOZZLE_STEPNOZZLE + 1) * ( pPrintParam->DPI * CHECKNOZZLE_NOZZLE_LINEWIDTH/ PATTERN_BASE_DPI);
		}
#endif			
#endif		
	case StepCheck:
		if((pPrintParam->Option & 0x1) == 0)
			return (STEPCHECK_UNITCOUNT * (STEPCHECK_UNITLINEWIDTH*2+STEPCHECK_UNITLINE_INTERVAL)) * pPrintParam->DPI / PATTERN_BASE_DPI;
		else
		{
#ifdef STEP_ALL_WIDTH
                	INT32U len = ((INT32U)(LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX)* X_BASE_RES) * pPrintParam->DPI / PATTERN_BASE_DPI;
                        
                        return len;
#else
			INT8U passNum = pPrintParam->PassNum;
			if(passNum == 1)
				passNum = STEPCHECK_BASEPASSNUM;
			return (STEPCHECK_COLORNUM * STEPCHECK_UNITCOUNT * (STEPCHECK_UNITLINEWIDTH*2+STEPCHECK_UNITLINE_INTERVAL) +
					STEPCHECK_COLORINTERVAL + passNum*STEPCHECK_BAR_WIDTH*STEPCHECK_COLORNUM) * pPrintParam->DPI / PATTERN_BASE_DPI;
#endif
		}
	case StepCheck_Micro:
		return STEPCHECK_MICRO_UNITCOUNT * STEPCHECK_COLORNUM * (STEPCHECK_UNITLINEWIDTH*2 + STEPCHECK_UNITLINE_INTERVAL)*
			pPrintParam->DPI / PATTERN_BASE_DPI;
	case VerticalCheck:
		return (VERTICAL_CALI_UNITCOUNT * (VERTICAL_CALI_UNITLINEWIDTH*2+VERTICAL_CALI_UNITLINE_INTERVAL) +
				LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH) *
			pPrintParam->DPI / PATTERN_BASE_DPI;
	case AngleCheck:
		return ((epsonCaliSetting.printerSetting.GroupCount -1) * VERTICALCHECK_HEADINTERVAL +
				epsonCaliSetting.printerSetting.GroupCount * VERTICALCHECK_COLORNUM *
					(VERTICALCHECK_COLORINTERVAL + VERTICALCHECK_LINEINTERVAL + VERTICALCHECK_LINECOUNT)) *
			pPrintParam->DPI / PATTERN_BASE_DPI;
	case InterleaveCheck:
		if((pPrintParam->Option & 0x10) == 0)
		{
			INT8S interleaveUnit, head_group = epsonCaliSetting.printerSetting.head_group;
			
			if( epsonCaliSetting.printerSetting.bIntegratedHead)
			{
				interleaveUnit = (NOZZLE_LINE_COUNT_PER_HEAD / INTERLEAVE_CALIBRATION);
				return INTERLEAVECHECK_UNITCOUNT * (INTERLEAVECHECK_UNITINTERVAL + interleaveUnit *
													(INTERLEAVECHECK_COLORINTERVAL+ INTERLEAVECHECK_UNITLINEWIDTH*2+INTERLEAVECHECK_UNITLINE_OVERLAP)) *
					pPrintParam->DPI / PATTERN_BASE_DPI;
			}
			else
			{
				interleaveUnit = head_group;
				return INTERLEAVECHECK_UNITCOUNT * (INTERLEAVECHECK_UNITINTERVAL + interleaveUnit *
													(INTERLEAVECHECK_COLORINTERVAL+ INTERLEAVECHECK_UNITLINEWIDTH*2+INTERLEAVECHECK_UNITLINE_OVERLAP)) *
					pPrintParam->DPI / PATTERN_BASE_DPI;
			}
		}
		else //not completed, for check the linear of X guide.
			return 40 * PATTERN_BASE_DPI * pPrintParam->DPI / PATTERN_BASE_DPI;
	case GroupOffsetCheck:
		{
			return (GetUnitCount(type, pPrintParam)* (LEFTCHECK_LINECOUNT + LEFTCHECK_LINEINTERVAL + LEFTCHECK_COLORINTERVAL) +
					LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH) * pPrintParam->DPI / PATTERN_BASE_DPI;
		}
	case LeftCheck:
	case RightCheck:
		{
			return (GetUnitCount(type, pPrintParam)* (LEFTCHECK_LINECOUNT + LEFTCHECK_LINEINTERVAL + LEFTCHECK_COLORINTERVAL) +
					LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH) * pPrintParam->DPI / PATTERN_BASE_DPI;
		}
	case BiDirCheck:
		{
			return GetUnitCount(type, pPrintParam) * (LEFTCHECK_LINECOUNT + LEFTCHECK_LINEINTERVAL + LEFTCHECK_COLORINTERVAL) *
				pPrintParam->DPI/ PATTERN_BASE_DPI;
		}
	case HeadYIntervalCheck:
#if !(defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV))
		return (epsonCaliSetting.printerSetting.GroupCount *(X_BASE_RES/2) + YINTERVALCHECK_COLOR_HOR_LEN*13 + (X_BASE_RES/2) +X_BASE_RES* 5)*pPrintParam->DPI/PATTERN_BASE_DPI + YINTERVALCHECK_COLORNUM *(YINTERVALCHECK_UNITINTERVAL + 
																																																			YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) * 
			pPrintParam->DPI/PATTERN_BASE_DPI;
#else
		return YINTERVALCHECK_COLORNUM *(YINTERVALCHECK_UNITINTERVAL + 
										 YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) * 
			pPrintParam->DPI/PATTERN_BASE_DPI;
#endif
	case VerticalAlignCheck:
		return (NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead) * 
			(epsonCaliSetting.printerSetting.GroupCount_YInterleave * epsonCaliSetting.printerSetting.head_group - 1) * 
				((VERTICAL_ALIGN_UNITLINEWIDTH*2)+VERTICAL_ALIGN_UNITLINE_INTERVAL);
	case AngleLeftCheck:
	case AngleRightCheck:
		return epsonCaliSetting.printerSetting.HeadCount * GetUnitCount(type, pPrintParam) * NOZZLE_LINE_COUNT_PER_HEAD * (ANGLECHECK_LINECOUNT + ANGLECHECK_LINEINTERVAL + ANGLECHECK_UNITINTERVAL) *
			pPrintParam->DPI / 720 + 2 * GetUnitCount(type, pPrintParam);
	case HWDynOffsetTest:
		return HWTEST_DYN_WIDTH;
	default:
		return 0;
	}
}

INT32U GetPatternWidthEx(struct EPSON_X_Cali_Runtime * pCurXCali, INT32U PatternWidth, INT8U bIsPosDir)
{
	if(bIsPosDir)
	{
		PatternWidth += pCurXCali->PosDirAllHeadWidth;
		if(epsonCaliSetting_runtime.angleXOffset.bEnable)
			PatternWidth += epsonCaliSetting_runtime.angleXOffset.PosDirWidthEx;
	}
	else
	{
		PatternWidth += pCurXCali->NegDirAllHeadWidth;
		if(epsonCaliSetting_runtime.angleXOffset.bEnable)
			PatternWidth += epsonCaliSetting_runtime.angleXOffset.NegDirWidthEx;
	}
	
	return PatternWidth;
}

INT32U CalcBuffSize(INT32U PatternWidthEx)
{
	INT32U size;
	
	size = (PatternWidthEx * EPSON_DATA_PER_FIRE+BUFF_BLOCK_SIZE-1)/BUFF_BLOCK_SIZE*BUFF_BLOCK_SIZE;
	
	return size;
}

INT32U GetCaliJobHeadSize(void)
{
	return BUFF_BLOCK_SIZE;
}

INT32U GetCaliBandHeadSize(void)
{
	return BUFF_BLOCK_SIZE;
}

INT32U GetCaliJobEndSize(void)
{
	return BUFF_BLOCK_SIZE;
}

INT8U GetPrintDir(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam, INT16U PassNO)
{
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	switch (type)
	{
	case DotCheck:
		return 1;
	case NozzleCheck:
		return 1;
	case StepCheck:
	case StepCheck_Micro:
	case VerticalCheck:
		return 1;
	case AngleCheck:
		if(PassNO <3)
			return 1;
		else
			return 0;
	case InterleaveCheck:
		return 1;
	case GroupOffsetCheck:
		return 1;
	case LeftCheck:
		if(	epsonCaliSetting.printerSetting.bOriginIsRight)
			return 1;
		else
		{
			if(PassNO == 0 )
				return 1;
			else
			{
				INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
				INT8U PassNumInSubCycle = (PassNO - 1) % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
				INT8U baseGroupNO;
				INT16S StepNozzleBefNobase;
				INT16S StepNozzleAftNobase;
				INT8U order;
				INT8U UnitNO = (PassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
				INT8U colorNO = UnitNO / epsonCaliSetting.printerSetting.YInterleaveNum;
				INT8U InterleaveNO = UnitNO % epsonCaliSetting.printerSetting.YInterleaveNum;
				INT8U headNO_in_group = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[colorNO],InterleaveNO) % epsonCaliSetting.printerSetting.head_group;
				INT8U insertedPassNO = GetNoBasePassNO_ForHorizatalLikeCheck(
																			 divideNum, colorNO, (InterleaveNO == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) ),
																			 headNO_in_group, &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
				
				if(PassNumInSubCycle >= insertedPassNO && PassNumInSubCycle < insertedPassNO + divideNum)
					return 0;
				else
					return 1;
			}
		}
	case RightCheck:
		if(	!epsonCaliSetting.printerSetting.bOriginIsRight)
			return 1;
		else
		{
			if(PassNO == 0 )
				return 1;
			else
			{
				INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
				INT8U PassNumInSubCycle = (PassNO - 1) % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
				INT8U baseGroupNO;
				INT16S StepNozzleBefNobase;
				INT16S StepNozzleAftNobase;
				INT8U order;
				INT8U UnitNO = (PassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
				INT8U colorNO = UnitNO / epsonCaliSetting.printerSetting.YInterleaveNum;
				INT8U InterleaveNO = UnitNO % epsonCaliSetting.printerSetting.YInterleaveNum;
				INT8U headNO_in_group = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[colorNO],InterleaveNO) % epsonCaliSetting.printerSetting.head_group;
				INT8U insertedPassNO = GetNoBasePassNO_ForHorizatalLikeCheck(
																			 divideNum, colorNO, (InterleaveNO == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) ),
																			 headNO_in_group, &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase, &order);
				
				if(PassNumInSubCycle >= insertedPassNO && PassNumInSubCycle < insertedPassNO + divideNum)
					return 0;
				else
					return 1;
			}
		}
	case BiDirCheck:
#ifdef  BI_TEST
		if( PassNO == 0)
			return 1;
		else if( PassNO % 2 == 1)
			return 1;
		else
			return 0;
#else
		if( PassNO < 2)
			return 1;
		else
			return 0;
#endif		
	case HeadYIntervalCheck:
		return 1;
	case VerticalAlignCheck:
		return 1;
	case AngleLeftCheck:
		if(	epsonCaliSetting.printerSetting.bOriginIsRight)
			return 1;
		else
		{
			if(PassNO == 0 )
				return 1;
			else
				return 0;
		}
	case AngleRightCheck:
		if(	!epsonCaliSetting.printerSetting.bOriginIsRight)
			return 1;
		else
		{
			if(PassNO == 0 )
				return 1;
			else
				return 0;
		}
	case HWDynOffsetTest:
		return 1;
	default:
		return 0;
	}
}

INT8U * LockPrintBuff(INT32U Size)
{
	INT8U *pBuff;
	OS_CPU_SR  cpu_sr;	
	INT8U err;
	
	do
	{
		pBuff = NULL;
		OS_ENTER_CRITICAL();	
		if(HugeBuf_GetSize() + Size < HUGEBUF_SIZE)
		{
			pBuff = HugeBuf_GetRecAddr();	
		}
		OS_EXIT_CRITICAL();	
		OSTimeDly(100);
		if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_CANCEL, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
			break;
	}while(pBuff == NULL);
	
	return pBuff;
}

INT8U UnlockPrintBuff(INT8U * pBuff, INT32U Size)
{
	HugeBuf_PutSize(Size);
	return True;
}

INT8U GenJobHead(INT8U * pBuff, enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam)
{
	INT16U passheight;
	INT8U SpeedIndex;
	
	DataHeaderType * pHead = (DataHeaderType *)pBuff;
	pHead->header_flag = 'BYHX';
	pHead->header_size = HUGEBUF_ALIGN_SIZE;
	pHead->header_type = JOB_START_HEAD;
	pHead->data_bytescnt = 0;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	if(pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X / 2 ||
	   pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X *3 /4 / 2)
		pHead->Inf.job_inf.encoder_div = 2;
	else if(pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X / 3)
		pHead->Inf.job_inf.encoder_div = 3;
	else if(pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X / 4)
		pHead->Inf.job_inf.encoder_div = 4;	
	else
		pHead->Inf.job_inf.encoder_div = 1;
	
	pHead->Inf.job_inf.dir_space = 0;
	
	INT8U groupNo_YContinue = epsonCaliSetting.printerSetting.BaseColor_HeadIndex / head_group % 
		epsonCaliSetting.printerSetting.GroupCount_YContinue;
	
	switch(type)
	{
	case DotCheck:
		passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * 
			epsonCaliSetting.printerSetting.GroupCount_YContinue;
		pHead->Inf.job_inf.jobHeight = passheight * GetPassNum(type, pPrintParam);
		break;
	case NozzleCheck:
		pHead->Inf.job_inf.jobHeight = epsonCaliSetting.printerSetting.NozzleCount *
			epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.GroupCount_YContinue;
		break;
	case StepCheck:
		passheight = epsonCaliSetting.printerSetting.NozzleCount *
			epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.GroupCount_YContinue;
		if((pPrintParam->Option & 0x1) == 0)
		{
			pHead->Inf.job_inf.jobHeight = (epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNo_YContinue) *
				epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
			if( pPrintParam->PassNum == 1)
				pHead->Inf.job_inf.jobHeight += passheight/STEPCHECK_BASEPASSNUM;
			else
				pHead->Inf.job_inf.jobHeight += passheight/(pPrintParam->PassNum);
		}
		else
		{
			pHead->Inf.job_inf.jobHeight = (epsonCaliSetting.printerSetting.NozzleCount - STEPCHECK_DIGITAL_STARTNOZZLE) * epsonCaliSetting.printerSetting.YInterleaveNum;
			pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) *
				epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
			pHead->Inf.job_inf.jobHeight += passheight * STEPCHECK_REPEATCOUNT;
		}
		break;
		//    case StepCheck_Micro:
		//        passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.GroupCount_YContinue;
		//        pHead->Inf.job_inf.jobHeight = (epsonCaliSetting.printerSetting.GroupCount_YContinue - epsonCaliSetting.printerSetting.BaseColor_HeadIndex) *
		//            epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		//        if(pPrintParam->PassNum == 1)
		//            pHead->Inf.job_inf.jobHeight += GetPassNum(type, pPrintParam)*passheight/STEPCHECK_BASEPASSNUM;
		//        else
		//            pHead->Inf.job_inf.jobHeight += GetPassNum(type, pPrintParam)*passheight/(pPrintParam->PassNum);
		//        break;
	case VerticalCheck:
		passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight = passheight * epsonCaliSetting.printerSetting.ColorCount + 
			(epsonCaliSetting.printerSetting.NozzleCount - (TITLE_STARTNOZZLE-TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.GroupCount_YContinue -1 - groupNo_YContinue) *
			epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		break;
	case AngleCheck:
		passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.GroupCount_YContinue;
		pHead->Inf.job_inf.jobHeight = passheight + epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * (9+11+9)/10 +
			(epsonCaliSetting.printerSetting.NozzleCount - (VERTICALCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.GroupCount_YContinue -1 - groupNo_YContinue) *
			epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		break;
	case InterleaveCheck:
		if((pPrintParam->Option & 0x10) == 0)
		{
			passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.GroupCount_YContinue;
			if(epsonCaliSetting.printerSetting.bIntegratedHead)
			{
				pHead->Inf.job_inf.jobHeight = passheight * epsonCaliSetting.printerSetting.head_group * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
				if(epsonCaliSetting.printerSetting.GroupCount_YInterleave>1)
					pHead->Inf.job_inf.jobHeight += passheight * epsonCaliSetting.printerSetting.head_group;
			}
			else
			{
				pHead->Inf.job_inf.jobHeight = passheight * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
				if(epsonCaliSetting.printerSetting.GroupCount_YInterleave>1)
					pHead->Inf.job_inf.jobHeight += passheight;
			}
			pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.NozzleCount - (INTERLEAVECHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum;
			pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.GroupCount_YContinue -1 - groupNo_YContinue) *
				epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		}
		else
		{//not completed
			passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.GroupCount_YContinue;
			pHead->Inf.job_inf.jobHeight = passheight;
		}
		break;
	case GroupOffsetCheck:
		//however, the total step is same.
		passheight = epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight = passheight * epsonCaliSetting.printerSetting.GroupCount_YInterleave * head_group;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) *
			epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		break;
	case LeftCheck:
	case RightCheck:
		passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.GroupCount_YContinue;
		pHead->Inf.job_inf.jobHeight = passheight * epsonCaliSetting.printerSetting.ColorCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.GroupCount_YContinue -1 - groupNo_YContinue) *
			epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		break;
	case BiDirCheck:
		passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight = passheight + (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) *
			epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		break;
	case HeadYIntervalCheck:
		passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.GroupCount_YContinue;
		pHead->Inf.job_inf.jobHeight = passheight + (epsonCaliSetting.printerSetting.NozzleCount - (YINTERVALCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.GroupCount_YContinue -1 - groupNo_YContinue) *
			epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		break;
	case VerticalAlignCheck:
		passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight = passheight + (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) *
			epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
	case AngleLeftCheck:
	case AngleRightCheck:
		passheight = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.HeadCount;
		passheight += (epsonCaliSetting.printerSetting.NozzleCount - ANGLECHECK_OVERLAP_NOZZLE)* epsonCaliSetting.printerSetting.YInterleaveNum; //add one head height.
		pHead->Inf.job_inf.jobHeight = passheight + (epsonCaliSetting.printerSetting.NozzleCount - (ANGLECHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum;
		pHead->Inf.job_inf.jobHeight += (epsonCaliSetting.printerSetting.HeadCount -1 - epsonCaliSetting.printerSetting.BaseColor_HeadIndex) *
			epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		break;
	case HWDynOffsetTest:
		pHead->Inf.job_inf.jobHeight = HWTEST_DYN_PASSNUM;
		break;
	default:
		pHead->Inf.job_inf.jobHeight = 0;
		break;
	}
	
	switch(pPrintParam->VSDModel)
	{
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	case 1:
		SpeedIndex = LCDMenuConfig_EX.Calc_X_Speed;
		break;
	case 2:
		SpeedIndex = LCDMenuConfig_EX.Calc_X_Speed + (2-1)*MAX_SPEED_LEVEL_EACH_VSD;
		break;
#else
	case 1:
		SpeedIndex = 1;
		break;
	case 2:
		SpeedIndex = 2;
		//SpeedIndex = 4;
		break;
#endif
	case 3:
	default:
		SpeedIndex = 4;
		break;
	case 4:
		SpeedIndex = 0;
		break;
	}
	pHead->Inf.job_inf.speed = SpeedIndex;
	pHead->Inf.job_inf.blankBandDryTimeMS = 0x3E8;
	
	pHead->Inf.job_inf.language = 0;
	memset(pHead->Inf.job_inf.languageStr,0,sizeof(pHead->Inf.job_inf.languageStr));
	
	pHead->Inf.job_inf.YDir = 0;
	pHead->Inf.job_inf.dataBitDeep = 2;
	pHead->Inf.job_inf.printBitDeep = 2;
	pHead->Inf.job_inf.mappedDataFor1Bit = 3;
	pHead->Inf.job_inf.VSDModel = pPrintParam->VSDModel;
	
	if(pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X / 2 ||
	   pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X / 4 ||
	   pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X)
		pHead->Inf.job_inf.baseDPI = 0;
	else if(pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X *3 /4 / 2 ||
			pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X *3 /4)
		pHead->Inf.job_inf.baseDPI = 1;
	else
		pHead->Inf.job_inf.baseDPI = 2;
	
	if(type == StepCheck || type == StepCheck_Micro)
		pHead->Inf.job_inf.PassNumber = pPrintParam->PassNum;
	else
		pHead->Inf.job_inf.PassNumber = 1;
	
	pHead->Inf.job_inf.displayResolution = pPrintParam->DPI;
	
	return True;
}

INT32S GetYStep_StepCheck_Micro(INT8U index, INT32S step_per_nozzle, INT8U total_count)
{
	INT8U count = total_count/2;
	INT32S cur_step = step_per_nozzle;
	INT8U cur_index = 0;
	
	if(cur_step < count)
		cur_step = count;
	if(index < count)
	{
		while(cur_index < index)
		{
			if(cur_step >(count- (cur_index+1))*2 )
				cur_step /= 2;
			else
			{
				cur_step = count - index;
				break;
			}
			cur_index ++;
		}
		cur_step = - cur_step;
	}
	else if(index == count)
	{
		cur_step = 0;
	}
	else
	{
		while(cur_index < total_count - 1 - index)
		{
			if(cur_step >(count- (cur_index+1))*2 )
				cur_step /= 2;
			else
			{
				cur_step = count - (total_count - 1 - index);
				break;
			}
			cur_index ++;
		}
	}
	
	return cur_step;
}

//在GroupOffsetCheck中，分成若干循环。每循环包括base和被校正的喷头。为了提高效率，被校正的喷头实际上是GroupCount_YContinue个，在一pass中打印出来。
//   所以Base需要打印GroupCount_YContinue次。根据base和当前被校正的喷头的垂直位置，这个函数得到在第几pass打被校正的喷头。
//   循环次数是head_group*GroupCount_YInterleave，因为没有办法在一pass内打出来。必须多个循环才能打完。
INT8U GetNoBasePassNO_ForGroupOffsetCheck(INT8U divideNum, INT8U headNO_in_group, INT8U * baseGroupNO_YContinue,
										  INT16S * StepNozzleBefNobase, INT16S * StepNozzleAftNobase,INT8U *order )
{
  *order = 0;
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	INT8S head_group_pure_color = (factoryData.color_num * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
								   NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	//here, we assert white and vanish is use 
	INT8S head_group_white  = (factoryData.spot_color1_headnum * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
							   NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	INT8S head_group_coat  = (factoryData.spot_color2_headnum * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
							  NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	if(head_group_white ==0 && factoryData.spot_color1_headnum  > 0)
		head_group_white = 1;
	if(head_group_coat ==0 && factoryData.spot_color2_headnum  > 0)
		head_group_coat = 1;
	
	* baseGroupNO_YContinue = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index] / 
		head_group % epsonCaliSetting.printerSetting.GroupCount_YContinue;
	
	INT8U baseHeadNO_in_group = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index] % head_group;
	//#ifndef MANUFACTURER_DYSS
   INT8U index[4]={0};
	if(factoryData.spot_color1_headnum + factoryData.spot_color2_headnum > 0)
	{
		
      switch((factoryDataEx.LayoutType>>4)&0x7)
		{
          case 0:
          {
              index[0]=1;
              index[1]=0;
              index[2]=2;
              break;
          }
          case 1:
          {
              index[0]=1;
              index[1]=2;
              index[2]=0;
              break;
          }
          case 2:
          {
              index[0]=1;
              index[1]=0;
              index[2]=0;
              break;
          }
          case 3:
          {
              index[0]=0;
              index[1]=1;
              index[2]=1;
              break;
          }
          default:
          break;
      }
      if( baseHeadNO_in_group < head_group_pure_color)
			{
					baseHeadNO_in_group = index[0];
			}
			else if(baseHeadNO_in_group < head_group_pure_color + head_group_white)
			{
					baseHeadNO_in_group = index[1];
			}
			else
				  baseHeadNO_in_group = index[2];
      if( headNO_in_group < head_group_pure_color)
      {
            headNO_in_group = index[0];
      }
      else if(headNO_in_group < head_group_pure_color + head_group_white)
      {
            headNO_in_group = index[1];
      }
      else
            headNO_in_group = index[2];
    }
	INT8S shiftGroupNum;
	
	if( (epsonCaliSetting.printerSetting.verticalNozzleShift >= 0 && baseHeadNO_in_group <= headNO_in_group) ||
	   (epsonCaliSetting.printerSetting.verticalNozzleShift <= 0 && baseHeadNO_in_group >= headNO_in_group) )
	{
		INT8S group_shift = headNO_in_group - baseHeadNO_in_group ;
		shiftGroupNum = epsonCaliSetting.printerSetting.verticalNozzleShift * group_shift/
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		*StepNozzleBefNobase = (epsonCaliSetting.printerSetting.verticalNozzleShift * headNO_in_group) %
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);

		*StepNozzleAftNobase = (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum) - *StepNozzleBefNobase;
	}
	else
	{     
		INT8S group_shift = headNO_in_group - baseHeadNO_in_group ;		
		shiftGroupNum = epsonCaliSetting.printerSetting.verticalNozzleShift * group_shift/
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		if( epsonCaliSetting.printerSetting.verticalNozzleShift * headNO_in_group %
		   (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum) != 0)
			shiftGroupNum --;
		* StepNozzleAftNobase = (-epsonCaliSetting.printerSetting.verticalNozzleShift * headNO_in_group) %
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		if(* StepNozzleAftNobase == 0)
			* StepNozzleAftNobase = (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		
		* StepNozzleBefNobase = (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum) - *StepNozzleAftNobase;
	}
	
	INT8S PassNO = epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - *baseGroupNO_YContinue + 1 + shiftGroupNum;
	
	if(PassNO >= (epsonCaliSetting.printerSetting.GroupCount_YContinue + 1))
	{
		*StepNozzleBefNobase += (PassNO - (epsonCaliSetting.printerSetting.GroupCount_YContinue + 1 - 1)) *
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		* StepNozzleAftNobase = 0;
		PassNO = (epsonCaliSetting.printerSetting.GroupCount_YContinue + 1) - 1;
		*order =1;
	}
	else if(PassNO < 0)
	{
		* StepNozzleAftNobase += (-PassNO) *
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		* StepNozzleBefNobase = 0;
		PassNO = 0;
	}

	return PassNO;
}

//类似GroupOffsetCheck，在左右校正中，分成若干循环。每循环包括base和被校正的一列喷嘴。为了提高效率，被校正的喷嘴实际上有GroupCount_YContinue高，在一pass中打印出来。
//   所以Base需要打印GroupCount_YContinue次。根据base和当前被校正的喷头的垂直位置，这个函数得到在第几pass打被校正的喷嘴。
//   循环次数是ColorCount*GroupCount_YInterleave，因为没有办法在一pass内打出来。必须多个循环才能打完。
INT8U GetNoBasePassNO_ForHorizatalLikeCheck(INT8U divideNum, INT8U colorNO, INT8U bAllBase,
											INT8U headNO_in_group, INT8U * baseGroupNO_YContinue,
											INT16S * StepNozzleBefNobase, INT16S * StepNozzleAftNobase,INT8U *order )
{
	if(bAllBase)
		return GetNoBasePassNO_ForGroupOffsetCheck(
												   divideNum, headNO_in_group, baseGroupNO_YContinue, StepNozzleBefNobase, StepNozzleAftNobase,order );
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	*order = 0;
	INT8S head_group_pure_color = (factoryData.color_num * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
								   NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	//here, we assert white and vanish is use 
	INT8S head_group_white  = (factoryData.spot_color1_headnum * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
							   NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	if(head_group_white ==0 && factoryData.spot_color1_headnum  > 0)
		head_group_white = 1;
	INT8S head_group_coat  = (factoryData.spot_color2_headnum * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
							  NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
	if(head_group_coat ==0 && factoryData.spot_color2_headnum  > 0)
		head_group_coat = 1;
	
	* baseGroupNO_YContinue = epsonCaliSetting.colorMap.BaseHeadNo[colorNO] / head_group % 
		epsonCaliSetting.printerSetting.GroupCount_YContinue;
	
	INT8U baseHeadNO_in_group = epsonCaliSetting.colorMap.BaseHeadNo[colorNO]%head_group;
  INT8U index[4]={0};
    if(factoryData.spot_color1_headnum + factoryData.spot_color2_headnum > 0)
    {
       switch((factoryDataEx.LayoutType>>4)&0x7)
       {
           case 0:
           {
               index[0]=1;
               index[1]=0;
               index[2]=2;
               break;
           }
           case 1:
           {
               index[0]=1;
               index[1]=2;
               index[2]=0;
               break;
           }
           case 2:
           {
               index[0]=1;
               index[1]=0;
               index[2]=0;
               break;
           }
           case 3:
           {
               index[0]=0;
               index[1]=1;
               index[2]=1;
               break;
           }
           default:
           break;
       }
       if( baseHeadNO_in_group < head_group_pure_color)
       {
           baseHeadNO_in_group = index[0];
       }
       else if(baseHeadNO_in_group < head_group_pure_color + head_group_white)
       {
           baseHeadNO_in_group = index[1];
       }
       else
           baseHeadNO_in_group = index[2];
       if( headNO_in_group < head_group_pure_color)
       {
             headNO_in_group = index[0];
       }
       else if(headNO_in_group < head_group_pure_color + head_group_white)
       {
             headNO_in_group = index[1];
       }
       else
             headNO_in_group = index[2];
     }

	INT8S shiftGroupNum;
	
	if(epsonCaliSetting.printerSetting.verticalNozzleShift >= 0)
	{
		
		INT8S group_shift = headNO_in_group - baseHeadNO_in_group ;

		shiftGroupNum = epsonCaliSetting.printerSetting.verticalNozzleShift * group_shift/
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		* StepNozzleBefNobase = (epsonCaliSetting.printerSetting.verticalNozzleShift * headNO_in_group) %
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		
		* StepNozzleAftNobase = (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum) - * StepNozzleBefNobase;
	}
	else
	{		
		INT8S group_shift = headNO_in_group - baseHeadNO_in_group ;
 
		shiftGroupNum = epsonCaliSetting.printerSetting.verticalNozzleShift * group_shift/
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		if( epsonCaliSetting.printerSetting.verticalNozzleShift * headNO_in_group %
		   (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum) != 0)
			shiftGroupNum --;
		
		* StepNozzleAftNobase = (-epsonCaliSetting.printerSetting.verticalNozzleShift * headNO_in_group) %
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		if(* StepNozzleAftNobase == 0)
			* StepNozzleAftNobase = (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		
		* StepNozzleBefNobase = (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum) - * StepNozzleAftNobase;
	}
	
	INT8S PassNO = epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - * baseGroupNO_YContinue + 1 + shiftGroupNum;
	
	if(PassNO >= (epsonCaliSetting.printerSetting.GroupCount_YContinue + 1))
	{
		*StepNozzleBefNobase += (PassNO - (epsonCaliSetting.printerSetting.GroupCount_YContinue + 1 - 1)) *
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		* StepNozzleAftNobase = 0;
		PassNO = (epsonCaliSetting.printerSetting.GroupCount_YContinue + 1) - 1;
	}
	else if(PassNO < 0)
	{
		* StepNozzleAftNobase += (-PassNO) *
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum);
		* StepNozzleBefNobase = 0;
		PassNO = 0;
	}
	//#ifdef MANUFACTURER_DYSS
	//    return 1;
	//#else
	return PassNO;
	//#endif
}


INT32S GetYStep(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam,
				struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT32S step, nozzle, passnum, step_per_nozzle;
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	INT8U groupNo_YContinue = epsonCaliSetting.printerSetting.BaseColor_HeadIndex / head_group % 
		epsonCaliSetting.printerSetting.GroupCount_YContinue;
	
	switch (type)
	{
	case DotCheck:
		if(pRuntimeParam->curpassNO != GetPassNum(DotCheck, pPrintParam) - 1)
		{
			INT32S adjustNozzle = 0;
			if(head_group > 1)
			{
				INT8U headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0], 0);
				INT8U lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[0], 0);
				INT32S YOffset1 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead], 0);
				lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead],
											   NOZZLE_LINE_COUNT_PER_HEAD % epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				INT32S YOffset2 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				adjustNozzle = (YOffset2 - YOffset1)*epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
			}
			
			INT8U passCountWhenChanged;
			if(pPrintParam->VSDModel !=2)
				passCountWhenChanged = (NOZZLE_LINE_COUNT_PER_HEAD * epsonCaliSetting.printerSetting.GroupCount_YInterleave) / DOTCHECK_LINECOUNT_PERPASS;
			else
				passCountWhenChanged = (NOZZLE_LINE_COUNT_PER_HEAD * epsonCaliSetting.printerSetting.GroupCount_YInterleave) / DOTCHECK_FIXDOT_LINECOUNT_PERPASS;
			
			INT8U HeadCountWhenChanged = 1;
			if(passCountWhenChanged == 0)
			{
				passCountWhenChanged = 1;
				if(pPrintParam->VSDModel !=2)
					HeadCountWhenChanged = DOTCHECK_LINECOUNT_PERPASS/(NOZZLE_LINE_COUNT_PER_HEAD * epsonCaliSetting.printerSetting.GroupCount_YInterleave);
				else
					HeadCountWhenChanged = DOTCHECK_FIXDOT_LINECOUNT_PERPASS/(NOZZLE_LINE_COUNT_PER_HEAD * epsonCaliSetting.printerSetting.GroupCount_YInterleave);
			}
			
			if( (pRuntimeParam->curpassNO) % passCountWhenChanged == passCountWhenChanged - 1 )
			{
				nozzle = (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum *
						  epsonCaliSetting.printerSetting.GroupCount_YContinue) + 
					(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum *
					 epsonCaliSetting.printerSetting.GroupCount_YContinue)/10 + 
						adjustNozzle * HeadCountWhenChanged;
				if(nozzle <0)
					nozzle = 0;
				return epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
					(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum *
					 epsonCaliSetting.printerSetting.GroupCount_YContinue);
			}
			else
			{
				return epsonCaliSetting_runtime.stepPass.BaseStep + epsonCaliSetting_runtime.stepPass.BaseStep/10;
			}
		}
		else
			return epsonCaliSetting_runtime.stepPass.BaseStep;
	case NozzleCheck:
		return epsonCaliSetting_runtime.stepPass.BaseStep;
	case StepCheck:
		if((pPrintParam->Option & 0x1) == 0)
		{
			if(pRuntimeParam->curpassNO == 0)
			{
				if(pPrintParam->PassNum == 1)
					passnum = STEPCHECK_BASEPASSNUM;
				else
					passnum = pPrintParam->PassNum;
				
				nozzle = epsonCaliSetting.printerSetting.NozzleCount*epsonCaliSetting.printerSetting.GroupCount_YContinue/passnum;
				step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
					(epsonCaliSetting.printerSetting.NozzleCount *epsonCaliSetting.printerSetting.GroupCount_YContinue);
				step += epsonCaliSetting_runtime.stepPass.PassStep[pPrintParam->PassNum-1];
				return step;
			}
			else
			{
				return epsonCaliSetting_runtime.stepPass.BaseStep *
					(epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNo_YContinue)
						/epsonCaliSetting.printerSetting.GroupCount_YContinue;
			}
		}
		else
		{
			if(pRuntimeParam->curpassNO == 0)
			{
				nozzle = (epsonCaliSetting.printerSetting.NozzleCount - STEPCHECK_DIGITAL_STARTNOZZLE) * epsonCaliSetting.printerSetting.YInterleaveNum;
				step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
					(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.GroupCount_YContinue);
				step += epsonCaliSetting_runtime.stepPass.BaseStep * (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue)/
					epsonCaliSetting.printerSetting.GroupCount_YContinue;
				return step;
			}
			else
			{
				if(pPrintParam->PassNum == 1)
					passnum = STEPCHECK_BASEPASSNUM;
				else
					passnum = pPrintParam->PassNum;
				
				nozzle = (epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM*2) *epsonCaliSetting.printerSetting.GroupCount_YContinue/passnum;
				step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
					((epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM*2) * epsonCaliSetting.printerSetting.GroupCount_YContinue);
				step += epsonCaliSetting_runtime.stepPass.PassStep[pPrintParam->PassNum-1];
				return step;
			}
		}
		//    case StepCheck_Micro:
		//        if(pRuntimeParam->curpassNO != STEPCHECK_MICRO_UNITCOUNT)
		//        {
		//            if(pPrintParam->PassNum == 1)
		//                passnum = STEPCHECK_BASEPASSNUM;
		//            else
		//                passnum = pPrintParam->PassNum;
		//
		//            nozzle = epsonCaliSetting.printerSetting.NozzleCount*epsonCaliSetting.printerSetting.HeadCount/passnum;
		//            step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
		//                (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.HeadCount);
		//            step += epsonCaliSetting_runtime.stepPass.PassStep[pPrintParam->PassNum-1];
		//            step_per_nozzle = step/nozzle;
		//            step += GetYStep_StepCheck_Micro(pRuntimeParam->curpassNO, step_per_nozzle, STEPCHECK_MICRO_UNITCOUNT);
		//            return step;
		//        }
		//        else
		//            return epsonCaliSetting_runtime.stepPass.BaseStep *
		//                (epsonCaliSetting.printerSetting.HeadCount - epsonCaliSetting.printerSetting.BaseColor_HeadIndex)/epsonCaliSetting.printerSetting.HeadCount;
	case VerticalCheck:
		switch(pRuntimeParam->curpassNO)
		{
		case 0:
			nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (INTERLEAVECHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
			step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
				(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
			step += epsonCaliSetting_runtime.stepPass.BaseStep *
				(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue)/epsonCaliSetting.printerSetting.GroupCount_YContinue;
			return step;
		case 1:
		default:
			return epsonCaliSetting_runtime.stepPass.BaseStep/epsonCaliSetting.printerSetting.GroupCount_YContinue;
		}
	case AngleCheck:
		switch(pRuntimeParam->curpassNO)
		{
		case 0:
			nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (VERTICALCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
			step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
				(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
			step += epsonCaliSetting_runtime.stepPass.BaseStep *
				(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue)/epsonCaliSetting.printerSetting.GroupCount_YContinue;
			return step;
		case 1:
		case 3:
			nozzle = epsonCaliSetting.printerSetting.NozzleCount * 9 / 10;
			step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
				(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
			return step;
		case 2:
			nozzle = epsonCaliSetting.printerSetting.NozzleCount * 11 / 10;
			step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
				(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
			return step;
		case 4:
		default:
			return epsonCaliSetting_runtime.stepPass.BaseStep;
		}
	case InterleaveCheck:
		if((pPrintParam->Option & 0x10) == 0)
		{
			INT32S adjustNozzle = 0;
			if(head_group > 1)
			{
				INT8U headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0], 0);
				INT8U lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[0], 0);
				INT32S YOffset1 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead], 0);
				lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead],
											   NOZZLE_LINE_COUNT_PER_HEAD % epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				INT32S YOffset2 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				adjustNozzle = (YOffset2 - YOffset1)*epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
			}
			
			switch(pRuntimeParam->curpassNO)
			{
			case 0:
				nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (INTERLEAVECHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
				step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
					(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
				step += epsonCaliSetting_runtime.stepPass.BaseStep * (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue)/
					epsonCaliSetting.printerSetting.GroupCount_YContinue;
				return step;
			case 1:
			default:
				if(pRuntimeParam->curpassNO == GetPassNum(type, pPrintParam) - 1)
					return epsonCaliSetting_runtime.stepPass.BaseStep;
				else
				{
					if( (pRuntimeParam->curpassNO - 1) % head_group != head_group - 1 )
					{
						nozzle = (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue *
								  epsonCaliSetting.printerSetting.YInterleaveNum) + adjustNozzle;
						if(nozzle <0)
							nozzle = 0;
						step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
							(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue * 
							 epsonCaliSetting.printerSetting.YInterleaveNum);
						return step;
					}
					else
					{
						nozzle = (epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue * 
								  epsonCaliSetting.printerSetting.YInterleaveNum) - adjustNozzle * (head_group - 1 );
						if(nozzle <0)
							nozzle = 0;
						step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle/
							(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue * 
							 epsonCaliSetting.printerSetting.YInterleaveNum);
						return step;
					}
				}
			}
		}
		else
		{
			return epsonCaliSetting_runtime.stepPass.BaseStep;
		}
	case GroupOffsetCheck:
		if( !epsonCaliSetting.printerSetting.bIntegratedHead || epsonCaliSetting.printerSetting.HeadCount != 2)
		{
			if(pRuntimeParam->curpassNO == 0)
			{
				nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
				step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
					(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
				return step;
			}
			else
			{
				INT8U divideNum = 1;  //must be 720 or 600 DPI.
				INT8U baseGroupNO;
				INT16S StepNozzleBefNobase;
				INT16S StepNozzleAftNobase;
				INT8U order ;
				INT8U headNO_in_group = ((pRuntimeParam->curpassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum)) %
					epsonCaliSetting.printerSetting.head_group;
				INT8U insertedPassNO = GetNoBasePassNO_ForGroupOffsetCheck(divideNum, headNO_in_group,
																		   &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
				INT8U PassNumInSubCycle = (pRuntimeParam->curpassNO - 1) % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);           
				if(PassNumInSubCycle == insertedPassNO - 1)
				{
					return epsonCaliSetting_runtime.stepPass.BaseStep * StepNozzleBefNobase / (epsonCaliSetting.printerSetting.NozzleCount *
																							   epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
				}
				else if(PassNumInSubCycle > insertedPassNO - 1 && PassNumInSubCycle < insertedPassNO - 1 + divideNum)
					return 0;
				else if(PassNumInSubCycle == insertedPassNO - 1 + divideNum)
				{
					return epsonCaliSetting_runtime.stepPass.BaseStep * StepNozzleAftNobase / (epsonCaliSetting.printerSetting.NozzleCount *
																							   epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
				}
				else
				{
					return epsonCaliSetting_runtime.stepPass.BaseStep/epsonCaliSetting.printerSetting.GroupCount_YContinue;
				}
			}
		}
		else
		{
			//only total 2H.
			if(head_group == 1)
			{
				INT8U baseGroupNO;
				baseGroupNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index]/head_group % 
					epsonCaliSetting.printerSetting.GroupCount_YContinue;
				INT8U BaseIsUp = (baseGroupNO == 0 && epsonCaliSetting.printerSetting.GroupCount_YContinue > 1);
				INT8U BaseIsDown = (baseGroupNO != 0 && epsonCaliSetting.printerSetting.GroupCount_YContinue > 1);
				if(BaseIsUp)
				{
					if(pRuntimeParam->curpassNO == 0)
					{
						nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
						step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
							(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
						return step;
					}
					else
						return epsonCaliSetting_runtime.stepPass.BaseStep/epsonCaliSetting.printerSetting.GroupCount_YContinue;
				}
				else if(BaseIsDown)
				{
					if(pRuntimeParam->curpassNO == 0)
						return 0;
					else
						return epsonCaliSetting_runtime.stepPass.BaseStep/epsonCaliSetting.printerSetting.GroupCount_YContinue;
				}
				else
				{
					//for epsonCaliSetting.printerSetting.GroupCount_YInterleave > 1 
					if(pRuntimeParam->curpassNO == 0)
					{
						nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
						step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
							(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
						return step;
					}
					else if(pRuntimeParam->curpassNO == 1)
						return 0;
					else
						return epsonCaliSetting_runtime.stepPass.BaseStep/epsonCaliSetting.printerSetting.GroupCount_YContinue;
				}
			}
			else
			{
				INT8U divideNum = 1;  //must be 720 or 600 DPI.
				INT8U baseGroupNO;
				INT16S StepNozzleBefNobase;
				INT16S StepNozzleAftNobase;
				INT8U * order;
				INT8U baseHeadNO_in_group = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index]%head_group;
				INT8U headNO_in_group = (baseHeadNO_in_group == 0 ? 1:0);
				INT8U insertedPassNO = GetNoBasePassNO_ForGroupOffsetCheck(divideNum, headNO_in_group,
																		   &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,order);
				if(insertedPassNO == 0)
				{
					if(pRuntimeParam->curpassNO == 0)
					{
						nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
						INT16S verShift = (epsonCaliSetting.printerSetting.verticalNozzleShift >= 0 ?
										   epsonCaliSetting.printerSetting.verticalNozzleShift :
											   -epsonCaliSetting.printerSetting.verticalNozzleShift );
						if(verShift/epsonCaliSetting.printerSetting.YInterleaveNum >= nozzle)
							nozzle = 0;
						else
							nozzle -= verShift/epsonCaliSetting.printerSetting.YInterleaveNum;
						step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
							(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
						return step;
					}
					else
					{
						if(pRuntimeParam->curpassNO == 1)
							return epsonCaliSetting_runtime.stepPass.BaseStep * StepNozzleAftNobase / (epsonCaliSetting.printerSetting.NozzleCount *
																									   epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
						else
							return epsonCaliSetting_runtime.stepPass.BaseStep/epsonCaliSetting.printerSetting.GroupCount_YContinue;
					}
				}
				else
				{
					if(pRuntimeParam->curpassNO == 0)
					{
						nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
						step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
							(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
						return step;
					}
					else
					{
						if(pRuntimeParam->curpassNO == 1)
							return epsonCaliSetting_runtime.stepPass.BaseStep * StepNozzleBefNobase / (epsonCaliSetting.printerSetting.NozzleCount *
																									   epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
						else
							return epsonCaliSetting_runtime.stepPass.BaseStep * StepNozzleAftNobase / (epsonCaliSetting.printerSetting.NozzleCount *
																									   epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
					}
				}
			}
		}
		break;
	case LeftCheck:
	case RightCheck:
		if(pRuntimeParam->curpassNO == 0)
		{
			nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
			step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
				(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
			return step;
		}
		else
		{
			INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
			INT8U PassNumInSubCycle = (pRuntimeParam->curpassNO - 1) % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
			INT8U baseGroupNO;
			INT16S StepNozzleBefNobase;
			INT16S StepNozzleAftNobase;
			INT8U order;
			INT8U UnitNO = (pRuntimeParam->curpassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
			INT8U colorNO = UnitNO / epsonCaliSetting.printerSetting.YInterleaveNum;
			INT8U InterleaveNO = UnitNO % epsonCaliSetting.printerSetting.YInterleaveNum;
			INT8U headNO_in_group = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[colorNO], InterleaveNO) % epsonCaliSetting.printerSetting.head_group;
			INT8U insertedPassNO = GetNoBasePassNO_ForHorizatalLikeCheck(
																		 divideNum, colorNO, (InterleaveNO == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) ),
																		 headNO_in_group, &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
			
			INT16S Nozzle_Adjust_for_base_changed =0;
			if(PassNumInSubCycle == (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum)-1)
			{
				UnitNO ++;
				if(UnitNO < epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.ColorCount)
				{
                                  INT8U cur_base_head_no;
                                  INT8U next_base_head_no;
                                  INT8U order_next;
                                  INT8U PassNumInSubCycle_next = pRuntimeParam->curpassNO % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
                                  INT8U baseGroupNO_next;
                                  INT16S StepNozzleBefNobase_next;
                                  INT16S StepNozzleAftNobase_next;
                                  INT8U colorNO_next = UnitNO / epsonCaliSetting.printerSetting.YInterleaveNum;
                                  INT8U InterleaveNO_next = UnitNO % epsonCaliSetting.printerSetting.YInterleaveNum;
                                  INT8U headNO_in_group_next = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[colorNO_next], InterleaveNO_next) % epsonCaliSetting.printerSetting.head_group;
                                  INT8U insertedPassNO_next = GetNoBasePassNO_ForHorizatalLikeCheck(
                                              divideNum, colorNO_next, (InterleaveNO_next == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) ),
                                   headNO_in_group_next, &baseGroupNO_next, &StepNozzleBefNobase_next, &StepNozzleAftNobase_next,&order_next);

                                  if(InterleaveNO == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) )
                                          cur_base_head_no = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
                                  else
                                          cur_base_head_no = epsonCaliSetting.colorMap.BaseHeadNo[colorNO];
                                  if(( PassNumInSubCycle_next >= insertedPassNO_next && PassNumInSubCycle_next < insertedPassNO_next + divideNum))
                                  {    					
                                      next_base_head_no = epsonCaliSetting.colorMap.BaseHeadNo[colorNO_next];
                                  }
                                  else
                                  {
                                      if(InterleaveNO_next == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) )
                                        next_base_head_no = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
                                      else
                                        next_base_head_no = epsonCaliSetting.colorMap.BaseHeadNo[colorNO_next];
                                  }
                                  if(order_next==1)
                                      Nozzle_Adjust_for_base_changed = StepNozzleAftNobase*epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
                                  if(cur_base_head_no != next_base_head_no)
                                  {
                                      INT32S YOffset1 = epsonCaliSetting_runtime.yOffset.YOffset[cur_base_head_no][0];
						INT32S YOffset2 = epsonCaliSetting_runtime.yOffset.YOffset[next_base_head_no][0];
						Nozzle_Adjust_for_base_changed += (YOffset2 - YOffset1)*epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
					}
				}
			}
#if defined(MANUFACTURER_LONGKE_FLAT_UV_150)||defined(MANUFACTURER_LONGKE_FLAT_UV_130)
			if(type == LeftCheck)
			{
				if(pRuntimeParam->curpassNO>16)
				{
					if((pRuntimeParam->curpassNO-16)%2 == 0) 
						return 0;
					else
					{
						return  epsonCaliSetting_runtime.stepPass.BaseStep * 360 / 
							(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
					}
				}
			}
#endif
#if defined(MANUFACTURER_LONGKE_FLAT_UV_150)||defined(MANUFACTURER_LONGKE_FLAT_UV_130)
			if(type == LeftCheck)
			{
				insertedPassNO = (insertedPassNO+headNO_in_group)%2;
			}
#endif
			if(PassNumInSubCycle == insertedPassNO - 1)
			{
                            int offset = 0;
                            if(order ==1)
                                offset = (epsonCaliSetting.printerSetting.GroupCount_YContinue-1)*epsonCaliSetting.printerSetting.NozzleCount *epsonCaliSetting.printerSetting.YInterleaveNum;
				return epsonCaliSetting_runtime.stepPass.BaseStep * (StepNozzleBefNobase-offset) / (epsonCaliSetting.printerSetting.NozzleCount *
																						   epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
			}
			else if(PassNumInSubCycle > insertedPassNO - 1 && PassNumInSubCycle < insertedPassNO - 1 + divideNum)
				return 0;
			else if(PassNumInSubCycle == insertedPassNO - 1 + divideNum)
			{
				nozzle = (StepNozzleAftNobase + Nozzle_Adjust_for_base_changed);
				if(nozzle < 0)
					nozzle = 0;
				return epsonCaliSetting_runtime.stepPass.BaseStep * nozzle / 
					(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
			}
			else
			{
				if(insertedPassNO == 0 && PassNumInSubCycle == (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum)-1)
				{
					nozzle = (StepNozzleBefNobase + Nozzle_Adjust_for_base_changed);
					if(nozzle < 0)
						nozzle = 0;
					return epsonCaliSetting_runtime.stepPass.BaseStep * nozzle / (epsonCaliSetting.printerSetting.NozzleCount *
																				  epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
				}
				
				nozzle = (epsonCaliSetting.printerSetting.NozzleCount *epsonCaliSetting.printerSetting.YInterleaveNum + Nozzle_Adjust_for_base_changed);
				if(nozzle < 0)
					nozzle = 0;
				return epsonCaliSetting_runtime.stepPass.BaseStep* nozzle / (epsonCaliSetting.printerSetting.NozzleCount *
																			 epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum);
				
			}
		}
	case BiDirCheck:
		if(pRuntimeParam->curpassNO == 0)
		{
			nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
			step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
				(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
			return step;
		}
#ifdef BI_TEST
		else if(pRuntimeParam->curpassNO%2 == 1)
			return 0;
		else
			return epsonCaliSetting_runtime.stepPass.BaseStep * 
				(epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNo_YContinue)/epsonCaliSetting.printerSetting.GroupCount_YContinue;
#else
else
{
	INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
	if( (pRuntimeParam->curpassNO - 1)  < divideNum)
		return 0;
	else
		return epsonCaliSetting_runtime.stepPass.BaseStep * 
			(epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNo_YContinue)/epsonCaliSetting.printerSetting.GroupCount_YContinue;
}
#endif		
	case HeadYIntervalCheck:
	if(pRuntimeParam->curpassNO == 0)
	{
		nozzle = 0;
		if(epsonCaliSetting.printerSetting.bYIntervalReplaceVertical && epsonCaliSetting.printerSetting.HeadCount == 2)
		{
			INT32S adjustNozzle = 0;
			if(head_group > 1)
			{
				INT8U headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0], 0);
				INT8U lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[0], 0);
				INT32S YOffset1 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead], 0);
				lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead],
											   NOZZLE_LINE_COUNT_PER_HEAD % epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				INT32S YOffset2 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				adjustNozzle = (YOffset2 - YOffset1)*epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
			}
			INT8U HeadNo_InGroup = epsonCaliSetting.printerSetting.BaseColor_HeadIndex % head_group;
			if(adjustNozzle > 0)
				nozzle = (head_group - 1 - HeadNo_InGroup) * adjustNozzle;
			else
				nozzle = HeadNo_InGroup * (- adjustNozzle);
			nozzle /= epsonCaliSetting.printerSetting.YInterleaveNum;
		}
		nozzle += (epsonCaliSetting.printerSetting.NozzleCount - (YINTERVALCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
		step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
		step += epsonCaliSetting_runtime.stepPass.BaseStep *
			(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue)/epsonCaliSetting.printerSetting.GroupCount_YContinue;
		return step;
	}
	else
		return epsonCaliSetting_runtime.stepPass.BaseStep;
	case VerticalAlignCheck:
		if(pRuntimeParam->curpassNO == 0)
		{
			nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (YINTERVALCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
			step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
				(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
			return step;
		}
		else
		{
			return epsonCaliSetting_runtime.stepPass.BaseStep * 
				(epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNo_YContinue)/epsonCaliSetting.printerSetting.GroupCount_YContinue;
		}
	case AngleLeftCheck:
	case AngleRightCheck:
		//need to improve for the case that vertical offset is NOT ZERO.
		if(pRuntimeParam->curpassNO == 0)
		{
			nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (ANGLECHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
			step = epsonCaliSetting_runtime.stepPass.BaseStep * nozzle /
				(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.HeadCount);
			step += epsonCaliSetting_runtime.stepPass.BaseStep *
				(epsonCaliSetting.printerSetting.HeadCount - 1 - epsonCaliSetting.printerSetting.BaseColor_HeadIndex)/epsonCaliSetting.printerSetting.HeadCount;
			return step;
		}
		else
		{
			INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
			if( (pRuntimeParam->curpassNO - 1)  < divideNum - 1)
				return 0;
			else if((pRuntimeParam->curpassNO - 1)  == divideNum - 1)
				return epsonCaliSetting_runtime.stepPass.BaseStep*(epsonCaliSetting.printerSetting.NozzleCount -ANGLECHECK_OVERLAP_NOZZLE)/
					(epsonCaliSetting.printerSetting.HeadCount*epsonCaliSetting.printerSetting.NozzleCount);
			else if((pRuntimeParam->curpassNO - 1) < 2 * divideNum - 1)
				return 0;
			else
				return epsonCaliSetting_runtime.stepPass.BaseStep;
		}
	case HWDynOffsetTest:
		step = epsonCaliSetting_runtime.stepPass.BaseStep * 1 /
			(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.GroupCount_YContinue);
		return step;
	default:
		return 0;
	}
}

INT32S GetNextYBand(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam,
					struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT32S passnum, nozzle;
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	INT8U groupNo_YContinue = epsonCaliSetting.printerSetting.BaseColor_HeadIndex / head_group % 
		epsonCaliSetting.printerSetting.GroupCount_YContinue;
	
	INT32S HeadStepNozzle = epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
	INT32S AllStepNozzle = HeadStepNozzle * epsonCaliSetting.printerSetting.GroupCount_YContinue;
	
	switch (type)
	{
	case DotCheck:
		if(pRuntimeParam->curpassNO != GetPassNum(DotCheck, pPrintParam) - 1)
		{
			INT32S adjustNozzle = 0;
			if(head_group > 1)
			{
				INT8U headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0], 0);
				INT8U lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[0], 0);
				INT32S YOffset1 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead], 0);
				lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead],
											   NOZZLE_LINE_COUNT_PER_HEAD % epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				INT32S YOffset2 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				adjustNozzle = (YOffset2 - YOffset1)*epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
			}
			
			INT8U passCountWhenChanged;
			if(pPrintParam->VSDModel !=2)
				passCountWhenChanged = (NOZZLE_LINE_COUNT_PER_HEAD * epsonCaliSetting.printerSetting.GroupCount_YInterleave) / DOTCHECK_LINECOUNT_PERPASS;
			else
				passCountWhenChanged = (NOZZLE_LINE_COUNT_PER_HEAD * epsonCaliSetting.printerSetting.GroupCount_YInterleave) / DOTCHECK_FIXDOT_LINECOUNT_PERPASS;
			
			INT8U HeadCountWhenChanged = 1;
			if(passCountWhenChanged == 0)
			{
				passCountWhenChanged = 1;
				if(pPrintParam->VSDModel !=2)
					HeadCountWhenChanged = DOTCHECK_LINECOUNT_PERPASS/(NOZZLE_LINE_COUNT_PER_HEAD * epsonCaliSetting.printerSetting.GroupCount_YInterleave);
				else
					HeadCountWhenChanged = DOTCHECK_FIXDOT_LINECOUNT_PERPASS/(NOZZLE_LINE_COUNT_PER_HEAD * epsonCaliSetting.printerSetting.GroupCount_YInterleave);
			}
			
			if( (pRuntimeParam->curpassNO) % passCountWhenChanged == passCountWhenChanged - 1 )
			{
				nozzle = AllStepNozzle + AllStepNozzle/10 + adjustNozzle * HeadCountWhenChanged;
				if(nozzle <0)
					nozzle = 0;
				return nozzle;
			}
			else
			{
				return AllStepNozzle + AllStepNozzle/10;
			}
		}
		else
			return AllStepNozzle;
	case NozzleCheck:
		return AllStepNozzle;
	case StepCheck:
		if((pPrintParam->Option & 0x1) == 0)
		{
			if(pRuntimeParam->curpassNO == 0)
			{
				if( pPrintParam->PassNum == 1)
					passnum = STEPCHECK_BASEPASSNUM;
				else
					passnum = pPrintParam->PassNum;
				nozzle = AllStepNozzle/passnum;
				return nozzle;
			}
			else
				return HeadStepNozzle * (epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNo_YContinue);
		}
		else
		{
			if(pRuntimeParam->curpassNO == 0)
			{
				nozzle = (epsonCaliSetting.printerSetting.NozzleCount - STEPCHECK_DIGITAL_STARTNOZZLE) * epsonCaliSetting.printerSetting.YInterleaveNum;
				nozzle += (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) * HeadStepNozzle;
				return nozzle;
			}
			else
			{
				if(pPrintParam->PassNum == 1)
					passnum = STEPCHECK_BASEPASSNUM;
				else
					passnum = pPrintParam->PassNum;
				
				nozzle = AllStepNozzle / passnum;
				return nozzle;
			}
		}
		//    case StepCheck_Micro:
		//        if(pRuntimeParam->curpassNO != STEPCHECK_MICRO_UNITCOUNT)
		//        {
		//            if(pPrintParam->PassNum == 1)
		//                passnum = STEPCHECK_BASEPASSNUM;
		//            else
		//                passnum = pPrintParam->PassNum;
		//            nozzle = epsonCaliSetting.printerSetting.NozzleCount*epsonCaliSetting.printerSetting.HeadCount/passnum;
		//            return nozzle * epsonCaliSetting.printerSetting.YInterleaveNum;
		//        }
		//        else
		//            return epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum *
		//                (epsonCaliSetting.printerSetting.HeadCount - epsonCaliSetting.printerSetting.BaseColor_HeadIndex);
	case VerticalCheck:
		switch(pRuntimeParam->curpassNO)
		{
		case 0:
			return (epsonCaliSetting.printerSetting.NozzleCount - (INTERLEAVECHECK_TITLE_STARTNOZZLE - TITLE_Y_SPACE))* epsonCaliSetting.printerSetting.YInterleaveNum +
				(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) * HeadStepNozzle;
		case 1:
		default:
			//it is better to consider Vertical offset. However, we assert all color 's vertical offset is less than a head height.
			//  PrinterSetting_Cali.bVerticalValid assure it.
			return HeadStepNozzle;
		}
	case AngleCheck:
		switch(pRuntimeParam->curpassNO)
		{
		case 0:
			return (epsonCaliSetting.printerSetting.NozzleCount - (VERTICALCHECK_TITLE_STARTNOZZLE - TITLE_Y_SPACE))* epsonCaliSetting.printerSetting.YInterleaveNum +
				(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) * HeadStepNozzle;
		case 1:
		case 3:
			return nozzle = HeadStepNozzle * 9 / 10;
		case 2:
			return nozzle = HeadStepNozzle * 11 / 10;
		case 4:
		default:
			return AllStepNozzle;
		}
	case InterleaveCheck:
		if((pPrintParam->Option & 0x10) == 0)
		{
			INT32S adjustNozzle = 0;
			if(head_group > 1)
			{
				INT8U headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0], 0);
				INT8U lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[0], 0);
				INT32S YOffset1 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead], 0);
				lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead],
											   NOZZLE_LINE_COUNT_PER_HEAD % epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				INT32S YOffset2 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
				
				adjustNozzle = (YOffset2 - YOffset1)*epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
			}
			
			switch(pRuntimeParam->curpassNO)
			{
			case 0:
				return (epsonCaliSetting.printerSetting.NozzleCount - (INTERLEAVECHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE))* epsonCaliSetting.printerSetting.YInterleaveNum +
					(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) * HeadStepNozzle;
			case 1:
			default:
				if(pRuntimeParam->curpassNO == GetPassNum(type, pPrintParam) - 1)
					return AllStepNozzle;
				else
				{
					if( (pRuntimeParam->curpassNO - 1) % head_group != head_group - 1 )
					{
						nozzle = AllStepNozzle + adjustNozzle;
						if(nozzle <0)
							nozzle = 0;
						return nozzle;
					}
					else
					{
						nozzle = AllStepNozzle - adjustNozzle * (head_group - 1 );
						if(nozzle <0)
							nozzle = 0;
						return nozzle;
					}
				}
			}
		}
		else
		{
			return AllStepNozzle;
		}
	case GroupOffsetCheck:  //for group number >2, the Y step of the last pass is error.
		if( !epsonCaliSetting.printerSetting.bIntegratedHead || epsonCaliSetting.printerSetting.HeadCount != 2)
		{
			if(pRuntimeParam->curpassNO == 0)
			{
				nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
				nozzle *= epsonCaliSetting.printerSetting.YInterleaveNum;
				return nozzle;
			}
			else
			{
				INT8U divideNum = 1;  //must be 720 or 600 DPI.
				INT8U baseGroupNO;
				INT16S StepNozzleBefNobase;
				INT16S StepNozzleAftNobase;
				INT8U order;
				INT8U headNO_in_group = ((pRuntimeParam->curpassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum)) %
					epsonCaliSetting.printerSetting.head_group;
				INT8U insertedPassNO = GetNoBasePassNO_ForGroupOffsetCheck(divideNum, headNO_in_group,
																		   &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
				INT8U PassNumInSubCycle = (pRuntimeParam->curpassNO - 1) % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
				
				if(PassNumInSubCycle == insertedPassNO - 1)
				{
					return StepNozzleBefNobase;
				}
				else if(PassNumInSubCycle > insertedPassNO - 1 && PassNumInSubCycle < insertedPassNO - 1 + divideNum)
					return 0;
				else if(PassNumInSubCycle == insertedPassNO - 1 + divideNum)
				{
					return StepNozzleAftNobase;
				}
				else
				{
					return HeadStepNozzle;
				}
			}
		}
		else
		{
			//only total 2H.
			if(head_group == 1)
			{
				INT8U baseGroupNO;
				baseGroupNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index]/head_group % 
					epsonCaliSetting.printerSetting.GroupCount_YContinue;
				INT8U BaseIsUp = (baseGroupNO == 0);
				if(BaseIsUp)
				{
					if(pRuntimeParam->curpassNO == 0)
					{
						nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
						nozzle *= epsonCaliSetting.printerSetting.YInterleaveNum;
						return nozzle;
					}
					else
						return HeadStepNozzle;
				}
				else
				{
					if(pRuntimeParam->curpassNO == 0)
						return 0;
					else
						return HeadStepNozzle;
				}
			}
			else
			{
				INT8U divideNum = 1;  //must be 720 or 600 DPI.
				INT8U baseGroupNO;
				INT16S StepNozzleBefNobase;
				INT16S StepNozzleAftNobase;
				INT8U order;
				INT8U baseHeadNO_in_group = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index]%head_group;
				INT8U headNO_in_group = (baseHeadNO_in_group == 0 ? 1:0);
				INT8U insertedPassNO = GetNoBasePassNO_ForGroupOffsetCheck(divideNum, headNO_in_group,
																		   &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
				if(insertedPassNO == 0)
				{
					if(pRuntimeParam->curpassNO == 0)
					{
						nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
						nozzle *= epsonCaliSetting.printerSetting.YInterleaveNum;
						INT16S verShift = (epsonCaliSetting.printerSetting.verticalNozzleShift >= 0 ?
										   epsonCaliSetting.printerSetting.verticalNozzleShift :
											   -epsonCaliSetting.printerSetting.verticalNozzleShift );
						if(verShift >= nozzle)
							nozzle = 0;
						else
							nozzle -= verShift;
						return nozzle;
					}
					else
					{
						if(pRuntimeParam->curpassNO == 1)
							return StepNozzleAftNobase;
						else
							return HeadStepNozzle;
					}
				}
				else
				{
					if( pRuntimeParam->curpassNO == 0)
					{
						nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (LEFTCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
						nozzle *= epsonCaliSetting.printerSetting.YInterleaveNum;
						return nozzle;
					}
					else
					{
						if( pRuntimeParam->curpassNO == 1)
							return StepNozzleBefNobase;
						else
							return StepNozzleAftNobase;
					}
				}
			}
		}
		break;
		
	case LeftCheck:
	case RightCheck:  //for group number >2, the Y step of the last pass is error.
		if(pRuntimeParam->curpassNO == 0)
			return (epsonCaliSetting.printerSetting.NozzleCount - LEFTCHECK_TITLE_STARTNOZZLE)* epsonCaliSetting.printerSetting.YInterleaveNum ;
		else
		{
			INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
			INT8U PassNumInSubCycle = (pRuntimeParam->curpassNO - 1) % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
			INT8U baseGroupNO;
			INT16S StepNozzleBefNobase;
			INT16S StepNozzleAftNobase;
			INT8U order;
			INT8U UnitNO = (pRuntimeParam->curpassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
			INT8U colorNO = UnitNO / epsonCaliSetting.printerSetting.YInterleaveNum;
			INT8U InterleaveNO = UnitNO % epsonCaliSetting.printerSetting.YInterleaveNum;
			INT8U headNO_in_group = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[colorNO], InterleaveNO) % epsonCaliSetting.printerSetting.head_group;
			INT8U insertedPassNO = GetNoBasePassNO_ForHorizatalLikeCheck(
																		 divideNum, colorNO, (InterleaveNO == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) ),
																		 headNO_in_group, &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
			
			INT16S Nozzle_Adjust_for_base_changed =0;
			if(PassNumInSubCycle == (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum)-1)
			{
				UnitNO ++;
				if(UnitNO < epsonCaliSetting.printerSetting.YInterleaveNum * epsonCaliSetting.printerSetting.ColorCount)
				{
					INT8U colorNO_Next = UnitNO / epsonCaliSetting.printerSetting.YInterleaveNum;
					INT8U InterleaveNO_Next = UnitNO % epsonCaliSetting.printerSetting.YInterleaveNum;
					INT8U cur_base_head_no;
					if(InterleaveNO == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) )
						cur_base_head_no = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
					else
						cur_base_head_no = epsonCaliSetting.colorMap.BaseHeadNo[colorNO];
					INT8U next_base_head_no;
					if(InterleaveNO_Next == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) )
						next_base_head_no = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
					else
						next_base_head_no = epsonCaliSetting.colorMap.BaseHeadNo[colorNO_Next];
					if(cur_base_head_no != next_base_head_no)
					{
						INT32S YOffset1 = epsonCaliSetting_runtime.yOffset.YOffset[cur_base_head_no][0];
						INT32S YOffset2 = epsonCaliSetting_runtime.yOffset.YOffset[next_base_head_no][0];
						Nozzle_Adjust_for_base_changed = (YOffset2 - YOffset1)*epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
					}
				}
			}
			if(PassNumInSubCycle == insertedPassNO - 1)
			{
				return StepNozzleBefNobase;
			}
			else if(PassNumInSubCycle > insertedPassNO - 1 && PassNumInSubCycle < insertedPassNO - 1 + divideNum)
				return 0;
			else if(PassNumInSubCycle == insertedPassNO - 1 + divideNum)
			{
				nozzle = (StepNozzleAftNobase + Nozzle_Adjust_for_base_changed);
				if(nozzle < 0)
					nozzle = 0;
				return nozzle;
			}
			else
			{
				if(insertedPassNO == 0 && PassNumInSubCycle == (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum)-1)
				{
					nozzle = (StepNozzleBefNobase + Nozzle_Adjust_for_base_changed);
					if(nozzle < 0)
						nozzle = 0;
					return nozzle;
				}
				nozzle = (HeadStepNozzle + Nozzle_Adjust_for_base_changed);
				if(nozzle < 0)
					nozzle = 0;
				return nozzle;
			}
		}
	case BiDirCheck:
		if(pRuntimeParam->curpassNO == 0)
			return (epsonCaliSetting.printerSetting.NozzleCount - LEFTCHECK_TITLE_STARTNOZZLE)* epsonCaliSetting.printerSetting.YInterleaveNum;
		else
		{
			INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
			if( (pRuntimeParam->curpassNO - 1)  < divideNum)
				return 0;
			else
				return HeadStepNozzle * (epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNo_YContinue);
		}
	case HeadYIntervalCheck:
		if(pRuntimeParam->curpassNO == 0)
		{
			nozzle = 0;
			if(epsonCaliSetting.printerSetting.bYIntervalReplaceVertical && epsonCaliSetting.printerSetting.HeadCount == 2)
			{
				INT32S adjustNozzle = 0;
				if(head_group > 1)
				{
					INT8U headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0], 0);
					INT8U lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[0], 0);
					INT32S YOffset1 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
					
					headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead], 0);
					lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead],
												   NOZZLE_LINE_COUNT_PER_HEAD % epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
					INT32S YOffset2 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
					
					adjustNozzle = (YOffset2 - YOffset1)*epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.eprCaliSetting.yOffset.nDPI;
				}
				INT8U HeadNo_InGroup = epsonCaliSetting.printerSetting.BaseColor_HeadIndex % head_group;
				if(adjustNozzle > 0)
					nozzle = (head_group - 1 - HeadNo_InGroup) * adjustNozzle;
				else
					nozzle = HeadNo_InGroup * (- adjustNozzle);
			}
			return nozzle + (epsonCaliSetting.printerSetting.NozzleCount - YINTERVALCHECK_TITLE_STARTNOZZLE)* epsonCaliSetting.printerSetting.YInterleaveNum +
				(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) * HeadStepNozzle;
		}
		else
			return AllStepNozzle;
	case VerticalAlignCheck:
		if(pRuntimeParam->curpassNO == 0)
		{
			nozzle = (epsonCaliSetting.printerSetting.NozzleCount - (YINTERVALCHECK_TITLE_STARTNOZZLE-TITLE_Y_SPACE));
			nozzle *= epsonCaliSetting.printerSetting.YInterleaveNum;
			return nozzle;
		}
		else
		{
			return (epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNo_YContinue) * HeadStepNozzle;
		}
	case AngleLeftCheck:
	case AngleRightCheck:
		if(pRuntimeParam->curpassNO == 0)
		{
			return (epsonCaliSetting.printerSetting.NozzleCount - (ANGLECHECK_TITLE_STARTNOZZLE - TITLE_Y_SPACE)) * epsonCaliSetting.printerSetting.YInterleaveNum +
				(epsonCaliSetting.printerSetting.HeadCount - 1 - epsonCaliSetting.printerSetting.BaseColor_HeadIndex) *
					epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum;
		}
		else
		{
			INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
			if( (pRuntimeParam->curpassNO - 1)  < divideNum - 1)
				return 0;
			else if( (pRuntimeParam->curpassNO - 1)  == divideNum - 1)
				return (epsonCaliSetting.printerSetting.NozzleCount -ANGLECHECK_OVERLAP_NOZZLE) * epsonCaliSetting.printerSetting.YInterleaveNum;
			else if((pRuntimeParam->curpassNO - 1) < 2*divideNum - 1)
				return 0;
			else
				return epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.YInterleaveNum *
					epsonCaliSetting.printerSetting.HeadCount;
		}
	case HWDynOffsetTest:
		return 1;
	default:
		return 0;
	}
}

INT8U XPhaseModify(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam, INT32U passNO)
{
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	INT8U groupNo_YContinue = epsonCaliSetting.printerSetting.BaseColor_HeadIndex / head_group % 
		epsonCaliSetting.printerSetting.GroupCount_YContinue;
	
	switch (type)
	{
	case LeftCheck:
	case RightCheck:
		if(passNO == 0 )
			return 0;
		else
		{
			INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
			INT8U PassNumInSubCycle = (passNO - 1) % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
			INT8U baseGroupNO;
			INT16S StepNozzleBefNobase;
			INT16S StepNozzleAftNobase;
			INT8U order;
			INT8U UnitNO = (passNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
			INT8U colorNO = UnitNO / epsonCaliSetting.printerSetting.YInterleaveNum;
			INT8U InterleaveNO = UnitNO % epsonCaliSetting.printerSetting.YInterleaveNum;
			INT8U headNO_in_group = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[colorNO], InterleaveNO) % epsonCaliSetting.printerSetting.head_group;
			INT8U insertedPassNO = GetNoBasePassNO_ForHorizatalLikeCheck(
																		 divideNum, colorNO, (InterleaveNO == 0 && ((type == LeftCheck) == epsonCaliSetting.printerSetting.bOriginIsRight) ),
																		 headNO_in_group, &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
			
			
			if( ( PassNumInSubCycle >= insertedPassNO) &&
			   ( PassNumInSubCycle <= insertedPassNO + divideNum))
				return PassNumInSubCycle - insertedPassNO;
			else
				return 0;
		}
	case BiDirCheck:
		if(passNO == 0 )
			return 0;
		else
		{
			INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
			INT8U PassNumInSubCycle = (passNO - 1) % (1 + divideNum);
			if(PassNumInSubCycle >= 1 && PassNumInSubCycle < 1 + divideNum)
				return PassNumInSubCycle - 1;
			else
				return 0;
		}
	case AngleLeftCheck:
	case AngleRightCheck:
		if(passNO == 0 )
			return 0;
		else
		{
			INT8U divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
			INT8U PassNumInSubCycle = (passNO - 1) % divideNum;
			return PassNumInSubCycle;
		}
	default:
		//unplement.
		return 0;
	}
}

INT32U CalcBandX(INT8U divideNum, INT8U encoder_div, INT8U bIsPosDir, INT32U StartPosX, struct EPSON_X_Cali_Runtime* pCurXCali,
				 enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam, INT32U curpassNO)
{
	INT32S band_X = 0;
	
	if(epsonCaliSetting.printerSetting.bOriginIsRight)
	{
		if(bIsPosDir)
		{
			band_X -= pCurXCali->PosDirAllHeadWidth;
			if(epsonCaliSetting_runtime.angleXOffset.bEnable)
				band_X -= epsonCaliSetting_runtime.angleXOffset.PosDirWidthEx;
		}
		else
		{
			band_X -= pCurXCali->NegDirAllHeadWidth;
			if(epsonCaliSetting_runtime.angleXOffset.bEnable)
				band_X -= epsonCaliSetting_runtime.angleXOffset.NegDirWidthEx;
		}
		
		band_X -= pCurXCali->PosDirOriginOffset;
		if(!bIsPosDir)
			band_X -= pCurXCali->NegDirOriginOffset;
		if(epsonCaliSetting_runtime.angleXOffset.bEnable)
		{
			if(bIsPosDir)
				band_X -= epsonCaliSetting_runtime.angleXOffset.PosDirOriginOffset;
			else
				band_X -= epsonCaliSetting_runtime.angleXOffset.NegDirOriginOffset;
		}
		
		//PM's print origin differs with FW. its base point is the leftest nozzle , that is, Y2.
		// But FW base point is the rightest nozzle, that is K1.
		// We calculate Plat space based on PM print origin.
		//  So, we add a printhead width.
		
		// cheney: 20120720, How do it? In new Design, the leftest nozzle may not be XOffset[0][0].
		//  keep on the old code, waiting more testing.
		band_X += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[0][0]);
	}
	else
	{
		band_X += pCurXCali->PosDirOriginOffset;
		band_X += pCurXCali->PosDirAllHeadWidth;
		if(!bIsPosDir)
			band_X += pCurXCali->NegDirOriginOffset;
		if(epsonCaliSetting_runtime.angleXOffset.bEnable)
		{
			if(bIsPosDir)
				band_X += epsonCaliSetting_runtime.angleXOffset.PosDirOriginOffset;
			else
				band_X += epsonCaliSetting_runtime.angleXOffset.NegDirOriginOffset;
		}
		//Utill now, PM print origin is NOT clear for the case that Origin Is left.
		//  so we assure that PM print origin is the rightest nozzle.
	}
	
	if( divideNum == 1)
	{
		band_X += StartPosX;
		band_X *= encoder_div;
	}
	else
	{
		//assert divideNum == encoder_div.
		band_X += StartPosX * encoder_div;
		band_X += XPhaseModify(type, pPrintParam, curpassNO);
	}
	
	return band_X;
}

INT8U GenBandHead(INT8U * pBuff, enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam,
				  struct CaliPatternRuntimeParam * pRuntimeParam)
//struct EPSON_X_Cali_Runtime * pCurXCali, INT16U passNO)
{
	INT32U PatternWidth = GetPatternWidth(type, pPrintParam, pRuntimeParam->curpassNO);
	INT8U bIsPosDir = GetPrintDir(type, pPrintParam, pRuntimeParam->curpassNO);
	INT8U divideNum = ((pPrintParam->Option & 0x6) >> 1) + 1;
	INT32U PatternWidthEx;
	INT8U encoder_div = 1;
	
	DataHeaderType * pHead = (DataHeaderType *)pBuff;
	pHead->header_flag = 'BYHX';
	pHead->header_size = HUGEBUF_ALIGN_SIZE;
	pHead->header_type = BAND_HEAD;
	
	if(PatternWidth >epsonCaliSetting.printerSetting.PrintRegionWidth)
	{
		if(!bIsPosDir)
			pRuntimeParam->PatternShift = PatternWidth - epsonCaliSetting.printerSetting.PrintRegionWidth;
		else
			pRuntimeParam->PatternShift = 0;
		PatternWidth = epsonCaliSetting.printerSetting.PrintRegionWidth;
	}
	else
	{
		pRuntimeParam->PatternShift = 0;
	}
	
	PatternWidthEx = GetPatternWidthEx(pRuntimeParam->pCurXCali, PatternWidth*divideNum, bIsPosDir);
	
	if(pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X / 2 ||
	   pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X *3 /4 / 2)
		encoder_div = 2;
	else if(pPrintParam->DPI == epsonCaliSetting.printerSetting.DPI_X / 4)
		encoder_div = 4;
	else
		encoder_div = 1;
	
	pHead->Inf.band_inf.dir = bIsPosDir ? 1 : 2;
	if( divideNum == 1)
	{
		pHead->Inf.band_inf.band_width = PatternWidthEx * encoder_div;
	}
	else
	{
		//assert divideNum == encoder_div.
		pHead->Inf.band_inf.band_width = (PatternWidthEx + encoder_div -1) / encoder_div * encoder_div;
	}
	pHead->data_bytescnt = CalcBuffSize(pHead->Inf.band_inf.band_width/encoder_div);
	
	pHead->Inf.band_inf.band_X = CalcBandX(divideNum, encoder_div, bIsPosDir, pPrintParam->StartPosX,
										   pRuntimeParam->pCurXCali,type, pPrintParam, pRuntimeParam->curpassNO);
	
	pHead->Inf.band_inf.band_Y = pRuntimeParam->curBandY;
	pHead->Inf.band_inf.step_distance = GetYStep(type, pPrintParam, pRuntimeParam);
#ifdef  MANUFACTURER_DYSS 
	if(pRuntimeParam->curpassNO > 1)
	{
		if((pRuntimeParam->curpassNO<=(2*factoryData.color_num*factoryDataEx.YInterleaveNum)&&type == LeftCheck)||
		type == RightCheck||(pRuntimeParam->curpassNO<=factoryData.color_num*factoryDataEx.YInterleaveNum&&type == GroupOffsetCheck))
		{
		  pHead->Inf.band_inf.step_distance = pHead->Inf.band_inf.step_distance * (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE)/NOZZLE_COUNT_PER_LINE;
		}
  }
#endif	
	pHead->Inf.band_inf.compress_data_bytescnt = pHead->Inf.band_inf.band_width/encoder_div * EPSON_DATA_PER_FIRE;	
	
	PatternWidth = GetPatternWidth(type, pPrintParam, pRuntimeParam->curpassNO+1);
	if(PatternWidth >epsonCaliSetting.printerSetting.PrintRegionWidth)
		PatternWidth = epsonCaliSetting.printerSetting.PrintRegionWidth;
	bIsPosDir = GetPrintDir(type, pPrintParam, pRuntimeParam->curpassNO+1);
	PatternWidthEx = GetPatternWidthEx(pRuntimeParam->pCurXCali, PatternWidth*divideNum, bIsPosDir);
	pHead->Inf.band_inf.next_dir = bIsPosDir ? 1 : 2;
	
	if( divideNum == 1)     
		pHead->Inf.band_inf.next_band_width = PatternWidthEx * encoder_div;
	else
	{
		//assert divideNum == encoder_div.
		pHead->Inf.band_inf.next_band_width = (PatternWidthEx + encoder_div -1) / encoder_div * encoder_div;
	}
	pHead->Inf.band_inf.next_band_X = CalcBandX(divideNum, encoder_div, bIsPosDir, pPrintParam->StartPosX,
												pRuntimeParam->pCurXCali, type, pPrintParam, pRuntimeParam->curpassNO+1);
	
	pHead->Inf.band_inf.compressMode = 4;   //FW not use
	pHead->Inf.band_inf.stepTimeMS = 0x3E8;   //FW not use
#ifdef  MANUFACTURER_DYSS 
	if( pRuntimeParam->curpassNO > 1 &&((pRuntimeParam->curpassNO!=(2*factoryData.color_num*factoryDataEx.YInterleaveNum+1)&&pRuntimeParam->curpassNO!=(2*(factoryData.color_num+factoryData.spot_color1_headnum)*factoryDataEx.YInterleaveNum+1)&&type == LeftCheck) ||type == RightCheck ||type == GroupOffsetCheck))		
		pHead->Inf.band_inf.nextBandY = pHead->Inf.band_inf.band_Y + GetNextYBand(type, pPrintParam, pRuntimeParam)* (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE)/NOZZLE_COUNT_PER_LINE;
	else
#endif
	pHead->Inf.band_inf.nextBandY = pHead->Inf.band_inf.band_Y + GetNextYBand(type, pPrintParam, pRuntimeParam);

	pHead->Inf.band_inf.shiftY = 0;
	pHead->Inf.band_inf.bandResY = 0;
	pHead->Inf.band_inf.bandIndex = 0;
	memset(pHead->Inf.band_inf.dotCount, 0, sizeof(pHead->Inf.band_inf.dotCount));
	pHead->Inf.band_inf.dotCheckSum = 0;
	pHead->Inf.band_inf.GapStartPos = 4096;
	
	pRuntimeParam->PassBuffSize = pHead->data_bytescnt;
	pRuntimeParam->curBandY = pHead->Inf.band_inf.nextBandY;
	pRuntimeParam->curDir = pHead->Inf.band_inf.dir == 1 ? 1 : 0;
	return True;
}

INT8U * GetFireBuff(INT32S xOffset, INT8U * pBuff)
{
	INT8U * pTargetBuf;
	
	pTargetBuf = pBuff + xOffset * epsonCaliSetting.printerSetting.BytesPerFire;
	if(pBuff + xOffset * epsonCaliSetting.printerSetting.BytesPerFire >= (INT8U *)HUGEBUF_END_ADDR)
		pTargetBuf +=  (- HUGEBUF_END_ADDR + HUGEBUF_START_ADDR);
	
	if(pTargetBuf + epsonCaliSetting.printerSetting.BytesPerFire >= (INT8U *)HUGEBUF_END_ADDR)
	{
		runtimeParam.bCacheBuff = True;
		runtimeParam.pTargetBuf = pTargetBuf;
		return runtimeParam.FireBuf;
	}
	else
	{
		return pTargetBuf;
	}
}

INT8U CheckOutOfWidth(INT32S xOffset, INT8U LineIndex, INT8U bLeftDir)
{
	if(bLeftDir)
	{
		if(xOffset >= runtimeParam.AllHeadWidth - runtimeParam.LineOffsetTable[LineIndex] + epsonCaliSetting.printerSetting.PrintRegionWidth)
			return False;
		if(xOffset < runtimeParam.AllHeadWidth - runtimeParam.LineOffsetTable[LineIndex])
			return False;
	}
	else
	{
		if(xOffset >= runtimeParam.LineOffsetTable[LineIndex] + epsonCaliSetting.printerSetting.PrintRegionWidth)
			return False;
		if(xOffset < runtimeParam.LineOffsetTable[LineIndex])
			return False;
	}
	return True;
}

INT8U * GetFireBuffEx(INT32S xOffset, INT8U * pBuff, INT8U LineIndex)
{
	INT8U bLeftDir = runtimeParam.curDir == epsonCaliSetting.printerSetting.bOriginIsRight;
	
	if(!runtimeParam.curDir)
	{
		if(xOffset < runtimeParam.PatternShift)
			return (INT8U*)0;
		xOffset -= runtimeParam.PatternShift;
	}
	if(CheckOutOfWidth(xOffset, LineIndex, bLeftDir))
		return GetFireBuff(xOffset, pBuff);
	else
		return (INT8U*)0;
}
volatile int my_dummy_tick =0;

void FillFireBuff_1H(INT8U LineIndex, INT8U *pFireBuff, INT8U DotSize, INT16U startNozzle, INT16U endNozzle)
{
	if(factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin)
	{
		INT16U i;
		INT32U *pFireBuff_back = (INT32U *)pFireBuff;
		INT32U *pFireBuff_word = (INT32U *)pFireBuff;
		INT32U bitMask;
		INT32U firstMask, secondMask;
		
		bitMask = (1<<LineIndex)+(1<<(LineIndex+16));
		if(LineIndex%2==0)
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				secondMask = 0;
				firstMask = 1<<(LineIndex+16);
				break;
			case 2:
				secondMask = 1<<LineIndex;
				firstMask = 0;
				break;
			case 3:
			default:
				secondMask = 1<<LineIndex;
				firstMask = 1<<(LineIndex+16);
				break;
			} 
		}
		else
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				firstMask = 0;
				secondMask = 1<<LineIndex;
				break;
			case 2:
				firstMask = 1<<(LineIndex+16);
				secondMask = 0;
				break;
			case 3:
			default:
				firstMask = 1<<LineIndex;
				secondMask = 1<<(LineIndex+16);
				break;
			} 
		}
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		INT16U k=0;
		for(i=0; i< endNozzle - startNozzle; i++)
		{
			k = startNozzle+i;
			
			if(LineIndex%2==1)
			{
				
				*(pFireBuff_word+k) &= ~bitMask;
				*(pFireBuff_word+k) |= firstMask;
				*(pFireBuff_word+k)|= secondMask;
			}
			else     
			{
				if(k<96)
					k=96-k;
				else
					k=288-k;      
				*(pFireBuff_word+epsonCaliSetting.printerSetting.NozzleCount-k) &= ~bitMask;
				*(pFireBuff_word+epsonCaliSetting.printerSetting.NozzleCount-k) |= firstMask;
				*(pFireBuff_word+epsonCaliSetting.printerSetting.NozzleCount-k) |= secondMask;
			}
		}
	}
	else
	{
		INT16U i;
		INT16U *pFireBuff_back = (INT16U *)pFireBuff;
		INT16U *pFireBuff_word = (INT16U *)pFireBuff;
		INT16U bitMask;
		INT16U firstMask, secondMask;
		
		bitMask = 1<<LineIndex;
		
		switch(DotSize)
		{
		case 0:
			firstMask = secondMask = 0;
			break;
		case 1:
			firstMask = 0;
			secondMask = 1<<LineIndex;
			break;
		case 2:
			firstMask = 1<<LineIndex;
			secondMask = 0;
			break;
		case 3:
		default:
			firstMask = 1<<LineIndex;
			secondMask = 1<<LineIndex;
			break;
		}
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		pFireBuff_word += startNozzle;

		if(RIPSTAR_PRT_BIT == 2)
		{
			for(i=0; i< endNozzle - startNozzle; i++,pFireBuff_word++)
			{
				*pFireBuff_word &= ~bitMask;
				*pFireBuff_word |= bitMask;
			}
		}
		else
		{
			for(i=0; i< endNozzle - startNozzle; i++,pFireBuff_word++)
			{
				*pFireBuff_word &= ~bitMask;
				*pFireBuff_word |= firstMask;
			}
			pFireBuff_word = pFireBuff_back + epsonCaliSetting.printerSetting.NozzleCount + startNozzle;
			for(i=0; i< endNozzle - startNozzle; i++,pFireBuff_word++)
			{
				*pFireBuff_word &= ~bitMask;
				*pFireBuff_word |= secondMask;
			}
		}
	}
}

void FillFireBuff_2H(INT8U LineIndex, INT8U *pFireBuff, INT8U DotSize, INT16U startNozzle, INT16U endNozzle)
{
	if(factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin)
	{
		INT16U i;
		INT32U bitMask;
		INT32U firstMask, secondMask;
		INT32U *pFireBuff_INT32U = (INT32U *)pFireBuff;
		bitMask = (1<<LineIndex)+(1<<(LineIndex+16));
		if(LineIndex%2==0)
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				secondMask = 0;
				firstMask = 1<<(LineIndex+16);
				break;
			case 2:
				secondMask = 1<<LineIndex;
				firstMask = 0;
				break;
			case 3:
			default:
				secondMask = 1<<LineIndex;
				firstMask = 1<<(LineIndex+16);
				break;
			} 
		}
		else
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				firstMask = 0;
				secondMask = 1<<LineIndex;
				break;
			case 2:
				firstMask = 1<<(LineIndex+16);
				secondMask = 0;
				break;
			case 3:
			default:
				firstMask = 1<<LineIndex;
				secondMask = 1<<(LineIndex+16);
				break;
			} 
		}
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		INT16U k=0;
		for(i=0; i< endNozzle - startNozzle; i++)
		{
			k = startNozzle+i;
			if(LineIndex%2==1)
			{
				*(pFireBuff_INT32U+k) &= ~bitMask;
				*(pFireBuff_INT32U+k) |= firstMask;
				*(pFireBuff_INT32U+k)|= secondMask;
			}
			else
			{
				if(k<96)
					k=96-k;
				else
					k=288-k;
				
				*(pFireBuff_INT32U+epsonCaliSetting.printerSetting.NozzleCount-k) &= ~bitMask;
				*(pFireBuff_INT32U+epsonCaliSetting.printerSetting.NozzleCount-k) |= firstMask;
				*(pFireBuff_INT32U+epsonCaliSetting.printerSetting.NozzleCount-k) |= secondMask;
			}
		}
	}
	else
	{
		INT16U i;
		INT16U bitMask;
		INT16U firstMask, secondMask;
		INT16U *pFireBuff_INT16U = (INT16U *)pFireBuff;
		
		bitMask = 1<<LineIndex;	
		switch(DotSize)
		{
		case 0:
			firstMask = secondMask = 0;
			break;
		case 1:
			firstMask = 0;
			secondMask = 1<<LineIndex;
			break;
		case 2:
			firstMask = 1<<LineIndex;
			secondMask = 0;
			break;
		case 3:
		default:
			firstMask = 1<<LineIndex;
			secondMask = 1<<LineIndex;
			break;
		}
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		pFireBuff_INT16U += startNozzle;
		if(RIPSTAR_PRT_BIT == 2)
		{
			for(i=0; i< endNozzle - startNozzle; i++,pFireBuff_INT16U++)
			{
				*pFireBuff_INT16U &= ~bitMask;
				*pFireBuff_INT16U |= bitMask;
			}
		}
		else
		{
			for(i=0; i< endNozzle - startNozzle; i++,pFireBuff_INT16U++)
			{
				*pFireBuff_INT16U &= ~bitMask;
				*pFireBuff_INT16U |= firstMask;
			}
			pFireBuff_INT16U = (INT16U *)pFireBuff + epsonCaliSetting.printerSetting.NozzleCount + startNozzle;
			for(i=0; i< endNozzle - startNozzle; i++,pFireBuff_INT16U++)
			{
				*pFireBuff_INT16U &= ~bitMask;
				*pFireBuff_INT16U |= secondMask;
			}
		}
	}
}
void FillFireBuff_4H(INT8U LineIndex, INT8U *pFireBuff, INT8U DotSize, INT16U startNozzle, INT16U endNozzle)
{
	if(factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin)
	{
		INT16U i;
		INT32U bitMask = 1<<LineIndex;
		INT32U firstMask, secondMask;
		INT32U *pFireBuff_INT32U = (INT32U *)pFireBuff;
		bitMask = 1<<LineIndex;
		if(LineIndex%2==0)
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				secondMask = 0;
				firstMask = 1<<LineIndex;
				break;
			case 2:
				secondMask = 1<<LineIndex;
				firstMask = 0;
				break;
			case 3:
			default:
				secondMask = 1<<LineIndex;
				firstMask = 1<<LineIndex;
				break;
			}
		}
		else
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				firstMask = 0;
				secondMask = 1<<LineIndex;
				break;
			case 2:
				firstMask = 1<<LineIndex;
				secondMask = 0;
				break;
			case 3:
			default:
				firstMask = 1<<LineIndex;
				secondMask = 1<<LineIndex;
				break;
			}   
		}
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		INT16U k=0;
		for(i=0; i< endNozzle - startNozzle; i++)
		{
			k = startNozzle+i;
			if(LineIndex%2==1)
			{
				
				*(pFireBuff_INT32U+2*k) &= ~bitMask;
				*(pFireBuff_INT32U+2*k) |= firstMask;
				*(pFireBuff_INT32U+2*k+1) &= ~bitMask;
				*(pFireBuff_INT32U+2*k+1)|= secondMask;        
			}
			else
			{
				if(k<96)
					k=96-k;
				else
					k=288-k;     
				*(pFireBuff_INT32U+2*epsonCaliSetting.printerSetting.NozzleCount-2*k) &= ~bitMask;
				*(pFireBuff_INT32U+2*epsonCaliSetting.printerSetting.NozzleCount-2*k) |= firstMask;
				*(pFireBuff_INT32U+2*epsonCaliSetting.printerSetting.NozzleCount-2*k+1) &= ~bitMask;
				*(pFireBuff_INT32U+2*epsonCaliSetting.printerSetting.NozzleCount-2*k+1) |= secondMask;
			}
		}   
	}
	else
	{
		INT16U i;
		INT32U bitMask = 1<<LineIndex;
		INT32U firstMask, secondMask;
		INT32U *pFireBuff_INT32U = (INT32U *)pFireBuff;
		
		switch(DotSize)
		{
		case 0:
			firstMask = secondMask = 0;
			break;
		case 1:
			firstMask = 0;
			secondMask = 1<<LineIndex;
			break;
		case 2:
			firstMask = 1<<LineIndex;
			secondMask = 0;
			break;
		case 3:
		default:
			firstMask = 1<<LineIndex;
			secondMask = 1<<LineIndex;
			break;
		}
#ifdef EPSON_4H
		if(rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND && (*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x10)) <= 0x400)
		{
			//DMA is busy (from DDR2 to FPGA)
			//delay.
			for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
		}      
#endif

#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif


		pFireBuff_INT32U += startNozzle;
		if(RIPSTAR_PRT_BIT == 2)
		{
			for(i=0; i< endNozzle - startNozzle; i++,pFireBuff_INT32U++)
			{
				*pFireBuff_INT32U &= ~bitMask;
				*pFireBuff_INT32U |= bitMask;
			}
		}
		else
		{
			for(i=0; i< endNozzle - startNozzle; i++,pFireBuff_INT32U++)
			{
				*pFireBuff_INT32U &= ~bitMask;
				*pFireBuff_INT32U |= firstMask;
			}
			pFireBuff_INT32U = (INT32U *)pFireBuff + epsonCaliSetting.printerSetting.NozzleCount + startNozzle;
			for(i=0; i< endNozzle - startNozzle; i++,pFireBuff_INT32U++)
			{
				*pFireBuff_INT32U &= ~bitMask;
				*pFireBuff_INT32U |= secondMask;
			}
		}
	}
}
void FillFireBuff_1H_Nozzle(INT8U LineIndex, INT16U NozzleIndex, INT8U *pFireBuff, INT8U DotSize)
{
	if(factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin)
	{
		INT32U bitMask;
		INT32U firstMask, secondMask;
		INT32U *pFireBuff_INT32U = (INT32U *)pFireBuff;
		bitMask = (1<<LineIndex)+(1<<(LineIndex+16));
		if(LineIndex%2==0)
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				secondMask = 0;
				firstMask = 1<<(LineIndex+16);
				break;
			case 2:
				secondMask = 1<<LineIndex;
				firstMask = 0;
				break;
			case 3:
			default:
				secondMask = 1<<LineIndex;
				firstMask = 1<<(LineIndex+16);
				break;
			} 
		}
		else
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				firstMask = 0;
				secondMask = 1<<LineIndex;
				break;
			case 2:
				firstMask = 1<<(LineIndex+16);
				secondMask = 0;
				break;
			case 3:
			default:
				firstMask = 1<<LineIndex;
				secondMask = 1<<(LineIndex+16);
				break;
			} 
		}
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		if(LineIndex%2==1)
		{
			
			*(pFireBuff_INT32U + NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT32U + NozzleIndex) |= firstMask;
			*(pFireBuff_INT32U + NozzleIndex) |= secondMask;
		}
		else
		{
			if(NozzleIndex<96)
				NozzleIndex=96-NozzleIndex;
			else
				NozzleIndex=288-NozzleIndex;
			*(pFireBuff_INT32U +epsonCaliSetting.printerSetting.NozzleCount- NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT32U +epsonCaliSetting.printerSetting.NozzleCount- NozzleIndex) |= firstMask;
			*(pFireBuff_INT32U +epsonCaliSetting.printerSetting.NozzleCount- NozzleIndex) |= secondMask;
		}  
	}
	else
	{
		INT16U bitMask;
		INT16U firstMask, secondMask;
		INT16U *pFireBuff_INT16U = (INT16U *)pFireBuff;
		
		bitMask = 1<<LineIndex;
		switch(DotSize)
		{
		case 0:
			firstMask = secondMask = 0;
			break;
		case 1:
			firstMask = 0;
			secondMask = 1<<LineIndex;
			break;
		case 2:
			firstMask = 1<<LineIndex;
			secondMask = 0;
			break;
		case 3:
		default:
			firstMask = 1<<LineIndex;
			secondMask = 1<<LineIndex;
			break;
		}
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		if(RIPSTAR_PRT_BIT == 2)
		{
			*(pFireBuff_INT16U+NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT16U + NozzleIndex) |= bitMask;
		}
		else
		{
			*(pFireBuff_INT16U+NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT16U+NozzleIndex) |= firstMask;
			*(pFireBuff_INT16U+epsonCaliSetting.printerSetting.NozzleCount+NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT16U+epsonCaliSetting.printerSetting.NozzleCount+NozzleIndex) |= secondMask;
		}		
	}
}
void FillFireBuff_2H_Nozzle(INT8U LineIndex, INT16U NozzleIndex, INT8U *pFireBuff, INT8U DotSize)
{
	if(factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin)
	{
		INT32U bitMask;
		INT32U firstMask, secondMask;
		INT32U *pFireBuff_INT32U = (INT32U *)pFireBuff;
		bitMask = (1<<LineIndex)+(1<<(LineIndex+16));
		if(LineIndex%2==0)
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				secondMask = 0;
				firstMask = 1<<(LineIndex+16);
				break;
			case 2:
				secondMask = 1<<LineIndex;
				firstMask = 0;
				break;
			case 3:
			default:
				secondMask = 1<<LineIndex;
				firstMask = 1<<(LineIndex+16);
				break;
			} 
		}
		
		else
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				firstMask = 0;
				secondMask = 1<<LineIndex;
				break;
			case 2:
				firstMask = 1<<(LineIndex+16);
				secondMask = 0;
				break;
			case 3:
			default:
				firstMask = 1<<LineIndex;
				secondMask = 1<<(LineIndex+16);
				break;
			} 
		}
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		if(LineIndex%2==1)
		{
			
			*(pFireBuff_INT32U + NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT32U + NozzleIndex) |= firstMask;
			*(pFireBuff_INT32U + NozzleIndex) |= secondMask;
		}
		else
		{
			if(NozzleIndex<96)
				NozzleIndex=96-NozzleIndex;
			else
				NozzleIndex=288-NozzleIndex;
			*(pFireBuff_INT32U +epsonCaliSetting.printerSetting.NozzleCount- NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT32U +epsonCaliSetting.printerSetting.NozzleCount- NozzleIndex) |= firstMask;
			*(pFireBuff_INT32U +epsonCaliSetting.printerSetting.NozzleCount- NozzleIndex) |= secondMask;
		}
	}
	else
	{
		INT16U bitMask;
		INT16U firstMask, secondMask;
		INT16U *pFireBuff_INT16U = (INT16U *)pFireBuff;
		
		bitMask = 1<<LineIndex;	
		switch(DotSize)
		{
		case 0:
			firstMask = secondMask = 0;
			break;
		case 1:
			firstMask = 0;
			secondMask = 1<<LineIndex;
			break;
		case 2:
			firstMask = 1<<LineIndex;
			secondMask = 0;
			break;
		case 3:
		default:
			firstMask = 1<<LineIndex;
			secondMask = 1<<LineIndex;
			break;
		}
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		if(RIPSTAR_PRT_BIT == 2)
		{
			*(pFireBuff_INT16U + NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT16U + NozzleIndex) |= bitMask;
		}
		else
		{
			*(pFireBuff_INT16U + NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT16U + NozzleIndex) |= firstMask;
			*(pFireBuff_INT16U + epsonCaliSetting.printerSetting.NozzleCount + NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT16U + epsonCaliSetting.printerSetting.NozzleCount + NozzleIndex) |= secondMask;
		}		
	}
}
void FillFireBuff_4H_Nozzle(INT8U LineIndex, INT16U NozzleIndex, INT8U *pFireBuff, INT8U DotSize)
{
	if(factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin)
	{
		INT32U bitMask= 1<<LineIndex;
		INT32U firstMask, secondMask;
		INT32U *pFireBuff_INT32U = (INT32U *)pFireBuff;
		if(LineIndex%2==0)
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				secondMask = 0;
				firstMask = 1<<LineIndex;
				break;
			case 2:
				secondMask = 1<<LineIndex;
				firstMask = 0;
				break;
			case 3:
			default:
				secondMask = 1<<LineIndex;
				firstMask = 1<<LineIndex;
				break;
			}
		}   
		else
		{
			switch(DotSize)
			{
			case 0:
				firstMask = secondMask = 0;
				break;
			case 1:
				firstMask = 0;
				secondMask = 1<<LineIndex;
				break;
			case 2:
				firstMask = 1<<LineIndex;
				secondMask = 0;
				break;
			case 3:
			default:
				firstMask = 1<<LineIndex;
				secondMask = 1<<LineIndex;
				break;
			}   
		}
		
		
#ifdef HEAD_RICOH_G4
		while (!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif

		if(LineIndex%2==1)
		{
			
			*(pFireBuff_INT32U + 2*NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT32U + 2*NozzleIndex) |= firstMask;
			*(pFireBuff_INT32U + 2*NozzleIndex+1) &= ~bitMask;
			*(pFireBuff_INT32U + 2*NozzleIndex+1)|= secondMask;
		}
		else
		{
			if(NozzleIndex<96)
				NozzleIndex=96-NozzleIndex;
			else
				NozzleIndex=288-NozzleIndex;
			*(pFireBuff_INT32U + 2*epsonCaliSetting.printerSetting.NozzleCount- 2*NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT32U + 2*epsonCaliSetting.printerSetting.NozzleCount- 2*NozzleIndex) |= firstMask;
			*(pFireBuff_INT32U + 2*epsonCaliSetting.printerSetting.NozzleCount - 2*NozzleIndex+1) &= ~bitMask;
			*(pFireBuff_INT32U + 2*epsonCaliSetting.printerSetting.NozzleCount - 2*NozzleIndex+1) |= secondMask;
		}
		
	}
	else
	{
		INT32U bitMask = 1<<LineIndex;
		INT32U firstMask, secondMask;
		INT32U *pFireBuff_INT32U = (INT32U *)pFireBuff;
		
		switch(DotSize)
		{
		case 0:
			firstMask = secondMask = 0;
			break;
		case 1:
			firstMask = 0;
			secondMask = 1<<LineIndex;
			break;
		case 2:
			firstMask = 1<<LineIndex;
			secondMask = 0;
			break;
		case 3:
		default:
			firstMask = 1<<LineIndex;
			secondMask = 1<<LineIndex;
			break;
		}
#ifdef EPSON_4H
		if(rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND && (*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x10)) <= 0x400)
		{
			//DMA is busy (from DDR2 to FPGA)
			//delay.
			for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
		}      
#endif

#ifdef HEAD_RICOH_G4
		while(!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif
		
		if(RIPSTAR_PRT_BIT == 2)
		{
			*(pFireBuff_INT32U + NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT32U + NozzleIndex) |= bitMask;
		}
		else
		{
			*(pFireBuff_INT32U + NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT32U + NozzleIndex) |= firstMask;
			*(pFireBuff_INT32U + epsonCaliSetting.printerSetting.NozzleCount + NozzleIndex) &= ~bitMask;
			*(pFireBuff_INT32U + epsonCaliSetting.printerSetting.NozzleCount + NozzleIndex) |= secondMask;
		}		
	}
}

void FillVerLineEx(INT8U LineIndex, INT32S xOffset, INT8U * pBuff, INT8U DotSize, INT16U startNozzle, INT16U endNozzle)
{
	INT8U * pFireBuff = GetFireBuffEx(xOffset, pBuff, LineIndex);
	
	if(pFireBuff==0)
		return;
	
	if(!epsonCaliSetting.printerSetting.bHeadElectricDir)
	{
		INT16U backup;
		backup = startNozzle;
		startNozzle = epsonCaliSetting.printerSetting.NozzleCount - endNozzle;
		endNozzle = epsonCaliSetting.printerSetting.NozzleCount - backup;
	}
#ifdef CLOSE_NOZZLE
	if(startNozzle < CLOSE_NOZZLE_NUM)
		startNozzle = CLOSE_NOZZLE_NUM;
	if(endNozzle > epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM)
		endNozzle = epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM;
#endif
	switch(epsonCaliSetting.printerSetting.BusWidth/8)
	{
	case 1:
	default:
		FillFireBuff_1H(LineIndex, pFireBuff, DotSize, startNozzle, endNozzle);
		break;
	case 2:
		FillFireBuff_2H(LineIndex, pFireBuff, DotSize, startNozzle, endNozzle);
		break;
	case 4:
		FillFireBuff_4H(LineIndex, pFireBuff, DotSize, startNozzle, endNozzle);
		break;
	}
}

void FillVerLine(INT8U LineIndex, INT32S xOffset, INT8U * pBuff, INT8U DotSize)
{
	INT8U * pFireBuff = GetFireBuffEx(xOffset, pBuff, LineIndex);
	
	if(pFireBuff==0)
		return;
#ifdef CLOSE_NOZZLE
	switch(epsonCaliSetting.printerSetting.BusWidth/8)
	{
	case 1:
	default:
		FillFireBuff_1H(LineIndex, pFireBuff, DotSize, CLOSE_NOZZLE_NUM, epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM);
		break;
	case 2:
		FillFireBuff_2H(LineIndex, pFireBuff, DotSize, CLOSE_NOZZLE_NUM, epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM);
		break;
	case 4:
		FillFireBuff_4H(LineIndex, pFireBuff, DotSize, CLOSE_NOZZLE_NUM, epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM);
		break;
	}
#else	
	switch(epsonCaliSetting.printerSetting.BusWidth/8)
	{
	case 1:
	default:
		FillFireBuff_1H(LineIndex, pFireBuff, DotSize, 0, epsonCaliSetting.printerSetting.NozzleCount);
		break;
	case 2:
		FillFireBuff_2H(LineIndex, pFireBuff, DotSize, 0, epsonCaliSetting.printerSetting.NozzleCount);
		break;
	case 4:
		FillFireBuff_4H(LineIndex, pFireBuff, DotSize, 0, epsonCaliSetting.printerSetting.NozzleCount);
		break;
	}
#endif	
}

void FillHorLine(INT8U LineIndex, INT16U NozzleIndex, INT32S xOffset, INT32S xLen, INT8U * pBuff, INT8U DotSize)
{
	INT8U * pFireBuff;
	INT32S i;
	
#ifdef CLOSE_NOZZLE
	if(NozzleIndex < CLOSE_NOZZLE_NUM || NozzleIndex > (epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM))
		return;
#endif
	if(!epsonCaliSetting.printerSetting.bHeadElectricDir)
	{
		NozzleIndex = epsonCaliSetting.printerSetting.NozzleCount - 1 - NozzleIndex;
	}
	for(i=0; i<xLen; i++)
	{
		pFireBuff = GetFireBuffEx(xOffset+i, pBuff, LineIndex);
		
		if(pFireBuff==0)
			return;
		
		switch(epsonCaliSetting.printerSetting.BusWidth/8)
		{
		case 1:
		default:
			FillFireBuff_1H_Nozzle(LineIndex, NozzleIndex, pFireBuff, DotSize);
			break;
		case 2:
			FillFireBuff_2H_Nozzle(LineIndex, NozzleIndex, pFireBuff, DotSize);
			break;
		case 4:
			FillFireBuff_4H_Nozzle(LineIndex, NozzleIndex, pFireBuff, DotSize);
			break;
		}
	}
}

void FillNozzle(INT8U LineIndex, INT16U NozzleIndex, INT32S xOffset, INT8U * pBuff, INT8U DotSize)
{
	INT8U * pFireBuff;
	
	pFireBuff = GetFireBuffEx(xOffset, pBuff, LineIndex);
	
	if(pFireBuff==0)
		return;
	
#ifdef CLOSE_NOZZLE
	if(NozzleIndex < CLOSE_NOZZLE_NUM || NozzleIndex > (epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM))
		return;
#endif
	if(!epsonCaliSetting.printerSetting.bHeadElectricDir)
	{
		NozzleIndex = epsonCaliSetting.printerSetting.NozzleCount - 1 - NozzleIndex;
	}
	switch(epsonCaliSetting.printerSetting.BusWidth/8)
	{
	case 1:
	default:
		FillFireBuff_1H_Nozzle(LineIndex, NozzleIndex, pFireBuff, DotSize);
		break;
	case 2:
		FillFireBuff_2H_Nozzle(LineIndex, NozzleIndex, pFireBuff, DotSize);
		break;
	case 4:
		FillFireBuff_4H_Nozzle(LineIndex, NozzleIndex, pFireBuff, DotSize);
		break;
	}
}
void FillHorLine_GAP(INT8U LineIndex, INT16U NozzleIndex, INT32S xOffset, INT32S xLen, INT8U * pBuff, INT8U DotSize,INT8U point_num)
{
	INT32U i = 0;
	for(i = 0; i < xLen ; i += point_num)
	{
		FillNozzle(LineIndex, NozzleIndex, xOffset + i, pBuff, DotSize);
	}
}
//here, LineIndex is LineIndex in bus width.
INT8U DrawASCIIChar_Internal(INT8U * pBuff, INT32S xOffset, INT8U LineIndex, INT16U NozzleIndex, INT8U DotSize,
							 INT8U CharWidth, INT8U CharHeight, INT8U *pCharBitmap, INT8U bXReserved, INT8U bYReserved, INT8U InterleaveNum, INT8U InterleavePhase,
							 INT8U xScale, INT8U yScale) //InterleaveNum must be 1,2.
{
	INT16U i,j;
	INT8U filled = False, clipped = False;
	INT8U pixelPage, pixelOffset, bitIndex;
	INT8U * pixel;
	for(i=0; i<CharHeight*yScale/InterleaveNum; i++)
	{
		if(NozzleIndex+i >= epsonCaliSetting.printerSetting.NozzleCount)
		{
			clipped = True;
			break;
		}
		else
			filled = True;
		for(j=0; j<CharWidth*xScale; j++)
		{
			pixelPage = i*InterleaveNum/(8*yScale);
			pixelOffset = j/xScale;
			bitIndex = (((i*InterleaveNum)%(8*yScale)) + InterleavePhase)/yScale;
			
			if(!bYReserved)
			{
				if(!bXReserved)
					pixel = pCharBitmap + pixelPage*CharWidth + pixelOffset;
				else
					pixel = pCharBitmap + pixelPage*CharWidth + (CharWidth - 1 - pixelOffset);
			}
			else
			{
				if(!bXReserved)
					pixel = pCharBitmap + (CharHeight/8 - 1 - pixelPage)*CharWidth + pixelOffset;
				else
					pixel = pCharBitmap + (CharHeight/8 - 1 - pixelPage)*CharWidth + (CharWidth - 1 - pixelOffset);
				bitIndex = 7 - bitIndex;
			}
			
			if((1<<bitIndex) & *pixel)
			{
				FillNozzle(LineIndex, NozzleIndex+i, xOffset+j, pBuff, DotSize);
			}
		}
	}
	if(!filled) return 0;
	else if(clipped) return 1;
	else return 2;
}

INT8U DrawASCIIString(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct EPSON_X_Cali_Runtime * pCurXCali, INT32S xOffset,
					  INT16U NozzleIndex, INT8U DotSize, INT8U *pString, INT8U bDirIsPos, INT16U CharWidth)
{
	INT16U slen = strlen((const char *)pString), Y_DPI;
	INT8U ch, xReserve = False, yReserve = False;
	INT8U filled = False, clipped = False;
	INT8U i,ret,charWidth, charHeight, xScale, yScale;
	INT16S xShift[NOZZLE_LINE_COUNT_PER_HEAD];
	
#ifdef CLOSE_NOZZLE
	if(NozzleIndex < CLOSE_NOZZLE_NUM)
		NozzleIndex = CLOSE_NOZZLE_NUM;
	if(NozzleIndex >  epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM)
		NozzleIndex = epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM;	
#endif
	charWidth = CharWidth * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	xScale = (charWidth+ASCII_WIDTH-1)/ASCII_WIDTH;
	if(xScale ==0)
		xScale = 1;
	charWidth = xScale * ASCII_WIDTH;
	
	Y_DPI = epsonCaliSetting.printerSetting.DPI_Y * epsonCaliSetting.printerSetting.YInterleaveNum_InHead / 
		epsonCaliSetting.printerSetting.YInterleaveNum;
	charHeight = CharWidth * ASCII_HEIGHT/ASCII_WIDTH *Y_DPI /epsonCaliSetting.printerSetting.DPI_X;
	yScale = (charHeight+ASCII_HEIGHT-1)/ASCII_HEIGHT;
	if(yScale ==0)
		yScale = 1;
	charHeight = yScale * ASCII_HEIGHT;
	
	if((epsonCaliSetting.printerSetting.bOriginIsRight && bDirIsPos) ||
	   (!epsonCaliSetting.printerSetting.bOriginIsRight && !bDirIsPos))
	{
		xReserve = True;
		xOffset += (slen - 1) * charWidth;
	}
	xShift[0] = 0;
	for( i = 1; i<epsonCaliSetting.printerSetting.YInterleaveNum_InHead; i++)
	{
		if(bDirIsPos)
		{
			xShift[i] = (pCurXCali->xPosDirOffset.XOffset[epsonCaliSetting.printerSetting.BaseColor_HeadIndex][epsonCaliSetting.printerSetting.BaseColor_LineIndex[0]] -
						 pCurXCali->xPosDirOffset.XOffset[epsonCaliSetting.printerSetting.BaseColor_HeadIndex][epsonCaliSetting.printerSetting.BaseColor_LineIndex[i]]);
		}
		else
		{
			xShift[i] = (pCurXCali->xNegDirOffset.XOffset[epsonCaliSetting.printerSetting.BaseColor_HeadIndex][epsonCaliSetting.printerSetting.BaseColor_LineIndex[i]] -
						 pCurXCali->xNegDirOffset.XOffset[epsonCaliSetting.printerSetting.BaseColor_HeadIndex][epsonCaliSetting.printerSetting.BaseColor_LineIndex[0]]);
		}
		if(!epsonCaliSetting.printerSetting.bOriginIsRight)
			xShift[i] = -xShift[i];
	}
	
	while(* pString != 0)
	{
		ch = (INT8U)*pString;	
		if (ch < 30)
			ch = 30;
		else if (ch > 129)
			ch = 129;
		
		ch -= 30;
		
		pString ++;
		
		for( i =0; i<epsonCaliSetting.printerSetting.YInterleaveNum_InHead; i++)
		{
			ret = DrawASCIIChar_Internal(pBuff, xOffset + xShift[i]/(((pPrintParam->Option & 0x6) >> 1) + 1),
										 epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[epsonCaliSetting.printerSetting.BaseColor_HeadIndex][epsonCaliSetting.printerSetting.BaseColor_LineIndex[i]],
										 NozzleIndex, DotSize,
										 ASCII_WIDTH, ASCII_HEIGHT, &ASC_Bmp_Tab[ch*16], xReserve, yReserve,
										 epsonCaliSetting.printerSetting.YInterleaveNum_InHead, i, xScale, yScale);
			if(ret > 0) filled = 1;
			if(ret < 2) clipped = 1;
		}
		
		if(xReserve)
			xOffset -= charWidth;
		else
			xOffset += charWidth;
	}
	if(!filled) return 0;
	else if(clipped) return 1;
	else return 2;
}

//
INT8U GenDotCheckBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_linenum, group_line, headindex, lineindex;
	INT8U cur_lineNo;
	INT8U i,j;
	INT32S k,l,m,n, xOffset, xLenPerLine;
	INT32S percent,cur_percent_1,cur_percent_0,cur_dot_count;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;

	INT16U percent_nozzle_max = 0;

	if(epsonCaliSetting.printerSetting.NozzleCount/2 < 100)
		percent_nozzle_max = 100;
	else
		percent_nozzle_max = 200;
	
	if(pPrintParam->VSDModel !=2)
		cur_linenum = DOTCHECK_LINECOUNT_PERPASS;
	else
		cur_linenum = DOTCHECK_FIXDOT_LINECOUNT_PERPASS;
	
	INT8U bY_Up_In_Group = True;
	if(head_group > 1)
	{
		headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0], 0);
		lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[0], 0);
		INT32S YOffset1 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
		
		headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead], 0);
		lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead],
									   NOZZLE_LINE_COUNT_PER_HEAD % epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
		INT32S YOffset2 = epsonCaliSetting_runtime.yOffset.YOffset[headindex][lineindex];
		
		if(YOffset1 < YOffset2)
			bY_Up_In_Group = False;
	}
#ifdef NEW_DOT_CHECK 	
	if(pRuntimeParam->curpassNO == GetPassNum(DotCheck, pPrintParam) - 1)
	{ 		
		for(k = 0; k < epsonCaliSetting.printerSetting.YInterleaveNum; k++)
		{
			for(i = 0; i < epsonCaliSetting.printerSetting.GroupCount_YContinue; i++)
			{
				for(j = 0; j<epsonCaliSetting.printerSetting.ColorCount; j++)
				{
					xOffset = 0;
					headindex = GetHeadIndexWithInterleave(i, factoryDataEx.m_nColorOrder[j],k);
					lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[j], k);
					
					cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headindex][lineindex];
					
					
					if( epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset += (pRuntimeParam->pCurXCali->PosDirAllHeadWidth -
									pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]) ;
					else
						xOffset += (pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]) ;
					for(m=0; m<pRuntimeParam->pCurXCali->PosDirAllHeadWidth*2; m++)
						FillVerLine(cur_lineNo, xOffset + m, pBuff, 3);
					
				}
			}
		}
	}
	else
	{
#endif    
		if(pPrintParam->VSDModel ==2)
		{
			xLenPerLine = ((36+1+36+36+36+1+36) + (90+36));
			for(i=0; i<epsonCaliSetting.printerSetting.GroupCount_YContinue; i++)
			{
				for(j=0; j<cur_linenum; j++)
				{
					cur_lineNo = pRuntimeParam->curpassNO * cur_linenum + j;
					if(cur_lineNo/epsonCaliSetting.printerSetting.YInterleaveNum >= epsonCaliSetting.printerSetting.ColorCount)
						continue ;
					
					headindex = GetHeadIndexWithInterleave(i, factoryDataEx.m_nColorOrder[cur_lineNo / epsonCaliSetting.printerSetting.YInterleaveNum], 
														   cur_lineNo % epsonCaliSetting.printerSetting.YInterleaveNum);
					lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[cur_lineNo/epsonCaliSetting.printerSetting.YInterleaveNum],
												   cur_lineNo % epsonCaliSetting.printerSetting.YInterleaveNum);
					
					cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headindex][lineindex];
					
					if(bY_Up_In_Group)
						xOffset = i*72 + j*xLenPerLine;
					else
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - i)*72 + j*xLenPerLine;
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset += (pRuntimeParam->pCurXCali->PosDirAllHeadWidth -
									pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]);
					else
						xOffset += (pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]);
					
					xOffset += 36;
					FillVerLine(cur_lineNo, xOffset, pBuff, 3);
					
					xOffset += 1+36;
					for(k=0; k<36; k++)
						FillVerLine(cur_lineNo, xOffset + k, pBuff, 3);
					
					xOffset += 36+36;
					FillVerLine(cur_lineNo, xOffset, pBuff, 3);
					
					xOffset += 1+36;
					for(k=0; k<4; k++)
					{
						for(l=0; l<epsonCaliSetting.printerSetting.NozzleCount/4; l++)
						{
							if(k == 0)
								percent = (percent_nozzle_max - l*2);
							else if(k == 1)
								percent = (percent_nozzle_max - (epsonCaliSetting.printerSetting.NozzleCount/4-(l+1))*2);
							else if(k == 2)
							{
								if(l%2 == 0)
									percent = (percent_nozzle_max - l*2);
								else
									percent = (percent_nozzle_max - (epsonCaliSetting.printerSetting.NozzleCount/4-(l+1))*2);
							}
							else
							{
								if(l%2==0)
									percent = (percent_nozzle_max - (l/2)*2);
								else
									percent = (percent_nozzle_max - ((l/2+(epsonCaliSetting.printerSetting.NozzleCount/4/2+1))*2));
							}
							
							percent *= 0x10000;
							cur_dot_count = 0;
							for(m=0; m<90; m++)
							{
								cur_percent_1 = (cur_dot_count+1)*percent_nozzle_max*0x10000/(m+1);
								cur_percent_0 = (cur_dot_count)*percent_nozzle_max*0x10000/(m+1);
								if(cur_percent_1 - percent <= percent - cur_percent_0)
								{
									FillNozzle(cur_lineNo, k*epsonCaliSetting.printerSetting.NozzleCount/4+l,
											   (xOffset + m), pBuff, 3);
									cur_dot_count++;
								}
							}
						}
					}
				}
			}	
		}
		else
		{
			xLenPerLine = ((6+3*6)*3+(36+1+36+36+36+1+36)*3+(90+36)*(3+3*6));
			for(i=0; i<epsonCaliSetting.printerSetting.GroupCount_YContinue; i++)
			{
				for(j=0; j<cur_linenum; j++)
				{
					cur_lineNo = pRuntimeParam->curpassNO * cur_linenum + j;
					if(cur_lineNo/epsonCaliSetting.printerSetting.YInterleaveNum >= epsonCaliSetting.printerSetting.ColorCount)
						continue ;
					
					headindex = GetHeadIndexWithInterleave(i, factoryDataEx.m_nColorOrder[cur_lineNo/epsonCaliSetting.printerSetting.YInterleaveNum],
														   cur_lineNo % epsonCaliSetting.printerSetting.YInterleaveNum);
					lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[cur_lineNo/epsonCaliSetting.printerSetting.YInterleaveNum],
												   cur_lineNo % epsonCaliSetting.printerSetting.YInterleaveNum);
					
					cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headindex][lineindex];
					
					if(bY_Up_In_Group)
						xOffset = i*72 + j*xLenPerLine;
					else
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - i)*72 + j*xLenPerLine;
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset += (pRuntimeParam->pCurXCali->PosDirAllHeadWidth -
									pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]);
					else
						xOffset += (pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]);
					
					for(k=0; k<6; k++)
					{
						INT8U dotsize[3];
						switch(k)
						{
						default:
						case 0: dotsize[0] = 3,dotsize[1] = 2,dotsize[2] = 1; break;
						case 1: dotsize[0] = 3,dotsize[1] = 1,dotsize[2] = 2; break;
						case 2: dotsize[0] = 2,dotsize[1] = 3,dotsize[2] = 1; break;
						case 3: dotsize[0] = 2,dotsize[1] = 1,dotsize[2] = 3; break;
						case 4: dotsize[0] = 1,dotsize[1] = 3,dotsize[2] = 2; break;
						case 5: dotsize[0] = 1,dotsize[1] = 2,dotsize[2] = 3; break;
						}
						for(m=0; m<epsonCaliSetting.printerSetting.NozzleCount/3; m++)
						{
							for(n=0; n<3;n++)
								FillHorLine(cur_lineNo, m*3+n, xOffset+k*3, 3, pBuff, dotsize[n]);
						}
						for(m=0; m<3; m++)
						{
							for(n=0; n<3;n++)
								FillVerLine(cur_lineNo, xOffset+6*3+k*3*3+m*3+n, pBuff, dotsize[n]);
						}
					}
					
					xOffset += 6*3 + 6*3*3;
					for(m=0; m<3; m++)
					{
						xOffset += 36;
						FillVerLine(cur_lineNo, xOffset, pBuff, 3-m);
						
						xOffset += 1+36;
						for(k=0; k<36; k++)
							FillVerLine(cur_lineNo, xOffset+k, pBuff, 3-m);
						
						xOffset += 36+36;
						FillVerLine(cur_lineNo, xOffset, pBuff, 3-m);
						
						xOffset += 1+36;
					}
					
					for(n=0; n<3+3*6; n++)
					{
						INT8U dotsize_yx[3][3];
						if(n<3)
							memset(dotsize_yx, 3-n, sizeof(dotsize_yx));
						else
						{
							switch((n-3)%6)
							{
							case 0: dotsize_yx[0][0] = 3,dotsize_yx[0][1] = 2,dotsize_yx[0][2] = 1; break;
							case 1: dotsize_yx[0][0] = 3,dotsize_yx[0][1] = 1,dotsize_yx[0][2] = 2; break;
							case 2: dotsize_yx[0][0] = 2,dotsize_yx[0][1] = 3,dotsize_yx[0][2] = 1; break;
							case 3: dotsize_yx[0][0] = 2,dotsize_yx[0][1] = 1,dotsize_yx[0][2] = 3; break;
							case 4: dotsize_yx[0][0] = 1,dotsize_yx[0][1] = 3,dotsize_yx[0][2] = 2; break;
							case 5: dotsize_yx[0][0] = 1,dotsize_yx[0][1] = 2,dotsize_yx[0][2] = 3; break;
							}
							
							switch((n-3)/6)
							{
							case 0:
							default:
								for(k=0; k<3; k++)
								{
									dotsize_yx[1][0] = dotsize_yx[2][0] = dotsize_yx[0][0];
									dotsize_yx[1][1] = dotsize_yx[2][1] = dotsize_yx[0][1];
									dotsize_yx[1][2] = dotsize_yx[2][2] = dotsize_yx[0][2];
								}
								break;
							case 1:
								for(k=0; k<3; k++)
								{
									dotsize_yx[1][1] = dotsize_yx[2][2] = dotsize_yx[0][0];
									dotsize_yx[1][2] = dotsize_yx[2][0] = dotsize_yx[0][1];
									dotsize_yx[1][0] = dotsize_yx[2][1] = dotsize_yx[0][2];
								}
								break;
							case 2:
								for(k=0; k<3; k++)
								{
									dotsize_yx[1][2] = dotsize_yx[2][1] = dotsize_yx[0][0];
									dotsize_yx[1][0] = dotsize_yx[2][2] = dotsize_yx[0][1];
									dotsize_yx[1][1] = dotsize_yx[2][0] = dotsize_yx[0][2];
								}
								break;
							}
						}
						for(k=0; k<4; k++)
						{
							for(l=0; l<epsonCaliSetting.printerSetting.NozzleCount/4; l++)
							{
								if(k==0)
									percent = (percent_nozzle_max - l*2);
								else if(k==1)
									percent = (percent_nozzle_max - (epsonCaliSetting.printerSetting.NozzleCount/4-(l+1))*2);
								else if(k==2)
								{
									if(l%2==0)
										percent = (percent_nozzle_max - l*2);
									else
										percent = (percent_nozzle_max - (epsonCaliSetting.printerSetting.NozzleCount/4-(l+1))*2);
								}
								else
								{
									if(l%2==0)
										percent = (percent_nozzle_max - (l/2)*2);
									else
										percent = (percent_nozzle_max - ((l/2+(epsonCaliSetting.printerSetting.NozzleCount/4/2+1))*2));
								}
								
								percent *= 0x10000;
								cur_dot_count = 0;
								for(m=0; m<90; m++)
								{
									cur_percent_1 = (cur_dot_count+1)*percent_nozzle_max*0x10000/(m+1);
									cur_percent_0 = (cur_dot_count)*percent_nozzle_max*0x10000/(m+1);
									if(cur_percent_1 - percent <= percent - cur_percent_0)
									{
										FillNozzle(cur_lineNo, k*epsonCaliSetting.printerSetting.NozzleCount/4+l, (xOffset+m), pBuff, dotsize_yx[l%3][cur_dot_count%3]);
										cur_dot_count++;
									}
								}
							}
						}
						xOffset += 90+36;
					}
				}
			}	
		}
#ifdef NEW_DOT_CHECK 			
	}
#endif	
	return True;
}

INT8U GenCheckNozzleBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U group_line, headindex_0, headindex, lineindex, bGroupYContinue0IsLeft, bGroupYInterleave0IsLeft, bHead0InGroupIsLeft;
	INT8U cur_lineNo;
	INT8U i,j, dotsize;
	INT32S k,l,m, n, xOffset, xLenPerLine;
	
	if(pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
#ifdef MANUFACTURER_DYSS
	INT32S xOffset_1 = 0,xOffset_color = 0;
	INT8U x = 0;
	xOffset = 0;
	xLenPerLine = CHECKNOZZLE_NOZZLE_LINEWIDTH/epsonCaliSetting.printerSetting.YInterleaveNum*pPrintParam->DPI/PATTERN_BASE_DPI;
	for(m = 0; m < CHECKNOZZLE_MAX_STEP; m++)
	{
		for(k = 0; k < epsonCaliSetting.printerSetting.YInterleaveNum; k++)
		{
			for(i = 0; i < epsonCaliSetting.printerSetting.GroupCount_YContinue; i++)
			{
				for(j = 0; j<epsonCaliSetting.printerSetting.ColorCount; j++)
				{
					headindex = GetHeadIndexWithInterleave(i, factoryDataEx.m_nColorOrder[j],k);
					lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[j], k);
					
					cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headindex][lineindex];
					

					if( epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_color = (pRuntimeParam->pCurXCali->PosDirAllHeadWidth -
									pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]) ;
					else
						xOffset_color = (pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]) ;

					if(j < 4)
					{
						for(x = 0;x <= m;x++)
						{
							for(n = x; n < NOZZLE_COUNT_PER_LINE;n+=(m+1) )
							{
								FillHorLine(cur_lineNo, n, xOffset + xOffset_color, xLenPerLine, pBuff, dotsize);
							}
							xOffset += xLenPerLine;
						}
					}
					else
					{
						for(x = 0;x <= m;x++)
						{
							for(n = x; n < NOZZLE_COUNT_PER_LINE;n+=(m+1) )
							{
								FillHorLine(cur_lineNo, n, xOffset_1 + xOffset_color, xLenPerLine, pBuff, dotsize);
							}
							xOffset_1 += xLenPerLine;
						}
					}

				}
			}
		}
	}
#else	
	xLenPerLine = (CHECKNOZZLE_STEPNOZZLE + 1)*(CHECKNOZZLE_NOZZLE_LINEWIDTH*pPrintParam->DPI/PATTERN_BASE_DPI);
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	group_line = NOZZLE_LINE_COUNT_PER_HEAD * head_group;
	
	bGroupYContinue0IsLeft = False;
	if(epsonCaliSetting.printerSetting.GroupCount_YContinue >1)
	{
		headindex_0 = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0],0);
		headindex = GetHeadIndexWithInterleave(1, factoryDataEx.m_nColorOrder[0],0);
		if( pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex_0][0] >
		   pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][0])
			bGroupYContinue0IsLeft = True;
	}
	bGroupYInterleave0IsLeft = False;
	if(epsonCaliSetting.printerSetting.GroupCount_YInterleave >1)
	{
		headindex_0 = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0],0);
		headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0],1);
		if( pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex_0][0] >
		   pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][0])
			bGroupYInterleave0IsLeft = True;
	}
	bHead0InGroupIsLeft = False;
	if(epsonCaliSetting.printerSetting.head_group >1)
	{
		if( pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[0][0] >
		   pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[1][0])
			bHead0InGroupIsLeft = True;
	}
	for(k = 0; k < epsonCaliSetting.printerSetting.YInterleaveNum; k++)
	{
		for(i = 0; i < epsonCaliSetting.printerSetting.GroupCount_YContinue; i++)
		{
			for(j = 0; j<epsonCaliSetting.printerSetting.ColorCount; j++)
			{
				headindex = GetHeadIndexWithInterleave(i, factoryDataEx.m_nColorOrder[j],k);
				lineindex = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[j], k);
				
				cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headindex][lineindex];
				
				//the order is, nozzle line inside head, head inside group, Y interleave group, Y continue group.
#ifndef HEAD_RICOH_G4	//老逻辑可能对ricoh 有问题待验证
				if( (bGroupYContinue0IsLeft && epsonCaliSetting.printerSetting.bOriginIsRight) ||
				   (!bGroupYContinue0IsLeft && !epsonCaliSetting.printerSetting.bOriginIsRight))
					xOffset =  (2 - 1 - i % 2) * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
				else
					xOffset = (i % 2) * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
				
#else	//新逻辑不适用于epson 4h  
				if( (bGroupYContinue0IsLeft && epsonCaliSetting.printerSetting.bOriginIsRight) ||
				   (!bGroupYContinue0IsLeft && !epsonCaliSetting.printerSetting.bOriginIsRight))
					xOffset =  (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - i) * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
				else
					xOffset = (i) * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
#endif
				if( (bGroupYInterleave0IsLeft && epsonCaliSetting.printerSetting.bOriginIsRight) ||
				   (!bGroupYInterleave0IsLeft && !epsonCaliSetting.printerSetting.bOriginIsRight))
					xOffset += epsonCaliSetting.printerSetting.GroupCount_YInterleave - 1 - 
						k % epsonCaliSetting.printerSetting.GroupCount_YInterleave;
				else
					xOffset += k % epsonCaliSetting.printerSetting.GroupCount_YInterleave;
				
				//however, currently, I do NOT consider the order of head inside group.
				xOffset = (xOffset *  group_line + j*epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
						   k / epsonCaliSetting.printerSetting.GroupCount_YInterleave)* xLenPerLine;
				
				if( epsonCaliSetting.printerSetting.bOriginIsRight)
					xOffset += (pRuntimeParam->pCurXCali->PosDirAllHeadWidth -
								pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]) ;
				else
					xOffset += (pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][lineindex]) ;
				
				FillVerLine(cur_lineNo, xOffset, pBuff, dotsize);
				for(n = 0; n < CHECKNOZZLE_STEPNOZZLE; n++)
				{
					for(l = 0; l < (CHECKNOZZLE_NOZZLE_LINEWIDTH*pPrintParam->DPI/PATTERN_BASE_DPI); l++)
					{
						xOffset ++;
						for(m = 0; m < (epsonCaliSetting.printerSetting.NozzleCount - 2*CLOSE_NOZZLE_NUM)/CHECKNOZZLE_STEPNOZZLE; m++)
							FillNozzle(cur_lineNo, m * CHECKNOZZLE_STEPNOZZLE + n + CLOSE_NOZZLE_NUM, xOffset, pBuff, dotsize);
						if( n < (epsonCaliSetting.printerSetting.NozzleCount - 2*CLOSE_NOZZLE_NUM)%CHECKNOZZLE_STEPNOZZLE)
							FillNozzle(cur_lineNo, m * CHECKNOZZLE_STEPNOZZLE + n + CLOSE_NOZZLE_NUM, xOffset, pBuff, dotsize);
					}
				}
				FillVerLine(cur_lineNo, xOffset, pBuff, dotsize);
			}
		}
	}
#endif	
	return True;
}

INT8U GetDigitalString(INT8U curCount, INT8U totalCount, INT8U *pBuf)
{
	if(curCount == totalCount/2)
	{
		pBuf[0] = '0';
		pBuf[1] = 0;
		return 1;
	}
	else if(curCount < totalCount/2)
	{
		if(epsonCaliSetting.printerSetting.bOriginIsRight)
			pBuf[0] = '+';
		else
			pBuf[0] = '-';
		if(totalCount/2 - curCount >= 10)
		{
			pBuf[1] = '0' + (totalCount/2 - curCount)/10;
			pBuf[2] = '0' + (totalCount/2 - curCount)%10;
			pBuf[3] = 0;
			return 3;
		}
		else
		{
			pBuf[1] = '0' + (totalCount/2 - curCount);
			pBuf[2] = 0;
			return 2;
		}
	}
	else
	{
		if(epsonCaliSetting.printerSetting.bOriginIsRight)
			pBuf[0] = '-';
		else
			pBuf[0] = '+';
		if(curCount - totalCount/2 >= 10)
		{
			pBuf[1] = '0' + (curCount - totalCount/2)/10;
			pBuf[2] = '0' + (curCount - totalCount/2)%10;
			pBuf[3] = 0;
			return 3;
		}
		else
		{
			pBuf[1] = '0' + (curCount - totalCount/2);
			pBuf[2] = 0;
			return 2;
		}
	}
}

INT8U GenYStepBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, headNO;
	INT8U i,j, dotsize, digitalBuff[4], digitalLen;
	INT32S k, xOffset, xOffset_cur, xLenPerLine, shiftNozzle, nozzle;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	if( pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	xLenPerLine = (STEPCHECK_UNITLINEWIDTH*2+STEPCHECK_UNITLINE_INTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	
	if( pPrintParam->PassNum == 1)
		shiftNozzle = (epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM*2)*epsonCaliSetting.printerSetting.GroupCount_YContinue/STEPCHECK_BASEPASSNUM;
	else
		shiftNozzle = (epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM*2)*epsonCaliSetting.printerSetting.GroupCount_YContinue/pPrintParam->PassNum;
	
	if( pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if(epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]) ;
		else
			xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset, STEPCHECK_TITLE_STARTNOZZLE, 3, "Feed Adjustment",
						pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
	}
	
	headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
	
	//for(i=0; i<STEPCHECK_COLORNUM; i++)
	i = 0; //for user model, only print one color
	{
		if(pRuntimeParam->curpassNO == 0)
		{
			cur_lineNo = epsonCaliSetting.printerSetting.YStepColor_Index[i] % NOZZLE_LINE_COUNT_PER_HEAD;
			xOffset = (STEPCHECK_UNITCOUNT * xLenPerLine + STEPCHECK_COLORINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X)* i ;
			if( epsonCaliSetting.printerSetting.bOriginIsRight)
				xOffset += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]);
			else
				xOffset += pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
		}
		else
		{
			cur_lineNo = epsonCaliSetting.printerSetting.YStepColor_Index[i] % NOZZLE_LINE_COUNT_PER_HEAD;
			xOffset = (STEPCHECK_UNITCOUNT * xLenPerLine + STEPCHECK_COLORINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X)* i ;
		}
		
		for(j=0; j<STEPCHECK_UNITCOUNT; j++)
		{
			if(pRuntimeParam->curpassNO == 0)
			{
				for(k= 0; k < STEPCHECK_NOZZLECOUNT; k++)
				{
					FillHorLine(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
								STEPCHECK_STARTNOZZLE + k*STEPCHECK_NOZZLESTEP, xOffset,
								STEPCHECK_UNITLINEWIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
				}
				digitalLen = GetDigitalString(j, STEPCHECK_UNITCOUNT, digitalBuff);
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset+(STEPCHECK_UNITLINEWIDTH*2-digitalLen*STEPCHECK_CHAR_WIDTH)/2 * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
								STEPCHECK_DIGITAL_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
			}
			else
			{
				for(k= 0; k < STEPCHECK_NOZZLECOUNT; k++)
				{
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						nozzle = STEPCHECK_STARTNOZZLE - CLOSE_NOZZLE_NUM + shiftNozzle - (STEPCHECK_UNITCOUNT/2 - j) + k*STEPCHECK_NOZZLESTEP;
					else
						nozzle = STEPCHECK_STARTNOZZLE - CLOSE_NOZZLE_NUM + shiftNozzle + (STEPCHECK_UNITCOUNT/2 - j) + k*STEPCHECK_NOZZLESTEP;
					headNO = nozzle/(epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM*2) * head_group +
						epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
					nozzle = nozzle%(epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM*2) + CLOSE_NOZZLE_NUM;
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]) ;
					else
						xOffset_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
					xOffset_cur += (STEPCHECK_UNITLINEWIDTH - STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					
					FillHorLine(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
								nozzle, xOffset_cur,
								(STEPCHECK_UNITLINEWIDTH + STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
								pBuff, dotsize);
				}
			}
			xOffset += xLenPerLine;
		}
	}
	return True;
}

//Engineer model:
INT8U GenYStepBand_EM(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U lineNo, headNO, cur_lineNo, cur_headNO, passNO, nozzle_step, nozzle_count;
	INT8U i,j, dotsize, digitalBuff[4], digitalLen;
	INT32S k,l, xOffset, xOffset_cur, xLenPerLine, shiftNozzle, nozzle;
	INT32S startNozzle, endNozzle;
	INT8U head_group;
	INT16U NozzleCount_Real = epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM*2;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	if(pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	xLenPerLine = (STEPCHECK_UNITLINEWIDTH*2+STEPCHECK_UNITLINE_INTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	
	if(pPrintParam->PassNum == 1)
		passNO = STEPCHECK_BASEPASSNUM;
	else
		passNO = pPrintParam->PassNum;
	shiftNozzle = NozzleCount_Real*epsonCaliSetting.printerSetting.GroupCount_YContinue/passNO;
	INT32S Nozzle_Pass = NozzleCount_Real*epsonCaliSetting.printerSetting.GroupCount_YContinue*epsonCaliSetting.printerSetting.YInterleaveNum/passNO;
	head_group = epsonCaliSetting.printerSetting.head_group;
	
	if(pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if(epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]) ;
		else
			xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset, STEPCHECK_TITLE_STARTNOZZLE, 3, "Advanced Feed Adjustment",
						pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
	}
#ifdef STEP_ALL_WIDTH
  INT32U len = ((INT32U)(LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX)* X_BASE_RES)/(STEPCHECK_UNITLINEWIDTH*2+STEPCHECK_UNITLINE_INTERVAL)-2;
  for(i=0; i<1; i++)
  {

    {
      cur_lineNo = epsonCaliSetting.printerSetting.YStepColor_Index[i] % NOZZLE_LINE_COUNT_PER_HEAD;
      headNO = epsonCaliSetting.printerSetting.YStepColor_Index[i] / NOZZLE_LINE_COUNT_PER_HEAD;
      
      xOffset = 0;
    }
    if(!epsonCaliSetting.printerSetting.bOriginIsRight)
      xOffset += (STEPCHECK_BAR_WIDTH * passNO *STEPCHECK_COLORNUM + STEPCHECK_COLORINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
    
    for(j=0; j<len; j++)
    {
      if(pRuntimeParam->curpassNO != 0)
      {
        for(l=1; l<10; l++)
        {
          if(l*l >= shiftNozzle/2)
            break;
        }
        if(l*l >= shiftNozzle)
          l--;
        nozzle_step = nozzle_count = l;
        if((pRuntimeParam->curpassNO - 1) % passNO == 0)
        {
          for(l =0; l < passNO - 1; l++)
          {
            for(k= 0; k < nozzle_count; k++)
            {
              nozzle = shiftNozzle*l + k*nozzle_step;
              cur_headNO = nozzle/epsonCaliSetting.printerSetting.NozzleCount * head_group + headNO;
              nozzle = nozzle%epsonCaliSetting.printerSetting.NozzleCount;
              
              if(epsonCaliSetting.printerSetting.bOriginIsRight)
                xOffset_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo]) ;
              else
                xOffset_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo];
              
              FillHorLine(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[cur_headNO][cur_lineNo],
                    nozzle, xOffset_cur,
                    STEPCHECK_UNITLINEWIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
            }
          }
        }
        else
        {
            for(k= 0; k < nozzle_count; k++)
            {
                        if(epsonCaliSetting.printerSetting.bOriginIsRight)
                            nozzle = (passNO - 1) * shiftNozzle  + k*nozzle_step;
                        else
                            nozzle = (passNO - 1) * shiftNozzle  + k*nozzle_step;

              cur_headNO = nozzle/epsonCaliSetting.printerSetting.NozzleCount * head_group + headNO;
              nozzle = nozzle%epsonCaliSetting.printerSetting.NozzleCount;
              
              if(epsonCaliSetting.printerSetting.bOriginIsRight)
                xOffset_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo]) ;
              else
                xOffset_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo];
              xOffset_cur += (STEPCHECK_UNITLINEWIDTH - STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
              
              FillHorLine(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[cur_headNO][cur_lineNo],
                    nozzle, xOffset_cur, (STEPCHECK_UNITLINEWIDTH + STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
                    pBuff, dotsize);
            }
          }
      }
      xOffset += xLenPerLine;
    }
  }
#else
	for(i=0; i<STEPCHECK_COLORNUM; i++)
	{
		if(pRuntimeParam->curpassNO == 0)
		{
			cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
			headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
			xOffset = (STEPCHECK_UNITCOUNT * xLenPerLine + STEPCHECK_COLORINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X)* i ;
			if(epsonCaliSetting.printerSetting.bOriginIsRight)
				xOffset += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]);
			else
				xOffset += pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
		}
		else
		{
			cur_lineNo = epsonCaliSetting.printerSetting.YStepColor_Index[i] % NOZZLE_LINE_COUNT_PER_HEAD;
			headNO = epsonCaliSetting.printerSetting.YStepColor_Index[i] / NOZZLE_LINE_COUNT_PER_HEAD;
			
			xOffset = (STEPCHECK_UNITCOUNT * xLenPerLine + STEPCHECK_COLORINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X)* i ;
		}
		if(!epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset += (STEPCHECK_BAR_WIDTH * passNO *STEPCHECK_COLORNUM + STEPCHECK_COLORINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
		
		for(j=0; j<STEPCHECK_UNITCOUNT; j++)
		{
			if(pRuntimeParam->curpassNO == 0)
			{
				digitalLen = GetDigitalString(j, STEPCHECK_UNITCOUNT, digitalBuff);
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset+(STEPCHECK_UNITLINEWIDTH*2-digitalLen*STEPCHECK_CHAR_WIDTH)/2 * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
								STEPCHECK_DIGITAL_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
			}
			else
			{
#if  0
				for(l =0; l < passNO ; l++)
				{
					nozzle = 180/passNO/2 + 180/passNO *l;
					cur_headNO = nozzle/epsonCaliSetting.printerSetting.NozzleCount * head_group + headNO;
					nozzle = nozzle%epsonCaliSetting.printerSetting.NozzleCount;
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo]) ;
					else
						xOffset_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo];
					xOffset_cur += (STEPCHECK_UNITLINEWIDTH - STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					
					nozzle_count = 3;
					if(l==0)
						nozzle_count = 4;
					for(k=0; k<nozzle_count; k++)
						FillHorLine(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[cur_headNO][cur_lineNo],
									nozzle+k, xOffset_cur+l*9, (STEPCHECK_UNITLINEWIDTH + STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
									pBuff, dotsize);
				}
#else
				for(l=1; l<10; l++)
				{
					if(l*l >= shiftNozzle/2)
						break;
				}
				if(l*l >= shiftNozzle)
					l--;
				nozzle_step = nozzle_count = l;
				if((pRuntimeParam->curpassNO - 1) % passNO == 0)
				{
					for(l =0; l < passNO - 1; l++)
					{
						for(k= 0; k < nozzle_count; k++)
						{
							nozzle = shiftNozzle*l + k*nozzle_step;
							cur_headNO = nozzle/NozzleCount_Real * head_group + headNO;
							nozzle = nozzle%NozzleCount_Real + CLOSE_NOZZLE_NUM;
							
							if(epsonCaliSetting.printerSetting.bOriginIsRight)
								xOffset_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo]) ;
							else
								xOffset_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo];
							
							FillHorLine(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[cur_headNO][cur_lineNo],
										nozzle, xOffset_cur,
										STEPCHECK_UNITLINEWIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
						}
					}
				}
				else
				{
					for(k= 0; k < nozzle_count; k++)
					{
						if(epsonCaliSetting.printerSetting.bOriginIsRight)
							nozzle = (passNO - 1) * shiftNozzle - (STEPCHECK_UNITCOUNT/2 - j) + k*nozzle_step;
						else
							nozzle = (passNO - 1) * shiftNozzle + (STEPCHECK_UNITCOUNT/2 - j) + k*nozzle_step;
						cur_headNO = nozzle/NozzleCount_Real * head_group + headNO;
						nozzle = nozzle%NozzleCount_Real + CLOSE_NOZZLE_NUM;
						
						if(epsonCaliSetting.printerSetting.bOriginIsRight)
							xOffset_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo]) ;
						else
							xOffset_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo];
						xOffset_cur += (STEPCHECK_UNITLINEWIDTH - STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						
						FillHorLine(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[cur_headNO][cur_lineNo],
									nozzle, xOffset_cur, (STEPCHECK_UNITLINEWIDTH + STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
									pBuff, dotsize);
					}
				}
#endif
			}
			xOffset += xLenPerLine;
		}
	}
#ifndef  MANUFACTURER_DYSS 				
	if(pRuntimeParam->curpassNO != 0)
	{
		for(k= 0; k<STEPCHECK_COLORNUM; k++)
		{
			cur_lineNo = epsonCaliSetting.printerSetting.YStepColor_Index[k] % NOZZLE_LINE_COUNT_PER_HEAD;
			headNO = epsonCaliSetting.printerSetting.YStepColor_Index[k] / NOZZLE_LINE_COUNT_PER_HEAD;
			
			if(epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				xOffset = k * passNO * STEPCHECK_BAR_WIDTH* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				xOffset += ((pRuntimeParam->curpassNO - 1) % passNO) * STEPCHECK_BAR_WIDTH* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				xOffset += (STEPCHECK_UNITCOUNT * xLenPerLine + STEPCHECK_COLORINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X)* STEPCHECK_COLORNUM ;
			}
			else
			{
				xOffset = (STEPCHECK_COLORNUM - 1 - k)* passNO * STEPCHECK_BAR_WIDTH* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				xOffset += (passNO - 1 - (pRuntimeParam->curpassNO - 1) % passNO) * STEPCHECK_BAR_WIDTH* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			}
			for(j=0; j<epsonCaliSetting.printerSetting.GroupCount_YContinue; j++)
			{
				cur_headNO = j * head_group + headNO;
				if( epsonCaliSetting.printerSetting.bOriginIsRight)
					xOffset_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo]);
				else
					xOffset_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[cur_headNO][cur_lineNo];
				
				startNozzle = 0;
				endNozzle = passNO * shiftNozzle;
				
				if( j*NozzleCount_Real > startNozzle)
					startNozzle = j*NozzleCount_Real;
				if( (j+1)*NozzleCount_Real < endNozzle)
					endNozzle = (j+1)*NozzleCount_Real;
				
				startNozzle -= j*NozzleCount_Real;
				endNozzle -= j*NozzleCount_Real;
				
				for(i=0; i<STEPCHECK_BAR_WIDTH* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; i++)
				{
					FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[cur_headNO][cur_lineNo],
								  xOffset_cur+i, pBuff, dotsize, startNozzle + CLOSE_NOZZLE_NUM, endNozzle + CLOSE_NOZZLE_NUM);
				}
			}		
		}
	}
#endif
#endif
	return True;
}

INT8U GetDigitalString_MicroYStep(INT32S step, INT8U *pBuf)
{
	INT8U index = 0, totalnumber = 0, len;
	INT32S step_backup;
	if(step > 0)
	{
		pBuf[index] = '+';
		index++;
	}
	else if(step < 0)
	{
		pBuf[index] = '-';
		index++;
		step = -step;
	}
	else
	{
		pBuf[0] = '0';
		pBuf[1] = 0;
		return 1;
	}
	
	step_backup = step;
	while(step != 0)
	{
		step /= 10;
		totalnumber ++;
	}
	
	len = index+totalnumber;
	step = step_backup;
	pBuf[index+totalnumber] = 0;
	while(totalnumber > 0)
	{
		pBuf[index+totalnumber-1] = '0' + step % 10;;
		totalnumber --;
		step /= 10;
	}
	return len;
}

INT8U GenYStepMicroBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, cur_lineNo_base, headNO, headNO_base;
	INT8U i, dotsize, digitalBuff[8], digitalLen;
	INT32S k, xOffset, xOffset_cur, xOffset_base_cur, xLenPerLine, shiftNozzle, nozzle;
	INT32S YStep;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	if(pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	xLenPerLine = (STEPCHECK_UNITLINEWIDTH*2+STEPCHECK_UNITLINE_INTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	
	if(pPrintParam->PassNum == 1)
		shiftNozzle = epsonCaliSetting.printerSetting.NozzleCount*epsonCaliSetting.printerSetting.HeadCount/STEPCHECK_BASEPASSNUM;
	else
		shiftNozzle = epsonCaliSetting.printerSetting.NozzleCount*epsonCaliSetting.printerSetting.HeadCount/pPrintParam->PassNum;
	
	YStep = epsonCaliSetting_runtime.stepPass.BaseStep * shiftNozzle/
		(epsonCaliSetting.printerSetting.NozzleCount * epsonCaliSetting.printerSetting.HeadCount);
	YStep += epsonCaliSetting_runtime.stepPass.PassStep[pPrintParam->PassNum-1];
	YStep = GetYStep_StepCheck_Micro(pRuntimeParam->curpassNO, YStep/shiftNozzle, STEPCHECK_MICRO_UNITCOUNT);
	
	cur_lineNo_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
	headNO_base = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
	for(i=0; i<STEPCHECK_COLORNUM; i++)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.YStepColor_Index[i] % NOZZLE_LINE_COUNT_PER_HEAD;
		if(pRuntimeParam->curpassNO == 0)
			xOffset = xLenPerLine * i;
		else
			xOffset = ((pRuntimeParam->curpassNO - 1) * STEPCHECK_COLORNUM + i)* xLenPerLine;
		
		if(pRuntimeParam->curpassNO != STEPCHECK_MICRO_UNITCOUNT)
		{
			headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
			if(epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				xOffset_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]) ;
				xOffset_base_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][cur_lineNo_base]) ;
			}
			else
			{
				xOffset_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
				xOffset_base_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[headNO_base][cur_lineNo_base];
			}
			if(pRuntimeParam->curpassNO != 0)
			{
				xOffset_cur += STEPCHECK_COLORNUM * xLenPerLine;
				xOffset_base_cur += STEPCHECK_COLORNUM * xLenPerLine;
			}
			if(pRuntimeParam->curpassNO == 0 && i == 0)
			{
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base_cur, STEPCHECK_TITLE_STARTNOZZLE, 3, "Y Micro-Step Calibration",
								pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
			}
			
			for(k= 0; k < STEPCHECK_NOZZLECOUNT; k++)
			{
				FillHorLine(cur_lineNo + headNO * NOZZLE_LINE_COUNT_PER_HEAD, STEPCHECK_STARTNOZZLE + k*STEPCHECK_NOZZLESTEP, xOffset_cur,
							STEPCHECK_UNITLINEWIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
			}
			digitalLen = GetDigitalString_MicroYStep(YStep, digitalBuff);
			DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base_cur+(STEPCHECK_UNITLINEWIDTH*2-digitalLen*STEPCHECK_CHAR_WIDTH)/2 * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
							STEPCHECK_DIGITAL_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
		}
		
		if(pRuntimeParam->curpassNO != 0)
		{
			for(k= 0; k < STEPCHECK_NOZZLECOUNT; k++)
			{
				nozzle = STEPCHECK_STARTNOZZLE + shiftNozzle + k*STEPCHECK_NOZZLESTEP;
				headNO = nozzle/epsonCaliSetting.printerSetting.NozzleCount;
				nozzle = nozzle%epsonCaliSetting.printerSetting.NozzleCount;
				
				if(epsonCaliSetting.printerSetting.bOriginIsRight)
					xOffset_cur = xOffset + (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]) ;
				else
					xOffset_cur = xOffset + pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
				xOffset_cur += (STEPCHECK_UNITLINEWIDTH - STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				FillHorLine(cur_lineNo + headNO * NOZZLE_LINE_COUNT_PER_HEAD, nozzle, xOffset_cur,
							(STEPCHECK_UNITLINEWIDTH + STEPCHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
							pBuff, dotsize);
			}
		}
	}
	return True;
}

INT8U GenAngleCheckBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, lineNo, headNO, headindex_0, headindex;
	INT8U i,j,m, dotsize;
	INT32S k, xOffset, xOffset_cur;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	INT8U bGroupYContinue0IsLeft = False;
	if(epsonCaliSetting.printerSetting.GroupCount_YContinue >1)
	{
		headindex_0 = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0],0);
		headindex = GetHeadIndexWithInterleave(1, factoryDataEx.m_nColorOrder[0],0);
		if( pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex_0][0] >
		   pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][0])
			bGroupYContinue0IsLeft = True;
	}
	INT8U bGroupYInterleave0IsLeft = False;
	if(epsonCaliSetting.printerSetting.GroupCount_YInterleave >1)
	{
		headindex_0 = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0],0);
		headindex = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[0],1);
		if( pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex_0][0] >
		   pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[headindex][0])
			bGroupYInterleave0IsLeft = True;
	}
	INT8U bHead0InGroupIsLeft = False;
	if(epsonCaliSetting.printerSetting.head_group >1)
	{
		if( pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[0][0] >
		   pRuntimeParam->pCurXCali->xPosDirOffset.XOffset[1][0])
			bHead0InGroupIsLeft = True;
	}
	
	if( pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	
	if( pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]);
		else
			xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, VERTICALCHECK_TITLE_STARTNOZZLE,
						3, "Angle Check", pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
	}
	else if(pRuntimeParam->curpassNO < 3)
	{
		for(j = 0; j < epsonCaliSetting.printerSetting.GroupCount; j++)
		{
			INT8U groupNO_YInterleave = j / epsonCaliSetting.printerSetting.GroupCount_YContinue;
			INT8U groupNo_YContinue = j % epsonCaliSetting.printerSetting.GroupCount_YContinue;
			if(epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				if(factoryDataEx.LayoutType & 0x1)
				{
					if(bGroupYContinue0IsLeft)
					{
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) * 
							epsonCaliSetting.printerSetting.GroupCount_YInterleave;
					}
					else
					{
						xOffset = groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
					}
					
					if(bGroupYInterleave0IsLeft)
					{
						xOffset += (epsonCaliSetting.printerSetting.GroupCount_YInterleave - 1 - groupNO_YInterleave);
					}
					else
					{
						xOffset += groupNO_YInterleave;
					}
				}
				else
				{   
					if(bGroupYInterleave0IsLeft)
					{
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YInterleave - 1 - groupNO_YInterleave) * 
							epsonCaliSetting.printerSetting.GroupCount_YContinue;
					}
					else
					{
						xOffset = groupNO_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue;
					}
					
					if(bGroupYContinue0IsLeft)
					{
						xOffset += (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue);
					}
					else
					{
						xOffset += groupNo_YContinue;
					}
				}
			}
			else
			{
				if(factoryDataEx.LayoutType & 0x1)
				{			  
					if(!bGroupYContinue0IsLeft)
					{
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) * 
							epsonCaliSetting.printerSetting.GroupCount_YInterleave;
					}
					else
					{
						xOffset = groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
					}
					if(!bGroupYInterleave0IsLeft)
					{
						xOffset += (epsonCaliSetting.printerSetting.GroupCount_YInterleave - 1 - groupNO_YInterleave);
					}
					else
					{
						xOffset += groupNO_YInterleave;
					}						
				}
				else
				{
					if(!bGroupYInterleave0IsLeft)
					{
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YInterleave - 1 - groupNO_YInterleave) * 
							epsonCaliSetting.printerSetting.GroupCount_YContinue;
					}
					else
					{
						xOffset = groupNO_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue;
					}
					
					if(!bGroupYContinue0IsLeft)
					{
						xOffset += (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue);
					}
					else
					{
						xOffset += groupNo_YContinue;
					}
				}
			}
			xOffset *= (VERTICALCHECK_HEADINTERVAL + VERTICALCHECK_COLORNUM *
						(VERTICALCHECK_COLORINTERVAL + VERTICALCHECK_LINEINTERVAL + VERTICALCHECK_LINECOUNT)) *
				pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			for(i=0; i< epsonCaliSetting.printerSetting.ColorCount; i++)
			{
				INT8U headNO_InGroup = i /(NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				INT8U colorNO_InHead = i %(NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				if(!epsonCaliSetting.printerSetting.bOriginIsRight)
				{	
					headNO_InGroup = (epsonCaliSetting.printerSetting.ColorCount - 1 - i) / (NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
					colorNO_InHead = (epsonCaliSetting.printerSetting.ColorCount - 1 - i) % (NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				}
				INT8U posNO = colorNO_InHead;
				if(bHead0InGroupIsLeft)
					posNO += (epsonCaliSetting.printerSetting.head_group - 1 - headNO_InGroup) * 
						(NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				else
					posNO += headNO_InGroup * (NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				
				xOffset_cur = xOffset + (VERTICALCHECK_COLORINTERVAL + VERTICALCHECK_LINEINTERVAL + VERTICALCHECK_LINECOUNT) * posNO  *
					pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], groupNO_YInterleave);
				headNO = GetHeadIndexWithInterleave(groupNo_YContinue, factoryDataEx.m_nColorOrder[i], groupNO_YInterleave);
				
				if(epsonCaliSetting.printerSetting.bOriginIsRight)
					xOffset_cur += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
				else
					xOffset_cur += pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
				cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
				
				FillVerLine(cur_lineNo, xOffset_cur, pBuff, dotsize);
				
				xOffset_cur += VERTICALCHECK_LINEINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				for(k = 0; k<VERTICALCHECK_LINECOUNT * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; k++)
				{
					FillVerLine(cur_lineNo, xOffset_cur, pBuff, dotsize);
					xOffset_cur ++;
				}
			}
		}
	}
	else
	{
		for(j = 0; j < epsonCaliSetting.printerSetting.GroupCount; j++)
		{
			INT8U groupNO_YInterleave = j / epsonCaliSetting.printerSetting.GroupCount_YContinue;
			INT8U groupNo_YContinue = j % epsonCaliSetting.printerSetting.GroupCount_YContinue;
			
			if(epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				if(factoryDataEx.LayoutType & 0x1)
				{             
					if(!bGroupYContinue0IsLeft)
					{
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) * 
							epsonCaliSetting.printerSetting.GroupCount_YInterleave;
					}
					else
					{
						xOffset = groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
					}
					if(!bGroupYInterleave0IsLeft)
					{
						xOffset += (epsonCaliSetting.printerSetting.GroupCount_YInterleave - 1 - groupNO_YInterleave);
					}
					else
					{
						xOffset += groupNO_YInterleave;
					}                       
				}
				else
				{
					if(!bGroupYInterleave0IsLeft)
					{
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YInterleave - 1 - groupNO_YInterleave) * 
							epsonCaliSetting.printerSetting.GroupCount_YContinue;
					}
					else
					{
						xOffset = groupNO_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue;
					}
					
					if(!bGroupYContinue0IsLeft)
					{
						xOffset += (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue);
					}
					else
					{
						xOffset += groupNo_YContinue;
					}
				}
			}
			else
			{
				if(factoryDataEx.LayoutType & 0x1)
				{
					if(bGroupYContinue0IsLeft)
					{
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue) * 
							epsonCaliSetting.printerSetting.GroupCount_YInterleave;
					}
					else
					{
						xOffset = groupNo_YContinue * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
					}
					
					if(bGroupYInterleave0IsLeft)
					{
						xOffset += (epsonCaliSetting.printerSetting.GroupCount_YInterleave - 1 - groupNO_YInterleave);
					}
					else
					{
						xOffset += groupNO_YInterleave;
					}
				}
				else
				{   
					if(bGroupYInterleave0IsLeft)
					{
						xOffset = (epsonCaliSetting.printerSetting.GroupCount_YInterleave - 1 - groupNO_YInterleave) * 
							epsonCaliSetting.printerSetting.GroupCount_YContinue;
					}
					else
					{
						xOffset = groupNO_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue;
					}
					
					if(bGroupYContinue0IsLeft)
					{
						xOffset += (epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo_YContinue);
					}
					else
					{
						xOffset += groupNo_YContinue;
					}
				}
			}
			xOffset *= (VERTICALCHECK_HEADINTERVAL + VERTICALCHECK_COLORNUM *
						(VERTICALCHECK_COLORINTERVAL + VERTICALCHECK_LINEINTERVAL + VERTICALCHECK_LINECOUNT)) *
				pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			
			for(i=0; i< epsonCaliSetting.printerSetting.ColorCount; i++)
			{
				INT8U headNO_InGroup = (epsonCaliSetting.printerSetting.ColorCount - 1 - i) / (NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				INT8U colorNO_InHead = (epsonCaliSetting.printerSetting.ColorCount - 1 - i) % (NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				if(!epsonCaliSetting.printerSetting.bOriginIsRight)
				{
					headNO_InGroup = i /(NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
					colorNO_InHead = i %(NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				}
				INT8U posNO = colorNO_InHead;
				if(bHead0InGroupIsLeft)
					posNO += (epsonCaliSetting.printerSetting.head_group - 1 - headNO_InGroup) * 
						(NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				else
					posNO += headNO_InGroup * (NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
				posNO = (epsonCaliSetting.printerSetting.ColorCount - 1 - posNO);
				
				xOffset_cur = xOffset + (VERTICALCHECK_COLORINTERVAL + VERTICALCHECK_LINEINTERVAL + VERTICALCHECK_LINECOUNT) * posNO  *
					pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[epsonCaliSetting.printerSetting.ColorCount - 1 - i], groupNO_YInterleave);
				headNO = GetHeadIndexWithInterleave(groupNo_YContinue, 
													factoryDataEx.m_nColorOrder[epsonCaliSetting.printerSetting.ColorCount - 1 - i], groupNO_YInterleave);
				
				if(!epsonCaliSetting.printerSetting.bOriginIsRight)
					xOffset_cur += (pCurXCali->NegDirAllHeadWidth - pCurXCali->xNegDirOffset.XOffset[headNO][lineNo]);
				else
					xOffset_cur += pCurXCali->xNegDirOffset.XOffset[headNO][lineNo];
				cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
				
				xOffset_cur += VERTICALCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				for(k = 0; k<VERTICALCHECK_LINECOUNT * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; k++)
				{
					FillVerLine(cur_lineNo, xOffset_cur, pBuff, dotsize);
					xOffset_cur ++;
				}
				
				xOffset_cur += VERTICALCHECK_LINEINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				FillVerLine(cur_lineNo, xOffset_cur, pBuff, dotsize);
			}
		}
	}
	
	return True;
}
//need improvement, because the current resolution is 150 DPI for ricoh. maybe, we can improve it to 300DPI
INT8U GenVerticalCheckBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, lineNo, headNO, cur_lineNo_base, lineNo_base, headNO_base, colorNO;
	INT8U i,j, dotsize, digitalBuff[4], digitalLen;
	INT32S k, xOffset, xOffset_cur, xOffset_base, xOffset_base_cur, xLenPerLine, nozzle;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	if( pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	
	xLenPerLine = (VERTICAL_CALI_UNITLINEWIDTH*2+VERTICAL_CALI_UNITLINE_INTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	
	if( pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]);
		else
			xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, VERTICALCHECK_TITLE_STARTNOZZLE,
						3, "Vertical Calibration", pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
	}
	else
	{
		colorNO = pRuntimeParam->curpassNO - 1;
		
		lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[colorNO], 0);
		headNO = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[colorNO], 0);
		cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
		lineNo_base = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[epsonCaliSetting.printerSetting.BaseColor_Index], 0);
		headNO_base = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[epsonCaliSetting.printerSetting.BaseColor_Index], 0);
		cur_lineNo_base = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_base][lineNo_base];
		
		if(epsonCaliSetting.printerSetting.bOriginIsRight)
		{
			xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]);
			xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNo_base]);
		}
		else
		{
			xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
			xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNo_base];
		}
		
		if(factoryDataEx.m_nColorOrder[colorNO] >= COLORCODE_SPOTCOLOR0 &&
		   factoryDataEx.m_nColorOrder[colorNO] <= COLORCODE_SPOTCOLOR3)
		{
			k = factoryDataEx.m_nColorOrder[colorNO] - COLORCODE_SPOTCOLOR0;
			k = LCDMenuConfig.SpotColorName[k];
			INT8U * pName = GetPrintedSpotColorName(k);
			
			for(k=0; pName[k] != 0; k++)
			{
				digitalBuff[k] = pName[k];
			}
			digitalBuff[k] = 0;
			digitalLen = k+1;
		}
		else
		{
			digitalBuff[0] = factoryDataEx.m_nColorOrder[colorNO];
			digitalBuff[1] = 0;
			digitalLen = 1;
		}
		
		if(!epsonCaliSetting.printerSetting.bOriginIsRight)
		{
			DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
							(LEFTCHECK_STARTNOZZLE + LEFTCHECK_ENDNOZZLE)/2, 3, digitalBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
			xOffset += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			xOffset_base += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			
			xOffset += VERTICAL_CALI_UNITLINE_INTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			xOffset_base += VERTICAL_CALI_UNITLINE_INTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
		}
		else
		{
			DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base + VERTICAL_CALI_UNITCOUNT*xLenPerLine,
							(LEFTCHECK_STARTNOZZLE + LEFTCHECK_ENDNOZZLE)/2, 3, digitalBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
		}
		
		INT16S startNozzle_base = VERTICAL_CALI_STARTNOZZLE;
		INT16S startNozzle = VERTICAL_CALI_STARTNOZZLE;
		
		INT16S VerShift = epsonCaliSetting_runtime.yOffset.YOffset[headNO][lineNo] -
			epsonCaliSetting_runtime.yOffset.YOffset[headNO_base][lineNo_base];
		startNozzle -= VerShift / epsonCaliSetting.printerSetting.YInterleaveNum_Head;
		
		if(startNozzle < VERTICAL_CALI_UNITCOUNT/2 + 1)
		{
			startNozzle_base += VERTICAL_CALI_UNITCOUNT/2 + 1 - startNozzle;
			startNozzle = VERTICAL_CALI_UNITCOUNT/2 + 1;
		}
		else if(startNozzle + VERTICAL_CALI_NOZZLESTEP*VERTICAL_CALI_NOZZLECOUNT +
				VERTICAL_CALI_UNITCOUNT/2 + 1 > NOZZLE_COUNT_PER_LINE)
		{
			startNozzle_base -= startNozzle + VERTICAL_CALI_NOZZLESTEP*VERTICAL_CALI_NOZZLECOUNT +
				VERTICAL_CALI_UNITCOUNT/2 + 1 - NOZZLE_COUNT_PER_LINE;
			startNozzle = NOZZLE_COUNT_PER_LINE - (VERTICAL_CALI_NOZZLESTEP*VERTICAL_CALI_NOZZLECOUNT +
												   VERTICAL_CALI_UNITCOUNT/2 + 1);
		}
		
		for(j=0; j<VERTICAL_CALI_UNITCOUNT; j++)
		{
			for(k= 0; k < VERTICAL_CALI_NOZZLECOUNT; k++)
			{
				FillHorLine(cur_lineNo_base,
							startNozzle_base + k*VERTICAL_CALI_NOZZLESTEP, xOffset_base + j*xLenPerLine,
							VERTICAL_CALI_UNITLINEWIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
			}
			digitalLen = GetDigitalString(j, VERTICAL_CALI_UNITCOUNT, digitalBuff);
			DrawASCIIString(pBuff, pPrintParam, pCurXCali,
							xOffset_base + j*xLenPerLine+(VERTICAL_CALI_UNITLINEWIDTH*2-digitalLen*STEPCHECK_CHAR_WIDTH)/2 * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
							STEPCHECK_DIGITAL_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
			
			for(k= 0; k < VERTICAL_CALI_NOZZLECOUNT; k++)
			{
				if(epsonCaliSetting.printerSetting.bOriginIsRight)
					nozzle = startNozzle - (VERTICAL_CALI_UNITCOUNT/2 - j) + k*VERTICAL_CALI_NOZZLESTEP;
				else
					nozzle = startNozzle + (VERTICAL_CALI_UNITCOUNT/2 - j) + k*VERTICAL_CALI_NOZZLESTEP;
				
				xOffset_cur = xOffset + j*xLenPerLine;
				xOffset_cur += (VERTICAL_CALI_UNITLINEWIDTH - VERTICAL_CALI_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				FillHorLine(cur_lineNo, nozzle, xOffset_cur,
							(VERTICAL_CALI_UNITLINEWIDTH + VERTICAL_CALI_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
							pBuff, dotsize);
			}
		}
	}
	
	return True;
}

INT8U GenVerticalAlignCheckBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, lineNo, headNO, cur_lineNo_base, lineNo_base, headNO_base, colorNO;
	INT8U i,j,n, dotsize, digitalBuff[4], digitalLen;
	INT32S k, m, xOffset, xOffset_cur, xOffset_base, xOffset_base_cur, xLenPerLine, nozzle;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	if( pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	
	xLenPerLine = (VERTICAL_ALIGN_UNITLINEWIDTH*2+VERTICAL_ALIGN_UNITLINE_INTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	
	if( pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]);
		else
			xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, VERTICALCHECK_TITLE_STARTNOZZLE,
						3, "Vertical Alignment Check", pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
	}
	else
	{
		INT8U baseHeadNO_InGroup = epsonCaliSetting.printerSetting.BaseColor_HeadIndex % epsonCaliSetting.printerSetting.head_group;
		INT8U baseGroupNO_YInterleave = epsonCaliSetting.printerSetting.BaseColor_HeadIndex/
			(epsonCaliSetting.printerSetting.head_group * epsonCaliSetting.printerSetting.GroupCount_YContinue);
		INT8U baseGroupNO_YContinue = (epsonCaliSetting.printerSetting.BaseColor_HeadIndex/
									   epsonCaliSetting.printerSetting.head_group) % epsonCaliSetting.printerSetting.GroupCount_YContinue;
		
		lineNo_base = GetNozzleLineIndex(epsonCaliSetting.printerSetting.BaseColor_ColorCode, baseGroupNO_YInterleave);
		headNO_base = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		cur_lineNo_base = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_base][lineNo_base];
		
		INT8U UnitNO = 0;
		for(i =0; i<epsonCaliSetting.printerSetting.GroupCount_YInterleave; i++)
		{
			for(j=0; j<epsonCaliSetting.printerSetting.head_group; j++)
			{
#ifdef MANUFACTURER_DYSS
		if(COLOR_NUMBER_CALIBRATION>factoryData.color_num&&(j==factoryData.color_num*factoryDataEx.YInterleaveNum/2))
					continue;	
#endif    
				if(i == baseGroupNO_YInterleave && j == baseHeadNO_InGroup)
					continue;
				
				for(n=0; n<NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead; n++)
				{
					colorNO = j * (NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead) + n;
					
					lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[colorNO], i);
					headNO = GetHeadIndexWithInterleave(baseGroupNO_YContinue, factoryDataEx.m_nColorOrder[colorNO], i);
					cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
					
					if( epsonCaliSetting.printerSetting.bOriginIsRight)
					{
						xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]);
						xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNo_base]);
					}
					else
					{
						xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
						xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNo_base];
					}
					
					INT16S startNozzle_base = VERTICAL_CALI_STARTNOZZLE;
					INT16S startNozzle = VERTICAL_CALI_STARTNOZZLE;
					
					for(k= 0; k < VERTICAL_CALI_NOZZLECOUNT; k++)
					{
						for( m = -1; m <= 1; m++)
							FillHorLine(cur_lineNo_base, startNozzle_base + k*VERTICAL_CALI_NOZZLESTEP + m, 
										xOffset_base + UnitNO * xLenPerLine,
										VERTICAL_ALIGN_UNITLINEWIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
					}
					
					for(k= 0; k < VERTICAL_CALI_NOZZLECOUNT; k++)
					{
						nozzle = startNozzle + k*VERTICAL_CALI_NOZZLESTEP;
						
						xOffset_cur = xOffset + UnitNO * xLenPerLine;
						xOffset_cur += (VERTICAL_ALIGN_UNITLINEWIDTH - VERTICAL_ALIGN_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						
						FillHorLine(cur_lineNo, nozzle, xOffset_cur,
									(VERTICAL_ALIGN_UNITLINEWIDTH + VERTICAL_ALIGN_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
									pBuff, dotsize);
					}
					UnitNO ++;
				}
			}
		}
	}
	
	return True;
}


INT8U GenInterleaveCheckBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, cur_lineNo_base, lineNo, lineNo_base, headNO, headNO_base, interleaveUnit;
	INT8U i,j, dotsize;
	INT32S k,l,m, xOffset, xOffset_cur, xOffset_base, nozzle;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	if(pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	
	if( pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]) ;
		else
			xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo] ;
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, INTERLEAVECHECK_TITLE_STARTNOZZLE, 3, "Interleave Check",
						pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
	}
	else
	{
		INT8U passUnitCount;
		if( epsonCaliSetting.printerSetting.bIntegratedHead)
		{
			interleaveUnit = (NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_Head);
			passUnitCount = head_group;
		}
		else
		{
			interleaveUnit = head_group;
			passUnitCount = 1;
		}
		
		if(pRuntimeParam->curpassNO - 1 < epsonCaliSetting.printerSetting.GroupCount_YInterleave * passUnitCount)
		{
			int unitCount = INTERLEAVECHECK_UNITCOUNT;
#ifdef HEAD_RICOH_G4
			unitCount = 1;
#endif
			INT8U groupNO_YInterleave = (pRuntimeParam->curpassNO - 1)/passUnitCount;
			
			//only support YInterleaveNum_Head == 2.
			for(m = 0; m < epsonCaliSetting.printerSetting.GroupCount_YContinue; m++)
			{
				for(i = 0; i < unitCount; i++)
				{
					for(j = 0; j < interleaveUnit; j++)
					{
						int head_group_NO;
						INT8U YInterleaveNo_InHead;
						if( epsonCaliSetting.printerSetting.bIntegratedHead)
						{
							head_group_NO = (pRuntimeParam->curpassNO - 1) % head_group;
							
							if(i == 0)
							{
								lineNo_base = j * 2;
								YInterleaveNo_InHead = 0;
							}
							else if(epsonCaliSetting.printerSetting.YInterleaveNum_InHead == 2)
							{
								if(i == 2)
								{
									lineNo_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[1];
									YInterleaveNo_InHead = 1;
								}
								else
								{
									lineNo_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
									YInterleaveNo_InHead = 0;
								}
							}
							else
							{
								if(i == 2)
								{
									lineNo_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0]/2*2+1;
									YInterleaveNo_InHead = 1;
								}
								else
								{
									lineNo_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0]/2*2;
									YInterleaveNo_InHead = 0;
								}
							}
						}
						else
						{
							head_group_NO = j;
							
							if(i == 0)
							{
								lineNo_base = 0;
								YInterleaveNo_InHead = 0;
							}
							else if(i == 2)
							{
								lineNo_base = 1;
								YInterleaveNo_InHead = 1;
							}
							else
							{
								lineNo_base = 0;
								YInterleaveNo_InHead = 0;
							}
						}
						headNO_base = GetHeadIndexWithInterleave(m, factoryDataEx.m_nColorOrder[epsonCaliSetting.printerSetting.BaseColor_Index], 
																 YInterleaveNo_InHead * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + groupNO_YInterleave);
						if( epsonCaliSetting.printerSetting.bIntegratedHead || i==0)
							headNO_base = headNO_base/head_group*head_group + head_group_NO;
						cur_lineNo_base = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_base][lineNo_base];
						
						if(epsonCaliSetting.printerSetting.bOriginIsRight)
							xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNo_base]);
						else
							xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNo_base];
						
						if( epsonCaliSetting.printerSetting.bIntegratedHead)
						{
							if(i == 1)
							{
								lineNo = j * 2;
								YInterleaveNo_InHead = 0;
							}
							else
							{
								lineNo = j * 2 + 1;
								YInterleaveNo_InHead = 1;
							}
						}
						else
						{
							if(i == 1)
							{
								lineNo = 0;
								YInterleaveNo_InHead = 0;
							}
							else
							{
								lineNo = 1;
								YInterleaveNo_InHead = 1;
							}
						}
#ifdef HEAD_RICOH_G4
						headNO = j;
#else           
						headNO = GetHeadIndexWithInterleave(m, factoryDataEx.m_nColorOrder[(interleaveUnit * head_group_NO + j) *
															epsonCaliSetting.printerSetting.YInterleaveNum_Head / epsonCaliSetting.printerSetting.YInterleaveNum_InHead],
															YInterleaveNo_InHead * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + groupNO_YInterleave);
#endif
						cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
						
						if(epsonCaliSetting.printerSetting.bOriginIsRight)
							xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
						else
							xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
						
						xOffset_base += i * (INTERLEAVECHECK_UNITINTERVAL + interleaveUnit *
											 (INTERLEAVECHECK_COLORINTERVAL + INTERLEAVECHECK_UNITLINEWIDTH *2 + INTERLEAVECHECK_UNITLINE_OVERLAP))
							* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						xOffset_base += j * (INTERLEAVECHECK_COLORINTERVAL + INTERLEAVECHECK_UNITLINEWIDTH *2 + INTERLEAVECHECK_UNITLINE_OVERLAP)
							* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						xOffset += i * (INTERLEAVECHECK_UNITINTERVAL + interleaveUnit *
										(INTERLEAVECHECK_COLORINTERVAL + INTERLEAVECHECK_UNITLINEWIDTH *2 + INTERLEAVECHECK_UNITLINE_OVERLAP))
							* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						xOffset += j * (INTERLEAVECHECK_COLORINTERVAL + INTERLEAVECHECK_UNITLINEWIDTH *2 + INTERLEAVECHECK_UNITLINE_OVERLAP)
							* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						for(k = 0; k < epsonCaliSetting.printerSetting.NozzleCount/INTERLEAVECHECK_NOZZLESTEP; k++)
						{
							for(l = 0; l < INTERLEAVECHECK_NOZZLECOUNT; l++)
							{
								FillHorLine(cur_lineNo_base, k * INTERLEAVECHECK_NOZZLESTEP + l, xOffset_base,
											(INTERLEAVECHECK_UNITLINEWIDTH) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
											pBuff, dotsize);
								FillHorLine(cur_lineNo_base, k * INTERLEAVECHECK_NOZZLESTEP + l,
											xOffset_base + (INTERLEAVECHECK_UNITLINEWIDTH + INTERLEAVECHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
											(INTERLEAVECHECK_UNITLINEWIDTH) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
											pBuff, dotsize);
							}
							if(i==0 || i==3)
								nozzle = INTERLEAVECHECK_NOZZLECOUNT - 1;
							else
								nozzle = INTERLEAVECHECK_NOZZLECOUNT;
							for(l = 0; l < nozzle; l++)
							{
								FillHorLine(cur_lineNo, k * INTERLEAVECHECK_NOZZLESTEP + l,
											xOffset + (INTERLEAVECHECK_UNITLINEWIDTH - INTERLEAVECHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
											(INTERLEAVECHECK_UNITLINE_OVERLAP * 3) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
											pBuff, dotsize);
							}
						}
					}
				}
			}
		}
		else
		{
			//only support YInterleaveNum_Head == 2.
			
			INT8U groupNO_YInterleave;
			for(m = 0; m < epsonCaliSetting.printerSetting.GroupCount_YContinue; m++)
			{
				for(j = 0; j < interleaveUnit; j++)
				{
					int head_group_NO;
					if( epsonCaliSetting.printerSetting.bIntegratedHead)
						head_group_NO = (pRuntimeParam->curpassNO - 1 - epsonCaliSetting.printerSetting.GroupCount_YInterleave * passUnitCount);
					else
						head_group_NO = j;
					
					INT8U colorNO = (interleaveUnit * head_group_NO + j) *
						epsonCaliSetting.printerSetting.YInterleaveNum_Head / epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
					
					for(i = 0; i < epsonCaliSetting.printerSetting.YInterleaveNum; i++)
					{
						headNO = GetHeadIndexWithInterleave(m, factoryDataEx.m_nColorOrder[colorNO], i);
						lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[colorNO], i);
						cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
						
						if(epsonCaliSetting.printerSetting.bOriginIsRight)
							xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
						else
							xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
						
						xOffset += j * (INTERLEAVECHECK_COLORINTERVAL + INTERLEAVECHECK_UNITLINEWIDTH *2 + INTERLEAVECHECK_UNITLINE_OVERLAP)
							* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						for(k = 0; k < epsonCaliSetting.printerSetting.NozzleCount/INTERLEAVECHECK_NOZZLESTEP; k++)
						{
							if(i==0)
							{
								nozzle = INTERLEAVECHECK_NOZZLECOUNT;
								
								for(l = 0; l < INTERLEAVECHECK_NOZZLECOUNT; l++)
								{
									FillHorLine(cur_lineNo, k * INTERLEAVECHECK_NOZZLESTEP + l, xOffset,
												(INTERLEAVECHECK_UNITLINEWIDTH) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
												pBuff, dotsize);
									FillHorLine(cur_lineNo, k * INTERLEAVECHECK_NOZZLESTEP + l,
												xOffset + (INTERLEAVECHECK_UNITLINEWIDTH + INTERLEAVECHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
												(INTERLEAVECHECK_UNITLINEWIDTH) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
												pBuff, dotsize);
								}
							}
							else
							{
								nozzle = INTERLEAVECHECK_NOZZLECOUNT - 1;
								for(l = 0; l < nozzle; l++)
								{
									FillHorLine(cur_lineNo, k * INTERLEAVECHECK_NOZZLESTEP + l,
												xOffset + (INTERLEAVECHECK_UNITLINEWIDTH * (i+1)/(epsonCaliSetting.printerSetting.YInterleaveNum + 1) ) * 
													pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
													(INTERLEAVECHECK_UNITLINE_OVERLAP * 3) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
													pBuff, dotsize);
								}
							}
						}
					}
				}
			}
		}
	}
	
	return True;
}

//unplement until now.
INT8U GenFullInterleaveCheckBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, cur_lineNo_base, headNO;
	INT8U i,j, dotsize;
	INT32S k,l,m, xOffset, xOffset_cur, xOffset_base, nozzle;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	if(pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	
	//    if( pRuntimeParam->curpassNO == 0)
	//    {
	//        cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
	//        headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
	//        if( epsonCaliSetting.printerSetting.bOriginIsRight)
	//            xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]) ;
	//        else
	//            xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo] ;
	//        DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, INTERLEAVECHECK_TITLE_STARTNOZZLE, 3, "Interleave Check",
	//            pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
	//    }
	//    else
	{
		for(m = 0; m < epsonCaliSetting.printerSetting.HeadCount; m++)
		{
			for(i = 0; i < INTERLEAVECHECK_UNITCOUNT; i++)
			{
				for(j = 0; j < INTERLEAVECHECK_COLORNUM; j++)
				{
					if(i == 0)
						cur_lineNo_base = j * 2;
					else if(i == 2)
						cur_lineNo_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[1];
					else
						cur_lineNo_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[m][cur_lineNo_base]) ;
					else
						xOffset_base = pCurXCali->xPosDirOffset.XOffset[m][cur_lineNo_base];
					
					if(i == 1)
						cur_lineNo = j * 2;
					else
						cur_lineNo = j * 2 + 1;
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[m][cur_lineNo]) ;
					else
						xOffset = pCurXCali->xPosDirOffset.XOffset[m][cur_lineNo];
					
					xOffset_base += i * (INTERLEAVECHECK_UNITINTERVAL + INTERLEAVECHECK_COLORNUM *
										 (INTERLEAVECHECK_COLORINTERVAL + INTERLEAVECHECK_UNITLINEWIDTH *2 + INTERLEAVECHECK_UNITLINE_OVERLAP))
						* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					xOffset_base += j * (INTERLEAVECHECK_COLORINTERVAL + INTERLEAVECHECK_UNITLINEWIDTH *2 + INTERLEAVECHECK_UNITLINE_OVERLAP)
						* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					xOffset += i * (INTERLEAVECHECK_UNITINTERVAL + INTERLEAVECHECK_COLORNUM *
									(INTERLEAVECHECK_COLORINTERVAL + INTERLEAVECHECK_UNITLINEWIDTH *2 + INTERLEAVECHECK_UNITLINE_OVERLAP))
						* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					xOffset += j * (INTERLEAVECHECK_COLORINTERVAL + INTERLEAVECHECK_UNITLINEWIDTH *2 + INTERLEAVECHECK_UNITLINE_OVERLAP)
						* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					for(k = 0; k < epsonCaliSetting.printerSetting.NozzleCount/INTERLEAVECHECK_NOZZLESTEP; k++)
					{
						for(l = 0; l < INTERLEAVECHECK_NOZZLECOUNT; l++)
						{
							FillHorLine(cur_lineNo_base + m*NOZZLE_LINE_COUNT_PER_HEAD, k * INTERLEAVECHECK_NOZZLESTEP + l, xOffset_base,
										(INTERLEAVECHECK_UNITLINEWIDTH) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
										pBuff, dotsize);
							FillHorLine(cur_lineNo_base + m*NOZZLE_LINE_COUNT_PER_HEAD, k * INTERLEAVECHECK_NOZZLESTEP + l,
										xOffset_base + (INTERLEAVECHECK_UNITLINEWIDTH + INTERLEAVECHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
										(INTERLEAVECHECK_UNITLINEWIDTH) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
										pBuff, dotsize);
						}
						if(i==0 || i==3)
							nozzle = INTERLEAVECHECK_NOZZLECOUNT - 1;
						else
							nozzle = INTERLEAVECHECK_NOZZLECOUNT;
						for(l = 0; l < nozzle; l++)
						{
							FillHorLine(cur_lineNo + m*NOZZLE_LINE_COUNT_PER_HEAD, k * INTERLEAVECHECK_NOZZLESTEP + l,
										xOffset + (INTERLEAVECHECK_UNITLINEWIDTH - INTERLEAVECHECK_UNITLINE_OVERLAP) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
										(INTERLEAVECHECK_UNITLINE_OVERLAP * 3) * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X,
										pBuff, dotsize);
						}
					}
				}
			}
		}
	}
	
	return True;
}

INT8U GetGroupString(INT8U groupNO, INT8U headNOInGroup, INT8U colorCode, INT8U InterleaveNO_InHead, INT8U * pBuff)
{
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	INT8U index =0;
	if(epsonCaliSetting.printerSetting.GroupCount >1)
	{
		if(head_group == 1)
		{
			pBuff[index] = 'H';
			index++;
		}
		else
		{
			pBuff[index] = 'G';
			index++;
		}
		pBuff[index] = '1' + groupNO % 10 ;
		index++;
	}
	if(colorCode >= COLORCODE_SPOTCOLOR0 && colorCode <= COLORCODE_SPOTCOLOR3)
	{
		INT8U k = colorCode - COLORCODE_SPOTCOLOR0;
		k = LCDMenuConfig.SpotColorName[k];
		INT8U * pName = GetPrintedSpotColorName(k);
		
		for(k=0; pName[k] != 0; k++)
		{
			pBuff[index] = pName[k];
			index ++;
		}
	}
	else
	{
		pBuff[index] = colorCode;
		index++;
	}
	if(epsonCaliSetting.printerSetting.YInterleaveNum_InHead > 1)
	{
		pBuff[index] = '1' + InterleaveNO_InHead % 10 ;
		index++;
	}
	pBuff[index] = 0;
	return index;
}

INT8U GenLeftRightAlignBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam, INT8U bLeft)
{
	INT8U cur_lineNo, lineNo, InterleaveNO, groupNO, headNO, colorNO, colorCode, UnitNO, phaseNO;
	INT8U lineNO_base, headNO_base, pass_NO_in_sub_cycle;
	INT8U i,j, dotsize, digitalBuff[8], sub_titleBuff[8];
	INT32S xOffset, xOffset_cur, xOffset_base, xLenPerLine;
	INT8U divideNum =0, UnitCount = GetUnitCount( (bLeft?LeftCheck:RightCheck) , pPrintParam);
	INT8S temp;
	char pStr[0x80];
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	if(pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	xLenPerLine = (LEFTCHECK_LINECOUNT + LEFTCHECK_LINEINTERVAL + LEFTCHECK_COLORINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
	pass_NO_in_sub_cycle = (pRuntimeParam->curpassNO - 1) % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
	
	if( pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo])/divideNum;
		else
			xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]/divideNum;
		if(bLeft)
			strcpy(pStr, "Left Alignment Calibration ");
		else
			strcpy(pStr, "Right Alignment Calibration ");
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
		if(LCDMenuConfig_EX.Calc_X_Speed == 0)
			strcat(pStr, "(Low Speed,");
		else if(LCDMenuConfig_EX.Calc_X_Speed == 1)	
			strcat(pStr, "(Middle Speed,");
		else
			strcat(pStr, "(High Speed,");
#else				
		if(LCDMenuConfig.Head_Height_Level == 0)
			strcat(pStr, "(High,");
		else
			strcat(pStr, "(Low,");
#endif				
		switch(pPrintParam->VSDModel)
		{
		case 1:
			strcat(pStr, "VSD1)");
			break;
		case 2:
			strcat(pStr, "VSD2)");
			break;
		default:
		case 3:
			strcat(pStr, "VSD3)");
			break;
		case 4:
			strcat(pStr, "VSD4)");
			break;
		}
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, LEFTCHECK_TITLE_STARTNOZZLE, 3, (INT8U*)pStr,
						pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
		return True;
	}
	
	UnitNO = (pRuntimeParam->curpassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
	InterleaveNO = UnitNO % epsonCaliSetting.printerSetting.YInterleaveNum;
	colorNO = UnitNO / epsonCaliSetting.printerSetting.YInterleaveNum;
	
	INT8U baseGroupNO;
	INT16S StepNozzleBefNobase;
	INT16S StepNozzleAftNobase;
	INT8U order;
	INT8U headNO_in_group = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[colorNO], InterleaveNO) % epsonCaliSetting.printerSetting.head_group;
	INT8U insertedPassNO = GetNoBasePassNO_ForHorizatalLikeCheck(
																 divideNum, colorNO, (InterleaveNO == 0 && (bLeft == epsonCaliSetting.printerSetting.bOriginIsRight) ),
																 headNO_in_group, &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
	
	if(( pass_NO_in_sub_cycle >= insertedPassNO && pass_NO_in_sub_cycle < insertedPassNO + divideNum))
	{
		for(groupNO = 0; groupNO<epsonCaliSetting.printerSetting.GroupCount_YContinue; groupNO++)
		{
			headNO = GetHeadIndexWithInterleave(groupNO, factoryDataEx.m_nColorOrder[colorNO], InterleaveNO);
			lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[colorNO], InterleaveNO);
			cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
			
			{
				if(bLeft)
				{
					if( epsonCaliSetting.printerSetting.bOriginIsRight)
					{
						xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]);
					}
					else
					{
						xOffset = (pCurXCali->NegDirAllHeadWidth - pCurXCali->xNegDirOffset.XOffset[headNO][lineNo]);
					}
					
					phaseNO = pass_NO_in_sub_cycle - insertedPassNO;
					if( !epsonCaliSetting.printerSetting.bOriginIsRight)
						phaseNO = (divideNum - phaseNO) % divideNum;
					
					for(i=0; i<UnitCount; i++)
					{
						temp = (xOffset - (UnitCount/2 - i) - xOffset_base%divideNum ) % divideNum;
						if(temp <0)
							temp +=divideNum;
						if(temp != phaseNO)
						{
							//xOffset_cur = i *xLenPerLine + (xOffset - (LeftUnitCount/2 - i) - xOffset_base%divideNum)/divideNum;
							//FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, 0, LEFTCHECK_STARTNOZZLE+10);
							continue;
						}
						
						xOffset_cur = (i *xLenPerLine * divideNum + xOffset - (UnitCount/2 - i) - xOffset_base%divideNum)/divideNum;
						
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
						
						xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
						{
							FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
							xOffset_cur ++;
						}
					}
				}
				else
				{
					lineNO_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNo]];
					headNO_base = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNo]];
					
					if(!epsonCaliSetting.printerSetting.bOriginIsRight)
					{
						xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
						xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base];
					}
					else
					{
						xOffset = pCurXCali->xNegDirOffset.XOffset[headNO][lineNo];
						xOffset_base = pCurXCali->xNegDirOffset.XOffset[headNO_base][lineNO_base];
					}
					
					phaseNO = pass_NO_in_sub_cycle - insertedPassNO;
					
					if( !epsonCaliSetting.printerSetting.bOriginIsRight)
						phaseNO = (divideNum - phaseNO) % divideNum;
					
					//                for(l=0; l<GetPatternWidth(RightCheck, pPrintParam, pRuntimeParam->curpassNO); l++)
					//                {
					//                    xOffset_cur = xOffset + l;
					//                    FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, phaseNO * 30, phaseNO * 30+60);
					//                }
					
					for(i=0; i<UnitCount; i++)
					{
						temp = (xOffset - (UnitCount/2 - i) - xOffset_base%divideNum) % divideNum;
						if(temp <0)
							temp +=divideNum;
						temp = (divideNum - temp) % divideNum;
						if(temp != phaseNO)
						{
							//xOffset_cur = (xOffset - (LeftUnitCount/2 - i) - xOffset_base%divideNum) / divideNum +  i *xLenPerLine;
							//FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, 0, LEFTCHECK_STARTNOZZLE+10);
							continue;
						}
						
						xOffset_cur = (LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH + LEFTCHECK_COLORINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						xOffset_cur += i *xLenPerLine;
						xOffset_cur *= divideNum;
						xOffset_cur += (xOffset - (UnitCount/2 - i) - xOffset_base%divideNum);
						xOffset_cur /= divideNum;
						//the following 公式不等于上面的公式，因为，如果(xOffset - (UnitCount/2 - i) - xOffset_base%divideNum)为
						//  负值时，两者不相等。
						//  如：(-1/4) + 3 != (-1+3*4)/4.
						//xOffset_cur = (xOffset - (UnitCount/2 - i) - xOffset_base%divideNum) / divideNum + i *xLenPerLine +
						//    (LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH + LEFTCHECK_COLORINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						if((xOffset - (UnitCount/2 - i) - xOffset_base%divideNum) % divideNum != 0)
							xOffset_cur ++;
						for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
						{
							FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
							xOffset_cur ++;
						}
						xOffset_cur--;
						xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
					}
				}
			}
		}
	}
	else
	{
		lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[colorNO], InterleaveNO);
		
		if(pass_NO_in_sub_cycle < insertedPassNO)
		{
                    if(order ==1)
                    {
                        if(pass_NO_in_sub_cycle==0)
                                groupNO = pass_NO_in_sub_cycle;
                        else
                            groupNO = epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - pass_NO_in_sub_cycle + 1;
                    }
                    else
			groupNO = epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - pass_NO_in_sub_cycle;
		}
		else
			groupNO = epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - (pass_NO_in_sub_cycle - divideNum);
		
		headNO = GetHeadIndexWithInterleave(groupNO, factoryDataEx.m_nColorOrder[colorNO], InterleaveNO);
		
		colorCode = epsonCaliSetting.colorMap.ColorCode[headNO][lineNo];
		GetGroupString(groupNO + InterleaveNO % epsonCaliSetting.printerSetting.GroupCount_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue, 
					   headNO_in_group, colorCode, InterleaveNO / epsonCaliSetting.printerSetting.GroupCount_YInterleave, sub_titleBuff);
		
		lineNO_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNo]];
		headNO_base = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNo]];
		if(headNO_base == headNO && lineNO_base == lineNo && (bLeft == epsonCaliSetting.printerSetting.bOriginIsRight))
		{
			lineNo = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			headNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
		}
		else
		{
			lineNo = lineNO_base;
			headNO = headNO_base;
		}
		lineNO_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO_base = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		
		INT16S nozzle_shift = 0;
		INT8U bUseColorBase = 0;
		INT8U BaseColor_HeadIndex_backup;
		INT8U BaseColor_LineIndex_backup[MAX_INTERLEAVE_CALIBRATION];
		if(headNO != headNO_base)
		{ //we use BaseColor('K') to print title and digital normally. But this BaseColor can NOT be the same base of color-alignment.
			//when BaseColor('K') and color-alignment base has Y shift, we use color-alignment base to print title and digital.
			nozzle_shift = epsonCaliSetting_runtime.yOffset.YOffset[headNO_base][0] -
				epsonCaliSetting_runtime.yOffset.YOffset[headNO][0];
			nozzle_shift /= (epsonCaliSetting.eprCaliSetting.yOffset.nDPI/
							 (epsonCaliSetting.printerSetting.DPI_Y/epsonCaliSetting.printerSetting.YInterleaveNum));
			if(nozzle_shift >= 8 || nozzle_shift <= -8) //only for safe.
			{
				lineNO_base = lineNo;
				headNO_base = headNO;
				bUseColorBase = True;
				BaseColor_HeadIndex_backup = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
				epsonCaliSetting.printerSetting.BaseColor_HeadIndex = headNO;
				for(i = 0; i<epsonCaliSetting.printerSetting.YInterleaveNum; i++)
				{
					BaseColor_LineIndex_backup[i] = epsonCaliSetting.printerSetting.BaseColor_LineIndex[i];
					epsonCaliSetting.printerSetting.BaseColor_LineIndex[i] = lineNo + (BaseColor_LineIndex_backup[i] - BaseColor_LineIndex_backup[0]);
				}
			}
		}
		
		cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
		
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
		{
			xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo])/divideNum;
			xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base])/divideNum;
		}
		else
		{
			xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]/divideNum;
			xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base]/divideNum;
		}
		
		if(!epsonCaliSetting.printerSetting.bOriginIsRight)
		{
			DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
							(LEFTCHECK_STARTNOZZLE + LEFTCHECK_ENDNOZZLE)/2, 3, sub_titleBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
			xOffset += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			xOffset_base += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			
			xOffset += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			xOffset_base += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
		}
		for(i=0; i<UnitCount; i++)
		{

#ifdef  MANUFACTURER_DYSS   
    		if( pRuntimeParam->curpassNO == 1)
#endif	
		{
			GetDigitalString(i, UnitCount, digitalBuff);
			DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
							LEFTCHECK_TITLE_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
		}
			xOffset_base += xLenPerLine;
			
			xOffset_cur = xOffset + i * xLenPerLine;
			
			if(epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
							  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
				FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
							  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
				
				xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
				{
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
								  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
								  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
					xOffset_cur ++;
				}
			}
			else
			{
				for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
				{
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
								  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
								  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
					xOffset_cur ++;
				}
				xOffset_cur --;
				xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
							  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
				FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
							  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
			}
		}
		if(epsonCaliSetting.printerSetting.bOriginIsRight)
		{
			DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
							(LEFTCHECK_STARTNOZZLE + LEFTCHECK_ENDNOZZLE)/2, 3, sub_titleBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
		}
		
		if(bUseColorBase)
		{
			epsonCaliSetting.printerSetting.BaseColor_HeadIndex = BaseColor_HeadIndex_backup;
			for(i = 0; i<epsonCaliSetting.printerSetting.YInterleaveNum; i++)
			{
				epsonCaliSetting.printerSetting.BaseColor_LineIndex[i] = BaseColor_LineIndex_backup[i];
			}
		}
	}
	
	return True;
}

INT8U GenGroupOffsetCheckBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, lineNo, headNO, phaseNO, groupNO, UnitNO, InterleaveNO, colorNO;
	INT8U lineNO_base, headNO_base, pass_NO_in_sub_cycle;
	INT8U i,j, dotsize, digitalBuff[8], sub_titleBuff[8];
	INT32S xOffset, xOffset_cur, xOffset_base, xLenPerLine;
	INT8U divideNum =0, LeftUnitCount = GetUnitCount( GroupOffsetCheck , pPrintParam);
	INT8S temp;
	char pStr[80];
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	INT8S head_group = epsonCaliSetting.printerSetting.head_group;
	
	if( pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	xLenPerLine = (LEFTCHECK_LINECOUNT + LEFTCHECK_LINEINTERVAL + LEFTCHECK_COLORINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	divideNum = 1; //must be 720 DPI.
	pass_NO_in_sub_cycle = (pRuntimeParam->curpassNO - 1) % (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
	
	if(pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo])/divideNum;
		else
			xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]/divideNum;
		strcpy(pStr, "Head Offset Calibration ");
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
		if(LCDMenuConfig_EX.Calc_X_Speed == 0)
			strcat(pStr, "(Low Speed,");
		else if(LCDMenuConfig_EX.Calc_X_Speed == 1)	
			strcat(pStr, "(Middle Speed,");
		else
			strcat(pStr, "(High Speed,");
#endif				
		switch(pPrintParam->VSDModel)
		{
		case 1:
			strcat(pStr, "VSD1)");
			break;
		case 2:
			strcat(pStr, "VSD2)");
			break;
		default:
		case 3:
			strcat(pStr, "VSD3)");
			break;
		case 4:
			strcat(pStr, "VSD4)");
			break;
		}				
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, LEFTCHECK_TITLE_STARTNOZZLE, 3, (INT8U*)pStr,
						pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
		return True;
	}
	
	if( !epsonCaliSetting.printerSetting.bIntegratedHead || epsonCaliSetting.printerSetting.HeadCount != 2)
	{
		INT8U divideNum = 1;  //must be 720 or 600 DPI.
		INT8U baseGroupNO;
		INT16S StepNozzleBefNobase;
		INT16S StepNozzleAftNobase;
		INT8U order;
		INT8U headNO_in_group = ((pRuntimeParam->curpassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum)) %
			epsonCaliSetting.printerSetting.head_group;
		INT8U insertedPassNO = GetNoBasePassNO_ForGroupOffsetCheck(divideNum, headNO_in_group,
																   &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
		
		if( pass_NO_in_sub_cycle >= insertedPassNO && pass_NO_in_sub_cycle < insertedPassNO + divideNum)
		{
			lineNO_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			headNO_base = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			
			lineNo = lineNO_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			
			UnitNO = (pRuntimeParam->curpassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
			int baseColorNO_In_Head = epsonCaliSetting.printerSetting.BaseColor_Index % 
				(NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
			colorNO = (UnitNO % epsonCaliSetting.printerSetting.head_group) * NOZZLE_LINE_COUNT_PER_HEAD / 
				epsonCaliSetting.printerSetting.YInterleaveNum;
			colorNO += baseColorNO_In_Head;
			INT8U interleaveNO = UnitNO / epsonCaliSetting.printerSetting.head_group;
			
			for(groupNO = 0; groupNO<epsonCaliSetting.printerSetting.GroupCount_YContinue; groupNO++)
			{
				headNO = GetHeadIndexWithInterleave(groupNO, factoryDataEx.m_nColorOrder[colorNO], interleaveNO);
				lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[colorNO], interleaveNO);
				cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
				
				if( epsonCaliSetting.printerSetting.bOriginIsRight)
				{
					xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]);
					xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base]);
				}
				else
				{
					xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
					xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base];
				}
				if(!epsonCaliSetting.printerSetting.bOriginIsRight)
				{
					xOffset += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					xOffset_base += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					
					xOffset += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					xOffset_base += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				}
				
				for(i=0; i<LeftUnitCount; i++)
				{
					xOffset_cur = xOffset + i * xLenPerLine;
					
					if( epsonCaliSetting.printerSetting.bOriginIsRight)
					{
						xOffset_cur -= (LeftUnitCount/2 - i);
						
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
						
						xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
						{
							FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
							xOffset_cur ++;
						}
					}
					else
					{
						xOffset_cur -= (LeftUnitCount/2 - i);
						
						for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
						{
							FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
							xOffset_cur ++;
						}
						xOffset_cur --;
						xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
					}
				}
			}
		}
		else
		{
			UnitNO = (pRuntimeParam->curpassNO - 1) / (epsonCaliSetting.printerSetting.GroupCount_YContinue + divideNum);
			colorNO = (UnitNO % epsonCaliSetting.printerSetting.head_group) * NOZZLE_LINE_COUNT_PER_HEAD / 
				epsonCaliSetting.printerSetting.YInterleaveNum;
			INT8U interleaveNO = UnitNO / epsonCaliSetting.printerSetting.head_group;
			
			lineNo = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			headNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
			
			INT8U groupNO_YContinue = pass_NO_in_sub_cycle;
			if(pass_NO_in_sub_cycle > insertedPassNO)
				groupNO_YContinue -= divideNum;
			INT8U groupNO_YInterleave = (interleaveNO % epsonCaliSetting.printerSetting.GroupCount_YInterleave);
			//INT8U groupNO = (interleaveNO % epsonCaliSetting.printerSetting.GroupCount_YInterleave)*
			//    epsonCaliSetting.printerSetting.GroupCount_YContinue + groupNO_YContinue;
			
			INT8U title_index = 0;
			memset(sub_titleBuff, 0, sizeof(sub_titleBuff));
			if( epsonCaliSetting.printerSetting.GroupCount > 1)
			{
				if( epsonCaliSetting.printerSetting.head_group > 1)
				{
					sub_titleBuff[title_index] = 'G';
					title_index++;
					sub_titleBuff[title_index] = '0' + (epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNO_YContinue) + 
						groupNO_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue;
					title_index++;
					
					if(NOZZLE_LINE_COUNT_PER_HEAD != epsonCaliSetting.printerSetting.YInterleaveNum_InHead)
					{
#ifdef MANUFACTURER_DYSS
						//the color Index is same to the head index
						INT8U colorIndex = UnitNO % epsonCaliSetting.printerSetting.head_group;
#ifdef RICOH_3H
						colorIndex *= 2;		//三头：一个头两种颜色
						if(colorIndex == 0 || colorIndex == 2)
							sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c,%c", factoryDataEx.m_nColorOrder[colorIndex], factoryDataEx.m_nColorOrder[colorIndex + 1]);
						else if(colorIndex == 4)
							sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c,%c", COLORCODE_W, COLORCODE_V);
#else	//RICOH 6H
						if(colorIndex <= 3)
							sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c", factoryDataEx.m_nColorOrder[colorIndex]);
						else if(colorIndex == 4)
							sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c", COLORCODE_W);
						else if(colorIndex == 5)
							sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c", COLORCODE_V);						
#endif	//end RICOH_3H
						title_index = strlen(sub_titleBuff);
#else
						sub_titleBuff[title_index] = 'H';
						title_index++;
						sub_titleBuff[title_index] = '0' + (UnitNO % epsonCaliSetting.printerSetting.head_group) + 1;
						title_index++;
						sub_titleBuff[title_index] = 0;
#endif	//end MANUFACTURER_DYSS
					}
					else
					{
						if( factoryDataEx.m_nColorOrder[colorNO] >= COLORCODE_SPOTCOLOR0 &&
						   factoryDataEx.m_nColorOrder[colorNO] <= COLORCODE_SPOTCOLOR3)
						{
							INT8U k = factoryDataEx.m_nColorOrder[colorNO] - COLORCODE_SPOTCOLOR0;
							k = LCDMenuConfig.SpotColorName[k];
							INT8U * pName = GetPrintedSpotColorName(k);
							
							for(k=0; pName[k] != 0; k++)
							{
								sub_titleBuff[k+title_index] = pName[k];
							}
							sub_titleBuff[k+title_index] = 0;
						}
						else
						{
							sub_titleBuff[title_index] = factoryDataEx.m_nColorOrder[colorNO];
							title_index++;
							sub_titleBuff[title_index] = 0;
						}
					}
				}
				else
				{
#ifdef MANUFACTURER_DYSS		
					//the color Index is same to the head index
					INT8U colorIndex = (epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNO_YContinue) + 
					groupNO_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue - 1;
#ifdef RICOH_3H
					colorIndex *= 2;		//三头：一个头两种颜色
					if(colorIndex == 0 || colorIndex == 2)
						sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c,%c", factoryDataEx.m_nColorOrder[colorIndex], factoryDataEx.m_nColorOrder[colorIndex + 1]);
					else if(colorIndex == 4)
						sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c,%c", COLORCODE_W, COLORCODE_V);
#else	//RICOH 6H
					if(colorIndex <= 3)
						sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c", factoryDataEx.m_nColorOrder[colorIndex]);
					else if(colorIndex == 4)
						sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c", COLORCODE_W);
					else if(colorIndex == 5)
						sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c", COLORCODE_V);							
#endif	//end RICOH_3H
					title_index = strlen(sub_titleBuff);
#else
					sub_titleBuff[title_index] = 'H';
					title_index++;
					sub_titleBuff[title_index] = '0' + (epsonCaliSetting.printerSetting.GroupCount_YContinue - groupNO_YContinue) + 
						groupNO_YInterleave * epsonCaliSetting.printerSetting.GroupCount_YContinue;
					title_index++;
#endif	//end MANUFACTURER_DYSS
				}
			}
			else
			{
#ifdef MANUFACTURER_DYSS		
				//the color Index is same to the head index
				INT8U colorIndex = UnitNO % epsonCaliSetting.printerSetting.head_group;
#ifdef RICOH_3H
				colorIndex *= 2;		//三头：一个头两种颜色
				if(colorIndex == 0 || colorIndex == 2)
					sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c,%c", factoryDataEx.m_nColorOrder[colorIndex], factoryDataEx.m_nColorOrder[colorIndex + 1]);
				else if(colorIndex == 4)
					sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c,%c", COLORCODE_W, COLORCODE_V);
#else	//RICOH 6H
				if(colorIndex <= 3)
					sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c", factoryDataEx.m_nColorOrder[colorIndex]);
				else if(colorIndex == 4)
					sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c", COLORCODE_W);
				else if(colorIndex == 5)
					sprintf(sub_titleBuff+strlen(sub_titleBuff),"%c", COLORCODE_V);							
#endif	//end RICOH_3H
				title_index = strlen(sub_titleBuff);
#else
				sub_titleBuff[title_index] = 'H';
				title_index++;
				sub_titleBuff[title_index] = '0' + (UnitNO % epsonCaliSetting.printerSetting.head_group) + 1;
				title_index++;
#endif	//end MANUFACTURER_DYSS
			}
			
			lineNO_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
			headNO_base = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
			
			if( epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo])/divideNum;
				xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base])/divideNum;
			}
			else
			{
				xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]/divideNum;
				xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base]/divideNum;
			}
			
			if(!epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
								(LEFTCHECK_STARTNOZZLE + LEFTCHECK_ENDNOZZLE)/2, 3, sub_titleBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
				xOffset += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				xOffset_base += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				xOffset += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				xOffset_base += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			}
			
			for(i=0; i<LeftUnitCount; i++)
			{
#ifdef  MANUFACTURER_DYSS   
    		if( pRuntimeParam->curpassNO == 1)
#endif	
		{
				GetDigitalString(i, LeftUnitCount, digitalBuff);
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
								LEFTCHECK_TITLE_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
		}
				xOffset_base += xLenPerLine;
				
				xOffset_cur = xOffset + i * xLenPerLine;
				
				if(epsonCaliSetting.printerSetting.bOriginIsRight)
				{
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
								  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
								  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
					
					xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
					{
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
									  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
									  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
						xOffset_cur ++;
					}
				}
				else
				{
					for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
					{
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
									  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
									  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
						xOffset_cur ++;
					}
					xOffset_cur --;
					xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
								  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
								  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
				}
			}
			
			if(epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
								(LEFTCHECK_STARTNOZZLE + LEFTCHECK_ENDNOZZLE)/2, 3, sub_titleBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
			}
		}
	}
	else
	{
		//only total 2H.
		INT8U BaseIsUp;
		INT8U baseGroupNO;
		INT8U headNO_in_group;
		if(head_group == 1)
		{
			baseGroupNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index]/head_group;
			BaseIsUp = (baseGroupNO == 0);
			headNO_in_group = 0;
		}
		else
		{
			INT8U divideNum = 1;  //must be 720 or 600 DPI.
			INT16S StepNozzleBefNobase;
			INT16S StepNozzleAftNobase;
			INT8U order;
			INT8U baseHeadNO_in_group = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index]%head_group;
			headNO_in_group = (baseHeadNO_in_group == 0 ? 1:0);
			INT8U insertedPassNO = GetNoBasePassNO_ForGroupOffsetCheck(divideNum, headNO_in_group,
																	   &baseGroupNO, &StepNozzleBefNobase, &StepNozzleAftNobase,&order);
			if(insertedPassNO == 0)
				BaseIsUp = False;
			else
				BaseIsUp = True;
		}
		if( (pass_NO_in_sub_cycle == 0 && BaseIsUp == False) ||(pass_NO_in_sub_cycle == 1 && BaseIsUp == True))
		{
			//print the calibrated head.
			lineNo = lineNO_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			headNO_base = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			
			if(head_group == 1)
				groupNO = baseGroupNO == 0 ? 1 : 0;
			else
				groupNO = 0;
			//for(groupNO = 0; groupNO<epsonCaliSetting.printerSetting.GroupCount; groupNO++)
			{
				headNO = GetHeadIndexWithInterleave(groupNO, factoryDataEx.m_nColorOrder[0], 0);
				headNO += headNO_in_group;
				
				if(epsonCaliSetting.printerSetting.bOriginIsRight)
				{
					xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]);
					xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base]);
				}
				else
				{
					xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
					xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base];
					xOffset += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					xOffset_base += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					
					xOffset += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					xOffset_base += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					
				}
				
				cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
				
				for(i=0; i<LeftUnitCount; i++)
				{
					xOffset_cur = xOffset + i * xLenPerLine;
					
					if( epsonCaliSetting.printerSetting.bOriginIsRight)
					{
						xOffset_cur -= (LeftUnitCount/2 - i);
						
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
						
						xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
						{
							FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
							xOffset_cur ++;
						}
					}
					else
					{
						xOffset_cur -= (LeftUnitCount/2 - i);
						
						for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
						{
							FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
							xOffset_cur ++;
						}
						xOffset_cur --;
						xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
						
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
					}
				}
			}
		}
		else
		{
			//print base head.
			
			lineNo = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			headNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
			cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
			
			sub_titleBuff[0] = 'H';
			if(head_group == 1)
			{
				groupNO = baseGroupNO == 0 ? 1 : 0;
				sub_titleBuff[1] = '0' + groupNO + 1;
			}
			else
			{
				sub_titleBuff[1] = '0' + headNO_in_group + 1;
			}
			sub_titleBuff[2] = 0;
			
			lineNO_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
			headNO_base = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
			
			if( epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo])/divideNum;
				xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base])/divideNum;
			}
			else
			{
				xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]/divideNum;
				xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base]/divideNum;
			}
			
			if(!epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
								(LEFTCHECK_STARTNOZZLE + LEFTCHECK_ENDNOZZLE)/2, 3, sub_titleBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
				xOffset += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				xOffset_base += LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				xOffset += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				xOffset_base += LEFTCHECK_COLORINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			}
			
			for(i=0; i<LeftUnitCount; i++)
			{
				GetDigitalString(i, LeftUnitCount, digitalBuff);
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
								LEFTCHECK_TITLE_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
				xOffset_base += xLenPerLine;
				
				xOffset_cur = xOffset + i * xLenPerLine;
				
				if(epsonCaliSetting.printerSetting.bOriginIsRight)
				{
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
								  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
								  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
					
					xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
					{
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
									  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
									  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
						xOffset_cur ++;
					}
				}
				else
				{
					for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
					{
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
									  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
						FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
									  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
						xOffset_cur ++;
					}
					xOffset_cur --;
					xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
								  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
					FillVerLineEx(cur_lineNo, xOffset_cur, pBuff, dotsize,
								  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
				}
			}
			
			if(epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
								(LEFTCHECK_STARTNOZZLE + LEFTCHECK_ENDNOZZLE)/2, 3, sub_titleBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
			}
		}
	}
	
	return True;
}

INT8U GenBiDirAlignBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, headNO, phaseNO;
	INT8U lineNO_base, headNO_base, pass_NO_in_sub_cycle;
	INT8U i,j, dotsize, digitalBuff[8];
	INT32S xOffset, xOffset_cur, xOffset_base, xLenPerLine;
	INT8U divideNum =0, LeftUnitCount = GetUnitCount( BiDirCheck , pPrintParam);
	INT8S temp;
	char pStr[80];
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	if(pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
#ifdef  BI_TEST
	if(pRuntimeParam->curpassNO < 3)
		dotsize = 0x1;
	else if(pRuntimeParam->curpassNO < 5)	
		dotsize = 0x2;
	else
		dotsize = 0x3;
#endif	
	xLenPerLine = (LEFTCHECK_LINECOUNT + LEFTCHECK_LINEINTERVAL + LEFTCHECK_COLORINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
	pass_NO_in_sub_cycle = (pRuntimeParam->curpassNO - 1) % (1 + divideNum);
	
	if(pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo])/divideNum;
		else
			xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]/divideNum;
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
		if(LCDMenuConfig_EX.Calc_X_Speed == 0)
			strcpy(pStr, "Bi-Direction Calibration (Low Speed, ");
		else if(LCDMenuConfig_EX.Calc_X_Speed == 1)
			strcpy(pStr, "Bi-Direction Calibration (Middle Speed, ");
		else
			strcpy(pStr, "Bi-Direction Calibration (High Speed, ");
#else				
		if(LCDMenuConfig.Head_Height_Level == 0)
			strcpy(pStr, "Bi-Direction Calibration (High, ");
		else
			strcpy(pStr, "Bi-Direction Calibration (Low, ");
#endif				
		switch(pPrintParam->VSDModel)
		{
		case 1:
			strcat(pStr, "VSD1)");
			break;
		case 2:
			strcat(pStr, "VSD2)");
			break;
		default:
		case 3:
			strcat(pStr, "VSD3)");
			break;
		case 4:
			strcat(pStr, "VSD4)");
			break;
		}
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, LEFTCHECK_TITLE_STARTNOZZLE, 3, (INT8U*)pStr,
						pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
	}
	else if(pass_NO_in_sub_cycle >= 1)
	{
#ifdef  BI_TEST    
		if(LCDMenuConfig.Step_PassIndex <= 5)
			i = LCDMenuConfig.Step_PassIndex;
		else
			i = 0;
		cur_lineNo = lineNO_base = epsonCaliSetting.colorMap.BaseLineNo[i];
		headNO = headNO_base = epsonCaliSetting.colorMap.BaseHeadNo[i];
#else
		cur_lineNo = lineNO_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
		headNO = headNO_base = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
#endif
		if(!epsonCaliSetting.printerSetting.bOriginIsRight)
		{
			xOffset = pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
			xOffset_base = pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base];
		}
		else
		{
			xOffset = pCurXCali->xNegDirOffset.XOffset[headNO][cur_lineNo];
			xOffset_base = pCurXCali->xNegDirOffset.XOffset[headNO_base][lineNO_base];
		}
		
		phaseNO = pass_NO_in_sub_cycle - 1;
		if( !epsonCaliSetting.printerSetting.bOriginIsRight)
			phaseNO = (divideNum - phaseNO) % divideNum;
		
		//      for(l=0; l<GetPatternWidth(RightCheck, pPrintParam, pRuntimeParam->curpassNO); l++)
		//      {
		//          xOffset_cur = xOffset + l;
		//          FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, phaseNO * 30, phaseNO * 30+60);
		//      }
		
		for(i=0; i<LeftUnitCount; i++)
		{
			temp = (xOffset - (LeftUnitCount/2 - i) - xOffset_base%divideNum) % divideNum;
			if(temp <0)
				temp +=divideNum;
			temp = (divideNum - temp) % divideNum;
			if(temp != phaseNO)
			{
				//xOffset_cur = (xOffset - (LeftUnitCount/2 - i) - xOffset_base%divideNum) / divideNum +  i *xLenPerLine;
				//FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, 0, LEFTCHECK_STARTNOZZLE+10);
				continue;
			}
			if(!epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				xOffset_cur = (i+1) *xLenPerLine;
				xOffset_cur *= divideNum;
				xOffset_cur += (xOffset - (LeftUnitCount/2 - i) - xOffset_base%divideNum);
				xOffset_cur /= divideNum;
			}
			else
			{
				xOffset_cur = (LEFTCHECK_COLORINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				xOffset_cur += i *xLenPerLine;
				xOffset_cur *= divideNum;
				xOffset_cur += (xOffset - (LeftUnitCount/2 - i) - xOffset_base%divideNum);
				xOffset_cur /= divideNum;
			}
			//the following 公式不等于上面的公式，因为，如果(xOffset - (LeftUnitCount/2 - i) - xOffset_base%divideNum)为
			//  负值时，两者不相等。
			//  如：(-1/4) + 3 != (-1+3*4)/4.
			//xOffset_cur = (xOffset - (LeftUnitCount/2 - i) - xOffset_base%divideNum) / divideNum + i *xLenPerLine +
			//    (LEFTCHECK_INFO_MAX_CHAR_NUM * STEPCHECK_CHAR_WIDTH + LEFTCHECK_COLORINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			if((xOffset - (LeftUnitCount/2 - i) - xOffset_base%divideNum) % divideNum != 0)
				xOffset_cur ++;
			for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
			{
				FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo], xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
				if(epsonCaliSetting.printerSetting.bOriginIsRight)
					xOffset_cur ++;
				else
					xOffset_cur --; 
			}
			if(!epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				xOffset_cur ++;
				xOffset_cur -= LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			}
			else
			{
				xOffset_cur --; 
				xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			}
			FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo], xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE, LEFTCHECK_ENDNOZZLE);
		}
	}
	else
	{
#ifdef  BI_TEST    
		if(LCDMenuConfig.Step_PassIndex <= 5)
			i = LCDMenuConfig.Step_PassIndex;
		else
			i = 0;
		cur_lineNo = epsonCaliSetting.colorMap.BaseLineNo[i];
		headNO = epsonCaliSetting.colorMap.BaseHeadNo[i];
#else
		cur_lineNo = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
		headNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
#endif    
		lineNO_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO_base = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
		{
			xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo])/divideNum;
			xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base])/divideNum;
		}
		else
		{
			xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]/divideNum;
			xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base]/divideNum;
		}
		
		for(i=0; i<LeftUnitCount; i++)
		{
			GetDigitalString(i, LeftUnitCount, digitalBuff);
			DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
							LEFTCHECK_TITLE_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
			xOffset_base += xLenPerLine;
			
			xOffset_cur = xOffset + i * xLenPerLine;
			
			if(epsonCaliSetting.printerSetting.bOriginIsRight)
			{
				FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
							  xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
							  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
				FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
							  xOffset_cur, pBuff, dotsize,
							  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
				
				xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
				{
					FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
								  xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
								  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
					FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
								  xOffset_cur, pBuff, dotsize,
								  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
					xOffset_cur ++;
				}
			}
			else
			{
				for(j=0; j<LEFTCHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; j++)
				{
					FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
								  xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
								  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
					FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
								  xOffset_cur, pBuff, dotsize,
								  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
					xOffset_cur ++;
				}
				xOffset_cur --;
				xOffset_cur += LEFTCHECK_LINEINTERVAL* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
							  xOffset_cur, pBuff, dotsize, LEFTCHECK_STARTNOZZLE,
							  LEFTCHECK_STARTNOZZLE + (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2 );
				FillVerLineEx(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][cur_lineNo],
							  xOffset_cur, pBuff, dotsize,
							  LEFTCHECK_ENDNOZZLE - (LEFTCHECK_ENDNOZZLE - LEFTCHECK_STARTNOZZLE - LEFTCHECK_SPACENOZZLE)/2, LEFTCHECK_ENDNOZZLE);
			}
		}
	}
	
	return True;
}

INT8U GenYIntervalCheckBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo, lineNo, headNO, headNO_C,headNO_K,headNO_M,lineNo_C,lineNo_K,lineNo_M;
	INT8U i,j, dotsize;
	INT32S k, xOffset, xOffset_cur, len,xOffset_M,xOffset_K,xOffset_C;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	if( pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	
	if( pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]);
		else
			xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
		DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, VERTICALCHECK_TITLE_STARTNOZZLE, 3, "Head Vertical Interval Check", pRuntimeParam->curDir, STEPCHECK_CHAR_WIDTH);
	}
	else
	{

			if(epsonCaliSetting.printerSetting.bYIntervalReplaceVertical && (epsonCaliSetting.printerSetting.HeadCount == 2)&& (factoryData.spot_color1_headnum + factoryData.spot_color2_headnum == 0))
			{
				for(j = 0; j < epsonCaliSetting.printerSetting.head_group; j++)
				{
					if((epsonCaliSetting.printerSetting.verticalNozzleShift>0 && j != epsonCaliSetting.printerSetting.head_group - 1) ||
					   (epsonCaliSetting.printerSetting.verticalNozzleShift<0 && j != 0))
					{
						for(i=0; i< NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead; i++)
						{
							if( j * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead + i >=
							   epsonCaliSetting.printerSetting.ColorCount)
								continue;
							
							xOffset = i * (YINTERVALCHECK_UNITINTERVAL +
										   YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) *
								pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
							
							lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[j * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum + i], 0);
							headNO = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[j * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum + i], 0);
							
							cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
							
							if( epsonCaliSetting.printerSetting.bOriginIsRight)
								xOffset += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
							else
								xOffset += pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
							
							len = YINTERVALCHECK_DOTCOUNT_START;
							for(k=epsonCaliSetting.printerSetting.NozzleCount -  YINTERVALCHECK_LINECOUNT; k<epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM; k++)
							{
								FillHorLine(cur_lineNo, k, xOffset, len * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
								len += YINTERVALCHECK_DOTCOUNT_STEP;
							}
						}
					}
					if((epsonCaliSetting.printerSetting.verticalNozzleShift<0 && j != epsonCaliSetting.printerSetting.head_group - 1) ||
					   (epsonCaliSetting.printerSetting.verticalNozzleShift>0 && j != 0))
					{
						for(i=0; i< NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead; i++)
						{
							if(j * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead + i >=
							   epsonCaliSetting.printerSetting.ColorCount)
								continue;
							
							xOffset = i * (YINTERVALCHECK_UNITINTERVAL +
										   YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) *
								pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
							
							xOffset += YINTERVALCHECK_BLOCKSHIFT;
							
							lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[j * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum + i], 0);
							headNO = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[j * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum + i], 0);
							
							cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
							
							if(epsonCaliSetting.printerSetting.bOriginIsRight)
								xOffset += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
							else
								xOffset += pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
							
							len = YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT - CLOSE_NOZZLE_NUM - 1) * YINTERVALCHECK_DOTCOUNT_STEP - YINTERVALCHECK_BLOCKSHIFT;
							for(k=CLOSE_NOZZLE_NUM; k<YINTERVALCHECK_LINECOUNT; k++)
							{
								FillHorLine(cur_lineNo, k, xOffset, len * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
								len += YINTERVALCHECK_DOTCOUNT_STEP;
							}
						}
					}
				}
#if !(defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV))
				INT32S cur_nozzle_num = 0;
				for(i=0; i< epsonCaliSetting.printerSetting.ColorCount; i++)
				{
					if(factoryDataEx.m_nColorOrder[i] == COLORCODE_M)
						lineNo_M = epsonCaliSetting.colorMap.BaseLineNo[i];	
					if(factoryDataEx.m_nColorOrder[i] == COLORCODE_K)
						lineNo_K = epsonCaliSetting.colorMap.BaseLineNo[i];	
					if(factoryDataEx.m_nColorOrder[i] == COLORCODE_C)
						lineNo_C = epsonCaliSetting.colorMap.BaseLineNo[i];	
				}
				
				xOffset = (NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum) * (YINTERVALCHECK_UNITINTERVAL +
																										 YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) *
					pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				for(j = 0; j < epsonCaliSetting.printerSetting.head_group; j++)
				{
					headNO_M = j;
					headNO_K = j;
					headNO_C = j;
					cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_M][lineNo_M];
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_M = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_M][lineNo_M]) ;
					else
						xOffset_M = pCurXCali->xPosDirOffset.XOffset[headNO_M][lineNo_M];
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_K = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_K][lineNo_K]) ;
					else
						xOffset_K = pCurXCali->xPosDirOffset.XOffset[headNO_K][lineNo_K];
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_C = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_C][lineNo_C]) ;
					else
						xOffset_C = pCurXCali->xPosDirOffset.XOffset[headNO_C][lineNo_C];
					xOffset_M +=  xOffset;
					xOffset_K +=  xOffset;
					xOffset_C +=  xOffset;
					for(k = 0;k < YINTERVALCHECK_COLOR_BLANK1; k++)
					{
						for(cur_nozzle_num = k; cur_nozzle_num < NOZZLE_COUNT_PER_LINE;cur_nozzle_num+= YINTERVALCHECK_COLOR_BLANK1)
						{
							FillHorLine(cur_lineNo, cur_nozzle_num, xOffset_M, YINTERVALCHECK_COLOR_HOR_LEN, pBuff, dotsize);
						}
						xOffset_M += YINTERVALCHECK_COLOR_HOR_LEN;
					}
					for(k = 0;k < YINTERVALCHECK_COLOR_BLANK2; k++)
					{
						for(cur_nozzle_num = k; cur_nozzle_num < NOZZLE_COUNT_PER_LINE;cur_nozzle_num+= YINTERVALCHECK_COLOR_BLANK2)
						{
							FillHorLine(cur_lineNo, cur_nozzle_num, xOffset_M, YINTERVALCHECK_COLOR_HOR_LEN, pBuff, dotsize);
						}
						xOffset_M += YINTERVALCHECK_COLOR_HOR_LEN;
					}
					xOffset_K += YINTERVALCHECK_COLOR_HOR_LEN*(YINTERVALCHECK_COLOR_BLANK1+YINTERVALCHECK_COLOR_BLANK2);
					xOffset_C += YINTERVALCHECK_COLOR_HOR_LEN*(YINTERVALCHECK_COLOR_BLANK1+YINTERVALCHECK_COLOR_BLANK2);
					
					if((epsonCaliSetting.printerSetting.verticalNozzleShift>0 && j != epsonCaliSetting.printerSetting.head_group - 1) ||
					   (epsonCaliSetting.printerSetting.verticalNozzleShift<0 && j != 0))
					{
						FillHorLine_GAP(cur_lineNo, NOZZLE_COUNT_PER_LINE -1, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
						FillHorLine_GAP(cur_lineNo, NOZZLE_COUNT_PER_LINE -2, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
						xOffset_K += (X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI);
						FillHorLine_GAP(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_K][lineNo_K], NOZZLE_COUNT_PER_LINE -1, xOffset_K, X_BASE_RES - (X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI), pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI)*2);
					}
					if((epsonCaliSetting.printerSetting.verticalNozzleShift<0 && j != epsonCaliSetting.printerSetting.head_group - 1) ||
					   (epsonCaliSetting.printerSetting.verticalNozzleShift>0 && j != 0))
					{
						FillHorLine_GAP(cur_lineNo, 0, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
						FillHorLine_GAP(cur_lineNo, 1, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
						FillHorLine_GAP(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_C][lineNo_C], 0, xOffset_C, X_BASE_RES , pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI)*2);
					}
					
					xOffset_M += X_BASE_RES*2;
					xOffset_C += X_BASE_RES*2;
					xOffset_M += (X_BASE_RES/2)*(epsonCaliSetting.printerSetting.GroupCount - 1 - j);
					xOffset_C += (X_BASE_RES/2)*(epsonCaliSetting.printerSetting.GroupCount - 1 - j);
					
					FillHorLine_GAP(cur_lineNo, 0, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
					FillHorLine_GAP(cur_lineNo, NOZZLE_COUNT_PER_LINE -1, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
					if((epsonCaliSetting.printerSetting.verticalNozzleShift>0 && j != epsonCaliSetting.printerSetting.head_group - 1) ||
					   (epsonCaliSetting.printerSetting.verticalNozzleShift<0 && j != 0))
					{
						FillHorLine_GAP(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_C][lineNo_C], 0, xOffset_C, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
					}
					if((epsonCaliSetting.printerSetting.verticalNozzleShift<0 && j != epsonCaliSetting.printerSetting.head_group - 1) ||
					   (epsonCaliSetting.printerSetting.verticalNozzleShift>0 && j != 0))
					{
						FillHorLine_GAP(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_C][lineNo_C], 0, xOffset_C, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
						FillHorLine_GAP(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_C][lineNo_C], NOZZLE_COUNT_PER_LINE -1, xOffset_C, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
					}
					
					FillVerLine(cur_lineNo, xOffset_M, pBuff, dotsize);
					FillVerLine(cur_lineNo, xOffset_M + X_BASE_RES, pBuff, dotsize);			
				}
#endif
			}
			else if(epsonCaliSetting.printerSetting.bYIntervalReplaceVertical && (factoryData.spot_color1_headnum + factoryData.spot_color2_headnum > 0))
			{
				
				INT8S head_group_pure_color = (factoryData.color_num * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
											   NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
				INT8S head_group_white  = (factoryData.spot_color1_headnum * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
										   NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
				if(head_group_white ==0 && factoryData.spot_color1_headnum  > 0)
					head_group_white = 1;
				INT8S head_group_coat  = (factoryData.spot_color2_headnum * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
										  NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
				if(head_group_coat ==0 && factoryData.spot_color2_headnum  > 0)
					head_group_coat = 1;
				INT8S head_group_unin = ((factoryData.spot_color1_headnum+factoryData.spot_color1_headnum) * epsonCaliSetting.printerSetting.YInterleaveNum_InHead + 
										   NOZZLE_LINE_COUNT_PER_HEAD - 1)/NOZZLE_LINE_COUNT_PER_HEAD;
				if(head_group_unin ==0 && factoryData.spot_color1_headnum+factoryData.spot_color1_headnum > 0)
					head_group_unin = 1;				
                               struct WCV_POSITION pos[4];
                               for(int i=0;i<4;i++)
                               {
                                 pos[i].colornum = 0;
                                 pos[i].colorindex = 0;
                               }
                               
                               switch((factoryDataEx.LayoutType>>4)&0x7)
                               {
                                   case 0:
                                   {
                                     pos[0].colornum = head_group_white;
                                     pos[0].colorindex = head_group_pure_color;
                                     pos[1].colornum = head_group_pure_color;
                                     pos[1].colorindex = 0;
                                     pos[2].colornum = head_group_coat;
                                     pos[2].colorindex = head_group_pure_color+factoryData.spot_color1_headnum;

                                     break;
                                   }
                                   case 1:
                                   {
                                     pos[2].colornum = head_group_white;
                                     pos[2].colorindex = head_group_pure_color;
                                     pos[1].colornum = head_group_pure_color;
                                     pos[1].colorindex = 0;
                                     pos[0].colornum = head_group_coat;
                                     pos[0].colorindex = head_group_pure_color+factoryData.spot_color1_headnum;

                                       break;
                                   }
                                   case 2:
                                   {
                                     pos[0].colornum = head_group_unin;
                                     pos[0].colorindex = head_group_pure_color;
                                     pos[1].colornum = head_group_pure_color;
                                     pos[1].colorindex = 0;

                                       break;
                                   }
                                   case 3:
                                   {
                                     pos[1].colornum = head_group_unin;
                                     pos[1].colorindex = head_group_pure_color;
                                     pos[0].colornum = head_group_pure_color;
                                     pos[0].colorindex = 0;
                                     break;
                                   }
                                   default:
                                   break;
                               }
                               INT8S end =0;
                               for(int m=0;m<4;m++)
                               {
                                if(pos[m].colornum != 0)
                                end = m;
                               }
                              for(int k = 0; k < epsonCaliSetting.printerSetting.GroupCount_YContinue; k++)
                              {
                                for(int index =0;index<3;index++)
                                {
                                  int min= (pos[index].colornum < pos[index+1].colornum)? index:index+1;
                                  for(j = 0; j < pos[min].colornum; j++)
                                  {           	
					for(i=0; i< NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead; i++)
					{
                                      int color_index = (j + pos[index].colorindex) * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead + i;
						int white_color_index = (j) * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead + i;
						
						if( color_index >= epsonCaliSetting.printerSetting.ColorCount)
							continue;
						
						xOffset = white_color_index  * (YINTERVALCHECK_UNITINTERVAL +
														YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) *
							pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
                                      
                                      lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[color_index], i);
                                      headNO = GetHeadIndexWithInterleave(k, factoryDataEx.m_nColorOrder[color_index], i);
                                      
                                      cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
                                      
                                      if( epsonCaliSetting.printerSetting.bOriginIsRight)
                                        xOffset += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
                                      else
                                        xOffset += pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
                                      
                                      len = YINTERVALCHECK_DOTCOUNT_START;
                                      for(int n=epsonCaliSetting.printerSetting.NozzleCount -  YINTERVALCHECK_LINECOUNT; n<epsonCaliSetting.printerSetting.NozzleCount; n++)
                                      {
                                        FillHorLine(cur_lineNo, n, xOffset, len * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
                                        len += YINTERVALCHECK_DOTCOUNT_STEP;
                                      }
                                    }
                                    for(i=0; i< NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead; i++)
                                    {
                                      int color_index = (j+ pos[index+1].colorindex) * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead + i;
                                      int white_color_index = (j) * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead + i;
                                      
                                      if( color_index >= epsonCaliSetting.printerSetting.ColorCount)
                                        continue;
                                      
                                      xOffset = white_color_index * (YINTERVALCHECK_UNITINTERVAL +
                                                       YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) *
                                        pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
                                      
                                      xOffset += YINTERVALCHECK_BLOCKSHIFT;
                                      
                                      lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[color_index], i);
                                      headNO = GetHeadIndexWithInterleave(k, factoryDataEx.m_nColorOrder[color_index], i);
                                      
                                      cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
                                      
                                      if(epsonCaliSetting.printerSetting.bOriginIsRight)
                                        xOffset += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
                                      else
                                        xOffset += pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
                                      
                                      len = YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT - 1) * YINTERVALCHECK_DOTCOUNT_STEP - YINTERVALCHECK_BLOCKSHIFT;
                                      for(int n=0; n<YINTERVALCHECK_LINECOUNT; n++)
                                      {
                                        FillHorLine(cur_lineNo, n, xOffset, len * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
                                        len += YINTERVALCHECK_DOTCOUNT_STEP;
                                      }
                                    }
                                  }
                                }
                                if(k != epsonCaliSetting.printerSetting.GroupCount_YContinue - 1)
                                {
                                  for(i=0; i< NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead; i++)
                                  {
                                    int color_index = (j+ pos[end].colorindex) * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead + i;
                                    int white_color_index = (j) * NOZZLE_LINE_COUNT_PER_HEAD/epsonCaliSetting.printerSetting.YInterleaveNum_InHead + i;
                                    
                                    if( color_index >= epsonCaliSetting.printerSetting.ColorCount)
                                      continue;
                                    
                                    xOffset = white_color_index * (YINTERVALCHECK_UNITINTERVAL +
                                                     YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) *
                                      pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
                                    
                                    xOffset += YINTERVALCHECK_BLOCKSHIFT;
                                    
                                    lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[color_index], i);
                                    headNO = GetHeadIndexWithInterleave(k, factoryDataEx.m_nColorOrder[color_index], i);
                                    
                                    cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
                                    
                                    if(epsonCaliSetting.printerSetting.bOriginIsRight)
                                      xOffset += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
                                    else
                                      xOffset += pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
                                    
                                    len = YINTERVALCHECK_DOTCOUNT_START;
                                    for(int n=epsonCaliSetting.printerSetting.NozzleCount -  YINTERVALCHECK_LINECOUNT; n<epsonCaliSetting.printerSetting.NozzleCount; n++)
                                    {
                                      FillHorLine(cur_lineNo, n, xOffset, len * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
                                      len += YINTERVALCHECK_DOTCOUNT_STEP;
                                    }
                                  }
                                }
                              }
            }
			else
			{
				INT8U groupNO_YInterleave = pRuntimeParam->curpassNO - 1;
				
				for(j = 0; j < epsonCaliSetting.printerSetting.GroupCount_YContinue; j++)
				{
					if(j != epsonCaliSetting.printerSetting.GroupCount_YContinue - 1)
					{
						for(i=0; i< YINTERVALCHECK_COLORNUM; i++)
						{
							xOffset = i * (YINTERVALCHECK_UNITINTERVAL +
										   YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) *
								pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
							
							lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], groupNO_YInterleave);
							headNO = GetHeadIndexWithInterleave(j, factoryDataEx.m_nColorOrder[i], groupNO_YInterleave);
							
							cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
							
							if( epsonCaliSetting.printerSetting.bOriginIsRight)
								xOffset += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
							else
								xOffset += pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
							
							len = YINTERVALCHECK_DOTCOUNT_START;
							for(k=epsonCaliSetting.printerSetting.NozzleCount -  YINTERVALCHECK_LINECOUNT; k< epsonCaliSetting.printerSetting.NozzleCount - CLOSE_NOZZLE_NUM; k++)
							{
								FillHorLine(cur_lineNo, k, xOffset, len * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
								len += YINTERVALCHECK_DOTCOUNT_STEP;
							}
						}
					}
					if(j != 0)
					{
						for(i=0; i< YINTERVALCHECK_COLORNUM; i++)
						{
							xOffset = i * (YINTERVALCHECK_UNITINTERVAL +
										   YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) *
								pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
							
							xOffset += YINTERVALCHECK_BLOCKSHIFT;
							
							lineNo = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], groupNO_YInterleave);
							headNO = GetHeadIndexWithInterleave(j, factoryDataEx.m_nColorOrder[i], groupNO_YInterleave);
							
							cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO][lineNo];
							
							if(epsonCaliSetting.printerSetting.bOriginIsRight)
								xOffset += (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][lineNo]) ;
							else
								xOffset += pCurXCali->xPosDirOffset.XOffset[headNO][lineNo];
							
							len = YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT - CLOSE_NOZZLE_NUM - 1) * YINTERVALCHECK_DOTCOUNT_STEP - YINTERVALCHECK_BLOCKSHIFT;
							for(k=CLOSE_NOZZLE_NUM; k<YINTERVALCHECK_LINECOUNT; k++)
							{
								FillHorLine(cur_lineNo, k, xOffset, len * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X, pBuff, dotsize);
								len += YINTERVALCHECK_DOTCOUNT_STEP;
							}
						}
					}
				}
#if !(defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV))            
				xOffset = YINTERVALCHECK_COLORNUM * (YINTERVALCHECK_UNITINTERVAL +
													 YINTERVALCHECK_DOTCOUNT_START + (YINTERVALCHECK_LINECOUNT * 2 - 1) * YINTERVALCHECK_DOTCOUNT_STEP ) *
					pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				
				INT32S cur_nozzle_num = 0;
				for(i=0; i< epsonCaliSetting.printerSetting.ColorCount; i++)
				{
					if(factoryDataEx.m_nColorOrder[i] == COLORCODE_M)
						lineNo_M = epsonCaliSetting.colorMap.BaseLineNo[i];	
					if(factoryDataEx.m_nColorOrder[i] == COLORCODE_K)
						lineNo_K = epsonCaliSetting.colorMap.BaseLineNo[i];	
					if(factoryDataEx.m_nColorOrder[i] == COLORCODE_C)
						lineNo_C = epsonCaliSetting.colorMap.BaseLineNo[i];	
				}
				
				for(j = 0; j < epsonCaliSetting.printerSetting.GroupCount_YContinue; j++)
				{
					headNO_M = GetHeadIndexWithInterleave(j, COLORCODE_M, groupNO_YInterleave);
					headNO_K = GetHeadIndexWithInterleave(j, COLORCODE_K, groupNO_YInterleave);
					headNO_C = GetHeadIndexWithInterleave(j, COLORCODE_C, groupNO_YInterleave);
					cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_M][lineNo_M];
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_M = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_M][lineNo_M]) ;
					else
						xOffset_M = pCurXCali->xPosDirOffset.XOffset[headNO_M][lineNo_M];
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_K = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_K][lineNo_K]) ;
					else
						xOffset_K = pCurXCali->xPosDirOffset.XOffset[headNO_K][lineNo_K];
					
					if(epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset_C = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_C][lineNo_C]) ;
					else
						xOffset_C = pCurXCali->xPosDirOffset.XOffset[headNO_C][lineNo_C];
					xOffset_M +=  xOffset;
					xOffset_K +=  xOffset;
					xOffset_C +=  xOffset;
					for(k = 0;k < YINTERVALCHECK_COLOR_BLANK1; k++)
					{
						for(cur_nozzle_num = k + CLOSE_NOZZLE_NUM; cur_nozzle_num < NOZZLE_COUNT_PER_LINE - CLOSE_NOZZLE_NUM;cur_nozzle_num+= YINTERVALCHECK_COLOR_BLANK1)
						{
							FillHorLine(cur_lineNo, cur_nozzle_num, xOffset_M, YINTERVALCHECK_COLOR_HOR_LEN, pBuff, dotsize);
						}
						xOffset_M += YINTERVALCHECK_COLOR_HOR_LEN;
					}
					for(k = 0;k < YINTERVALCHECK_COLOR_BLANK2; k++)
					{
						for(cur_nozzle_num = k + CLOSE_NOZZLE_NUM; cur_nozzle_num < NOZZLE_COUNT_PER_LINE - CLOSE_NOZZLE_NUM;cur_nozzle_num+= YINTERVALCHECK_COLOR_BLANK2)
						{
							FillHorLine(cur_lineNo, cur_nozzle_num, xOffset_M, YINTERVALCHECK_COLOR_HOR_LEN, pBuff, dotsize);
						}
						xOffset_M += YINTERVALCHECK_COLOR_HOR_LEN;
					}
					xOffset_K += YINTERVALCHECK_COLOR_HOR_LEN*(YINTERVALCHECK_COLOR_BLANK1+YINTERVALCHECK_COLOR_BLANK2);
					xOffset_C += YINTERVALCHECK_COLOR_HOR_LEN*(YINTERVALCHECK_COLOR_BLANK1+YINTERVALCHECK_COLOR_BLANK2);
					
					if(j != epsonCaliSetting.printerSetting.GroupCount_YContinue - 1)
					{
						FillHorLine_GAP(cur_lineNo, NOZZLE_COUNT_PER_LINE -1 - CLOSE_NOZZLE_NUM, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
						FillHorLine_GAP(cur_lineNo, NOZZLE_COUNT_PER_LINE -2 - CLOSE_NOZZLE_NUM, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
						xOffset_K += (X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI);
						FillHorLine_GAP(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_K][lineNo_K], NOZZLE_COUNT_PER_LINE -1 - CLOSE_NOZZLE_NUM, xOffset_K, X_BASE_RES - (X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI), pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI)*2);
					}
					if(j != 0)
					{
						FillHorLine_GAP(cur_lineNo, 0 + CLOSE_NOZZLE_NUM, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
						FillHorLine_GAP(cur_lineNo, 1 + CLOSE_NOZZLE_NUM, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
						FillHorLine_GAP(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_C][lineNo_C], 0 + CLOSE_NOZZLE_NUM, xOffset_C, X_BASE_RES , pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI)*2);
					}
					
					xOffset_M += X_BASE_RES*2;
					xOffset_C += X_BASE_RES*2;
					xOffset_M += (X_BASE_RES/2)*(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - j);
					xOffset_C += (X_BASE_RES/2)*(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - j);
					
					FillHorLine_GAP(cur_lineNo, 0 + CLOSE_NOZZLE_NUM, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
					FillHorLine_GAP(cur_lineNo, NOZZLE_COUNT_PER_LINE -1 - CLOSE_NOZZLE_NUM, xOffset_M, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
					if(j != epsonCaliSetting.printerSetting.GroupCount_YContinue - 1)
					{
						FillHorLine_GAP(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_C][lineNo_C], NOZZLE_COUNT_PER_LINE -1 - CLOSE_NOZZLE_NUM, xOffset_C, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
					}
					if(j != 0)
					{
						FillHorLine_GAP(epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[headNO_C][lineNo_C], 0 + CLOSE_NOZZLE_NUM, xOffset_C, X_BASE_RES, pBuff, dotsize,(X_BASE_RES/YINTERVALCHECK_COLOR_HOR_DPI));
					}
					
					FillVerLine(cur_lineNo, xOffset_M, pBuff, dotsize);
					FillVerLine(cur_lineNo, xOffset_M + X_BASE_RES, pBuff, dotsize);			
				}
#endif            
			}
	}
	return True;
}

INT8U GetAngleShiftSegment(INT8S AngleShift)
{
	if(AngleShift<0)
		AngleShift = -AngleShift;
	return AngleShift + 1;
}
INT8S GetAngleShift(INT8S AngleShift, INT8U Segment, INT16S * pStart, INT16S * pEnd)
{
	float segmentlen = ((float)NOZZLE_COUNT_PER_LINE)/GetAngleShiftSegment(AngleShift);
	if(Segment == 0)
		*pStart = 0;
	else
		*pStart = (INT16S)(INT32S)(segmentlen * Segment);
	if((Segment + 1)== GetAngleShiftSegment(AngleShift))
		*pEnd = NOZZLE_COUNT_PER_LINE;
	else
		*pEnd = (INT16S)(INT32S)(segmentlen *(Segment+1));
	if(AngleShift<0)
		return 0 -Segment;
	else
		return Segment;
}

INT8U GetAngleGroupString(INT8U headNO, INT8U * pBuff)
{
	INT8U index =0;
	if(epsonCaliSetting.printerSetting.HeadCount >1)
	{
		pBuff[index] = 'H';
		index++;
		pBuff[index] = '1' + headNO % 10 ;
		index++;
	}
	pBuff[index] = 0;
	return index;
}

#ifdef HEAD_EPSON_GEN5
INT8U GenLeftRightAngleBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam, INT8U bLeft)
{
	INT8U cur_lineNo, headNO, phaseNO;
	INT8U lineNO_base, headNO_base, pass_NO_in_sub_cycle;
	INT8U i,j, dotsize, digitalBuff[8], digitalLen, sub_titleBuff[8];
	INT32S k,xOffset, xOffset_cur, xOffset_base, xLenPerLine, shift_x;
	INT8U divideNum =0, UnitCount = GetUnitCount( (bLeft?AngleLeftCheck:AngleRightCheck) , pPrintParam);
	INT8S temp;
	
	struct EPSON_X_Cali_Runtime * pCurXCali = pRuntimeParam->pCurXCali;
	
	if(pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	xLenPerLine = (ANGLECHECK_LINECOUNT + ANGLECHECK_LINEINTERVAL + ANGLECHECK_UNITINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
	divideNum = (((pPrintParam->Option & 0x6) >> 1) + 1);
	pass_NO_in_sub_cycle = (pRuntimeParam->curpassNO - 1) % divideNum;
	
	if(pRuntimeParam->curpassNO == 0)
	{
		cur_lineNo = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
		headNO = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		if( epsonCaliSetting.printerSetting.bOriginIsRight)
			xOffset_cur = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo])/divideNum;
		else
			xOffset_cur = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]/divideNum;
		if(bLeft)
			DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, LEFTCHECK_TITLE_STARTNOZZLE, 3, "Left Angle Calibration",
							pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
		else
			DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_cur, LEFTCHECK_TITLE_STARTNOZZLE, 3, "Right Angle Calibration",
							pRuntimeParam->curDir, LEFTCHECK_CHAR_WIDTH);
	}
	else if((pRuntimeParam->curpassNO - 1) >= divideNum)
	{
		for(headNO = 0; headNO<epsonCaliSetting.printerSetting.HeadCount; headNO++)
		{
			shift_x = 0;
			if(epsonCaliSetting_runtime.angleXOffset.bEnable)
			{
				if(pRuntimeParam->curDir)
					shift_x = epsonCaliSetting_runtime.angleXOffset.xPosDirOffset[headNO];
				else
					shift_x = - epsonCaliSetting_runtime.angleXOffset.xNegDirOffset[headNO];
			}
			
			for(cur_lineNo = 0; cur_lineNo < NOZZLE_LINE_COUNT_PER_HEAD; cur_lineNo++)
			{
				if(bLeft)
				{
					if( epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]);
					else
						xOffset = (pCurXCali->NegDirAllHeadWidth - pCurXCali->xNegDirOffset.XOffset[headNO][cur_lineNo]);
					xOffset += (headNO * UnitCount * NOZZLE_LINE_COUNT_PER_HEAD * xLenPerLine+ UnitCount)*divideNum;
					
					phaseNO = pass_NO_in_sub_cycle;
					if( !epsonCaliSetting.printerSetting.bOriginIsRight)
						phaseNO = (divideNum - phaseNO) % divideNum;
					
					for(i=0; i<UnitCount; i++)
					{
						INT8U seg = GetAngleShiftSegment((INT8S)UnitCount/2 - i + shift_x);
						for(j=0; j<seg; j++)
						{
							INT16S Start, End;
							INT8S cur_shift = GetAngleShift((INT8S)UnitCount/2 - i + shift_x, j, &Start, &End);
							
							temp = (xOffset - cur_shift) % divideNum;
							if(temp <0)
								temp +=divideNum;
							if(temp != phaseNO)
								continue;
							
							xOffset_cur = ((i * NOZZLE_LINE_COUNT_PER_HEAD+ cur_lineNo) * xLenPerLine *divideNum + xOffset - cur_shift)/divideNum;
							
							FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, Start, End);
							
							xOffset_cur += ANGLECHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
							for(k=0; k<ANGLECHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; k++)
							{
								FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, Start, End);
								xOffset_cur ++;
							}
						}
					}
				}
				else
				{
					if(!epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
					else
						xOffset = pCurXCali->xNegDirOffset.XOffset[headNO][cur_lineNo];
					xOffset += (headNO * UnitCount * NOZZLE_LINE_COUNT_PER_HEAD * xLenPerLine+ UnitCount)*divideNum;
					xOffset += ANGLECHECK_INFO_MAX_CHAR_NUM * TITLE_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					xOffset += ANGLECHECK_UNITINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					
					phaseNO = pass_NO_in_sub_cycle;
					if( !epsonCaliSetting.printerSetting.bOriginIsRight)
						phaseNO = (divideNum - phaseNO) % divideNum;
					
					for(i=0; i<UnitCount; i++)
					{
						INT8U seg = GetAngleShiftSegment((INT8S)UnitCount/2 - i + shift_x);
						for(j=0; j<seg; j++)
						{
							INT16S Start, End;
							INT8S cur_shift = GetAngleShift((INT8S)UnitCount/2 - i + shift_x, j, &Start, &End);
							
							temp = (xOffset - cur_shift) % divideNum;
							if(temp <0)
								temp +=divideNum;
							temp = (divideNum - temp) % divideNum;
							if(temp != phaseNO)
							{
								continue;
							}
							
							xOffset_cur = (i * NOZZLE_LINE_COUNT_PER_HEAD+ (NOZZLE_LINE_COUNT_PER_HEAD - 1 - cur_lineNo)) * xLenPerLine;
							xOffset_cur *= divideNum;
							xOffset_cur += xOffset - cur_shift;
							xOffset_cur /= divideNum;
							if((xOffset - cur_shift) % divideNum != 0)
								xOffset_cur ++;
							for(k=0; k<ANGLECHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; k++)
							{
								FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, Start, End);
								xOffset_cur ++;
							}
							xOffset_cur--;
							xOffset_cur += ANGLECHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
							
							FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, Start, End);
						}
					}
				}
			}
		}
	}
	else
	{
		INT8U HeadIndex_Backup = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
		for(headNO = 0; headNO<epsonCaliSetting.printerSetting.HeadCount; headNO++)
		{
			shift_x = 0;
			if(epsonCaliSetting_runtime.angleXOffset.bEnable)
			{
				if(pRuntimeParam->curDir)
					shift_x = epsonCaliSetting_runtime.angleXOffset.xPosDirOffset[headNO];
				else
					shift_x = - epsonCaliSetting_runtime.angleXOffset.xNegDirOffset[headNO];
			}
			
			epsonCaliSetting.printerSetting.BaseColor_HeadIndex = headNO;
			digitalLen = GetAngleGroupString(headNO, sub_titleBuff);
			
			lineNO_base = epsonCaliSetting.printerSetting.BaseColor_LineIndex[0];
			headNO_base = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
			
			if( bLeft)
				xOffset_base = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base])/divideNum;
			else
				xOffset_base = pCurXCali->xPosDirOffset.XOffset[headNO_base][lineNO_base]/divideNum;
			
			if(!bLeft)
			{
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
								0, 3, sub_titleBuff, pRuntimeParam->curDir, TITLE_CHAR_WIDTH);
				xOffset_base += ANGLECHECK_INFO_MAX_CHAR_NUM * TITLE_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
				xOffset_base += ANGLECHECK_UNITINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
			}
			for(cur_lineNo = 0; cur_lineNo < NOZZLE_LINE_COUNT_PER_HEAD; cur_lineNo++)
			{
				if(bLeft)
				{
					if( epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset = (pCurXCali->PosDirAllHeadWidth - pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo]);
					else
						xOffset = (pCurXCali->NegDirAllHeadWidth - pCurXCali->xNegDirOffset.XOffset[headNO][cur_lineNo]);
					//xOffset += ((ANGLECHECK_INFO_MAX_CHAR_NUM * TITLE_CHAR_WIDTH + ANGLECHECK_UNITINTERVAL)* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X +
					//    headNO * (UnitCount * NOZZLE_LINE_COUNT_PER_HEAD * xLenPerLine+ UnitCount))*divideNum;
					xOffset += (headNO * UnitCount * NOZZLE_LINE_COUNT_PER_HEAD * xLenPerLine+ UnitCount)*divideNum;
					
					phaseNO = pass_NO_in_sub_cycle;
					if( !epsonCaliSetting.printerSetting.bOriginIsRight)
						phaseNO = (divideNum - phaseNO) % divideNum;
					
					for(i=0; i<UnitCount; i++)
					{
						INT8U seg = GetAngleShiftSegment((INT8S)UnitCount/2 - i + shift_x);
						
						if(cur_lineNo == 0)
						{
							digitalLen = GetDigitalString(i, UnitCount, digitalBuff);
							DrawASCIIString(pBuff, pPrintParam, pCurXCali,
											xOffset_base + (NOZZLE_LINE_COUNT_PER_HEAD * xLenPerLine - digitalLen*TITLE_CHAR_WIDTH* pPrintParam->DPI/epsonCaliSetting.printerSetting.DPI_X)/2,
											LEFTCHECK_TITLE_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, TITLE_CHAR_WIDTH);
							xOffset_base += NOZZLE_LINE_COUNT_PER_HEAD * xLenPerLine;
						}
						
						for(j=0; j<seg; j++)
						{
							INT16S Start, End;
							INT8S cur_shift = GetAngleShift((INT8S)UnitCount/2 - i + shift_x, j, &Start, &End);
							
							if(Start >= LEFTCHECK_ENDNOZZLE)
								continue;
							if(End >LEFTCHECK_ENDNOZZLE)
								End = LEFTCHECK_ENDNOZZLE;
							
							temp = (xOffset - cur_shift) % divideNum;
							if(temp <0)
								temp +=divideNum;
							if(temp != phaseNO)
								continue;
							
							xOffset_cur = ((i * NOZZLE_LINE_COUNT_PER_HEAD+ cur_lineNo) * xLenPerLine *divideNum + xOffset - cur_shift)/divideNum;
							
							FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, Start, End);
							
							xOffset_cur += ANGLECHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
							for(k=0; k<ANGLECHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; k++)
							{
								FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, Start, End);
								xOffset_cur ++;
							}
						}
					}
				}
				else
				{
					if(!epsonCaliSetting.printerSetting.bOriginIsRight)
						xOffset = pCurXCali->xPosDirOffset.XOffset[headNO][cur_lineNo];
					else
						xOffset = pCurXCali->xNegDirOffset.XOffset[headNO][cur_lineNo];
					xOffset += (headNO * UnitCount * NOZZLE_LINE_COUNT_PER_HEAD * xLenPerLine+ UnitCount)*divideNum;
					xOffset += ANGLECHECK_INFO_MAX_CHAR_NUM * TITLE_CHAR_WIDTH * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					xOffset += ANGLECHECK_UNITINTERVAL * pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
					
					phaseNO = pass_NO_in_sub_cycle;
					if( !epsonCaliSetting.printerSetting.bOriginIsRight)
						phaseNO = (divideNum - phaseNO) % divideNum;
					
					for(i=0; i<UnitCount; i++)
					{
						INT8U seg = GetAngleShiftSegment((INT8S)UnitCount/2 - i + shift_x);
						
						if(cur_lineNo == 0)
						{
							digitalLen = GetDigitalString(UnitCount - 1 - i, UnitCount, digitalBuff);
							DrawASCIIString(pBuff, pPrintParam, pCurXCali,
											xOffset_base + (NOZZLE_LINE_COUNT_PER_HEAD * xLenPerLine - digitalLen*TITLE_CHAR_WIDTH* pPrintParam->DPI/epsonCaliSetting.printerSetting.DPI_X)/2,
											LEFTCHECK_TITLE_STARTNOZZLE, 3, digitalBuff, pRuntimeParam->curDir, TITLE_CHAR_WIDTH);
							xOffset_base += NOZZLE_LINE_COUNT_PER_HEAD * xLenPerLine;
						}
						
						for(j=0; j<seg; j++)
						{
							INT16S Start, End;
							INT8S cur_shift = GetAngleShift((INT8S)UnitCount/2 - i + shift_x, j, &Start, &End);
							
							if(Start >= LEFTCHECK_ENDNOZZLE)
								continue;
							if(End >LEFTCHECK_ENDNOZZLE)
								End = LEFTCHECK_ENDNOZZLE;
							
							temp = (xOffset - cur_shift) % divideNum;
							if(temp <0)
								temp +=divideNum;
							temp = (divideNum - temp) % divideNum;
							if(temp != phaseNO)
							{
								continue;
							}
							
							xOffset_cur = (i * NOZZLE_LINE_COUNT_PER_HEAD+ (NOZZLE_LINE_COUNT_PER_HEAD - 1 - cur_lineNo)) * xLenPerLine;
							xOffset_cur *= divideNum;
							xOffset_cur += xOffset - cur_shift;
							xOffset_cur /= divideNum;
							if((xOffset - cur_shift) % divideNum != 0)
								xOffset_cur ++;
							for(k=0; k<ANGLECHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X; k++)
							{
								FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, Start, End);
								xOffset_cur ++;
							}
							xOffset_cur--;
							xOffset_cur += ANGLECHECK_LINECOUNT* pPrintParam->DPI / epsonCaliSetting.printerSetting.DPI_X;
							
							FillVerLineEx(cur_lineNo + headNO *NOZZLE_LINE_COUNT_PER_HEAD, xOffset_cur, pBuff, dotsize, Start, End);
						}
					}
				}
			}
			if(bLeft)
			{
				DrawASCIIString(pBuff, pPrintParam, pCurXCali, xOffset_base,
								0, 3, sub_titleBuff, pRuntimeParam->curDir, TITLE_CHAR_WIDTH);
			}
		}
		epsonCaliSetting.printerSetting.BaseColor_HeadIndex = HeadIndex_Backup;
	}
	
	return True;
}
#endif

void CopyVerNozzleLine(INT8U LineIndex, INT32S xOffset, INT8U * pBuff, INT8U * pCopiedBuff)
{
	INT8U * pFireBuff;
	pFireBuff = GetFireBuffEx(xOffset, pBuff, LineIndex);
	if(pFireBuff==0)
		return;
	memcpy(pBuff, pCopiedBuff, epsonCaliSetting.printerSetting.BytesPerFire);
}

INT8U GenHWDynOffsetTestBand(INT8U * pBuff, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U cur_lineNo;
	INT8U i,j, dotsize, *pSrc, *pDest;
	INT16U Loading = pPrintParam->Option, current_Weight;
	INT32S k,l;
	
	if(pPrintParam->VSDModel == 2)
		dotsize = 0x3;
	else
		dotsize = pPrintParam->DotSetting;
	
	for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			cur_lineNo = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[i][j];
			
			current_Weight = 0;
			for(l = 0; l< NOZZLE_COUNT_PER_LINE; l++)
			{
				current_Weight += Loading;
				if(current_Weight >= 100)
				{
					current_Weight -= 100;
					FillNozzle(cur_lineNo, l, 0, pBuff, dotsize);
				}
			}
		}
	}
	
	pSrc = GetFireBuff(0, pBuff);
	for(k = 1; k< HWTEST_DYN_WIDTH; k++)
	{
		pDest = GetFireBuff(k, pBuff);
		memcpy(pDest, pSrc, EPSON_DATA_PER_FIRE);
	}
	return True;
}

//memset in runtime lib is too fast and spent too mush bus width. 
//  it make DMA from DDR2 to FPGA is too slow and make up "internal error 1".
void My_memset(INT8U * s, INT8U b, int size)
{
	int size_int = size/4;
	//#ifdef RICOH_G5_3H
	//  int b_int = 0x0003;
	//#else  
	int b_int = b + (b<<8) + (b<<16) + (b<<24);
	//#endif
	int* s_int = (int*)s;
	while(size_int > 0)
	{
		*s_int = b_int;
		s_int ++;
		size_int --;
		if(size_int % 0x100 == 0)
		{
#ifdef EPSON_4H
			if(rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND && (*(volatile unsigned short*)(FPGA_FIFO_ADDR + 0x10)) <= 0x400)
			{
				//DMA is busy (from DDR2 to FPGA)
				//delay.
				for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
			}
#endif

#ifdef HEAD_RICOH_G4
		while(!PIO_Get(&Pin_FPGA_nHalfEmpty) && rFPGA_COMMAND == rFPGA_COMMAND_BEGIN_BAND )
						for(my_dummy_tick=0; my_dummy_tick<0x1000; my_dummy_tick++);
#endif

		}
	}
}

void ClearPrintBuf(INT32S PassBuffSize)
{
	INT8U *pBuf;
	OS_CPU_SR  cpu_sr;	
	
	OS_ENTER_CRITICAL();	
	pBuf = HugeBuf_GetRecAddr();
	OS_EXIT_CRITICAL();	
	
	if(pBuf + PassBuffSize <= (INT8U*)HUGEBUF_END_ADDR)
	{
		//memset(pBuf, 0, PassBuffSize);
		My_memset(pBuf, 0, PassBuffSize);
	}
	else
	{
		//memset(pBuf, 0, (INT8U*)HUGEBUF_END_ADDR - pBuf);
		//memset((INT8U*)HUGEBUF_START_ADDR, 0, PassBuffSize - ((INT8U*)HUGEBUF_END_ADDR - pBuf));
		My_memset(pBuf, 0, (INT8U*)HUGEBUF_END_ADDR - pBuf);
		My_memset((INT8U*)HUGEBUF_START_ADDR, 0, PassBuffSize - ((INT8U*)HUGEBUF_END_ADDR - pBuf));
	}
}

void CopyCacheBuf()
{
	INT16S len;
	
	if(runtimeParam.bCacheBuff)
	{
		len = (INT8U *)HUGEBUF_END_ADDR - runtimeParam.pTargetBuf;
		
		memcpy(runtimeParam.pTargetBuf, runtimeParam.FireBuf, len);
		memcpy((INT8U *)HUGEBUF_START_ADDR, runtimeParam.FireBuf+len, epsonCaliSetting.printerSetting.BytesPerFire - len);
	}
}

INT8U GenBand(INT8U * pBuff, enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam,
			  struct CaliPatternRuntimeParam * pRuntimeParam)
{
	ClearPrintBuf(pRuntimeParam->PassBuffSize);
	//#ifdef RICOH_G5_3H
	//    return TRUE;
	//#endif
	
	switch (type)
	{
	case DotCheck:
		GenDotCheckBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	case NozzleCheck:
		GenCheckNozzleBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	case StepCheck:
		if((pPrintParam->Option & 0x1) == 0)
			GenYStepBand(pBuff, pPrintParam, pRuntimeParam);
		else
			GenYStepBand_EM(pBuff, pPrintParam, pRuntimeParam);
		break;
	case StepCheck_Micro:
		GenYStepMicroBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	case AngleCheck:
		GenAngleCheckBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	case VerticalCheck:
		GenVerticalCheckBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	case InterleaveCheck:
		if( (pPrintParam->Option & 0x10) == 0)
			GenInterleaveCheckBand(pBuff, pPrintParam, pRuntimeParam);
		else
			GenFullInterleaveCheckBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	case GroupOffsetCheck:
		GenGroupOffsetCheckBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	case LeftCheck:
		GenLeftRightAlignBand(pBuff, pPrintParam, pRuntimeParam, True);
		break;
	case RightCheck:
		GenLeftRightAlignBand(pBuff, pPrintParam, pRuntimeParam, False);
		break;
	case BiDirCheck:
		GenBiDirAlignBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	case HeadYIntervalCheck:
		GenYIntervalCheckBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	case VerticalAlignCheck:
		GenVerticalAlignCheckBand(pBuff, pPrintParam, pRuntimeParam);
		break;
#ifdef HEAD_EPSON_GEN5
	case AngleLeftCheck:
		GenLeftRightAngleBand(pBuff, pPrintParam, pRuntimeParam, True);
		break;
	case AngleRightCheck:
		GenLeftRightAngleBand(pBuff, pPrintParam, pRuntimeParam, False);
		break;
#endif
	case HWDynOffsetTest:
		GenHWDynOffsetTestBand(pBuff, pPrintParam, pRuntimeParam);
		break;
	default:
		return 0;
	}
	CopyCacheBuf();
	return True;
}

INT8U GenJobEnd(INT8U * pBuff, enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam)
{
	DataHeaderType * pHead = (DataHeaderType *)pBuff;
	pHead->header_flag = 'BYHX';
	pHead->header_size = HUGEBUF_ALIGN_SIZE;
	pHead->header_type = JOB_END_HEAD;
	pHead->data_bytescnt = 0;
	
	return True;
}

INT8U MonitorPrintStatus(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam, struct CaliPatternRuntimeParam * pRuntimeParam)
{
	INT8U err;
	if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_CANCEL, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
		return 1;
	else
		return 0;
}

INT8U CancelCaliPrint(void)
{
	return 0;
}

INT8U StartPrintStatus(void)
{
	return USBD_AbortDataRead(USB_OUT_EP);	
}
INT8U EndPrintStatus(void)
{
	return 0;
}

INT8U StartPrintCali(enum Cali_Pattern_Type type, struct CaliPrintParam* pPrintParam)
{
	INT16U passcount = GetPassNum(type, pPrintParam);
	INT8U bExit,i,j;
	INT8U BaseColor_HeadIndex_backup = epsonCaliSetting.printerSetting.BaseColor_HeadIndex;
	
	if( type == BiDirCheck || type == LeftCheck || type == RightCheck || type == GroupOffsetCheck)
		epsonCaliSetting.printerSetting.BaseColor_HeadIndex = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
	else if((type == StepCheck && (pPrintParam->Option & 0x1) == 0) || type == StepCheck_Micro)
	{
		INT8U passnum = pPrintParam->PassNum;
		INT16U nozzle_per_pass;
		INT8U groupNo;
		INT8S head_group = epsonCaliSetting.printerSetting.head_group;
		if(passnum == 1)
			passnum = STEPCHECK_BASEPASSNUM;
		nozzle_per_pass = epsonCaliSetting.printerSetting.GroupCount_YContinue*NOZZLE_COUNT_PER_LINE/passnum;
		groupNo = (nozzle_per_pass + STEPCHECK_STARTNOZZLE+ STEPCHECK_NOZZLECOUNT*STEPCHECK_NOZZLESTEP + 1)/NOZZLE_COUNT_PER_LINE;
		
		epsonCaliSetting.printerSetting.BaseColor_HeadIndex =
			GetHeadIndexWithInterleave(epsonCaliSetting.printerSetting.GroupCount_YContinue - 1 - groupNo,
									   epsonCaliSetting.printerSetting.BaseColor_ColorCode, 0);
	}
	epsonCaliSetting.printerSetting.PrintRegionWidth = (INT32U)((LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX) * pPrintParam->DPI);
	
	CalcRuntimeParam();
	
	runtimeParam.curBandY = 0;
	runtimeParam.pCurXCali = &epsonCaliSetting_runtime.xOffset;
	runtimeParam.curpassNO =0;
	runtimeParam.curDir = 1;
	memset(&runtimeParam.LineOffsetTable, 0, sizeof(runtimeParam.LineOffsetTable));
	for(i=0; i<EPSON_ACTIVE_GROUPNUM*epsonCaliSetting.printerSetting.head_group; i++)
	{
		for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
		{
			runtimeParam.LineOffsetTable[IndexShiftMap[i*NOZZLE_LINE_COUNT_PER_HEAD+j]] = 
				epsonCaliSetting_runtime.xOffset.xPosDirOffset.XOffset[i][j];
			runtimeParam.LineOffsetTable[IndexShiftMap[i*NOZZLE_LINE_COUNT_PER_HEAD+j]] /= ((pPrintParam->Option & 0x6) >> 1) + 1;
		}
	}
	runtimeParam.AllHeadWidth = epsonCaliSetting_runtime.xOffset.PosDirAllHeadWidth /= ((pPrintParam->Option & 0x6) >> 1) + 1;
	runtimeParam.PatternShift = 0;
	
	StartPrintStatus();
	
	bExit = False;
	for(runtimeParam.curpassNO =0; runtimeParam.curpassNO< passcount; runtimeParam.curpassNO++)
	{
		INT8U *pBuff;
		INT32U JobHeadSize,BandHeadSize,JobEndSize;
		
		if(runtimeParam.curpassNO ==0)
		{
			JobHeadSize = GetCaliJobHeadSize();
			pBuff = LockPrintBuff(JobHeadSize);
			if(pBuff == NULL)
				goto checkCancel;
			GenJobHead(pBuff, type, pPrintParam);
			UnlockPrintBuff(pBuff, JobHeadSize);
		}
		
		BandHeadSize = GetCaliBandHeadSize();
		pBuff = LockPrintBuff(BandHeadSize);
		if(pBuff == NULL)
			goto checkCancel;
		GenBandHead(pBuff, type, pPrintParam, &runtimeParam);
		UnlockPrintBuff(pBuff, BandHeadSize);
		
		pBuff = LockPrintBuff(runtimeParam.PassBuffSize);
		if(pBuff == NULL)
			goto checkCancel;
		GenBand(pBuff, type, pPrintParam, &runtimeParam);
		UnlockPrintBuff(pBuff, runtimeParam.PassBuffSize);
		
		if(runtimeParam.curpassNO == passcount-1)
		{
			JobEndSize = GetCaliJobEndSize();
			pBuff = LockPrintBuff(JobEndSize);
			if(pBuff == NULL)
				goto checkCancel;
			GenJobEnd(pBuff, type, pPrintParam);
			UnlockPrintBuff(pBuff, JobEndSize);
			//bExit = True;
		}
		
	checkCancel:		
		switch(MonitorPrintStatus(type, pPrintParam, &runtimeParam))
		{
		case 1:
			CancelCaliPrint();
			bExit = True;
			break;
		}
		if(bExit)
			break;
	}
	EndPrintStatus();
	
	epsonCaliSetting.printerSetting.BaseColor_HeadIndex = BaseColor_HeadIndex_backup;
	return bExit;
}


void Cali_Task(void* data)
{
	INT8U err;
	OS_CPU_SR  cpu_sr;	
	enum Cali_Pattern_Type type;
	INT8U tmp[32];
	struct CaliPrintParam caliParam_backup;
	INT32S passAdvance_backup;
	
#ifdef HEAD_RICOH_G4
	factoryDataEx.Vsd2ToVsd3_ColorDeep = 0;
#endif
	while (1)
	{
		OSFlagPend(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_SET_ANY, 0, &err);
		
		OS_ENTER_CRITICAL();
		type = CaliPrintSettingFromUSB.type;
		if(type == HWDynOffsetTest)
		{
			caliParam_backup = caliParam;
		}
		caliParam.VSDModel = CaliPrintSettingFromUSB.VSDModel;
#if defined(HEAD_RICOH_G4)&&defined(ONE_BIT_DATA)
			if(caliParam.VSDModel == 2)
				RIPSTAR_PRT_BIT = 2;
			else
				RIPSTAR_PRT_BIT = 1;
#endif
		
		switch(CaliPrintSettingFromUSB.DPIModel)
		{
		case 1:
			caliParam.DPI = epsonCaliSetting.printerSetting.DPI_X;
			break;
		case 2:
		default:
			caliParam.DPI = epsonCaliSetting.printerSetting.DPI_X/2;
			break;
#if defined( HEAD_EPSON_GEN5)
#ifndef EPSON_4H
		case 3:
			caliParam.DPI = epsonCaliSetting.printerSetting.DPI_X/4*3;
			break;
		case 4:
			caliParam.DPI = epsonCaliSetting.printerSetting.DPI_X/4*3/2;
			break;
		case 5:
			caliParam.DPI = epsonCaliSetting.printerSetting.DPI_X * 2;
			break;
#endif
#else

		case 3:
#ifdef DYSS_1270
			caliParam.DPI = epsonCaliSetting.printerSetting.DPI_X/4;
#else
			caliParam.DPI = epsonCaliSetting.printerSetting.DPI_X/3;
#endif
			break;
#endif
		}
		
		caliParam.StartPosX = CaliPrintSettingFromUSB.StartPos;
		caliParam.DotSetting = CaliPrintSettingFromUSB.DotSetting;
		if(caliParam.DotSetting == 0)
			caliParam.DotSetting = 3;
		caliParam.MediaType = CaliPrintSettingFromUSB.MediaType;
		caliParam.PassNum = CaliPrintSettingFromUSB.PassNum;
		caliParam.Option = CaliPrintSettingFromUSB.Option;
		
		OS_EXIT_CRITICAL();	
		
		//		for( i=0; i<=(14000-5750)/250; i++)
		{
			if(type == HWDynOffsetTest)
			{
				tmp[0] = 5;
				tmp[1] = UART_HEAD_EPSON_OUT_WAVE;
				tmp[2] = caliParam.VSDModel;
				tmp[3] = caliParam.Option;
				tmp[4] = 0; //wave value.
				UART_SendCMD(UART_HEAD_CHANNEL, tmp);
			}
			//			tmp[0] = 5;
			//			tmp[1] = UART_HEAD_EPSON_SET_RES;
			//			tmp[2] = caliParam.VSDModel;
			//			tmp[3] = (INT8U)(i * 250 + 5750);
			//			tmp[4] = (INT8U)((i * 250 + 5750)>>8);
			//			UART_SendCMD(UART_HEAD_CHANNEL, tmp);
			
			passAdvance_backup = g_adjustInf.passAdvance;
			//if(type == StepCheck || type == StepCheck_Micro)
			g_adjustInf.passAdvance = 0;
			
			if(!StartPrintCali(type, &caliParam))
			{
				
				while(1)
				{
					OSFlagPend(status_FLAG_GRP,STATUS_PRINT,OS_FLAG_CLR,100,&err);
					if(err != OS_NO_ERR)
					{
						if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_CANCEL, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
						{
							break;
						}
					}
					else
						break;
				}
			}
			//if(type == StepCheck || type == StepCheck_Micro)
			g_adjustInf.passAdvance = passAdvance_backup;
			
			if(type == HWDynOffsetTest)
				caliParam = caliParam_backup;
			//			OSTimeDly(10000);
		}
		OSFlagPost(mix_FLAG_GRP, EPSON_CALI_RUN,OS_FLAG_CLR,&err);
	}
}
#endif

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
INT8U StartCalibrationJob(struct CaliPrintSetting * pInfo)
{
	OS_CPU_SR  cpu_sr;	
	INT8U err;
	
	if((pInfo->type == StepCheck_Micro || pInfo->type == AngleLeftCheck ||
		pInfo->type == AngleRightCheck)&&
	   factoryData.HeadType != HeadNo_Epson_Gen5)
	{ //for ricoh, the above is not implement still.
		return False;
	}
	
	OS_ENTER_CRITICAL();
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_PRINT_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		{
			memcpy((void*)&CaliPrintSettingFromUSB, pInfo, sizeof(struct CaliPrintSetting));
			OS_EXIT_CRITICAL();	
			
			OSFlagPost(mix_FLAG_GRP, EPSON_CALI_RUN,OS_FLAG_SET,&err);
			return True;
		}
	}
	OS_EXIT_CRITICAL();	
	return False;
}

INT8U StartHWDynOffsetJob(INT8U VsdModel, INT8U Loading)
{
	OS_CPU_SR  cpu_sr;	
	INT8U err;
	
	OS_ENTER_CRITICAL();
	if (OSFlagAccept(status_FLAG_GRP, STATUS_NO_PRINT_BITS, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
	{
		if(OSFlagAccept(mix_FLAG_GRP, EPSON_CALI_RUN, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
		{
			memset((void*)&CaliPrintSettingFromUSB, 0, sizeof(struct CaliPrintSetting));
			CaliPrintSettingFromUSB.type = HWDynOffsetTest;
			CaliPrintSettingFromUSB.VSDModel = VsdModel;
			if(Loading >100)
				Loading = 100;
			if(CaliPrintSettingFromUSB.VSDModel == 2)
			{
				CaliPrintSettingFromUSB.DotSetting = 3;
				CaliPrintSettingFromUSB.Option = Loading;
			}
			else
			{
				if(Loading>67)
				{
					CaliPrintSettingFromUSB.DotSetting = 3;
					Loading -= 67;
					Loading *= 3;
				}
				else if(Loading>33)
				{
					CaliPrintSettingFromUSB.DotSetting = 2;
					Loading -= 33;
					Loading *= 3;
				}
				else
				{
					CaliPrintSettingFromUSB.DotSetting = 1;
					Loading *= 3;
				}
			}
			if(Loading >100)
				Loading = 100;
			CaliPrintSettingFromUSB.Option = Loading;
			CaliPrintSettingFromUSB.DPIModel = 1;
			CaliPrintSettingFromUSB.StartPos = 0;
			CaliPrintSettingFromUSB.MediaType = 0;
			CaliPrintSettingFromUSB.PassNum = 4;
			OS_EXIT_CRITICAL();	
			
			OSFlagPost(mix_FLAG_GRP, EPSON_CALI_RUN,OS_FLAG_SET,&err);
			return True;
		}
	}
	OS_EXIT_CRITICAL();	
	return False;
}

INT8U SaveEPSONCaliSetting()
{
	INT8U ret = True;
	INT16U len = sizeof(struct EPR_EPSON_CaliSetting);
	
	EPM_Head head =
	{
		EPR_EPSON_CALI_SETTING_FLAG,
		EPR_EPSON_CALI_SETTING_VERSION,
		sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head),
		0
	};
	
	CalcCheckSum((INT8U*)&epsonCaliSetting.eprCaliSetting, &head);
	ret = IIC_WriteEEPRom_Head(EPR_CALIBRATION_SETTING_OFFSET, &head);
	
	ret &= IIC_WriteEEPRom_Ex(EPR_CALIBRATION_SETTING_OFFSET + sizeof(EPM_Head), (INT8U *)&epsonCaliSetting.eprCaliSetting, &len);
	return ret;
}

//Printer configure part
INT8U GetGroupNumber()
{
	return EPSON_ACTIVE_GROUPNUM;
}

INT8U GetGroupHeadNumber()
{
	INT8U YInterleaveNum_InHead;
	
	if( factoryDataEx.YInterleaveNum > INTERLEAVE_CALIBRATION) 
		YInterleaveNum_InHead = INTERLEAVE_CALIBRATION;
	else
		YInterleaveNum_InHead = factoryDataEx.YInterleaveNum;
	
	INT8S head_group = (COLOR_NUMBER_CALIBRATION * YInterleaveNum_InHead + NOZZLE_LINE_COUNT_PER_HEAD - 1)/
		NOZZLE_LINE_COUNT_PER_HEAD;
	
	return head_group;
}

INT8U GetColorNumber()
{
	return COLOR_NUMBER_CALIBRATION;
}

INT8U IsWeakSolventPrinter()
{
	return factoryDataEx.m_nBitFlagEx & 0x2;
}

INT8U IsIntegradeHead()
{
	return epsonCaliSetting.printerSetting.bIntegratedHead;
}

INT8U IsYIntervalReplaceVertical()
{
	return epsonCaliSetting.printerSetting.bYIntervalReplaceVertical;
}


//calibration part.
//user model of calibration
INT8U Cali_UM_CheckNozzle(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = NozzleCheck;
	Info.VSDModel = CALI_VSD_DEFAULT;
	Info.DotSetting = 3;
	Info.DPIModel = 1; //720 dpi for slow speed.
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif	
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 0;
	return StartCalibrationJob(&Info);
}

// Index: 0:GlossPaper, 1:Vinyl, 2:PP, 3:Film, 4:Other1, 5:Other2, 6:Other3.
INT8U Cali_UM_Print_Step(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = StepCheck;
	Info.VSDModel = CALI_VSD_DEFAULT;
	Info.DotSetting = 3;
	Info.DPIModel = 1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = Index;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 0;
	return StartCalibrationJob(&Info);
}

INT32S Cali_UM_Get_Step(void * pData, int Index)
{
	INT8U err;
	INT32S step;
	OSSemPend(CalibrationParamSem, 0, &err);
	step = epsonCaliSetting.eprCaliSetting.stepPass[Index].BaseStep;
	step += epsonCaliSetting.eprCaliSetting.stepPass[Index].PassStep[STEPCHECK_BASEPASSNUM-1] * STEPCHECK_BASEPASSNUM;
	OSSemPost(CalibrationParamSem);
	return step;
}

INT8U Cali_UM_Set_Step_Modify(void * pData, int Index, float Data)
{
	double step;
	INT8U err;
	INT32U nozzle;
	
	if(pData == (void *)-1)
	{
		return True;
	}
	
	OSSemPend(CalibrationParamSem, 0, &err);
	
	step = epsonCaliSetting.eprCaliSetting.stepPass[Index].BaseStep;
	step += epsonCaliSetting.eprCaliSetting.stepPass[Index].PassStep[STEPCHECK_BASEPASSNUM-1] * STEPCHECK_BASEPASSNUM;
	nozzle = (epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum *  
			  epsonCaliSetting.printerSetting.NozzleCount /STEPCHECK_BASEPASSNUM);
	if(Data > (((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum) )
		Data = (float)nozzle;
	else if(Data < - (((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum)/2)
		Data = - (((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum)/2;
	step = step * ((((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum) + Data) / 
		(((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum);
	if((INT32U)step == 0)
		step = 1;
	epsonCaliSetting.eprCaliSetting.stepPass[Index].BaseStep = (INT32U)step;
	epsonCaliSetting.eprCaliSetting.stepPass[Index].PassStep[STEPCHECK_BASEPASSNUM-1] = 0;
	bCaliSettingDirty = True;
	
	OSSemPost(CalibrationParamSem);
	
	return True;
}

float Cali_UM_Get_Step_Modify(void * pData, int Index)
{
	return 0.0;
}

//Index: 0~3:VSD1~VSD4
INT8U Cali_UM_Print_BiDir(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = BiDirCheck;
	Info.VSDModel = Index+1;
	Info.DotSetting = 2;
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && Info.VSDModel == 2)
	{
		Info.VSDModel = factoryDataEx.Vsd2ToVsd3 + 1;
		Info.DotSetting = factoryDataEx.Vsd2ToVsd3_ColorDeep;
	}
#endif
	
#ifdef RICOH_VSD2
	if(Info.VSDModel == 2 ||Info.VSDModel == 3)
		Info.DPIModel = 1;
	else
#endif
		Info.DPIModel = DEFAULT_DPI2_TO;
#ifdef DYSS_1270
	Info.DPIModel = 2 ; //因为get set is 2
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType =  LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	//Info.Option = 2;
	Info.Option = 0;
	return StartCalibrationJob(&Info);
}

INT8U Cali_UM_Set_BiDir(void * pData, int Index, int Data)
{
	INT32S BiDir;
	struct EPSON_X_Cali *pXCali;
	INT8U headNO, lineNO, HeightLevel;
	INT8U err;
	
	if(pData == (void *)-1)
	{
		return True;
	}
	
	headNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
	lineNO = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
	
	HeightLevel = Index/10;
	Index = Index%10;
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && Index == 1)
	{
		Index = factoryDataEx.Vsd2ToVsd3;
	}
#endif 	
	OSSemPend(CalibrationParamSem, 0, &err);
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	Index = Index*MAX_SPEED_LEVEL_EACH_VSD + LCDMenuConfig_EX.Calc_X_Speed;
#endif
	
	pXCali = &epsonCaliSetting.eprCaliSetting.xOffset[HeightLevel][GetResIndex(epsonCaliSetting.printerSetting.DPI_X/2)][Index]; //360DPI or 300DPI.
	BiDir = pXCali->xNegDirOffset.XOffset[headNO][lineNO];
	
	pXCali->nDPI = epsonCaliSetting.printerSetting.DPI_X/2;
	if(Data != BiDir)
	{
		for(headNO = 0; headNO < epsonCaliSetting.printerSetting.HeadCount; headNO++)
		{
			for(lineNO = 0; lineNO < NOZZLE_LINE_COUNT_PER_HEAD; lineNO ++)
			{
				pXCali->xNegDirOffset.XOffset[headNO][lineNO] += (Data-BiDir);
			}
		}
		bCaliSettingDirty = True;
	}
	
	OSSemPost(CalibrationParamSem);
	
	return True;
}

INT32S Cali_UM_Get_BiDir(void * pData, int Index)
{
	INT32S BiDir;
	INT8U headNO, lineNO;
	struct EPSON_X_Cali *pXCali;
	INT8U err, HeightLevel;
	
	headNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
	lineNO = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
	
	HeightLevel = Index/10;
	Index = Index%10;
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && Index == 1)
	{
		Index = factoryDataEx.Vsd2ToVsd3;
	}
#endif 	
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	Index = Index*MAX_SPEED_LEVEL_EACH_VSD + LCDMenuConfig_EX.Calc_X_Speed;
#endif
	OSSemPend(CalibrationParamSem, 0, &err);
	pXCali = &epsonCaliSetting.eprCaliSetting.xOffset[HeightLevel][GetResIndex(epsonCaliSetting.printerSetting.DPI_X/2)][Index]; //360DPI.
	BiDir = pXCali->xNegDirOffset.XOffset[headNO][lineNO];
	OSSemPost(CalibrationParamSem);
	
	return BiDir;
}

INT8U Cali_Print_GroupSpace(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = GroupOffsetCheck;
	Info.VSDModel = Index + 1;
	Info.DotSetting = 2;
	Info.DPIModel = 1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType =  LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 0;
	return StartCalibrationJob(&Info);
}

INT8U Cali_Set_GroupSpace(void * pData, int Index, int Data)
{
	INT8U err,j,k;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	Index = Index%1000 + ((Index/1000)*MAX_SPEED_LEVEL_EACH_VSD + LCDMenuConfig_EX.Calc_X_Speed)*1000;
#endif
	
	OSSemPend(CalibrationParamSem, 0, &err);
	
	if(epsonCaliSetting.printerSetting.bIntegratedHead)
	{
		if(Data != epsonCaliSetting.eprCaliSetting.headOffset.XOffset[Index/1000][Index%1000])
		{
			epsonCaliSetting.eprCaliSetting.headOffset.XOffset[Index/1000][Index%1000] = Data;
			bCaliSettingDirty = True;
		}
	}
	else if(epsonCaliSetting.printerSetting.YInterleaveNum_Head == epsonCaliSetting.printerSetting.YInterleaveNum)
	{
		INT8U headNO, lineNo, i;
		headNO = GetHeadIndexWithInterleave(((Index%1000) / 100) % epsonCaliSetting.printerSetting.GroupCount_YContinue, 
											factoryDataEx.m_nColorOrder[Index % 100], 
											((Index%1000) / 100) / epsonCaliSetting.printerSetting.GroupCount_YContinue);
		if(Data != epsonCaliSetting.eprCaliSetting.headOffset.XOffset[Index/1000][headNO])
		{
			epsonCaliSetting.eprCaliSetting.headOffset.XOffset[Index/1000][headNO] = Data;
			bCaliSettingDirty = True;
		}
	}
	else
	{
		INT8U headNO, lineNo, i;
		i=(Index % 100)* NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
		headNO = GetHeadIndexWithInterleave(((Index%1000) / 100) % epsonCaliSetting.printerSetting.GroupCount_YContinue, 
											factoryDataEx.m_nColorOrder[i],
											((Index%1000) / 100) / epsonCaliSetting.printerSetting.GroupCount_YContinue);
		if(Data != epsonCaliSetting.eprCaliSetting.headOffset.XOffset[Index/1000][headNO] )
		{
			epsonCaliSetting.eprCaliSetting.headOffset.XOffset[Index/1000][headNO] = Data;
			bCaliSettingDirty = True;
		}
	}
	
	OSSemPost(CalibrationParamSem);
	
	return True;
}

INT32S Cali_Get_GroupSpace(void * pData, int Index)
{
	INT32S Offset;
	INT8U err;
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	Index = Index%1000 + ((Index/1000)*MAX_SPEED_LEVEL_EACH_VSD + LCDMenuConfig_EX.Calc_X_Speed)*1000;
#endif
	
	OSSemPend(CalibrationParamSem, 0, &err);
	if(epsonCaliSetting.printerSetting.bIntegratedHead)
	{
		Offset = epsonCaliSetting.eprCaliSetting.headOffset.XOffset[Index/1000][Index%1000];
	}
	else if(NOZZLE_LINE_COUNT_PER_HEAD == epsonCaliSetting.printerSetting.YInterleaveNum_InHead)
	{
		INT8U headNO, lineNo, i;
		headNO = GetHeadIndexWithInterleave(((Index%1000) / 100) % epsonCaliSetting.printerSetting.GroupCount_YContinue, 
											factoryDataEx.m_nColorOrder[Index % 100], 
											((Index%1000) / 100) / epsonCaliSetting.printerSetting.GroupCount_YContinue);
		Offset = epsonCaliSetting.eprCaliSetting.headOffset.XOffset[Index/1000][headNO];
	}
	else
	{
		INT8U headNO, lineNo, i;
		i=(Index % 100)* NOZZLE_LINE_COUNT_PER_HEAD / epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
		headNO = GetHeadIndexWithInterleave(((Index%1000) / 100) % epsonCaliSetting.printerSetting.GroupCount_YContinue, 
											factoryDataEx.m_nColorOrder[i],
											((Index%1000) / 100) / epsonCaliSetting.printerSetting.GroupCount_YContinue);
		Offset = epsonCaliSetting.eprCaliSetting.headOffset.XOffset[Index/1000][headNO];
	}
	
	OSSemPost(CalibrationParamSem);
	
	return Offset;
}

INT8U GetLCDMenuPassNum(INT8U Index)
{
	switch(Index)
	{
	case 0:
		return 2;
	case 1:
		return 3;
	case 2:
	default:
		return STEPCHECK_BASEPASSNUM;
	case 3:
		return 6;
	case 4:
		return 8;
	case 5:
		return 12;
	case 6:
		return 16;
	case 7:
		return 24;
	}
}

INT8U Cali_UM_Print_TestingPrint(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = TestPrint;
	Info.VSDModel = LCDMenuConfig.TestPrint_VSDIndex+1;
	Info.DotSetting = 3;
	Info.DPIModel = LCDMenuConfig.TestPrint_DPIIndex+1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType =  LCDMenuConfig.SheetType;
	Info.PassNum = GetLCDMenuPassNum(LCDMenuConfig.TestPrint_PassIndex);
	Info.Option = 0;
	return StartCalibrationJob(&Info);
}

//Engineer model of calibration
//Index: 0~3:VSD1~VSD4
INT8U Cali_EM_Print_CheckNozzle(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = NozzleCheck;
	Info.VSDModel = Index + 1;
	Info.DotSetting = 3;
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && Info.VSDModel == 2)
	{
		Info.VSDModel = factoryDataEx.Vsd2ToVsd3 + 1;
		Info.DotSetting = factoryDataEx.Vsd2ToVsd3_ColorDeep;
	}
#endif 	
	Info.DPIModel = 1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif	
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

//Index: 0~3:VSD1~VSD4
INT8U Cali_EM_Print_DotCheck(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = DotCheck;
	Info.VSDModel = Index + 1;
	Info.DotSetting = 3;
#ifdef VSD2TOVSD3
	if( factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && Info.VSDModel == 2)
	{
		Info.VSDModel = factoryDataEx.Vsd2ToVsd3 + 1;
		Info.DotSetting = factoryDataEx.Vsd2ToVsd3_ColorDeep;
	}
#endif
	
#ifdef RICOH_VSD2
	if(Info.VSDModel == 2 ||Info.VSDModel == 3)
		Info.DPIModel = 1;
	else
#endif
		Info.DPIModel = DEFAULT_DPI2_TO;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif	
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

INT8U Cali_EM_Print_InterleaveCheck(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = InterleaveCheck;
	Info.VSDModel = CALI_VSD_DEFAULT;
	Info.DotSetting = 3;
	Info.DPIModel = 1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

INT8U Cali_EM_Print_HeadSlantCheck(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = AngleCheck;
	Info.VSDModel = CALI_VSD_DEFAULT;
	Info.DotSetting = 3;
	Info.DPIModel = 1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

INT8U Cali_EM_Print_HeadIntervalCheck(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = HeadYIntervalCheck;
	Info.VSDModel = CALI_VSD_DEFAULT;
	Info.DotSetting = 3;
	Info.DPIModel = 1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

//Radio button:
//Index: 0:2pass, 1:3Pass, 2:4Pass, 3:6Pass. 4:8Pass, 5:12Pass,
INT8U Cali_EM_Get_Step_PassNum(void * pData, int index)
{
	return LCDMenuConfig.Step_PassIndex;
}

INT8U Cali_EM_Set_Step_PassNum(void * pData, int Index, int Data)
{
	if(pData == (void *)-1)
	{
		return True;
	}
	
	LCDMenuConfig.Step_PassIndex = Data;
	SaveLCDMenuConfig();
	return True;
}

const INT8U printed_ColorName[MAX_SPOTCOLOR_NUM][4] =
{
	"S1", "S2", "S3", "S4", "Lc", "Lm", "Ly", "Lk", "Llk", "Mk", "R", "Gr", "B", "Or", "W", "V", "W1", "W2", "W3", "W4"
};

INT8U * GetPrintedSpotColorName(INT8U index)
{
	return (INT8U*) printed_ColorName[index];
}

INT8U UI_EM_Get_SpotColorName(void * pData, int index)
{
	index = index / 100;
	return LCDMenuConfig.SpotColorName[index];
}

INT8U UI_EM_Set_SpotColorName(void * pData, int Index, int Data)
{
	if(pData == (void *)-1)
	{
		return True;
	}
	
	Index = Index / 100;
	LCDMenuConfig.SpotColorName[Index] = Data % 100;
	SaveLCDMenuConfig();
	return True;
}

// Index: 0:GlossPaper, 1:Vinyl, 2:PP, 3:Film, 4:Other1, 5:Other2, 6:Other3.
INT8U Cali_EM_Print_Step(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = StepCheck;
#ifndef UV_PRINTER
	if(factoryData.HeadType == HeadNo_Ricoh_Gen4_G4)
		Info.VSDModel = 3;
	else	
#endif
		Info.VSDModel = CALI_VSD_DEFAULT;
	
#if 1
	Info.DotSetting = 3;
#else
	Info.DotSetting = 2;
#endif
	
#ifdef RICOH_VSD2
	if(Info.VSDModel == 2 ||Info.VSDModel == 3)
		Info.DPIModel = 1;
	else
#endif
		Info.DPIModel = DEFAULT_DPI2_TO;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = Index;
	Info.PassNum = GetLCDMenuPassNum(LCDMenuConfig.Step_PassIndex);
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

INT32S Cali_EM_Get_Step(void * pData, int Index)
{
	INT8U err;
	INT32S step, passnum;
	OSSemPend(CalibrationParamSem, 0, &err);
	step = epsonCaliSetting.eprCaliSetting.stepPass[Index].BaseStep;
	passnum = GetLCDMenuPassNum(LCDMenuConfig.Step_PassIndex);
	step += epsonCaliSetting.eprCaliSetting.stepPass[Index].PassStep[passnum-1] * passnum;
	OSSemPost(CalibrationParamSem);
	return step;
}

INT8U Cali_EM_Set_Step_Modify(void * pData, int Index, float Data)
{
	double step_d;
	INT32U step, step_old;
	INT32S step_modify;
	INT8U err, passnum;
	INT32U nozzle;
	
	if(pData == (void *)-1)
	{
		return True;
	}
	
	OSSemPend(CalibrationParamSem, 0, &err);
	
	passnum = GetLCDMenuPassNum(LCDMenuConfig.Step_PassIndex);
	step = epsonCaliSetting.eprCaliSetting.stepPass[Index].BaseStep;
	step += epsonCaliSetting.eprCaliSetting.stepPass[Index].PassStep[passnum-1] * passnum;
	step_old = step;
	step_d = step;
	
	nozzle = (epsonCaliSetting.printerSetting.GroupCount_YContinue * epsonCaliSetting.printerSetting.YInterleaveNum *  
			  epsonCaliSetting.printerSetting.NozzleCount /passnum);
	if(Data > (((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum) )
		Data = (float)nozzle;
	else if(Data < - (((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum)/2)
		Data = - (((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum)/2;
	step_d = step_d * ((((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum) + Data) / 
		(((float)nozzle)/epsonCaliSetting.printerSetting.YInterleaveNum);
	
	if(passnum == STEPCHECK_BASEPASSNUM)
	{
		step = (INT32U)(step_d + 0.5);
		if(step <= 0)
			step = 1;
		epsonCaliSetting.eprCaliSetting.stepPass[Index].BaseStep = step;
		epsonCaliSetting.eprCaliSetting.stepPass[Index].PassStep[STEPCHECK_BASEPASSNUM-1] = 0;
	}
	else
	{
		step_d -= epsonCaliSetting.eprCaliSetting.stepPass[Index].BaseStep;
		step_d /= passnum;
		if(step_d >= 0)
			step_modify = (INT32S)(step_d + 0.5);
		else
			step_modify = (INT32S)(step_d - 0.5);
		
		epsonCaliSetting.eprCaliSetting.stepPass[Index].PassStep[passnum-1] = (INT16S) step_modify;
	}
	bCaliSettingDirty = True;
	
	OSSemPost(CalibrationParamSem);
	
	return True;
}

float Cali_EM_Get_Step_Modify(void * pData, int Index)
{
	return 0.0;
}

//Index: enum RES_VSD_INDEX { E_360_VSD1=0, E_360_VSD2, E_360_VSD3, E_360_VSD4,
//    E_720_VSD1=10, E_720_VSD2, E_720_VSD3, E_720_VSD4,
//    in the future, there are 540,270,1440.... More, 360speed with 720 precision.
INT8U Cali_EM_Print_Hor_Left(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = LeftCheck;
	Info.VSDModel = Index % 10 + 1;
	Info.DotSetting = 2;
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && Info.VSDModel == 2)
	{
		Info.VSDModel = factoryDataEx.Vsd2ToVsd3 + 1;
		Info.DotSetting = factoryDataEx.Vsd2ToVsd3_ColorDeep;
	}
#endif 			
	switch(Index / 10)
	{
	case 0:
	default:
#ifdef RICOH_VSD2
		if(Info.VSDModel == 2 ||Info.VSDModel == 3)
			Info.DPIModel = 1;
		else
#endif
			Info.DPIModel = 2;
		
#ifdef DYSS_1270
		if(Info.VSDModel == 1)
			Info.DPIModel = 3;
		else
			Info.DPIModel = 2;
#endif
		break;
	case 1:
		Info.DPIModel = 1;
#ifdef DYSS_1270
		if(Info.VSDModel == 1)
			Info.DPIModel = 2;
		else
			Info.DPIModel = 1;
#endif		
		break;
#if defined( HEAD_EPSON_GEN5)
	case 2:
		Info.DPIModel = 4;
		break;
	case 3:
		Info.DPIModel = 3;
		break;
	case 4:
		Info.DPIModel = 5;
		break;
#else
	case 2:
		Info.DPIModel = 3;
		break;
#endif
	}
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType =  LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

INT8U Cali_EM_Print_Hor_BiDir(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = BiDirCheck;
	Info.VSDModel = Index % 10 + 1;
	Info.DotSetting = 2;
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && Info.VSDModel == 2)
	{
		Info.VSDModel = factoryDataEx.Vsd2ToVsd3 + 1;
		Info.DotSetting = factoryDataEx.Vsd2ToVsd3_ColorDeep;
	}
#endif
	switch(Index / 10)
	{
	case 0:
	default:
		
#ifdef RICOH_VSD2
		if(Info.VSDModel == 2 ||Info.VSDModel == 3)
			Info.DPIModel = 1;
		else
#endif
			Info.DPIModel = 2;
#ifdef DYSS_1270
		if(Info.VSDModel == 1)
			Info.DPIModel = 3;
		else
			Info.DPIModel = 2;
#endif		
		break;
	case 1:
		Info.DPIModel = 1;
#ifdef DYSS_1270
		if(Info.VSDModel == 1)
			Info.DPIModel = 2;
		else
			Info.DPIModel = 1;
#endif			
		break;
#if defined( HEAD_EPSON_GEN5)
	case 2:
		Info.DPIModel = 4;
		break;
	case 3:
		Info.DPIModel = 3;
		break;
	case 4:
		Info.DPIModel = 5;
		break;
#else
	case 2:
		Info.DPIModel = 3;
		break;
#endif
	}
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType =  LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

INT8U Cali_EM_Print_Hor_Right(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = RightCheck;
	Info.VSDModel = Index % 10 + 1;
	Info.DotSetting = 2;
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && Info.VSDModel == 2)
	{
		Info.VSDModel = factoryDataEx.Vsd2ToVsd3 + 1;
		Info.DotSetting = factoryDataEx.Vsd2ToVsd3_ColorDeep;
	}
#endif 	
	switch(Index / 10)
	{
	case 0:
	default:
		
#ifdef RICOH_VSD2
		if(Info.VSDModel == 2 ||Info.VSDModel == 3)
			Info.DPIModel = 1;
		else
#endif
			Info.DPIModel = 2;
#ifdef DYSS_1270
		if(Info.VSDModel == 1)
			Info.DPIModel = 3;
		else
			Info.DPIModel = 2;
#endif		
		break;
	case 1:
		Info.DPIModel = 1;
#ifdef DYSS_1270
		if(Info.VSDModel == 1)
			Info.DPIModel = 2;
		else
			Info.DPIModel = 1;
#endif			
		break;
#if defined( HEAD_EPSON_GEN5)
	case 2:
		Info.DPIModel = 4;
		break;
	case 3:
		Info.DPIModel = 3;
		break;
	case 4:
		Info.DPIModel = 5;
		break;
#else
	case 2:
		Info.DPIModel = 3;
		break;
#endif
	}
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType =  LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

//Index: there are three part: the first is Color&Head index, such as K1,K2,C1,C2,..., or H1K1,H1K2....
//  the second is enum RES_VSD_INDEX.
//  the third is head height level(High/Low), 0, high; 1, low
//  the final index = (head height level)*10000+(enum RES_VSD_INDEX)*100+(Color&Head index).
INT32S Cali_EM_Get_Hor_LeftOrRight(void * pData, int Index, INT8U bLeft)
{
	INT32S Offset, vsd_index;
	INT8U groupNO, colorNO, headNO, lineNO, resNO, vsdNO;
	struct EPSON_X_Cali *pXCali;
	INT8U err, HeightLevel;
	
	HeightLevel = Index/10000;
	Index = Index%10000;
	
	vsd_index = Index/100;
	Index = Index%100;
	
	groupNO = Index/(epsonCaliSetting.printerSetting.ColorCount * epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
	INT8U groupNO_YContinue = groupNO % epsonCaliSetting.printerSetting.GroupCount_YContinue;
	INT8U groupNO_YInterleave = groupNO / epsonCaliSetting.printerSetting.GroupCount_YContinue;
	Index = Index%(epsonCaliSetting.printerSetting.ColorCount * epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
	colorNO = Index / epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
	INT8U YInterleaveNO_InHead = Index % epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
	INT8U YInterleaveNO = groupNO_YInterleave + YInterleaveNO_InHead * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	lineNO = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[colorNO], YInterleaveNO);
	headNO = GetHeadIndexWithInterleave(groupNO_YContinue, factoryDataEx.m_nColorOrder[colorNO], YInterleaveNO);
	
	resNO = vsd_index/10;
	vsdNO = vsd_index%10;
	switch(resNO)
	{
	case 0:
	default:
		resNO = 1;
#ifdef DYSS_1270
		if(vsdNO == 0)
			resNO = 2;
		else
			resNO = 1;
#endif		
		break;
	case 1:
		resNO = 0;
#ifdef DYSS_1270
		if(vsdNO == 0)
			resNO = 1;
		else
			resNO = 0;
#endif			
		break;
#if defined( HEAD_EPSON_GEN5)
	case 2:
		resNO = 3;
		break;
	case 3:
		resNO = 2;
		break;
	case 4:
		resNO = 4;
		break;
#else
	case 2:
		resNO = 2;
		break;
#endif
	}
	
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && vsdNO == 1)
	{
		vsdNO = factoryDataEx.Vsd2ToVsd3;
	}
#endif
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	vsdNO = vsdNO*MAX_SPEED_LEVEL_EACH_VSD + LCDMenuConfig_EX.Calc_X_Speed;
#endif
	OSSemPend(CalibrationParamSem, 0, &err);
	
	pXCali = &epsonCaliSetting.eprCaliSetting.xOffset[HeightLevel][resNO][vsdNO]; //360DPI.
	if(epsonCaliSetting.printerSetting.bOriginIsRight == bLeft)
	{
		INT8U lineindex_base,headindex_base,lineindex_allbase,headindex_allbase;
		
		headindex_allbase = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
		lineindex_allbase = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
		
		headindex_base = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNO]];
		lineindex_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNO]];
		
		if( headindex_base == headNO && lineindex_base == lineNO)
			Offset = pXCali->xPosDirOffset.XOffset[headNO][lineNO] - pXCali->xPosDirOffset.XOffset[headindex_allbase][lineindex_allbase];
		else
			Offset = pXCali->xPosDirOffset.XOffset[headNO][lineNO] - pXCali->xPosDirOffset.XOffset[headindex_base][lineindex_base];
	}
	else
	{
		INT8U lineindex_base,headindex_base;
		
		headindex_base = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNO]];
		lineindex_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNO]];
		
		Offset = pXCali->xNegDirOffset.XOffset[headNO][lineNO] - pXCali->xPosDirOffset.XOffset[headindex_base][lineindex_base];
	}
	
	OSSemPost(CalibrationParamSem);
	
	return Offset;
}

INT32S Cali_EM_Get_Hor_Left(void * pData, int Index)
{
	return Cali_EM_Get_Hor_LeftOrRight(pData, Index, True);
}

INT32S Cali_EM_Get_Hor_Right(void * pData, int Index)
{
	return Cali_EM_Get_Hor_LeftOrRight(pData, Index, False);
}

INT8U Cali_EM_Set_Hor_LeftOrRight(void * pData, int Index, int Data, INT8U bLeft)
{
	INT32S vsd_index;
	INT8U groupNO, colorNO, headNO, lineNO, resNO, vsdNO;
	struct EPSON_X_Cali *pXCali;
	INT8U err, HeightLevel, i, j;
	signed char old;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	HeightLevel = Index/10000;
	Index = Index%10000;
	
	vsd_index = Index/100;
	Index = Index%100;
	
	groupNO = Index/(epsonCaliSetting.printerSetting.ColorCount * epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
	INT8U groupNO_YContinue = groupNO % epsonCaliSetting.printerSetting.GroupCount_YContinue;
	INT8U groupNO_YInterleave = groupNO / epsonCaliSetting.printerSetting.GroupCount_YContinue;
	Index = Index%(epsonCaliSetting.printerSetting.ColorCount * epsonCaliSetting.printerSetting.YInterleaveNum_InHead);
	colorNO = Index / epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
	INT8U YInterleaveNO_InHead = Index % epsonCaliSetting.printerSetting.YInterleaveNum_InHead;
	INT8U YInterleaveNO = groupNO_YInterleave + YInterleaveNO_InHead * epsonCaliSetting.printerSetting.GroupCount_YInterleave;
	lineNO = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[colorNO], YInterleaveNO);
	headNO = GetHeadIndexWithInterleave(groupNO_YContinue, factoryDataEx.m_nColorOrder[colorNO], YInterleaveNO);
	
	resNO = vsd_index/10;
	vsdNO = vsd_index%10;
	switch(resNO)
	{
	case 0:
	default:
		resNO = 1;
#ifdef DYSS_1270
		if(vsdNO == 0)
			resNO = 2;
		else
			resNO = 1;
#endif		
		break;
	case 1:
		resNO = 0;
#ifdef DYSS_1270
		if(vsdNO == 0)
			resNO = 1;
		else
			resNO = 0;
#endif			
		break;
#if defined( HEAD_EPSON_GEN5)
	case 2:
		resNO = 3;
		break;
	case 3:
		resNO = 2;
		break;
	case 4:
		resNO = 4;
		break;
#else
	case 2:
		resNO = 2;
		break;
#endif
	}
	
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && vsdNO == 1)
	{
		vsdNO = factoryDataEx.Vsd2ToVsd3;
	}
#endif
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	vsdNO = vsdNO*MAX_SPEED_LEVEL_EACH_VSD + LCDMenuConfig_EX.Calc_X_Speed;
#endif
	OSSemPend(CalibrationParamSem, 0, &err);
	
	pXCali = &epsonCaliSetting.eprCaliSetting.xOffset[HeightLevel][resNO][vsdNO]; //360DPI.
	if(epsonCaliSetting.printerSetting.bOriginIsRight == bLeft)
	{
		INT8U lineindex_base,headindex_base,lineindex_allbase,headindex_allbase;
		
		headindex_allbase = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
		lineindex_allbase = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
		
		headindex_base = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNO]];
		lineindex_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNO]];
		
		if(headNO == headindex_allbase && lineNO == lineindex_allbase)
		{
			//pXCali->xPosDirOffset.XOffset[headNO][lineNO] = Data;
		}
		else if(headNO == headindex_base && lineNO == lineindex_base)
		{
			old = pXCali->xPosDirOffset.XOffset[headNO][lineNO];
			for(i=0; i<epsonCaliSetting.printerSetting.HeadCount; i++)
			{
				for(j=0; j<NOZZLE_LINE_COUNT_PER_HEAD; j++)
				{
					if(epsonCaliSetting.colorMap.ColorIndex[headNO][lineNO] == epsonCaliSetting.colorMap.ColorIndex[i][j])
					{
						pXCali->xPosDirOffset.XOffset[i][j] += (Data - old);
						pXCali->xNegDirOffset.XOffset[i][j] += (Data - old);
					}
				}
			}
		}
		else
		{
			pXCali->xPosDirOffset.XOffset[headNO][lineNO] = pXCali->xPosDirOffset.XOffset[headindex_base][lineindex_base] + Data;
		}
	}
	else
	{
		INT8U lineindex_base,headindex_base;
		
		headindex_base = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNO]];
		lineindex_base = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.colorMap.ColorIndex[headNO][lineNO]];
		
		pXCali->xNegDirOffset.XOffset[headNO][lineNO] = pXCali->xPosDirOffset.XOffset[headindex_base][lineindex_base] + Data;
	}
	
	bCaliSettingDirty = True;
	
	OSSemPost(CalibrationParamSem);
	
	return True;
}

INT8U Cali_EM_Set_Hor_Left(void * pData, int Index, int Data)
{
	return Cali_EM_Set_Hor_LeftOrRight(pData, Index, Data, True);
}
INT8U Cali_EM_Set_Hor_Right(void * pData, int Index, int Data)
{
	return Cali_EM_Set_Hor_LeftOrRight(pData, Index, Data, False);
}

//Index: enum RES_VSD_INDEX.
INT32S Cali_EM_Get_Hor_BiDir(void * pData, int Index)
{
	INT32S BiDir;
	INT8U headNO, lineNO, resNO, vsdNO;
	struct EPSON_X_Cali *pXCali;
	INT8U err, HeightLevel;
	
	HeightLevel = Index/100;
	Index = Index%100;
	
	headNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
	lineNO = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
	
	resNO = Index/10;
	vsdNO = Index%10;
	switch(resNO)
	{
	case 0:
	default:
		resNO = 1;
#ifdef DYSS_1270
		if(vsdNO == 0)
			resNO = 2;
		else
			resNO = 1;
#endif		
		break;
	case 1:
		resNO = 0;
#ifdef DYSS_1270
		if(vsdNO == 0)
			resNO = 1;
		else
			resNO = 0;
#endif	
		break;
#if defined( HEAD_EPSON_GEN5)
	case 2:
		resNO = 3;
		break;
	case 3:
		resNO = 2;
		break;
	case 4:
		resNO = 4;
		break;
#else
	case 2:
		resNO = 2;
		break;
#endif
	}
	
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && vsdNO == 1)
	{
		vsdNO = factoryDataEx.Vsd2ToVsd3;
	}
#endif
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	vsdNO = vsdNO*MAX_SPEED_LEVEL_EACH_VSD + LCDMenuConfig_EX.Calc_X_Speed;
#endif
	OSSemPend(CalibrationParamSem, 0, &err);
	pXCali = &epsonCaliSetting.eprCaliSetting.xOffset[HeightLevel][resNO][vsdNO];
	BiDir = pXCali->xNegDirOffset.XOffset[headNO][lineNO];
	OSSemPost(CalibrationParamSem);
	
	return BiDir;
}

INT8U Cali_EM_Set_Hor_BiDir(void * pData, int Index, int Data)
{
	INT32S BiDir;
	struct EPSON_X_Cali *pXCali;
	INT8U headNO, lineNO, resNO, vsdNO;
	INT8U err, HeightLevel;
	INT16U dpi;
	
	if(pData == (void *)-1)
		return True;
	
	HeightLevel = Index/100;
	Index = Index%100;
	
	headNO = epsonCaliSetting.colorMap.BaseHeadNo[epsonCaliSetting.printerSetting.BaseColor_Index];
	lineNO = epsonCaliSetting.colorMap.BaseLineNo[epsonCaliSetting.printerSetting.BaseColor_Index];
	
	resNO = Index/10;
	vsdNO = Index%10;
	switch(resNO)
	{
	case 0:
	default:
		resNO = 1;
#ifdef DYSS_1270
		if(vsdNO == 0)
			resNO = 2;
		else
			resNO = 1;
#endif		
		break;
	case 1:
		resNO = 0;
#ifdef DYSS_1270
		if(vsdNO == 0)
			resNO = 1;
		else
			resNO = 0;
#endif	
		break;
#if defined( HEAD_EPSON_GEN5)
	case 2:
		resNO = 3;
		break;
	case 3:
		resNO = 2;
		break;
	case 4:
		resNO = 4;
		break;
#else
	case 2:
		resNO = 2;
		break;
#endif
	}
	dpi = GetRes(resNO);
	
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && vsdNO == 1)
	{
		vsdNO = factoryDataEx.Vsd2ToVsd3;
	}
#endif
	
#if defined(RIPSTAR_FLAT_EX)&&(defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN_6SPEED))
	vsdNO = vsdNO*MAX_SPEED_LEVEL_EACH_VSD + LCDMenuConfig_EX.Calc_X_Speed;
#endif
	
	OSSemPend(CalibrationParamSem, 0, &err);
	
	pXCali = &epsonCaliSetting.eprCaliSetting.xOffset[HeightLevel][resNO][vsdNO]; //360DPI.
	BiDir = pXCali->xNegDirOffset.XOffset[headNO][lineNO];
	
	pXCali->nDPI = dpi;
	if(Data != BiDir)
	{
		for(headNO = 0; headNO < epsonCaliSetting.printerSetting.HeadCount; headNO++)
		{
			for(lineNO = 0; lineNO < NOZZLE_LINE_COUNT_PER_HEAD; lineNO ++)
			{
				pXCali->xNegDirOffset.XOffset[headNO][lineNO] += (Data-BiDir);
			}
		}
		bCaliSettingDirty = True;
	}
	
	OSSemPost(CalibrationParamSem);
	
	return True;
}

INT8U Cali_EM_Print_Vertical(void * pData, int Index)
{
	if(!epsonCaliSetting.printerSetting.bVerticalValid)
		return False;
	
	struct CaliPrintSetting Info;
	Info.type = VerticalCheck;
	Info.VSDModel = CALI_VSD_DEFAULT;
	Info.DotSetting = 3;
	Info.DPIModel = 1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

//Index: 0~3:color index.
INT32S Cali_EM_Get_Vertical(void * pData, int Index)
{
	INT32S Vertical;
	INT8U headNO, lineNO;
	INT8U err;
	
	headNO = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[Index], 0);
	lineNO = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[Index], 0);
	
	OSSemPend(CalibrationParamSem, 0, &err);
	Vertical = epsonCaliSetting.eprCaliSetting.yOffset.YOffset[headNO][lineNO]/
		epsonCaliSetting.printerSetting.YInterleaveNum;
	OSSemPost(CalibrationParamSem);
	
	return Vertical;
}

INT8U Cali_EM_Set_Vertical(void * pData, int Index, int Data)
{
	INT8U headNO, lineNO;
	INT8U err;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	headNO = GetHeadIndexWithInterleave(0, factoryDataEx.m_nColorOrder[Index], 0);
	lineNO = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[Index], 0);
	
	OSSemPend(CalibrationParamSem, 0, &err);
	epsonCaliSetting.eprCaliSetting.yOffset.YOffset[headNO][lineNO] =
		Data * epsonCaliSetting.printerSetting.YInterleaveNum;
	bCaliSettingDirty = True;
	OSSemPost(CalibrationParamSem);
	
	return True;
}

INT8U Cali_EM_Print_VerticalAlign(void * pData, int Index)
{
	//here, ought to check 合法性。
	// it must , a. head_group > 1. b. verticalNozzleShift == 0.
	
	struct CaliPrintSetting Info;
	Info.type = VerticalAlignCheck;
	Info.VSDModel = CALI_VSD_DEFAULT;
	Info.DotSetting = 3;
	Info.DPIModel = 1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * GetRes(Info.DPIModel - 1));
	Info.MediaType = LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}


#ifdef HEAD_EPSON_GEN5
//Index: enum RES_VSD_INDEX.
INT8U Cali_EM_Print_Angle_LeftOrRight(void * pData, int Index, INT8U bLeft)
{
	struct CaliPrintSetting Info;
	Info.type = bLeft?AngleLeftCheck:AngleRightCheck;
	Info.VSDModel = Index % 10 + 1;
	Info.DotSetting = 2;
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && Info.VSDModel == 2)
	{
		Info.VSDModel = factoryDataEx.Vsd2ToVsd3 + 1;
		Info.DotSetting = factoryDataEx.Vsd2ToVsd3_ColorDeep;
	}
#endif 		
#ifdef DYSS_1270
#error //need check
#endif
	switch(Index / 10)
	{
	case 0:
	default:
		
#ifdef RICOH_VSD2
		if(Info.VSDModel == 2 ||Info.VSDModel == 3)
			Info.DPIModel = 1;
		else
#endif
			Info.DPIModel = 2;
		
		Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * 360);
		break;
	case 1:
		Info.DPIModel = 1;
		Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * 720);
		break;
	case 2:
		Info.DPIModel = 4;
		Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * 270);
		break;
	case 3:
		Info.DPIModel = 3;
		Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * 540);
		break;
	case 4:
		Info.DPIModel = 5;
		Info.StartPos = (INT32S)(LCDMenuConfig.OriginX * 1440);
		break;
	}
	Info.MediaType =  LCDMenuConfig.SheetType;
	Info.PassNum = STEPCHECK_BASEPASSNUM;
	Info.Option = 9;
	return StartCalibrationJob(&Info);
}

INT8U Cali_EM_Print_Angle_Left(void * pData, int Index)
{
	return Cali_EM_Print_Angle_LeftOrRight(pData, Index, True);
}
INT8U Cali_EM_Print_Angle_Right(void * pData, int Index)
{
	return Cali_EM_Print_Angle_LeftOrRight(pData, Index, False);
}

//Index: enum RES_VSD_INDEX.
INT32S Cali_EM_Get_Angle(void * pData, int Index, INT8U bLeft, INT8U HeadNO)
{
	INT32S AngleOffset;
	struct EPSON_Angle_X_Cali *pXCali;
	INT8U resNO, vsdNO;
	INT8U err;
	
	resNO = Index/10;
	switch(resNO)
	{
	case 0:
	default:
		resNO = 1;
		break;
	case 1:
		resNO = 0;
		break;
	case 2:
		resNO = 3;
		break;
	case 3:
		resNO = 2;
		break;
	case 4:
		resNO = 4;
		break;
	}
	vsdNO = Index%10;
#ifdef VSD2TOVSD3
	if(factoryDataEx.Vsd2ToVsd3_ColorDeep != 0 && vsdNO == 1)
	{
		vsdNO = factoryDataEx.Vsd2ToVsd3;
	}
#endif
	OSSemPend(CalibrationParamSem, 0, &err);
	
#if defined( HEAD_EPSON_GEN5)&&!defined( EPSON_4H)
	pXCali = &epsonCaliSetting.eprCaliSetting.xAngleOffset[LCDMenuConfig.Head_Height_Level][resNO][vsdNO];
#endif
	
	if(epsonCaliSetting.printerSetting.bOriginIsRight == bLeft)
		AngleOffset = pXCali->xPosDirOffset[HeadNO];
	else
		AngleOffset = pXCali->xNegDirOffset[HeadNO];
	
	OSSemPost(CalibrationParamSem);
	
	return AngleOffset;
}

INT32S Cali_EM_Get_Angle_Left_H1(void * pData, int Index)
{
	return Cali_EM_Get_Angle(pData, Index, True, 0);
}
INT32S Cali_EM_Get_Angle_Left_H2(void * pData, int Index)
{
	return Cali_EM_Get_Angle(pData, Index, True, 1);
}
INT32S Cali_EM_Get_Angle_Right_H1(void * pData, int Index)
{
	return Cali_EM_Get_Angle(pData, Index, False, 0);
}
INT32S Cali_EM_Get_Angle_Right_H2(void * pData, int Index)
{
	return Cali_EM_Get_Angle(pData, Index, False, 1);
}
INT8U Cali_EM_Set_Angle(void * pData, int Index, int Data, INT8U bLeft, INT8U HeadNO)
{
	struct EPSON_Angle_X_Cali *pXCali;
	INT8U resNO, vsdNO;
	INT8U err;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	resNO = Index/10;
	switch(resNO)
	{
	case 0:
	default:
		resNO = 1;
		break;
	case 1:
		resNO = 0;
		break;
	case 2:
		resNO = 3;
		break;
	case 3:
		resNO = 2;
		break;
	case 4:
		resNO = 4;
		break;
	}
	vsdNO = Index%10;
	
	OSSemPend(CalibrationParamSem, 0, &err);
#if defined( HEAD_EPSON_GEN5)&&!defined( EPSON_4H)
	pXCali = &epsonCaliSetting.eprCaliSetting.xAngleOffset[LCDMenuConfig.Head_Height_Level][resNO][vsdNO];
#endif
	if(epsonCaliSetting.printerSetting.bOriginIsRight == bLeft)
		pXCali->xPosDirOffset[HeadNO] = Data;
	else
		pXCali->xNegDirOffset[HeadNO] = Data;
	
	bCaliSettingDirty = True;
	OSSemPost(CalibrationParamSem);
	
	return True;
}
INT8U Cali_EM_Set_Angle_Left_H1(void * pData, int Index, int Data)
{
	return Cali_EM_Set_Angle(pData, Index, Data, True, 0);
}
INT8U Cali_EM_Set_Angle_Left_H2(void * pData, int Index, int Data)
{
	return Cali_EM_Set_Angle(pData, Index, Data, True, 1);
}
INT8U Cali_EM_Set_Angle_Right_H1(void * pData, int Index, int Data)
{
	return Cali_EM_Set_Angle(pData, Index, Data, False, 0);
}
INT8U Cali_EM_Set_Angle_Right_H2(void * pData, int Index, int Data)
{
	return Cali_EM_Set_Angle(pData, Index, Data, False, 1);
}
#endif

INT8U Cali_EM_Print_TestingPrint(void * pData, int Index)
{
	struct CaliPrintSetting Info;
	Info.type = TestPrint;
	Info.VSDModel = LCDMenuConfig.TestPrint_VSDIndex+1;
	Info.DotSetting = 3;
	Info.DPIModel = LCDMenuConfig.TestPrint_DPIIndex+1;
#ifdef DYSS_1270
	Info.DPIModel = CHANGE_DPI_TO ;
#endif		
	Info.StartPos = (INT32S)GetRes(Info.DPIModel - 1);
	Info.MediaType =  LCDMenuConfig.SheetType;
	Info.PassNum = GetLCDMenuPassNum(LCDMenuConfig.TestPrint_PassIndex);
	Info.Option = 1;
	return StartCalibrationJob(&Info);
}

//Index: 0~4:720/360/540/270/1440
INT8U Cali_EM_Get_TestingPrint_DPI(void * pData, int Index)
{
	return LCDMenuConfig.TestPrint_DPIIndex;
}
INT8U Cali_EM_Set_TestingPrint_DPI(void * pData, int Index, int Data)
{
	if(pData == (void *)-1)
		return True;
	
	LCDMenuConfig.TestPrint_DPIIndex = Data;
	SaveLCDMenuConfig();
	return True;
}
//Index: 0~3:VSD1~VSD4
INT8U Cali_EM_Get_TestingPrint_VSD(void * pData, int Index)
{
	return LCDMenuConfig.TestPrint_VSDIndex;
}
INT8U Cali_EM_Set_TestingPrint_VSD(void * pData, int Index, int Data)
{
	if(pData == (void *)-1)
		return True;
	
	LCDMenuConfig.TestPrint_VSDIndex = Data;
	SaveLCDMenuConfig();
	return True;
}
//Index: 0:4pass, 1:6Pass, 2:8Pass, 3:12Pass.
INT8U Cali_EM_Get_TestingPrint_PassNum(void * pData, int Index)
{
	return LCDMenuConfig.TestPrint_PassIndex;
}
INT8U Cali_EM_Set_TestingPrint_PassNum(void * pData, int Index, int Data)
{
	if(pData == (void *)-1)
		return True;
	
	LCDMenuConfig.TestPrint_PassIndex = Data;
	SaveLCDMenuConfig();
	return True;
}

#if defined(EPSON_4H)	
INT8U Menu_SetHead4Active(void * pData, int Index, int Data)
{
	U8 err;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	Index = Data;
	if(Index >8 || Index < 0)
		return False;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	switch(Index)
	{
	default:
	case 0:
		LCDMenuConfig.Active_HeadMask = 0xF;
		break;
	case 1:
		LCDMenuConfig.Active_HeadMask = 0x7;
		break;
	case 2:
		LCDMenuConfig.Active_HeadMask = 0xC;
		break;
	case 3:
		LCDMenuConfig.Active_HeadMask = 0x3;
		break;
	case 4:
		LCDMenuConfig.Active_HeadMask = 0x1;
		break;
	case 5:
		LCDMenuConfig.Active_HeadMask = 0x2;
		break;				
	case 6:
		LCDMenuConfig.Active_HeadMask = 0x4;
		break;	
	case 7:
		LCDMenuConfig.Active_HeadMask = 0x8;
		break;	
	}
	OSSemPost(LCDMenuConfigureSem);
	
	Cal_active_Group_num();
	
	return SaveLCDMenuConfig();
}

INT8U Menu_GetHead4Active(void * pData, int Index)
{
	U8 err;
	
	INT8U ret;
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	switch(LCDMenuConfig.Active_HeadMask)
	{
	default:
	case 0xF:
		ret = 0;
		break;
	case 0x7:
		ret = 1;
		break;
	case 0xC:
		ret = 2;
		break;
	case 0x3:
		ret = 3;
		break;
	case 0x1:
		ret = 4;
		break;
	case 0x2:
		ret = 5;
		break;				
	case 0x4:
		ret = 6;
		break;	
	case 0x8:
		ret = 7;
		break;	
	}	
	OSSemPost(LCDMenuConfigureSem);
	return ret;
}
#endif
// only support max group number is 4
INT8U Menu_SetHeadsActiveCfg(void * pData, int Index, int OnOff)
{
	INT8U err, cur_shift;
	int actived_headnum;
	
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	if( Index < 0 || Index >= EPSON_PERMIT_GROUPNUM || OnOff < 0 || OnOff > 1)
		return 0;
	
	OnOff = !OnOff;
	
	if( OnOff)
	{
		OSSemPend(LCDMenuConfigureSem, 0, &err);
		LCDMenuConfig.Active_HeadMask |= (1<<Index);
		actived_headnum = LCDMenuConfig.Active_HeadMask;
		OSSemPost(LCDMenuConfigureSem);
		
		//must consider 1H2C.
		if(factoryData.group_num>=0)
			factoryData.group_num ++;
		else 
			factoryData.group_num --;
	}
	else
	{
		actived_headnum = LCDMenuConfig.Active_HeadMask;
		actived_headnum &= ~(1<<Index);
		if( (actived_headnum & ((1<<EPSON_PERMIT_GROUPNUM)-1)) == 0)
			return 0;
		//cheney, 20120725: 这儿应该需要更多的合法性检查。
		
		OSSemPend(LCDMenuConfigureSem, 0, &err);
		LCDMenuConfig.Active_HeadMask = actived_headnum;
		OSSemPost(LCDMenuConfigureSem);
		
		//must consider 1H2C.
		if(factoryData.group_num>=0)
			factoryData.group_num --;
		else 
			factoryData.group_num ++;
	}
	
	if(epsonCaliSetting.printerSetting.bIntegratedHead && epsonCaliSetting.printerSetting.HeadCount == 2)
	{ //keep on the old implement.
		switch(actived_headnum)
		{
		case 1:
			actived_headnum = 0;
			break;
		case 2:
			actived_headnum = 1;
			break;
		default:
			actived_headnum = 2;
			break;
		}
		
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)	//?????
		SetCleanParam_EPSON_MICOLOR_HeadMask(0, 0, actived_headnum);
#else
		SetCleanParam_EPSON_ALLWIN_HeadMask(0, 0, actived_headnum);
#endif
	}
	
	return SaveLCDMenuConfig();
}

INT8U Menu_GetHeadsActiveCfg(void * pData, int Index)
{
	INT8U err;
	int actived_headnum;
	if( Index < 0 || Index >= EPSON_PERMIT_GROUPNUM)
		return 1;
	
	OSSemPend(LCDMenuConfigureSem, 0, &err);
	actived_headnum = LCDMenuConfig.Active_HeadMask & ((1<<EPSON_PERMIT_GROUPNUM)-1);
	OSSemPost(LCDMenuConfigureSem);
	if(actived_headnum & (1<<Index) )
		return 0;
	else
		return 1;
}

INT8U Menu_SetHead1ActiveCfg(void * pData, int Index)
{
	return Menu_SetHeadsActiveCfg(pData, 0, Index);
}

INT8U Menu_GetHead1ActiveCfg(void * pData)
{
	return Menu_GetHeadsActiveCfg(pData, 0);
}

INT8U Menu_SetHead2ActiveCfg(void * pData, int Index)
{
	return Menu_SetHeadsActiveCfg(pData, 1, Index);
}

INT8U Menu_GetHead2ActiveCfg(void * pData)
{
	return Menu_GetHeadsActiveCfg(pData, 1);
}

INT8U Menu_SetHead3ActiveCfg(void * pData, int Index)
{
	return Menu_SetHeadsActiveCfg(pData, 2, Index);
}

INT8U Menu_GetHead3ActiveCfg(void * pData)
{
	return Menu_GetHeadsActiveCfg(pData, 2);
}

INT8U Menu_SetHead4ActiveCfg(void * pData, int Index)
{
	return Menu_SetHeadsActiveCfg(pData, 3, Index);
}

INT8U Menu_GetHead4ActiveCfg(void * pData)
{
	return Menu_GetHeadsActiveCfg(pData, 3);
}

INT8U ResetCalibration()
{
	INT8U i,k,m,n;
	
	//only for safe and temp testing.
	memset(&epsonCaliSetting.eprCaliSetting, 0, sizeof(struct EPR_EPSON_CaliSetting));
	epsonCaliSetting.eprCaliSetting.len = sizeof(struct EPR_EPSON_CaliSetting);
	epsonCaliSetting.eprCaliSetting.version = EPR_EPSON_CALI_SETTING_VERSION;
	
	for(k=0; k<MAX_HEIGHT_LEVEL; k++)
	{
		for(i=0; i<MAX_X_RES_NUM; i++)
		{
			INT8U j;
			for(j=0; j<MAX_VSDMODEL_NUM; j++)
			{
				switch (i)
				{
				case 0:
				default:
					//epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = epsonCaliSetting.printerSetting.DPI_X;
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI;
					for(m= 0; m<MAX_HEAD_NUMBER_CALIBRATION; m++)
					{
						for(n=0; n<NOZZLE_LINE_COUNT_PER_HEAD; n++)
						{
							switch(j)
							{
							case 0:
							default:
								epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -8;
								break;
							case 1:
								epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -5;
								break;
							case 2:
								epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -9;
								break;
							case 3:
								epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -9;
								break;
							}
						}
					}
#if defined( HEAD_EPSON_GEN5)&&!defined( EPSON_4H)
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI;
#endif
					break;
				case 1:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI/2;
					
					for(m= 0; m<MAX_HEAD_NUMBER_CALIBRATION; m++)
					{
						for(n=0; n<NOZZLE_LINE_COUNT_PER_HEAD; n++)
						{
							switch(j)
							{
							case 0:
							default:
								if(k==0)
									epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -12;
								else
									epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -7;
								break;
							case 1:
								if(k==0)
									epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -8;
								else
									epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -4;
								break;
							case 2:
								if(k==0)
									epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -14;
								else
									epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -8;
								break;
							case 3:
								if(k==0)
									epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -13;
								else
									epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].xNegDirOffset.XOffset[m][n] = -8;
								break;
							}
						}
					}
#if defined( HEAD_EPSON_GEN5)&&!defined( EPSON_4H)
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI/2;
#endif
					break;
#ifdef HEAD_EPSON_GEN5
				case 2:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI/4*3;
#if !defined(EPSON_4H)										                    
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI/4*3;
#endif                    
					break;
				case 3:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI/4*3/2;
#if !defined(EPSON_4H)										                    
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI/4*3/2;
#endif                    
					break;
				case 4:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI*2;
#if !defined(EPSON_4H)										                    
					epsonCaliSetting.eprCaliSetting.xAngleOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI*2;
#endif                    
					break;
#else
				case 2:
					epsonCaliSetting.eprCaliSetting.xOffset[k][i][j].nDPI = factoryDataEx.m_nXEncoderDPI/3;
#endif
				}
			}
		}
	}
	
	for(i=0; i< MAX_MEDIA_TYPE_CALIBRATION; i++)
	{
#if defined(R600)
		epsonCaliSetting.eprCaliSetting.stepPass[i].BaseStep = 9660 * EPSON_ACTIVE_GROUPNUM;
#elif defined(A1802)			
		epsonCaliSetting.eprCaliSetting.stepPass[i].BaseStep = 10914 * EPSON_ACTIVE_GROUPNUM;
#elif defined(TATE_BEMAJET)	
#ifdef BEMAJET_2H
		epsonCaliSetting.eprCaliSetting.stepPass[i].BaseStep = 9229 * EPSON_ACTIVE_GROUPNUM;
#else
		epsonCaliSetting.eprCaliSetting.stepPass[i].BaseStep = 14500 * EPSON_ACTIVE_GROUPNUM;
#endif
#elif defined(ALLWIN_EPSON_SAME)
		epsonCaliSetting.eprCaliSetting.stepPass[i].BaseStep = 12451 * EPSON_ACTIVE_GROUPNUM;
#elif (defined(MANUFACTURER_TATE_EPSON)||defined(MANUFACTURER_TATE_EPSON_UV))&&!defined(SUPPORT_MOTOR_CONTROL)		
		epsonCaliSetting.eprCaliSetting.stepPass[i].BaseStep = 11093 * EPSON_ACTIVE_GROUPNUM;
#else
		epsonCaliSetting.eprCaliSetting.stepPass[i].BaseStep = 7859 * EPSON_ACTIVE_GROUPNUM;
#endif
	}
	
	if(factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L)
		epsonCaliSetting.eprCaliSetting.yOffset.nDPI = NOZZLE_DPI_G4L * INTERLEAVE_CALIBRATION;
	else
		epsonCaliSetting.eprCaliSetting.yOffset.nDPI = NOZZLE_DPI * INTERLEAVE_CALIBRATION;
	
	epsonCaliSetting.eprCaliSetting.headOffset.nDPI = factoryDataEx.m_nXEncoderDPI;
	
	return SaveEPSONCaliSetting();
}

INT8U ResetMenuConfig()
{
	memset(&LCDMenuConfig, 0, sizeof(struct LCD_Menu_Configure));
	
	LCDMenuConfig.len = sizeof(struct LCD_Menu_Configure);
	LCDMenuConfig.version = LCD_MENU_CONFIG_VERSION;
	
	LCDMenuConfig.OriginX = 0.0;
	LCDMenuConfig.MediaWidth = PAPER_MEDIA_WIDTH_INCH;
//#ifdef MANUFACTURER_DYSS
#if 0
	if(LCDMenuConfig.MediaWidth >=DYSS_MAX_X_PRINT_WIDTH)
		LCDMenuConfig.MediaWidth = DYSS_MAX_X_PRINT_WIDTH;	
#endif
	LCDMenuConfig.Margin = 0.0;
	
	LCDMenuConfig.SheetType = 0;
	LCDMenuConfig.SheetIsTransparent = False;
	
	LCDMenuConfig.AutoCut = False;
	LCDMenuConfig.AutoMeasure = False;
	LCDMenuConfig.AutoLoad = False;
	LCDMenuConfig.AutoLackMediaCheck = False;
	LCDMenuConfig.AutoCoverCheck = False;
	LCDMenuConfig.AutoPlateFan = False;
	LCDMenuConfig.AutoFixCheck = False;
#ifdef MANUFACTURER_GZ
	LCDMenuConfig.PrintQuality = 1;
#else
	LCDMenuConfig.PrintQuality = 0;
#endif   
	LCDMenuConfig.LanguageIndex = password.m_bLanguage;
	
	LCDMenuConfig.Step_PassIndex = 2;
	
	LCDMenuConfig.TestPrint_DPIIndex = 1;
	LCDMenuConfig.TestPrint_VSDIndex = 2;
	LCDMenuConfig.TestPrint_PassIndex = 2;
	
	LCDMenuConfig.Head_Height_Level = 0;
	
	LCDMenuConfig.Active_HeadMask = (1<<EPSON_PERMIT_GROUPNUM) -1;
	
	LCDMenuConfig.AutoPowerOnClean = False;
	LCDMenuConfig.AutoGoHome = False;
	
	LCDMenuConfig.PlateFanMode = PLATEFANMODE_STRONG;
	
	LCDMenuConfig.SpotColorName[0] = 0;
	LCDMenuConfig.SpotColorName[1] = 1;
	LCDMenuConfig.SpotColorName[2] = 2;
	LCDMenuConfig.SpotColorName[3] = 3;
	LCDMenuConfig.AutoStep = False;
	
	//must consider 1H2C.
#if defined( RICOH_3H	) || defined (RICOH_G5_3H)
	factoryData.group_num = - EPSON_PERMIT_GROUPNUM;
#elif defined(RICOH_G5_4H)
	if(COLOR_NUMBER_CALIBRATION > 4)
		factoryData.group_num = - EPSON_PERMIT_GROUPNUM;
	else
		factoryData.group_num = EPSON_PERMIT_GROUPNUM;
#else
	factoryData.group_num = EPSON_PERMIT_GROUPNUM;
#endif
	LCDMenuConfig.cfg_bit = False;
	
	return SaveLCDMenuConfig();
}

INT8U UI_Get_HeadHeightLevel(void * pData)
{
	return LCDMenuConfig.Head_Height_Level;
}

INT8U UI_Reset_Calibration(void * pData)
{
	INT8U err;
	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}
	
	INT8U ret = ResetMenuConfig();

	ret &= ResetMenuConfig_EX();

	ret &= ResetCalibration();
	//temp way:
#if defined(EPSON_BOTTOM_BOARD_V3)||defined(EPSON_BOTTOM_BOARD_V2_1)
	ret &=  ResetHeaterInfo();
#endif
	return ret;
}

INT8U ReadEPSONCaliSetting()
{
	INT8U err,j,k;
	INT16U length;
	EPM_Head head;
	
	length = sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head);
	if(length > ERP_CALIBRATION_SETTING_SIZE)
		return False;
	
	if(!IIC_ReadEEPRom_Head(EPR_CALIBRATION_SETTING_OFFSET, &head))
	{
		return False;
	}
	
	memset(&epsonCaliSetting.eprCaliSetting, 0, sizeof(struct EPR_EPSON_CaliSetting));
	if( (head.version == 0x103 || head.version == 0x102)
	   && (head.flag != EPR_EPSON_CALI_SETTING_FLAG && head.flag != EPR_EPSON_CALI_SETTING_FLAG_1) &&
		   head.flag < ERP_CALIBRATION_SETTING_SIZE) //it is old structure before add the head.
	{
		length = head.flag; //really, it is element len of old structure.
		if(length > sizeof(struct EPR_EPSON_CaliSetting))
			length = sizeof(struct EPR_EPSON_CaliSetting);
		if(!IIC_ReadEEPRom_Ex(EPR_CALIBRATION_SETTING_OFFSET, (INT8U * )&epsonCaliSetting.eprCaliSetting, &length))
		{
			return False;
		}
		//add new elememt initialization here.
		if( epsonCaliSetting.eprCaliSetting.version == 0x102)
		{
			INT8U i;
			epsonCaliSetting.eprCaliSetting.headOffset.nDPI = 720;
			for(j=0;j<MAX_VSDMODEL_NUM;j++)
				for(i=0; i<MAX_HEAD_NUMBER_CALIBRATION; i++)
					epsonCaliSetting.eprCaliSetting.headOffset.XOffset[j][i] = 0;
		}
		
		epsonCaliSetting.eprCaliSetting.len = sizeof(struct EPR_EPSON_CaliSetting);
		epsonCaliSetting.eprCaliSetting.version = EPR_EPSON_CALI_SETTING_VERSION;
		
		return SaveEPSONCaliSetting();
	}
	else if(head.flag == EPR_EPSON_CALI_SETTING_FLAG)
	{
		if(!IIC_ReadEEPRom_Struct(EPR_CALIBRATION_SETTING_OFFSET + sizeof(EPM_Head),
								  (INT8U * )&epsonCaliSetting.eprCaliSetting, sizeof(struct EPR_EPSON_CaliSetting), &head))
		{
			return False;
		}
		
		if( epsonCaliSetting.eprCaliSetting.len != sizeof(struct EPR_EPSON_CaliSetting) ||
		   epsonCaliSetting.eprCaliSetting.version != EPR_EPSON_CALI_SETTING_VERSION)
		{
			epsonCaliSetting.eprCaliSetting.len = sizeof(struct EPR_EPSON_CaliSetting);
			epsonCaliSetting.eprCaliSetting.version = EPR_EPSON_CALI_SETTING_VERSION;
			for(j=1;j<MAX_VSDMODEL_NUM;j++)
				for(k=0;k<MAX_HEAD_NUMBER_CALIBRATION;k++)
					epsonCaliSetting.eprCaliSetting.headOffset.XOffset[j][k]=epsonCaliSetting.eprCaliSetting.headOffset.XOffset[0][k];
			
			for(j=1;j<MAX_VSDMODEL_NUM;j++)
				for(k=0;k<MAX_HEAD_NUMBER_CALIBRATION;k++)
					epsonCaliSetting.eprCaliSetting.headOffset.XOffset[j][k]=epsonCaliSetting.eprCaliSetting.headOffset.XOffset[0][k];
			return SaveEPSONCaliSetting();
		}
	}
	else
	{
		return ResetCalibration();
	}
	
	return True;
}
INT8U SaveEPSONCaliConfig()
{
	INT8U ret = True;
	INT16U len = sizeof(CaliConfigType);
	
	EPM_Head head =
	{
		CALI_CONFIG_FLAG,
		CALI_CONFIG_VERSION,
		sizeof(CaliConfigType) + sizeof(EPM_Head),
		0
	};
	
	CalcCheckSum((INT8U*)&caliConfig, &head);
	ret = IIC_WriteEEPRom_Head(EPR_CALIBRATION_CONFIG_OFFSET, &head);
	
	ret &= IIC_WriteEEPRom_Ex(EPR_CALIBRATION_CONFIG_OFFSET + sizeof(EPM_Head), (INT8U *)&caliConfig, &len);
	return ret;
}

INT8U ReadEPSONCaliConfig()
{
	INT8U err;
	INT16U length;
	EPM_Head head;
	
	length = sizeof(CaliConfigType) + sizeof(EPM_Head);
	if(length > ERP_CALIBRATION_CONFIG_SIZE)
		return False;
	
	if(!IIC_ReadEEPRom_Head(EPR_CALIBRATION_CONFIG_OFFSET, &head))
	{
		return False;
	}
	
	memset(&caliConfig, 0, sizeof(CaliConfigType));
	if((head.flag & 0xFF) == 0x40 && (head.flag >> 8) == 1 ) //it is old structure before add the head.
	{
		length = (head.flag & 0xFF); //really, it is element len of old structure.
		if(length > sizeof(CaliConfigType))
			length = sizeof(CaliConfigType);
		if(!IIC_ReadEEPRom_Ex(EPR_CALIBRATION_CONFIG_OFFSET, (INT8U * )&caliConfig, &length))
		{
			return False;
		}
		//add new elememt initialization here.
		
		caliConfig.len = sizeof(CaliConfigType);  //current len is sizeof(CaliConfigType)
		caliConfig.version = CALI_CONFIG_VERSION;  //current version is 0x01
		
		return SaveEPSONCaliConfig();
	}
	else if(head.flag == CALI_CONFIG_FLAG)
	{
		if(!IIC_ReadEEPRom_Struct(EPR_CALIBRATION_CONFIG_OFFSET + sizeof(EPM_Head),
								  (INT8U * )&caliConfig, sizeof(CaliConfigType), &head))
		{
			return False;
		}
		
		if( caliConfig.len != sizeof(CaliConfigType) ||
		   caliConfig.version != CALI_CONFIG_VERSION)
		{
			caliConfig.len = sizeof(CaliConfigType);
			caliConfig.version = CALI_CONFIG_VERSION;
			
			return SaveEPSONCaliConfig();
		}
	}
	else
	{
		//only for safe and temp testing.
		memset(&caliConfig, 0, sizeof(struct CaliConfig));
		caliConfig.len = sizeof(struct CaliConfig);
		caliConfig.version = CALI_CONFIG_VERSION;
		
		caliConfig.BaseColor = COLORCODE_K;
		caliConfig.YStepColor[0] = COLORCODE_K;
		caliConfig.YStepColor[1] = COLORCODE_M;
		
		return SaveEPSONCaliConfig();
	}
	
	return True;
}

INT32U GetStepPerHead()
{
	return epsonCaliSetting.eprCaliSetting.stepPass[LCDMenuConfig.SheetType].BaseStep /
		epsonCaliSetting.printerSetting.GroupCount_YContinue;
}

extern INT8U MediaType;
extern struct Struct_JobStart job_info;
INT8U UpDownPrintingStep(void * pData, unsigned char bUp)
{
	U8 err;
	
	if(OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_SET_ALL,&err), err == OS_NO_ERR)
	{
		OSSemPend(CalibrationParamSem, 0, &err);
		if(factoryDataEx.Only_Used_1head == 1)
		{	
			if((job_info.PassNumber -1)*2 < MAX_PASS_NUMBER_CALIBRATION)
			{
				if(bUp)
					epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[(job_info.PassNumber -1)*2]++;
				else
					epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[(job_info.PassNumber -1)*2]--;
			}
		}
		else
		{
			if(bUp)
				epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[job_info.PassNumber -1]++;
			else
				epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[job_info.PassNumber -1]--;
		}				
		bCaliStepDirty = True;
		OSSemPost(CalibrationParamSem);	
		return True;
	}
	else
		return False;
}

INT16S GetPrintingStep(void * pData)
{
	if(factoryDataEx.Only_Used_1head == 1)
	{	
		if((job_info.PassNumber -1)*2 < MAX_PASS_NUMBER_CALIBRATION)
			return epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[(job_info.PassNumber -1)*2];
		else
			return epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[3];
	}
	else
		return epsonCaliSetting.eprCaliSetting.stepPass[MediaType].PassStep[job_info.PassNumber -1];
}
#ifdef MANUFACTURER_DYSS
INT16U write_read_ptr = 0;
INT8U GetCalibrationLength(INT8U *EP0INdata)
{
	EPM_Head head;
	ALLLEN= sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head);
	
	if(ALLLEN > ERP_CALIBRATION_SETTING_SIZE)
		return False;
	ALLLEN += EX_MAX_BYTE;
	*(INT16U*)EP0INdata=ALLLEN;
	return sizeof(INT16U);
}
INT8U InCaliData(INT8U *EP0INdata,INT16U index,INT16U setuplength)
{
	INT16U len;
	if(index == 0)
		write_read_ptr = 0;
	
	if (index >=ALLLEN)
		len = 0;
	else if(index <= sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head))
	{
		if(index + setuplength > sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head)) 
		{            //calierrtype = 3;//setuplength erro
			len =sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head) - index;
		}
		else
			len = setuplength;
		if (len > 64)
			len = 64;
		if(!IIC_ReadEEPRom_Ex(EPR_CALIBRATION_SETTING_OFFSET+index, EP0INdata,&len))
		{
			return False;
		}
		if(len!=64 &&(index + setuplength > sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head)))
		{
			if (setuplength > 64)
				setuplength = 64;
			memcpy((void *)((INT8U *)EP0INdata + len), (void *)((INT8U *)&LCDMenuConfig_EX.add[0] + write_read_ptr), setuplength -len);
			write_read_ptr +=  (setuplength -len);
			len = setuplength;
		}
	}
	else 
	{
		if(index + setuplength > sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head) + EX_MAX_BYTE) 
		{            //calierrtype = 3;//setuplength erro
			len =sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head)+ EX_MAX_BYTE - index;
		}
		else
			len = setuplength;
		if (len > 64)
			len = 64;
		memcpy((void *)EP0INdata, (void *)((INT8U *)&LCDMenuConfig_EX.add[0] + write_read_ptr), len);
		write_read_ptr +=  len;
		if(write_read_ptr >= EX_MAX_BYTE)
			write_read_ptr = 0;
	}
	
	return (INT8U)len;
}

INT8U OutCaliData(INT8U *EP0OUTdata,INT16U index,INT16U setuplength)
{
	INT16U len;
	if(index == 0)
		write_read_ptr = 0;
	
	if(index <= sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head))
	{
		if(index + setuplength > sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head)) 
		{            //calierrtype = 3;//setuplength erro
			len =sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head) - index;
		}
		else
			len = setuplength;	
		if(!IIC_WriteEEPRom_Ex(EPR_CALIBRATION_SETTING_OFFSET+index, EP0OUTdata, &len))
		{
			return False;
		}
		if(len!=64 &&(index + setuplength > sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head)))
		{
			if (setuplength > 64)
				setuplength = 64;
			memcpy((void *)((INT8U *)&LCDMenuConfig_EX.add[0] + write_read_ptr), (void *)((INT8U *)EP0OUTdata + len), setuplength -len);
			write_read_ptr +=  (setuplength -len);
		}
	}
	else 
	{
		if(index + setuplength > sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head) + EX_MAX_BYTE) 
		{            //calierrtype = 3;//setuplength erro
			len =sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head)+ EX_MAX_BYTE - index;
		}
		else
			len = setuplength;
		if (len > 64)
			len = 64;
		memcpy((void *)((INT8U *)&LCDMenuConfig_EX.add[0] + write_read_ptr), (void *)EP0OUTdata, len);
		write_read_ptr +=  len;
		if(write_read_ptr >= EX_MAX_BYTE)
		{
			write_read_ptr = 0;
			SaveLCDMenuConfig_EX();
		}
	}
	return setuplength;
}
#else
INT8U GetCalibrationLength(INT8U *EP0INdata)
{
	EPM_Head head;
	ALLLEN= sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head);
	
	if(ALLLEN > ERP_CALIBRATION_SETTING_SIZE)
		return False;
	
	*(INT16U*)EP0INdata=ALLLEN;
	return sizeof(INT16U);
}
INT8U InCaliData(INT8U *EP0INdata,INT16U index,INT16U setuplength)
{
	INT16U len;
	
	if (index >=ALLLEN)
		len = 0;
	else if(index + setuplength > sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head))
	{            //calierrtype = 3;//setuplength erro
		len =sizeof(struct EPR_EPSON_CaliSetting) + sizeof(EPM_Head) - index;
	}
	else
		len = setuplength;
	if (len > 64)
		len = 64;
	if(!IIC_ReadEEPRom_Ex(EPR_CALIBRATION_SETTING_OFFSET+index, EP0INdata,&len))
	{
		return False;
	}
	return (INT8U)len;
}

INT8U OutCaliData(INT8U *EP0OUTdata,INT16U index,INT16U setuplength)
{
	
	if(!IIC_WriteEEPRom_Ex(EPR_CALIBRATION_SETTING_OFFSET+index, EP0OUTdata, (INT16U*)&setuplength))
	{
		return False;
	}
	return setuplength;
}
#endif

#endif



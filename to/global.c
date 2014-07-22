// All global parameters will be put here
#define _GLOBAL_C_

#include "global.h"
#include "consol.h"
#include "status.h"
#include "keylcd.h"
#include "miscellance.h"
#include "HugeBuf.h"
#include "control.h"

#include "onewire.h"

#ifdef FUNCTION_CLOSE_LCD
#include "../LCD_GUI/resources.h"
#endif

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
#include "calibration.h"
#include "usb.h"
#endif

//volatile INT32S curPos = 0; //Current X position of the carriadge
MediaEdgeType mediaEdge[MAX_PAPER_VOLUME];
CleanParaType	cleanPara;
INT8S volatile mediaVolNum = 1;
PasswordType password;
status_EP0BUF_TYPE status_EP0Buf;
FWInfoType fwInfo;
PrinterParaType printer; 

OS_FLAG_GRP	*status_FLAG_GRP;
OS_FLAG_GRP *mix_FLAG_GRP;
OS_FLAG_GRP *mix_FLAG_GRP_2;

//EEPromDataType epromPrinterParms;
EPR_FactoryDataType factoryData FAST_DATA_SECTION;
EPR_BYHXDataType byhxData;
HeadBoardInfoType headboardInfo;

#ifdef COORD_NEW_UV
EPR_FactoryUVSetting_Type Ripstar_UV_Setting;
#include "uv.h"
#endif

#if defined(HEAD_RICOH_G4)&&defined(ONE_BIT_DATA)
volatile INT8U RIPSTAR_PRT_BIT = 1;
#elif defined(MANUFACTURER_FULGENCY_FLAT_UV)
const INT8U RIPSTAR_PRT_BIT = 2;
#else
const INT8U RIPSTAR_PRT_BIT = 1;
#endif
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
EPR_FactoryDataExType factoryDataEx;
#if defined( HEAD_EPSON_GEN5)
#ifdef EPSON_4H
INT8U EpsonChannelMap[MAX_EPSON_HEAD_CHANNEL] = {3,2,1,0,7,6,5,4}; 
#else
INT8U EpsonChannelMap[MAX_EPSON_HEAD_CHANNEL] = {3,2,1,0}; 
#endif
#else
INT8U EpsonChannelMap[MAX_EPSON_HEAD_CHANNEL] = {6,4,2,0,7,5,3,1}; //????
#ifdef RICOH_G5_3H
INT8U RICOH_ChannelMap[MAX_EPSON_HEAD_CHANNEL] = {2,1,0};
#elif defined(RICOH_G5_4H)
INT8U RICOH_ChannelMap[MAX_EPSON_HEAD_CHANNEL] = {3,1,2,0};
#else
INT8U RICOH_ChannelMap[MAX_EPSON_HEAD_CHANNEL] = {4,5,2,3,0,1};
#endif
#endif
CaliConfigType caliConfig;
SPMCaliSettingType pmCaliSettingFWFormat, pmCaliSettingPMFormat = {0};
INT8U bPMCaliSettingDirty = False;
INT8U bCaliSettingDirty = False;
INT8U bCaliStepDirty = False;
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
INT8U bResistanceDirty = False;
#endif
#endif

INT8U bCleanSprayDirty = False;
INT8U bMotionCfgDirty = False;
INT8U bDyssMeasureDirty = False;
volatile UsedInkVolumeType usedInkVolume;

volatile INT8S clean_single_indexgl;
volatile INT8S move_dir = 1;
volatile INT32S move_distance = 1;
volatile INT8U FPGAOK = False;
volatile INT8U printerFlashing = False; 
volatile INT8U cleanParaDirty = True;
volatile INT8U cleanParaDirty_ToPM = False;
volatile INT32U curBandByteLeft = 0;

volatile INT8U headBusUsedByPrintData = False;

volatile INT32U debug_int1 = 0;
volatile INT32U debug_int2 = 0;
volatile INT32U debug_int3 = 0;

volatile INT16U g_FPGADMA_sendPkg = 0;
volatile VoltageParaType g_headVoltageData;
volatile FirePulseWParaType g_headFirePulseWData;

INT8U epromDataRead = TRUE;

INT32U g_DSP_FireNumber = 0;
INT32U g_FPGA_FireNumber = 0;

INT8U g_USBTransStoped = False;

OS_EVENT *IICSem; //Sem to lock IIC bus for EEPRom
OS_EVENT *IIC_KeyboardSem; //Sem to lock IIC bus for keyboard
OS_EVENT *CleaningParamSem; //Sem to lock CleaningParam
OS_EVENT *CalibrationParamSem; //Sem to lock all Calibration Setting
//OS_EVENT *PMCaliSettingSem; //Sem to lock PM Calibration Setting
OS_EVENT *LCDMenuConfigureSem; //

#ifndef IIC_Key_Board_LCD
OS_EVENT *MenuSem; //Sem to lock LCD menu operate
#endif

HostParaType host_paras;

INT8U factoryAuthorityStr[9] = "FACTORYZ";
INT8U byhxAuthorityStr[9] = "BYHXDATA";

//头板报告的支持喷头类型是大类，下面这个数组是大类包含的小类
INT8U headBoard_Category_Type_List[13][10] = {
	{0,0,0,0,0,0,0,0,0,0}, //虚类，喷头板不该报告支持这种喷头
	{HeadNo_Xaar_128_360,	HeadNo_Xaar_128_PLUS,	HeadNo_Xaar_Electron_360,	HeadNo_Xaar_Electron_PLUS,0,0,0,0,0,0}, 		//HEAD_CATEGORY_XAAR_128
	{HeadNo_Konica_KM_256_M,HeadNo_Konica_KM_256_L,	0,0,0,0,0,0,0,0},	//HEAD_CATEGORY_KONICA_KM256
	{HeadNo_Spectra_S_128,	HeadNo_Spectra_NOVA,	HeadNo_Spectra_GALAXY,HeadNo_Spectra_Polaris,HeadNo_Spectra_Polaris_35,0,0,0,0,0}, //HEAD_CATEGORY_SPECTRA
	{HeadNo_Konica_KM_512_M,HeadNo_Konica_KM_512_L, HeadNo_Konica_KM512LNX,0,0,0,0,0,0,0}, //HEAD_CATEGORY_KONICA_KM512
	{HeadNo_Xaar_382,0,0,0,0,0,0,0,0,0},
	{HeadNo_Konica_New512,0,0,0,0,0,0,0,0,0}, //HEAD_CATEGORY_KONICA_NEW512, no use
	{HeadNo_Spectra_Polaris,HeadNo_Spectra_Polaris_35,0,0,0,0,0,0,0,0}, //HEAD_CATEGORY_SPECTRA_POLARIS, for GZ
	{HeadNo_Epson_Gen5,0,0,0,0,0,0,0,0,0},
	{HeadNo_Konica_KM1024_M,HeadNo_Konica_KM1024_L,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{HeadNo_Ricoh_Gen4_64Pin,HeadNo_Ricoh_Gen4_G4,HeadNo_Ricoh_Gen4_G4L,0,0,0,0,0,0,0},
	{HeadNo_Ricoh_Gen5_2C_100Pin,0,0,0,0,0,0,0,0,0},
};


INT8U IIC_ReadEEPRom(INT16U subaddr, INT8U * buf, INT8U * len);
INT8U vol_CalHeadVoltage(INT8U headNum);
INT8U ReadLCDMenuConfig();
#ifdef FUNCTION_CLOSE_LCD
unsigned char MenuStr_EM_4_ID1C8[MAX_HEAD_DRIVE_NUM][MAX_HEAD_PARAM_NAME] ={0};
INT8U IS_SAME(char* str1,char *str2,INT8U len)
{
	INT8U i = 0;
	if(str1 == str2)
		return False;
	while(i < len)
	{
		if( *(str1 + i) != *(str2 + i))
			return False;
		i++;
	}
	return True;
}
INT8U Find_last_color(INT8U channel,char * str, INT8U len)
{
	INT8U i,j,k;
	if(MAX_HEAD_DRIVE_NUM != channel)
	{
		for( j = 0;j < MAX_HEAD_PARAM_NAME;j++)
		{
			if(MenuStr_EM_4_ID1C8[channel][j] != 0 && IS_SAME(&MenuStr_EM_4_ID1C8[channel][j],str,len))	
				return True;
		}
		return False;
	}
	else
	{
		for( i = 0;i < MAX_HEAD_DRIVE_NUM;i++)
		{
			for( j = 0;j < MAX_HEAD_PARAM_NAME;j++)
			{
				if(MenuStr_EM_4_ID1C8[i][j] != 0 && IS_SAME(&MenuStr_EM_4_ID1C8[i][j],str,len))	
					k++;
			}
		}
		return k;
	}
}

void UpdateHeadParamName()
{
	INT8U i,j,k,m,driver_num;
#ifdef EPSON_4H
	driver_num = MAX_HEAD_DRIVE_NUM;
#else
	if(factoryData.HeadType == HeadNo_Epson_Gen5 && headboardInfo.headBoardType == HEAD_BOARD_TYPE_EPSON_GEN5_2HEAD)
	{
		driver_num = 4;
	}
	else if((factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin || factoryData.HeadType == HeadNo_Ricoh_Gen4_G4 ||
				factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L) && headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_64Pin_8H)
	{
		driver_num = 8;
	}
	else if((factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin || factoryData.HeadType == HeadNo_Ricoh_Gen4_G4 ||
				factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L) && headboardInfo.headBoardType == HEAD_BOARD_TYPE_RICOH_GEN4_3H)
	{
		driver_num = 3;
	}
	else if(factoryData.HeadType == HeadNo_Ricoh_Gen5_2C_100Pin)
	{
		driver_num = MAX_HEAD_DRIVE_NUM;
	}
	else
		return;
#endif
	for(i=0; i<MAX_HEAD_DRIVE_NUM; i++)
	{
		MenuStr_EM_4_ID1C8[i][0] = 0;
	}

	for(i=0; i<epsonCaliSetting.printerSetting.ColorCount; i++)
	{
#if defined(EPSON_4H)||defined(HEAD_RICOH_G4)
		INT8U	j,k,g;
		for(g = 0; g < epsonCaliSetting.printerSetting.GroupCount_YContinue; g++ )
		{
			for(k = 0; k < epsonCaliSetting.printerSetting.YInterleaveNum; k++ )
			{
				INT8U colorCode = factoryDataEx.m_nColorOrder[i];
				INT8U lineNo = GetNozzleLineIndex(colorCode, k);
				INT8U HeadNo = GetHeadIndexWithInterleave(g, colorCode, k);
				INT8U GroupNo = g+ (k%epsonCaliSetting.printerSetting.GroupCount_YInterleave)*epsonCaliSetting.printerSetting.GroupCount_YContinue;
				INT8U Channle = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[HeadNo][lineNo];
#ifdef HEAD_RICOH_G4
#if defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
				INT8U Menu_Channle = Channle;
#else
				INT8U Menu_Channle = Channle/2 ;
#endif
#else
				INT8U Menu_Channle = (Channle%8)/2 +(Channle/16)*4;
#endif
				INT8U DIS_HEAD_NUM =True;
				INT8U DIS_COLOR_NUM =True;
				INT8U j_base = 0,color_tmp = 0,comma = 0;
				j=0;
				while(MenuStr_EM_4_ID1C8[Menu_Channle][j] != 0)
					j++;
				j_base = j; 		
				if(j!= 0)
				{
					MenuStr_EM_4_ID1C8[Menu_Channle][j] = ',';
					j++;
					comma = 1;
				}
				if( EPSON_ACTIVE_GROUPNUM == 1)
				{
					DIS_HEAD_NUM = False;
				}
				else
				{
					DIS_HEAD_NUM = True;
				}
				if((factoryDataEx.LayoutType & 0x2) == 0)
				{
					DIS_COLOR_NUM = False;
				}
				else
				{
					DIS_COLOR_NUM = True;
				}
#if defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
				DIS_COLOR_NUM = True;
#endif
				if(DIS_HEAD_NUM)
				{
					if(GetGroupHeadNumber()==1)
					{ 
						MenuStr_EM_4_ID1C8[Menu_Channle][j] = 'H';
						j++;
						MenuStr_EM_4_ID1C8[Menu_Channle][j] = '1' + HeadNo;
					}
					else
					{ 
						MenuStr_EM_4_ID1C8[Menu_Channle][j] = 'G';
						j++;
						MenuStr_EM_4_ID1C8[Menu_Channle][j] = '1' + GroupNo;
					}
					j++;			
				}
				if(factoryDataEx.m_nColorOrder[i] >= COLORCODE_SPOTCOLOR0 && factoryDataEx.m_nColorOrder[i] <= COLORCODE_SPOTCOLOR3)
				{
					color_tmp = factoryDataEx.m_nColorOrder[i] - COLORCODE_SPOTCOLOR0;
					color_tmp = LCDMenuConfig.SpotColorName[color_tmp];
					INT8U * pName = GetPrintedSpotColorName(color_tmp);

					for(color_tmp=0; pName[color_tmp] != 0; color_tmp++)
					{
						MenuStr_EM_4_ID1C8[Menu_Channle][j] = pName[color_tmp];
						j++;
					}
				}
				else
				{
					MenuStr_EM_4_ID1C8[Menu_Channle][j] = factoryDataEx.m_nColorOrder[i];
					j++;
				}
				INT8U is_zero = 0;
				if(MenuStr_EM_4_ID1C8[Menu_Channle][j_base] == ',')
					is_zero = 1;

				if(DIS_COLOR_NUM)
				{
					color_tmp = Find_last_color(MAX_HEAD_DRIVE_NUM,&MenuStr_EM_4_ID1C8[Menu_Channle][j_base + is_zero],j - j_base - comma);

					{
						MenuStr_EM_4_ID1C8[Menu_Channle][j] = '0' + color_tmp;
					}
				}
				else
				{
					if(Find_last_color(Menu_Channle,&MenuStr_EM_4_ID1C8[Menu_Channle][j_base + is_zero],j - j_base - comma))
					{
						MenuStr_EM_4_ID1C8[Menu_Channle][j_base] = 0;
						j = j_base;
					}				
				}

			}				
		}
#else
		INT8U last_d;
		for(m=0; m<epsonCaliSetting.printerSetting.GroupCount; m++)
		{
			INT8U h = GetHeadIndex(m, factoryDataEx.m_nColorOrder[i]);
			INT8U l = GetNozzleLineIndex(factoryDataEx.m_nColorOrder[i], 0);
			INT8U d = epsonCaliSetting.colorMap.ColorIndex2DataChannelIndex[h][l];

			if(factoryData.HeadType == HeadNo_Epson_Gen5 && headboardInfo.headBoardType == HEAD_BOARD_TYPE_EPSON_GEN5_2HEAD)
				d = d%8;
			d /= NOZZLE_BIT_WIDTH;
			if(factoryData.HeadType == HeadNo_Epson_Gen5 && headboardInfo.headBoardType == HEAD_BOARD_TYPE_EPSON_GEN5_2HEAD)
				d = DataChNO2DriverNO_EPSON_Gen5_2H[d];
			else
				d = DataChNO2DriverNO_RICOH_G4P_8H[d];

			if(m==0)
				last_d = d;
			else if(last_d == d)
				continue;

			j=0;
			while(MenuStr_EM_4_ID1C8[d][j] != 0)
				j++;
			if(j!= 0)
			{
				MenuStr_EM_4_ID1C8[d][j] = ',';
				j++;
			}
			if( epsonCaliSetting.printerSetting.GroupCount > 1 &&
					epsonCaliSetting.printerSetting.HeadCount > 2)
			{
				if(epsonCaliSetting.printerSetting.head_group == 1)
				{ //for epson 4H 4C
					MenuStr_EM_4_ID1C8[d][j] = 'H';
					j++;
				}
				else
				{ //for ricoh. and epson 4H 8C.
					MenuStr_EM_4_ID1C8[d][j] = 'G';
					j++;
				}
				MenuStr_EM_4_ID1C8[d][j] = '1'+m;
				j++;
			}
			if(factoryDataEx.m_nColorOrder[i] >= COLORCODE_SPOTCOLOR0 && factoryDataEx.m_nColorOrder[i] <= COLORCODE_SPOTCOLOR3)
			{
				k = factoryDataEx.m_nColorOrder[i] - COLORCODE_SPOTCOLOR0;
				k = LCDMenuConfig.SpotColorName[k];
				INT8U * pName = GetPrintedSpotColorName(k);

				for(k=0; pName[k] != 0; k++)
				{
					MenuStr_EM_4_ID1C8[d][j] = pName[k];
					j++;
				}
			}
			else
			{
				MenuStr_EM_4_ID1C8[d][j] = factoryDataEx.m_nColorOrder[i];
				j++;
			}

			MenuStr_EM_4_ID1C8[d][j] = 0;
		}
#endif				
	}

	for(i=0; i<MAX_HEAD_DRIVE_NUM; i++)
	{
		if(i<driver_num)
		{
			if( MenuStr_EM_4_ID1C8[i][0] == 0)
			{
				MenuStr_EM_4_ID1C8[i][0] = 'N';
				MenuStr_EM_4_ID1C8[i][1] = 'C';
				MenuStr_EM_4_ID1C8[i][2] = 0;
			}
		}
	}

}
#endif

INT8U Read_Dsp_Data(INT8U * EP0INdata,INT16U index,INT16U setuplength)
{
	INT16U len;

	if(index + setuplength > EPR_DSP_DISK_SIZE)
	{            
		len =EPR_DSP_DISK_SIZE - index;
	}
	else
		len = setuplength;
	if (len > 64)
		len = 64;
	if(!IIC_ReadEEPRom_Ex(EPR_DSP_DISK_OFFSET+index, (INT8U *)EP0INdata,&len))
	{
		return False;
	}
	return (INT8U)len;
}

INT8U Write_Dsp_Data(INT8U * EP0OUTdata,INT16U index,INT16U setuplength)
{
	INT16U len;

	if(index + setuplength > EPR_DSP_DISK_SIZE)
	{            
		len =EPR_DSP_DISK_SIZE - index;
	}
	else
		len = setuplength;
	if (len > 64)
		len = 64;

	if(!IIC_WriteEEPRom_Ex(EPR_DSP_DISK_OFFSET+index, (INT8U *)EP0OUTdata, &len))
	{
		return False;
	}
	return (INT8U)len;
}

void Printer_ReInitPos(INT32S farestPos)
{
	INT8U i;
	printer.org_oppLimit = farestPos;
	printer.autoCleanStartPos =farestPos - 3600*POS_FAC;
	printer.autoCleanEndPos = farestPos;
	printer.manualCleanPos = farestPos;
	for (i = 0; i < MAX_HEADS_CLEAN_NUMBER; i++)
		printer.headsCleanPos[i] = farestPos - 780*POS_FAC - i*PRINTER_SINGLECLEAN_SPACE;
}
#ifdef COORD_NEW_UV
void UV_Power_Percent(INT8U *src,INT8U power_level,INT8U base)
{
	INT8U i,j,k;
	memset((void *)src, 0, 13);
	for(i = 0 ;i < 100;i++)
		if((i >= base - 1)&&(( i - (base - 1))%power_level == 0))
		{
			src[i/8] |=(1<<i%8);		
		}

}
INT8U ReadFactoryUV_Setting()
{
	INT8U err;
	INT16U length;
	length = sizeof(EPR_FactoryUVSetting_Type);
	if(!IIC_ReadEEPRom_Ex(EPR_NEW_UV_SETTING_OFFSET, (INT8U * )&Ripstar_UV_Setting, &length))
	{
		return False;
	}
	if(Ripstar_UV_Setting.len != length ||Ripstar_UV_Setting.flag != UV_SETTING_FLAT)//最后2个字节PM未使用
	{
		memset(&Ripstar_UV_Setting, 0, sizeof(EPR_FactoryUVSetting_Type));
		Ripstar_UV_Setting.len = sizeof(EPR_FactoryUVSetting_Type);
		Ripstar_UV_Setting.flag = UV_SETTING_FLAT;
		Ripstar_UV_Setting.Front_Light_Head0_Length = 10.5f;//cm
		Ripstar_UV_Setting.Back_Light_Head0_Length = 2.0f;
		Ripstar_UV_Setting.Head_Region_length = 4.5f;
		Ripstar_UV_Setting.UV_Light_Length = 2.0f;
		Ripstar_UV_Setting.Advance_Open_Length = 0;
		Ripstar_UV_Setting.Delay_Close_Length = 0;
		UV_Power_Percent(&Ripstar_UV_Setting.Power[0],10,50);
		Ripstar_UV_Setting.UV_SETTING = 0x3232000F;
		SaveFactoryUV_Setting();
	}
	return True;
}

INT8U SaveFactoryUV_Setting()
{
	INT8U ret = True;
	INT16U len = sizeof(EPR_FactoryUVSetting_Type);
	ret = IIC_WriteEEPRom_Ex(EPR_NEW_UV_SETTING_OFFSET, (INT8U *)&Ripstar_UV_Setting, &len);
	return ret;
}
#endif
void Global_Init(void)
{
	INT8U i;

	headboardInfo.headCategory = 0;
	headboardInfo.headBoardType = 0;
	curPos.x = 0;
	curPos.y = 0;
	curPos.z = 0;
	curPos.f = 0;

	printerPhysicalSize.x = 0;
	printerPhysicalSize.y = 0;
	printerPhysicalSize.z = 0;
	printerPhysicalSize.f = 0;

	fwInfo.version = BOARD_VERSION;
	fwInfo.mt_version = 0;
	fwInfo.hd_version = 0;
	fwInfo.manufacturerID = MANUFACTURER_ID;
	fwInfo.productID = PRODUCT_ID;
	strcpy(fwInfo.mfw_date, __DATE__);
	memset(fwInfo.mtfw_date, 0, 12);
	memset(fwInfo.hdfw_date, 0, 12);

	//#ifdef Y_ENCODER_ENABLE					
	g_yEncoderInf.bandIndex = -1;
	g_yEncoderInf.encoderPos = 0;
	g_yEncoderInf.bandY = 0;
	g_yEncoderInf.nextBandY = 0;
	g_yEncoderInf.bandResY = 0;
	g_yEncoderInf.fStepTotal = 0;

	g_adjustInf.passStepValue = 0;
	g_adjustInf.passAdvance = 0;
	g_adjustInf.feedBackY = 0;
	g_adjustInf.bidirectionValue = 0;

	g_shakehandPassed = False;

	for (i = 0; i<32;i++)
		dspDebugInfo[i] = 0;
	for (i = 0; i<32;i++)
		dspPrivateData[i] = 0;				
	//#endif				

#if defined( HEAD_EPSON_GEN5) ||  defined(HEAD_RICOH_G4)
	FireCount = FireCountEx = 0;
	bFPGADebugInfoUpdated = FALSE;				
	for (i = 0; i<sizeof(headFPGADebugInfo);i++)
		headFPGADebugInfo[i] = 0;
	bReadHeadBoardParam = 0;
	HBReadAddr = HBReadLen = 0;
	for (i = 0; i<sizeof(HBParamBuf);i++)
		HBParamBuf[i] = 0;

	memset((void*)&CaliPrintSettingFromUSB, 0, sizeof(CaliPrintSettingFromUSB));

	CaliPrintSettingFromUSB.VSDModel = 3;

	memset((void*)g_headParameterPercent, 0, sizeof(g_headParameterPercent));
#endif

}


//Find if a printhead is in a printhead category
//False, not included
//True, included
INT8U global_findPrintHeadInCategoryList(INT8U category, INT8U headType)
{
	INT8U i;
	for ( i = 0; i < 10; i++)
	{
		if (headBoard_Category_Type_List[category][i] == headType)
			return True;
	}
	return False;
}

#if defined (EPSON_CLEAN_UPDOWN)||defined (RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
void ReadCleanParamEPRData()
{
	if(ReadCleanParamEPR_EPSON_Allwin(&cleanparam_EPSON_ALLWIN) == False)
	{
		epromDataRead = False;
		CONSOL_Printf("cleanparam_EPSON_ALLWIN read failed.");
	}
#ifdef MANUFACTURER_DGEN
	if(cleanparam_EPSON_ALLWIN.IdleClean_intervalTime < 1 || cleanparam_EPSON_ALLWIN.IdleClean_intervalTime > 12)
	{
		cleanparam_EPSON_ALLWIN.IdleClean_intervalTime = 0;
	}
#endif
}
#elif (defined (EPSON_CLEAN_INTEGRATE)|| defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3))
void ReadCleanParamEPRData()
{
	if(ReadCleanParamEPR_EPSON_Micolor(&cleanparam_EPSON_MICOLOR) == False)
	{
		epromDataRead = False;
		CONSOL_Printf("cleanparam_EPSON_MICOLOR read failed.");
	}
}
#endif

void ReadFirePulseEPRData()
{
	INT16U i;
	INT8U err;
	INT16U length;
	//	INT8U len;

	i = 0;
	length = sizeof(FirePulseWParaType);
	while (i < length)
	{
		INT8U len;
		if (length - i > 64)
			len = 64;
		else
			len = length - i;
		OSSemPend(IICSem, 0, &err);
		if (IIC_ReadEEPRom(EPR_FIREPULSEW_DATA_OFFSET+i, ((INT8U *)&g_headFirePulseWData)+i, &len) == False)
		{
			OSSemPost(IICSem);
			break;
		}
		OSSemPost(IICSem);
		i += len;			
	}

	if (i< length)
	{
		epromDataRead = False;
		CONSOL_Printf("Head Pulse Data read failed.");
	}
	else
	{	
		if (g_headFirePulseWData.flag != EPR_HEAD_FIREPULSEW_FLAG)
		{
			CONSOL_Printf("Head Pulse Data is not set.");
			for (i=0;i<16;i++)
				g_headFirePulseWData.data[i] = 0xFF;
			g_headFirePulseWData.headUpdated = True;
		}else
			g_headFirePulseWData.headUpdated = False;
	}
}

#ifdef SUPPORT_HEAD_HEATER
void ReadUVHeatTemprature()
{
	INT16U i;
	INT8U err;
	INT16U length;
	//	INT8U len;

	i = 0;
	length = sizeof(UVHeatTempratureType);
	while (i < length)
	{
		INT8U len;
		if (length - i > 64)
			len = 64;
		else
			len = length - i;
		OSSemPend(IICSem, 0, &err);
		if (IIC_ReadEEPRom(EPR_HEAT_TEMPRATURE_DATA_OFFSET+i, ((INT8U *)&g_UVheatTemprature)+i, &len) == False)
		{
			OSSemPost(IICSem);
			break;
		}
		OSSemPost(IICSem);
		i += len;			
	}

	if (i< length)
	{
		epromDataRead = False;
		CONSOL_Printf("UV Heat Temparature Data read failed.");
	}
	else
	{	
		if (g_UVheatTemprature.flag != EPR_HEAD_HEATTEMPRATURE_FLAG)
		{
			CONSOL_Printf("UV Heat Temparature Data is not set.");
#ifdef CONVERSION_BOARD
			for (i=0;i<MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM + MAX_COLOR_NUMBER;i++)	
#else			
				for (i=0;i<MAX_HEAD_NUMBER + MAX_COLOR_NUMBER;i++)	
#endif				
					g_UVheatTemprature.data[i] = 50;  //42.4 degree			
		}

#ifdef CONVERSION_BOARD
		for (i=0;i<MAX_HEAT_CHANNEL_PRE_HB*MAX_HB_NUM + MAX_COLOR_NUMBER;i++)	
#else		
			for (i=0;i<MAX_HEAD_NUMBER + MAX_COLOR_NUMBER;i++)
#endif
			{
#ifdef MANUFACTURER_DYSS
				if((g_UVheatTemprature.data[i]/2) > 55)
					g_UVheatTemprature.data[i] = 20;
#else
				if((g_UVheatTemprature.data[i]/5) > 50)
					g_UVheatTemprature.data[i] = 100;
#endif
			}
		g_UVheatTemprature.headUpdated = False;					
	}
}
#endif

//unit is pl.
INT16U GetDotVolume(INT8U HeadType)
{
	switch(HeadType)
	{
	case HeadNo_Konica_KM_512_M:
	case HeadNo_Konica_KM_256_M:
	case HeadNo_Konica_KM_128_M:
	case HeadNo_Konica_KM1024_M:
		return 14;
	case HeadNo_Konica_KM_512_L:
	case HeadNo_Konica_KM_256_L:
	case HeadNo_Konica_KM_128_L:
	case HeadNo_Konica_KM1024_L:
		return 42;
	case HeadNo_Konica_KM512LNX:
		return 35;
	case HeadNo_Spectra_Polaris:
		return 15;
	case HeadNo_Spectra_Polaris_35:
		return 35;
	case HeadNo_Epson_Gen5: //VSD4:3.5/7/10. VSD3:7/14/20. VSD1:10/20/30. VSD2:25.
		return 14; //use EPSON VSD3 middle.
	case HeadNo_Ricoh_Gen4_64Pin: //similar with EPSON.
		return 14;  //????, need testing.
	case HeadNo_Ricoh_Gen4_G4:
	case HeadNo_Ricoh_Gen4_G4L:  
		return 20;  //????, need testing.
	case HeadNo_Ricoh_Gen5_2C_100Pin:
		return 20;  //????, need testing.
	case HeadNo_Xaar_126:
	case HeadNo_Xaar_128_360:
	case HeadNo_Xaar_128_PLUS:
	case HeadNo_Xaar_500:
	case HeadNo_Spectra_S_128:
	case HeadNo_Spectra_NOVA:
	case HeadNo_Spectra_GALAXY:
	case HeadNo_Xaar_Electron_360:
	case HeadNo_Xaar_Electron_PLUS:
	case HeadNo_Xaar_382:
	case HeadNo_Xaar_1001:
	default:
		//Cheney: I don't known the dot size of these heads. so I only return the default value.
#ifdef INK_PASSWORD
		status_ReportStatus(STATUS_WAR_INK_PASSWORD_UNKNOWN_DOT_VOLUME,	STATUS_SET);
#endif
		return 42;
	}
}

INT16U GetNozzleCount(INT8U HeadType)
{
	switch(HeadType)
	{
	case HeadNo_Xaar_126:
		return 126;
	case HeadNo_Xaar_128_360:
	case HeadNo_Xaar_128_PLUS:
	case HeadNo_Xaar_Electron_360:
	case HeadNo_Xaar_Electron_PLUS:
		return 128;
	case HeadNo_Xaar_382:
		return 384;
	case HeadNo_Xaar_500:
		return 512;
	case HeadNo_Xaar_1001:
		return 1001;    //????
	case HeadNo_Spectra_S_128:
		return 128;
	case HeadNo_Spectra_NOVA:
	case HeadNo_Spectra_GALAXY:
	case HeadNo_Spectra_Polaris:
	case HeadNo_Spectra_Polaris_35:
		return 256;
	case HeadNo_Konica_KM1024_M:
	case HeadNo_Konica_KM1024_L:
		return 1024;
	case HeadNo_Konica_KM_512_M:
	case HeadNo_Konica_KM_512_L:
	case HeadNo_Konica_New512:
	case HeadNo_Konica_KM512LNX:
		return 512;
	case HeadNo_Konica_KM_256_M:
	case HeadNo_Konica_KM_256_L:
		return 256;
	case HeadNo_Konica_KM_128_M:
	case HeadNo_Konica_KM_128_L:
		return 128;
	case HeadNo_Epson_Gen5:
		return 180*8;
	case HeadNo_Ricoh_Gen4_64Pin:
	case HeadNo_Ricoh_Gen4_G4:
	case HeadNo_Ricoh_Gen4_G4L:
		return 192*2;
	case HeadNo_Ricoh_Gen5_2C_100Pin:
		return 320*4;
	default:
		//Cheney: I don't known the nozzle count of these heads.
		status_ReportStatus(STATUS_WAR_UNKNOWN_NOZZLE_COUNT,	STATUS_SET);
		return 180*8; //only for safe.
	}
}

INT8U GetPhysicsHeadNum(void)
{
	if(factoryData.HeadType == HeadNo_Epson_Gen5 || factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin || 
			factoryData.HeadType == HeadNo_Ricoh_Gen4_G4 || factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L ||
			factoryData.HeadType == HeadNo_Ricoh_Gen5_2C_100Pin)
	{
		int YInterleaveNum_Head = INTERLEAVE_CALIBRATION;
		int YInterleaveNum_InHead;
		if(factoryDataEx.YInterleaveNum> YInterleaveNum_Head)
			YInterleaveNum_InHead = YInterleaveNum_Head;
		else
			YInterleaveNum_InHead = factoryDataEx.YInterleaveNum;
		int head_group = (COLOR_NUMBER_CALIBRATION * YInterleaveNum_InHead + NOZZLE_LINE_COUNT_PER_HEAD - 1)/ NOZZLE_LINE_COUNT_PER_HEAD;

		return head_group * EPSON_PERMIT_GROUPNUM;
	}
	else
	{
		INT8U HeadNumber;
		if(IS_2COLOR_1HEAD)
			HeadNumber = (factoryData.color_num + factoryData.spot_color1_headnum + factoryData.spot_color2_headnum + 1)/2 * FACTORYDATA_LOGIC_GROUP_NUM;
		else
			HeadNumber = (factoryData.color_num + factoryData.spot_color1_headnum + factoryData.spot_color2_headnum) * FACTORYDATA_LOGIC_GROUP_NUM;
		return HeadNumber;
	}
}

INT8U IsNeed128M(void)
{
	INT16U nozzlecount = GetNozzleCount(factoryData.HeadType);  
	INT8U HeadNumber = GetPhysicsHeadNum();
	INT8U BitPerNozzle = (factoryData.HeadType == HeadNo_Epson_Gen5 ||
			factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin || 
			factoryData.HeadType == HeadNo_Ricoh_Gen4_G4 || 
			factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L ||
			factoryData.HeadType == HeadNo_Ricoh_Gen5_2C_100Pin) ? 2 : 1 ;
	//Here, we don't consider the bus width align problem. 
	INT32U DataPerFire = (HeadNumber * nozzlecount * BitPerNozzle + 15) / 16 * 2;
	INT32U DPI = (factoryData.HeadType == HeadNo_Epson_Gen5 ||
			factoryData.HeadType == HeadNo_Ricoh_Gen4_64Pin || 
			factoryData.HeadType == HeadNo_Ricoh_Gen4_G4 || 
			factoryData.HeadType == HeadNo_Ricoh_Gen4_G4L ||
			factoryData.HeadType == HeadNo_Ricoh_Gen5_2C_100Pin 
		     ) ? 1440 : 720;
	const float CarriageWidth = 40.0; //cm
	INT32U PassMemSize = (INT32U)((PAPER_MEDIA_WIDTH_INCH + CarriageWidth/2.54) * DPI * DataPerFire);

	if(PassMemSize >= 64 * 1024 * 1024 - 2 * RESERVED_MEMSIZE)
		return True;
	else
		return False;
}

//if return TRUE, Ink is runout.
INT8U CheckInkVolume()
{
	if ( (password.m_bHaveInkPwd) && (password.m_bInkLimited) )
	{
		int i;
		INT32U totalInk = 0;
		for(i =0; i<MAX_COLOR_NUMBER; i++)
			totalInk +=  usedInkVolume.InkVolume[i][1];
		totalInk /= INK_VOLUME_FACTOR_PER_LITER;

		if (password.m_bInkLimited < totalInk )
		{
			status_ReportStatus(STATUS_FTA_LIMITEDINK_RUNOUT, STATUS_SET); //Ink Limited run out
			status_ReportStatus(STATUS_WAR_LIMITEDINK_LAST,	STATUS_CLR); 
			return True;
		}
		else if (password.m_bInkLimited < (totalInk + INK_LIMIT_WAR_LAST))
		{
			status_ReportStatus(STATUS_WAR_LIMITEDINK_SECOND, STATUS_CLR); 
			status_ReportStatus(STATUS_WAR_LIMITEDINK_LAST,	STATUS_SET); 
		}
		else if (password.m_bInkLimited < (totalInk + INK_LIMIT_WAR_SECOND))
		{
			status_ReportStatus(STATUS_WAR_LIMITEDINK_FIRST,	STATUS_CLR);
			status_ReportStatus(STATUS_WAR_LIMITEDINK_SECOND,	STATUS_SET); 
		}else if (password.m_bInkLimited < (totalInk +  INK_LIMIT_WAR_FIRST))
			status_ReportStatus(STATUS_WAR_LIMITEDINK_FIRST,	STATUS_SET);
	}
	return False;
}

//return no zero if pass handshake.
INT8U HandShakeInkDot(INT32U * pInkDotNumber)
{
	INT32U totalInk = 0;
	int i;

	for(i =0; i< MAX_COLOR_NUMBER; i++)
	{
		totalInk += pInkDotNumber[i];
	}
	totalInk ^= INK_DOT_CHECK_MASK;
	if(totalInk != pInkDotNumber[i])
	{
		if ( (password.m_bHaveInkPwd) && (password.m_bInkLimited) )
			return INKDOT_CHK_NO_PASS;
		else
			return INKDOT_CHK_OLD_SW;
	}
	else
	{
		if ( (password.m_bHaveInkPwd) && (password.m_bInkLimited) )
			return INKDOT_CHK_PASS;
		else
			return INKDOT_CHK_NO_LIMIT;
	}
}

void FlushInkVolume(INT32U * pInkDotNumber, INT8U bForceFlush)
{
	INT32U totalInk = 0;
	int i;

	//if ( (password.m_bHaveInkPwd) && (password.m_bInkLimited) )
	{
		if(pInkDotNumber != 0)
		{
			for(i =0; i< MAX_COLOR_NUMBER; i++)
			{
				INT32U dotsVolume = usedInkVolume.InkVolume[i][0];
				dotsVolume += ((pInkDotNumber[i] * usedInkVolume.DotVolume)/16)*1000;
				if(dotsVolume < usedInkVolume.InkVolume[i][0])
				{
					usedInkVolume.InkVolume[i][1]++;
				}
				usedInkVolume.InkVolume[i][0] = dotsVolume; 
				totalInk += usedInkVolume.InkVolume[i][1];
			}
		}
		else
			totalInk = usedInkVolume.CurrentTotalInk;

		if(bForceFlush || totalInk - usedInkVolume.CurrentTotalInk >= INK_LIMIT_EPR_WRT_UNIT)
		{
			INT8U len;
			INT8U err;
			usedInkVolume.CurrentTotalInk = totalInk;
			len = sizeof(usedInkVolume);
			OSSemPend(IICSem, 0, &err);
			if (IIC_WriteEEPRom(EPR_USED_INK_VOLUME_OFFSET, (INT8U *)&usedInkVolume, &len) == False)
				err = 1;
			else
				err = 0;
			OSSemPost(IICSem);
			if (err > 0)	
				status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
		}
	}
}

#if defined( HEAD_EPSON_GEN5) ||  defined(HEAD_RICOH_G4)
void InitHeadParaChannelMap()
{
}
void AdjustPlatSpace()
{
	if(EPSON_PERMIT_GROUPNUM > EPSON_ACTIVE_GROUPNUM)
	{
		if( EPSON_PERMIT_GROUPNUM == 2)
		{
			if( (LCDMenuConfig.Active_HeadMask & ((1<<EPSON_PERMIT_GROUPNUM)-1)) == 0x2)
			{
				printer.platSpace -= (INT32S)(factoryData.m_fHeadXGroupSpace * (INT32S)factoryDataEx.m_nXEncoderDPI);
			}
		}
		else if( EPSON_PERMIT_GROUPNUM == 4)
		{
			if((factoryDataEx.LayoutType & (1<<3)) == 0)
			{
				if((LCDMenuConfig.Active_HeadMask & ((1<<EPSON_PERMIT_GROUPNUM)-1)) == 0x2 ||
						(LCDMenuConfig.Active_HeadMask & ((1<<EPSON_PERMIT_GROUPNUM)-1)) == 0x8
				  )
				{
					printer.platSpace -= (INT32S)(factoryData.m_fHeadXGroupSpace * (INT32S)factoryDataEx.m_nXEncoderDPI);
				}				
			}
			else
			{
				if(EPSON_ACTIVE_GROUPNUM == 1)
				{
					if(LCDMenuConfig.Active_HeadMask == 2)
						printer.platSpace -= (INT32S)(factoryData.m_fHeadXGroupSpace * (INT32S)factoryDataEx.m_nXEncoderDPI);
					else if(LCDMenuConfig.Active_HeadMask == 4)
						printer.platSpace -= (INT32S)(2*factoryData.m_fHeadXGroupSpace * (INT32S)factoryDataEx.m_nXEncoderDPI);
					else if(LCDMenuConfig.Active_HeadMask == 8)
						printer.platSpace -= (INT32S)(3*factoryData.m_fHeadXGroupSpace * (INT32S)factoryDataEx.m_nXEncoderDPI);
				}
				else if(EPSON_ACTIVE_GROUPNUM == 2)
				{
					if(LCDMenuConfig.Active_HeadMask == 0xC)
						printer.platSpace -= (INT32S)(2*factoryData.m_fHeadXGroupSpace * (INT32S)factoryDataEx.m_nXEncoderDPI);
				}
				else if(EPSON_ACTIVE_GROUPNUM == 3)
				{
					if(LCDMenuConfig.Active_HeadMask == 0xe)
						printer.platSpace -= (INT32S)(factoryData.m_fHeadXGroupSpace * (INT32S)factoryDataEx.m_nXEncoderDPI);
				}
			}
		}
	}
#ifdef MANUFACTURER_DYSS
	if(COLOR_NUMBER_CALIBRATION == 6)
#ifdef RICOH_3H
		printer.platSpace -= (INT32S)(factoryData.m_fHeadXColorSpace * (INT32S)factoryDataEx.m_nXEncoderDPI);
#else
	printer.platSpace -= (INT32S)(factoryData.m_fHeadXColorSpace * (INT32S)factoryDataEx.m_nXEncoderDPI)*2;
#endif
#endif
}
#endif
void Cal_active_Group_num(void)
{
	INT8U flag = 0;
#if defined( RICOH_3H	) || defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
	if(factoryData.group_num < 0)
		flag = 1;
#endif
	if(EPSON_PERMIT_GROUPNUM == 2)
	{
		if((LCDMenuConfig.Active_HeadMask & 0x3) == 0x3)
			factoryData.group_num = 2;
		else
			factoryData.group_num = 1;
	}
	else if(EPSON_PERMIT_GROUPNUM == 4)	
	{
		if((LCDMenuConfig.Active_HeadMask & 0xF) == 0xF)
			factoryData.group_num = 4;
		else if((LCDMenuConfig.Active_HeadMask & 0xF) == 0x3)
			factoryData.group_num = 2;
		else if((LCDMenuConfig.Active_HeadMask & 0xF) == 0xC)
			factoryData.group_num = 2;
		else if((LCDMenuConfig.Active_HeadMask & 0xF) == 0x7)
			factoryData.group_num = 3;
		else		
			factoryData.group_num = 1;
	}
	else
		factoryData.group_num = 1;
	if(flag) 
		factoryData.group_num = -factoryData.group_num;
}
//Initial printer initial setting here
void Printer_Init(void)
{
	INT16U i;
	INT8U err;
	INT16U length;
	INT8U len;
#if 0
	INT8U epromlen;
	byhxData.boardID = 0xFFFFFFFF;
	byhxData.manufacturerID = 0xFFFF;
	byhxData.supportedHeadList[0] = HeadNo_Ricoh_Gen5_2C_100Pin;

	OSSemPend(IICSem, 0, &err);
	if (IIC_WriteEEPRom(EPR_BYHX_DATA_OFFSET, (INT8U *)&byhxData, &epromlen) == False)
	{
		OSSemPost(IICSem);
		status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
	}

	epromlen = EPR_BYHX_AUTHORITY_FLAG_SIZE;
	if (IIC_WriteEEPRom(EPR_BYHX_AUTHORITY_FLAG_OFFSET, byhxAuthorityStr, &epromlen) == False)
	{
		OSSemPost(IICSem);
		status_ReportStatus(STATUS_FTA_EEPROM_WRITE, STATUS_SET);
	}				
	OSSemPost(IICSem);
#endif    

	printer.manualCleanPos = PRINTER_MANUAL_CLEAN_POS;

	printer.flashNearPos = PRINTER_FLASH_NEAR_POS;
	printer.flashFarPos = PRINTER_FLASH_FAR_POS;

#ifdef SUPPORT_MOTOR_CONTROL  
	printer.movSpeed = 0;
#else
	printer.movSpeed = PRINTER_MOVE_SPEED;
#endif
	printer.cleanSpeed = PRINTER_CLEAN_SPEED;	
	printer.feedSpeed = PRINTER_MEDIAFEED_SPEED;

	//cleanPara.flash = True;
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Just test
#if ((defined(HEAD_RICOH_G4)&&defined(RICOH_CLEAN_PRESS))&&!defined(UV_PRINTER))||defined(MANUFACTURER_DOCAN_UV)
	cleanPara.flash = True;
#else	
#ifdef OPEN_IDLE_FLUSH
	cleanPara.flash = True;
#else
	cleanPara.flash = False;
#endif	
#endif
	// cleanPara.flash = False;

	cleanPara.pause_gohome = False;
	cleanPara.flash_interval = 100; //10 times per second. 2007/8/2 in INWEAR
	cleanPara.longflash_beforeprint = True;
	cleanPara.autoCleanTimes = 1;
	cleanPara.manualCleanTimes = 4;
#if defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_INTEGRATE_3)
	cleanPara.longflash_passInterval = 1;
#else
	cleanPara.longflash_passInterval = 10;
#endif
	cleanPara.blowInk_passInterval = 10;
#if defined (EPSON_CLEAN_UPDOWN) || defined (EPSON_CLEAN_INTEGRATE)||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
	cleanPara.autoClean_way = 4;
#endif
	password.m_bHaveTimePwd = False;
	password.m_bHaveOptionPwd = False;
	password.m_bHaveInkPwd = False;
	password.m_bLanguage = 0; //Chinese

	len = EPR_TIME_PASSWORD_STR_SIZE;
	OSSemPend(IICSem, 0, &err);
	if (IIC_ReadEEPRom(EPR_TIME_PASSWORD_STR_OFFSET, password.m_sTimePwd, &len) == False)
		status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);

	password.m_sTimePwd[EPR_TIME_PASSWORD_STR_SIZE] = 0;
	len = EPR_OPTION_PASSWORD_STR_SIZE;
	if (IIC_ReadEEPRom(EPR_OPTION_PASSWORD_STR_OFFSET, password.m_sOptionPwd, &len) == False)
		status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);
	password.m_sOptionPwd[EPR_OPTION_PASSWORD_STR_SIZE] = 0;

	len = EPR_INK_PASSWORD_STR_SIZE;
	if (IIC_ReadEEPRom(EPR_INK_PASSWORD_STR_OFFSET, password.m_sInkPwd, &len) == False)
		status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);
	password.m_sInkPwd[EPR_INK_PASSWORD_STR_SIZE] = 0;
	OSSemPost(IICSem);
#if 0
	i = 0;
	length = sizeof(EPR_FactoryDataType);
	while (i < length)
	{
		if (length - i > 64)
			len = 64;
		else
			len = length - i;
		OSSemPend(IICSem, 0, &err);
		if (IIC_ReadEEPRom(EPR_FACTORY_DATA_OFFSET+i, ((INT8U *)&factoryData)+i, &len) == False)
		{
			OSSemPost(IICSem);
			break;
		}
		OSSemPost(IICSem);
		i += len;			
	}
#endif	

#ifdef FUNCTION_CLOSE_LCD
	UpdateHeadParamName();
#endif

	if (ReadFactoryData(&factoryData))
	{
		//		epromDataRead = TRUE;
		printer.xEncoder = factoryData.EncoderType; ////!0 使用光栅， 0 使用司服编码
		//		CONSOL_Printf("Read printer parameters OK\r\n.");
	}	
	else
	{
		epromDataRead = FALSE;
		printer.xEncoder = 0; ////!0 使用光栅， 0 使用司服编码
		CONSOL_Printf("Read printer parameters Failed\r\n.");
	}	
	//#if defined(HEAD_RICOH_G4)
#if 0
#ifdef  RICOH_G5_3H
	factoryData.HeadType = HeadNo_Ricoh_Gen5_2C_100Pin;
#else
#ifdef MANUFACTURER_TATE_EPSON
	factoryData.HeadType = HeadNo_Ricoh_Gen4_G4;
	//factoryData.HeadType = HeadNo_Ricoh_Gen4_64Pin;
#else
	factoryData.HeadType = HeadNo_Ricoh_Gen4_G4;
	//factoryData.HeadType = HeadNo_Ricoh_Gen4_64Pin;
#endif
#endif
#endif

#if defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
	factoryData.HeadType = HeadNo_Ricoh_Gen5_2C_100Pin;
#endif

#if defined( RICOH_3H	) || defined (RICOH_G5_3H)
	if(factoryData.group_num > 0)
		factoryData.group_num = -factoryData.group_num;
#endif
#if defined( HEAD_EPSON_GEN5) ||  defined(HEAD_RICOH_G4)
	if(!ReadFactoryDataEx_EPSON(&factoryDataEx))
	{
		epromDataRead = FALSE;
		CONSOL_Printf("Read extended printer parameters Failed\r\n.");
	}	
#endif

#if defined(EPSON_DX5E)

#if defined(HEAD_EPSON_GEN5)
	factoryDataEx.m_nBitFlagEx |= FACTORYDATA_EX_DX5E;
#else
#error
#endif
#else
	factoryDataEx.m_nBitFlagEx &= ~FACTORYDATA_EX_DX5E;
#endif

#ifdef GZ_KEY_USED
	factoryDataEx.m_nBitFlagEx |= FACTORYDATA_EX_GZ_KEY;
#else
	factoryDataEx.m_nBitFlagEx &= ~FACTORYDATA_EX_GZ_KEY;
#endif

#ifdef HEAD_EPSON_GEN5S
	factoryDataEx.m_nBitFlagEx |= FACTORYDATA_EX_EPSON_GEN6;
#else
	factoryDataEx.m_nBitFlagEx &= ~FACTORYDATA_EX_EPSON_GEN6;
#endif

#ifdef RIPSTAR_FLAT_EX
	factoryDataEx.m_nBitFlagEx |= FACTORYDATA_EX_RIPSTAR_EX;
#else
	factoryDataEx.m_nBitFlagEx &= ~FACTORYDATA_EX_RIPSTAR_EX;
#endif

	//#ifdef DYSS_1270
	//	factoryDataEx.m_nXEncoderDPI = 1270;
	//#endif

	printer.autoCleanStartPos = PRINTER_AUTOCLEAN_START_POS;
	printer.autoCleanEndPos = PRINTER_AUTOCLEAN_END_POS;

	printer.orgLimit = PRINTER_POSITION_N_LIMIT;
	printer.org_oppLimit = PRINTER_POSITION_F_LIMIT;

	printer.cleanPosOrgOpp = PRINTER_CLEAN_FAR;
	for (i = 0; i < MAX_HEADS_CLEAN_NUMBER; i++)  //For ALLWIN 8 KM256  heads, 2008/4/29
		printer.headsCleanPos[i] = 1950 + 1880*i;


	i = 0;
	length = sizeof(VoltageParaType);
	while (i < length)
	{
		if (length - i > 64)
			len = 64;
		else
			len = length - i;
		OSSemPend(IICSem, 0, &err);
		if (IIC_ReadEEPRom(EPR_VOLTAGE_DATA_OFFSET+i, ((INT8U *)&g_headVoltageData)+i, &len) == False)
		{
			OSSemPost(IICSem);
			break;
		}
		OSSemPost(IICSem);
		i += len;			
	}

	g_headVoltageData.headUpdated = False;

	if (i< length)
	{
		CONSOL_Printf("Head Voltage Data read failed.");
		epromDataRead = False;
	}
	else
	{		
		if (g_headVoltageData.flag != EPR_HEAD_VOLTAGE_FLAG)
		{
			CONSOL_Printf("Head Voltage Data is not set.");
			for (i=0;i<MAX_VOLTAGENUMBER;i++)
				g_headVoltageData.manualData[i] = 20;  //0 V
		}else
			g_headVoltageData.autoCon = False;
	}
#ifdef CONVERSION_BOARD	
	for (i=0;i<MAX_HEAD_NUMBER*MAX_HB_NUM;i++)
#else
		for (i=0;i<MAX_HEAD_NUMBER;i++)
#endif
		{
#ifdef MANUFACTURER_DYSS
			g_headTemeprature[i] = 2*i; //25 degree
#else
			g_headTemeprature[i] = 5*i; //25 degree
#endif			
		}

	for (i=0;i<MAX_COLOR_NUMBER;i++)
	{
		g_headHeaterThermistorTemeprature[i] = 0;
	}	
	for (i=0;i<MAX_VOLTAGENUMBER;i++)
		g_curHeadVoltage[i] = vol_CalHeadVoltage(i);

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	ReadCleanParamEPRData();
#endif

#if defined(EPSON_CLEAN_UPDOWN)||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
	if(cleanparam_EPSON_ALLWIN.bAutoClean)
		cleanPara.autoClean_passInterval = cleanparam_EPSON_ALLWIN.AutoClean_passInterval;
	else
		cleanPara.autoClean_passInterval = 0; //关闭
#elif defined(MANUFACTURER_MICOLOR_EPSON) || defined(MANUFACTURER_COLORFUL)
	if(cleanparam_EPSON_MICOLOR.bAutoClean)
		cleanPara.autoClean_passInterval = cleanparam_EPSON_MICOLOR.AutoClean_passInterval;
	else
		cleanPara.autoClean_passInterval = 0; //关闭
#else   
	cleanPara.autoClean_passInterval = 20;	
#endif

	ReadFirePulseEPRData();

#ifdef SUPPORT_HEAD_HEATER	
	ReadUVHeatTemprature();
#endif

	i = 0;
	length = sizeof(EPR_BYHXDataType);

#ifdef COORD_NEW_UV
	ReadFactoryUV_Setting();
#if defined( MANUFACTURER_SAIBO) ||defined(MANUFACTURER_SAIBO_ROLL_UV)
	UV_Power_Percent(&Ripstar_UV_Setting.Power[0],25,50);
	uv_UVMode = (Ripstar_UV_Setting.UV_SETTING >> 24);
#elif defined(MANUFACTURER_DYSS)||defined(ORIC_FUNCTION)
	UV_Power_Percent(&Ripstar_UV_Setting.Power[0],10,10);
	uv_UVMode = (Ripstar_UV_Setting.UV_SETTING >> 24)/10;
#elif defined(MANUFACTURER_DOCAN_UV)
	UV_Power_Percent(&Ripstar_UV_Setting.Power[0],100,100);	
	uv_UVMode = (Ripstar_UV_Setting.UV_SETTING >> 24)
#elif defined(MANUFACTURER_FULGENCY_FLAT_UV)
		UV_Power_Percent(&Ripstar_UV_Setting.Power[0],25,25);
	uv_UVMode = (Ripstar_UV_Setting.UV_SETTING >> 24);		
#else
	UV_Power_Percent(&Ripstar_UV_Setting.Power[0],10,10);
#endif
	Configure_Uv_Power(uv_UVMode);
#endif

	while (i < length)
	{
		if (length - i > E2PROM_PAGESIZE)
			len = E2PROM_PAGESIZE;
		else
			len = length - i;
		OSSemPend(IICSem, 0, &err);
		if (IIC_ReadEEPRom(EPR_BYHX_DATA_OFFSET+i, ((INT8U *)&byhxData)+i, &len) == False)
		{
			OSSemPost(IICSem);
			break;
		}
		OSSemPost(IICSem);
		i += len;			
	}

	if ( i < length)
		status_ReportStatus(STATUS_FTA_EEPROM_READ, STATUS_SET);

	byhxData.boardID = owBoardID;
	byhxData.manufacturerID = owManufacturerID;

	fwInfo.boardID = byhxData.boardID;
	//fwInfo.manufacturerID = byhxData.manufacturerID;

#ifdef LB_TEST	

#error
	//byhxData.manufacturerID =  fwInfo.manufacturerID;
	fwInfo.manufacturerID = byhxData.manufacturerID = 0x98;
	//factoryData.HeadType = HeadNo_Ricoh_Gen4_G4;
#endif
	//WriteBoardAndManufacturerID(byhxData.boardID, 0x98);   
	if (byhxData.manufacturerID != fwInfo.manufacturerID)
	{
		status_ReportStatus(STATUS_FTA_MANUFACTURERID_NOTMATCH, STATUS_SET);				
	}
	// fwInfo.manufacturerID = byhxData.manufacturerID;
	i = 0;
	length = sizeof(HostParaType);
	while (i < length)
	{
		if (length - i > E2PROM_PAGESIZE)
			len = E2PROM_PAGESIZE;
		else
			len = length - i;
		OSSemPend(IICSem, 0, &err);
		if (IIC_ReadEEPRom(EPR_HOST_PARA_OFFSET+i, ((INT8U *)&host_paras)+i, &len) == False)
		{
			OSSemPost(IICSem);
			break;
		}
		OSSemPost(IICSem);
		i += len;							 
	}	

	if (i < length)
	{
		epromDataRead = FALSE;
		//		CONSOL_Printf("Read EPROM Failed\r\n.");
	}

#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
	if(!ReadEPSONCaliConfig())
	{
		epromDataRead = FALSE;
		CONSOL_Printf("Read calibration configure Failed\r\n.");
	}

	if (!ReadEPSONCaliSetting())
	{
		epromDataRead = FALSE;
		epsonCaliSetting.eprCaliSetting.len = 0;
		epsonCaliSetting.eprCaliSetting.version = 0;
		CONSOL_Printf("Read extended printer parameters Failed\r\n.");
	}

	if(!ReadLCDMenuConfig())
	{
		epromDataRead = False;
		CONSOL_Printf("EPR_LCDMENU_SETTING_OFFSET read failed.");
	}
	LCDMenuConfig.Active_HeadMask &= (1<<EPSON_PERMIT_GROUPNUM) -1;
#ifdef MANUFACTURER_DYSS		
	LCDMenuConfig.SheetType = 0;
	LCDMenuConfig.Head_Height_Level = 0;
	if(LCDMenuConfig.PlateFanMode == PLATEFANMODE_OPEN)
		OpenPlateFan();
	else
		ClosePlateFan();
#endif
	if (security_CheckOptionPassword())
	{
		if(password.m_bHaveOptionPwd)
		{
			if( LCDMenuConfig.LanguageIndex != password.m_bLanguage)
				LCDMenuConfig.LanguageIndex = password.m_bLanguage;
		}
	}

	if(!ReadLCDMenuConfig_EX())
	{
		epromDataRead = False;
		CONSOL_Printf("EPR_LCDMENU_SETTING_OFFSET read failed.");
	}

	Cal_active_Group_num();
	if(LCDMenuConfig.OriginX >= LCDMenuConfig.MediaWidth)
		LCDMenuConfig.OriginX = LCDMenuConfig.MediaWidth;
#ifdef RIPSTAR_FLAT_EX    
	if(LCDMenuConfig_EX.OriginY>= LCDMenuConfig_EX.MediaL)
		LCDMenuConfig_EX.OriginY= LCDMenuConfig_EX.MediaL;
	mainUIPara_ToPM.PrintYOrigin = mainUIPara.PrintYOrigin = LCDMenuConfig_EX.OriginY;
	mainUIPara_ToPM.PrintZOrigin = mainUIPara.PrintZOrigin = LCDMenuConfig_EX.OriginZ;
	mediaInfo_ToPM.Prt_Y_Continue = mediaInfo.Prt_Y_Continue =  LCDMenuConfig_EX.PRINT_Y_CONTINUE;
	mediaInfo_ToPM.MediaLength = mediaInfo.MediaLength =  LCDMenuConfig_EX.MediaL;
#endif    
	mainUIPara_ToPM.PrintOrigin = LCDMenuConfig.OriginX;
	mediaInfo_ToPM.MediaOrigin = LCDMenuConfig.OriginX;
	mediaInfo_ToPM.MediaWidth = LCDMenuConfig.MediaWidth - LCDMenuConfig.OriginX;
	mediaInfo_ToPM.Margin = LCDMenuConfig.Margin;
	printQuality_ToPM.PrintQuality = LCDMenuConfig.PrintQuality;

	InitHeadParaChannelMap();
#endif

#if defined(EPSON_BOTTOM_BOARD_V3)||defined(EPSON_BOTTOM_BOARD_V2_1) 
	if(!ReadHeaterInfo())
	{
		epromDataRead = False;
		CONSOL_Printf("HEATER_INFO_VERSION read failed.");
	}
#endif
	printer.platSpace = PRINTER_PLAT_SPACE;
	printer.accSpace = PRINTER_ACC_SPACE;
}
#if 1
struct LCD_Menu_Configure_EX LCDMenuConfig_EX;

INT8U SaveLCDMenuConfigEX_EM(struct LCD_Menu_Configure_EX *pCleanPara, INT16S start, INT16U len)
{
	INT8U ret = True;

	ret &= IIC_WriteEEPRom_Ex(EPR_LCDMENU_SETTING_EX_OFFSET + start, 
			((INT8U *)pCleanPara)+ start, &len);
	return ret;
}

INT8U SaveLCDMenuConfig_EX()
{
	INT8U ret = True;
	INT16U len = sizeof(struct LCD_Menu_Configure_EX);    
	ret = IIC_WriteEEPRom_Ex(EPR_LCDMENU_SETTING_EX_OFFSET, (INT8U *)&LCDMenuConfig_EX, &len);
	return ret;
}
INT8U ResetMenuConfig_EX()
{
	INT8U ret;
	LCDMenuConfig_EX.len = sizeof(struct LCD_Menu_Configure_EX);
	LCDMenuConfig_EX.version = LCD_MENU_CONFIG_EX_VERSION;
	LCDMenuConfig_EX.MediaH = 0.0f;
	LCDMenuConfig_EX.MediaL = 16.5355f;
	LCDMenuConfig_EX.OriginY = 0.0f;
#ifdef FULGENCY_FUN
	LCDMenuConfig_EX.OriginZ = 0.15f/2.54f;
#else	
	LCDMenuConfig_EX.OriginZ = 0.0f;
#endif
	LCDMenuConfig_EX.PRINT_Y_CONTINUE = 0;
	LCDMenuConfig_EX.Calc_X_Speed = 1;
#ifdef MANUFACTURER_DYSS	        
	memset((void *)&LCDMenuConfig_EX.add[0], 0, sizeof(LCDMenuConfig_EX.add));
#endif
	LCDMenuConfig_EX.Color_Bar_Flash_On = False;
	LCDMenuConfig_EX.Color_Bar_POSI = 9.5/2.54;
	LCDMenuConfig_EX.Move_Paper_Speed_Percent = 25;
	LCDMenuConfig_EX.X_Plat_Correction = 0;
	LCDMenuConfig_EX.Y_Plat_Correction = 0;
	LCDMenuConfig_EX.Head_Encry_Mask = 0;
	LCDMenuConfig_EX.Head_PUMP_Mask = 0;
	LCDMenuConfig_EX.Head_PUMP1_ON = 10;
	LCDMenuConfig_EX.Head_PUMP1_OFF = 10;
	LCDMenuConfig_EX.Head_PUMP2_ON = 10;
	LCDMenuConfig_EX.Head_PUMP2_OFF = 10;	
	LCDMenuConfig_EX.Interval_Pump_On_Time = 10;
	LCDMenuConfig_EX.Interval_Pump_Off_Time = 10;
	return SaveLCDMenuConfig_EX();
}
INT8U ReadLCDMenuConfig_EX()
{
	INT8U err;
	INT16U length;
	INT16U version_bak;
	length = sizeof(struct LCD_Menu_Configure_EX);
	if(length > ERP_LCDMENU_SETTING_EX_SIZE)
		return False;
	memset(&LCDMenuConfig_EX, 0, sizeof(struct LCD_Menu_Configure_EX));
	if(!IIC_ReadEEPRom_Ex(EPR_LCDMENU_SETTING_EX_OFFSET, (INT8U * )&LCDMenuConfig_EX, &length))
	{
		return False;
	}
	if( LCDMenuConfig_EX.version != LCD_MENU_CONFIG_EX_VERSION || LCDMenuConfig_EX.len != length) //it is old structure before add the head.
	{
		version_bak = LCDMenuConfig_EX.version;
		LCDMenuConfig_EX.len = sizeof(struct LCD_Menu_Configure_EX);
		LCDMenuConfig_EX.version = LCD_MENU_CONFIG_EX_VERSION;
		if(LCD_MENU_CONFIG_EX_VERSION_1 == version_bak)
		{

			LCDMenuConfig_EX.Color_Bar_Flash_On = False;
			LCDMenuConfig_EX.Color_Bar_POSI = 9.5/2.54;
			LCDMenuConfig_EX.Move_Paper_Speed_Percent = 25;
			LCDMenuConfig_EX.X_Plat_Correction = 0;
			LCDMenuConfig_EX.Y_Plat_Correction = 0;
			LCDMenuConfig_EX.Head_Encry_Mask = 0;
			LCDMenuConfig_EX.Head_PUMP_Mask = 0;
			LCDMenuConfig_EX.Head_PUMP1_ON = 10;
			LCDMenuConfig_EX.Head_PUMP1_OFF = 10;
			LCDMenuConfig_EX.Head_PUMP2_ON = 10;
			LCDMenuConfig_EX.Head_PUMP2_OFF = 10;
			LCDMenuConfig_EX.Interval_Pump_On_Time = 10;
			LCDMenuConfig_EX.Interval_Pump_Off_Time = 10;
			LCDMenuConfig_EX.b_Sundry = 0;
			LCDMenuConfig_EX.Z_Plat_Correction = 0;
			return  SaveLCDMenuConfig_EX();
		}
		else if(LCD_MENU_CONFIG_EX_VERSION_2 == version_bak)
		{
			LCDMenuConfig_EX.X_Plat_Correction = 0;
			LCDMenuConfig_EX.Y_Plat_Correction = 0;
			LCDMenuConfig_EX.Head_Encry_Mask = 0;
			LCDMenuConfig_EX.Head_PUMP_Mask = 0;
			LCDMenuConfig_EX.Head_PUMP1_ON = 10;
			LCDMenuConfig_EX.Head_PUMP1_OFF = 10;
			LCDMenuConfig_EX.Head_PUMP2_ON = 10;
			LCDMenuConfig_EX.Head_PUMP2_OFF = 10;
			LCDMenuConfig_EX.Interval_Pump_On_Time = 10;
			LCDMenuConfig_EX.Interval_Pump_Off_Time = 10;
			LCDMenuConfig_EX.b_Sundry = 0;
			LCDMenuConfig_EX.Z_Plat_Correction = 0;
			return  SaveLCDMenuConfig_EX();
		}
		else if(LCD_MENU_CONFIG_EX_VERSION_3 == version_bak)
		{
			LCDMenuConfig_EX.Head_Encry_Mask = 0;
			LCDMenuConfig_EX.Head_PUMP_Mask = 0;
			LCDMenuConfig_EX.Head_PUMP1_ON = 10;
			LCDMenuConfig_EX.Head_PUMP1_OFF = 10;
			LCDMenuConfig_EX.Head_PUMP2_ON = 10;
			LCDMenuConfig_EX.Head_PUMP2_OFF = 10;
			LCDMenuConfig_EX.Interval_Pump_On_Time = 10;
			LCDMenuConfig_EX.Interval_Pump_Off_Time = 10;
			LCDMenuConfig_EX.b_Sundry = 0;
			LCDMenuConfig_EX.Z_Plat_Correction = 0;
			return  SaveLCDMenuConfig_EX();
		}
		else if(LCD_MENU_CONFIG_EX_VERSION_4 == version_bak)
		{
			LCDMenuConfig_EX.b_Sundry = 0;
			LCDMenuConfig_EX.Z_Plat_Correction = 0;
			return  SaveLCDMenuConfig_EX();
		}
		else if(LCD_MENU_CONFIG_EX_VERSION_5 == version_bak)
		{
			LCDMenuConfig_EX.Z_Plat_Correction = 0;
			return  SaveLCDMenuConfig_EX();
		}
		else
			return ResetMenuConfig_EX();
	}
	return True;
}
#endif	
#if defined( HEAD_EPSON_GEN5) || defined(HEAD_RICOH_G4)
INT8U ReadLCDMenuConfig()
{
	INT8U err;
	INT16U length;
	EPM_Head head;

	length = sizeof(struct LCD_Menu_Configure) + sizeof(EPM_Head);
	if(length > ERP_LCDMENU_SETTING_SIZE)
		return False;

	if(!IIC_ReadEEPRom_Head(EPR_LCDMENU_SETTING_OFFSET, &head))
	{
		return False;
	}

	memset(&LCDMenuConfig, 0, sizeof(struct LCD_Menu_Configure));
	if( head.flag == 0x40 && (head.version == 3 || head.version == 2) ) //it is old structure before add the head.
	{
		length = head.flag; //really, it is element len of old structure.
		if(length > sizeof(struct LCD_Menu_Configure))
			length = sizeof(struct LCD_Menu_Configure);
		if(!IIC_ReadEEPRom_Ex(EPR_LCDMENU_SETTING_OFFSET, (INT8U * )&LCDMenuConfig, &length))
		{
			return False;
		}
		//add new elememt initialization here.

		LCDMenuConfig.Active_HeadMask = (1<<EPSON_PERMIT_GROUPNUM) -1;
		LCDMenuConfig.AutoPowerOnClean = False;
		LCDMenuConfig.AutoGoHome = False;
		LCDMenuConfig.PlateFanMode = PLATEFANMODE_STRONG;
		LCDMenuConfig.AutoStep = False;
		LCDMenuConfig.cfg_bit = False;
		LCDMenuConfig.MeasureSensorOffset= 0.0;
		LCDMenuConfig.len = sizeof(struct LCD_Menu_Configure);
		LCDMenuConfig.version = LCD_MENU_CONFIG_VERSION;  

		return SaveLCDMenuConfig();
	}
	else if(head.flag == LCD_MENU_CONFIG_FLAG)
	{
		if(!IIC_ReadEEPRom_Struct(EPR_LCDMENU_SETTING_OFFSET + sizeof(EPM_Head), 
					(INT8U *)&LCDMenuConfig, sizeof(struct LCD_Menu_Configure), &head))
		{
			return False;
		}

		if(LCDMenuConfig.version <= 3)
		{
			LCDMenuConfig.AutoPowerOnClean = False;
			LCDMenuConfig.AutoGoHome = False;
		}
		if(LCDMenuConfig.version <= 4)
		{
			LCDMenuConfig.PlateFanMode = PLATEFANMODE_STRONG;
		}
		if(LCDMenuConfig.version <= 5)
		{
			LCDMenuConfig.SpotColorName[0] = 0;
			LCDMenuConfig.SpotColorName[1] = 1;
			LCDMenuConfig.SpotColorName[2] = 2;
			LCDMenuConfig.SpotColorName[3] = 3;
		}
		if(LCDMenuConfig.version <= 6)
		{
			LCDMenuConfig.AutoStep = False;
		}

		if(LCDMenuConfig.version <= 7)
		{
			LCDMenuConfig.MeasureSensorOffset= 0.0;
		}
		if( LCDMenuConfig.len != sizeof(struct LCD_Menu_Configure) || 
				LCDMenuConfig.version != LCD_MENU_CONFIG_VERSION)
		{
			LCDMenuConfig.len = sizeof(struct LCD_Menu_Configure);
			LCDMenuConfig.version = LCD_MENU_CONFIG_VERSION;

			return SaveLCDMenuConfig();
		}
	}
	else
	{
		return ResetMenuConfig();
	}

	return True;
}

INT8U SaveLCDMenuConfig()
{
	INT8U ret = True;
	INT16U len = sizeof(struct LCD_Menu_Configure);

	EPM_Head head = 
	{
		LCD_MENU_CONFIG_FLAG,
		LCD_MENU_CONFIG_VERSION,
		sizeof(struct LCD_Menu_Configure) + sizeof(EPM_Head),
		0
	};

	CalcCheckSum((INT8U*)&LCDMenuConfig, &head);
	ret = IIC_WriteEEPRom_Head(EPR_LCDMENU_SETTING_OFFSET, &head);

	ret &= IIC_WriteEEPRom_Ex(EPR_LCDMENU_SETTING_OFFSET + sizeof(EPM_Head), (INT8U *)&LCDMenuConfig, &len);
	return ret;
}


INT8U SetHeadParameter(void * pData, int Index, int Data)
{
	INT8U i, tmp[32];
	INT8U err;

	if(pData == (void *)-1)
	{
		if (OSFlagAccept(status_FLAG_GRP, STATUS_PRINT, OS_FLAG_WAIT_CLR_ALL,&err), err == OS_NO_ERR)
			return True;
		else
			return False;
	}

	if(Index >= MAX_EPSON_HEAD_CHANNEL || Index <0)
		return False;

	if(Data > 50)
		Data = 50;
	else if(Data < -50)
		Data =-50;
#if defined(HEAD_RICOH_G4)
	g_headParameterPercent[Index] = Data; 
#else
	g_headParameterPercent[EpsonChannelMap[Index]] = Data; 
#endif

	tmp[0] = MAX_EPSON_HEAD_CHANNEL+2;
	tmp[1] = UART_HEAD_EPSON_PARAM_PERCENT;
	for (i = 0; i < MAX_EPSON_HEAD_CHANNEL; i++)
	{
#ifdef RICOH_G4_16H
		if(Index >= 8 )
		{
			if(i + 8 >= MAX_EPSON_HEAD_CHANNEL )
				tmp[2+i] = 0;
			tmp[2+i] = g_headParameterPercent[i + 8];
		}
		else
#endif
			tmp[2+i] = g_headParameterPercent[i];
	}
#ifdef CONVERSION_BOARD
	tmp[0]++;
	tmp[tmp[0] - 1] = Index/8 +1;
#endif
	UART_SendCMD(UART_HEAD_CHANNEL, tmp);

	return True;
}

INT32S GetHeadParameter(void * pData, int Index)
{
	if(Index >= MAX_EPSON_HEAD_CHANNEL || Index <0)
		Index = 0;
#if defined(HEAD_RICOH_G4)
	return g_headParameterPercent[Index];
#else
	return g_headParameterPercent[EpsonChannelMap[Index]];
#endif
}
INT8U ReadFactoryData(EPR_FactoryDataType *pData)
{
	INT8U err;
	INT16U length;
	length = sizeof(EPR_FactoryDataType);
	if(!IIC_ReadEEPRom_Ex(EPR_FACTORY_DATA_OFFSET, (INT8U * )pData, &length))
	{
		return False;
	}
	if(pData->len != length -2)//最后2个字节PM未使用
	{
		memset(pData, 0, sizeof(EPR_FactoryDataType));
		pData->len = sizeof(EPR_FactoryDataType);
		pData->EncoderType = 0x01;
		pData->HeadType = 0x16;
		pData->paper_w_type = 0x0d;
		pData->color_num = 0x04;
		pData->group_num = 0x01;
		pData->nop1 = 0;
		pData->nope2 = 0;
		pData->m_fHeadXColorSpace = 1.0f;
		pData->m_fHeadXGroupSpace = 1.0f;
		pData->m_fHeadYSpace = 0;
		pData->m_fHeadAngle = 0;
		pData->m_nVersion = 1;
		pData->m_nCrc = 0;
		pData->spot_color1_headnum = 0;
		pData->spot_color2_headnum = 0;
		pData->m_nBitFlag = 5;
		pData->paper_w_left = 0;
		length = sizeof(EPR_FactoryDataType)-((INT32U)&(pData->reserved) - (INT32U)pData);
		memset(pData->reserved, 0, length);	
		status_ReportStatus(STATUS_FTA_FACTORY_DATA_ERROR,STATUS_SET);
	}
	return True;
}
INT8U ReadFactoryDataEx_EPSON(EPR_FactoryDataExType *pData)
{
	INT8U err;
	INT16U length;
	EPM_Head head;

	length = sizeof(struct EPR_FactoryData_Ex) + sizeof(EPM_Head);
	if(length > ERP_FACTORY_DATA_EX_SIZE)
		return False;

	if(!IIC_ReadEEPRom_Head(EPR_FACTORY_DATA_EX_OFFSET, &head))
	{
		return False;
	}

	memset(pData, 0, sizeof(struct EPR_FactoryData_Ex));
	if((head.flag & 0xFF) == 0x40 && ((head.flag >> 8) == 1 || (head.flag >> 8) == 2) ) //it is old structure before add the head.
	{
		length = (head.flag & 0xFF); //really, it is element len of old structure.
		if(length > sizeof(struct EPR_FactoryData_Ex))
			length = sizeof(struct EPR_FactoryData_Ex);
		if(!IIC_ReadEEPRom_Ex(EPR_FACTORY_DATA_EX_OFFSET, (INT8U * )pData, &length))
		{
			return False;
		}
		//add new elememt initialization here.

		if((head.flag >> 8) == 1)
		{
			pData->MaxGroupNumber = absv(factoryData.group_num);
		}
		pData->len = sizeof(struct EPR_FactoryData_Ex);  //current len is sizeof(struct EPR_FactoryData_Ex)
		pData->version = FACTORYDATA_EX_VER;  //current version is 0x02

		return SaveFactoryDataEx_EPSON(pData);
	}
	else if(head.flag == FACTORYDATA_EX_FLAG)
	{
		if(!IIC_ReadEEPRom_Struct(EPR_FACTORY_DATA_EX_OFFSET + sizeof(EPM_Head), 
					(INT8U * )pData, sizeof(struct EPR_FactoryData_Ex), &head))
		{
			return False;
		}

		if( pData->len != sizeof(struct EPR_FactoryData_Ex) || 
				pData->version != FACTORYDATA_EX_VER)
		{
			pData->len = sizeof(struct EPR_FactoryData_Ex);  //current len is sizeof(struct EPR_FactoryData_Ex)
			pData->version = FACTORYDATA_EX_VER;  //current version is 0x02

			return SaveFactoryDataEx_EPSON(pData);
		}
	}
	else
	{
		memset(&factoryDataEx, 0, sizeof(struct EPR_FactoryData_Ex));
		factoryDataEx.len = sizeof(struct EPR_FactoryData_Ex);
		factoryDataEx.version = FACTORYDATA_EX_VER;

		factoryDataEx.m_nXEncoderDPI = 720;
		factoryDataEx.m_nBitFlagEx = FACTORYDATA_EX_MASK_HEADDIR_IS_POS;
		factoryDataEx.m_nColorOrder[0] = COLORCODE_Y;
		factoryDataEx.m_nColorOrder[1] = COLORCODE_M;
		factoryDataEx.m_nColorOrder[2] = COLORCODE_C;
		factoryDataEx.m_nColorOrder[3] = COLORCODE_K;

		factoryDataEx.YInterleaveNum = 2; //Ｙ向拼插的数目。对于EPSON，双四色打印机为2.
		factoryDataEx.LayoutType = 0;   //reserved for special layout. default is 0. 0 means PrintHead is Y continue. 

		strcpy(factoryDataEx.ManufacturerName, "Unknown");
		strcpy(factoryDataEx.PrinterName, "UnknownPrinter");

		factoryDataEx.MaxGroupNumber = absv(factoryData.group_num);
		return SaveFactoryDataEx_EPSON(&factoryDataEx);
	}

	return True;
}

INT8U SaveFactoryDataEx_EPSON(EPR_FactoryDataExType *pData)
{
	INT8U ret = True;
	INT16U len = sizeof(EPR_FactoryDataExType);

	EPM_Head head = 
	{
		FACTORYDATA_EX_FLAG,
		FACTORYDATA_EX_VER,
		sizeof(EPR_FactoryDataExType) + sizeof(EPM_Head),
		0
	};

	CalcCheckSum((INT8U*)pData, &head);
	ret = IIC_WriteEEPRom_Head(EPR_FACTORY_DATA_EX_OFFSET, &head);

	ret &= IIC_WriteEEPRom_Ex(EPR_FACTORY_DATA_EX_OFFSET + sizeof(EPM_Head), (INT8U *)pData, &len);
	return ret;
}

#define CHECK_PASSWORD_NUM 2
INT8U UI_CheckPassword(void * pData, INT8U * password, INT8U len)
{
	int i = 0;
	while(i < len)
	{
		if(i < CHECK_PASSWORD_NUM)
		{
			if((factoryDataEx.ManufacturerName[i]>='A'&&factoryDataEx.ManufacturerName[i]<='Z')||(factoryDataEx.ManufacturerName[i]>='0'&&factoryDataEx.ManufacturerName[i]<='9'))	
			{
				if(password[i]!=factoryDataEx.ManufacturerName[i])
					return False;
			}
			else if(factoryDataEx.ManufacturerName[i]>='a'&&factoryDataEx.ManufacturerName[i]<='z')	
			{
				if(password[i]!=(factoryDataEx.ManufacturerName[i] - ('a' - 'A')))
					return False;
			}
			else
			{
				if(password[i]!=factoryDataEx.ManufacturerName[i])
					return False;
			}
		}
		else if(i >= CHECK_PASSWORD_NUM)
		{
			if(password[i]!=0)
				return False;
		}
		i++;
	}

	return True;
}


#endif

#undef _GLOBAL_C_

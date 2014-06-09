#include "global.h"
#include "status.h"
#include "consol.h"
/* ================================================================
des()
Description: DES algorithm,do encript or descript.
flg: 0 Ω‚√‹	1:º”√‹
inkey: 8x8 bytes
================================================================ */

void des(unsigned char *source,unsigned char * dest,unsigned char * inkey, int flg);
//0 Chinese simple
//1 Chinese Traditionl
//2 no use
//3 Endlish
//4 no use
//...
//15 no use
INT8U tLanguageCode[16][16] = {
	{0x35, 0x75, 0x90, 0x6e, 0x35, 0x75, 0x8b, 0x53, 0xbe, 0x8b, 0x6e, 0x7f, 0x63, 0x6b, 0x6e, 0x78},
	{0xfb, 0x96, 0x90, 0x6e, 0xfb, 0x96, 0xd3, 0x58, 0x2d, 0x8a, 0x6e, 0x7f, 0x63, 0x6b, 0xba, 0x78},  "",
	{0x56, 0x00, 0x6f, 0x00, 0x6c, 0x00, 0x74, 0x00, 0x61, 0x00, 0x67, 0x00, 0x65, 0x00, 0x20, 0x00},
	"","",""};

#ifndef BYHX_WTITE_BOARD_TOOL

//#define INTERNAL_TESTING

//#ifdef VENDOR_PASSWORD_TOOL
#if defined( VENDOR_PASSWORD_TOOL) && ! defined(INTERNAL_TESTING)
INT8U desKey[] = {0, 0, 0, 0, 0, 0, 0, 0};
#elif (MANUFACTURER_NAME == LIYU)
INT8U desKey[] = {0x03,	0x16,	0x49,	0x70,	0xFE,	0x96,	0x00,	0x01};
#elif (MANUFACTURER_NAME == INWEAR)
INT8U desKey[] = {0x36,	0xE5,	0x49,	0x70,	0xFE,	0x96,	0x00,	0x02};
#elif (MANUFACTURER_NAME == ALLWIN) 
INT8U desKey[] = {0x1E,	0x48,	0xC2,	0xD0, 	0xF1,	0xAF,	0x00,	0x03};
#elif (MANUFACTURER_NAME == KINCOLOR) 
INT8U desKey[] = {0x1E,	0xA8,	0xD2,	0x45, 	0xF1,	0xA7,	0x00,	0x04};
#elif (MANUFACTURER_NAME == HUMAN) 
INT8U desKey[] = {0x2C,	0x79,	0xBC,	0xCD, 	0x09,	0x4E,	0x00,	0x05};
#elif (MANUFACTURER_NAME == MYJET) 
INT8U desKey[] = {0x78,	0x5D,	0x21,	0x06, 	0x88,	0xBC,	0x00,	0x06};
#elif (MANUFACTURER_NAME == LOTUS) 
INT8U desKey[] = {0x03,	0x16,	0xAA,	0x34, 	0xBC,	0x28,	0x00,	0x07};
#elif (MANUFACTURER_NAME == COLORJET)
INT8U desKey[] = {0x89,	0x64,	0x02,	0x68, 	0x34,	0xDE,	0x00,	0x08};
#elif (MANUFACTURER_NAME == HAPOND)
INT8U desKey[] = {0x72,	0xFC,	0x58,	0x99, 	0x09,	0x12,	0x00,	0x09};
#elif (MANUFACTURER_NAME == RODINJET)
INT8U desKey[] = {0x38,	0x8F,	0x9A,	0x3C, 	0x84,	0x16,	0x00,	0x0A};
#elif (MANUFACTURER_NAME == GZ)
INT8U desKey[] = {0xAB,	0x25,	0x74,	0x82, 	0x3E,	0xB9,	0x00,	0x0B};
#elif (MANUFACTURER_NAME == BRITEJET)
INT8U desKey[] = {0x42,	0x52,	0x49,	0x54, 	0x45,	0xBF,	0x00,	0x0C};
#elif (MANUFACTURER_NAME == VOXINDIA)
INT8U desKey[] = {0x78,	0xB9,	0x88,	0x34, 	0xFE,	0x25,	0x00,	0x0E};
#elif (MANUFACTURER_NAME == INWEAR_FLAT)
INT8U desKey[] = {0x36,	0xE5,	0x49,	0x70,	0xFE,	0x96,	0x00,	0x82};
#elif (MANUFACTURER_NAME == ALLWIN_FLAT) 
INT8U desKey[] = {0x1E,	0x48,	0xC2,	0xD0, 	0xF1,	0xAF,	0x00,	0x83};
#elif (MANUFACTURER_NAME == KINCOLOR_FLAT) 
INT8U desKey[] = {0x1E,	0xA8,	0xD2,	0x45, 	0xF1,	0xA7,	0x00,	0x84};
#elif (MANUFACTURER_NAME == HUMAN_FLAT) 
INT8U desKey[] = {0x2C,	0x79,	0xBC,	0xCD, 	0x09,	0x4E,	0x00,	0x85};
#elif (MANUFACTURER_NAME == MYJET_FLAT) 
INT8U desKey[] = {0x78,	0x5D,	0x21,	0x06, 	0x88,	0xBC,	0x00,	0x86};
#elif (MANUFACTURER_NAME == HAPOND_FLAT)
INT8U desKey[] = {0x72,	0xFC,	0x58,	0x99, 	0x09,	0x12,	0x00,	0x89};
#elif (MANUFACTURER_NAME == RODINJET_FLAT)
INT8U desKey[] = {0x38,	0x8F,	0x9A,	0x3C, 	0x84,	0x16,	0x00,	0x8A};
#elif (MANUFACTURER_NAME == GZ_UV)
INT8U desKey[] = {0xAB,	0x25,	0x74,	0x82, 	0x3E,	0xB9,	0x00,	0x8B};
#elif (MANUFACTURER_NAME == VOXINDIA_UV)
INT8U desKey[] = {0x78,	0xB9,	0x88,	0x34, 	0xFE,	0x25,	0x00,	0x8E};
#elif (MANUFACTURER_NAME == WITCOLOR)
INT8U desKey[] = {0xdf, 0xfa, 	0x21,	0x37,	0x30, 	0x32, 	0x00, 	0x0F};
#elif (MANUFACTURER_NAME == WITCOLOR_UV)
INT8U desKey[] = {0xdf, 0xfa, 	0x21, 	0x37,  	0x30, 	0x32, 	0x00, 	0x8F};
#elif (MANUFACTURER_NAME == MONOTECH)
INT8U desKey[] = {0x38, 0xa5, 	0x07,	0x24,	0xbf, 	0x88, 	0x00, 	0x10};
#elif (MANUFACTURER_NAME == MONOTECH_UV)
INT8U desKey[] = {0x38, 0xa5, 	0x07,	0x24,	0xbf, 	0x88, 	0x00, 	0x90};
#elif (MANUFACTURER_NAME == ALLWIN_EPSON) 
INT8U desKey[] = {0x38,	0xCE,	0x8E,	0xA9, 	0xF3,	0x89,	0x00,	0x43};
#elif (MANUFACTURER_NAME == MICOLOR) 
INT8U desKey[] = {0x97,	0x10,	0xEC,	0x13, 	0x53,	0x4C,	0x2C,	0x90};
#elif (MANUFACTURER_NAME == TOPJET) 
INT8U desKey[] = {0x87,	0x29,	0x89,	0x9C, 	0xe9,	0x30,	0x93,	0x08};
#elif (MANUFACTURER_NAME == DESIGN)  
INT8U desKey[] = {0x38,   0xA5,	0xE9,	0xB3, 	0xFC,	0x92,	0x00,	0x17};
#elif (MANUFACTURER_NAME == JHF)  
INT8U desKey[] = {0x27,   0x43,	0x92,	0x33, 	0x82,	0x0c,	0x54,	0x50};
#elif (MANUFACTURER_NAME == RASTER)  
INT8U desKey[] = {0x19,   0x29,	0x84,	0x3C, 	0x30,	0x4c,	0x00,	0x21};
#elif (MANUFACTURER_NAME == TATE)  
INT8U desKey[] = {0x29,   0x88,	0x12,	0x30, 	0x84,	0xb2,	0x00,	0x18};
#elif (MANUFACTURER_NAME == TATE_UV)  
INT8U desKey[] = {0x30,   0x89,	0x10,	0x28, 	0x74,	0xa2,	0x00,	0x98};
#elif (MANUFACTURER_NAME == BEMAJET)  
INT8U desKey[] = {0x19,   0xB8,	0x1C,	0x32, 	0x85,	0x22,	0x00,	0x1B};   
#elif (MANUFACTURER_NAME == BEMAJET_UV)  
INT8U desKey[] = {0x20,   0x3C,	0x22,	0x89, 	0xBA,	0xCA,	0x00,	0x9B};  
#elif (MANUFACTURER_NAME == INKWIN)  
INT8U desKey[] = {0x88,   0x98,	0x2C,	0x44, 	0x12,	0x9F,	0x00,	0x1D}; 
#elif (MANUFACTURER_NAME == HUMAN_STITCH) 
INT8U desKey[] = {0x22,	0x19,	0xBC,	0x0D, 	0x19,	0x4F,	0x00,	0x15};
#elif (MANUFACTURER_NAME == ALLWIN_WATER_4H) 
INT8U desKey[] = {0x23,	0x29,	0x1C,	0xED, 	0x39,	0x4C,	0x00,	0x1E};
#elif (MANUFACTURER_NAME == ALLWIN_WATER_2H) 
INT8U desKey[] = {0x24,	0x89,	0xAA,	0x33, 	0x19,	0x5B,	0x00,	0x50};
#elif (MANUFACTURER_NAME == ALLWIN_4H) 
INT8U desKey[] = {0x45,	0x89,	0xAB,	0xCC, 	0x16,	0x48,	0x00,	0x51};
#elif (MANUFACTURER_NAME == ALLWIN_4H_UV) 
INT8U desKey[] = {0x33,	0x12,	0xCD,	0xAB, 	0x23,	0x78,	0x00,	0xD1};
#elif (MANUFACTURER_NAME == DOCAN_FLAT)
INT8U desKey[] = {0xAA, 0xBB, 	0x19,	0x87,	0x05, 	0x14, 	0x00, 	0x92};
#elif (MANUFACTURER_NAME == KINCOLOR_FLAT) 
INT8U desKey[] = {0x1E,	0xA8,	0xD2,	0x45, 	0xF1,	0xA7,	0x00,	0x84};
#elif (MANUFACTURER_NAME == INWEAR_FLAT_UV) 
INT8U desKey[] = {0x36,   0xE5,  0x49,   0x70,  0xFE,   0x96,  0x00,  0x93};
#elif (MANUFACTURER_NAME == SHANGHAI_YAODI) 
INT8U desKey[] = {0x12,   0x45,  0x49,   0x89,  0xAB,   0x06,  0x00,  0x31};
#elif (MANUFACTURER_NAME == DYSS_RICOH_FLAT_UV) 
INT8U desKey[] = {0x52,   0xAB,  0x12,   0x56,  0x3E,   0x26,  0x00,  0xB0};
#elif (MANUFACTURER_NAME == TENGFENG)  
INT8U desKey[] = {0x36,   0x12,	0xca,	0xab, 	0x45,	0xa2,	0x00,	0x32};
#elif (MANUFACTURER_NAME == LONGKE)  
INT8U desKey[] = {0x38,   0x21,	0x56,	0xcd, 	0x2c,	0x12,	0x00,	0x33};
#elif (MANUFACTURER_NAME == SAISHUN)  
INT8U desKey[] = {0x33,   0x2c,	0x16,	0xca, 	0x2b,	0x82,	0x00,	0x34};
#elif (MANUFACTURER_NAME == SAISHUN_WATER)  
INT8U desKey[] = {0x39,   0x1c,	0xb6,	0x2a, 	0xab,	0x12,	0x00,	0x35};
#elif (MANUFACTURER_NAME == RIPSTAR_TEST)  
INT8U desKey[] = {0x99,   0x88,	0x77,	0x34, 	0xef,	0x3a,	0x00,	0x36};
#elif (MANUFACTURER_NAME == RIPSTAR_UV_TEST)  
INT8U desKey[] = {0x11,   0x34,	0x85,	0x45, 	0x3f,	0x08,	0x00,	0xB6};
#elif (MANUFACTURER_NAME == LONGKE_FLAT_UV)  
INT8U desKey[] = {0x93,   0x68,	0x17,	0x39, 	0x1f,	0x3C,	0x00,	0xB3};
#elif (MANUFACTURER_NAME == ALLWIN_2H_ROLL_UV)  
INT8U desKey[] = {0x13,   0x88,	0x67,	0x33, 	0x1b,	0x45,	0x00,	0X37};
#elif (MANUFACTURER_NAME == DESIGN_UV)  
INT8U desKey[] = {0x33,   0xB5,	0x19,	0xB6, 	0xBB,	0x12,	0x00,	0x97};
#elif (MANUFACTURER_NAME == MARTIN)
INT8U desKey[] = {0x12,   0x15,	0x32,	0x16, 	0xac,	0x11,	0x00,	0x38};
#elif (MANUFACTURER_NAME == DGEN)
INT8U desKey[] = {0x22,   0x12,	0xBB,	0xCC, 	0x13,	0xB1,	0x00,	0x39};
#elif (MANUFACTURER_NAME == FULGENCY_FLAT_UV)
INT8U desKey[] = {0x12,   0x32,	0xCB,	0xC3, 	0x45,	0x11,	0x00,	0xBA};
#elif (MANUFACTURER_NAME == ZHONGYE_FLAT_UV)
INT8U desKey[] = {0x23,   0x76,	0x56,	0x90, 	0xAC,	0x1B,	0x00,	0xBB};
#elif (MANUFACTURER_NAME == XINHANGJIDIAN_STITCH)
INT8U desKey[] = {0x34,   0x72,	0x1B,	0x33, 	0x4A,	0x9B,	0x00,	0x3C};
#elif (MANUFACTURER_NAME == XUKE)
INT8U desKey[] = {0x04,   0x12,	0xCB,	0x23, 	0x67,	0x23,	0x00,	0x3D};
#elif (MANUFACTURER_NAME == ORIC)
INT8U desKey[] = {0x14,   0xBB,	0xCC,	0x13, 	0x89,	0x1B,	0x00,	0x3E};
#elif (MANUFACTURER_NAME == ORIC_ROLL_UV)
INT8U desKey[] = {0xca,   0x13,	0x11,	0xbe, 	0x1c,	0xc3,	0x00,	0xBC};
#elif (MANUFACTURER_NAME == ORIC_FLAT_UV)
INT8U desKey[] = {0x13,   0x48,	0x23,	0xc9, 	0x17,	0x22,	0x00,	0xBE};
#elif (MANUFACTURER_NAME == RUICAI)
INT8U desKey[] = {0x23,   0x38,	0x29,	0x44, 	0x68,	0x13,	0x00,	0x3F};
#elif (MANUFACTURER_NAME == LECAI_4H)
INT8U desKey[] = {0x12,   0x42,	0x11,	0xBB, 	0xCA,	0x2C,	0x00,	0x40};
#elif (MANUFACTURER_NAME == LECAI_FLAT_UV)
INT8U desKey[] = {0xCC,   0x28,	0x19,	0x11, 	0x90,	0x1B,	0x00,	0xC0};
#elif (MANUFACTURER_NAME == RASTER_4H)
INT8U desKey[] = {0x27,   0x45,	0x1b,	0x31, 	0x00,	0x12,	0x00,	0X41};
#elif (MANUFACTURER_NAME == LAILI)
INT8U desKey[] = {0x14,   0x35,	0x11,	0xB4, 	0x10,	0xC2,	0x00,	0X42};
#elif (MANUFACTURER_NAME == COLORFUL)
INT8U desKey[] = {0x44,   0x15,	0xba,	0x3c, 	0x4e,	0x27,	0x00,	0X44};
#else
Compile_Error("No manufacturer name is defined")
#endif


#ifdef VENDOR_PASSWORD_TOOL
void bin2Str(INT8U *binArr, INT8U * str)
{
	INT8U i,j;
	INT8U binValue;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 2; j++)
		{	
			binValue = (binArr[i] >> (j*4))&0xF;
			if ( ((binValue&0xF) >= 0) && ((binValue&0xF) <= 9))
				str[2*i+j] = '0' + binValue;
			else
				str[2*i+j] = 'A' + binValue - 10;
		}
	}
}

//manufacturer ID, board ID, headtype, longest time (10 minutes), longest period(5 minutes) 
void security_getPassword(INT8U *strPWD, INT8U mID, INT16U bID, INT8U hType, INT16U period, INT16U reserveValue)
{
	INT8U container[8];
	INT8U binPassword[8];
	
	container[0] = mID;
	container[1] = (bID>>8)&0xFF;
	container[2] = bID & 0xFF;
	container[3] = hType;
	container[4] = (period>>8)&0xFF;
	container[5] = period&0xFF;
	container[6] = reserveValue&0xFF;
	container[7] = (reserveValue>>8)&0xFF;
	//container[6] = 0x34;
	//container[7] = 0x12;
	
	des(container, binPassword, desKey, 1);
	bin2Str(binPassword, strPWD);	
}
#endif

INT8U str2BIN(INT8U * str, INT8U *binArr)
{
	INT8U i,j;
	char ch;
	for (i = 0; i < 8; i++)
	{
		binArr[i] = 0;
		for (j = 0; j < 2; j++)
		{	
			ch = str[2*i+j];
			if ( ( ch >= '0') && (ch <= '9'))
				binArr[i] |= (ch - '0') << (j*4);
			else if ((ch >= 'A') && (ch <= 'F'))
				binArr[i] |= (ch - 'A' + 10) << (j*4);
			else
				return False;
		}
	}
	return True;
}


/*Input: 1.	password string
2. manufacturer ID
3. Board ID
4. Des key
Output:	1. print head type, no use
2. limited time period
3. reserved value, no use
Retuen value: 0. sucess
1. Wrong password (There is one or more characters are not 0~F) 
2. Wrong password (Reserve value != 0x1234)
3. Manufacturer ID not match
4. Board ID not match
*/
INT8U security_desPassword(INT8U *strPWD, INT8U mID, INT16U bID, INT8U *hType, INT16U *period, INT16U *reserveValue)
{
	INT8U container[8];
	INT8U binPassword[8];
	INT16U pwdBoardID = 0;
	//	INT8U i;
	//	CONSOL_Printf("Password Str:\r\n");
	//	for (i = 0; i<16;i++)
	//		CONSOL_Printf("%x,", strPWD[i]);
	if (!str2BIN(strPWD, container))
		return 1;		
	des(container, binPassword, desKey, 0);
	
	//	CONSOL_Printf("Manufacturer ID = 0x%x\r\n", binPassword[0]);	
	//	if ((binPassword[6] != 0x34) || (binPassword[7] != 0x12))
	//		return 2;
	if ((binPassword[6] != ((*reserveValue)&0xFF)) || (binPassword[7] != (((*reserveValue)>>8)&0xFF)))
		return 2;
	
	if (binPassword[0] != mID)
		return 3;
	pwdBoardID = (binPassword[1]<<8)|binPassword[2];
	
	//	CONSOL_Printf("Board ID = 0x%x\r\n", pwdBoardID);
	
	if (pwdBoardID != bID)
		return 4;
	*hType = binPassword[3];
	*period = (binPassword[4]<<8)|binPassword[5];
	*reserveValue = (binPassword[6]<<8)|binPassword[7];
	
	//	CONSOL_Printf("Limited time = %d\r\n", *period);
	
	return 0;
}

INT8U security_CheckTimePassword(void)
{
	INT8U ret;
	INT8U hType;
	INT16U reserveValue;
	INT16U period;
	
	status_ReportStatus(STATUS_WAR_TIME_PASSWORD_WRONGINPUT, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_TIME_PASSWORD_WRONG, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_TIME_PASSWORD_MANUFACTURERIDNOTMATCH, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_TIME_PASSWORD_BOARDIDNOTMATCH, STATUS_CLR);
	
	status_ReportStatus(STATUS_WAR_LIMITEDTIME_FIRST, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_LIMITEDTIME_SECOND, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_LIMITEDTIME_LAST, STATUS_CLR);
	status_ReportStatus(STATUS_FTA_LIMITEDTIME_RUNOUT, STATUS_CLR);
	
	reserveValue = 0x1234;
	ret = security_desPassword(password.m_sTimePwd, byhxData.manufacturerID, byhxData.boardID, &hType, &period, &reserveValue);
	if ( ret == 0)
	{
		password.m_bHaveTimePwd = True;
		
		//		CONSOL_Printf("period value = %d\r\n", period);
		password.m_bTimeLimited = period;
		
		//		CONSOL_Printf("Password limited time = %d\r\n", password.m_bTimeLimited);
		return True;
	}else
	{
		status_ReportStatus(STATUS_WAR_TIME_PASSWORD_WRONGINPUT+ret-1, STATUS_SET);
		return False;
	}
}

INT8U security_CheckInkPassword(void)
{
	INT8U ret;
	INT8U hType;
	INT16U reserveValue;
	INT16U period;
	
	status_ReportStatus(STATUS_WAR_INK_PASSWORD_WRONGINPUT, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_INK_PASSWORD_WRONG, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_INK_PASSWORD_MANUFACTURERIDNOTMATCH, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_INK_PASSWORD_BOARDIDNOTMATCH, STATUS_CLR);
	
	status_ReportStatus(STATUS_WAR_LIMITEDINK_FIRST, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_LIMITEDINK_SECOND, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_LIMITEDINK_LAST, STATUS_CLR);
	status_ReportStatus(STATUS_FTA_LIMITEDINK_RUNOUT, STATUS_CLR);
	
	reserveValue = 0x9ABC;
	ret = security_desPassword(password.m_sInkPwd, byhxData.manufacturerID, byhxData.boardID, &hType, &period, &reserveValue);
	if ( ret == 0)
	{
		password.m_bHaveInkPwd = True;
		
		//		CONSOL_Printf("period value = %d\r\n", period);
		password.m_bInkLimited = period;
		
		//		CONSOL_Printf("Password limited ink = %d\r\n", password.m_bInkLimited);
		return True;
	}else
	{
		status_ReportStatus(STATUS_WAR_INK_PASSWORD_WRONGINPUT+ret-1, STATUS_SET);
		return False;
	}
}

INT8U security_CheckOptionPassword(void)
{
	INT8U ret;
	INT8U hType;
	INT16U reserveValue;
	INT16U period;
	
	status_ReportStatus(STATUS_WAR_OPTION_PASSWORD_WRONGINPUT, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_OPTION_PASSWORD_WRONG, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_OPTION_PASSWORD_MANUFACTURERIDNOTMATCH, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_OPTION_PASSWORD_BOARDIDNOTMATCH, STATUS_CLR);
	
	reserveValue = 0x5678;
	ret = security_desPassword(password.m_sOptionPwd, byhxData.manufacturerID, byhxData.boardID, &hType, &period, &reserveValue);
	if ( ret == 0)
	{
		password.m_bHaveOptionPwd = True;
		
		//		CONSOL_Printf("period value = %d\r\n", period);
		password.m_bLanguage = period;
		
		//		CONSOL_Printf("Password limited time = %d\r\n", password.m_bTimeLimited);
		return True;
	}else
	{
#ifdef ENGLISH_LANGUAGE_BASE		
		password.m_bHaveOptionPwd = True;
		LCDMenuConfig.LanguageIndex = password.m_bLanguage = 3;
#endif    
		status_ReportStatus(STATUS_WAR_OPTION_PASSWORD_WRONGINPUT+ret-1, STATUS_SET);
		return False;
	}
}

INT8U security_CheckSWUILanguageSetting(INT8U language, INT8U *str, INT8U isEP0)
{
#ifdef ACTIVE_LANGUAGE_LIMIT
	INT8U i;
	INT8U pwLanguage = password.m_bLanguage;
	INT32U pwLanguageStr[4];
	if (!password.m_bHaveOptionPwd)
		pwLanguage = 0;
	if (pwLanguage != language)
		return False;
	if (language > 15)
		return False;
	if (tLanguageCode[language] == NULL)
		return False;
	memcpy(pwLanguageStr, tLanguageCode[language], 16);
	
	//	CONSOL_Printf("%x, ",g_uiRandomNumber[0]);
	//	CONSOL_Printf("%x, ",g_uiRandomNumber[1]);
	//	CONSOL_Printf("%x, ",g_uiRandomNumber[2]);
	//	CONSOL_Printf("%x, ",g_uiRandomNumber[3]);
	
	//	CONSOL_Printf("\r\nCompare string...\r\n");
	//	CONSOL_Printf("%x, ",pwLanguageStr[0]);
	//	CONSOL_Printf("%x, ",pwLanguageStr[1]);
	//	CONSOL_Printf("%x, ",pwLanguageStr[2]);
	//	CONSOL_Printf("%x, ",pwLanguageStr[3]);
	
	if (isEP0)
	{
		pwLanguageStr[0] ^= g_uiRandomNumber[0];
		pwLanguageStr[1] ^= g_uiRandomNumber[1];
		pwLanguageStr[2] ^= g_uiRandomNumber[2];
		pwLanguageStr[3] ^= g_uiRandomNumber[3];
	}else
	{
		pwLanguageStr[0] ^= g_uiRandomNumber[2];
		pwLanguageStr[1] ^= g_uiRandomNumber[0];
		pwLanguageStr[2] ^= g_uiRandomNumber[3];
		pwLanguageStr[3] ^= g_uiRandomNumber[1];
	}	
	
	//	CONSOL_Printf("\r\nAfter XOR:\r\n");
	//	CONSOL_Printf("%x, ",pwLanguageStr[0]);
	//	CONSOL_Printf("%x, ",pwLanguageStr[1]);
	//	CONSOL_Printf("%x, ",pwLanguageStr[2]);
	//	CONSOL_Printf("%x, ",pwLanguageStr[3]);
	
	for (i = 0; i < 4; i++)
	{
		if (*((__packed INT32U *)&str[i*4]) != pwLanguageStr[i])
			break; 
	}
	
	if (i < 4)
		return False;
	else
		return True;
#else
	return True;
#endif		
}
#endif

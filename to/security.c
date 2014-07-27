#include "global.h"
#include "status.h"
#include "consol.h"
/* ================================================================
des()
Description: DES algorithm,do encript or descript.
flg: 0 Ω‚√‹	1:º”√‹
inkey: 8x8 bytes
================================================================ */

void des(unsigned char *, unsigned char *, unsigned char *, int);

INT8U tLanguageCode[16][16] = {
	{
		0x35, 0x75, 0x90, 0x6e, 0x35, 0x75, 0x8b, 0x53,
		0xbe, 0x8b, 0x6e, 0x7f, 0x63, 0x6b, 0x6e, 0x78
	},
	{
		0xfb, 0x96, 0x90, 0x6e, 0xfb, 0x96, 0xd3, 0x58,
		0x2d, 0x8a, 0x6e, 0x7f, 0x63, 0x6b, 0xba, 0x78
	},
       	"",
	{
		0x56, 0x00, 0x6f, 0x00, 0x6c, 0x00, 0x74, 0x00,
		0x61, 0x00, 0x67, 0x00, 0x65, 0x00, 0x20, 0x00
	},
	"",
	"",
	""
};

#ifndef BYHX_WTITE_BOARD_TOOL

INT8U desKey[] = { 0x52, 0xAB, 0x12, 0x56, 0x3E, 0x26, 0x00, 0xB0 };
//INT8U desKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

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

void security_getPassword(INT8U *strPWD, INT8U mID, INT16U bID,
		INT8U hType, INT16U period, INT16U reserveValue)
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
INT8U security_desPassword(INT8U *strPWD, INT8U mID, INT16U bID,
		INT8U *hType, INT16U *period, INT16U *reserveValue)
{
	INT8U container[8];
	INT8U binPassword[8];
	INT16U pwdBoardID = 0;
	if (!str2BIN(strPWD, container))
		return 1;
	des(container, binPassword, desKey, 0);

	if ((binPassword[6] != ((*reserveValue)&0xFF))
			|| (binPassword[7] != (((*reserveValue)>>8)&0xFF)))
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
	status_ReportStatus(STATUS_WAR_TIME_PASSWORD_MANUFACTURERIDNOTMATCH,
			STATUS_CLR);
	status_ReportStatus(STATUS_WAR_TIME_PASSWORD_BOARDIDNOTMATCH,
			STATUS_CLR);

	status_ReportStatus(STATUS_WAR_LIMITEDTIME_FIRST, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_LIMITEDTIME_SECOND, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_LIMITEDTIME_LAST, STATUS_CLR);
	status_ReportStatus(STATUS_FTA_LIMITEDTIME_RUNOUT, STATUS_CLR);

	reserveValue = 0x1234;
	ret = security_desPassword(password.m_sTimePwd, byhxData.manufacturerID,
		       byhxData.boardID, &hType, &period, &reserveValue);
	if ( ret == 0)
	{
		password.m_bHaveTimePwd = True;
		password.m_bTimeLimited = period;
		return True;
	}else
	{
		status_ReportStatus(STATUS_WAR_TIME_PASSWORD_WRONGINPUT+ret-1,
				STATUS_SET);
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
	status_ReportStatus(STATUS_WAR_INK_PASSWORD_MANUFACTURERIDNOTMATCH,
			STATUS_CLR);
	status_ReportStatus(STATUS_WAR_INK_PASSWORD_BOARDIDNOTMATCH, STATUS_CLR);

	status_ReportStatus(STATUS_WAR_LIMITEDINK_FIRST, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_LIMITEDINK_SECOND, STATUS_CLR);
	status_ReportStatus(STATUS_WAR_LIMITEDINK_LAST, STATUS_CLR);
	status_ReportStatus(STATUS_FTA_LIMITEDINK_RUNOUT, STATUS_CLR);

	reserveValue = 0x9ABC;
	ret = security_desPassword(password.m_sInkPwd, byhxData.manufacturerID,
			byhxData.boardID, &hType, &period, &reserveValue);
	if ( ret == 0)
	{
		password.m_bHaveInkPwd = True;
		password.m_bInkLimited = period;
		return True;
	}else
	{
		status_ReportStatus(STATUS_WAR_INK_PASSWORD_WRONGINPUT+ret-1,
				STATUS_SET);
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
	status_ReportStatus(STATUS_WAR_OPTION_PASSWORD_MANUFACTURERIDNOTMATCH,
			STATUS_CLR);
	status_ReportStatus(STATUS_WAR_OPTION_PASSWORD_BOARDIDNOTMATCH,
			STATUS_CLR);

	reserveValue = 0x5678;
	ret = security_desPassword(password.m_sOptionPwd,
			byhxData.manufacturerID, byhxData.boardID,
			&hType, &period, &reserveValue);
	if ( ret == 0)
	{
		password.m_bHaveOptionPwd = True;
		password.m_bLanguage = period;
		return True;
	}else
	{
		status_ReportStatus(STATUS_WAR_OPTION_PASSWORD_WRONGINPUT+ret-1,
				STATUS_SET);
		return False;
	}
}

INT8U security_CheckSWUILanguageSetting(INT8U language, INT8U *str, INT8U isEP0)
{
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

	for (i = 0; i < 4; i++)
	{
		if (*((__packed INT32U *)&str[i*4]) != pwLanguageStr[i])
			break;
	}

	if (i < 4)
		return False;
	else
		return True;
}
#endif

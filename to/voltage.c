#include "global.h"
#include "miscellance.h"
#include "inktable.h"
#include "status.h"

#define COLOR_NUMBER 6


const float (* voltageRatio)[30] = voltageRatio_14P_55MPS;

//KM256_12 Voltage
//HeadBoard 左	0	1	2	3	4	5	6	7	8	9	10	11 右
//Host      	11	7	3	10	6	2	9	5	1	8	4	0	//4 color  
//		11	5	10	4	9	3	8	2	7	1	6	0   //6 color

INT8U map_KM256_12[2][16] = { 
	{11,	7,	3,	10,	6,	2,	9,	5,	1,	8,	4,	0,	12,	13,	14,	15},	//4 color  
	{11,	5,	10,	4,	9,	3,	8,	2,	7,	1,	6,	0,	12,	13,	14,	15}   //6 color
};

//KM256_8 map实际有8路控制，但是虚拟为12路，其中，9，6，3，0为虚拟路
//HeadBoard  0		1	2	3	4	5	6	7	8	9	10	11 
//Host       x		7	3	x	6	2	x	5	1	x	4	0	//4 color  
//	 x		7N	6N	x	5	4	x	3	2	x	1	0   //6 color

INT8U map_KM256_8[2][16] = { 
	{11,	7,	3,	10,	6,	2,	9,	5,	1,	8,	4,	0,	12,	13,	14,	15},	//4 color  
	{11,	7,	6,	10,	5,	4,	9,	3,	2,	8,	1,	0,	12,	13,	14,	15}   //6 color
};

INT8U map_KM256_16[2][16] = { 
	{15, 11,  7,  3,  14, 10,  6,  2, 13, 9, 5,	1,	12, 8,	4,	0},	//4 color  
	{15,  9, 14,  8,  13,  7, 12,  6, 11, 5, 10, 4,  0, 1,  2,  3}  //6 color
};


#ifdef HEAD_XAAR382
INT8U map_XAAR382_8[2][16] = { 
	{0,	4,	1,	5,	2,	6,	3,	7,	8,	9,	10,	11,	12,	13,	14,	15},	//4 color  
	{0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15}   //6 color
};
#endif		


//KM512_8_8 and KM512_8_16 各有8个温度, 但是主板从头板会收到16个温度, 0=1, 2=3, ... 14=15

//KM512_8_8map: 电压8路，头板只使用头8个电压值
//HeadBoard  0		1	2	3	4	5	6	7	
//Head		 0LR	1LR	2LR	3LR	4LR	5LR	6LR	7LR										
//Host       7	3	6	2	5	1	4	0	//4 color  
//	 7	6	5	4	3	2	1	0   //6 color
//Host       5w	4w	x	x	3	2	1	0	//4 color  with White color
//	 7w	6w	5	4	3	2	1	0   //6 color  with White color


//下标：头板编号
//数值：Host编号，同主板编号
INT8U map_KM512_8_8[][16] = { 
	{7,	3,	6,	2,	5,	1,	4,	0,	15,	14,	13,	12,	11,	10,	9,	8},   //4 color  
	{7,	6,	5,	4,	3,	2,	1,	0,	15,	14,	13,	12,	11,	10,	9,	8},   //6 color
	{5,	4,	7,	6,	3,	2,	1,	0,	15,	14,	13,	12,	11,	10,	9,	8},   //4 color UV with white color, 6, 7 no use
	{7,	6,	5,	4,	3,	2,	1,	0,	15,	14,	13,	12,	11,	10,	9,	8},    //6 color UV with white color
	{6,	2,	7,	3,	5,	1,	4,	0,	15,	14,	13,	12,	11,	10,	9,	8},   //4 color, 1head 2color, logic group number is 2 or 4. Symmetry color order. 
	{7,	6,	3,	4,	5,	2,	1,	0,	15,	14,	13,	12,	11,	10,	9,	8},   //6 color, 1head 2color, logic group number is 2. Symmetry color order.
};

//KM512_8_16map: 电压16路，
//HeadBoard  0		1	2	3	4	5	6	7	8	9	10	11	12	13	14	15 
//Head		 0L		0R	1L	1R	2L	2R	3L	3R	4L	4R	5L	5R	6L	6R	7L	7R										
//Host       15		14	7	6	13	12	5	4	11	10	3	2	9	8	1	0	//4 color  
//	 15		14	13	12	11	10	9	8	7	6	5	4	3	2	1	0   //6 color
//	 11w	10w	9w	8w	x	x	x	x	7	6	5	4	3	2	1	0   //4 color with white color
//	 15w	14w	13w	12w	11	10	9	8	7	6	5	4	3	2	1	0   //6 color with white color

INT8U map_KM512_8_16[][16] = { 
	{15,	14,	7,	6,	13,	12,	5,	4,	11,	10,	3,	2,	9,	8,	1,	0},   //4 color  
	{15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	0},   //6 color
	{11,	10,	9,	8,	15,	14,	13,	12,	7,	6,	5,	4,	3,	2,	1,	0},   //4 color UV with white color
	{15,	14,	13,	12,	11,	10,	9,	8,	7,	6,	5,	4,	3,	2,	1,	0},   //6 color UV with white color
	{12,	13,	4,	5,	14,	15,	6,	7,	11,	10,	3,	2,	9,	8,	1,	0},   //4 color, 1head 2color, logic group number is 2 or 4. Symmetry color order. 
	{15,	14,	13,	12,	6,	7,	8,	9,	10,	11,	5,	4,	3,	2,	1,	0},   //6 color, 1head 2color, logic group number is 2. Symmetry color order.
};


INT8U map_KM512_16H_Temp[2][16] = { 
	{3,	7,	11,	15,	2,	6,	10,	14,	1,	5,	9,	13,	0,	4,	8,	12},	//4 color  
	{3,	9,	11,	15,	2,	8,	5,	14,	1,	7,	10,	13,	0,	6,	4,	12}     //6 color  //12~15 no use
};

//KM512_16H: 电压32路，

INT8U map_KM512_16H_Vol[2][32] = { 
	{7, 6,15,14,23,22,31,30, 5, 4,13,12,21,20,29,28, 3, 2,11,10,19,18,27,26, 1, 0, 9, 8,17,16,25,24},	//4 color  
	{7, 6,19,18,23,22,31,30, 5, 4,17,16,11,10,29,28, 3,	2,15,14,21,20,27,26, 1, 0,13,12, 9, 8,25,24}     //6 color  //24~31 no use
};

//下标：头板编号
//数值：Host编号，同主板编号
//GZ spectra只支持脉宽设定，最多6个
INT8U map_Spectra_BYHX_V4_Vol[4][16] = { 
	{0,	4,	1,	5,	2,	6,	3,	7,	15,	14,	13,	12,	11,	10,	9,	8},   //4 color  
	{0,	1,	2,	3,	4,	5,	6,	7,	15,	14,	13,	12,	11,	10,	9,	8},   //6 color
	{0,	1,	2,	3,	6,	7,	4,	5,	15,	14,	13,	12,	11,	10,	9,	8},   //4 color UV with white color, 6, 7 no use
	{0,	1,	2,	3,	4,	5,	6,	7,	15,	14,	13,	12,	11,	10,	9,	8}    //6 color UV with white color
};
//for polaris. each polaris head have 4 temperature, 0,3 is for heater temperature, 1,2 is for head internal temperature  		
//0 3 4 7 8 11 12 15 is for heater, 编号为头板编号
//1 2 5 6 9 10 13 14 is for normal ther, 编号为头板编号 
INT8U map_Spectra_BYHX_V5_Temp[4][16] = {
	{8,	0,	4,	9,	10,	1,	5,	11,	12,	2,	6,	13,	14,	3,	7,	15},   //4 color  
	{8,	0,	1,	9,	10,	2,	3,	11,	12,	4,	5,	13,	14,	6,	7,	15},   //6 color
	{8,	0,	1,	9,	10,	2,	3,	11,	12,	6,	7,	13,	14,	4,	5,	15},   //4 color UV with white color, 6, 7 no use, 这个设定只对NOVA有效
	{8,	0,	1,	9,	10,	2,	3,	11,	12,	4,	5,	13,	14,	6,	7,	15}    //6 color UV with white color
};
//for galaxy. each galaxy head have 2 temperature, 0 is for head internal temperature, 1 is not used.  		
INT8U map_Spectra_Galaxy_BYHX_V5_Temp[4][16] = {
	{0,	8,	4,	9,	1,	10,	5,	11,	2,	12,	6,	13,	3,	14,	7,	15},   //4 color , for current HW version, 8~15 is not use.
	{0,	8,	1,	9,	2,	10,	3,	11,	4,	12,	5,	13,	6,	14,	7,	15},   //6 color , for current HW version, 8~15 is not use. 6~7 is not connected.
	{8,	0,	1,	9,	10,	2,	3,	11,	12,	6,	7,	13,	14,	4,	5,	15},   //(Not verify, only copy from polaris)4 color UV with white color, 6, 7 no use, 这个设定只对NOVA有效
	{8,	0,	1,	9,	10,	2,	3,	11,	12,	4,	5,	13,	14,	6,	7,	15}    //(Not verify, only copy from polaris)6 color UV with white color
};

INT8U map_Spectra_BYHX_V5_Temp_HeaterThermistor[4][16] = {
	{0,	8,	9,	4,	1,	10,	11,	5,	2,	12,	13,	6,	3,	14,	15,	7},   //4 color  
	{0,	8,	9,	1,	2,	10,	11,	3,	4,	12,	13,	5,	6,	14,	15,	7},   //6 color
	{0,	8,	9,	1,	2,	10,	11,	3,	6,	14,	15,	7,	4,	12,	13,	7},   //4 color UV with white color, 6, 7 no use, 这个设定只对NOVA有效
	{0,	8,	9,	1,	2,	10,	11,	3,	4,	12,	13,	5,	6,	14,	15,	7}    //6 color UV with white color
};

//Cheney: 20100817, I find the sort of printhead is sequential. that is, K1 is H1,K2 is H2(notH5).
//INT8U map_Spectra_GZ_PulseWidth[4][16] = {  //For GZ 8 head polaris
//			{0,	4,	1,	5,	2,	6,	3,	7,	8,	9,	10,	11,	12,	13,	14,	15},   //4 color  
//			{0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15},   //6 color
//			{0,	4,	1,	5,	2,	6,	3,	7,	8,	9,	10,	11,	12,	13,	14,	15},   //4 color UV with white color, 没有使用
//			{0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15}    //6 color UV with white color，没有使用
//		};

//Input: head board no
//Output: host no
//voltage, temparature, heater, heater temprature, pulse width
//default is voltage, so far only Polaris has heater thermistor(BYHX_V5, V4 has not)
//For konica, voltage, temparature, heater, and temparatue == heater
//For spectra V4, voltage(8), temparature(8), heater(8), pulse width(4)
//For spectra V5, voltage(8), temparature(8) + heater temprature(8), heater(8), pulse width(8)
//For GZ spectra, pulse width(4)

//INT8U vol_MapHeadNumber(INT8U hd_HeadNum, INT8U isTamparature)  
INT8U vol_MapHeadNumber(INT8U hd_HeadNum, INT8U isTamparature, INT8U isHeater, INT8U isHeaterThermistorTemp, INT8U isPulseWidth)
{
	INT8U k = 0;
	
#ifdef USE_SPOT_COLOR
	if ((factoryData.spot_color1_headnum != 0) || (factoryData.spot_color2_headnum != 0))//每组几个白色
	{
		//WARNING: NOT consider the case of 1head 2color.
		if (factoryData.color_num == 6) //6 color
			k = 3;
		else if (factoryData.color_num == 4)
			k = 2;
		else
			status_ReportStatus(STATUS_FTA_FACTORY_COLORNUM_WRONG, STATUS_SET);
	}else
#endif	
	{
#ifdef TWOCOLOR_ONEHEAD
		if (!(IS_2COLOR_1HEAD && headboardInfo.headBoardType == HEAD_BOARD_TYPE_KM512_8HEAD_16VOL) )
		{
#endif
			if (factoryData.color_num == 6) //6 color
				k = 1;
			else if (factoryData.color_num == 4)
				k = 0;
			else if (factoryData.color_num == 1)
				k = 0;
			else
				status_ReportStatus(STATUS_FTA_FACTORY_COLORNUM_WRONG, STATUS_SET);
#ifdef TWOCOLOR_ONEHEAD
		}
		else
		{
			if (FACTORYDATA_LOGIC_GROUP_NUM == 1) //use 4 head layout.  
				k = 0;
			else if (FACTORYDATA_LOGIC_GROUP_NUM == 2 && factoryData.color_num == 4) // use 4 head layout, but color order is different. 
				k = 4;
			else if (FACTORYDATA_LOGIC_GROUP_NUM == 2 && factoryData.color_num == 6) // use 6 head layout, but color order is different. 
				k = 5;
			else if (FACTORYDATA_LOGIC_GROUP_NUM == 4 && factoryData.color_num == 4) // use 8 head layout, but color order is different. 
				k = 4;
			else
				status_ReportStatus(STATUS_FTA_NOT_SUPPORT_2C_SHARE_1H, STATUS_SET);
		}
#endif
	}
#ifdef TWOCOLOR_ONEHEAD
	if(IS_2COLOR_1HEAD)
	{
		if ((headboardInfo.headBoardType != HEAD_BOARD_TYPE_KM512_8HEAD_16VOL)
			&&(headboardInfo.headBoardType != HEAD_BOARD_TYPE_SPECTRA_POLARIS_4)
				&&(headboardInfo.headBoardType != HEAD_BOARD_TYPE_SPECTRA_POLARIS_6))
			status_ReportStatus(STATUS_FTA_NOT_SUPPORT_2C_SHARE_1H, STATUS_SET);
	}
#endif
	
	switch(headboardInfo.headBoardType)
	{
	case HEAD_BOARD_TYPE_KM256_12HEAD:
		return map_KM256_12[k][hd_HeadNum]; 
	case HEAD_BOARD_TYPE_KM256_8HEAD:
		return map_KM256_8[k][hd_HeadNum]; 
	case HEAD_BOARD_TYPE_KM256_16HEAD:
		return map_KM256_16[k][hd_HeadNum];
	case HEAD_BOARD_TYPE_KM512_8HEAD_16VOL:
	case HEAD_BOARD_TYPE_KM512_BANNER:
	case HEAD_BOARD_TYPE_NEW512_8HEAD:
	case HEAD_BOARD_TYPE_KM1024_8HEAD:
		if (isTamparature||isHeater)
			return map_KM512_8_8[k][hd_HeadNum]; //This is not bug
		else 
			return map_KM512_8_16[k][hd_HeadNum];
#ifdef HEAD_XAAR382				
	case HEAD_BOARD_TYPE_XAAR382_8HEAD:
		return map_XAAR382_8[k][hd_HeadNum];
#endif				
	case HEAD_BOARD_TYPE_KM512_16HEAD: 
	case HEAD_BOARD_TYPE_KM512_16HEAD_V2:
		if (isTamparature||isHeater)
			return map_KM512_16H_Temp[k][hd_HeadNum];
		else
			return map_KM512_16H_Vol[k][hd_HeadNum];
	case HEAD_BOARD_TYPE_KM512_8HEAD_8VOL:
	case HEAD_BOARD_TYPE_SPECTRA_BYHX_V4:
		if (isPulseWidth)
			return hd_HeadNum; //no map
		else
			return map_Spectra_BYHX_V4_Vol[k][hd_HeadNum]; 
	case HEAD_BOARD_TYPE_SPECTRA_BYHX_V5:
	case HEAD_BOARD_TYPE_SPECTRA_BYHX_V5_8:
		if (isHeaterThermistorTemp)
			return map_Spectra_BYHX_V5_Temp_HeaterThermistor[k][hd_HeadNum];
		else if (isTamparature)
		{
			if(factoryData.HeadType == HeadNo_Spectra_NOVA || 
			   factoryData.HeadType == HeadNo_Spectra_GALAXY)
				return map_Spectra_Galaxy_BYHX_V5_Temp[k][hd_HeadNum];
			else
				return map_Spectra_BYHX_V5_Temp[k][hd_HeadNum];
		}
		else if (isHeater||isPulseWidth)
			return map_Spectra_BYHX_V4_Vol[k][hd_HeadNum];
		else
			return map_Spectra_BYHX_V4_Vol[k][hd_HeadNum];
		//		case HEAD_BOARD_TYPE_SPECTRA_POLARIS_4: //For spectra polaris, 2/4 heads, GZ
		//		case HEAD_BOARD_TYPE_SPECTRA_POLARIS_6: //For spectra polaris, 6 heads, GZ
		//		case HEAD_BOARD_TYPE_SPECTRA_POLARIS_8: //For spectra polaris, 8 heads, GZ
		//			return map_Spectra_GZ_PulseWidth[k][hd_HeadNum];
	default:
		return hd_HeadNum; //no map
	}
}

//Voltage 0~254, 0.1V, 255: use default head voltage
//Temepratue: 0~255, 0.2 degree
INT8U vol_CalHeadVoltage(INT8U headNum)
{
	INT8U temeprature;
	INT8U number;
#ifdef USE_SPOT_COLOR
	INT8U spot_color_number = 0;
#endif	
	
#ifdef USE_SPOT_COLOR
	if (factoryData.spot_color1_headnum != 0)
		spot_color_number++;
	
	if (factoryData.spot_color2_headnum != 0)
		spot_color_number++;		
#endif
	
	switch(headboardInfo.headBoardType)
	{
	case HEAD_BOARD_TYPE_KM256_12HEAD:
	case HEAD_BOARD_TYPE_KM256_8HEAD:
	case HEAD_BOARD_TYPE_KM512_8HEAD_8VOL:
	case HEAD_BOARD_TYPE_KM256_16HEAD:
		temeprature = (INT8U)(g_headTemeprature[headNum]*0.2 + 0.9);
		
#ifdef UV_PRINTER
		if (temeprature < 35)
			temeprature = 35;
		else if (temeprature > 50)
			temeprature = 50;
		temeprature -= 35;	 
#else	
		if (temeprature < 10)
			temeprature = 10;
		else if (temeprature > 39)
			temeprature = 39;
		temeprature -= 10;	 
#endif
		//Here the headnumber is the host headnumber
		return (INT8U)(g_headVoltageData.autoBaseData[headNum]*voltageRatio[headNum%factoryData.color_num][temeprature]);
	case HEAD_BOARD_TYPE_KM512_8HEAD_16VOL:
	case HEAD_BOARD_TYPE_KM512_16HEAD:
	case HEAD_BOARD_TYPE_KM512_BANNER:
	case HEAD_BOARD_TYPE_NEW512_8HEAD:
	case HEAD_BOARD_TYPE_KM512_16HEAD_V2:
	case HEAD_BOARD_TYPE_KM1024_8HEAD:
		temeprature = (INT8U)(g_headTemeprature[headNum/2]*0.2 + 0.9);
#ifdef UV_PRINTER
		if (temeprature < 35)
			temeprature = 35;
		else if (temeprature > 50)
			temeprature = 50;
		temeprature -= 35;	 
#else	
		if (temeprature < 10)
			temeprature = 10;
		else if (temeprature > 39)
			temeprature = 39;
		temeprature -= 10;	 
#endif
#ifdef TWOCOLOR_ONEHEAD
		if(IS_2COLOR_1HEAD)
			number = headNum;
		else
#endif			
			number = headNum/2;
#ifdef USE_SPOT_COLOR
		number = number%(factoryData.color_num + factoryData.spot_color1_headnum + factoryData.spot_color1_headnum);
		if (number >= factoryData.color_num) //Spot color
		{ 
			if (number < factoryData.color_num + factoryData.spot_color1_headnum)
				number = 0; //should be 6; //spot color 1
			else 
				number = 1; //should be 7; //spot color 2
		}
#else			
		number = number%factoryData.color_num;
#endif			
		
#ifdef ADJUST_VOLTAGE_TABLE
		return (INT8U)(g_headVoltageData.autoBaseData[headNum]*(voltageRatio[number][temeprature]-voltageRatio_adjust_value[temeprature]));
#else
		return (INT8U)(g_headVoltageData.autoBaseData[headNum]*voltageRatio[number][temeprature]);
#endif			
	default:
		return (INT8U)g_headVoltageData.autoBaseData[headNum]; //Error Head board type
	}
}

//  -- 0: 8.8	8.8x5   132us	KM512LN over speed
//  -- 1: 10.2	10.2x5  153us	KM512LN
//  -- 2: 5.0 	5.0x5	75us	KM512MN
//  -- 3: 7.7	33.3us	100us	KM512LNX	High Speed
//  -- 4: 7.7	7.7x5	115.5us	KM512LNX	Middle/Low Speed
//  -- 5, 6, 7 == 4

void headboardSwitchL40M14(void)
{
	INT8U UartCMD[20];
	UartCMD[1] = UART_HEAD_HEADTYPE_CMD;
	switch (factoryData.HeadType)
	{
	case HeadNo_Konica_KM_512_L:
	case HeadNo_Konica_KM_256_L:
	case HeadNo_Konica_KM_128_L:  
		UartCMD[0] = 3;
		UartCMD[2] = 1; //40PL  KMxxx_L
#if (MANUFACTURER_NAME == ALLWIN)   	
		voltageRatio = voltageRatio_42P_5MPS;
#else
		voltageRatio = voltageRatio_42P_55MPS;   			
#endif   			
		break;
	case HeadNo_Konica_KM_512_M:
	case HeadNo_Konica_KM_256_M:
	case HeadNo_Konica_KM_128_M: 
		UartCMD[0] = 3;
		UartCMD[2] = 2; //14PL KMxxx_M
		
#if (MANUFACTURER_NAME == ALLWIN)   			   			
		voltageRatio = voltageRatio_14P_5MPS;
#else
		voltageRatio = voltageRatio_14P_55MPS;
#endif  
		break;
	case HeadNo_Konica_New512:
		UartCMD[0] = 3;
		UartCMD[2] = 2; //7.7us x 21
		voltageRatio = voltageRatio_42P_5MPS; 			//@@@@@@@@@@@@@@@@@@@
		break;
	case HeadNo_Konica_KM512LNX:
		UartCMD[0] = 3;
		UartCMD[2] = 4; //7.7us x 15
		voltageRatio = voltageRatio_42P_5MPS;
		break;
		//T1 is the pulse width of half voltage
		//T2 is the pulse width of full voltage
		//T2_5 is the time of one fire cycle,
		//Normally T1 = 2*T2; T2_5 = 5 * T2;
		//Value unit is 20ns(0.02us)    
		//LEN(=10) + CMD + T1(2bytes) + T2(2bytes) + T2_5(2bytes) + Reserved(2bytes),
		//高位在前，低位在后
	case HeadNo_Konica_KM1024_L: //10us 
		UartCMD[0] = 10;
		UartCMD[2] = ((1000>>8)&0xFF);  //T1 = 10*50*2 = 1000 = 20us 
		UartCMD[3] = (1000&0xFF);
		UartCMD[4] = ((500>>8)&0xFF);   //T2 = 10*50 = 500 = 10us
		UartCMD[5] = (500&0xFF);
		UartCMD[6] = ((2500>>8)&0xFF);  //T2_5 = 10*50*5 = 2500 = 50us 
		UartCMD[7] = (2500&0xFF);
		UartCMD[8] = 0;
		UartCMD[9] = 0;
		voltageRatio = voltageRatio_42P_5MPS;
		break;
	case HeadNo_Konica_KM1024_M:
		UartCMD[0] = 10;
		UartCMD[2] = ((500>>8)&0xFF);  //T1 = 5*50*2 = 500 = 10us 
		UartCMD[3] = (500&0xFF);
		UartCMD[4] = ((250>>8)&0xFF);  //T2 = 5*50 = 250 = 5us
		UartCMD[5] = (250&0xFF);
		UartCMD[6] = ((1250>>8)&0xFF); //T2_5 = 5*50*5 = 1250 = 25us 
		UartCMD[7] = (1250&0xFF);
		UartCMD[8] = 0;
		UartCMD[9] = 0;
		voltageRatio = voltageRatio_14P_5MPS;
		break;
	default: //cheney: must return, else UART_SendCMD will fail, it cause unlimited loop.
		//for case BYHX_WTITE_BOARD_TOOL, it will run to this branch.
		return ;
	}  	
	while (!UART_SendCMD(UART_HEAD_CHANNEL, UartCMD))
		OSTimeDly(10);
}

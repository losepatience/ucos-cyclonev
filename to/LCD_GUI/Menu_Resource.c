//========================================================================
// 文件名: Menu_Resource.c
// 作  者: Xinqiang Zhang(email: Xinqiang@Mzdesign.com.cn)
//			www.Mzdesign.com.cn
// 日  期: 2007/03/24
// 描  述: 菜单应用范例程序--UI显示资源定义文件
//			有关汉字字库的资源请参考铭正同创网站上有关LCD显示中文的文章，或
//			直接参考铭正同创（Mzdesign）提供的LCD通用版基本驱动程序
//
// 参  考:
// 版  本:
//      2007/03/24      First version    Mz Design
//		2007/07/26      V1.01		     Mz Design
//
//========================================================================
//定义单条菜单项内容，格式有两种,一种为支持汉字库LCD的纯汉字菜单项,一种是西文字符与自定义
//汉字库的混合菜单项的如下：
//一:直接用汉字的字串即可,不同的编译器可能在汉字的GB码数据类型上有所不一样
//二:菜单项字符数,第一个字符在字库中的序号,第二个字符,....
//注：在第二种情况下,为了区分自定义汉字与ASCII码，特定将自定义汉字库中的汉字编码前加上128
//		作为标识
#include "resources.h"
#include "Menu_GUI.h"
#include "stdio.h"

#ifdef IIC_Key_Board_LCD
//assure LCD_MODULE_ZX19264M1A

// Main
unsigned char MenuStr_0_ID00_MenuChilds[]="Main";
// 纸张类型
unsigned char MenuStr_1_ID01_MediaTYPE[]="纸张类型";
// 透明
unsigned char MenuStr_2_ID02_Open[]="透明";
// 不透明
unsigned char MenuStr_3_ID04_Opaque[]="不透明";
// 高光
unsigned char MenuStr_3_ID06_Highlight[]="高光";
// 车贴
unsigned char MenuStr_3_ID07_Carstickers[]="车贴";
// PP
unsigned char MenuStr_3_ID08_PP[]="PP";
// 灯片
unsigned char MenuStr_3_ID09_Lighting[]="灯片";
// 其他1
unsigned char MenuStr_3_ID0A_Other1[]="其他1";
// 其他2
unsigned char MenuStr_3_ID0B_Other2[]="其他2";
// 其他3
unsigned char MenuStr_3_ID0C_Other3[]="其他3";
// 原点&纸宽
unsigned char MenuStr_1_ID0D_OriginAndMediaWidth[]="原点和纸宽";
#if defined(LCD_MODULE_128_64_ST5668A)
// 设置原点
unsigned char MenuStr_2_ID0E_SetOrigin[]="设置X原点";
// 输入原点
unsigned char MenuStr_2_ID0F_InputValue[]="X原点";
// 输入纸宽
unsigned char MenuStr_2_ID10_InputMediaWidth[]="纸宽";
#else
// 设置原点
unsigned char MenuStr_2_ID0E_SetOrigin[]="设置X原点";
// 输入原点
unsigned char MenuStr_2_ID0F_InputValue[]="输入X原点";
// 输入纸宽
unsigned char MenuStr_2_ID10_InputMediaWidth[]="输入纸宽";
#endif
#ifdef RIPSTAR_FLAT_EX
// 移动到Z原点
unsigned char MenuStr_2_ID0E_MoveZOrigin[]="移动到Z原点";
#if defined(LCD_MODULE_128_64_ST5668A)
// 输入原点
unsigned char MenuStr_2_ID0F_InputYValue[]="Y原点";
// 输入纸宽
unsigned char MenuStr_2_ID10_InputMediaLength[]="纸长";
// 输入原点
unsigned char MenuStr_2_ID0F_InputZValue[]="Z原点";
#else
// 输入原点
unsigned char MenuStr_2_ID0F_InputYValue[]="输入Y原点";
// 输入纸宽
unsigned char MenuStr_2_ID10_InputMediaLength[]="输入纸长";
// 输入原点
unsigned char MenuStr_2_ID0F_InputZValue[]="输入Z原点";
#endif
// 输入纸宽
//unsigned char MenuStr_2_ID10_InputMediaHigh[]="输入纸高";
// 校准设置
unsigned char MenuStr_2_ID0F_AdjSetting[]="校准设置";
// Y向连续打印
unsigned char MenuStr_2_ID0F_Y_PRT_CONTINUE[]="Y向连续打印";
// 水平方向校准速度选择
#if defined(LCD_MODULE_128_64_ST5668A)
	unsigned char MenuStr_2_ID0F_X_PRT_SPEED[]="水平校准速度";
#else
	unsigned char MenuStr_2_ID0F_X_PRT_SPEED[]="水平方向校准速度选择";
#endif
// 低速
unsigned char MenuStr_2_ID0F_X_PRT_LOW_SPEED[]="低速";
// 中速
unsigned char MenuStr_2_ID0F_X_PRT_MIDDLE_SPEED[]="中速";
// 高速
unsigned char MenuStr_2_ID0F_X_PRT_HIGH_SPEED[]="高速";
#endif

#if 1
// 切纸后自动进纸
unsigned char MenuStr_2_ID11_AutoFeed[]="切纸后自动进纸";
unsigned char MenuStr_2_ID11_FeedSpeed[]="手动进纸速度";
// 上纸
unsigned char MenuStr_2_ID11_Loadmedia[]="上纸";
// 自动测量
unsigned char MenuStr_2_ID12_Measurement[]="自动测量";
// 老化
unsigned char MenuStr_2_ID12_1_Measurement[]="老化";
// 切纸
unsigned char MenuStr_2_ID13_SheetCut[]="切纸";
// 余白
unsigned char MenuStr_2_ID14_Margin[]="余白";
// NewNodeText
unsigned char MenuStr_1_ID15_NewNodeTag[]="NewNodeText";
// 自动功能设置
unsigned char MenuStr_MICOLOR_EM_1_ID16_SetAutofunction[]="自动功能设置";
// 自动切纸
unsigned char MenuStr_MICOLOR_EM_2_ID17_AutoCutting[]="自动切纸";
// 自动测量纸宽
unsigned char MenuStr_MICOLOR_EM_2_ID18_Automaticmeasurement[]="自动测量纸宽";
// 自动上纸
unsigned char MenuStr_MICOLOR_EM_2_ID19_AutoLoadMedia[]="自动上纸";
// 缺纸检测
unsigned char MenuStr_MICOLOR_EM_2_ID1A_Lackpaperdetection[]="缺纸检测";
// 前面板安全保护
unsigned char MenuStr_MICOLOR_EM_2_ID1B_Thefrontpanelsecurityprotection[]="前面板安全保护";
// 自动吸风
unsigned char MenuStr_MICOLOR_EM_2_ID1C_Automaticairsuction[]="自动吸风";
// 压纸杆安全保护
unsigned char MenuStr_MICOLOR_EM_2_ID1C_Thepaperpolecurityprotection[]="压纸杆安全保护";
#if defined(LCD_MODULE_128_64_ST5668A)
// 自动吸风
unsigned char MenuStr_MICOLOR_EM_2_ID1C_AutoPowerOnClean[]="开机不自动清洗";
// 自动吸风
unsigned char MenuStr_MICOLOR_EM_2_ID1C_AutoGoHome[]="空闲自动回原点";
#else
// 自动吸风
unsigned char MenuStr_MICOLOR_EM_2_ID1C_AutoPowerOnClean[]="开机不在原点自动清洗";
// 自动吸风
unsigned char MenuStr_MICOLOR_EM_2_ID1C_AutoGoHome[]="空闲状态自动返回原点";
#endif
#endif
#if defined(LCD_MODULE_128_64_ST5668A)
// 自动Step
unsigned char MenuStr_MICOLOR_EM_2_ID1C_AutoStep[]="打印开始步进";
unsigned char MenuStr_TATE_EM_2_ID1C_AutoFlashPrint[]="打印前闪喷";
#else
// 自动Step
unsigned char MenuStr_MICOLOR_EM_2_ID1C_AutoStep[]="打印开始自动步进";
unsigned char MenuStr_TATE_EM_2_ID1C_AutoFlashPrint[]="打印前自动闪喷";
#endif
// 平台加热
unsigned char MenuStr_1_ID1D_Platformheating[]="加热";
// 加热开关
unsigned char MenuStr_2_ID1E_HeatingOnOff[]="加热开关";
// 加热温度
unsigned char MenuStr_EM_2_ID1F_Heatingtemperature[]="加热温度";
// 实际温度
unsigned char MenuStr_EM_2_ID1F_RealTemperature[]="实际温度";
// 平台吸风
unsigned char MenuStr_1_ID1D_PlatformFan[]= "平台吸风";
#ifdef MANUFACTURER_DYSS
// 吸风自动
unsigned char MenuStr_2_ID1E_SuckStrong[] = "吸风自动";
// 吸风开
unsigned char MenuStr_2_ID1F_SuckWeak[]= "吸风开";
// 吸风关
unsigned char MenuStr_2_ID1F_SuckOff[] = "吸风关";
#else
// 吸风强
unsigned char MenuStr_2_ID1E_SuckStrong[] = "吸风强";
// 吸风弱
unsigned char MenuStr_2_ID1F_SuckWeak[]= "吸风弱";
// 吸风关
unsigned char MenuStr_2_ID1F_SuckOff[] = "吸风关";
#endif
// 打印质量
unsigned char MenuStr_1_ID20_QUALITY[]="打印质量";
// 单头打印
unsigned char MenuStr_2_ID21_1HEADPRINT[]="单头打印";
#if defined(LCD_MODULE_128_64_ST5668A)
// 单头打印功能开启
unsigned char MenuStr_3_ID22_1HEADPRINTON[]="单头打印开启";
#else
// 单头打印功能开启
unsigned char MenuStr_3_ID22_1HEADPRINTON[]="单头打印功能开启";
#endif
// 选择喷头
unsigned char MenuStr_3_ID22_1HEADSELECT[]="选择喷头";
// 只开启头1
unsigned char MenuStr_3_ID22_HEAD1[]="只开启头1";
// 只开启头2
unsigned char MenuStr_3_ID22_HEAD2[]="只开启头2";
#ifdef MANUFACTURER_GZ
// 质量选择
unsigned char MenuStr_2_ID21_ECLOSIONTYPE[]="质量选择";
unsigned char MenuStr_2_ID21_ECLOSIONLEVEL[]="羽化强度";
#else
// 羽化强度与羽化类型
unsigned char MenuStr_2_ID21_ECLOSIONTYPE[]="羽化类型";
unsigned char MenuStr_2_ID21_ECLOSIONLEVEL[]="羽化强度";
#endif
// 不羽化
unsigned char MenuStr_3_ID22_ECLOSIONNONE[]="不羽化";
#ifdef MANUFACTURER_GZ
// 羽化弱
unsigned char MenuStr_3_ID23_ECLOSIONLEVEL1[]="低";
// 羽化中
unsigned char MenuStr_3_ID24_ECLOSIONLEVEL2[]="中";
// 羽化强
unsigned char MenuStr_3_ID24_ECLOSIONLEVEL3[]="高";
#else
// 羽化弱 
unsigned char MenuStr_3_ID23_ECLOSIONLEVEL1[]="羽化弱";
// 羽化中
unsigned char MenuStr_3_ID24_ECLOSIONLEVEL2[]="羽化中";
// 羽化强
unsigned char MenuStr_3_ID24_ECLOSIONLEVEL3[]="羽化强";
#endif
//羽化类型
unsigned char MenuStr_3_ID25_ECLOSIONTYPE1[] = " 渐变";
unsigned char MenuStr_3_ID25_ECLOSIONTYPE2[] = " 平均";
unsigned char MenuStr_3_ID25_ECLOSIONTYPE3[] = " 波浪";
unsigned char MenuStr_3_ID25_ECLOSIONTYPE4[] = " 渐变波浪";
unsigned char MenuStr_3_ID25_ECLOSIONTYPE5[] = " 平均渐变";
unsigned char MenuStr_3_ID25_ECLOSIONTYPE6[] = " 梭形";
// 校准
unsigned char MenuStr_UM_1_ID25_ADJUSTMENT[]="校准";
// 喷嘴检查
unsigned char MenuStr_UM_2_ID26_CheckNozzle[]="喷嘴检查";

#ifdef DYSS_UV_CLEAN
unsigned char MenuStr_UM_2_ID26_PowerOff[]="关机模式";
#endif

#ifdef EPSON_FLASH_IDLE
unsigned char MenuStr_UM_2_ID26_CAPPING[]="扣墨栈帽";
#endif

// 步进
unsigned char MenuStr_UM_2_ID27_FEEDadjust[]="步进";
// 打印
unsigned char MenuStr_UM_4_ID29_Print[]="打印";
#ifdef RIPSTAR_FLAT_EX
// 打印(高)
unsigned char MenuStr_UM_4_ID30_Print[]="打印(中速)";
#if defined(LCD_MODULE_128_64_ST5668A)
unsigned char MenuStr_UM_4_ID30_Print_EN[]="Print (Mid Spd)";
#else
unsigned char MenuStr_UM_4_ID30_Print_EN[]="Print (Middle Speed)";
#endif
#else
unsigned char MenuStr_UM_4_ID30_Print[]="打印(高)";
unsigned char MenuStr_UM_4_ID30_Print_EN[]="Print(High)";
#endif
// 校正值
unsigned char MenuStr_UM_4_ID2A_CorrectionValue[]="校正值";
#ifndef RIPSTAR_FLAT_EX
// 校正值(高)
#if defined(LCD_MODULE_128_64_ST5668A)
unsigned char MenuStr_UM_4_ID2B_CorrectionValue[]="值(高)";
unsigned char MenuStr_UM_4_CorrectionValue_HIGH[]="Value(H)";
#else
unsigned char MenuStr_UM_4_ID2B_CorrectionValue[]="校正值(高)";
unsigned char MenuStr_UM_4_CorrectionValue_HIGH[]="Value(High)";
#endif
#else
unsigned char MenuStr_UM_4_ID2B_CorrectionValue[]="校正值";
unsigned char MenuStr_UM_4_CorrectionValue_HIGH[]="Value";
#endif
#if defined(LCD_MODULE_128_64_ST5668A)
// 校正值(低)
unsigned char MenuStr_UM_4_ID2C_CorrectionValue[]="值(低)";
unsigned char MenuStr_UM_4_CorrectionValue_LOW[]="Value(L)";
#else
// 校正值(低)
unsigned char MenuStr_UM_4_ID2C_CorrectionValue[]="校正值(低)";
unsigned char MenuStr_UM_4_CorrectionValue_LOW[]="Value(Low)";
#endif
// 灯片
unsigned char MenuStr_UM_3_ID31_glossyfilm[]="灯片";
// 双向
unsigned char MenuStr_UM_2_ID3D_BiDir[]="双向";
// 头间距
unsigned char MenuStr_UM_2_ID3E_Head_Space[]="头水平间距校准";
#if defined(LCD_MODULE_128_64_ST5668A)
#ifndef EPSON_4H
unsigned char MenuStr_2_ID3E_Head1_Encry[]="J12,J14加密头";
unsigned char MenuStr_2_ID3E_Head2_Encry[]="J13,J15加密头";
#else
unsigned char MenuStr_2_ID3E_Head1_Encry[]="J9J12加密头";
unsigned char MenuStr_2_ID3E_Head2_Encry[]="J10J13加密头";

unsigned char MenuStr_2_ID3E_Head3_Encry[]="J8J15加密头";
unsigned char MenuStr_2_ID3E_Head4_Encry[]="J11J14加密头";
#endif
#else
#ifndef EPSON_4H
unsigned char MenuStr_2_ID3E_Head1_Encry[]="J12,J14是加密喷头";
unsigned char MenuStr_2_ID3E_Head2_Encry[]="J13,J15是加密喷头";
#else
unsigned char MenuStr_2_ID3E_Head1_Encry[]="J9,J12是加密喷头";
unsigned char MenuStr_2_ID3E_Head2_Encry[]="J10,J13是加密喷头";

unsigned char MenuStr_2_ID3E_Head3_Encry[]="J8,J15是加密喷头";
unsigned char MenuStr_2_ID3E_Head4_Encry[]="J11,J14是加密喷头";
#endif
#endif
unsigned char MenuStr_Color_Name[MAX_COLOR_NUM][MAX_COLOR_NAME] =
    {"K","C","M","Y","S0","S1","S2","S3"};
unsigned char MenuStr_Color_Name_E[MAX_COLOR_NUM][MAX_COLOR_NAME] =
    {"K","C","M","Y","S0","S1","S2","S3"};
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)
unsigned char MenuStr_Clean_Color_Name[4][MAX_COLOR_NAME] =
    {"k","C","M","Y"};
unsigned char MenuStr_Clean_Color_Name_E[4][MAX_COLOR_NAME] =
    {"K","C","M","Y"};
#elif defined(EPSON_4H_CLEAN_SPLIT)
unsigned char MenuStr_Clean_Color_Name[4][MAX_COLOR_NAME+2] =
    {"头 1","头 2","头 3","头 4"};
unsigned char MenuStr_Clean_Color_Name_E[4][MAX_COLOR_NAME+2] =
    {"Head 1","Head 2","Head 3","Head 4"};
#endif
unsigned char MenuStr_DPI_Name[MAX_DPI_NUM][MAX_DPI_NAME] =
    {"高速度","高精度","270DPI","540DPI","1440DPI"};
unsigned char MenuStr_DPI_Name_E[MAX_DPI_NUM][MAX_DPI_NAME] =
    {"High_Speed","High_Quality","270DPI","540DPI","1440DPI"};

unsigned char MenuStr_LeftRightAlign_Name[MAX_NOZZLE_LINE][MAX_HOR_NAME] =
    {"G1K1","G2K1","G1C1","G2C1","G1M1","G2M1","G1Y1","G2Y1",
    "G1K2","G2K2","G1C2","G2C2","G1M2","G2M2","G1Y2","G2Y2"};
unsigned char MenuStr_LeftRightAlign_Name_E[MAX_NOZZLE_LINE][MAX_HOR_NAME] =
    {"G1K1","G2K1","G1C1","G2C1","G1M1","G2M1","G1Y1","G2Y1",
    "G1K2","G2K2","G1C2","G2C2","G1M2","G2M2","G1Y2","G2Y2"};
unsigned char MenuStr_Head_Space_Name[MAX_HEAD_NUM][MAX_HEAD_SPACE_NAME] =
    {"G1H1","G1H2","G1H3","G1H4","G2H1","G2H2","G2H3","G2H4"};
unsigned char MenuStr_Head_Space_Name_E[MAX_HEAD_NUM][MAX_HEAD_SPACE_NAME] =
    {"G1H1","G1H2","G1H3","G1H4","G2H1","G2H2","G2H3","G2H4"};
unsigned char MenuStr_EM_4_ID1C8[MAX_HEAD_DRIVE_NUM][MAX_HEAD_PARAM_NAME] =
    {"k","c","m","y",};
unsigned char MenuStr_EM_4_ID1C8_E[MAX_HEAD_DRIVE_NUM][MAX_HEAD_PARAM_NAME] =
    {"k","c","m","y",};

#ifdef HEAD_RICOH_G4
unsigned char MenuStr_EM_HEAD_EX[] = "电压设置";
#endif

#define MAX_COLORNAME_LEN   15
#define MAX_COLORNAME_E_LEN   30
unsigned char MenuStr_EM_4_ID237_ColorName[MAX_SPOTCOLOR_NUM][MAX_COLORNAME_LEN]=
{
    "专色1(S1)","专色2(S2)","专色3(S3)","专色4(S4)",
    "浅青(Lc)","浅品红(Lm)","浅黄(Ly)","浅黑(Lk)",
    "浅浅黑(Llk)","粗面黑(Mk)",
    "红(R)","绿(Gr)","蓝(B)","橙(Or)",
    "白(W)","亮油(V)","白1(W1)","白2(W2)","白3(W3)","白4(W4)"
};
unsigned char MenuStr_EM_4_ID237_ColorName_E[MAX_SPOTCOLOR_NUM][MAX_COLORNAME_E_LEN]=
{
    "Spot Color 1(S1)","Spot Color 2(S2)","Spot Color 3(S3)","Spot Color 4(S4)",
    "Light Cyan(Lc)","Light Magenta(Lm)","Light Yellow(Ly)","Light Black(Lk)",
    "Light Light Black(Llk)","Matte Black(Mk)",
    "Red(R)","Green(Gr)","Blue(B)","Orange(Or)",
    "White(W)","V(V)","White1(W1)","White2(W2)","White3(W3)","White4(W4)"
};

unsigned char MenuStr_EM_ID3FB_SpotName[4][6] =
{
    "专色1","专色2","专色3","专色4"
};

// VSD1
unsigned char MenuStr_UM_3_ID3E_VSD1[]="VSD1";
// VSD2
unsigned char MenuStr_UM_3_ID41_VSD2[]="VSD2";
// VSD3
unsigned char MenuStr_UM_3_ID44_VSD3[]="VSD3";
// VSD4
unsigned char MenuStr_UM_3_ID47_VSD4[]="VSD4";
// 测试打印
unsigned char MenuStr_UM_2_ID4A_TestingPrint[]="测试打印";
// 点检查
unsigned char MenuStr_EM_2_ID51_DotCheck[]="点检查";
// 错孔检查
unsigned char MenuStr_EM_2_ID56_InterleaveCheck[]="错孔检查";
// 倾斜检查
unsigned char MenuStr_EM_2_ID57_HeadSlantCheck[]="倾斜检查";
// 头间距检查
unsigned char MenuStr_EM_2_ID58_HeadIntervalCheck[]="头垂直间距检查";
// 水平检查
unsigned char MenuStr_EM_2_ID6F_Horizotalalignment[]="水平校准";
// 360DPI
unsigned char MenuStr_EM_3_ID70_360DPI[]="360DPI";
// 左对齐
unsigned char MenuStr_EM_5_ID72_leftalignment[]="左对齐";
// H1K1
unsigned char MenuStr_EM_7_ID75_H1K1[]="H1K1";
// H1K2
unsigned char MenuStr_EM_7_ID76_H1K2[]="H1K2";
// H1C1
unsigned char MenuStr_EM_7_ID77_H1C1[]="H1C1";
// H1C2
unsigned char MenuStr_EM_7_ID78_H1C2[]="H1C2";
// H1M1
unsigned char MenuStr_EM_7_ID79_H1M1[]="H1M1";
// H1M2
unsigned char MenuStr_EM_7_ID7A_H1M2[]="H1M2";
// H1Y1
unsigned char MenuStr_EM_7_ID7B_H1Y1[]="H1Y1";
// H1Y2
unsigned char MenuStr_EM_7_ID7C_H1Y2[]="H1Y2";
// H2K1
unsigned char MenuStr_EM_7_ID7D_H2K1[]="H2K1";
// H2K2
unsigned char MenuStr_EM_7_ID7E_H2K2[]="H2K2";
// H2C1
unsigned char MenuStr_EM_7_ID7F_H2C1[]="H2C1";
// H2C2
unsigned char MenuStr_EM_7_ID80_H2C2[]="H2C2";
// H2M1
unsigned char MenuStr_EM_7_ID81_H2M1[]="H2M1";
// H2M2
unsigned char MenuStr_EM_7_ID82_H2M2[]="H2M2";
// H2Y1
unsigned char MenuStr_EM_7_ID83_H2Y1[]="H2Y1";
// H2Y2
unsigned char MenuStr_EM_7_ID84_H2Y2[]="H2Y2";
// 右对齐
unsigned char MenuStr_EM_5_ID88_rightalignment[]="右对齐";
// 720DPI
unsigned char MenuStr_EM_3_ID119_720DPI[]="720DPI";
// 270DPI
unsigned char MenuStr_EM_3_ID1C2_270DPI[]="270DPI";
// 540DPI
unsigned char MenuStr_EM_3_ID1C3_540DPI[]="540DPI";
// 1440DPI
unsigned char MenuStr_EM_3_ID1C4_1440DPI[]="1440DPI";
// 垂直校准
unsigned char MenuStr_EM_2_ID1C5_Verticalalignment[]="垂直校准";
// 垂直检查
unsigned char MenuStr_EM_2_ID1C5_Verticalcheck[]="垂直检查";
// 角度检查
unsigned char MenuStr_EM_2_ID1CC_AngleAdjustment[]="角度检查";
// H1
unsigned char MenuStr_EM_7_ID1D2_H1[]="H1";
// H2
unsigned char MenuStr_EM_7_ID1D3_H2[]="H2";
// DPI
unsigned char MenuStr_EM_3_ID22B_DPI[]="DPI";
// 720
unsigned char MenuStr_EM_4_ID22C_720[]="720";
// 360
unsigned char MenuStr_EM_4_ID22D_360[]="360";
// 540
unsigned char MenuStr_EM_4_ID22E_540[]="540";
// 270
unsigned char MenuStr_EM_4_ID22F_270[]="270";
// 1440
unsigned char MenuStr_EM_4_ID230_1440[]="1440";
// VSD
unsigned char MenuStr_EM_3_ID231_VSD[]="VSD";
// Pass数
unsigned char MenuStr_EM_3_ID236_PassNumber[]="Pass数";
// 2
unsigned char MenuStr_EM_4_ID237_2[]="2";
// 3
unsigned char MenuStr_EM_4_ID237_3[]="3";
// 4
unsigned char MenuStr_EM_4_ID237_4[]="4";
// 6
unsigned char MenuStr_EM_4_ID238_6[]="6";
// 8
unsigned char MenuStr_EM_4_ID239_8[]="8";
// 12
unsigned char MenuStr_EM_4_ID23A_12[]="12";
// 16
unsigned char MenuStr_EM_4_ID23A_16[]="16";
// 24
unsigned char MenuStr_EM_4_ID23A_24[]="24";
// 喷头清洗
unsigned char MenuStr_ALLWIN_1_ID23C_HEADCLEANING[]="喷头清洗";
// 清洗模式
unsigned char MenuStr_ALLWIN_UM_2_ID23D_Whichhead[]="清洗模式";
// 喷头1
unsigned char MenuStr_ALLWIN_UM_3_CleanHead1[10]="喷头1";
// 喷头2
unsigned char MenuStr_ALLWIN_UM_3_CleanHead2[10]="喷头2";
#if defined (DYSS_UV_CLEAN) ||defined (RICOH_3H)
unsigned char MenuStr_ALLWIN_UM_3_CleanHead3[10]="喷头3";
#endif
#if defined (DYSS_UV_CLEAN) &&!defined (RICOH_3H)
unsigned char MenuStr_ALLWIN_UM_3_CleanHead4[2]="k";
unsigned char MenuStr_ALLWIN_UM_3_CleanHead5[2]="k";
unsigned char MenuStr_ALLWIN_UM_3_CleanHead6[2]="k";
#endif
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT)||defined (DYSS_UV_CLEAN) ||defined (RICOH_3H)
// 喷头1和2
unsigned char MenuStr_ALLWIN_UM_3_CleanHeadAll[15]="所有喷头";
#else
unsigned char MenuStr_ALLWIN_UM_3_CleanHeadAll[15]="喷头1和2";
#endif
// 喷头1
unsigned char MenuStr_ALLWIN_UM_3_CleanHead1_E[10]="Head 1 ";
// 喷头2
unsigned char MenuStr_ALLWIN_UM_3_CleanHead2_E[10]="Head 2 ";
#if defined (DYSS_UV_CLEAN) ||defined (RICOH_3H)
unsigned char MenuStr_ALLWIN_UM_3_CleanHead3_E[10]="Head 3 ";
#endif
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT)||defined (DYSS_UV_CLEAN) ||defined (RICOH_3H)
// 喷头1和2
unsigned char MenuStr_ALLWIN_UM_3_CleanHeadAll_E[]="All Head";
#else
unsigned char MenuStr_ALLWIN_UM_3_CleanHeadAll_E[]="Both Head";
#endif
// 自定义清洗
unsigned char MenuStr_ALLWIN_UM_2_ID241_Custom[]="自定义清洗";
// 强
unsigned char MenuStr_ALLWIN_UM_2_ID242_Powerful[]="强";
// 中
unsigned char MenuStr_ALLWIN_UM_2_ID243_Normal[]="中";
// 弱
unsigned char MenuStr_ALLWIN_UM_2_ID244_Soft[]="弱";
// 填充
unsigned char MenuStr_ALLWIN_UM_2_ID245_Fill[]="填充";
// 手动闪喷
unsigned char MenuStr_ALLWIN_UM_2_ID245_Flash[]="手动闪喷";
// 墨栈清洗
unsigned char MenuStr_ALLWIN_UM_2_ID245_Clean_Ink_Box[]="墨栈清洗";
// 快捷方式
unsigned char MenuStr_ALLWIN_EM_2_ID246_Shortcut[]="快捷方式";
// 自定义清洗参数
unsigned char MenuStr_ALLWIN_EM_2_ID24C_CustomSet[]="自定义清洗参数";
// 清洗次数
unsigned char MenuStr_ALLWIN_EM_3_ID24D_CleaningTimes[]="清洗次数";
// 吸墨时间
unsigned char MenuStr_ALLWIN_EM_3_ID24E_sucktime[]="吸墨时间";
// 放气时间
unsigned char MenuStr_ALLWIN_EM_3_ID24F_intaketime[]="放气时间";
// 吸废墨时间
unsigned char MenuStr_ALLWIN_EM_3_ID250_Wasteinktime[]="吸废墨时间";
// 刮墨次数
unsigned char MenuStr_ALLWIN_EM_3_ID251_WipeTimes[]="刮墨次数";
// 刮墨速度
unsigned char MenuStr_ALLWIN_EM_3_ID252_WipeSpeed[]="刮墨速度";
// 闪喷频率
unsigned char MenuStr_ALLWIN_EM_3_ID253_Flashfrequency[]="闪喷频率";
// 闪喷时间
unsigned char MenuStr_ALLWIN_EM_3_ID254_FlashTime[]="闪喷时间";
// 闪喷循环时间
unsigned char MenuStr_ALLWIN_EM_3_ID255_cycletime[]="闪喷循环时间";
// 循环内空闲时间
unsigned char MenuStr_ALLWIN_EM_3_ID256_intervaltime[]="循环内空闲时间";
// 强清洗参数
unsigned char MenuStr_ALLWIN_EM_2_ID257_PowerfulSet[]="强清洗参数";
// 中清洗参数
unsigned char MenuStr_ALLWIN_EM_2_ID262_NormalSet[]="中清洗参数";
// 弱清洗参数
unsigned char MenuStr_ALLWIN_EM_2_ID26D_SoftSet[]="弱清洗参数";
// 填充清洗参数
unsigned char MenuStr_ALLWIN_EM_2_ID278_fillinkset[]="填充清洗参数";
// 自动清洗
unsigned char MenuStr_ALLWIN_EM_2_ID283_AutoClean[]="自动清洗";
// 自动清洗开关
unsigned char MenuStr_ALLWIN_EM_3_ID284_AutoCleanOnOff[]="自动清洗开关";
#ifdef MANUFACTURER_DGEN
//空闲清洗
unsigned char MenuStrIdleClean[] = "空闲清洗";
unsigned char MenuStrIdleClean_EN[] = "Idle Clean";
//空闲清洗开关
unsigned char MenuStrIdleCleanOnOff[] = "空闲清洗开关";
#if defined(LCD_MODULE_128_64_ST5668A)
unsigned char MenuStrIdleCleanOnOff_EN[] = "Idle C On/Off";
#else
unsigned char MenuStrIdleCleanOnOff_EN[] = "Idle Clean On/Off";
#endif
//空闲清洗间隔时间
unsigned char MenuStrIdleCleanIntervalTime[] = "间隔时间";
unsigned char MenuStrIdleCleanIntervalTime_EN[] = "Interval Time";
#endif
#ifdef FAST_CLEAN
//快速清洗
unsigned char MenuStrAutoFlashOnOff[] = "自动闪喷开关";
#if defined(LCD_MODULE_128_64_ST5668A)
unsigned char MenuStrAutoFlashOnOff_EN[] = "On/Off";
#else
unsigned char MenuStrAutoFlashOnOff_EN[] = "Auto Flash On/Off";
#endif
#endif

#ifdef DYSS_UV_CLEAN
// 清洗时间
unsigned char MenuStr_ALLWIN_EM_3_ID286_CleanTime1[]="自动闪喷时间";
unsigned char MenuStr_ALLWIN_EM_3_ID286_CleanTime2[]="手动闪喷时间";
extern INT8U SetCleanParam_EPSON_ALLWIN_Flash_Time(void * pData, int Index, float Data);
extern float GetCleanParam_EPSON_ALLWIN_Flash_Time(void * pData, int Index);
#endif

// 清洗方式
unsigned char MenuStr_ALLWIN_EM_3_ID286_CleanType[]="清洗方式";
// 清洗步骤一
unsigned char MenuStr_MICOLOR_EM_3_ID298_Step1[]="清洗步骤一";
// 无
unsigned char MenuStr_MICOLOR_EM_5_ID29A_None[]="无";
// 仅吸墨
unsigned char MenuStr_MICOLOR_EM_5_ID29B_OnlySuck[]="仅吸墨";
// 吸和刮
unsigned char MenuStr_MICOLOR_EM_5_ID29C_suckwipe[]="吸和刮";
// 吸刮喷
unsigned char MenuStr_MICOLOR_EM_5_ID29D_suckwipeflash[]="吸刮喷";
// 仅喷墨
unsigned char MenuStr_MICOLOR_EM_5_ID29E_onlyflash[]="仅喷墨";
// 吸废墨
unsigned char MenuStr_MICOLOR_EM_5_ID29F_onlysuckwasterink[]="吸废墨";
// 吸墨次数
unsigned char MenuStr_MICOLOR_EM_4_ID2A0_sucktimes[]="吸墨次数";
// 等待时间
unsigned char MenuStr_MICOLOR_EM_4_ID2A2_freetime[]="等待时间";
// 吸墨速度
unsigned char MenuStr_MICOLOR_EM_4_ID2A4_SuckSpeed[]="吸墨速度";
// 清洗步骤二
unsigned char MenuStr_MICOLOR_EM_3_ID2A9_Step2[]="清洗步骤二";
// 清洗步骤三
unsigned char MenuStr_MICOLOR_EM_3_ID2BA_Step3[]="清洗步骤三";
// 清洗步骤四
unsigned char MenuStr_MICOLOR_EM_3_ID2CB_Step4[]="清洗步骤四";
// 自动闪喷
unsigned char MenuStr_MICOLOR_EM_2_ID3F0_AutoFlash[]="自动闪喷";
// 手动闪喷
unsigned char MenuStr_MICOLOR_EM_2_ID3F0_Flash[]="手动闪喷";
// 自动闪喷开关
unsigned char MenuStr_MICOLOR_EM_3_ID3F1_AutoFlashOnOff[]="自动闪喷开关";
// 自动闪喷Pass数
unsigned char MenuStr_MICOLOR_EM_3_ID3F1_AutoFlashPassNum[]="闪喷间隔";
// 填充清洗保护
unsigned char MenuStr_MICOLOR_EM_3_ID3F1_CleanFillProtect[]="填充清洗保护";
// 闪喷宽度
unsigned char MenuStr_MICOLOR_EM_3_ID3F2_Flashsize[]="闪喷宽度";
// 喷头参数
unsigned char MenuStr_EM_1_ID3F3_Nozzleparameter[]="喷头参数";
// 喷头关闭
unsigned char MenuStr_EM_1_ID3F3_HeadSwitch[]="喷头关闭";
// 关闭喷头1
unsigned char MenuStr_EM_1_ID3F3_DisableHead1[]="关闭喷头1";
// 关闭喷头2
unsigned char MenuStr_EM_1_ID3F3_DisableHead2[]="关闭喷头2";
#if defined(LCD_MODULE_128_64_ST5668A)
// 关闭喷头1
unsigned char MenuStr_EM_1_ID3F3_DisableHead1_E[]="Disable H1";
// 关闭喷头2
unsigned char MenuStr_EM_1_ID3F3_DisableHead2_E[]="Disable H2";
#else
// 关闭喷头1
unsigned char MenuStr_EM_1_ID3F3_DisableHead1_E[]="Disable Head 1";
// 关闭喷头2
unsigned char MenuStr_EM_1_ID3F3_DisableHead2_E[]="Disable Head 2";
#endif
// 关闭组1
unsigned char MenuStr_EM_1_ID3F3_DisableGroup1[]="关闭组1";
// 关闭组2
unsigned char MenuStr_EM_1_ID3F3_DisableGroup2[]="关闭组2";
#if defined(LCD_MODULE_128_64_ST5668A)
// 关闭组1
unsigned char MenuStr_EM_1_ID3F3_DisableGroup1_E[]="Dis Group 1";
// 关闭组1
unsigned char MenuStr_EM_1_ID3F3_DisableGroup2_E[]="Dis Group 2";
#else
// 关闭组1
unsigned char MenuStr_EM_1_ID3F3_DisableGroup1_E[]="Disable Group 1";
// 关闭组1
unsigned char MenuStr_EM_1_ID3F3_DisableGroup2_E[]="Disable Group 2";
#endif
// 开启全部喷头
unsigned char MenuStr_EM_1_ID3F3_4DisableHead[]="开启全部喷头";
// 关闭喷头4
unsigned char MenuStr_EM_1_ID3F3_4DisableHead4[]="关闭喷头4";
// 关闭喷头1,2
unsigned char MenuStr_EM_1_ID3F3_4DisableHead12[]="关闭喷头1,2";
// 关闭喷头3,4
unsigned char MenuStr_EM_1_ID3F3_4DisableHead34[]="关闭喷头3,4";
// 只开启喷头1
unsigned char MenuStr_EM_1_ID3F3_4DisableHead234[]="只开启喷头1";
// 只开启喷头2
unsigned char MenuStr_EM_1_ID3F3_4DisableHead134[]="只开启喷头2";
// 只开启喷头3
unsigned char MenuStr_EM_1_ID3F3_4DisableHead124[]="只开启喷头3";
// 只开启喷头4
unsigned char MenuStr_EM_1_ID3F3_4DisableHead123[]="只开启喷头4";
// 开启全部喷头
unsigned char MenuStr_EM_1_ID3F3_4DisableHead_E[]="Open All";
// 关闭喷头4
unsigned char MenuStr_EM_1_ID3F3_4DisableHead4_E[]="Close Head 4";
#if defined(LCD_MODULE_128_64_ST5668A)
// 关闭喷头1,2
unsigned char MenuStr_EM_1_ID3F3_4DisableHead12_E[]="Close H1, H2";
// 关闭喷头3,4
unsigned char MenuStr_EM_1_ID3F3_4DisableHead34_E[]="Close H3, H4";
// 只开启喷头1
unsigned char MenuStr_EM_1_ID3F3_4DisableHead234_E[]="Only Open H1";
// 只开启喷头2
unsigned char MenuStr_EM_1_ID3F3_4DisableHead134_E[]="Only Open H2";
// 只开启喷头3
unsigned char MenuStr_EM_1_ID3F3_4DisableHead124_E[]="Only Open H3";
// 只开启喷头4
unsigned char MenuStr_EM_1_ID3F3_4DisableHead123_E[]="Only Open H4";
#else
// 关闭喷头1,2
unsigned char MenuStr_EM_1_ID3F3_4DisableHead12_E[]="Close Head 1,2";
// 关闭喷头3,4
unsigned char MenuStr_EM_1_ID3F3_4DisableHead34_E[]="Close Head 3,4";
// 只开启喷头1
unsigned char MenuStr_EM_1_ID3F3_4DisableHead234_E[]="Only Open Head 1";
// 只开启喷头2
unsigned char MenuStr_EM_1_ID3F3_4DisableHead134_E[]="Only Open Head 2";
// 只开启喷头3
unsigned char MenuStr_EM_1_ID3F3_4DisableHead124_E[]="Only Open Head 3";
// 只开启喷头4
unsigned char MenuStr_EM_1_ID3F3_4DisableHead123_E[]="Only Open Head 4";
#endif
// 加热前开关
unsigned char MenuStr_UM_1_ID3F4_Heater_Pre_onoff[]="前加热开关";
// 加热中开关
unsigned char MenuStr_UM_1_ID3F4_Heater_Middle_onoff[]="中加热开关";
// 加热后开关
unsigned char MenuStr_UM_1_ID3F4_Heater_Post_onoff[]="后加热开关";
// 加热前温度
unsigned char MenuStr_EM_1_ID3F4_Temperature_Pre[]="前加热目标温度";
// 加热中温度
unsigned char MenuStr_EM_1_ID3F4_Temperature_Middle[]="中加热目标温度";
// 加热后温度
unsigned char MenuStr_EM_1_ID3F4_Temperature_Post[]="后加热目标温度";
// 加热前温度
unsigned char MenuStr_EM_1_ID3F4_RealTemp_Pre[]="前传感器温度";
// 加热中温度
unsigned char MenuStr_EM_1_ID3F4_RealTemp_Middle[]="中传感器温度";
// 加热后温度
unsigned char MenuStr_EM_1_ID3F4_RealTemp_Post[]="后传感器温度";
// 环境温度
unsigned char MenuStr_EM_1_ID3F4_RealTemp_Env[]="环境传感器温度";
// 语言
unsigned char MenuStr_1_ID3F8_Language[]="语言";
// 英文
unsigned char MenuStr_2_ID3F9_English[]="英文";
// 中文
unsigned char MenuStr_2_ID3FA_Chinese[]="中文";
// 系统
unsigned char MenuStr_1_ID3FB_System[]="系统";
// 高级模式
unsigned char MenuStr_2_ID3FC_AdvancedMode[]="高级模式";
// 校准参数复位
unsigned char MenuStr_2_ID3FC_Calibration_Reset[]="校准参数复位";
// 清洗参数复位
unsigned char MenuStr_2_ID3FC_CleanConfig_Reset[]="清洗参数复位";
// 运动参数复位
unsigned char MenuStr_2_ID3FC_MotionConfig_Reset[]="运动参数复位";
// Spot Color Name
unsigned char MenuStr_2_ID3FC_SpotColorName[]="专色名定义";
unsigned char MenuStr_2_ID3FC_Head_Decryption[]="喷头类型";
#ifdef FUNC_WAVE_MAP
unsigned char MenuStr_Sys_WaveMap_CN[] = "波形映射";
unsigned char MenuStr_Sys_WaveMap_EN[] = "Wave Map";
unsigned char MenuStr_Wave_VSD_EN[][8] = {"VSD1", "VSD2", "VSD3", "VSD4"};
unsigned char MenuStr_Wave_VSD_CN[][8] = {"VSD1", "VSD2", "VSD3", "VSD4"};
unsigned char MenuStr_VSD_WaveName_EN[8][8] = {"NAME1", "NAME2", "NAME3", "NAME4", "NAME5", "NAME6", "NAME7", "NAME8",};
unsigned char MenuStr_VSD_WaveName_CN[8][8] = {"NAME1", "NAME2", "NAME3", "NAME4", "NAME5", "NAME6", "NAME7", "NAME8",};
#endif
// 状态
unsigned char MenuStr_2_ID3FE_Status[]="状态";
// 百分比
unsigned char MenuStr_2_ID3FF_Percent[]="百分比";
// 错误号
unsigned char MenuStr_2_ID400_Errorcode[]="错误号";
// 纸宽
unsigned char MenuStr_2_ID401_MediaWidth[]="纸宽";
// 原点
unsigned char MenuStr_2_ID402_Origin[]="原点";
// pass数
unsigned char MenuStr_2_ID403_PassNumber[]="Pass数";
// 分辨率
unsigned char MenuStr_2_ID404_Resolution[]="分辨率";
// 错误信息
unsigned char MenuStr_2_ID405_Errorinfo[]="错误信息";

extern struct MenuItem MI_0_ID00_MenuChilds[];
struct MenuItem MI_2_ID05_SHEETTYPE[7];
#if defined(MICOLOR_AUTOFUCTION)
struct MenuItem MI_1_ID0D_OriginAndMediaWidth[10];
struct MenuItem MI_MICOLOR_EM_1_ID16_SetAutofunction[10];
#elif defined RIPSTAR_FLAT_EX
struct MenuItem MI_1_ID0D_OriginAndMediaWidth[8];
#elif defined (LECAI_EPSON_4H_Y_ROOLENCODER)
struct MenuItem MI_1_ID0D_OriginAndMediaWidth[7];
#else
struct MenuItem MI_1_ID0D_OriginAndMediaWidth[6];
#endif
struct MenuItem MI_1_ID1D_Platformheating[2];
struct MenuItem MI_1_ID20_QUALITY[5];
struct MenuItem MI_2_ID21_1HEADPRINT[2];
struct MenuItem MI_2_ID21_ECLOSIONTYPE[6];
struct MenuItem MI_2_ID21_ECLOSIONLEVEL[4];
extern struct MenuItem MI_UM_1_ID25_ADJUSTMENT[];
#ifdef RIPSTAR_FLAT_EX
struct MenuItem MI_UM_2_ID27_FEEDadjust[2];
struct MenuItem MI_UM_2_ID27_Adjustsetting[2];
struct MenuItem MI_UM_2_ID27_XAdjustSpeed[3];
#else
struct MenuItem MI_UM_2_ID27_FEEDadjust[7];
#endif
struct MenuItem MI_UM_3_ID28_Highglossyphotopaper[2];
struct MenuItem MI_UM_3_ID2B_vinly[2];
struct MenuItem MI_UM_3_ID2E_PP[2];
struct MenuItem MI_UM_3_ID31_glossyfilm[2];
struct MenuItem MI_UM_3_ID34_Other1[2];
struct MenuItem MI_UM_3_ID37_Other2[2];
struct MenuItem MI_UM_3_ID3A_Other3[2];
struct MenuItem MI_UM_2_ID3D_BiDir[4];
struct MenuItem MI_UM_3_ID3E_VSD1[3];
struct MenuItem MI_UM_3_ID41_VSD2[3];
struct MenuItem MI_UM_3_ID44_VSD3[3];
struct MenuItem MI_UM_3_ID47_VSD4[3];
struct MenuItem MI_EM_1_ID4B_ADJUSTMENT[11];
struct MenuItem MI_EM_2_ID4C_CheckNozzle[4];
struct MenuItem MI_EM_2_ID51_DotCheck[4];
#ifdef RIPSTAR_FLAT_EX
#ifdef MANUFACTURER_DYSS	
struct MenuItem MI_EM_2_ID59_Feed[4];
#else
struct MenuItem MI_EM_2_ID59_Feed[3];
#endif
#else
struct MenuItem MI_EM_2_ID59_Feed[8];
#endif
struct MenuItem MI_EM_3_ID5A_Highglossyphotopaper[2];
struct MenuItem MI_EM_3_ID5D_Vinly[2];
struct MenuItem MI_EM_3_ID60_PP[2];
struct MenuItem MI_EM_3_ID63_Glossyfilm[2];
struct MenuItem MI_EM_3_ID66_Other1[2];
struct MenuItem MI_EM_3_ID69_Other2[2];
struct MenuItem MI_EM_3_ID6C_Other3[2];
struct MenuItem MI_EM_2_ID6F_Horizotalalignment[5];
struct MenuItem MI_EM_3_ID70_360DPI[4];
struct MenuItem MI_EM_4_ID71_VSD1[3];
struct MenuItem MI_EM_5_ID72_leftalignment[3];
struct MenuItem MI_EM_6_ID74_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_5_ID85_BiDir[3];
struct MenuItem MI_EM_5_ID88_rightalignment[3];
struct MenuItem MI_EM_6_ID8A_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_4_ID9B_VSD2[3];
struct MenuItem MI_EM_5_ID9C_leftalignment[3];
struct MenuItem MI_EM_6_ID9E_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_5_IDAF_BiDir[3];
struct MenuItem MI_EM_5_IDB2_rightalignment[3];
struct MenuItem MI_EM_6_IDB4_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_4_IDC5_VSD3[3];
struct MenuItem MI_EM_5_IDC6_leftalignment[3];
struct MenuItem MI_EM_6_IDC8_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_5_IDD9_BiDir[3];
struct MenuItem MI_EM_5_IDDC_rightalignment[3];
struct MenuItem MI_EM_6_IDDE_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_4_IDEF_VSD4[3];
struct MenuItem MI_EM_5_IDF0_leftalignment[3];
struct MenuItem MI_EM_6_IDF2_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_5_ID103_BiDir[3];
struct MenuItem MI_EM_5_ID106_rightalignment[3];
struct MenuItem MI_EM_6_ID108_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_3_ID119_720DPI[4];
struct MenuItem MI_EM_4_ID11A_VSD1[3];
struct MenuItem MI_EM_5_ID11B_leftalignment[3];
struct MenuItem MI_EM_6_ID11D_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_5_ID12E_BiDir[3];
struct MenuItem MI_EM_5_ID131_rightalignment[3];
struct MenuItem MI_EM_6_ID133_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_4_ID144_VSD2[3];
struct MenuItem MI_EM_5_ID145_leftalignment[3];
struct MenuItem MI_EM_6_ID147_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_5_ID158_BiDir[3];
struct MenuItem MI_EM_5_ID15B_rightalignment[3];
struct MenuItem MI_EM_6_ID15D_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_4_ID16E_VSD3[3];
struct MenuItem MI_EM_5_ID16F_leftalignment[3];
struct MenuItem MI_EM_6_ID171_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_5_ID182_BiDir[3];
struct MenuItem MI_EM_5_ID185_rightalignment[3];
struct MenuItem MI_EM_6_ID187_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_4_ID198_VSD4[3];
struct MenuItem MI_EM_5_ID199_leftalignment[3];
struct MenuItem MI_EM_6_ID19B_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_5_ID1AC_BiDir[3];
struct MenuItem MI_EM_5_ID1AF_rightalignment[3];
struct MenuItem MI_EM_6_ID1B1_CorrectionValue[MAX_NOZZLE_LINE];
struct MenuItem MI_EM_2_ID1C5_Verticalalignment[2];
struct MenuItem MI_EM_3_ID1C7_CorrectionValue[MAX_COLOR_NUM];
struct MenuItem MI_EM_2_ID1CC_AngleAdjustment[5];
struct MenuItem MI_EM_3_ID1CD_360DPI[4];
struct MenuItem MI_EM_4_ID1CE_VSD1[2];
struct MenuItem MI_EM_5_ID1CF_leftalignment[2];
struct MenuItem MI_EM_6_ID1D1_CorrectionValue[2];
struct MenuItem MI_EM_5_ID1D4_rightalignment[2];
struct MenuItem MI_EM_6_ID1D6_CorrectionValue[2];
struct MenuItem MI_EM_4_ID1D9_VSD2[2];
struct MenuItem MI_EM_5_ID1DA_leftalignment[2];
struct MenuItem MI_EM_6_ID1DC_CorrectionValue[2];
struct MenuItem MI_EM_5_ID1DF_rightalignment[2];
struct MenuItem MI_EM_6_ID1E1_CorrectionValue[2];
struct MenuItem MI_EM_4_ID1E4_VSD3[2];
struct MenuItem MI_EM_5_ID1E5_leftalignment[2];
struct MenuItem MI_EM_6_ID1E7_CorrectionValue[2];
struct MenuItem MI_EM_5_ID1EA_rightalignment[2];
struct MenuItem MI_EM_6_ID1EC_CorrectionValue[2];
struct MenuItem MI_EM_4_ID1EF_VSD4[2];
struct MenuItem MI_EM_5_ID1F0_leftalignment[2];
struct MenuItem MI_EM_6_ID1F2_CorrectionValue[2];
struct MenuItem MI_EM_5_ID1F5_rightalignment[2];
struct MenuItem MI_EM_6_ID1F7_CorrectionValue[2];
struct MenuItem MI_EM_3_ID1FA_720DPI[4];
struct MenuItem MI_EM_4_ID1FB_VSD1[2];
struct MenuItem MI_EM_5_ID1FC_leftalignment[2];
struct MenuItem MI_EM_6_ID1FE_CorrectionValue[2];
struct MenuItem MI_EM_5_ID201_rightalignment[2];
struct MenuItem MI_EM_6_ID203_CorrectionValue[2];
struct MenuItem MI_EM_4_ID206_VSD2[2];
struct MenuItem MI_EM_5_ID207_leftalignment[2];
struct MenuItem MI_EM_6_ID209_CorrectionValue[2];
struct MenuItem MI_EM_5_ID20C_rightalignment[2];
struct MenuItem MI_EM_6_ID20E_CorrectionValue[2];
struct MenuItem MI_EM_4_ID211_VSD3[2];
struct MenuItem MI_EM_5_ID212_leftalignment[2];
struct MenuItem MI_EM_6_ID214_CorrectionValue[2];
struct MenuItem MI_EM_5_ID217_rightalignment[2];
struct MenuItem MI_EM_6_ID219_CorrectionValue[2];
struct MenuItem MI_EM_4_ID21C_VSD4[2];
struct MenuItem MI_EM_5_ID21D_leftalignment[2];
struct MenuItem MI_EM_6_ID21F_CorrectionValue[2];
struct MenuItem MI_EM_5_ID222_rightalignment[2];
struct MenuItem MI_EM_6_ID224_CorrectionValue[2];
struct MenuItem MI_EM_2_ID22A_TestingPrint[4];
struct MenuItem MI_EM_3_ID22B_DPI[5];
struct MenuItem MI_EM_3_ID231_VSD[4];
struct MenuItem MI_EM_3_ID236_PassNumber[8];
struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD1[MAX_HEAD_NUM];
struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD2[MAX_HEAD_NUM];
struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD3[MAX_HEAD_NUM];
struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD4[MAX_HEAD_NUM];
struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD1[MAX_HEAD_NUM];
struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD2[MAX_HEAD_NUM];
struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD3[MAX_HEAD_NUM];
struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD4[MAX_HEAD_NUM];
#if defined(EPSON_CLEAN_UPDOWN)||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT)
struct MenuItem MI_ALLWIN_UM_2_ID23D_Whichhead[5];
#elif defined (DYSS_UV_CLEAN) 
struct MenuItem MI_ALLWIN_UM_2_ID23D_Whichhead[4];
#else
struct MenuItem MI_ALLWIN_UM_2_ID23D_Whichhead[3];
#endif
#ifdef MANUFACTURER_DGEN
//空闲清洗
struct MenuItem MI_ALLWIN_EM_2_ID283_IdleClean[2];
#endif
struct MenuItem MI_ALLWIN_EM_2_ID283_AutoClean[3];
#ifdef MANUFACTURER_DGEN
struct MenuItem MI_ALLWIN_1_ID23C_HEADCLEANING[10];
#else
struct MenuItem MI_ALLWIN_1_ID23C_HEADCLEANING[9];
#endif
#elif defined(EPSON_CLEAN_INTEGRATE) ||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_INTEGRATE_3)
struct MenuItem MI_ALLWIN_EM_2_ID283_AutoClean[3];
struct MenuItem MI_MICOLOR_EM_2_ID292_Shortcut[4];
#ifdef MANUFACTURER_DGEN
struct MenuItem MI_MICOLOR_1_ID28B_HEADCLEANING[11];
#else
struct MenuItem MI_MICOLOR_1_ID28B_HEADCLEANING[10];
#endif
struct MenuItem MI_MICOLOR_UM_2_ID3F0_AutoFlash[2];
#endif

#ifdef FUNC_WAVE_MAP
struct MenuItem MI_Sys_WaveMap[];
struct MenuItem MI_Wave_VSD[];
struct MenuItem MI_VSD_WaveName[];
#endif
struct MenuItem MI_1_ID3FB_SpotList[4];
struct MenuItem MI_1_ID3F8_Language[2];
struct MenuItem MI_1_ID3FB_System[];

struct MenuItem MainScreenRoot;
struct MenuItem MainMenuRoot;
struct MenuItem MI_1_ID3FD_MainScreenChilds[8];

struct MenuItem MainScreenRoot = {0,-1,0,0,0,0,MI_1_ID3FD_MainScreenChilds,9,en,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone};

#if defined(EPSON_FLASH_IDLE)&&!defined(FULGENCY_FUN)
struct MenuItem MainMenuRoot = {0,-1,0,0,0,0,MI_0_ID00_MenuChilds,16,en,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone};
#else
struct MenuItem MainMenuRoot = {0,-1,0,0,0,0,MI_0_ID00_MenuChilds,15,en,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone};
#endif
// 纸张类型
struct MenuItem MI_2_ID05_SHEETTYPE[7]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[5],0,0,0,(unsigned short *)MenuStr_3_ID06_Highlight,"High Glossy",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaType,(UISetFloatFuncExType)Menu_SetMediaType,0,0,utNone},
#else
{&MI_0_ID00_MenuChilds[5],0,0,0,(unsigned short *)MenuStr_3_ID06_Highlight,"High Glossy Photopaper",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaType,(UISetFloatFuncExType)Menu_SetMediaType,0,0,utNone},
#endif
{&MI_0_ID00_MenuChilds[5],1,0,0,(unsigned short *)MenuStr_3_ID07_Carstickers,"Vinly",0,0,chs,radiocheckbox,radiochecked,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaType,(UISetFloatFuncExType)Menu_SetMediaType,0,1,utNone},
{&MI_0_ID00_MenuChilds[5],2,0,0,(unsigned short *)MenuStr_3_ID08_PP,"PP",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaType,(UISetFloatFuncExType)Menu_SetMediaType,0,2,utNone},
{&MI_0_ID00_MenuChilds[5],3,0,0,(unsigned short *)MenuStr_3_ID09_Lighting,"Glossy Film",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaType,(UISetFloatFuncExType)Menu_SetMediaType,0,3,utNone},
{&MI_0_ID00_MenuChilds[5],4,0,0,(unsigned short *)MenuStr_3_ID0A_Other1,"Other1",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaType,(UISetFloatFuncExType)Menu_SetMediaType,0,4,utNone},
{&MI_0_ID00_MenuChilds[5],5,0,0,(unsigned short *)MenuStr_3_ID0B_Other2,"Other2",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaType,(UISetFloatFuncExType)Menu_SetMediaType,0,5,utNone},
{&MI_0_ID00_MenuChilds[5],6,0,0,(unsigned short *)MenuStr_3_ID0C_Other3,"Other3",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaType,(UISetFloatFuncExType)Menu_SetMediaType,0,6,utNone}
};

#if 0
struct MenuItem MI_2_ID05_CUT[2]={
{&MI_1_ID0D_OriginAndMediaWidth[3],0,0,0,(unsigned short *)MenuStr_2_ID11_AutoFeed,"Auto Feed",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoFeed,(UISetFloatFuncExType)Menu_SetAutoFeed,0,0,utNone},
{&MI_1_ID0D_OriginAndMediaWidth[3],1,0,0,(unsigned short *)MenuStr_2_ID13_SheetCut,"Cut",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_SheetCut,1,utNone},
};
#endif

// 原点&纸宽
#ifdef MICOLOR_AUTOFUCTION
struct MenuItem MI_1_ID0D_OriginAndMediaWidth[10]={
#elif defined RIPSTAR_FLAT_EX
struct MenuItem MI_1_ID0D_OriginAndMediaWidth[8]={
#elif defined (LECAI_EPSON_4H_Y_ROOLENCODER)
struct MenuItem MI_1_ID0D_OriginAndMediaWidth[7]={
#else
struct MenuItem MI_1_ID0D_OriginAndMediaWidth[6]={
#endif
{&MI_0_ID00_MenuChilds[6],0,0,0,(unsigned short *)MenuStr_2_ID12_Measurement,"Auto Measure",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_MeasureMedia,0,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[6],1,0,0,(unsigned short *)MenuStr_2_ID0F_InputValue,"XOrig",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBasePoint,(UISetFloatFuncExType)Menu_InputBasePoint,0,1,utCM},
{&MI_0_ID00_MenuChilds[6],2,0,0,(unsigned short *)MenuStr_2_ID10_InputMediaWidth,"Width",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaWidth,(UISetFloatFuncExType)Menu_InputMediaWidth,0,2,utCM},
#else
{&MI_0_ID00_MenuChilds[6],1,0,0,(unsigned short *)MenuStr_2_ID0F_InputValue,"Input XOrigin",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBasePoint,(UISetFloatFuncExType)Menu_InputBasePoint,0,1,utCM},
{&MI_0_ID00_MenuChilds[6],2,0,0,(unsigned short *)MenuStr_2_ID10_InputMediaWidth,"Input Width",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaWidth,(UISetFloatFuncExType)Menu_InputMediaWidth,0,2,utCM},
#endif
#ifdef MICOLOR_AUTOFUCTION
{&MI_0_ID00_MenuChilds[6],3,0,0,(unsigned short *)MenuStr_2_ID0E_SetOrigin,"Set Origin",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_SetBasePoint,3,utNone},
{&MI_0_ID00_MenuChilds[6],4,0,0,(unsigned short *)MenuStr_2_ID13_SheetCut,"Sheet Cut",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_SheetCut,4,utNone},
#ifdef DGEN_AUTOFUCTION
{&MI_0_ID00_MenuChilds[6],5,0,0,(unsigned short *)MenuStr_2_ID11_AutoFeed,"Feed",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,(UIActionFuncExType)Menu_DGEN_Move_Paper,5,utNone},
{&MI_0_ID00_MenuChilds[6],6,0,0,(unsigned short *)MenuStr_2_ID11_FeedSpeed,"Feed Speed",0,0,chs,editor,normalStatus,0,tInt,4,0,0,0,0,{0},(UIGetFloatFuncExType)GetMove_Paper_Speed,(UISetFloatFuncExType)SetMove_Paper_Speed,0,6,utNone},
#else
{&MI_0_ID00_MenuChilds[6],5,0,0,(unsigned short *)MenuStr_2_ID11_AutoFeed,"Feed",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,5,utNone},
{&MI_0_ID00_MenuChilds[6],6,0,0,(unsigned short *)MenuStr_2_ID11_FeedSpeed,"Feed Speed",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,6,utNone},
#endif
{&MI_0_ID00_MenuChilds[6],7,0,0,(unsigned short *)MenuStr_2_ID11_Loadmedia,"Load Media",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_LoadMedia,7,utNone},
{&MI_0_ID00_MenuChilds[6],8,0,0,(unsigned short *)MenuStr_2_ID14_Margin,"Margin",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMargin,(UISetFloatFuncExType)Menu_SetMargin,0,8,utCM},
{&MI_0_ID00_MenuChilds[6],9,0,0,(unsigned short *)MenuStr_2_ID12_1_Measurement,"Aging Test",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,(UIActionFuncExType)Menu_MachineTest,9,utNone}
#elif defined RIPSTAR_FLAT_EX
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[6],3,0,0,(unsigned short *)MenuStr_2_ID0F_InputYValue,"YOrig",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseYPoint,(UISetFloatFuncExType)Menu_InputBaseYPoint,0,2,utCM},
{&MI_0_ID00_MenuChilds[6],4,0,0,(unsigned short *)MenuStr_2_ID10_InputMediaLength,"Length",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaLength,(UISetFloatFuncExType)Menu_InputMediaLength,0,3,utCM},
{&MI_0_ID00_MenuChilds[6],5,0,0,(unsigned short *)MenuStr_2_ID0F_InputZValue,"ZOrig",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseZPoint,(UISetFloatFuncExType)Menu_InputBaseZPoint,0,4,utCM},
//{&MI_0_ID00_MenuChilds[6],5,0,0,(unsigned short *)MenuStr_2_ID10_InputMediaHigh,"Input MediaH",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaHigh,(UISetFloatFuncExType)Menu_InputMediaHigh,0,5,utCM},
{&MI_0_ID00_MenuChilds[6],6,0,0,(unsigned short *)MenuStr_2_ID0E_MoveZOrigin,"Move To ZOrig",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_Move_Zpoint,6,utNone},
#else
{&MI_0_ID00_MenuChilds[6],3,0,0,(unsigned short *)MenuStr_2_ID0F_InputYValue,"Input YOrigin",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseYPoint,(UISetFloatFuncExType)Menu_InputBaseYPoint,0,2,utCM},
{&MI_0_ID00_MenuChilds[6],4,0,0,(unsigned short *)MenuStr_2_ID10_InputMediaLength,"Input Length",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaLength,(UISetFloatFuncExType)Menu_InputMediaLength,0,3,utCM},
{&MI_0_ID00_MenuChilds[6],5,0,0,(unsigned short *)MenuStr_2_ID0F_InputZValue,"Input ZOrigin",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseZPoint,(UISetFloatFuncExType)Menu_InputBaseZPoint,0,4,utCM},
//{&MI_0_ID00_MenuChilds[6],5,0,0,(unsigned short *)MenuStr_2_ID10_InputMediaHigh,"Input MediaH",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaHigh,(UISetFloatFuncExType)Menu_InputMediaHigh,0,5,utCM},
{&MI_0_ID00_MenuChilds[6],6,0,0,(unsigned short *)MenuStr_2_ID0E_MoveZOrigin,"Move To ZOrigin",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_Move_Zpoint,6,utNone},
#endif
{&MI_0_ID00_MenuChilds[6],7,0,0,(unsigned short *)MenuStr_2_ID14_Margin,"Margin",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMargin,(UISetFloatFuncExType)Menu_SetMargin,0,4,utCM}
#elif defined(LECAI_EPSON_4H_Y_ROOLENCODER)
{&MI_0_ID00_MenuChilds[6],3,0,0,(unsigned short *)MenuStr_2_ID0E_SetOrigin,"Set Origin",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_SetBasePoint,3,utNone},
{&MI_0_ID00_MenuChilds[6],4,0,0,(unsigned short *)MenuStr_2_ID13_SheetCut,"Sheet Cut",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_SheetCut,4,utNone},
{&MI_0_ID00_MenuChilds[6],5,0,0,(unsigned short *)MenuStr_2_ID14_Margin,"Margin",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMargin,(UISetFloatFuncExType)Menu_SetMargin,0,4,utCM},
{&MI_0_ID00_MenuChilds[6],6,0,0,(unsigned short *)MenuStr_2_ID12_1_Measurement,"Aging Test",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,(UIActionFuncExType)Menu_MachineTest,5,utNone}
#else
{&MI_0_ID00_MenuChilds[6],3,0,0,(unsigned short *)MenuStr_2_ID0E_SetOrigin,"Set Origin",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Menu_SetBasePoint,3,utNone},
{&MI_0_ID00_MenuChilds[6],4,0,0,(unsigned short *)MenuStr_2_ID14_Margin,"Margin",0,0,chs,editor,normalStatus,2,tFloat,7,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMargin,(UISetFloatFuncExType)Menu_SetMargin,0,4,utCM},
{&MI_0_ID00_MenuChilds[6],5,0,0,(unsigned short *)MenuStr_2_ID12_1_Measurement,"Aging Test",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,(UIActionFuncExType)Menu_MachineTest,5,utNone}
#endif
};

// 自动功能设置
#if defined(MICOLOR_AUTOFUCTION)
struct MenuItem MI_MICOLOR_EM_1_ID16_SetAutofunction[10]={
{&MI_0_ID00_MenuChilds[7],0,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID17_AutoCutting,"Auto Cutting",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoCut,(UISetFloatFuncExType)Menu_SetAutoCut,0,0,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[7],1,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID18_Automaticmeasurement,"Auto Measure",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoMeasureWidth,(UISetFloatFuncExType)Menu_SetAutoMeasureWidth,0,1,utNone},
{&MI_0_ID00_MenuChilds[7],2,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID19_AutoLoadMedia,"Auto Load Med",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoLoadMedia,(UISetFloatFuncExType)Menu_SetAutoLoadMedia,0,2,utNone},
{&MI_0_ID00_MenuChilds[7],3,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1A_Lackpaperdetection,"Lack Med Detec",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoOutofMedia,(UISetFloatFuncExType)Menu_SetAutoOutofMedia,0,3,utNone},
{&MI_0_ID00_MenuChilds[7],4,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1B_Thefrontpanelsecurityprotection,"Cover Protect",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoCoverCheck,(UISetFloatFuncExType)Menu_SetAutoCoverCheck,0,4,utNone},
{&MI_0_ID00_MenuChilds[7],5,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_Automaticairsuction,"Auto Plat Suc",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoPlateFan,(UISetFloatFuncExType)Menu_SetAutoPlateFan,0,5,utNone},
{&MI_0_ID00_MenuChilds[7],6,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_Thepaperpolecurityprotection,"Fix Pole Prote",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoFixCheck,(UISetFloatFuncExType)Menu_SetAutoFixCheck,0,6,utNone},
{&MI_0_ID00_MenuChilds[7],7,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_AutoPowerOnClean,"Auto Pow On Cl",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoPowerOnClean,(UISetFloatFuncExType)Menu_SetAutoPowerOnClean,0,7,utNone},
{&MI_0_ID00_MenuChilds[7],8,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_AutoGoHome,"Auto Go Home",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoGoHome,(UISetFloatFuncExType)Menu_SetAutoGoHome,0,8,utNone},
{&MI_0_ID00_MenuChilds[7],9,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_AutoStep,"Step Bef Print",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoStep,(UISetFloatFuncExType)Menu_SetAutoStep,0,9,utNone}
#else
{&MI_0_ID00_MenuChilds[7],1,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID18_Automaticmeasurement,"Automatic Measurement",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoMeasureWidth,(UISetFloatFuncExType)Menu_SetAutoMeasureWidth,0,1,utNone},
{&MI_0_ID00_MenuChilds[7],2,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID19_AutoLoadMedia,"Auto Load Media",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoLoadMedia,(UISetFloatFuncExType)Menu_SetAutoLoadMedia,0,2,utNone},
{&MI_0_ID00_MenuChilds[7],3,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1A_Lackpaperdetection,"Lack Media Detection",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoOutofMedia,(UISetFloatFuncExType)Menu_SetAutoOutofMedia,0,3,utNone},
{&MI_0_ID00_MenuChilds[7],4,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1B_Thefrontpanelsecurityprotection,"Cover Protection",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoCoverCheck,(UISetFloatFuncExType)Menu_SetAutoCoverCheck,0,4,utNone},
{&MI_0_ID00_MenuChilds[7],5,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_Automaticairsuction,"Automatic Platform Suction",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoPlateFan,(UISetFloatFuncExType)Menu_SetAutoPlateFan,0,5,utNone},
{&MI_0_ID00_MenuChilds[7],6,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_Thepaperpolecurityprotection,"Fix Pole Protection",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoFixCheck,(UISetFloatFuncExType)Menu_SetAutoFixCheck,0,6,utNone},
{&MI_0_ID00_MenuChilds[7],7,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_AutoPowerOnClean,"Automatic Power On Clean",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoPowerOnClean,(UISetFloatFuncExType)Menu_SetAutoPowerOnClean,0,7,utNone},
{&MI_0_ID00_MenuChilds[7],8,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_AutoGoHome,"Auto Go Home",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoGoHome,(UISetFloatFuncExType)Menu_SetAutoGoHome,0,8,utNone},
{&MI_0_ID00_MenuChilds[7],9,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_AutoStep,"Step Before Print",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoStep,(UISetFloatFuncExType)Menu_SetAutoStep,0,9,utNone}
#endif
};
#endif

// 羽化强度
struct MenuItem MI_2_ID21_ECLOSIONLEVEL[4]={
#if defined(LCD_MODULE_128_64_ST5668A)
#ifdef MANUFACTURER_GZ
{&MI_1_ID20_QUALITY[0],0,0,0,(unsigned short *)MenuStr_3_ID22_ECLOSIONNONE,"Off",0,0,chs,radiocheckbox,radiochecked,0,tNone,0,0,0,0,0,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,0,utNone},
{&MI_1_ID20_QUALITY[0],1,0,0,(unsigned short *)MenuStr_3_ID23_ECLOSIONLEVEL1,"Weak",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,1,utNone},
{&MI_1_ID20_QUALITY[0],2,0,0,(unsigned short *)MenuStr_3_ID24_ECLOSIONLEVEL2,"Normal",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,2,utNone},
{&MI_1_ID20_QUALITY[0],3,0,0,(unsigned short *)MenuStr_3_ID24_ECLOSIONLEVEL3,"Strong",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,3,utNone}
#else
{&MI_1_ID20_QUALITY[0],0,0,0,(unsigned short *)MenuStr_3_ID22_ECLOSIONNONE,"Off",0,0,chs,radiocheckbox,radiochecked,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,0,utNone},
{&MI_1_ID20_QUALITY[0],1,0,0,(unsigned short *)MenuStr_3_ID23_ECLOSIONLEVEL1,"Weak",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,1,utNone},
{&MI_1_ID20_QUALITY[0],2,0,0,(unsigned short *)MenuStr_3_ID24_ECLOSIONLEVEL2,"Normal",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,2,utNone},
{&MI_1_ID20_QUALITY[0],3,0,0,(unsigned short *)MenuStr_3_ID24_ECLOSIONLEVEL3,"Strong",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,3,utNone}
#endif

#else
#ifdef MANUFACTURER_GZ
{&MI_1_ID20_QUALITY[0],0,0,0,(unsigned short *)MenuStr_3_ID22_ECLOSIONNONE,"Feather Off",0,0,chs,radiocheckbox,radiochecked,0,tNone,0,0,0,0,0,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,0,utNone},
{&MI_1_ID20_QUALITY[0],1,0,0,(unsigned short *)MenuStr_3_ID23_ECLOSIONLEVEL1,"Weak",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,1,utNone},
{&MI_1_ID20_QUALITY[0],2,0,0,(unsigned short *)MenuStr_3_ID24_ECLOSIONLEVEL2,"Normal",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,2,utNone},
{&MI_1_ID20_QUALITY[0],3,0,0,(unsigned short *)MenuStr_3_ID24_ECLOSIONLEVEL3,"Strong",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,3,utNone}
#else
{&MI_1_ID20_QUALITY[0],0,0,0,(unsigned short *)MenuStr_3_ID22_ECLOSIONNONE,"Feather Off",0,0,chs,radiocheckbox,radiochecked,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,0,utNone},
{&MI_1_ID20_QUALITY[0],1,0,0,(unsigned short *)MenuStr_3_ID23_ECLOSIONLEVEL1,"Feather Weak",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,1,utNone},
{&MI_1_ID20_QUALITY[0],2,0,0,(unsigned short *)MenuStr_3_ID24_ECLOSIONLEVEL2,"Feather Normal",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,2,utNone},
{&MI_1_ID20_QUALITY[0],3,0,0,(unsigned short *)MenuStr_3_ID24_ECLOSIONLEVEL3,"Feather Strong",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityLevel,(UISetFloatFuncExType)Menu_SetPrintQualityLevel,0,3,utNone}
#endif
#endif
};

// 羽化类型
struct MenuItem MI_2_ID21_ECLOSIONTYPE[6]={
{&MI_1_ID20_QUALITY[0],0,0,0,(unsigned short *)MenuStr_3_ID25_ECLOSIONTYPE1,"Gradient",0,0,chs,radiocheckbox,radiochecked,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityType,(UISetFloatFuncExType)Menu_SetPrintQualityType,0,0,utNone},
{&MI_1_ID20_QUALITY[0],1,0,0,(unsigned short *)MenuStr_3_ID25_ECLOSIONTYPE2,"Uniform",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityType,(UISetFloatFuncExType)Menu_SetPrintQualityType,0,1,utNone},
{&MI_1_ID20_QUALITY[0],2,0,0,(unsigned short *)MenuStr_3_ID25_ECLOSIONTYPE3,"Wave",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityType,(UISetFloatFuncExType)Menu_SetPrintQualityType,0,2,utNone},
{&MI_1_ID20_QUALITY[0],3,0,0,(unsigned short *)MenuStr_3_ID25_ECLOSIONTYPE4,"Advance",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityType,(UISetFloatFuncExType)Menu_SetPrintQualityType,0,3,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_1_ID20_QUALITY[0],4,0,0,(unsigned short *)MenuStr_3_ID25_ECLOSIONTYPE5,"Unif Gradient",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityType,(UISetFloatFuncExType)Menu_SetPrintQualityType,0,4,utNone},
#else
{&MI_1_ID20_QUALITY[0],4,0,0,(unsigned short *)MenuStr_3_ID25_ECLOSIONTYPE5,"UniformGradient",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityType,(UISetFloatFuncExType)Menu_SetPrintQualityType,0,4,utNone},
#endif
{&MI_1_ID20_QUALITY[0],5,0,0,(unsigned short *)MenuStr_3_ID25_ECLOSIONTYPE6,"Tattinglace",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPrintQualityType,(UISetFloatFuncExType)Menu_SetPrintQualityType,0,5,utNone}
};

// 喷头选择
struct MenuItem MI_ALLWIN_EM_3_ID286_Head_select[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_2_ID21_1HEADPRINT[1],0,0,0,(unsigned short *)MenuStr_3_ID22_HEAD1,"Only Open H1",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetMenu_EPSON_ALLWIN_SELECTHeadPrint,(UISetFloatFuncExType)SetMenu_EPSON_ALLWIN_SELECTHeadPrint,0,0,utNone},
{&MI_2_ID21_1HEADPRINT[1],1,0,0,(unsigned short *)MenuStr_3_ID22_HEAD2,"Only Open H2",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetMenu_EPSON_ALLWIN_SELECTHeadPrint,(UISetFloatFuncExType)SetMenu_EPSON_ALLWIN_SELECTHeadPrint,0,1,utNone},
#else
{&MI_2_ID21_1HEADPRINT[1],0,0,0,(unsigned short *)MenuStr_3_ID22_HEAD1,"Only Open Head 1",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetMenu_EPSON_ALLWIN_SELECTHeadPrint,(UISetFloatFuncExType)SetMenu_EPSON_ALLWIN_SELECTHeadPrint,0,0,utNone},
{&MI_2_ID21_1HEADPRINT[1],1,0,0,(unsigned short *)MenuStr_3_ID22_HEAD2,"Only Open Head 2",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetMenu_EPSON_ALLWIN_SELECTHeadPrint,(UISetFloatFuncExType)SetMenu_EPSON_ALLWIN_SELECTHeadPrint,0,1,utNone},
#endif
};

// 单头打印
struct MenuItem MI_2_ID21_1HEADPRINT[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_1_ID20_QUALITY[2],0,0,0,(unsigned short *)MenuStr_3_ID22_1HEADPRINTON,"One H On/Off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetMenu_EPSON_ALLWIN_1HeadPrint_OnOff,(UISetFloatFuncExType)SetMenu_EPSON_ALLWIN_1HeadPrint_OnOff,0,0,utNone},
#else
{&MI_1_ID20_QUALITY[2],0,0,0,(unsigned short *)MenuStr_3_ID22_1HEADPRINTON,"One Head Print On/Off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetMenu_EPSON_ALLWIN_1HeadPrint_OnOff,(UISetFloatFuncExType)SetMenu_EPSON_ALLWIN_1HeadPrint_OnOff,0,0,utNone},
#endif
{&MI_1_ID20_QUALITY[2],1,0,0,(unsigned short *)MenuStr_3_ID22_1HEADSELECT,"Head Select",MI_ALLWIN_EM_3_ID286_Head_select,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}

};


// 打印质量
struct MenuItem MI_1_ID20_QUALITY[5]={
{&MI_0_ID00_MenuChilds[10],0,0,0,(unsigned short *)MenuStr_2_ID21_ECLOSIONTYPE,"Feather Type",MI_2_ID21_ECLOSIONTYPE,6,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_0_ID00_MenuChilds[10],1,0,0,(unsigned short *)MenuStr_2_ID21_ECLOSIONLEVEL,"Feather Level",MI_2_ID21_ECLOSIONLEVEL,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[10],2,0,0,(unsigned short *)MenuStr_2_ID21_1HEADPRINT,"One H Print",MI_2_ID21_1HEADPRINT,2,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,2,utNone},
{&MI_0_ID00_MenuChilds[10],3,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_AutoStep,"Step Prev Job",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoStep,(UISetFloatFuncExType)Menu_SetAutoStep,0,3,utNone},
#if defined(EPSON_CLEAN_UPDOWN)||defined(EPSON_CLEAN_INTEGRATE_FLASH_UNION)        
{&MI_0_ID00_MenuChilds[10],4,0,0,(unsigned short *)MenuStr_TATE_EM_2_ID1C_AutoFlashPrint,"Spit Prev Job",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoFlashBeforePrint,(UISetFloatFuncExType)Menu_SetAutoFlashBeforePrint,0,4,utNone}
#else
{&MI_0_ID00_MenuChilds[10],4,0,0,(unsigned short *)MenuStr_TATE_EM_2_ID1C_AutoFlashPrint,"Spit Prev Job",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,0,{0},(UIGetFloatFuncExType)Menu_GetAutoFlashBeforePrint,(UISetFloatFuncExType)Menu_SetAutoFlashBeforePrint,0,4,utNone}
#endif
#else
{&MI_0_ID00_MenuChilds[10],2,0,0,(unsigned short *)MenuStr_2_ID21_1HEADPRINT,"One Head Print",MI_2_ID21_1HEADPRINT,2,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,2,utNone},
{&MI_0_ID00_MenuChilds[10],3,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID1C_AutoStep,"Step Before Print",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoStep,(UISetFloatFuncExType)Menu_SetAutoStep,0,3,utNone},
#if defined(EPSON_CLEAN_UPDOWN)||defined(EPSON_CLEAN_INTEGRATE_FLASH_UNION)        
{&MI_0_ID00_MenuChilds[10],4,0,0,(unsigned short *)MenuStr_TATE_EM_2_ID1C_AutoFlashPrint,"Flash Before Print",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetAutoFlashBeforePrint,(UISetFloatFuncExType)Menu_SetAutoFlashBeforePrint,0,4,utNone}
#else
{&MI_0_ID00_MenuChilds[10],4,0,0,(unsigned short *)MenuStr_TATE_EM_2_ID1C_AutoFlashPrint,"Flash Before Print",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,0,{0},(UIGetFloatFuncExType)Menu_GetAutoFlashBeforePrint,(UISetFloatFuncExType)Menu_SetAutoFlashBeforePrint,0,4,utNone}
#endif
#endif
};

// 高光
struct MenuItem MI_UM_3_ID28_Highglossyphotopaper[2]={
{&MI_UM_2_ID27_FEEDadjust[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_Step,0,utNone},
{&MI_UM_2_ID27_FEEDadjust[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_Step_Modify,(UISetFloatFuncExType)Cali_UM_Set_Step_Modify,0,0,utNone}
};

// 车贴
struct MenuItem MI_UM_3_ID2B_vinly[2]={
{&MI_UM_2_ID27_FEEDadjust[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_Step,1,utNone},
{&MI_UM_2_ID27_FEEDadjust[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_Step_Modify,(UISetFloatFuncExType)Cali_UM_Set_Step_Modify,0,1,utNone}
};

// PP
struct MenuItem MI_UM_3_ID2E_PP[2]={
{&MI_UM_2_ID27_FEEDadjust[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_Step,2,utNone},
{&MI_UM_2_ID27_FEEDadjust[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_Step_Modify,(UISetFloatFuncExType)Cali_UM_Set_Step_Modify,0,2,utNone}
};

// 灯片
struct MenuItem MI_UM_3_ID31_glossyfilm[2]={
{&MI_UM_2_ID27_FEEDadjust[3],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_Step,3,utNone},
{&MI_UM_2_ID27_FEEDadjust[3],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_Step_Modify,(UISetFloatFuncExType)Cali_UM_Set_Step_Modify,0,3,utNone}
};

// 其他1
struct MenuItem MI_UM_3_ID34_Other1[2]={
{&MI_UM_2_ID27_FEEDadjust[4],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_Step,4,utNone},
{&MI_UM_2_ID27_FEEDadjust[4],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_Step_Modify,(UISetFloatFuncExType)Cali_UM_Set_Step_Modify,0,4,utNone}
};

// 其他2
struct MenuItem MI_UM_3_ID37_Other2[2]={
{&MI_UM_2_ID27_FEEDadjust[5],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_Step,5,utNone},
{&MI_UM_2_ID27_FEEDadjust[5],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_Step_Modify,(UISetFloatFuncExType)Cali_UM_Set_Step_Modify,0,5,utNone}
};

// 其他3
struct MenuItem MI_UM_3_ID3A_Other3[2]={
{&MI_UM_2_ID27_FEEDadjust[6],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_Step,6,utNone},
{&MI_UM_2_ID27_FEEDadjust[6],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_Step_Modify,(UISetFloatFuncExType)Cali_UM_Set_Step_Modify,0,6,utNone}
};

// 步进
#ifdef RIPSTAR_FLAT_EX
struct MenuItem MI_UM_2_ID27_FEEDadjust[2]={
{&MI_UM_1_ID25_ADJUSTMENT[3],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_Step,0,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[3],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_Step_Modify,(UISetFloatFuncExType)Cali_UM_Set_Step_Modify,0,0,utNone}
};
#else
struct MenuItem MI_UM_2_ID27_FEEDadjust[7]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_UM_1_ID25_ADJUSTMENT[3],0,0,0,(unsigned short *)MenuStr_3_ID06_Highlight,"High Glossy",MI_UM_3_ID28_Highglossyphotopaper,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#else
{&MI_UM_1_ID25_ADJUSTMENT[3],0,0,0,(unsigned short *)MenuStr_3_ID06_Highlight,"High Glossy Photopaper",MI_UM_3_ID28_Highglossyphotopaper,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#endif
{&MI_UM_1_ID25_ADJUSTMENT[3],1,0,0,(unsigned short *)MenuStr_3_ID07_Carstickers,"Vinly",MI_UM_3_ID2B_vinly,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[3],2,0,0,(unsigned short *)MenuStr_3_ID08_PP,"PP",MI_UM_3_ID2E_PP,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[3],3,0,0,(unsigned short *)MenuStr_UM_3_ID31_glossyfilm,"Glossy Film",MI_UM_3_ID31_glossyfilm,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[3],4,0,0,(unsigned short *)MenuStr_3_ID0A_Other1,"Other1",MI_UM_3_ID34_Other1,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[3],5,0,0,(unsigned short *)MenuStr_3_ID0B_Other2,"Other2",MI_UM_3_ID37_Other2,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[3],6,0,0,(unsigned short *)MenuStr_3_ID0C_Other3,"Other3",MI_UM_3_ID3A_Other3,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone}
};
#endif
// VSD1

struct MenuItem MI_UM_3_ID3E_VSD1[3]={
//{&MI_UM_2_ID3D_BiDir[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_BiDir,0,utNone},
{&MI_UM_2_ID3D_BiDir[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_BiDir,0,utNone},
{&MI_UM_2_ID3D_BiDir[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,(UISetFloatFuncExType)Cali_UM_Set_BiDir,0,0,utNone},
{&MI_UM_2_ID3D_BiDir[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,(UISetFloatFuncExType)Cali_UM_Set_BiDir,0,10,utNone}
};

// VSD2
struct MenuItem MI_UM_3_ID41_VSD2[3]={
{&MI_UM_2_ID3D_BiDir[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_BiDir,1,utNone},
{&MI_UM_2_ID3D_BiDir[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,(UISetFloatFuncExType)Cali_UM_Set_BiDir,0,1,utNone},
{&MI_UM_2_ID3D_BiDir[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,(UISetFloatFuncExType)Cali_UM_Set_BiDir,0,11,utNone}
};

// VSD3
struct MenuItem MI_UM_3_ID44_VSD3[3]={
{&MI_UM_2_ID3D_BiDir[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_BiDir,2,utNone},
{&MI_UM_2_ID3D_BiDir[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,(UISetFloatFuncExType)Cali_UM_Set_BiDir,0,2,utNone},
{&MI_UM_2_ID3D_BiDir[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,(UISetFloatFuncExType)Cali_UM_Set_BiDir,0,12,utNone}
};


// VSD4
struct MenuItem MI_UM_3_ID47_VSD4[3]={
{&MI_UM_2_ID3D_BiDir[3],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_Print_BiDir,3,utNone},
{&MI_UM_2_ID3D_BiDir[3],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,(UISetFloatFuncExType)Cali_UM_Set_BiDir,0,3,utNone},
{&MI_UM_2_ID3D_BiDir[3],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,(UISetFloatFuncExType)Cali_UM_Set_BiDir,0,13,utNone}
};


// 双向
struct MenuItem MI_UM_2_ID3D_BiDir[4]={
{&MI_UM_1_ID25_ADJUSTMENT[4],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",MI_UM_3_ID3E_VSD1,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,0,0,0,utNone},
#if defined(HEAD_EPSON_GEN5)||defined(RICOH_VSD2)
{&MI_UM_1_ID25_ADJUSTMENT[4],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_UM_3_ID41_VSD2,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,0,0,1,utNone},
#else
{&MI_UM_1_ID25_ADJUSTMENT[4],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_UM_3_ID41_VSD2,3,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,0,0,1,utNone},
#endif
{&MI_UM_1_ID25_ADJUSTMENT[4],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",MI_UM_3_ID44_VSD3,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,0,0,2,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[4],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",MI_UM_3_ID47_VSD4,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_UM_Get_BiDir,0,0,3,utNone}
};

extern struct MenuItem MI_UM_3_ID3E_Head_Space[4];

// VSD1
struct MenuItem MI_UM_6_ID74_VSD1_CorrectionValue[3]={
{&MI_UM_3_ID3E_Head_Space[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_Print_GroupSpace,0,utNone},
{&MI_UM_3_ID3E_Head_Space[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,0,tInt,3,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_UM_3_ID3E_Head_Space[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_UM_6_ID74_CorrectionValue_VSD1,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};
// 校正值
struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD1[MAX_HEAD_NUM]={
{&MI_UM_6_ID74_VSD1_CorrectionValue[2],0,0,0,(unsigned short *)MenuStr_Head_Space_Name[0],MenuStr_Head_Space_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,0,utNone},
{&MI_UM_6_ID74_VSD1_CorrectionValue[2],1,0,0,(unsigned short *)MenuStr_Head_Space_Name[1],MenuStr_Head_Space_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_UM_6_ID74_VSD1_CorrectionValue[2],2,0,0,(unsigned short *)MenuStr_Head_Space_Name[2],MenuStr_Head_Space_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2,utNone},
{&MI_UM_6_ID74_VSD1_CorrectionValue[2],3,0,0,(unsigned short *)MenuStr_Head_Space_Name[3],MenuStr_Head_Space_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3,utNone},
{&MI_UM_6_ID74_VSD1_CorrectionValue[2],4,0,0,(unsigned short *)MenuStr_Head_Space_Name[4],MenuStr_Head_Space_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,100,utNone},
{&MI_UM_6_ID74_VSD1_CorrectionValue[2],5,0,0,(unsigned short *)MenuStr_Head_Space_Name[5],MenuStr_Head_Space_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,101,utNone},
{&MI_UM_6_ID74_VSD1_CorrectionValue[2],6,0,0,(unsigned short *)MenuStr_Head_Space_Name[6],MenuStr_Head_Space_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,102,utNone},
{&MI_UM_6_ID74_VSD1_CorrectionValue[2],7,0,0,(unsigned short *)MenuStr_Head_Space_Name[7],MenuStr_Head_Space_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,103,utNone}
};

// VSD2
struct MenuItem MI_UM_6_ID74_VSD2_CorrectionValue[3]={
{&MI_UM_3_ID3E_Head_Space[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_Print_GroupSpace,1,utNone},
{&MI_UM_3_ID3E_Head_Space[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,0,tInt,3,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1001,utNone},
{&MI_UM_3_ID3E_Head_Space[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_UM_6_ID74_CorrectionValue_VSD2,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};
// 校正值
struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD2[MAX_HEAD_NUM]={
{&MI_UM_6_ID74_VSD2_CorrectionValue[2],0,0,0,(unsigned short *)MenuStr_Head_Space_Name[0],MenuStr_Head_Space_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,0,utNone},
{&MI_UM_6_ID74_VSD2_CorrectionValue[2],1,0,0,(unsigned short *)MenuStr_Head_Space_Name[1],MenuStr_Head_Space_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_UM_6_ID74_VSD2_CorrectionValue[2],2,0,0,(unsigned short *)MenuStr_Head_Space_Name[2],MenuStr_Head_Space_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2,utNone},
{&MI_UM_6_ID74_VSD2_CorrectionValue[2],3,0,0,(unsigned short *)MenuStr_Head_Space_Name[3],MenuStr_Head_Space_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3,utNone},
{&MI_UM_6_ID74_VSD2_CorrectionValue[2],4,0,0,(unsigned short *)MenuStr_Head_Space_Name[4],MenuStr_Head_Space_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,100,utNone},
{&MI_UM_6_ID74_VSD2_CorrectionValue[2],5,0,0,(unsigned short *)MenuStr_Head_Space_Name[5],MenuStr_Head_Space_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,101,utNone},
{&MI_UM_6_ID74_VSD2_CorrectionValue[2],6,0,0,(unsigned short *)MenuStr_Head_Space_Name[6],MenuStr_Head_Space_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,102,utNone},
{&MI_UM_6_ID74_VSD2_CorrectionValue[2],7,0,0,(unsigned short *)MenuStr_Head_Space_Name[7],MenuStr_Head_Space_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,103,utNone}
};

// VSD3
struct MenuItem MI_UM_6_ID74_VSD3_CorrectionValue[3]={
{&MI_UM_3_ID3E_Head_Space[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_Print_GroupSpace,2,utNone},
{&MI_UM_3_ID3E_Head_Space[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,0,tInt,3,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2001,utNone},
{&MI_UM_3_ID3E_Head_Space[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_UM_6_ID74_CorrectionValue_VSD3,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};
// 校正值
struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD3[MAX_HEAD_NUM]={
{&MI_UM_6_ID74_VSD3_CorrectionValue[2],0,0,0,(unsigned short *)MenuStr_Head_Space_Name[0],MenuStr_Head_Space_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,0,utNone},
{&MI_UM_6_ID74_VSD3_CorrectionValue[2],1,0,0,(unsigned short *)MenuStr_Head_Space_Name[1],MenuStr_Head_Space_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_UM_6_ID74_VSD3_CorrectionValue[2],2,0,0,(unsigned short *)MenuStr_Head_Space_Name[2],MenuStr_Head_Space_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2,utNone},
{&MI_UM_6_ID74_VSD3_CorrectionValue[2],3,0,0,(unsigned short *)MenuStr_Head_Space_Name[3],MenuStr_Head_Space_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3,utNone},
{&MI_UM_6_ID74_VSD3_CorrectionValue[2],4,0,0,(unsigned short *)MenuStr_Head_Space_Name[4],MenuStr_Head_Space_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,100,utNone},
{&MI_UM_6_ID74_VSD3_CorrectionValue[2],5,0,0,(unsigned short *)MenuStr_Head_Space_Name[5],MenuStr_Head_Space_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,101,utNone},
{&MI_UM_6_ID74_VSD3_CorrectionValue[2],6,0,0,(unsigned short *)MenuStr_Head_Space_Name[6],MenuStr_Head_Space_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,102,utNone},
{&MI_UM_6_ID74_VSD3_CorrectionValue[2],7,0,0,(unsigned short *)MenuStr_Head_Space_Name[7],MenuStr_Head_Space_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,103,utNone}
};

// VSD4
struct MenuItem MI_UM_6_ID74_VSD4_CorrectionValue[3]={
{&MI_UM_3_ID3E_Head_Space[3],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_Print_GroupSpace,3,utNone},
{&MI_UM_3_ID3E_Head_Space[3],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,0,tInt,3,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3001,utNone},
{&MI_UM_3_ID3E_Head_Space[3],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_UM_6_ID74_CorrectionValue_VSD4,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};
// 校正值
struct MenuItem MI_UM_6_ID74_CorrectionValue_VSD4[MAX_HEAD_NUM]={
{&MI_UM_6_ID74_VSD4_CorrectionValue[2],0,0,0,(unsigned short *)MenuStr_Head_Space_Name[0],MenuStr_Head_Space_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,0,utNone},
{&MI_UM_6_ID74_VSD4_CorrectionValue[2],1,0,0,(unsigned short *)MenuStr_Head_Space_Name[1],MenuStr_Head_Space_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_UM_6_ID74_VSD4_CorrectionValue[2],2,0,0,(unsigned short *)MenuStr_Head_Space_Name[2],MenuStr_Head_Space_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2,utNone},
{&MI_UM_6_ID74_VSD4_CorrectionValue[2],3,0,0,(unsigned short *)MenuStr_Head_Space_Name[3],MenuStr_Head_Space_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3,utNone},
{&MI_UM_6_ID74_VSD4_CorrectionValue[2],4,0,0,(unsigned short *)MenuStr_Head_Space_Name[4],MenuStr_Head_Space_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,100,utNone},
{&MI_UM_6_ID74_VSD4_CorrectionValue[2],5,0,0,(unsigned short *)MenuStr_Head_Space_Name[5],MenuStr_Head_Space_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,101,utNone},
{&MI_UM_6_ID74_VSD4_CorrectionValue[2],6,0,0,(unsigned short *)MenuStr_Head_Space_Name[6],MenuStr_Head_Space_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,102,utNone},
{&MI_UM_6_ID74_VSD4_CorrectionValue[2],7,0,0,(unsigned short *)MenuStr_Head_Space_Name[7],MenuStr_Head_Space_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,103,utNone}
};



// 头间距
struct MenuItem MI_UM_3_ID3E_Head_Space[4]={
{&MI_UM_1_ID25_ADJUSTMENT[5],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",MI_UM_6_ID74_VSD1_CorrectionValue,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[5],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_UM_6_ID74_VSD2_CorrectionValue,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[5],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",MI_UM_6_ID74_VSD3_CorrectionValue,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[5],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",MI_UM_6_ID74_VSD4_CorrectionValue,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone}
};

extern struct MenuItem MI_EM_3_ID3E_Head_Space[4];

// VSD1
struct MenuItem MI_EM_6_ID74_VSD1_CorrectionValue[3]={
{&MI_EM_3_ID3E_Head_Space[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_Print_GroupSpace,0,utNone},
{&MI_EM_3_ID3E_Head_Space[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,0,tInt,3,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_EM_3_ID3E_Head_Space[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID74_CorrectionValue_VSD1,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};
// 校正值
struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD1[MAX_HEAD_NUM]={
{&MI_EM_6_ID74_VSD1_CorrectionValue[2],0,0,0,(unsigned short *)MenuStr_Head_Space_Name[0],MenuStr_Head_Space_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,0,utNone},
{&MI_EM_6_ID74_VSD1_CorrectionValue[2],1,0,0,(unsigned short *)MenuStr_Head_Space_Name[1],MenuStr_Head_Space_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_EM_6_ID74_VSD1_CorrectionValue[2],2,0,0,(unsigned short *)MenuStr_Head_Space_Name[2],MenuStr_Head_Space_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2,utNone},
{&MI_EM_6_ID74_VSD1_CorrectionValue[2],3,0,0,(unsigned short *)MenuStr_Head_Space_Name[3],MenuStr_Head_Space_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3,utNone},
{&MI_EM_6_ID74_VSD1_CorrectionValue[2],4,0,0,(unsigned short *)MenuStr_Head_Space_Name[4],MenuStr_Head_Space_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,100,utNone},
{&MI_EM_6_ID74_VSD1_CorrectionValue[2],5,0,0,(unsigned short *)MenuStr_Head_Space_Name[5],MenuStr_Head_Space_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,101,utNone},
{&MI_EM_6_ID74_VSD1_CorrectionValue[2],6,0,0,(unsigned short *)MenuStr_Head_Space_Name[6],MenuStr_Head_Space_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,102,utNone},
{&MI_EM_6_ID74_VSD1_CorrectionValue[2],7,0,0,(unsigned short *)MenuStr_Head_Space_Name[7],MenuStr_Head_Space_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,103,utNone}
};

// VSD2
struct MenuItem MI_EM_6_ID74_VSD2_CorrectionValue[3]={
{&MI_EM_3_ID3E_Head_Space[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_Print_GroupSpace,1,utNone},
{&MI_EM_3_ID3E_Head_Space[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,0,tInt,3,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1001,utNone},
{&MI_EM_3_ID3E_Head_Space[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID74_CorrectionValue_VSD2,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};
// 校正值
struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD2[MAX_HEAD_NUM]={
{&MI_EM_6_ID74_VSD2_CorrectionValue[2],0,0,0,(unsigned short *)MenuStr_Head_Space_Name[0],MenuStr_Head_Space_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,0,utNone},
{&MI_EM_6_ID74_VSD2_CorrectionValue[2],1,0,0,(unsigned short *)MenuStr_Head_Space_Name[1],MenuStr_Head_Space_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_EM_6_ID74_VSD2_CorrectionValue[2],2,0,0,(unsigned short *)MenuStr_Head_Space_Name[2],MenuStr_Head_Space_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2,utNone},
{&MI_EM_6_ID74_VSD2_CorrectionValue[2],3,0,0,(unsigned short *)MenuStr_Head_Space_Name[3],MenuStr_Head_Space_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3,utNone},
{&MI_EM_6_ID74_VSD2_CorrectionValue[2],4,0,0,(unsigned short *)MenuStr_Head_Space_Name[4],MenuStr_Head_Space_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,100,utNone},
{&MI_EM_6_ID74_VSD2_CorrectionValue[2],5,0,0,(unsigned short *)MenuStr_Head_Space_Name[5],MenuStr_Head_Space_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,101,utNone},
{&MI_EM_6_ID74_VSD2_CorrectionValue[2],6,0,0,(unsigned short *)MenuStr_Head_Space_Name[6],MenuStr_Head_Space_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,102,utNone},
{&MI_EM_6_ID74_VSD2_CorrectionValue[2],7,0,0,(unsigned short *)MenuStr_Head_Space_Name[7],MenuStr_Head_Space_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,103,utNone}
};

// VSD3
struct MenuItem MI_EM_6_ID74_VSD3_CorrectionValue[3]={
{&MI_EM_3_ID3E_Head_Space[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_Print_GroupSpace,2,utNone},
{&MI_EM_3_ID3E_Head_Space[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,0,tInt,3,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2001,utNone},
{&MI_EM_3_ID3E_Head_Space[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID74_CorrectionValue_VSD3,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};
// 校正值
struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD3[MAX_HEAD_NUM]={
{&MI_EM_6_ID74_VSD3_CorrectionValue[2],0,0,0,(unsigned short *)MenuStr_Head_Space_Name[0],MenuStr_Head_Space_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,0,utNone},
{&MI_EM_6_ID74_VSD3_CorrectionValue[2],1,0,0,(unsigned short *)MenuStr_Head_Space_Name[1],MenuStr_Head_Space_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_EM_6_ID74_VSD3_CorrectionValue[2],2,0,0,(unsigned short *)MenuStr_Head_Space_Name[2],MenuStr_Head_Space_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2,utNone},
{&MI_EM_6_ID74_VSD3_CorrectionValue[2],3,0,0,(unsigned short *)MenuStr_Head_Space_Name[3],MenuStr_Head_Space_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3,utNone},
{&MI_EM_6_ID74_VSD3_CorrectionValue[2],4,0,0,(unsigned short *)MenuStr_Head_Space_Name[4],MenuStr_Head_Space_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,100,utNone},
{&MI_EM_6_ID74_VSD3_CorrectionValue[2],5,0,0,(unsigned short *)MenuStr_Head_Space_Name[5],MenuStr_Head_Space_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,101,utNone},
{&MI_EM_6_ID74_VSD3_CorrectionValue[2],6,0,0,(unsigned short *)MenuStr_Head_Space_Name[6],MenuStr_Head_Space_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,102,utNone},
{&MI_EM_6_ID74_VSD3_CorrectionValue[2],7,0,0,(unsigned short *)MenuStr_Head_Space_Name[7],MenuStr_Head_Space_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,103,utNone}
};

// VSD4
struct MenuItem MI_EM_6_ID74_VSD4_CorrectionValue[3]={
{&MI_EM_3_ID3E_Head_Space[3],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_Print_GroupSpace,3,utNone},
{&MI_EM_3_ID3E_Head_Space[3],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,0,tInt,3,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3001,utNone},
{&MI_EM_3_ID3E_Head_Space[3],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID74_CorrectionValue_VSD4,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};
// 校正值
struct MenuItem MI_EM_6_ID74_CorrectionValue_VSD4[MAX_HEAD_NUM]={
{&MI_EM_6_ID74_VSD4_CorrectionValue[2],0,0,0,(unsigned short *)MenuStr_Head_Space_Name[0],MenuStr_Head_Space_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,0,utNone},
{&MI_EM_6_ID74_VSD4_CorrectionValue[2],1,0,0,(unsigned short *)MenuStr_Head_Space_Name[1],MenuStr_Head_Space_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,1,utNone},
{&MI_EM_6_ID74_VSD4_CorrectionValue[2],2,0,0,(unsigned short *)MenuStr_Head_Space_Name[2],MenuStr_Head_Space_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,2,utNone},
{&MI_EM_6_ID74_VSD4_CorrectionValue[2],3,0,0,(unsigned short *)MenuStr_Head_Space_Name[3],MenuStr_Head_Space_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,3,utNone},
{&MI_EM_6_ID74_VSD4_CorrectionValue[2],4,0,0,(unsigned short *)MenuStr_Head_Space_Name[4],MenuStr_Head_Space_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,100,utNone},
{&MI_EM_6_ID74_VSD4_CorrectionValue[2],5,0,0,(unsigned short *)MenuStr_Head_Space_Name[5],MenuStr_Head_Space_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,101,utNone},
{&MI_EM_6_ID74_VSD4_CorrectionValue[2],6,0,0,(unsigned short *)MenuStr_Head_Space_Name[6],MenuStr_Head_Space_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,102,utNone},
{&MI_EM_6_ID74_VSD4_CorrectionValue[2],7,0,0,(unsigned short *)MenuStr_Head_Space_Name[7],MenuStr_Head_Space_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_Get_GroupSpace,(UISetFloatFuncExType)Cali_Set_GroupSpace,0,103,utNone}
};

// EM 头间距
struct MenuItem MI_EM_3_ID3E_Head_Space[4]={
{&MI_EM_1_ID4B_ADJUSTMENT[5],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",MI_EM_6_ID74_VSD1_CorrectionValue,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[5],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_EM_6_ID74_VSD2_CorrectionValue,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[5],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",MI_EM_6_ID74_VSD3_CorrectionValue,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[5],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",MI_EM_6_ID74_VSD4_CorrectionValue,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone}
};

// 校正值
struct MenuItem MI_EM_3_ID25_CorrectionValue[MAX_COLOR_NUM]={
{&MI_EM_2_ID25_Verticalalignment[1],0,0,0,(unsigned short *)MenuStr_Color_Name[0],MenuStr_Color_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,0,utNone},
{&MI_EM_2_ID25_Verticalalignment[1],1,0,0,(unsigned short *)MenuStr_Color_Name[1],MenuStr_Color_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,1,utNone},
{&MI_EM_2_ID25_Verticalalignment[1],2,0,0,(unsigned short *)MenuStr_Color_Name[2],MenuStr_Color_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,2,utNone},
{&MI_EM_2_ID25_Verticalalignment[1],3,0,0,(unsigned short *)MenuStr_Color_Name[3],MenuStr_Color_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,3,utNone},
{&MI_EM_2_ID25_Verticalalignment[1],4,0,0,(unsigned short *)MenuStr_Color_Name[4],MenuStr_Color_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,4,utNone},
{&MI_EM_2_ID25_Verticalalignment[1],5,0,0,(unsigned short *)MenuStr_Color_Name[5],MenuStr_Color_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,5,utNone},
{&MI_EM_2_ID25_Verticalalignment[1],6,0,0,(unsigned short *)MenuStr_Color_Name[6],MenuStr_Color_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,6,utNone},
{&MI_EM_2_ID25_Verticalalignment[1],7,0,0,(unsigned short *)MenuStr_Color_Name[7],MenuStr_Color_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,7,utNone}
};

// 垂直检查
struct MenuItem MI_EM_2_ID25_Verticalalignment[2]={
{&MI_UM_1_ID25_ADJUSTMENT[6],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Vertical,0,utNone},
{&MI_UM_1_ID25_ADJUSTMENT[6],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_3_ID25_CorrectionValue,MAX_COLOR_NUM,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 校准
struct MenuItem MI_UM_1_ID25_ADJUSTMENT[]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[3],0,0,0,(unsigned short *)MenuStr_EM_2_ID56_InterleaveCheck,"Inter Leave",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_InterleaveCheck,0,utNone},
{&MI_0_ID00_MenuChilds[3],1,0,0,(unsigned short *)MenuStr_EM_2_ID57_HeadSlantCheck,"Head Slant",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_HeadSlantCheck,1,utNone},
{&MI_0_ID00_MenuChilds[3],2,0,0,(unsigned short *)MenuStr_EM_2_ID58_HeadIntervalCheck,"Head Interval",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_HeadIntervalCheck,2,utNone},
#ifdef RIPSTAR_FLAT_EX
{&MI_0_ID00_MenuChilds[3],3,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjust",MI_UM_2_ID27_FEEDadjust,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
#else
{&MI_0_ID00_MenuChilds[3],3,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjust",MI_UM_2_ID27_FEEDadjust,7,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
#endif
{&MI_0_ID00_MenuChilds[3],4,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_UM_2_ID3D_BiDir,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
{&MI_0_ID00_MenuChilds[3],5,0,0,(unsigned short *)MenuStr_UM_2_ID3E_Head_Space,"Head Space",MI_UM_3_ID3E_Head_Space,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone}
, {&MI_0_ID00_MenuChilds[3],6,0,0,(unsigned short *)MenuStr_EM_2_ID1C5_Verticalalignment,"V Alignment",MI_EM_2_ID25_Verticalalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone}
#ifdef RIPSTAR_FLAT_EX
,{&MI_0_ID00_MenuChilds[3],7,0,0,(unsigned short *)MenuStr_2_ID0F_AdjSetting,"Adjust Set",MI_UM_2_ID27_Adjustsetting,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,7,utNone}
#endif
#else
{&MI_0_ID00_MenuChilds[3],0,0,0,(unsigned short *)MenuStr_EM_2_ID56_InterleaveCheck,"Interleave Check",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_InterleaveCheck,0,utNone},
{&MI_0_ID00_MenuChilds[3],1,0,0,(unsigned short *)MenuStr_EM_2_ID57_HeadSlantCheck,"Head Slant Check",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_HeadSlantCheck,1,utNone},
{&MI_0_ID00_MenuChilds[3],2,0,0,(unsigned short *)MenuStr_EM_2_ID58_HeadIntervalCheck,"Head Interval Check",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_HeadIntervalCheck,2,utNone},
#ifdef RIPSTAR_FLAT_EX
{&MI_0_ID00_MenuChilds[3],3,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjust",MI_UM_2_ID27_FEEDadjust,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
#else
{&MI_0_ID00_MenuChilds[3],3,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjust",MI_UM_2_ID27_FEEDadjust,7,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
#endif
{&MI_0_ID00_MenuChilds[3],4,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_UM_2_ID3D_BiDir,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
{&MI_0_ID00_MenuChilds[3],5,0,0,(unsigned short *)MenuStr_UM_2_ID3E_Head_Space,"Head Space",MI_UM_3_ID3E_Head_Space,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone}
, {&MI_0_ID00_MenuChilds[3],6,0,0,(unsigned short *)MenuStr_EM_2_ID1C5_Verticalalignment,"Vertical Alignment",MI_EM_2_ID25_Verticalalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone}
#ifdef RIPSTAR_FLAT_EX
,{&MI_0_ID00_MenuChilds[3],7,0,0,(unsigned short *)MenuStr_2_ID0F_AdjSetting,"Adjustment Setting",MI_UM_2_ID27_Adjustsetting,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,7,utNone}
#endif
#endif
};

// 喷嘴检查
struct MenuItem MI_EM_2_ID4C_CheckNozzle[4]={
{&MI_0_ID00_MenuChilds[1],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_CheckNozzle,0,utNone},
#if defined(HEAD_EPSON_GEN5)||defined(RICOH_VSD2)
{&MI_0_ID00_MenuChilds[1],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_CheckNozzle,1,utNone},
#else
{&MI_0_ID00_MenuChilds[1],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,(UIActionFuncExType)Cali_EM_Print_CheckNozzle,1,utNone},
#endif
{&MI_0_ID00_MenuChilds[1],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_CheckNozzle,2,utNone},
{&MI_0_ID00_MenuChilds[1],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_CheckNozzle,3,utNone}
};

// 点检查
struct MenuItem MI_EM_2_ID51_DotCheck[4]={
{&MI_EM_1_ID4B_ADJUSTMENT[0],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_DotCheck,0,utNone},
#if defined(HEAD_EPSON_GEN5)||defined(RICOH_VSD2)
{&MI_EM_1_ID4B_ADJUSTMENT[0],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_DotCheck,1,utNone},
#else
{&MI_EM_1_ID4B_ADJUSTMENT[0],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,(UIActionFuncExType)Cali_EM_Print_DotCheck,1,utNone},
#endif
{&MI_EM_1_ID4B_ADJUSTMENT[0],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_DotCheck,2,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[0],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_DotCheck,3,utNone}
};

// pass数
struct MenuItem MI_EM_3_ID236_PassNumber[8]={
{&MI_EM_2_ID59_Feed[0],0,0,0,(unsigned short *)MenuStr_EM_4_ID237_2,"2",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_PassNum,(UISetFloatFuncExType)Cali_EM_Set_Step_PassNum,0,0,utNone},
{&MI_EM_2_ID59_Feed[0],1,0,0,(unsigned short *)MenuStr_EM_4_ID237_3,"3",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_PassNum,(UISetFloatFuncExType)Cali_EM_Set_Step_PassNum,0,1,utNone},
{&MI_EM_2_ID59_Feed[0],2,0,0,(unsigned short *)MenuStr_EM_4_ID237_4,"4",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_PassNum,(UISetFloatFuncExType)Cali_EM_Set_Step_PassNum,0,2,utNone},
{&MI_EM_2_ID59_Feed[0],3,0,0,(unsigned short *)MenuStr_EM_4_ID238_6,"6",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_PassNum,(UISetFloatFuncExType)Cali_EM_Set_Step_PassNum,0,3,utNone},
{&MI_EM_2_ID59_Feed[0],4,0,0,(unsigned short *)MenuStr_EM_4_ID239_8,"8",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_PassNum,(UISetFloatFuncExType)Cali_EM_Set_Step_PassNum,0,4,utNone},
{&MI_EM_2_ID59_Feed[0],5,0,0,(unsigned short *)MenuStr_EM_4_ID23A_12,"12",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_PassNum,(UISetFloatFuncExType)Cali_EM_Set_Step_PassNum,0,5,utNone},
{&MI_EM_2_ID59_Feed[0],5,0,0,(unsigned short *)MenuStr_EM_4_ID23A_16,"16",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_PassNum,(UISetFloatFuncExType)Cali_EM_Set_Step_PassNum,0,6,utNone},
{&MI_EM_2_ID59_Feed[0],5,0,0,(unsigned short *)MenuStr_EM_4_ID23A_24,"24",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_PassNum,(UISetFloatFuncExType)Cali_EM_Set_Step_PassNum,0,7,utNone}
};

// 高光
struct MenuItem MI_EM_3_ID5A_Highglossyphotopaper[2]={
{&MI_EM_2_ID59_Feed[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Step,0,utNone},
{&MI_EM_2_ID59_Feed[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_Modify,(UISetFloatFuncExType)Cali_EM_Set_Step_Modify,0,0,utNone}
};

// 车贴
struct MenuItem MI_EM_3_ID5D_Vinly[2]={
{&MI_EM_2_ID59_Feed[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Step,1,utNone},
{&MI_EM_2_ID59_Feed[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_Modify,(UISetFloatFuncExType)Cali_EM_Set_Step_Modify,0,1,utNone}
};

// PP
struct MenuItem MI_EM_3_ID60_PP[2]={
{&MI_EM_2_ID59_Feed[3],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Step,2,utNone},
{&MI_EM_2_ID59_Feed[3],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_Modify,(UISetFloatFuncExType)Cali_EM_Set_Step_Modify,0,2,utNone}
};

// 灯片
struct MenuItem MI_EM_3_ID63_Glossyfilm[2]={
{&MI_EM_2_ID59_Feed[4],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Step,3,utNone},
{&MI_EM_2_ID59_Feed[4],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_Modify,(UISetFloatFuncExType)Cali_EM_Set_Step_Modify,0,3,utNone}
};

// 其他1
struct MenuItem MI_EM_3_ID66_Other1[2]={
{&MI_EM_2_ID59_Feed[5],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Step,4,utNone},
{&MI_EM_2_ID59_Feed[5],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_Modify,(UISetFloatFuncExType)Cali_EM_Set_Step_Modify,0,4,utNone}
};

// 其他2
struct MenuItem MI_EM_3_ID69_Other2[2]={
{&MI_EM_2_ID59_Feed[6],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Step,5,utNone},
{&MI_EM_2_ID59_Feed[6],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_Modify,(UISetFloatFuncExType)Cali_EM_Set_Step_Modify,0,5,utNone}
};

// 其他3
struct MenuItem MI_EM_3_ID6C_Other3[2]={
{&MI_EM_2_ID59_Feed[7],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Step,6,utNone},
{&MI_EM_2_ID59_Feed[7],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_Modify,(UISetFloatFuncExType)Cali_EM_Set_Step_Modify,0,6,utNone}
};

// 步进
#ifdef RIPSTAR_FLAT_EX
#ifdef MANUFACTURER_DYSS	
struct MenuItem MI_EM_2_ID59_Feed_EX[32]={
{&MI_EM_2_ID59_Feed[3],0,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 1",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,0,utNone},
{&MI_EM_2_ID59_Feed[3],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 2",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,1,utNone},
{&MI_EM_2_ID59_Feed[3],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 3",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,2,utNone},
{&MI_EM_2_ID59_Feed[3],3,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 4",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,3,utNone},
{&MI_EM_2_ID59_Feed[3],4,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 5",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,4,utNone},
{&MI_EM_2_ID59_Feed[3],5,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 6",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,5,utNone},
{&MI_EM_2_ID59_Feed[3],6,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 7",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,6,utNone},
{&MI_EM_2_ID59_Feed[3],7,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 8",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,7,utNone},
{&MI_EM_2_ID59_Feed[3],8,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 9",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,8,utNone},
{&MI_EM_2_ID59_Feed[3],9,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 10",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,9,utNone},
{&MI_EM_2_ID59_Feed[3],10,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 11",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,10,utNone},
{&MI_EM_2_ID59_Feed[3],11,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 12",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,11,utNone},
{&MI_EM_2_ID59_Feed[3],12,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 13",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,12,utNone},
{&MI_EM_2_ID59_Feed[3],13,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 14",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,13,utNone},
{&MI_EM_2_ID59_Feed[3],14,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 15",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,14,utNone},
{&MI_EM_2_ID59_Feed[3],15,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 16",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,15,utNone},
{&MI_EM_2_ID59_Feed[3],16,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 17",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,16,utNone},
{&MI_EM_2_ID59_Feed[3],17,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 18",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,17,utNone},
{&MI_EM_2_ID59_Feed[3],18,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 19",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,18,utNone},
{&MI_EM_2_ID59_Feed[3],19,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 20",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,19,utNone},
{&MI_EM_2_ID59_Feed[3],20,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 21",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,20,utNone},
{&MI_EM_2_ID59_Feed[3],21,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 22",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,21,utNone},
{&MI_EM_2_ID59_Feed[3],22,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 23",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,22,utNone},
{&MI_EM_2_ID59_Feed[3],23,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 24",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,23,utNone},
{&MI_EM_2_ID59_Feed[3],24,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 25",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,24,utNone},
{&MI_EM_2_ID59_Feed[3],25,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 26",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,25,utNone},
{&MI_EM_2_ID59_Feed[3],26,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 27",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,26,utNone},
{&MI_EM_2_ID59_Feed[3],27,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 28",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,27,utNone},
{&MI_EM_2_ID59_Feed[3],28,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 29",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,28,utNone},
{&MI_EM_2_ID59_Feed[3],29,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 30",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,29,utNone},
{&MI_EM_2_ID59_Feed[3],30,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 31",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,30,utNone},
{&MI_EM_2_ID59_Feed[3],31,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Region 32",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBaseY_CAL_EX,(UISetFloatFuncExType)Menu_InputBaseY_CAL_EX,0,31,utNone}

};
struct MenuItem MI_EM_2_ID59_Feed[4]={
{&MI_EM_1_ID4B_ADJUSTMENT[4],0,0,0,(unsigned short *)MenuStr_2_ID403_PassNumber,"Pass Number",MI_EM_3_ID236_PassNumber,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[4],1,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Step,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[4],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_Modify,(UISetFloatFuncExType)Cali_EM_Set_Step_Modify,0,0,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_1_ID4B_ADJUSTMENT[4],3,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjust EX",MI_EM_2_ID59_Feed_EX,32,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone}
#else
{&MI_EM_1_ID4B_ADJUSTMENT[4],3,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjustment EX",MI_EM_2_ID59_Feed_EX,32,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone}
#endif
};
#else
struct MenuItem MI_EM_2_ID59_Feed[3]={
{&MI_EM_1_ID4B_ADJUSTMENT[4],0,0,0,(unsigned short *)MenuStr_2_ID403_PassNumber,"Pass Number",MI_EM_3_ID236_PassNumber,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[4],1,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Step,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[4],2,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",0,0,chs,editor,normalStatus,2,tFloat,6,-9999.99,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_Modify,(UISetFloatFuncExType)Cali_EM_Set_Step_Modify,0,0,utNone}
};
#endif
#else
struct MenuItem MI_EM_2_ID59_Feed[8]={
{&MI_EM_1_ID4B_ADJUSTMENT[4],0,0,0,(unsigned short *)MenuStr_2_ID403_PassNumber,"Pass Number",MI_EM_3_ID236_PassNumber,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_1_ID4B_ADJUSTMENT[4],1,0,0,(unsigned short *)MenuStr_3_ID06_Highlight,"High Glossy",MI_EM_3_ID5A_Highglossyphotopaper,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#else
{&MI_EM_1_ID4B_ADJUSTMENT[4],1,0,0,(unsigned short *)MenuStr_3_ID06_Highlight,"High Glossy Photopaper",MI_EM_3_ID5A_Highglossyphotopaper,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#endif
{&MI_EM_1_ID4B_ADJUSTMENT[4],2,0,0,(unsigned short *)MenuStr_3_ID07_Carstickers,"Vinly",MI_EM_3_ID5D_Vinly,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[4],3,0,0,(unsigned short *)MenuStr_3_ID08_PP,"PP",MI_EM_3_ID60_PP,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[4],4,0,0,(unsigned short *)MenuStr_UM_3_ID31_glossyfilm,"Glossy Film",MI_EM_3_ID63_Glossyfilm,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[4],5,0,0,(unsigned short *)MenuStr_3_ID0A_Other1,"Other1",MI_EM_3_ID66_Other1,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[4],6,0,0,(unsigned short *)MenuStr_3_ID0B_Other2,"Other2",MI_EM_3_ID69_Other2,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[4],7,0,0,(unsigned short *)MenuStr_3_ID0C_Other3,"Other3",MI_EM_3_ID6C_Other3,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone}
};
#endif
// 校正值高
struct MenuItem MI_EM_6_ID74_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID72_leftalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,0,utNone},
{&MI_EM_5_ID72_leftalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1,utNone},
{&MI_EM_5_ID72_leftalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,2,utNone},
{&MI_EM_5_ID72_leftalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,3,utNone},
{&MI_EM_5_ID72_leftalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,4,utNone},
{&MI_EM_5_ID72_leftalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,5,utNone},
{&MI_EM_5_ID72_leftalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,6,utNone},
{&MI_EM_5_ID72_leftalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,7,utNone},
{&MI_EM_5_ID72_leftalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,8,utNone},
{&MI_EM_5_ID72_leftalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,9,utNone},
{&MI_EM_5_ID72_leftalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10,utNone},
{&MI_EM_5_ID72_leftalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11,utNone},
{&MI_EM_5_ID72_leftalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,12,utNone},
{&MI_EM_5_ID72_leftalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,13,utNone},
{&MI_EM_5_ID72_leftalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,14,utNone},
{&MI_EM_5_ID72_leftalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,15,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID72_leftalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,16,utNone},
{&MI_EM_5_ID72_leftalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,17,utNone},
{&MI_EM_5_ID72_leftalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,18,utNone},
{&MI_EM_5_ID72_leftalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,19,utNone},
{&MI_EM_5_ID72_leftalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,20,utNone},
{&MI_EM_5_ID72_leftalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,21,utNone},
{&MI_EM_5_ID72_leftalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,22,utNone},
{&MI_EM_5_ID72_leftalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,23,utNone},
{&MI_EM_5_ID72_leftalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,24,utNone},
{&MI_EM_5_ID72_leftalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,25,utNone},
{&MI_EM_5_ID72_leftalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,26,utNone},
{&MI_EM_5_ID72_leftalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,27,utNone},
{&MI_EM_5_ID72_leftalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,28,utNone},
{&MI_EM_5_ID72_leftalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,29,utNone},
{&MI_EM_5_ID72_leftalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,30,utNone},
{&MI_EM_5_ID72_leftalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,31,utNone}
#endif
};

// 校正值低
struct MenuItem MI_EM_6_ID75_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID72_leftalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10000,utNone},
{&MI_EM_5_ID72_leftalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10001,utNone},
{&MI_EM_5_ID72_leftalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10002,utNone},
{&MI_EM_5_ID72_leftalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10003,utNone},
{&MI_EM_5_ID72_leftalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10004,utNone},
{&MI_EM_5_ID72_leftalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10005,utNone},
{&MI_EM_5_ID72_leftalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10006,utNone},
{&MI_EM_5_ID72_leftalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10007,utNone},
{&MI_EM_5_ID72_leftalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10008,utNone},
{&MI_EM_5_ID72_leftalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10009,utNone},
{&MI_EM_5_ID72_leftalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10010,utNone},
{&MI_EM_5_ID72_leftalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10011,utNone},
{&MI_EM_5_ID72_leftalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10012,utNone},
{&MI_EM_5_ID72_leftalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10013,utNone},
{&MI_EM_5_ID72_leftalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10014,utNone},
{&MI_EM_5_ID72_leftalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10015,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID72_leftalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10016,utNone},
{&MI_EM_5_ID72_leftalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10017,utNone},
{&MI_EM_5_ID72_leftalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10018,utNone},
{&MI_EM_5_ID72_leftalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10019,utNone},
{&MI_EM_5_ID72_leftalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10020,utNone},
{&MI_EM_5_ID72_leftalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10021,utNone},
{&MI_EM_5_ID72_leftalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10022,utNone},
{&MI_EM_5_ID72_leftalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10023,utNone},
{&MI_EM_5_ID72_leftalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10024,utNone},
{&MI_EM_5_ID72_leftalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10025,utNone},
{&MI_EM_5_ID72_leftalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10026,utNone},
{&MI_EM_5_ID72_leftalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10027,utNone},
{&MI_EM_5_ID72_leftalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10028,utNone},
{&MI_EM_5_ID72_leftalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10029,utNone},
{&MI_EM_5_ID72_leftalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10030,utNone},
{&MI_EM_5_ID72_leftalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10031,utNone}
#endif
};
// 左对齐
struct MenuItem MI_EM_5_ID72_leftalignment[3]={
{&MI_EM_4_ID71_VSD1[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Left,0,utNone},
{&MI_EM_4_ID71_VSD1[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID74_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID71_VSD1[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID75_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 双向
struct MenuItem MI_EM_5_ID85_BiDir[3]={
{&MI_EM_4_ID71_VSD1[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_BiDir,0,utNone},
{&MI_EM_4_ID71_VSD1[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,0,utNone},
{&MI_EM_4_ID71_VSD1[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,100,utNone}
};

// 校正值高
struct MenuItem MI_EM_6_ID8A_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID88_rightalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,0,utNone},
{&MI_EM_5_ID88_rightalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1,utNone},
{&MI_EM_5_ID88_rightalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,2,utNone},
{&MI_EM_5_ID88_rightalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,3,utNone},
{&MI_EM_5_ID88_rightalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,4,utNone},
{&MI_EM_5_ID88_rightalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,5,utNone},
{&MI_EM_5_ID88_rightalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,6,utNone},
{&MI_EM_5_ID88_rightalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,7,utNone},
{&MI_EM_5_ID88_rightalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,8,utNone},
{&MI_EM_5_ID88_rightalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,9,utNone},
{&MI_EM_5_ID88_rightalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10,utNone},
{&MI_EM_5_ID88_rightalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11,utNone},
{&MI_EM_5_ID88_rightalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,12,utNone},
{&MI_EM_5_ID88_rightalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,13,utNone},
{&MI_EM_5_ID88_rightalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,14,utNone},
{&MI_EM_5_ID88_rightalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,15,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID88_rightalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,16,utNone},
{&MI_EM_5_ID88_rightalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,17,utNone},
{&MI_EM_5_ID88_rightalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,18,utNone},
{&MI_EM_5_ID88_rightalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,19,utNone},
{&MI_EM_5_ID88_rightalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,20,utNone},
{&MI_EM_5_ID88_rightalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,21,utNone},
{&MI_EM_5_ID88_rightalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,22,utNone},
{&MI_EM_5_ID88_rightalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,23,utNone},
{&MI_EM_5_ID88_rightalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,24,utNone},
{&MI_EM_5_ID88_rightalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,25,utNone},
{&MI_EM_5_ID88_rightalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,26,utNone},
{&MI_EM_5_ID88_rightalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,27,utNone},
{&MI_EM_5_ID88_rightalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,28,utNone},
{&MI_EM_5_ID88_rightalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,29,utNone},
{&MI_EM_5_ID88_rightalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,30,utNone},
{&MI_EM_5_ID88_rightalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,31,utNone}
#endif
};
// 校正值低
struct MenuItem MI_EM_6_ID8B_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID88_rightalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10000,utNone},
{&MI_EM_5_ID88_rightalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10001,utNone},
{&MI_EM_5_ID88_rightalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10002,utNone},
{&MI_EM_5_ID88_rightalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10003,utNone},
{&MI_EM_5_ID88_rightalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10004,utNone},
{&MI_EM_5_ID88_rightalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10005,utNone},
{&MI_EM_5_ID88_rightalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10006,utNone},
{&MI_EM_5_ID88_rightalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10007,utNone},
{&MI_EM_5_ID88_rightalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10008,utNone},
{&MI_EM_5_ID88_rightalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10009,utNone},
{&MI_EM_5_ID88_rightalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10010,utNone},
{&MI_EM_5_ID88_rightalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10011,utNone},
{&MI_EM_5_ID88_rightalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10012,utNone},
{&MI_EM_5_ID88_rightalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10013,utNone},
{&MI_EM_5_ID88_rightalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10014,utNone},
{&MI_EM_5_ID88_rightalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10015,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID88_rightalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10016,utNone},
{&MI_EM_5_ID88_rightalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10017,utNone},
{&MI_EM_5_ID88_rightalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10018,utNone},
{&MI_EM_5_ID88_rightalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10019,utNone},
{&MI_EM_5_ID88_rightalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10020,utNone},
{&MI_EM_5_ID88_rightalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10021,utNone},
{&MI_EM_5_ID88_rightalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10022,utNone},
{&MI_EM_5_ID88_rightalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10023,utNone},
{&MI_EM_5_ID88_rightalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10024,utNone},
{&MI_EM_5_ID88_rightalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10025,utNone},
{&MI_EM_5_ID88_rightalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10026,utNone},
{&MI_EM_5_ID88_rightalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10027,utNone},
{&MI_EM_5_ID88_rightalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10028,utNone},
{&MI_EM_5_ID88_rightalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10029,utNone},
{&MI_EM_5_ID88_rightalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10030,utNone},
{&MI_EM_5_ID88_rightalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10031,utNone}
#endif
};

// 右对齐
struct MenuItem MI_EM_5_ID88_rightalignment[3]={
{&MI_EM_4_ID71_VSD1[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Right,0,utNone},
{&MI_EM_4_ID71_VSD1[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID8A_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID71_VSD1[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID8B_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD1
struct MenuItem MI_EM_4_ID71_VSD1[3]={
{&MI_EM_3_ID70_360DPI[0],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID72_leftalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID70_360DPI[0],1,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_EM_5_ID85_BiDir,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID70_360DPI[0],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID88_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#else
{&MI_EM_3_ID70_360DPI[0],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID88_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#endif
};

// 校正值高
struct MenuItem MI_EM_6_ID9E_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID9C_leftalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,100,utNone},
{&MI_EM_5_ID9C_leftalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,101,utNone},
{&MI_EM_5_ID9C_leftalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,102,utNone},
{&MI_EM_5_ID9C_leftalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,103,utNone},
{&MI_EM_5_ID9C_leftalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,104,utNone},
{&MI_EM_5_ID9C_leftalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,105,utNone},
{&MI_EM_5_ID9C_leftalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,106,utNone},
{&MI_EM_5_ID9C_leftalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,107,utNone},
{&MI_EM_5_ID9C_leftalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,108,utNone},
{&MI_EM_5_ID9C_leftalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,109,utNone},
{&MI_EM_5_ID9C_leftalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,110,utNone},
{&MI_EM_5_ID9C_leftalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,111,utNone},
{&MI_EM_5_ID9C_leftalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,112,utNone},
{&MI_EM_5_ID9C_leftalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,113,utNone},
{&MI_EM_5_ID9C_leftalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,114,utNone},
{&MI_EM_5_ID9C_leftalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,115,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID9C_leftalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,116,utNone},
{&MI_EM_5_ID9C_leftalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,117,utNone},
{&MI_EM_5_ID9C_leftalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,118,utNone},
{&MI_EM_5_ID9C_leftalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,119,utNone},
{&MI_EM_5_ID9C_leftalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,120,utNone},
{&MI_EM_5_ID9C_leftalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,121,utNone},
{&MI_EM_5_ID9C_leftalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,122,utNone},
{&MI_EM_5_ID9C_leftalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,123,utNone},
{&MI_EM_5_ID9C_leftalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,124,utNone},
{&MI_EM_5_ID9C_leftalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,125,utNone},
{&MI_EM_5_ID9C_leftalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,126,utNone},
{&MI_EM_5_ID9C_leftalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,127,utNone},
{&MI_EM_5_ID9C_leftalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,128,utNone},
{&MI_EM_5_ID9C_leftalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,129,utNone},
{&MI_EM_5_ID9C_leftalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,130,utNone},
{&MI_EM_5_ID9C_leftalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,131,utNone}
#endif

};

// 校正值低
struct MenuItem MI_EM_6_ID9F_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID9C_leftalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10100,utNone},
{&MI_EM_5_ID9C_leftalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10101,utNone},
{&MI_EM_5_ID9C_leftalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10102,utNone},
{&MI_EM_5_ID9C_leftalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10103,utNone},
{&MI_EM_5_ID9C_leftalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10104,utNone},
{&MI_EM_5_ID9C_leftalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10105,utNone},
{&MI_EM_5_ID9C_leftalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10106,utNone},
{&MI_EM_5_ID9C_leftalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10107,utNone},
{&MI_EM_5_ID9C_leftalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10108,utNone},
{&MI_EM_5_ID9C_leftalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10109,utNone},
{&MI_EM_5_ID9C_leftalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10110,utNone},
{&MI_EM_5_ID9C_leftalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10111,utNone},
{&MI_EM_5_ID9C_leftalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10112,utNone},
{&MI_EM_5_ID9C_leftalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10113,utNone},
{&MI_EM_5_ID9C_leftalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10114,utNone},
{&MI_EM_5_ID9C_leftalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10115,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID9C_leftalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10116,utNone},
{&MI_EM_5_ID9C_leftalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10117,utNone},
{&MI_EM_5_ID9C_leftalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10118,utNone},
{&MI_EM_5_ID9C_leftalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10119,utNone},
{&MI_EM_5_ID9C_leftalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10120,utNone},
{&MI_EM_5_ID9C_leftalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10121,utNone},
{&MI_EM_5_ID9C_leftalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10122,utNone},
{&MI_EM_5_ID9C_leftalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10123,utNone},
{&MI_EM_5_ID9C_leftalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10124,utNone},
{&MI_EM_5_ID9C_leftalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10125,utNone},
{&MI_EM_5_ID9C_leftalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10126,utNone},
{&MI_EM_5_ID9C_leftalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10127,utNone},
{&MI_EM_5_ID9C_leftalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10128,utNone},
{&MI_EM_5_ID9C_leftalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10129,utNone},
{&MI_EM_5_ID9C_leftalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10130,utNone},
{&MI_EM_5_ID9C_leftalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10131,utNone}
#endif
};

// 左对齐
struct MenuItem MI_EM_5_ID9C_leftalignment[3]={
{&MI_EM_4_ID9B_VSD2[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Left,1,utNone},
{&MI_EM_4_ID9B_VSD2[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID9E_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID9B_VSD2[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID9F_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 双向
struct MenuItem MI_EM_5_IDAF_BiDir[3]={
{&MI_EM_4_ID9B_VSD2[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_BiDir,1,utNone},
{&MI_EM_4_ID9B_VSD2[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,1,utNone},
{&MI_EM_4_ID9B_VSD2[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,101,utNone}
};

// 校正值高
struct MenuItem MI_EM_6_IDB4_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_IDB2_rightalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,100,utNone},
{&MI_EM_5_IDB2_rightalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,101,utNone},
{&MI_EM_5_IDB2_rightalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,102,utNone},
{&MI_EM_5_IDB2_rightalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,103,utNone},
{&MI_EM_5_IDB2_rightalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,104,utNone},
{&MI_EM_5_IDB2_rightalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,105,utNone},
{&MI_EM_5_IDB2_rightalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,106,utNone},
{&MI_EM_5_IDB2_rightalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,107,utNone},
{&MI_EM_5_IDB2_rightalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,108,utNone},
{&MI_EM_5_IDB2_rightalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,109,utNone},
{&MI_EM_5_IDB2_rightalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,110,utNone},
{&MI_EM_5_IDB2_rightalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,111,utNone},
{&MI_EM_5_IDB2_rightalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,112,utNone},
{&MI_EM_5_IDB2_rightalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,113,utNone},
{&MI_EM_5_IDB2_rightalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,114,utNone},
{&MI_EM_5_IDB2_rightalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,115,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_IDB2_rightalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,116,utNone},
{&MI_EM_5_IDB2_rightalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,117,utNone},
{&MI_EM_5_IDB2_rightalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,118,utNone},
{&MI_EM_5_IDB2_rightalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,119,utNone},
{&MI_EM_5_IDB2_rightalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,120,utNone},
{&MI_EM_5_IDB2_rightalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,121,utNone},
{&MI_EM_5_IDB2_rightalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,122,utNone},
{&MI_EM_5_IDB2_rightalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,123,utNone},
{&MI_EM_5_IDB2_rightalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,124,utNone},
{&MI_EM_5_IDB2_rightalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,125,utNone},
{&MI_EM_5_IDB2_rightalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,126,utNone},
{&MI_EM_5_IDB2_rightalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,127,utNone},
{&MI_EM_5_IDB2_rightalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,128,utNone},
{&MI_EM_5_IDB2_rightalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,129,utNone},
{&MI_EM_5_IDB2_rightalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,130,utNone},
{&MI_EM_5_IDB2_rightalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,131,utNone}
#endif
};
// 校正值低
struct MenuItem MI_EM_6_IDB5_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_IDB2_rightalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10100,utNone},
{&MI_EM_5_IDB2_rightalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10101,utNone},
{&MI_EM_5_IDB2_rightalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10102,utNone},
{&MI_EM_5_IDB2_rightalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10103,utNone},
{&MI_EM_5_IDB2_rightalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10104,utNone},
{&MI_EM_5_IDB2_rightalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10105,utNone},
{&MI_EM_5_IDB2_rightalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10106,utNone},
{&MI_EM_5_IDB2_rightalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10107,utNone},
{&MI_EM_5_IDB2_rightalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10108,utNone},
{&MI_EM_5_IDB2_rightalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10109,utNone},
{&MI_EM_5_IDB2_rightalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10110,utNone},
{&MI_EM_5_IDB2_rightalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10111,utNone},
{&MI_EM_5_IDB2_rightalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10112,utNone},
{&MI_EM_5_IDB2_rightalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10113,utNone},
{&MI_EM_5_IDB2_rightalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10114,utNone},
{&MI_EM_5_IDB2_rightalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10115,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_IDB2_rightalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10116,utNone},
{&MI_EM_5_IDB2_rightalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10117,utNone},
{&MI_EM_5_IDB2_rightalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10118,utNone},
{&MI_EM_5_IDB2_rightalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10119,utNone},
{&MI_EM_5_IDB2_rightalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10120,utNone},
{&MI_EM_5_IDB2_rightalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10121,utNone},
{&MI_EM_5_IDB2_rightalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10122,utNone},
{&MI_EM_5_IDB2_rightalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10123,utNone},
{&MI_EM_5_IDB2_rightalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10124,utNone},
{&MI_EM_5_IDB2_rightalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10125,utNone},
{&MI_EM_5_IDB2_rightalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10126,utNone},
{&MI_EM_5_IDB2_rightalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10127,utNone},
{&MI_EM_5_IDB2_rightalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10128,utNone},
{&MI_EM_5_IDB2_rightalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10129,utNone},
{&MI_EM_5_IDB2_rightalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10130,utNone},
{&MI_EM_5_IDB2_rightalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10131,utNone}
#endif
};

// 右对齐
struct MenuItem MI_EM_5_IDB2_rightalignment[3]={
{&MI_EM_4_ID9B_VSD2[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Right,1,utNone},
{&MI_EM_4_ID9B_VSD2[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_IDB4_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID9B_VSD2[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_IDB5_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD2
struct MenuItem MI_EM_4_ID9B_VSD2[3]={
{&MI_EM_3_ID70_360DPI[1],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID9C_leftalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID70_360DPI[1],1,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_EM_5_IDAF_BiDir,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_3_ID70_360DPI[1],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_IDB2_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
};

// 校正值
struct MenuItem MI_EM_6_IDC8_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_IDC6_leftalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,200,utNone},
{&MI_EM_5_IDC6_leftalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,201,utNone},
{&MI_EM_5_IDC6_leftalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,202,utNone},
{&MI_EM_5_IDC6_leftalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,203,utNone},
{&MI_EM_5_IDC6_leftalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,204,utNone},
{&MI_EM_5_IDC6_leftalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,205,utNone},
{&MI_EM_5_IDC6_leftalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,206,utNone},
{&MI_EM_5_IDC6_leftalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,207,utNone},
{&MI_EM_5_IDC6_leftalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,208,utNone},
{&MI_EM_5_IDC6_leftalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,209,utNone},
{&MI_EM_5_IDC6_leftalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,210,utNone},
{&MI_EM_5_IDC6_leftalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,211,utNone},
{&MI_EM_5_IDC6_leftalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,212,utNone},
{&MI_EM_5_IDC6_leftalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,213,utNone},
{&MI_EM_5_IDC6_leftalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,214,utNone},
{&MI_EM_5_IDC6_leftalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,215,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_IDC6_leftalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,216,utNone},
{&MI_EM_5_IDC6_leftalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,217,utNone},
{&MI_EM_5_IDC6_leftalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,218,utNone},
{&MI_EM_5_IDC6_leftalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,219,utNone},
{&MI_EM_5_IDC6_leftalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,220,utNone},
{&MI_EM_5_IDC6_leftalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,221,utNone},
{&MI_EM_5_IDC6_leftalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,222,utNone},
{&MI_EM_5_IDC6_leftalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,223,utNone},
{&MI_EM_5_IDC6_leftalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,224,utNone},
{&MI_EM_5_IDC6_leftalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,225,utNone},
{&MI_EM_5_IDC6_leftalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,226,utNone},
{&MI_EM_5_IDC6_leftalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,227,utNone},
{&MI_EM_5_IDC6_leftalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,228,utNone},
{&MI_EM_5_IDC6_leftalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,229,utNone},
{&MI_EM_5_IDC6_leftalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,230,utNone},
{&MI_EM_5_IDC6_leftalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,231,utNone}
#endif
};

struct MenuItem MI_EM_6_IDC9_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_IDC6_leftalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10200,utNone},
{&MI_EM_5_IDC6_leftalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10201,utNone},
{&MI_EM_5_IDC6_leftalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10202,utNone},
{&MI_EM_5_IDC6_leftalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10203,utNone},
{&MI_EM_5_IDC6_leftalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10204,utNone},
{&MI_EM_5_IDC6_leftalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10205,utNone},
{&MI_EM_5_IDC6_leftalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10206,utNone},
{&MI_EM_5_IDC6_leftalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10207,utNone},
{&MI_EM_5_IDC6_leftalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10208,utNone},
{&MI_EM_5_IDC6_leftalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10209,utNone},
{&MI_EM_5_IDC6_leftalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10210,utNone},
{&MI_EM_5_IDC6_leftalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10211,utNone},
{&MI_EM_5_IDC6_leftalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10212,utNone},
{&MI_EM_5_IDC6_leftalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10213,utNone},
{&MI_EM_5_IDC6_leftalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10214,utNone},
{&MI_EM_5_IDC6_leftalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10215,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_IDC6_leftalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10216,utNone},
{&MI_EM_5_IDC6_leftalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10217,utNone},
{&MI_EM_5_IDC6_leftalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10218,utNone},
{&MI_EM_5_IDC6_leftalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10219,utNone},
{&MI_EM_5_IDC6_leftalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10220,utNone},
{&MI_EM_5_IDC6_leftalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10221,utNone},
{&MI_EM_5_IDC6_leftalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10222,utNone},
{&MI_EM_5_IDC6_leftalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10223,utNone},
{&MI_EM_5_IDC6_leftalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10224,utNone},
{&MI_EM_5_IDC6_leftalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10225,utNone},
{&MI_EM_5_IDC6_leftalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10226,utNone},
{&MI_EM_5_IDC6_leftalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10227,utNone},
{&MI_EM_5_IDC6_leftalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10228,utNone},
{&MI_EM_5_IDC6_leftalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10229,utNone},
{&MI_EM_5_IDC6_leftalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10230,utNone},
{&MI_EM_5_IDC6_leftalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10231,utNone}
#endif
};
// 左对齐
struct MenuItem MI_EM_5_IDC6_leftalignment[3]={
{&MI_EM_4_IDC5_VSD3[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Left,2,utNone},
{&MI_EM_4_IDC5_VSD3[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_IDC8_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_IDC5_VSD3[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_IDC9_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 双向
struct MenuItem MI_EM_5_IDD9_BiDir[3]={
{&MI_EM_4_IDC5_VSD3[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_BiDir,2,utNone},
{&MI_EM_4_IDC5_VSD3[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,2,utNone},
{&MI_EM_4_IDC5_VSD3[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,102,utNone}
};

// 校正值
struct MenuItem MI_EM_6_IDDE_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_IDDC_rightalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,200,utNone},
{&MI_EM_5_IDDC_rightalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,201,utNone},
{&MI_EM_5_IDDC_rightalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,202,utNone},
{&MI_EM_5_IDDC_rightalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,203,utNone},
{&MI_EM_5_IDDC_rightalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,204,utNone},
{&MI_EM_5_IDDC_rightalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,205,utNone},
{&MI_EM_5_IDDC_rightalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,206,utNone},
{&MI_EM_5_IDDC_rightalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,207,utNone},
{&MI_EM_5_IDDC_rightalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,208,utNone},
{&MI_EM_5_IDDC_rightalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,209,utNone},
{&MI_EM_5_IDDC_rightalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,210,utNone},
{&MI_EM_5_IDDC_rightalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,211,utNone},
{&MI_EM_5_IDDC_rightalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,212,utNone},
{&MI_EM_5_IDDC_rightalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,213,utNone},
{&MI_EM_5_IDDC_rightalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,214,utNone},
{&MI_EM_5_IDDC_rightalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,215,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_IDDC_rightalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,216,utNone},
{&MI_EM_5_IDDC_rightalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,217,utNone},
{&MI_EM_5_IDDC_rightalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,218,utNone},
{&MI_EM_5_IDDC_rightalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,219,utNone},
{&MI_EM_5_IDDC_rightalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,220,utNone},
{&MI_EM_5_IDDC_rightalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,221,utNone},
{&MI_EM_5_IDDC_rightalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,222,utNone},
{&MI_EM_5_IDDC_rightalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,223,utNone},
{&MI_EM_5_IDDC_rightalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,224,utNone},
{&MI_EM_5_IDDC_rightalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,225,utNone},
{&MI_EM_5_IDDC_rightalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,226,utNone},
{&MI_EM_5_IDDC_rightalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,227,utNone},
{&MI_EM_5_IDDC_rightalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,228,utNone},
{&MI_EM_5_IDDC_rightalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,229,utNone},
{&MI_EM_5_IDDC_rightalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,230,utNone},
{&MI_EM_5_IDDC_rightalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,231,utNone}
#endif

};
// 校正值
struct MenuItem MI_EM_6_IDDF_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_IDDC_rightalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10200,utNone},
{&MI_EM_5_IDDC_rightalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10201,utNone},
{&MI_EM_5_IDDC_rightalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10202,utNone},
{&MI_EM_5_IDDC_rightalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10203,utNone},
{&MI_EM_5_IDDC_rightalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10204,utNone},
{&MI_EM_5_IDDC_rightalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10205,utNone},
{&MI_EM_5_IDDC_rightalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10206,utNone},
{&MI_EM_5_IDDC_rightalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10207,utNone},
{&MI_EM_5_IDDC_rightalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10208,utNone},
{&MI_EM_5_IDDC_rightalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10209,utNone},
{&MI_EM_5_IDDC_rightalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10210,utNone},
{&MI_EM_5_IDDC_rightalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10211,utNone},
{&MI_EM_5_IDDC_rightalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10212,utNone},
{&MI_EM_5_IDDC_rightalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10213,utNone},
{&MI_EM_5_IDDC_rightalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10214,utNone},
{&MI_EM_5_IDDC_rightalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10215,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_IDDC_rightalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10216,utNone},
{&MI_EM_5_IDDC_rightalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10217,utNone},
{&MI_EM_5_IDDC_rightalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10218,utNone},
{&MI_EM_5_IDDC_rightalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10219,utNone},
{&MI_EM_5_IDDC_rightalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10220,utNone},
{&MI_EM_5_IDDC_rightalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10221,utNone},
{&MI_EM_5_IDDC_rightalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10222,utNone},
{&MI_EM_5_IDDC_rightalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10223,utNone},
{&MI_EM_5_IDDC_rightalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10224,utNone},
{&MI_EM_5_IDDC_rightalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10225,utNone},
{&MI_EM_5_IDDC_rightalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10226,utNone},
{&MI_EM_5_IDDC_rightalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10227,utNone},
{&MI_EM_5_IDDC_rightalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10228,utNone},
{&MI_EM_5_IDDC_rightalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10229,utNone},
{&MI_EM_5_IDDC_rightalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10230,utNone},
{&MI_EM_5_IDDC_rightalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10231,utNone}
#endif

};
// 右对齐
struct MenuItem MI_EM_5_IDDC_rightalignment[3]={
{&MI_EM_4_IDC5_VSD3[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Right,2,utNone},
{&MI_EM_4_IDC5_VSD3[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_IDDE_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_IDC5_VSD3[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_IDDF_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD3
struct MenuItem MI_EM_4_IDC5_VSD3[3]={
{&MI_EM_3_ID70_360DPI[2],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_IDC6_leftalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID70_360DPI[2],1,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_EM_5_IDD9_BiDir,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID70_360DPI[2],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_IDDC_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#else
{&MI_EM_3_ID70_360DPI[2],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_IDDC_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#endif
};

// 校正值高
struct MenuItem MI_EM_6_IDF2_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_IDF0_leftalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,300,utNone},
{&MI_EM_5_IDF0_leftalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,301,utNone},
{&MI_EM_5_IDF0_leftalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,302,utNone},
{&MI_EM_5_IDF0_leftalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,303,utNone},
{&MI_EM_5_IDF0_leftalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,304,utNone},
{&MI_EM_5_IDF0_leftalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,305,utNone},
{&MI_EM_5_IDF0_leftalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,306,utNone},
{&MI_EM_5_IDF0_leftalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,307,utNone},
{&MI_EM_5_IDF0_leftalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,308,utNone},
{&MI_EM_5_IDF0_leftalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,309,utNone},
{&MI_EM_5_IDF0_leftalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,310,utNone},
{&MI_EM_5_IDF0_leftalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,311,utNone},
{&MI_EM_5_IDF0_leftalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,312,utNone},
{&MI_EM_5_IDF0_leftalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,313,utNone},
{&MI_EM_5_IDF0_leftalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,314,utNone},
{&MI_EM_5_IDF0_leftalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,315,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_IDF0_leftalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,316,utNone},
{&MI_EM_5_IDF0_leftalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,317,utNone},
{&MI_EM_5_IDF0_leftalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,318,utNone},
{&MI_EM_5_IDF0_leftalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,319,utNone},
{&MI_EM_5_IDF0_leftalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,320,utNone},
{&MI_EM_5_IDF0_leftalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,321,utNone},
{&MI_EM_5_IDF0_leftalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,322,utNone},
{&MI_EM_5_IDF0_leftalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,323,utNone},
{&MI_EM_5_IDF0_leftalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,324,utNone},
{&MI_EM_5_IDF0_leftalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,325,utNone},
{&MI_EM_5_IDF0_leftalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,326,utNone},
{&MI_EM_5_IDF0_leftalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,327,utNone},
{&MI_EM_5_IDF0_leftalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,328,utNone},
{&MI_EM_5_IDF0_leftalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,329,utNone},
{&MI_EM_5_IDF0_leftalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,330,utNone},
{&MI_EM_5_IDF0_leftalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,331,utNone}
#endif

};

// 校正值低
struct MenuItem MI_EM_6_IDF3_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_IDF0_leftalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10300,utNone},
{&MI_EM_5_IDF0_leftalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10301,utNone},
{&MI_EM_5_IDF0_leftalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10302,utNone},
{&MI_EM_5_IDF0_leftalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10303,utNone},
{&MI_EM_5_IDF0_leftalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10304,utNone},
{&MI_EM_5_IDF0_leftalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10305,utNone},
{&MI_EM_5_IDF0_leftalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10306,utNone},
{&MI_EM_5_IDF0_leftalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10307,utNone},
{&MI_EM_5_IDF0_leftalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10308,utNone},
{&MI_EM_5_IDF0_leftalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10309,utNone},
{&MI_EM_5_IDF0_leftalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10310,utNone},
{&MI_EM_5_IDF0_leftalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10311,utNone},
{&MI_EM_5_IDF0_leftalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10312,utNone},
{&MI_EM_5_IDF0_leftalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10313,utNone},
{&MI_EM_5_IDF0_leftalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10314,utNone},
{&MI_EM_5_IDF0_leftalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10315,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_IDF0_leftalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10316,utNone},
{&MI_EM_5_IDF0_leftalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10317,utNone},
{&MI_EM_5_IDF0_leftalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10318,utNone},
{&MI_EM_5_IDF0_leftalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10319,utNone},
{&MI_EM_5_IDF0_leftalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10320,utNone},
{&MI_EM_5_IDF0_leftalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10321,utNone},
{&MI_EM_5_IDF0_leftalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10322,utNone},
{&MI_EM_5_IDF0_leftalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10323,utNone},
{&MI_EM_5_IDF0_leftalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10324,utNone},
{&MI_EM_5_IDF0_leftalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10325,utNone},
{&MI_EM_5_IDF0_leftalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10326,utNone},
{&MI_EM_5_IDF0_leftalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10327,utNone},
{&MI_EM_5_IDF0_leftalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10328,utNone},
{&MI_EM_5_IDF0_leftalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10329,utNone},
{&MI_EM_5_IDF0_leftalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10330,utNone},
{&MI_EM_5_IDF0_leftalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,10331,utNone}
#endif

};
// 左对齐
struct MenuItem MI_EM_5_IDF0_leftalignment[3]={
{&MI_EM_4_IDEF_VSD4[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Left,3,utNone},
{&MI_EM_4_IDEF_VSD4[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_IDF2_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_IDEF_VSD4[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_IDF3_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 双向
struct MenuItem MI_EM_5_ID103_BiDir[3]={
{&MI_EM_4_IDEF_VSD4[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_BiDir,3,utNone},
{&MI_EM_4_IDEF_VSD4[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,3,utNone},
{&MI_EM_4_IDEF_VSD4[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,103,utNone}
};

// 校正值高
struct MenuItem MI_EM_6_ID108_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID106_rightalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,300,utNone},
{&MI_EM_5_ID106_rightalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,301,utNone},
{&MI_EM_5_ID106_rightalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,302,utNone},
{&MI_EM_5_ID106_rightalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,303,utNone},
{&MI_EM_5_ID106_rightalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,304,utNone},
{&MI_EM_5_ID106_rightalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,305,utNone},
{&MI_EM_5_ID106_rightalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,306,utNone},
{&MI_EM_5_ID106_rightalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,307,utNone},
{&MI_EM_5_ID106_rightalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,308,utNone},
{&MI_EM_5_ID106_rightalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,309,utNone},
{&MI_EM_5_ID106_rightalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,310,utNone},
{&MI_EM_5_ID106_rightalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,311,utNone},
{&MI_EM_5_ID106_rightalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,312,utNone},
{&MI_EM_5_ID106_rightalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,313,utNone},
{&MI_EM_5_ID106_rightalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,314,utNone},
{&MI_EM_5_ID106_rightalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,315,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID106_rightalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,316,utNone},
{&MI_EM_5_ID106_rightalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,317,utNone},
{&MI_EM_5_ID106_rightalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,318,utNone},
{&MI_EM_5_ID106_rightalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,319,utNone},
{&MI_EM_5_ID106_rightalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,320,utNone},
{&MI_EM_5_ID106_rightalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,321,utNone},
{&MI_EM_5_ID106_rightalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,322,utNone},
{&MI_EM_5_ID106_rightalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,323,utNone},
{&MI_EM_5_ID106_rightalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,324,utNone},
{&MI_EM_5_ID106_rightalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,325,utNone},
{&MI_EM_5_ID106_rightalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,326,utNone},
{&MI_EM_5_ID106_rightalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,327,utNone},
{&MI_EM_5_ID106_rightalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,328,utNone},
{&MI_EM_5_ID106_rightalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,329,utNone},
{&MI_EM_5_ID106_rightalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,330,utNone},
{&MI_EM_5_ID106_rightalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,331,utNone}
#endif
} ;

// 校正值低
struct MenuItem MI_EM_6_ID109_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID106_rightalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10300,utNone},
{&MI_EM_5_ID106_rightalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10301,utNone},
{&MI_EM_5_ID106_rightalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10302,utNone},
{&MI_EM_5_ID106_rightalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10303,utNone},
{&MI_EM_5_ID106_rightalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10304,utNone},
{&MI_EM_5_ID106_rightalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10305,utNone},
{&MI_EM_5_ID106_rightalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10306,utNone},
{&MI_EM_5_ID106_rightalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10307,utNone},
{&MI_EM_5_ID106_rightalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10308,utNone},
{&MI_EM_5_ID106_rightalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10309,utNone},
{&MI_EM_5_ID106_rightalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10310,utNone},
{&MI_EM_5_ID106_rightalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10311,utNone},
{&MI_EM_5_ID106_rightalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10312,utNone},
{&MI_EM_5_ID106_rightalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10313,utNone},
{&MI_EM_5_ID106_rightalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10314,utNone},
{&MI_EM_5_ID106_rightalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10315,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID106_rightalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10316,utNone},
{&MI_EM_5_ID106_rightalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10317,utNone},
{&MI_EM_5_ID106_rightalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10318,utNone},
{&MI_EM_5_ID106_rightalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10319,utNone},
{&MI_EM_5_ID106_rightalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10320,utNone},
{&MI_EM_5_ID106_rightalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10321,utNone},
{&MI_EM_5_ID106_rightalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10322,utNone},
{&MI_EM_5_ID106_rightalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10323,utNone},
{&MI_EM_5_ID106_rightalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10324,utNone},
{&MI_EM_5_ID106_rightalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10325,utNone},
{&MI_EM_5_ID106_rightalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10326,utNone},
{&MI_EM_5_ID106_rightalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10327,utNone},
{&MI_EM_5_ID106_rightalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10328,utNone},
{&MI_EM_5_ID106_rightalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10329,utNone},
{&MI_EM_5_ID106_rightalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10330,utNone},
{&MI_EM_5_ID106_rightalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,10331,utNone}
#endif
} ;
// 右对齐
struct MenuItem MI_EM_5_ID106_rightalignment[3]={
{&MI_EM_4_IDEF_VSD4[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Right,3,utNone},
{&MI_EM_4_IDEF_VSD4[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID108_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_IDEF_VSD4[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID109_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD4
struct MenuItem MI_EM_4_IDEF_VSD4[3]={
{&MI_EM_3_ID70_360DPI[3],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_IDF0_leftalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID70_360DPI[3],1,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_EM_5_ID103_BiDir,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID70_360DPI[3],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID106_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#else
{&MI_EM_3_ID70_360DPI[3],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID106_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#endif
};

// 360DPI
struct MenuItem MI_EM_3_ID70_360DPI[4]={
{&MI_EM_2_ID6F_Horizotalalignment[0],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",MI_EM_4_ID71_VSD1,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#if defined(HEAD_EPSON_GEN5)||defined(RICOH_VSD2)
{&MI_EM_2_ID6F_Horizotalalignment[0],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_EM_4_ID9B_VSD2,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#else
{&MI_EM_2_ID6F_Horizotalalignment[0],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_EM_4_ID9B_VSD2,3,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,1,utNone},
#endif
{&MI_EM_2_ID6F_Horizotalalignment[0],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",MI_EM_4_IDC5_VSD3,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_EM_2_ID6F_Horizotalalignment[0],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",MI_EM_4_IDEF_VSD4,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone}
};

// 校正值高
struct MenuItem MI_EM_6_ID11D_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID11B_leftalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1000,utNone},
{&MI_EM_5_ID11B_leftalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1001,utNone},
{&MI_EM_5_ID11B_leftalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1002,utNone},
{&MI_EM_5_ID11B_leftalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1003,utNone},
{&MI_EM_5_ID11B_leftalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1004,utNone},
{&MI_EM_5_ID11B_leftalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1005,utNone},
{&MI_EM_5_ID11B_leftalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1006,utNone},
{&MI_EM_5_ID11B_leftalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1007,utNone},
{&MI_EM_5_ID11B_leftalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1008,utNone},
{&MI_EM_5_ID11B_leftalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1009,utNone},
{&MI_EM_5_ID11B_leftalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1010,utNone},
{&MI_EM_5_ID11B_leftalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1011,utNone},
{&MI_EM_5_ID11B_leftalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1012,utNone},
{&MI_EM_5_ID11B_leftalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1013,utNone},
{&MI_EM_5_ID11B_leftalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1014,utNone},
{&MI_EM_5_ID11B_leftalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1015,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID11B_leftalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1016,utNone},
{&MI_EM_5_ID11B_leftalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1017,utNone},
{&MI_EM_5_ID11B_leftalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1018,utNone},
{&MI_EM_5_ID11B_leftalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1019,utNone},
{&MI_EM_5_ID11B_leftalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1020,utNone},
{&MI_EM_5_ID11B_leftalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1021,utNone},
{&MI_EM_5_ID11B_leftalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1022,utNone},
{&MI_EM_5_ID11B_leftalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1023,utNone},
{&MI_EM_5_ID11B_leftalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1024,utNone},
{&MI_EM_5_ID11B_leftalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1025,utNone},
{&MI_EM_5_ID11B_leftalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1026,utNone},
{&MI_EM_5_ID11B_leftalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1027,utNone},
{&MI_EM_5_ID11B_leftalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1028,utNone},
{&MI_EM_5_ID11B_leftalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1029,utNone},
{&MI_EM_5_ID11B_leftalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1030,utNone},
{&MI_EM_5_ID11B_leftalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1031,utNone}
#endif
};
// 校正值低
struct MenuItem MI_EM_6_ID11E_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID11B_leftalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11000,utNone},
{&MI_EM_5_ID11B_leftalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11001,utNone},
{&MI_EM_5_ID11B_leftalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11002,utNone},
{&MI_EM_5_ID11B_leftalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11003,utNone},
{&MI_EM_5_ID11B_leftalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11004,utNone},
{&MI_EM_5_ID11B_leftalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11005,utNone},
{&MI_EM_5_ID11B_leftalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11006,utNone},
{&MI_EM_5_ID11B_leftalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11007,utNone},
{&MI_EM_5_ID11B_leftalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11008,utNone},
{&MI_EM_5_ID11B_leftalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11009,utNone},
{&MI_EM_5_ID11B_leftalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11010,utNone},
{&MI_EM_5_ID11B_leftalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11011,utNone},
{&MI_EM_5_ID11B_leftalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11012,utNone},
{&MI_EM_5_ID11B_leftalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11013,utNone},
{&MI_EM_5_ID11B_leftalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11014,utNone},
{&MI_EM_5_ID11B_leftalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11015,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID11B_leftalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11016,utNone},
{&MI_EM_5_ID11B_leftalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11017,utNone},
{&MI_EM_5_ID11B_leftalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11018,utNone},
{&MI_EM_5_ID11B_leftalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11019,utNone},
{&MI_EM_5_ID11B_leftalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11020,utNone},
{&MI_EM_5_ID11B_leftalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11021,utNone},
{&MI_EM_5_ID11B_leftalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11022,utNone},
{&MI_EM_5_ID11B_leftalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11023,utNone},
{&MI_EM_5_ID11B_leftalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11024,utNone},
{&MI_EM_5_ID11B_leftalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11025,utNone},
{&MI_EM_5_ID11B_leftalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11026,utNone},
{&MI_EM_5_ID11B_leftalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11027,utNone},
{&MI_EM_5_ID11B_leftalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11028,utNone},
{&MI_EM_5_ID11B_leftalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11029,utNone},
{&MI_EM_5_ID11B_leftalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11030,utNone},
{&MI_EM_5_ID11B_leftalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11031,utNone}
#endif
};

// 左对齐
struct MenuItem MI_EM_5_ID11B_leftalignment[3]={
{&MI_EM_4_ID11A_VSD1[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Left,10,utNone},
{&MI_EM_4_ID11A_VSD1[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID11D_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID11A_VSD1[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID11E_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 双向
struct MenuItem MI_EM_5_ID12E_BiDir[3]={
{&MI_EM_4_ID11A_VSD1[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_BiDir,10,utNone},
{&MI_EM_4_ID11A_VSD1[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,10,utNone},
{&MI_EM_4_ID11A_VSD1[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,110,utNone}
};

// 校正值高
struct MenuItem MI_EM_6_ID133_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID131_rightalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1000,utNone},
{&MI_EM_5_ID131_rightalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1001,utNone},
{&MI_EM_5_ID131_rightalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1002,utNone},
{&MI_EM_5_ID131_rightalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1003,utNone},
{&MI_EM_5_ID131_rightalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1004,utNone},
{&MI_EM_5_ID131_rightalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1005,utNone},
{&MI_EM_5_ID131_rightalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1006,utNone},
{&MI_EM_5_ID131_rightalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1007,utNone},
{&MI_EM_5_ID131_rightalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1008,utNone},
{&MI_EM_5_ID131_rightalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1009,utNone},
{&MI_EM_5_ID131_rightalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1010,utNone},
{&MI_EM_5_ID131_rightalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1011,utNone},
{&MI_EM_5_ID131_rightalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1012,utNone},
{&MI_EM_5_ID131_rightalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1013,utNone},
{&MI_EM_5_ID131_rightalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1014,utNone},
{&MI_EM_5_ID131_rightalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1015,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID131_rightalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1016,utNone},
{&MI_EM_5_ID131_rightalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1017,utNone},
{&MI_EM_5_ID131_rightalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1018,utNone},
{&MI_EM_5_ID131_rightalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1019,utNone},
{&MI_EM_5_ID131_rightalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1020,utNone},
{&MI_EM_5_ID131_rightalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1021,utNone},
{&MI_EM_5_ID131_rightalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1022,utNone},
{&MI_EM_5_ID131_rightalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1023,utNone},
{&MI_EM_5_ID131_rightalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1024,utNone},
{&MI_EM_5_ID131_rightalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1025,utNone},
{&MI_EM_5_ID131_rightalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1026,utNone},
{&MI_EM_5_ID131_rightalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1027,utNone},
{&MI_EM_5_ID131_rightalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1028,utNone},
{&MI_EM_5_ID131_rightalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1029,utNone},
{&MI_EM_5_ID131_rightalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1030,utNone},
{&MI_EM_5_ID131_rightalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1031,utNone}
#endif
};

// 校正值低
struct MenuItem MI_EM_6_ID134_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID131_rightalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11000,utNone},
{&MI_EM_5_ID131_rightalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11001,utNone},
{&MI_EM_5_ID131_rightalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11002,utNone},
{&MI_EM_5_ID131_rightalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11003,utNone},
{&MI_EM_5_ID131_rightalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11004,utNone},
{&MI_EM_5_ID131_rightalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11005,utNone},
{&MI_EM_5_ID131_rightalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11006,utNone},
{&MI_EM_5_ID131_rightalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11007,utNone},
{&MI_EM_5_ID131_rightalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11008,utNone},
{&MI_EM_5_ID131_rightalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11009,utNone},
{&MI_EM_5_ID131_rightalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11010,utNone},
{&MI_EM_5_ID131_rightalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11011,utNone},
{&MI_EM_5_ID131_rightalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11012,utNone},
{&MI_EM_5_ID131_rightalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11013,utNone},
{&MI_EM_5_ID131_rightalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11014,utNone},
{&MI_EM_5_ID131_rightalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11015,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID131_rightalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11016,utNone},
{&MI_EM_5_ID131_rightalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11017,utNone},
{&MI_EM_5_ID131_rightalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11018,utNone},
{&MI_EM_5_ID131_rightalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11019,utNone},
{&MI_EM_5_ID131_rightalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11020,utNone},
{&MI_EM_5_ID131_rightalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11021,utNone},
{&MI_EM_5_ID131_rightalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11022,utNone},
{&MI_EM_5_ID131_rightalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11023,utNone},
{&MI_EM_5_ID131_rightalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11024,utNone},
{&MI_EM_5_ID131_rightalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11025,utNone},
{&MI_EM_5_ID131_rightalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11026,utNone},
{&MI_EM_5_ID131_rightalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11027,utNone},
{&MI_EM_5_ID131_rightalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11028,utNone},
{&MI_EM_5_ID131_rightalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11029,utNone},
{&MI_EM_5_ID131_rightalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11030,utNone},
{&MI_EM_5_ID131_rightalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11031,utNone}
#endif
};

// 右对齐
struct MenuItem MI_EM_5_ID131_rightalignment[3]={
{&MI_EM_4_ID11A_VSD1[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Right,10,utNone},
{&MI_EM_4_ID11A_VSD1[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID133_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID11A_VSD1[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID134_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD1
struct MenuItem MI_EM_4_ID11A_VSD1[3]={
{&MI_EM_3_ID119_720DPI[0],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID11B_leftalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID119_720DPI[0],1,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_EM_5_ID12E_BiDir,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID119_720DPI[0],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID131_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#else
{&MI_EM_3_ID119_720DPI[0],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID131_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#endif
};

// 校正值高
struct MenuItem MI_EM_6_ID147_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID145_leftalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1100,utNone},
{&MI_EM_5_ID145_leftalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1101,utNone},
{&MI_EM_5_ID145_leftalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1102,utNone},
{&MI_EM_5_ID145_leftalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1103,utNone},
{&MI_EM_5_ID145_leftalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1104,utNone},
{&MI_EM_5_ID145_leftalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1105,utNone},
{&MI_EM_5_ID145_leftalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1106,utNone},
{&MI_EM_5_ID145_leftalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1107,utNone},
{&MI_EM_5_ID145_leftalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1108,utNone},
{&MI_EM_5_ID145_leftalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1109,utNone},
{&MI_EM_5_ID145_leftalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1110,utNone},
{&MI_EM_5_ID145_leftalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1111,utNone},
{&MI_EM_5_ID145_leftalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1112,utNone},
{&MI_EM_5_ID145_leftalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1113,utNone},
{&MI_EM_5_ID145_leftalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1114,utNone},
{&MI_EM_5_ID145_leftalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1115,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID145_leftalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1116,utNone},
{&MI_EM_5_ID145_leftalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1117,utNone},
{&MI_EM_5_ID145_leftalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1118,utNone},
{&MI_EM_5_ID145_leftalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1119,utNone},
{&MI_EM_5_ID145_leftalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1120,utNone},
{&MI_EM_5_ID145_leftalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1121,utNone},
{&MI_EM_5_ID145_leftalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1122,utNone},
{&MI_EM_5_ID145_leftalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1123,utNone},
{&MI_EM_5_ID145_leftalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1124,utNone},
{&MI_EM_5_ID145_leftalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1125,utNone},
{&MI_EM_5_ID145_leftalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1126,utNone},
{&MI_EM_5_ID145_leftalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1127,utNone},
{&MI_EM_5_ID145_leftalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1128,utNone},
{&MI_EM_5_ID145_leftalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1129,utNone},
{&MI_EM_5_ID145_leftalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1130,utNone},
{&MI_EM_5_ID145_leftalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1131,utNone}
#endif
};

// 校正值低
struct MenuItem MI_EM_6_ID148_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID145_leftalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11100,utNone},
{&MI_EM_5_ID145_leftalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11101,utNone},
{&MI_EM_5_ID145_leftalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11102,utNone},
{&MI_EM_5_ID145_leftalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11103,utNone},
{&MI_EM_5_ID145_leftalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11104,utNone},
{&MI_EM_5_ID145_leftalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11105,utNone},
{&MI_EM_5_ID145_leftalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11106,utNone},
{&MI_EM_5_ID145_leftalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11107,utNone},
{&MI_EM_5_ID145_leftalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11108,utNone},
{&MI_EM_5_ID145_leftalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11109,utNone},
{&MI_EM_5_ID145_leftalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11110,utNone},
{&MI_EM_5_ID145_leftalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11111,utNone},
{&MI_EM_5_ID145_leftalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11112,utNone},
{&MI_EM_5_ID145_leftalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11113,utNone},
{&MI_EM_5_ID145_leftalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11114,utNone},
{&MI_EM_5_ID145_leftalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11115,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID145_leftalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11116,utNone},
{&MI_EM_5_ID145_leftalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11117,utNone},
{&MI_EM_5_ID145_leftalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11118,utNone},
{&MI_EM_5_ID145_leftalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11119,utNone},
{&MI_EM_5_ID145_leftalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11120,utNone},
{&MI_EM_5_ID145_leftalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11121,utNone},
{&MI_EM_5_ID145_leftalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11122,utNone},
{&MI_EM_5_ID145_leftalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11123,utNone},
{&MI_EM_5_ID145_leftalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11124,utNone},
{&MI_EM_5_ID145_leftalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11125,utNone},
{&MI_EM_5_ID145_leftalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11126,utNone},
{&MI_EM_5_ID145_leftalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11127,utNone},
{&MI_EM_5_ID145_leftalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11128,utNone},
{&MI_EM_5_ID145_leftalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11129,utNone},
{&MI_EM_5_ID145_leftalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11130,utNone},
{&MI_EM_5_ID145_leftalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11131,utNone}
#endif
};

// 左对齐
struct MenuItem MI_EM_5_ID145_leftalignment[3]={
{&MI_EM_4_ID144_VSD2[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Left,11,utNone},
{&MI_EM_4_ID144_VSD2[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID147_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID144_VSD2[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID148_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 双向
struct MenuItem MI_EM_5_ID158_BiDir[3]={
{&MI_EM_4_ID144_VSD2[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_BiDir,11,utNone},
{&MI_EM_4_ID144_VSD2[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,11,utNone},
{&MI_EM_4_ID144_VSD2[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,111,utNone}
};

// 校正值高
struct MenuItem MI_EM_6_ID15D_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID15B_rightalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1100,utNone},
{&MI_EM_5_ID15B_rightalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1101,utNone},
{&MI_EM_5_ID15B_rightalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1102,utNone},
{&MI_EM_5_ID15B_rightalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1103,utNone},
{&MI_EM_5_ID15B_rightalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1104,utNone},
{&MI_EM_5_ID15B_rightalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1105,utNone},
{&MI_EM_5_ID15B_rightalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1106,utNone},
{&MI_EM_5_ID15B_rightalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1107,utNone},
{&MI_EM_5_ID15B_rightalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1108,utNone},
{&MI_EM_5_ID15B_rightalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1109,utNone},
{&MI_EM_5_ID15B_rightalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1110,utNone},
{&MI_EM_5_ID15B_rightalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1111,utNone},
{&MI_EM_5_ID15B_rightalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1112,utNone},
{&MI_EM_5_ID15B_rightalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1113,utNone},
{&MI_EM_5_ID15B_rightalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1114,utNone},
{&MI_EM_5_ID15B_rightalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1115,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID15B_rightalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1116,utNone},
{&MI_EM_5_ID15B_rightalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1117,utNone},
{&MI_EM_5_ID15B_rightalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1118,utNone},
{&MI_EM_5_ID15B_rightalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1119,utNone},
{&MI_EM_5_ID15B_rightalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1120,utNone},
{&MI_EM_5_ID15B_rightalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1121,utNone},
{&MI_EM_5_ID15B_rightalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1122,utNone},
{&MI_EM_5_ID15B_rightalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1123,utNone},
{&MI_EM_5_ID15B_rightalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1124,utNone},
{&MI_EM_5_ID15B_rightalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1125,utNone},
{&MI_EM_5_ID15B_rightalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1126,utNone},
{&MI_EM_5_ID15B_rightalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1127,utNone},
{&MI_EM_5_ID15B_rightalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1128,utNone},
{&MI_EM_5_ID15B_rightalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1129,utNone},
{&MI_EM_5_ID15B_rightalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1130,utNone},
{&MI_EM_5_ID15B_rightalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1131,utNone}
#endif
};

// 校正值低
struct MenuItem MI_EM_6_ID15E_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID15B_rightalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11100,utNone},
{&MI_EM_5_ID15B_rightalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11101,utNone},
{&MI_EM_5_ID15B_rightalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11012,utNone},
{&MI_EM_5_ID15B_rightalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11103,utNone},
{&MI_EM_5_ID15B_rightalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11104,utNone},
{&MI_EM_5_ID15B_rightalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11105,utNone},
{&MI_EM_5_ID15B_rightalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11106,utNone},
{&MI_EM_5_ID15B_rightalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11107,utNone},
{&MI_EM_5_ID15B_rightalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11108,utNone},
{&MI_EM_5_ID15B_rightalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11109,utNone},
{&MI_EM_5_ID15B_rightalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11110,utNone},
{&MI_EM_5_ID15B_rightalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11111,utNone},
{&MI_EM_5_ID15B_rightalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11112,utNone},
{&MI_EM_5_ID15B_rightalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11113,utNone},
{&MI_EM_5_ID15B_rightalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11114,utNone},
{&MI_EM_5_ID15B_rightalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11115,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID15B_rightalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11116,utNone},
{&MI_EM_5_ID15B_rightalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11117,utNone},
{&MI_EM_5_ID15B_rightalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11118,utNone},
{&MI_EM_5_ID15B_rightalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11119,utNone},
{&MI_EM_5_ID15B_rightalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11120,utNone},
{&MI_EM_5_ID15B_rightalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11121,utNone},
{&MI_EM_5_ID15B_rightalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11122,utNone},
{&MI_EM_5_ID15B_rightalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11123,utNone},
{&MI_EM_5_ID15B_rightalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11124,utNone},
{&MI_EM_5_ID15B_rightalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11125,utNone},
{&MI_EM_5_ID15B_rightalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11126,utNone},
{&MI_EM_5_ID15B_rightalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11127,utNone},
{&MI_EM_5_ID15B_rightalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11128,utNone},
{&MI_EM_5_ID15B_rightalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11129,utNone},
{&MI_EM_5_ID15B_rightalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11130,utNone},
{&MI_EM_5_ID15B_rightalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11131,utNone}
#endif
};
// 右对齐
struct MenuItem MI_EM_5_ID15B_rightalignment[3]={
{&MI_EM_4_ID144_VSD2[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Right,11,utNone},
{&MI_EM_4_ID144_VSD2[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID15D_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID144_VSD2[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID15E_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD2
struct MenuItem MI_EM_4_ID144_VSD2[3]={
{&MI_EM_3_ID119_720DPI[1],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID145_leftalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID119_720DPI[1],1,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_EM_5_ID158_BiDir,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID119_720DPI[1],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID15B_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#else
{&MI_EM_3_ID119_720DPI[1],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID15B_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#endif
};

// 校正值高
struct MenuItem MI_EM_6_ID171_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID16F_leftalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1200,utNone},
{&MI_EM_5_ID16F_leftalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1201,utNone},
{&MI_EM_5_ID16F_leftalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1202,utNone},
{&MI_EM_5_ID16F_leftalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1203,utNone},
{&MI_EM_5_ID16F_leftalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1204,utNone},
{&MI_EM_5_ID16F_leftalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1205,utNone},
{&MI_EM_5_ID16F_leftalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1206,utNone},
{&MI_EM_5_ID16F_leftalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1207,utNone},
{&MI_EM_5_ID16F_leftalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1208,utNone},
{&MI_EM_5_ID16F_leftalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1209,utNone},
{&MI_EM_5_ID16F_leftalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1210,utNone},
{&MI_EM_5_ID16F_leftalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1211,utNone},
{&MI_EM_5_ID16F_leftalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1212,utNone},
{&MI_EM_5_ID16F_leftalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1213,utNone},
{&MI_EM_5_ID16F_leftalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1214,utNone},
{&MI_EM_5_ID16F_leftalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1215,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID16F_leftalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1216,utNone},
{&MI_EM_5_ID16F_leftalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1217,utNone},
{&MI_EM_5_ID16F_leftalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1218,utNone},
{&MI_EM_5_ID16F_leftalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1219,utNone},
{&MI_EM_5_ID16F_leftalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1220,utNone},
{&MI_EM_5_ID16F_leftalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1221,utNone},
{&MI_EM_5_ID16F_leftalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1222,utNone},
{&MI_EM_5_ID16F_leftalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1223,utNone},
{&MI_EM_5_ID16F_leftalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1224,utNone},
{&MI_EM_5_ID16F_leftalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1225,utNone},
{&MI_EM_5_ID16F_leftalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1226,utNone},
{&MI_EM_5_ID16F_leftalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1227,utNone},
{&MI_EM_5_ID16F_leftalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1228,utNone},
{&MI_EM_5_ID16F_leftalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1229,utNone},
{&MI_EM_5_ID16F_leftalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1230,utNone},
{&MI_EM_5_ID16F_leftalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1231,utNone}
#endif

};
// 校正值低
struct MenuItem MI_EM_6_ID172_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID16F_leftalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11200,utNone},
{&MI_EM_5_ID16F_leftalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11201,utNone},
{&MI_EM_5_ID16F_leftalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11202,utNone},
{&MI_EM_5_ID16F_leftalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11203,utNone},
{&MI_EM_5_ID16F_leftalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11204,utNone},
{&MI_EM_5_ID16F_leftalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11205,utNone},
{&MI_EM_5_ID16F_leftalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11206,utNone},
{&MI_EM_5_ID16F_leftalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11207,utNone},
{&MI_EM_5_ID16F_leftalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11208,utNone},
{&MI_EM_5_ID16F_leftalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11209,utNone},
{&MI_EM_5_ID16F_leftalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11210,utNone},
{&MI_EM_5_ID16F_leftalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11211,utNone},
{&MI_EM_5_ID16F_leftalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11212,utNone},
{&MI_EM_5_ID16F_leftalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11213,utNone},
{&MI_EM_5_ID16F_leftalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11214,utNone},
{&MI_EM_5_ID16F_leftalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11215,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID16F_leftalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11216,utNone},
{&MI_EM_5_ID16F_leftalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11217,utNone},
{&MI_EM_5_ID16F_leftalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11218,utNone},
{&MI_EM_5_ID16F_leftalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11219,utNone},
{&MI_EM_5_ID16F_leftalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11220,utNone},
{&MI_EM_5_ID16F_leftalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11221,utNone},
{&MI_EM_5_ID16F_leftalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11222,utNone},
{&MI_EM_5_ID16F_leftalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11223,utNone},
{&MI_EM_5_ID16F_leftalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11224,utNone},
{&MI_EM_5_ID16F_leftalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11225,utNone},
{&MI_EM_5_ID16F_leftalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11226,utNone},
{&MI_EM_5_ID16F_leftalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11227,utNone},
{&MI_EM_5_ID16F_leftalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11228,utNone},
{&MI_EM_5_ID16F_leftalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11229,utNone},
{&MI_EM_5_ID16F_leftalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11230,utNone},
{&MI_EM_5_ID16F_leftalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11231,utNone}
#endif
};
// 左对齐
struct MenuItem MI_EM_5_ID16F_leftalignment[3]={
{&MI_EM_4_ID16E_VSD3[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Left,12,utNone},
{&MI_EM_4_ID16E_VSD3[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID171_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID16E_VSD3[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID172_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 双向
struct MenuItem MI_EM_5_ID182_BiDir[3]={
{&MI_EM_4_ID16E_VSD3[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_BiDir,12,utNone},
{&MI_EM_4_ID16E_VSD3[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,12,utNone},
{&MI_EM_4_ID16E_VSD3[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,112,utNone}
};

// 校正值高
struct MenuItem MI_EM_6_ID187_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID185_rightalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1200,utNone},
{&MI_EM_5_ID185_rightalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1201,utNone},
{&MI_EM_5_ID185_rightalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1202,utNone},
{&MI_EM_5_ID185_rightalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1203,utNone},
{&MI_EM_5_ID185_rightalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1204,utNone},
{&MI_EM_5_ID185_rightalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1205,utNone},
{&MI_EM_5_ID185_rightalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1206,utNone},
{&MI_EM_5_ID185_rightalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1207,utNone},
{&MI_EM_5_ID185_rightalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1208,utNone},
{&MI_EM_5_ID185_rightalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1209,utNone},
{&MI_EM_5_ID185_rightalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1210,utNone},
{&MI_EM_5_ID185_rightalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1211,utNone},
{&MI_EM_5_ID185_rightalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1212,utNone},
{&MI_EM_5_ID185_rightalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1213,utNone},
{&MI_EM_5_ID185_rightalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1214,utNone},
{&MI_EM_5_ID185_rightalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1215,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID185_rightalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1216,utNone},
{&MI_EM_5_ID185_rightalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1217,utNone},
{&MI_EM_5_ID185_rightalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1218,utNone},
{&MI_EM_5_ID185_rightalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1219,utNone},
{&MI_EM_5_ID185_rightalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1220,utNone},
{&MI_EM_5_ID185_rightalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1221,utNone},
{&MI_EM_5_ID185_rightalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1222,utNone},
{&MI_EM_5_ID185_rightalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1223,utNone},
{&MI_EM_5_ID185_rightalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1224,utNone},
{&MI_EM_5_ID185_rightalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1225,utNone},
{&MI_EM_5_ID185_rightalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1226,utNone},
{&MI_EM_5_ID185_rightalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1227,utNone},
{&MI_EM_5_ID185_rightalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1228,utNone},
{&MI_EM_5_ID185_rightalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1229,utNone},
{&MI_EM_5_ID185_rightalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1230,utNone},
{&MI_EM_5_ID185_rightalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1231,utNone}
#endif
};

// 校正值低
struct MenuItem MI_EM_6_ID188_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID185_rightalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11200,utNone},
{&MI_EM_5_ID185_rightalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11201,utNone},
{&MI_EM_5_ID185_rightalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11202,utNone},
{&MI_EM_5_ID185_rightalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11203,utNone},
{&MI_EM_5_ID185_rightalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11204,utNone},
{&MI_EM_5_ID185_rightalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11205,utNone},
{&MI_EM_5_ID185_rightalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11206,utNone},
{&MI_EM_5_ID185_rightalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11207,utNone},
{&MI_EM_5_ID185_rightalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11208,utNone},
{&MI_EM_5_ID185_rightalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11209,utNone},
{&MI_EM_5_ID185_rightalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11210,utNone},
{&MI_EM_5_ID185_rightalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11211,utNone},
{&MI_EM_5_ID185_rightalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11212,utNone},
{&MI_EM_5_ID185_rightalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11213,utNone},
{&MI_EM_5_ID185_rightalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11214,utNone},
{&MI_EM_5_ID185_rightalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11215,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID185_rightalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11216,utNone},
{&MI_EM_5_ID185_rightalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11217,utNone},
{&MI_EM_5_ID185_rightalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11218,utNone},
{&MI_EM_5_ID185_rightalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11219,utNone},
{&MI_EM_5_ID185_rightalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11220,utNone},
{&MI_EM_5_ID185_rightalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11221,utNone},
{&MI_EM_5_ID185_rightalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11222,utNone},
{&MI_EM_5_ID185_rightalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11223,utNone},
{&MI_EM_5_ID185_rightalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11224,utNone},
{&MI_EM_5_ID185_rightalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11225,utNone},
{&MI_EM_5_ID185_rightalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11226,utNone},
{&MI_EM_5_ID185_rightalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11227,utNone},
{&MI_EM_5_ID185_rightalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11228,utNone},
{&MI_EM_5_ID185_rightalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11229,utNone},
{&MI_EM_5_ID185_rightalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11230,utNone},
{&MI_EM_5_ID185_rightalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11231,utNone}
#endif
};

// 右对齐
struct MenuItem MI_EM_5_ID185_rightalignment[3]={
{&MI_EM_4_ID16E_VSD3[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Right,12,utNone},
{&MI_EM_4_ID16E_VSD3[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID187_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID16E_VSD3[2],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID188_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD3
struct MenuItem MI_EM_4_ID16E_VSD3[3]={
{&MI_EM_3_ID119_720DPI[2],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID16F_leftalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID119_720DPI[2],1,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_EM_5_ID182_BiDir,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID119_720DPI[2],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID185_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#else
{&MI_EM_3_ID119_720DPI[2],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID185_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#endif
};

// 校正值高
struct MenuItem MI_EM_6_ID19B_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID199_leftalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1300,utNone},
{&MI_EM_5_ID199_leftalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1301,utNone},
{&MI_EM_5_ID199_leftalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1302,utNone},
{&MI_EM_5_ID199_leftalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1303,utNone},
{&MI_EM_5_ID199_leftalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1304,utNone},
{&MI_EM_5_ID199_leftalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1305,utNone},
{&MI_EM_5_ID199_leftalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1306,utNone},
{&MI_EM_5_ID199_leftalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1307,utNone},
{&MI_EM_5_ID199_leftalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1308,utNone},
{&MI_EM_5_ID199_leftalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1309,utNone},
{&MI_EM_5_ID199_leftalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1310,utNone},
{&MI_EM_5_ID199_leftalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1311,utNone},
{&MI_EM_5_ID199_leftalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1312,utNone},
{&MI_EM_5_ID199_leftalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1313,utNone},
{&MI_EM_5_ID199_leftalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1314,utNone},
{&MI_EM_5_ID199_leftalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1315,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID199_leftalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1316,utNone},
{&MI_EM_5_ID199_leftalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1317,utNone},
{&MI_EM_5_ID199_leftalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1318,utNone},
{&MI_EM_5_ID199_leftalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1319,utNone},
{&MI_EM_5_ID199_leftalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1320,utNone},
{&MI_EM_5_ID199_leftalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1321,utNone},
{&MI_EM_5_ID199_leftalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1322,utNone},
{&MI_EM_5_ID199_leftalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1323,utNone},
{&MI_EM_5_ID199_leftalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1324,utNone},
{&MI_EM_5_ID199_leftalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1325,utNone},
{&MI_EM_5_ID199_leftalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1326,utNone},
{&MI_EM_5_ID199_leftalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1327,utNone},
{&MI_EM_5_ID199_leftalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1328,utNone},
{&MI_EM_5_ID199_leftalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1329,utNone},
{&MI_EM_5_ID199_leftalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1330,utNone},
{&MI_EM_5_ID199_leftalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,1331,utNone}
#endif
};
// 校正值低
struct MenuItem MI_EM_6_ID19C_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID199_leftalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11300,utNone},
{&MI_EM_5_ID199_leftalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11301,utNone},
{&MI_EM_5_ID199_leftalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11302,utNone},
{&MI_EM_5_ID199_leftalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11303,utNone},
{&MI_EM_5_ID199_leftalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11304,utNone},
{&MI_EM_5_ID199_leftalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11305,utNone},
{&MI_EM_5_ID199_leftalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11306,utNone},
{&MI_EM_5_ID199_leftalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11307,utNone},
{&MI_EM_5_ID199_leftalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11308,utNone},
{&MI_EM_5_ID199_leftalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11309,utNone},
{&MI_EM_5_ID199_leftalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11310,utNone},
{&MI_EM_5_ID199_leftalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11311,utNone},
{&MI_EM_5_ID199_leftalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11312,utNone},
{&MI_EM_5_ID199_leftalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11313,utNone},
{&MI_EM_5_ID199_leftalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11314,utNone},
{&MI_EM_5_ID199_leftalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11315,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID199_leftalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11316,utNone},
{&MI_EM_5_ID199_leftalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11317,utNone},
{&MI_EM_5_ID199_leftalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11318,utNone},
{&MI_EM_5_ID199_leftalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11319,utNone},
{&MI_EM_5_ID199_leftalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11320,utNone},
{&MI_EM_5_ID199_leftalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11321,utNone},
{&MI_EM_5_ID199_leftalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11322,utNone},
{&MI_EM_5_ID199_leftalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11323,utNone},
{&MI_EM_5_ID199_leftalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11324,utNone},
{&MI_EM_5_ID199_leftalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11325,utNone},
{&MI_EM_5_ID199_leftalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11326,utNone},
{&MI_EM_5_ID199_leftalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11327,utNone},
{&MI_EM_5_ID199_leftalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11328,utNone},
{&MI_EM_5_ID199_leftalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11329,utNone},
{&MI_EM_5_ID199_leftalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11330,utNone},
{&MI_EM_5_ID199_leftalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Left,(UISetFloatFuncExType)Cali_EM_Set_Hor_Left,0,11331,utNone}
#endif
};
// 左对齐
struct MenuItem MI_EM_5_ID199_leftalignment[3]={
{&MI_EM_4_ID198_VSD4[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Left,13,utNone},
{&MI_EM_4_ID198_VSD4[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID19B_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID198_VSD4[0],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID19C_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 双向
struct MenuItem MI_EM_5_ID1AC_BiDir[3]={
{&MI_EM_4_ID198_VSD4[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_BiDir,13,utNone},
{&MI_EM_4_ID198_VSD4[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,13,utNone},
{&MI_EM_4_ID198_VSD4[1],2,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_BiDir,(UISetFloatFuncExType)Cali_EM_Set_Hor_BiDir,0,113,utNone}
};

// 校正值高
struct MenuItem MI_EM_6_ID1B1_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID1AF_rightalignment[1],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1300,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1301,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1302,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1303,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1304,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1305,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1306,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1307,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1308,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1309,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1310,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1311,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1312,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1313,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1314,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1315,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID1AF_rightalignment[1],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1316,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1317,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1318,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1319,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1320,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1321,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1322,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1323,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1324,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1325,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1326,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1327,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1328,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1329,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1330,utNone},
{&MI_EM_5_ID1AF_rightalignment[1],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,1331,utNone}
#endif
};

// 校正值低
struct MenuItem MI_EM_6_ID1B2_CorrectionValue[MAX_NOZZLE_LINE]={
{&MI_EM_5_ID1AF_rightalignment[2],0,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[0],MenuStr_LeftRightAlign_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11300,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],1,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[1],MenuStr_LeftRightAlign_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11301,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],2,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[2],MenuStr_LeftRightAlign_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11302,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],3,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[3],MenuStr_LeftRightAlign_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11303,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],4,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[4],MenuStr_LeftRightAlign_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11304,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],5,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[5],MenuStr_LeftRightAlign_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11305,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],6,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[6],MenuStr_LeftRightAlign_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11306,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],7,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[7],MenuStr_LeftRightAlign_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11307,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],8,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[8],MenuStr_LeftRightAlign_Name_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11308,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],9,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[9],MenuStr_LeftRightAlign_Name_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11309,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],10,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[10],MenuStr_LeftRightAlign_Name_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11310,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],11,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[11],MenuStr_LeftRightAlign_Name_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11311,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],12,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[12],MenuStr_LeftRightAlign_Name_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11312,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],13,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[13],MenuStr_LeftRightAlign_Name_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11313,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],14,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[14],MenuStr_LeftRightAlign_Name_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11314,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],15,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[15],MenuStr_LeftRightAlign_Name_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11315,utNone}
#ifdef EPSON_4H
,{&MI_EM_5_ID1AF_rightalignment[2],16,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[16],MenuStr_LeftRightAlign_Name_E[16],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11316,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],17,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[17],MenuStr_LeftRightAlign_Name_E[17],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11317,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],18,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[18],MenuStr_LeftRightAlign_Name_E[18],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11318,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],19,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[19],MenuStr_LeftRightAlign_Name_E[19],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11319,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],20,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[20],MenuStr_LeftRightAlign_Name_E[20],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11320,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],21,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[21],MenuStr_LeftRightAlign_Name_E[21],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11321,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],22,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[22],MenuStr_LeftRightAlign_Name_E[22],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11322,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],23,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[23],MenuStr_LeftRightAlign_Name_E[23],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11323,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],24,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[24],MenuStr_LeftRightAlign_Name_E[24],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11324,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],25,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[25],MenuStr_LeftRightAlign_Name_E[25],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11325,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],26,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[26],MenuStr_LeftRightAlign_Name_E[26],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11326,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],27,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[27],MenuStr_LeftRightAlign_Name_E[27],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11327,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],28,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[28],MenuStr_LeftRightAlign_Name_E[28],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11328,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],29,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[29],MenuStr_LeftRightAlign_Name_E[29],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11329,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],30,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[30],MenuStr_LeftRightAlign_Name_E[30],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11330,utNone},
{&MI_EM_5_ID1AF_rightalignment[2],31,0,0,(unsigned short *)MenuStr_LeftRightAlign_Name[31],MenuStr_LeftRightAlign_Name_E[31],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Hor_Right,(UISetFloatFuncExType)Cali_EM_Set_Hor_Right,0,11331,utNone}
#endif
};
// 右对齐
struct MenuItem MI_EM_5_ID1AF_rightalignment[3]={
{&MI_EM_4_ID198_VSD4[2],0,0,0,(unsigned short *)MenuStr_UM_4_ID30_Print,MenuStr_UM_4_ID30_Print_EN,0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Hor_Right,13,utNone},
{&MI_EM_4_ID198_VSD4[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2B_CorrectionValue,MenuStr_UM_4_CorrectionValue_HIGH,MI_EM_6_ID1B1_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_4_ID198_VSD4[2],1,0,0,(unsigned short *)MenuStr_UM_4_ID2C_CorrectionValue,MenuStr_UM_4_CorrectionValue_LOW,MI_EM_6_ID1B2_CorrectionValue,MAX_NOZZLE_LINE,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD4
struct MenuItem MI_EM_4_ID198_VSD4[3]={
{&MI_EM_3_ID119_720DPI[3],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID199_leftalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID119_720DPI[3],1,0,0,(unsigned short *)MenuStr_UM_2_ID3D_BiDir,"Bi-Dir",MI_EM_5_ID1AC_BiDir,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID119_720DPI[3],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID1AF_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#else
{&MI_EM_3_ID119_720DPI[3],2,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID1AF_rightalignment,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
#endif
};

// 720DPI
struct MenuItem MI_EM_3_ID119_720DPI[4]={
{&MI_EM_2_ID6F_Horizotalalignment[1],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",MI_EM_4_ID11A_VSD1,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#if defined(HEAD_EPSON_GEN5)||defined(RICOH_VSD2)
{&MI_EM_2_ID6F_Horizotalalignment[1],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_EM_4_ID144_VSD2,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#else
{&MI_EM_2_ID6F_Horizotalalignment[1],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_EM_4_ID144_VSD2,3,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,1,utNone},
#endif
{&MI_EM_2_ID6F_Horizotalalignment[1],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",MI_EM_4_ID16E_VSD3,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_EM_2_ID6F_Horizotalalignment[1],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",MI_EM_4_ID198_VSD4,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone}
};

// 水平检查
struct MenuItem MI_EM_2_ID6F_Horizotalalignment[5]={
{&MI_EM_1_ID4B_ADJUSTMENT[6],0,0,0,(unsigned short *)MenuStr_DPI_Name[0],MenuStr_DPI_Name_E[0],MI_EM_3_ID70_360DPI,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[6],1,0,0,(unsigned short *)MenuStr_DPI_Name[1],MenuStr_DPI_Name_E[1],MI_EM_3_ID119_720DPI,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[6],2,0,0,(unsigned short *)MenuStr_DPI_Name[2],MenuStr_DPI_Name_E[2],0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,2,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[6],3,0,0,(unsigned short *)MenuStr_DPI_Name[3],MenuStr_DPI_Name_E[3],0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,3,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[6],4,0,0,(unsigned short *)MenuStr_DPI_Name[4],MenuStr_DPI_Name_E[4],0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,4,utNone}
};

// 校正值
struct MenuItem MI_EM_3_ID1C7_CorrectionValue[MAX_COLOR_NUM]={
{&MI_EM_2_ID1C5_Verticalalignment[1],0,0,0,(unsigned short *)MenuStr_Color_Name[0],MenuStr_Color_Name_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,0,utNone},
{&MI_EM_2_ID1C5_Verticalalignment[1],1,0,0,(unsigned short *)MenuStr_Color_Name[1],MenuStr_Color_Name_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,1,utNone},
{&MI_EM_2_ID1C5_Verticalalignment[1],2,0,0,(unsigned short *)MenuStr_Color_Name[2],MenuStr_Color_Name_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,2,utNone},
{&MI_EM_2_ID1C5_Verticalalignment[1],3,0,0,(unsigned short *)MenuStr_Color_Name[3],MenuStr_Color_Name_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,3,utNone},
{&MI_EM_2_ID1C5_Verticalalignment[1],4,0,0,(unsigned short *)MenuStr_Color_Name[4],MenuStr_Color_Name_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,4,utNone},
{&MI_EM_2_ID1C5_Verticalalignment[1],5,0,0,(unsigned short *)MenuStr_Color_Name[5],MenuStr_Color_Name_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,5,utNone},
{&MI_EM_2_ID1C5_Verticalalignment[1],6,0,0,(unsigned short *)MenuStr_Color_Name[6],MenuStr_Color_Name_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,6,utNone},
{&MI_EM_2_ID1C5_Verticalalignment[1],7,0,0,(unsigned short *)MenuStr_Color_Name[7],MenuStr_Color_Name_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Vertical,(UISetFloatFuncExType)Cali_EM_Set_Vertical,0,7,utNone}
};

// 垂直检查
struct MenuItem MI_EM_2_ID1C5_Verticalalignment[2]={
{&MI_EM_1_ID4B_ADJUSTMENT[7],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Vertical,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[7],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_3_ID1C7_CorrectionValue,MAX_COLOR_NUM,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

#ifdef HEAD_EPSON_GEN5
// 校正值
struct MenuItem MI_EM_6_ID1D1_CorrectionValue[2]={
{&MI_EM_5_ID1CF_leftalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H1,0,0,utNone},
{&MI_EM_5_ID1CF_leftalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H2,0,1,utNone}
};

// 左对齐
struct MenuItem MI_EM_5_ID1CF_leftalignment[2]={
{&MI_EM_4_ID1CE_VSD1[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Left,0,utNone},
{&MI_EM_4_ID1CE_VSD1[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID1D1_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 校正值
struct MenuItem MI_EM_6_ID1D6_CorrectionValue[2]={
{&MI_EM_5_ID1D4_rightalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H1,0,0,utNone},
{&MI_EM_5_ID1D4_rightalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H2,0,1,utNone}
};

// 右对齐
struct MenuItem MI_EM_5_ID1D4_rightalignment[2]={
{&MI_EM_4_ID1CE_VSD1[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Right,0,utNone},
{&MI_EM_4_ID1CE_VSD1[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID1D6_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD1
struct MenuItem MI_EM_4_ID1CE_VSD1[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID1CD_360DPI[0],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1CF_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1CD_360DPI[0],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID1D4_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#else
{&MI_EM_3_ID1CD_360DPI[0],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1CF_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1CD_360DPI[0],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID1D4_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#endif
};

// 校正值
struct MenuItem MI_EM_6_ID1DC_CorrectionValue[2]={
{&MI_EM_5_ID1DA_leftalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H1,0,1,utNone},
{&MI_EM_5_ID1DA_leftalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H2,0,1,utNone}
};

// 左对齐
struct MenuItem MI_EM_5_ID1DA_leftalignment[2]={
{&MI_EM_4_ID1D9_VSD2[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Left,1,utNone},
{&MI_EM_4_ID1D9_VSD2[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID1DC_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 校正值
struct MenuItem MI_EM_6_ID1E1_CorrectionValue[2]={
{&MI_EM_5_ID1DF_rightalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H1,0,1,utNone},
{&MI_EM_5_ID1DF_rightalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H2,0,1,utNone}
};

// 右对齐
struct MenuItem MI_EM_5_ID1DF_rightalignment[2]={
{&MI_EM_4_ID1D9_VSD2[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Right,1,utNone},
{&MI_EM_4_ID1D9_VSD2[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID1E1_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD2
struct MenuItem MI_EM_4_ID1D9_VSD2[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID1CD_360DPI[1],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1DA_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1CD_360DPI[1],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID1DF_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#else
{&MI_EM_3_ID1CD_360DPI[1],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1DA_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1CD_360DPI[1],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID1DF_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#endif
};

// 校正值
struct MenuItem MI_EM_6_ID1E7_CorrectionValue[2]={
{&MI_EM_5_ID1E5_leftalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H1,0,2,utNone},
{&MI_EM_5_ID1E5_leftalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H2,0,2,utNone}
};

// 左对齐
struct MenuItem MI_EM_5_ID1E5_leftalignment[2]={
{&MI_EM_4_ID1E4_VSD3[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Left,2,utNone},
{&MI_EM_4_ID1E4_VSD3[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID1E7_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 校正值
struct MenuItem MI_EM_6_ID1EC_CorrectionValue[2]={
{&MI_EM_5_ID1EA_rightalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H1,0,2,utNone},
{&MI_EM_5_ID1EA_rightalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H2,0,2,utNone}
};

// 右对齐
struct MenuItem MI_EM_5_ID1EA_rightalignment[2]={
{&MI_EM_4_ID1E4_VSD3[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Right,2,utNone},
{&MI_EM_4_ID1E4_VSD3[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID1EC_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD3
struct MenuItem MI_EM_4_ID1E4_VSD3[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID1CD_360DPI[2],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1E5_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1CD_360DPI[2],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID1EA_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#else
{&MI_EM_3_ID1CD_360DPI[2],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1E5_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1CD_360DPI[2],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID1EA_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#endif
};

// 校正值
struct MenuItem MI_EM_6_ID1F2_CorrectionValue[2]={
{&MI_EM_5_ID1F0_leftalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H1,0,3,utNone},
{&MI_EM_5_ID1F0_leftalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H2,0,3,utNone}
};

// 左对齐
struct MenuItem MI_EM_5_ID1F0_leftalignment[2]={
{&MI_EM_4_ID1EF_VSD4[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Left,3,utNone},
{&MI_EM_4_ID1EF_VSD4[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID1F2_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 校正值
struct MenuItem MI_EM_6_ID1F7_CorrectionValue[2]={
{&MI_EM_5_ID1F5_rightalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H1,0,3,utNone},
{&MI_EM_5_ID1F5_rightalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H2,0,3,utNone}
};

// 右对齐
struct MenuItem MI_EM_5_ID1F5_rightalignment[2]={
{&MI_EM_4_ID1EF_VSD4[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Right,3,utNone},
{&MI_EM_4_ID1EF_VSD4[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID1F7_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD4
struct MenuItem MI_EM_4_ID1EF_VSD4[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID1CD_360DPI[3],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1F0_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1CD_360DPI[3],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID1F5_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#else
{&MI_EM_3_ID1CD_360DPI[3],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1F0_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1CD_360DPI[3],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID1F5_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#endif
};

// 360DPI
struct MenuItem MI_EM_3_ID1CD_360DPI[4]={
{&MI_EM_2_ID1CC_AngleAdjustment[0],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",MI_EM_4_ID1CE_VSD1,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#if defined(HEAD_EPSON_GEN5)||defined(RICOH_VSD2)
{&MI_EM_2_ID1CC_AngleAdjustment[0],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_EM_4_ID1D9_VSD2,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#else
{&MI_EM_2_ID1CC_AngleAdjustment[0],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_EM_4_ID1D9_VSD2,2,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,1,utNone},
#endif
{&MI_EM_2_ID1CC_AngleAdjustment[0],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",MI_EM_4_ID1E4_VSD3,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_EM_2_ID1CC_AngleAdjustment[0],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",MI_EM_4_ID1EF_VSD4,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone}
};

// 校正值
struct MenuItem MI_EM_6_ID1FE_CorrectionValue[2]={
{&MI_EM_5_ID1FC_leftalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H1,0,10,utNone},
{&MI_EM_5_ID1FC_leftalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H2,0,10,utNone}
};

// 左对齐
struct MenuItem MI_EM_5_ID1FC_leftalignment[2]={
{&MI_EM_4_ID1FB_VSD1[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Left,10,utNone},
{&MI_EM_4_ID1FB_VSD1[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID1FE_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 校正值
struct MenuItem MI_EM_6_ID203_CorrectionValue[2]={
{&MI_EM_5_ID201_rightalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H1,0,10,utNone},
{&MI_EM_5_ID201_rightalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H2,0,10,utNone}
};

// 右对齐
struct MenuItem MI_EM_5_ID201_rightalignment[2]={
{&MI_EM_4_ID1FB_VSD1[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Right,10,utNone},
{&MI_EM_4_ID1FB_VSD1[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID203_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD1
struct MenuItem MI_EM_4_ID1FB_VSD1[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID1FA_720DPI[0],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1FC_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1FA_720DPI[0],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID201_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#else
{&MI_EM_3_ID1FA_720DPI[0],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID1FC_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1FA_720DPI[0],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID201_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#endif
};

// 校正值
struct MenuItem MI_EM_6_ID209_CorrectionValue[2]={
{&MI_EM_5_ID207_leftalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H1,0,11,utNone},
{&MI_EM_5_ID207_leftalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H2,0,11,utNone}
};

// 左对齐
struct MenuItem MI_EM_5_ID207_leftalignment[2]={
{&MI_EM_4_ID206_VSD2[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Left,11,utNone},
{&MI_EM_4_ID206_VSD2[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID209_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 校正值
struct MenuItem MI_EM_6_ID20E_CorrectionValue[2]={
{&MI_EM_5_ID20C_rightalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H1,0,11,utNone},
{&MI_EM_5_ID20C_rightalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H2,0,11,utNone}
};

// 右对齐
struct MenuItem MI_EM_5_ID20C_rightalignment[2]={
{&MI_EM_4_ID206_VSD2[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Right,11,utNone},
{&MI_EM_4_ID206_VSD2[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID20E_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD2
struct MenuItem MI_EM_4_ID206_VSD2[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID1FA_720DPI[1],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID207_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1FA_720DPI[1],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID20C_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#else
{&MI_EM_3_ID1FA_720DPI[1],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID207_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1FA_720DPI[1],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID20C_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#endif
};

// 校正值
struct MenuItem MI_EM_6_ID214_CorrectionValue[2]={
{&MI_EM_5_ID212_leftalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H1,0,12,utNone},
{&MI_EM_5_ID212_leftalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H2,0,12,utNone}
};

// 左对齐
struct MenuItem MI_EM_5_ID212_leftalignment[2]={
{&MI_EM_4_ID211_VSD3[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Left,12,utNone},
{&MI_EM_4_ID211_VSD3[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID214_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 校正值
struct MenuItem MI_EM_6_ID219_CorrectionValue[2]={
{&MI_EM_5_ID217_rightalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H1,0,12,utNone},
{&MI_EM_5_ID217_rightalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H2,0,12,utNone}
};

// 右对齐
struct MenuItem MI_EM_5_ID217_rightalignment[2]={
{&MI_EM_4_ID211_VSD3[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Right,12,utNone},
{&MI_EM_4_ID211_VSD3[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID219_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD3
struct MenuItem MI_EM_4_ID211_VSD3[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID1FA_720DPI[2],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID212_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1FA_720DPI[2],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID217_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#else
{&MI_EM_3_ID1FA_720DPI[2],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID212_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1FA_720DPI[2],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID217_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#endif
};

// 校正值
struct MenuItem MI_EM_6_ID21F_CorrectionValue[2]={
{&MI_EM_5_ID21D_leftalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H1,0,13,utNone},
{&MI_EM_5_ID21D_leftalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Left_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Left_H2,0,13,utNone}
};

// 左对齐
struct MenuItem MI_EM_5_ID21D_leftalignment[2]={
{&MI_EM_4_ID21C_VSD4[0],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Left,13,utNone},
{&MI_EM_4_ID21C_VSD4[0],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID21F_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// 校正值
struct MenuItem MI_EM_6_ID224_CorrectionValue[2]={
{&MI_EM_5_ID222_rightalignment[1],0,0,0,(unsigned short *)MenuStr_EM_7_ID1D2_H1,"H1",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H1,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H1,0,13,utNone},
{&MI_EM_5_ID222_rightalignment[1],1,0,0,(unsigned short *)MenuStr_EM_7_ID1D3_H2,"H2",0,0,chs,editor,normalStatus,0,tInt,3,-9999,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Angle_Right_H2,(UISetFloatFuncExType)Cali_EM_Set_Angle_Right_H2,0,13,utNone}
};

// 右对齐
struct MenuItem MI_EM_5_ID222_rightalignment[2]={
{&MI_EM_4_ID21C_VSD4[1],0,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_Angle_Right,13,utNone},
{&MI_EM_4_ID21C_VSD4[1],1,0,0,(unsigned short *)MenuStr_UM_4_ID2A_CorrectionValue,"Value",MI_EM_6_ID224_CorrectionValue,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
};

// VSD4
struct MenuItem MI_EM_4_ID21C_VSD4[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_3_ID1FA_720DPI[3],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID21D_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1FA_720DPI[3],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Rig Alignment",MI_EM_5_ID222_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#else
{&MI_EM_3_ID1FA_720DPI[3],0,0,0,(unsigned short *)MenuStr_EM_5_ID72_leftalignment,"Left Alignment",MI_EM_5_ID21D_leftalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_3_ID1FA_720DPI[3],1,0,0,(unsigned short *)MenuStr_EM_5_ID88_rightalignment,"Right Alignment",MI_EM_5_ID222_rightalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#endif
};

// 720DPI
struct MenuItem MI_EM_3_ID1FA_720DPI[4]={
{&MI_EM_2_ID1CC_AngleAdjustment[1],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",MI_EM_4_ID1FB_VSD1,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#if defined(HEAD_EPSON_GEN5)||defined(RICOH_VSD2)
{&MI_EM_2_ID1CC_AngleAdjustment[1],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_EM_4_ID206_VSD2,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#else
{&MI_EM_2_ID1CC_AngleAdjustment[1],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",MI_EM_4_ID206_VSD2,2,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,1,utNone},
#endif
{&MI_EM_2_ID1CC_AngleAdjustment[1],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",MI_EM_4_ID211_VSD3,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_EM_2_ID1CC_AngleAdjustment[1],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",MI_EM_4_ID21C_VSD4,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone}
};

#endif

// 角度检查
struct MenuItem MI_EM_2_ID1CC_AngleAdjustment[5]={
{&MI_EM_1_ID4B_ADJUSTMENT[8],0,0,0,(unsigned short *)MenuStr_DPI_Name[0],MenuStr_DPI_Name_E[0],MI_EM_3_ID1CD_360DPI,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[8],1,0,0,(unsigned short *)MenuStr_DPI_Name[1],MenuStr_DPI_Name_E[1],MI_EM_3_ID1FA_720DPI,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[8],2,0,0,(unsigned short *)MenuStr_DPI_Name[2],MenuStr_DPI_Name_E[2],0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,2,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[8],3,0,0,(unsigned short *)MenuStr_DPI_Name[3],MenuStr_DPI_Name_E[3],0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,3,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[8],4,0,0,(unsigned short *)MenuStr_DPI_Name[4],MenuStr_DPI_Name_E[4],0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,4,utNone}
};

// DPI
struct MenuItem MI_EM_3_ID22B_DPI[5]={
{&MI_EM_2_ID22A_TestingPrint[0],0,0,0,(unsigned short *)MenuStr_DPI_Name[0],MenuStr_DPI_Name_E[0],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_DPI,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_DPI,0,1,utNone},
{&MI_EM_2_ID22A_TestingPrint[0],1,0,0,(unsigned short *)MenuStr_DPI_Name[1],MenuStr_DPI_Name_E[1],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_DPI,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_DPI,0,0,utNone},
{&MI_EM_2_ID22A_TestingPrint[0],2,0,0,(unsigned short *)MenuStr_DPI_Name[2],MenuStr_DPI_Name_E[2],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,0,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_DPI,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_DPI,0,3,utNone},
{&MI_EM_2_ID22A_TestingPrint[0],3,0,0,(unsigned short *)MenuStr_DPI_Name[3],MenuStr_DPI_Name_E[3],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,0,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_DPI,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_DPI,0,2,utNone},
{&MI_EM_2_ID22A_TestingPrint[0],4,0,0,(unsigned short *)MenuStr_DPI_Name[4],MenuStr_DPI_Name_E[4],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,0,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_DPI,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_DPI,0,4,utNone}
};

// VSD
struct MenuItem MI_EM_3_ID231_VSD[4]={
{&MI_EM_2_ID22A_TestingPrint[1],0,0,0,(unsigned short *)MenuStr_UM_3_ID3E_VSD1,"VSD1",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_VSD,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_VSD,0,0,utNone},
#if defined(HEAD_EPSON_GEN5)||defined(RICOH_VSD2)
{&MI_EM_2_ID22A_TestingPrint[1],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_VSD,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_VSD,0,1,utNone},
#else
{&MI_EM_2_ID22A_TestingPrint[1],1,0,0,(unsigned short *)MenuStr_UM_3_ID41_VSD2,"VSD2",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,0,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_VSD,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_VSD,0,1,utNone},
#endif
{&MI_EM_2_ID22A_TestingPrint[1],2,0,0,(unsigned short *)MenuStr_UM_3_ID44_VSD3,"VSD3",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_VSD,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_VSD,0,2,utNone},
{&MI_EM_2_ID22A_TestingPrint[1],3,0,0,(unsigned short *)MenuStr_UM_3_ID47_VSD4,"VSD4",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_VSD,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_VSD,0,3,utNone}
};

// Pass数
struct MenuItem MI_EM_3_ID236_testPassNumber[6]={
{&MI_EM_2_ID22A_TestingPrint[2],0,0,0,(unsigned short *)MenuStr_EM_4_ID237_2,"2",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_PassNum,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_PassNum,0,0,utNone},
{&MI_EM_2_ID22A_TestingPrint[2],1,0,0,(unsigned short *)MenuStr_EM_4_ID237_3,"3",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_PassNum,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_PassNum,0,1,utNone},
{&MI_EM_2_ID22A_TestingPrint[2],2,0,0,(unsigned short *)MenuStr_EM_4_ID237_4,"4",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_PassNum,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_PassNum,0,2,utNone},
{&MI_EM_2_ID22A_TestingPrint[2],3,0,0,(unsigned short *)MenuStr_EM_4_ID238_6,"6",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_PassNum,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_PassNum,0,3,utNone},
{&MI_EM_2_ID22A_TestingPrint[2],4,0,0,(unsigned short *)MenuStr_EM_4_ID239_8,"8",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_PassNum,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_PassNum,0,4,utNone},
{&MI_EM_2_ID22A_TestingPrint[2],5,0,0,(unsigned short *)MenuStr_EM_4_ID23A_12,"12",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_TestingPrint_PassNum,(UISetFloatFuncExType)Cali_EM_Set_TestingPrint_PassNum,0,5,utNone}

};

// 测试打印
struct MenuItem MI_EM_2_ID22A_TestingPrint[4]={
{&MI_EM_1_ID4B_ADJUSTMENT[9],0,0,0,(unsigned short *)MenuStr_EM_3_ID22B_DPI,"DPI",MI_EM_3_ID22B_DPI,5,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[9],1,0,0,(unsigned short *)MenuStr_EM_3_ID231_VSD,"VSD",MI_EM_3_ID231_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[9],2,0,0,(unsigned short *)MenuStr_EM_3_ID236_PassNumber,"Pass Number",MI_EM_3_ID236_testPassNumber,6,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[9],3,0,0,(unsigned short *)MenuStr_UM_4_ID29_Print,"Print",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_TestingPrint,3,utNone}
};
#ifdef RIPSTAR_FLAT_EX

struct MenuItem MI_UM_2_ID27_XAdjustSpeed[3]={
{&MI_UM_2_ID27_Adjustsetting[1],0,0,0,(unsigned short *)MenuStr_2_ID0F_X_PRT_LOW_SPEED,"Low Speed",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCalcParam_RICOH_X_SPEED,(UISetFloatFuncExType)SetCalcParam_RICOH_X_SPEED,0,0,utNone},
{&MI_UM_2_ID27_Adjustsetting[1],1,0,0,(unsigned short *)MenuStr_2_ID0F_X_PRT_MIDDLE_SPEED,"Middle Speed",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCalcParam_RICOH_X_SPEED,(UISetFloatFuncExType)SetCalcParam_RICOH_X_SPEED,0,1,utNone},
{&MI_UM_2_ID27_Adjustsetting[1],2,0,0,(unsigned short *)MenuStr_2_ID0F_X_PRT_HIGH_SPEED,"High Speed",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCalcParam_RICOH_X_SPEED,(UISetFloatFuncExType)SetCalcParam_RICOH_X_SPEED,0,2,utNone}
};
struct MenuItem MI_UM_2_ID27_Adjustsetting[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_EM_1_ID4B_ADJUSTMENT[7],0,0,0,(unsigned short *)MenuStr_2_ID0F_Y_PRT_CONTINUE,"Y Continue On",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Y_PRT_CONTINUE_OnOff,(UISetFloatFuncExType)SetPRTParam_Y_PRT_CONTINUE_OnOff,0,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[7],1,0,0,(unsigned short *)MenuStr_2_ID0F_X_PRT_SPEED,"X Speed",MI_UM_2_ID27_XAdjustSpeed,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#else
{&MI_EM_1_ID4B_ADJUSTMENT[7],0,0,0,(unsigned short *)MenuStr_2_ID0F_Y_PRT_CONTINUE,"Y Print Continue On",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Y_PRT_CONTINUE_OnOff,(UISetFloatFuncExType)SetPRTParam_Y_PRT_CONTINUE_OnOff,0,0,utNone},
{&MI_EM_1_ID4B_ADJUSTMENT[7],1,0,0,(unsigned short *)MenuStr_2_ID0F_X_PRT_SPEED,"X Speed Selection",MI_UM_2_ID27_XAdjustSpeed,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone}
#endif
};
#endif
// 校准
struct MenuItem MI_EM_1_ID4B_ADJUSTMENT[]={
{&MI_0_ID00_MenuChilds[4],0,0,0,(unsigned short *)MenuStr_EM_2_ID51_DotCheck,"Dot Check",MI_EM_2_ID51_DotCheck,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[4],1,0,0,(unsigned short *)MenuStr_EM_2_ID56_InterleaveCheck,"Inter Leave",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_InterleaveCheck,1,utNone},
{&MI_0_ID00_MenuChilds[4],2,0,0,(unsigned short *)MenuStr_EM_2_ID57_HeadSlantCheck,"Head Slant",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_HeadSlantCheck,2,utNone},
{&MI_0_ID00_MenuChilds[4],3,0,0,(unsigned short *)MenuStr_EM_2_ID58_HeadIntervalCheck,"Head Interval",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_HeadIntervalCheck,3,utNone},
#ifdef RIPSTAR_FLAT_EX
#ifdef MANUFACTURER_DYSS	
{&MI_0_ID00_MenuChilds[4],4,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjust",MI_EM_2_ID59_Feed,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
#else
{&MI_0_ID00_MenuChilds[4],4,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjust",MI_EM_2_ID59_Feed,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
#endif
#else
{&MI_0_ID00_MenuChilds[4],4,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjust",MI_EM_2_ID59_Feed,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
#endif
{&MI_0_ID00_MenuChilds[4],5,0,0,(unsigned short *)MenuStr_UM_2_ID3E_Head_Space,"H Space Align",MI_EM_3_ID3E_Head_Space,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone},
{&MI_0_ID00_MenuChilds[4],6,0,0,(unsigned short *)MenuStr_EM_2_ID6F_Horizotalalignment,"Horizontal Adj",MI_EM_2_ID6F_Horizotalalignment,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone},
{&MI_0_ID00_MenuChilds[4],7,0,0,(unsigned short *)MenuStr_EM_2_ID1C5_Verticalalignment,"V Alignment",MI_EM_2_ID1C5_Verticalalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,7,utNone},
{&MI_0_ID00_MenuChilds[4],8,0,0,(unsigned short *)MenuStr_EM_2_ID1C5_Verticalcheck,"Vertical Check",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_VerticalAlign,8,utNone}
#ifdef HEAD_EPSON_GEN5
,
{&MI_0_ID00_MenuChilds[4],9,0,0,(unsigned short *)MenuStr_EM_2_ID1CC_AngleAdjustment,"Angle Adjust",MI_EM_2_ID1CC_AngleAdjustment,5,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,9,utNone}
#endif
//{&MI_0_ID00_MenuChilds[4],8,0,0,(unsigned short *)MenuStr_UM_2_ID4A_TestingPrint,"Testing Print",MI_EM_2_ID22A_TestingPrint,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,8,utNone}
#ifdef RIPSTAR_FLAT_EX
,
{&MI_0_ID00_MenuChilds[4],10,0,0,(unsigned short *)MenuStr_2_ID0F_AdjSetting,"Adjustment Set",MI_UM_2_ID27_Adjustsetting,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,9,utNone}
#endif
#else
{&MI_0_ID00_MenuChilds[4],1,0,0,(unsigned short *)MenuStr_EM_2_ID56_InterleaveCheck,"Interleave Check",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_InterleaveCheck,1,utNone},
{&MI_0_ID00_MenuChilds[4],2,0,0,(unsigned short *)MenuStr_EM_2_ID57_HeadSlantCheck,"Head Slant Check",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_HeadSlantCheck,2,utNone},
{&MI_0_ID00_MenuChilds[4],3,0,0,(unsigned short *)MenuStr_EM_2_ID58_HeadIntervalCheck,"Head Interval Check",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_HeadIntervalCheck,3,utNone},
#ifdef RIPSTAR_FLAT_EX
#ifdef MANUFACTURER_DYSS	
{&MI_0_ID00_MenuChilds[4],4,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjustment",MI_EM_2_ID59_Feed,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
#else
{&MI_0_ID00_MenuChilds[4],4,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjustment",MI_EM_2_ID59_Feed,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
#endif
#else
{&MI_0_ID00_MenuChilds[4],4,0,0,(unsigned short *)MenuStr_UM_2_ID27_FEEDadjust,"Feed Adjustment",MI_EM_2_ID59_Feed,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
#endif
{&MI_0_ID00_MenuChilds[4],5,0,0,(unsigned short *)MenuStr_UM_2_ID3E_Head_Space,"Head Space Alignment",MI_EM_3_ID3E_Head_Space,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone},
{&MI_0_ID00_MenuChilds[4],6,0,0,(unsigned short *)MenuStr_EM_2_ID6F_Horizotalalignment,"Horizontal adjustment",MI_EM_2_ID6F_Horizotalalignment,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone},
{&MI_0_ID00_MenuChilds[4],7,0,0,(unsigned short *)MenuStr_EM_2_ID1C5_Verticalalignment,"Vertical Alignment",MI_EM_2_ID1C5_Verticalalignment,2,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,7,utNone},
{&MI_0_ID00_MenuChilds[4],8,0,0,(unsigned short *)MenuStr_EM_2_ID1C5_Verticalcheck,"Vertical Check",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_EM_Print_VerticalAlign,8,utNone}
#ifdef HEAD_EPSON_GEN5
,
{&MI_0_ID00_MenuChilds[4],9,0,0,(unsigned short *)MenuStr_EM_2_ID1CC_AngleAdjustment,"Angle Adjustment",MI_EM_2_ID1CC_AngleAdjustment,5,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,9,utNone}
#endif
//{&MI_0_ID00_MenuChilds[4],8,0,0,(unsigned short *)MenuStr_UM_2_ID4A_TestingPrint,"Testing Print",MI_EM_2_ID22A_TestingPrint,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,8,utNone}
#ifdef RIPSTAR_FLAT_EX
,
{&MI_0_ID00_MenuChilds[4],10,0,0,(unsigned short *)MenuStr_2_ID0F_AdjSetting,"Adjustment Setting",MI_UM_2_ID27_Adjustsetting,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,9,utNone}
#endif
#endif
};

#if defined(EPSON_CLEAN_UPDOWN)||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT)
// 清洗模式
struct MenuItem MI_ALLWIN_UM_2_ID23D_Whichhead[5]={
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHeadAll,MenuStr_ALLWIN_UM_3_CleanHeadAll_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,0,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],1,0,0,(unsigned short *)MenuStr_Clean_Color_Name[0],MenuStr_Clean_Color_Name_E[0],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,1,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],2,0,0,(unsigned short *)MenuStr_Clean_Color_Name[1],MenuStr_Clean_Color_Name_E[1],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,2,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],3,0,0,(unsigned short *)MenuStr_Clean_Color_Name[2],MenuStr_Clean_Color_Name_E[2],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,3,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],4,0,0,(unsigned short *)MenuStr_Clean_Color_Name[3],MenuStr_Clean_Color_Name_E[3],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,4,utNone}
};
#elif defined (DYSS_UV_CLEAN)
struct MenuItem MI_ALLWIN_UM_2_ID23D_Whichhead[4]={
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHead1,MenuStr_ALLWIN_UM_3_CleanHead1_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,0,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],1,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHead2,MenuStr_ALLWIN_UM_3_CleanHead2_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,1,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],2,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHead3,MenuStr_ALLWIN_UM_3_CleanHead3_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,2,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],3,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHeadAll,MenuStr_ALLWIN_UM_3_CleanHeadAll_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,3,utNone}

};
#else
struct MenuItem MI_ALLWIN_UM_2_ID23D_Whichhead[3]={
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHead1,MenuStr_ALLWIN_UM_3_CleanHead1_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,0,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],1,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHead2,MenuStr_ALLWIN_UM_3_CleanHead2_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,1,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[0],2,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHeadAll,MenuStr_ALLWIN_UM_3_CleanHeadAll_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_HeadMask,0,2,utNone}
};
#endif
// 清洗方式
struct MenuItem MI_ALLWIN_EM_3_ID286_CleanType[4]={
{&MI_ALLWIN_EM_2_ID283_AutoClean[2],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID242_Powerful,"Strong",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_Way,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_Way,0,0,utNone},
{&MI_ALLWIN_EM_2_ID283_AutoClean[2],1,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID243_Normal,"Normal",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_Way,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_Way,0,1,utNone},
{&MI_ALLWIN_EM_2_ID283_AutoClean[2],2,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID244_Soft,"Weak",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_Way,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_Way,0,2,utNone},
{&MI_ALLWIN_EM_2_ID283_AutoClean[2],3,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID245_Fill,"Fill",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_Way,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_Way,0,3,utNone}
};
#ifdef DYSS_UV_CLEAN
struct MenuItem MI_ALLWIN_EM_2_ID283_AutoClean[3]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],0,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID284_AutoCleanOnOff,"Auto Spit On",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_OnOff,0,0,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],1,0,0,(unsigned short *)MenuStr_EM_3_ID236_PassNumber,"PassNum",0,0,chs,editor,normalStatus,0,tInt,4,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_Pass,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_Pass,0,1,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],2,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID286_CleanTime1,"Auto Spit Time",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_Flash_Time,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_Flash_Time,0,2,utNone}
#else
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],0,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID284_AutoCleanOnOff,"Auto Spitting On",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_OnOff,0,0,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],1,0,0,(unsigned short *)MenuStr_EM_3_ID236_PassNumber,"Pass Number",0,0,chs,editor,normalStatus,0,tInt,4,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_Pass,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_Pass,0,1,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],2,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID286_CleanTime1,"Auto Spitting Time",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_Flash_Time,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_Flash_Time,0,2,utNone}
#endif
};
#else
// 自动清洗
struct MenuItem MI_ALLWIN_EM_2_ID283_AutoClean[3]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],0,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID284_AutoCleanOnOff,"On/Off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_OnOff,0,0,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],1,0,0,(unsigned short *)MenuStr_EM_3_ID236_PassNumber,"PassNum",0,0,chs,editor,normalStatus,0,tInt,4,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_Pass,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_Pass,0,1,utNone},
#else
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],0,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID284_AutoCleanOnOff,"Auto Clean On/Off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_OnOff,0,0,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],1,0,0,(unsigned short *)MenuStr_EM_3_ID236_PassNumber,"Pass Number",0,0,chs,editor,normalStatus,0,tInt,4,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_AutoClean_Pass,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_AutoClean_Pass,0,1,utNone},
#endif
{&MI_ALLWIN_1_ID23C_HEADCLEANING[6],2,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID286_CleanType,"Clean Type",MI_ALLWIN_EM_3_ID286_CleanType,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
};
#endif
// 快捷方式
struct MenuItem MI_ALLWIN_EM_2_ID246_Shortcut[4]={
{&MI_ALLWIN_1_ID23C_HEADCLEANING[5],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID242_Powerful,"Strong",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_Shortcuts,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_Shortcuts,0,0,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[5],1,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID243_Normal,"Normal",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_Shortcuts,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_Shortcuts,0,1,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[5],2,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID244_Soft,"Weak",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_Shortcuts,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_Shortcuts,0,2,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[5],3,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID245_Fill,"Fill",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_Shortcuts,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_Shortcuts,0,3,utNone}
};
#ifdef MANUFACTURER_DGEN
struct MenuItem MI_ALLWIN_1_ID23C_HEADCLEANING[10]={
#else
struct MenuItem MI_ALLWIN_1_ID23C_HEADCLEANING[9]={
#endif
#if defined(EPSON_CLEAN_UPDOWN_TATE_8H_RICOH)||defined(EPSON_4H_CLEAN_SPLIT)
{&MI_0_ID00_MenuChilds[2],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID23D_Whichhead,"Clean Head",MI_ALLWIN_UM_2_ID23D_Whichhead,5,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#elif defined (DYSS_UV_CLEAN) ||defined (RICOH_3H)
{&MI_0_ID00_MenuChilds[2],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID23D_Whichhead,"Clean Head",MI_ALLWIN_UM_2_ID23D_Whichhead,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#else
{&MI_0_ID00_MenuChilds[2],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID23D_Whichhead,"Clean Head",MI_ALLWIN_UM_2_ID23D_Whichhead,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#endif
{&MI_0_ID00_MenuChilds[2],1,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID242_Powerful,"Strong",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoClean_EPSON_ALLWIN,0,utNone},
{&MI_0_ID00_MenuChilds[2],2,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID243_Normal,"Normal",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoClean_EPSON_ALLWIN,1,utNone},
{&MI_0_ID00_MenuChilds[2],3,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID244_Soft,"Weak",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoClean_EPSON_ALLWIN,2,utNone},
{&MI_0_ID00_MenuChilds[2],4,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID245_Fill,"Fill",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoClean_EPSON_ALLWIN,3,utNone},
{&MI_0_ID00_MenuChilds[2],5,0,0,(unsigned short *)MenuStr_ALLWIN_EM_2_ID246_Shortcut,"Shortcut",MI_ALLWIN_EM_2_ID246_Shortcut,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
{&MI_0_ID00_MenuChilds[2],6,0,0,(unsigned short *)MenuStr_ALLWIN_EM_2_ID283_AutoClean,"Auto Clean",MI_ALLWIN_EM_2_ID283_AutoClean,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone},
#ifdef DYSS_UV_CLEAN
{&MI_0_ID00_MenuChilds[2],7,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID286_CleanTime2,"Spitting Time",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_ALLWIN_Flash_Time,(UISetFloatFuncExType)SetCleanParam_EPSON_ALLWIN_Flash_Time,0,6,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[2],8,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID3F0_Flash,"Manual Spit",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoFlash_EPSON_ALLWIN,7,utNone}
#else
{&MI_0_ID00_MenuChilds[2],8,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID3F0_Flash,"Manual Spitting",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoFlash_EPSON_ALLWIN,7,utNone}
#endif
#else
{&MI_0_ID00_MenuChilds[2],7,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID245_Flash,"Flash",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoFlash_EPSON_ALLWIN,6,utNone},
{&MI_0_ID00_MenuChilds[2],8,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID245_Clean_Ink_Box,"Ink Box Clean",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoClean_InkBox_EPSON_ALLWIN,7,utNone}
#endif
#ifdef MANUFACTURER_DGEN
,{&MI_0_ID00_MenuChilds[2],9,0,0,(unsigned short *)MenuStrIdleClean,MenuStrIdleClean_EN,MI_ALLWIN_EM_2_ID283_IdleClean,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,9,utNone}
#endif
};

#ifdef MANUFACTURER_DGEN
//空闲清洗
struct MenuItem MI_ALLWIN_EM_2_ID283_IdleClean[2] = {
{&MI_ALLWIN_1_ID23C_HEADCLEANING[7],0,0,0,(unsigned short *)MenuStrIdleCleanOnOff,MenuStrIdleCleanOnOff_EN,0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_IdleClean_OnOff,(UISetFloatFuncExType)SetCleanParam_IdleClean_OnOff,0,0,utNone},
{&MI_ALLWIN_1_ID23C_HEADCLEANING[7],0,0,0,(unsigned short *)MenuStrIdleCleanIntervalTime,MenuStrIdleCleanIntervalTime_EN,0,0,chs,editor,normalStatus,0,tInt,3,0,0,0,1,{10},(UIGetFloatFuncExType)GetCleanParam_IdleClean_Time,(UISetFloatFuncExType)SetCleanParam_IdleClean_Time,0,1,utH}
};
#endif // MANUFACTURER_DGEN
#elif defined(EPSON_CLEAN_INTEGRATE)  ||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2)||defined(EPSON_CLEAN_INTEGRATE_3)
// 清洗方式
struct MenuItem MI_ALLWIN_EM_3_ID286_CleanType[4]={
{&MI_ALLWIN_EM_2_ID283_AutoClean[2],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID242_Powerful,"Strong",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoClean_Way,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoClean_Way,0,0,utNone},
{&MI_ALLWIN_EM_2_ID283_AutoClean[2],1,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID243_Normal,"Normal",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoClean_Way,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoClean_Way,0,1,utNone},
{&MI_ALLWIN_EM_2_ID283_AutoClean[2],2,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID244_Soft,"Weak",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoClean_Way,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoClean_Way,0,2,utNone},
{&MI_ALLWIN_EM_2_ID283_AutoClean[2],3,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID245_Fill,"Fill",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoClean_Way,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoClean_Way,0,3,utNone}
};
// 自动清洗
struct MenuItem MI_ALLWIN_EM_2_ID283_AutoClean[3]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_MICOLOR_1_ID28B_HEADCLEANING[9],0,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID284_AutoCleanOnOff,"A Clean On/Off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoClean_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoClean_OnOff,0,0,utNone},
#else
{&MI_MICOLOR_1_ID28B_HEADCLEANING[9],0,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID284_AutoCleanOnOff,"Auto Clean On/Off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoClean_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoClean_OnOff,0,0,utNone},
#endif
{&MI_MICOLOR_1_ID28B_HEADCLEANING[9],1,0,0,(unsigned short *)MenuStr_EM_3_ID236_PassNumber,"Pass Number",0,0,chs,editor,normalStatus,0,tInt,4,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoClean_Pass,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoClean_Pass,0,1,utNone},
{&MI_MICOLOR_1_ID28B_HEADCLEANING[9],2,0,0,(unsigned short *)MenuStr_ALLWIN_EM_3_ID286_CleanType,"Clean Type",MI_ALLWIN_EM_3_ID286_CleanType,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone}
};
// 清洗模式
struct MenuItem MI_MICOLOR_UM_2_ID23E_Whichhead[3]={
{&MI_MICOLOR_1_ID28B_HEADCLEANING[0],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHead1,MenuStr_ALLWIN_UM_3_CleanHead1_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_HeadMask,0,0,utNone},
{&MI_MICOLOR_1_ID28B_HEADCLEANING[0],1,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHead2,MenuStr_ALLWIN_UM_3_CleanHead2_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_HeadMask,0,1,utNone},
{&MI_MICOLOR_1_ID28B_HEADCLEANING[0],2,0,0,(unsigned short *)MenuStr_ALLWIN_UM_3_CleanHeadAll,MenuStr_ALLWIN_UM_3_CleanHeadAll_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_HeadMask,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_HeadMask,0,2,utNone}
};
// 快捷方式
struct MenuItem MI_MICOLOR_EM_2_ID292_Shortcut[4]={
{&MI_MICOLOR_1_ID28B_HEADCLEANING[5],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID242_Powerful,"Strong",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_Shortcuts,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_Shortcuts,0,0,utNone},
{&MI_MICOLOR_1_ID28B_HEADCLEANING[5],1,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID243_Normal,"Normal",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_Shortcuts,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_Shortcuts,0,1,utNone},
{&MI_MICOLOR_1_ID28B_HEADCLEANING[5],2,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID244_Soft,"Weak",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_Shortcuts,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_Shortcuts,0,2,utNone},
{&MI_MICOLOR_1_ID28B_HEADCLEANING[5],3,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID245_Fill,"Fill",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_Shortcuts,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_Shortcuts,0,3,utNone}
};
// 自动闪喷
struct MenuItem MI_MICOLOR_UM_2_ID3F0_AutoFlash[2]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_MICOLOR_1_ID28B_HEADCLEANING[6],0,0,0,(unsigned short *)MenuStr_MICOLOR_EM_3_ID3F1_AutoFlashOnOff,"On/Off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoSpray_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoSpray_OnOff,0,0,utNone},
{&MI_MICOLOR_1_ID28B_HEADCLEANING[6],0,0,0,(unsigned short *)MenuStr_MICOLOR_EM_3_ID3F1_AutoFlashPassNum,"PassNum",0,0,chs,editor,normalStatus,0,tInt,3,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoSpray_Pass,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoSpray_Pass,0,0,utNone},
#else
{&MI_MICOLOR_1_ID28B_HEADCLEANING[6],0,0,0,(unsigned short *)MenuStr_MICOLOR_EM_3_ID3F1_AutoFlashOnOff,"Auto Flash On/Off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoSpray_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoSpray_OnOff,0,0,utNone},
{&MI_MICOLOR_1_ID28B_HEADCLEANING[6],0,0,0,(unsigned short *)MenuStr_MICOLOR_EM_3_ID3F1_AutoFlashPassNum,"Pass Number",0,0,chs,editor,normalStatus,0,tInt,3,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_AutoSpray_Pass,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_AutoSpray_Pass,0,0,utNone},
#endif
};
struct MenuItem MI_MICOLOR_1_ID28B_HEADCLEANING[10]={
{&MI_0_ID00_MenuChilds[2],0,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID23D_Whichhead,"Clean Head",MI_MICOLOR_UM_2_ID23E_Whichhead,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_0_ID00_MenuChilds[2],1,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID242_Powerful,"Strong",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoClean_EPSON_MICOLOR,1,utNone},
{&MI_0_ID00_MenuChilds[2],2,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID243_Normal,"Normal",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoClean_EPSON_MICOLOR,2,utNone},
{&MI_0_ID00_MenuChilds[2],3,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID244_Soft,"Weak",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoClean_EPSON_MICOLOR,3,utNone},
{&MI_0_ID00_MenuChilds[2],4,0,0,(unsigned short *)MenuStr_ALLWIN_UM_2_ID245_Fill,"Fill",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoClean_EPSON_MICOLOR,4,utNone},
{&MI_0_ID00_MenuChilds[2],5,0,0,(unsigned short *)MenuStr_ALLWIN_EM_2_ID246_Shortcut,"Shortcut",MI_MICOLOR_EM_2_ID292_Shortcut,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone},
{&MI_0_ID00_MenuChilds[2],6,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID3F0_AutoFlash,"Auto Flash",MI_MICOLOR_UM_2_ID3F0_AutoFlash,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone},
{&MI_0_ID00_MenuChilds[2],7,0,0,(unsigned short *)MenuStr_MICOLOR_EM_2_ID3F0_Flash,"Flash",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoFlash_EPSON_MICOLOR,7,utNone},
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[2],8,0,0,(unsigned short *)MenuStr_MICOLOR_EM_3_ID3F1_CleanFillProtect,"C Fill Protect",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_FILL_PROTECT_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_FILL_PROTECT_OnOff,0,8,utNone},
#else
{&MI_0_ID00_MenuChilds[2],8,0,0,(unsigned short *)MenuStr_MICOLOR_EM_3_ID3F1_CleanFillProtect,"Clean Fill Protect",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetCleanParam_EPSON_MICOLOR_FILL_PROTECT_OnOff,(UISetFloatFuncExType)SetCleanParam_EPSON_MICOLOR_FILL_PROTECT_OnOff,0,8,utNone},
#endif
{&MI_0_ID00_MenuChilds[2],9,0,0,(unsigned short *)MenuStr_ALLWIN_EM_2_ID283_AutoClean,"Auto Clean",MI_ALLWIN_EM_2_ID283_AutoClean,3,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,9,utNone}
};
#endif

#ifdef HEAD_RICOH_G4
struct MenuItem MI_EM_1_ID3F3_Nozzleparameter_EX[MAX_HEAD_DRIVE_NUM*2]={
{&MI_0_ID00_MenuChilds[11],0,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[0],MenuStr_EM_4_ID1C8_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,0,utNone},
{&MI_0_ID00_MenuChilds[11],1,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[0],MenuStr_EM_4_ID1C8_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,1,utNone},
{&MI_0_ID00_MenuChilds[11],2,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[1],MenuStr_EM_4_ID1C8_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,2,utNone},
{&MI_0_ID00_MenuChilds[11],3,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[1],MenuStr_EM_4_ID1C8_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,3,utNone}
};
#endif
// 喷头参数
#ifdef HEAD_RICOH_G4
struct MenuItem MI_EM_1_ID3F3_Nozzleparameter[MAX_HEAD_DRIVE_NUM]={
#else
struct MenuItem MI_EM_1_ID3F3_Nozzleparameter[MAX_HEAD_DRIVE_NUM]={
#endif
{&MI_0_ID00_MenuChilds[11],0,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[0],MenuStr_EM_4_ID1C8_E[0],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,0,utNone},
{&MI_0_ID00_MenuChilds[11],1,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[1],MenuStr_EM_4_ID1C8_E[1],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,1,utNone},
{&MI_0_ID00_MenuChilds[11],2,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[2],MenuStr_EM_4_ID1C8_E[2],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,2,utNone},
{&MI_0_ID00_MenuChilds[11],3,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[3],MenuStr_EM_4_ID1C8_E[3],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,3,utNone}
#if defined(EPSON_4H)||defined(HEAD_RICOH_G4)
,{&MI_0_ID00_MenuChilds[11],4,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[4],MenuStr_EM_4_ID1C8_E[4],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,4,utNone},
{&MI_0_ID00_MenuChilds[11],5,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[5],MenuStr_EM_4_ID1C8_E[5],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,5,utNone},
{&MI_0_ID00_MenuChilds[11],6,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[6],MenuStr_EM_4_ID1C8_E[6],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,6,utNone},
{&MI_0_ID00_MenuChilds[11],7,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[7],MenuStr_EM_4_ID1C8_E[7],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,7,utNone}
#endif
#if defined(RICOH_G5_3H)||defined(RICOH_G5_4H)
,{&MI_0_ID00_MenuChilds[11],8,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[8],MenuStr_EM_4_ID1C8_E[8],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,8,utNone},
{&MI_0_ID00_MenuChilds[11],9,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[9],MenuStr_EM_4_ID1C8_E[9],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,9,utNone},
{&MI_0_ID00_MenuChilds[11],10,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[10],MenuStr_EM_4_ID1C8_E[10],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,10,utNone},
{&MI_0_ID00_MenuChilds[11],11,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[11],MenuStr_EM_4_ID1C8_E[11],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,11,utNone}
#endif
#if defined(RICOH_G5_4H)
,{&MI_0_ID00_MenuChilds[11],12,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[12],MenuStr_EM_4_ID1C8_E[12],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,12,utNone},
{&MI_0_ID00_MenuChilds[11],13,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[13],MenuStr_EM_4_ID1C8_E[13],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,13,utNone},
{&MI_0_ID00_MenuChilds[11],14,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[14],MenuStr_EM_4_ID1C8_E[14],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,14,utNone},
{&MI_0_ID00_MenuChilds[11],15,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[15],MenuStr_EM_4_ID1C8_E[15],0,0,chs,editor,normalStatus,0,tInt,3,-50,0,0,1,{0},(UIGetFloatFuncExType)GetHeadParameter,(UISetFloatFuncExType)SetHeadParameter,0,15,utNone}
#endif
};

struct MenuItem MI_EM_1_ID3F3_HeadSwitch[2]={	
{&MI_0_ID00_MenuChilds[12],0,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_DisableHead1,MenuStr_EM_1_ID3F3_DisableHead1_E,0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead1ActiveCfg,(UISetFloatFuncExType)Menu_SetHead1ActiveCfg,0,0,utNone},
{&MI_0_ID00_MenuChilds[12],1,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_DisableHead2,MenuStr_EM_1_ID3F3_DisableHead2_E,0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead2ActiveCfg,(UISetFloatFuncExType)Menu_SetHead2ActiveCfg,0,1,utNone}
};
#ifdef EPSON_4H
struct MenuItem MI_EM_1_ID3F3_Head4Switch[8]={	
{&MI_0_ID00_MenuChilds[12],0,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_4DisableHead,MenuStr_EM_1_ID3F3_4DisableHead_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead4Active,(UISetFloatFuncExType)Menu_SetHead4Active,0,0,utNone},
{&MI_0_ID00_MenuChilds[12],1,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_4DisableHead4,MenuStr_EM_1_ID3F3_4DisableHead4_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead4Active,(UISetFloatFuncExType)Menu_SetHead4Active,0,1,utNone},
{&MI_0_ID00_MenuChilds[12],2,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_4DisableHead12,MenuStr_EM_1_ID3F3_4DisableHead12_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead4Active,(UISetFloatFuncExType)Menu_SetHead4Active,0,2,utNone},
{&MI_0_ID00_MenuChilds[12],3,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_4DisableHead34,MenuStr_EM_1_ID3F3_4DisableHead34_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead4Active,(UISetFloatFuncExType)Menu_SetHead4Active,0,3,utNone},
{&MI_0_ID00_MenuChilds[12],4,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_4DisableHead234,MenuStr_EM_1_ID3F3_4DisableHead234_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead4Active,(UISetFloatFuncExType)Menu_SetHead4Active,0,4,utNone},
{&MI_0_ID00_MenuChilds[12],5,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_4DisableHead134,MenuStr_EM_1_ID3F3_4DisableHead134_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead4Active,(UISetFloatFuncExType)Menu_SetHead4Active,0,5,utNone},
{&MI_0_ID00_MenuChilds[12],6,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_4DisableHead124,MenuStr_EM_1_ID3F3_4DisableHead124_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead4Active,(UISetFloatFuncExType)Menu_SetHead4Active,0,6,utNone},
{&MI_0_ID00_MenuChilds[12],7,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_4DisableHead123,MenuStr_EM_1_ID3F3_4DisableHead123_E,0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHead4Active,(UISetFloatFuncExType)Menu_SetHead4Active,0,7,utNone}
};
#endif

#ifdef FUNC_WAVE_MAP
struct MenuItem MI_Sys_WaveMap[MAX_HEAD_DRIVE_NUM] = {
{&MI_1_ID3FB_System[6],0,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[0],MenuStr_EM_4_ID1C8_E[0],MI_Wave_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_1_ID3FB_System[6],1,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[1],MenuStr_EM_4_ID1C8_E[1],MI_Wave_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_1_ID3FB_System[6],2,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[2],MenuStr_EM_4_ID1C8_E[2],MI_Wave_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_1_ID3FB_System[6],3,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[3],MenuStr_EM_4_ID1C8_E[3],MI_Wave_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone}
#if defined(EPSON_4H)||defined(HEAD_RICOH_G4)
,{&MI_1_ID3FB_System[6],4,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[4],MenuStr_EM_4_ID1C8_E[4],MI_Wave_VSD,4,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
{&MI_1_ID3FB_System[6],5,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[5],MenuStr_EM_4_ID1C8_E[5],MI_Wave_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone},
{&MI_1_ID3FB_System[6],6,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[6],MenuStr_EM_4_ID1C8_E[6],MI_Wave_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone},
{&MI_1_ID3FB_System[6],7,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[7],MenuStr_EM_4_ID1C8_E[7],MI_Wave_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,7,utNone}
#endif
#ifdef RICOH_G5_3H
,{&MI_1_ID3FB_System[6],8,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[8],MenuStr_EM_4_ID1C8_E[8],MI_Wave_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,8,utNone},
{&MI_1_ID3FB_System[6],9,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[9],MenuStr_EM_4_ID1C8_E[9],MI_Wave_VSD,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,9,utNone},
{&MI_1_ID3FB_System[6],10,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[10],MenuStr_EM_4_ID1C8_E[10],MI_Wave_VSD,4,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,10,utNone},
{&MI_1_ID3FB_System[6],11,0,0,(unsigned short *)MenuStr_EM_4_ID1C8[11],MenuStr_EM_4_ID1C8_E[11],MI_Wave_VSD,4,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,11,utNone}
#endif
};

struct MenuItem MI_Wave_VSD[4] = {
{&MI_Sys_WaveMap[0],0,0,0,(unsigned short *)MenuStr_Wave_VSD_CN[0],MenuStr_Wave_VSD_EN[0],MI_VSD_WaveName,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_Sys_WaveMap[0],1,0,0,(unsigned short *)MenuStr_Wave_VSD_CN[1],MenuStr_Wave_VSD_EN[1],MI_VSD_WaveName,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_Sys_WaveMap[0],2,0,0,(unsigned short *)MenuStr_Wave_VSD_CN[2],MenuStr_Wave_VSD_EN[2],MI_VSD_WaveName,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_Sys_WaveMap[0],3,0,0,(unsigned short *)MenuStr_Wave_VSD_CN[3],MenuStr_Wave_VSD_EN[3],MI_VSD_WaveName,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
};

struct MenuItem MI_VSD_WaveName[8] = {
{&MI_Wave_VSD[0],0,0,0,(unsigned short *)MenuStr_VSD_WaveName_CN[0],MenuStr_VSD_WaveName_EN[0],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetUIWaveMap,(UISetFloatFuncExType)SetUIWaveMap,0,0,utNone},
{&MI_Wave_VSD[0],1,0,0,(unsigned short *)MenuStr_VSD_WaveName_CN[1],MenuStr_VSD_WaveName_EN[1],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetUIWaveMap,(UISetFloatFuncExType)SetUIWaveMap,0,1,utNone},
{&MI_Wave_VSD[0],2,0,0,(unsigned short *)MenuStr_VSD_WaveName_CN[2],MenuStr_VSD_WaveName_EN[2],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetUIWaveMap,(UISetFloatFuncExType)SetUIWaveMap,0,2,utNone},
{&MI_Wave_VSD[0],3,0,0,(unsigned short *)MenuStr_VSD_WaveName_CN[3],MenuStr_VSD_WaveName_EN[3],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetUIWaveMap,(UISetFloatFuncExType)SetUIWaveMap,0,3,utNone},
{&MI_Wave_VSD[0],4,0,0,(unsigned short *)MenuStr_VSD_WaveName_CN[4],MenuStr_VSD_WaveName_EN[4],0,0,chs,radiocheckbox,normalStatus,0,tInt,9999,-9999,0,0,1,{0},(UIGetFloatFuncExType)GetUIWaveMap,(UISetFloatFuncExType)SetUIWaveMap,0,4,utNone},
{&MI_Wave_VSD[0],5,0,0,(unsigned short *)MenuStr_VSD_WaveName_CN[5],MenuStr_VSD_WaveName_EN[5],0,0,chs,radiocheckbox,normalStatus,0,tInt,9999,-9999,0,0,1,{0},(UIGetFloatFuncExType)GetUIWaveMap,(UISetFloatFuncExType)SetUIWaveMap,0,5,utNone},
{&MI_Wave_VSD[0],6,0,0,(unsigned short *)MenuStr_VSD_WaveName_CN[6],MenuStr_VSD_WaveName_EN[6],0,0,chs,radiocheckbox,normalStatus,0,tInt,9999,-9999,0,0,1,{0},(UIGetFloatFuncExType)GetUIWaveMap,(UISetFloatFuncExType)SetUIWaveMap,0,6,utNone},
{&MI_Wave_VSD[0],7,0,0,(unsigned short *)MenuStr_VSD_WaveName_CN[7],MenuStr_VSD_WaveName_EN[7],0,0,chs,radiocheckbox,normalStatus,0,tInt,9999,-9999,0,0,1,{0},(UIGetFloatFuncExType)GetUIWaveMap,(UISetFloatFuncExType)SetUIWaveMap,0,7,utNone},
};
#endif
// 语言
struct MenuItem MI_1_ID3F8_Language[2]={
{&MI_0_ID00_MenuChilds[13],0,0,0,(unsigned short *)MenuStr_2_ID3F9_English,"English",0,0,chs,radiocheckbox,normalStatus,0,tInt,9999,-9999,0,0,1,{0},(UIGetFloatFuncExType)GetUILanguage,(UISetFloatFuncExType)SetUILanguage,0,0,utNone},
{&MI_0_ID00_MenuChilds[13],1,0,0,(unsigned short *)MenuStr_2_ID3FA_Chinese,"Chinese",0,0,chs,radiocheckbox,normalStatus,0,tInt,9999,-9999,0,0,1,{0},(UIGetFloatFuncExType)GetUILanguage,(UISetFloatFuncExType)SetUILanguage,0,1,utNone}
};

// SpotColor NameList for Spotcolor1
struct MenuItem MI_EM_3_ID236_Spot1Name[20]={
{&MI_1_ID3FB_SpotList[0],0,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[0],MenuStr_EM_4_ID237_ColorName_E[0],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,0,utNone},
{&MI_1_ID3FB_SpotList[0],1,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[1],MenuStr_EM_4_ID237_ColorName_E[1],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,1,utNone},
{&MI_1_ID3FB_SpotList[0],2,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[2],MenuStr_EM_4_ID237_ColorName_E[2],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,2,utNone},
{&MI_1_ID3FB_SpotList[0],3,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[3],MenuStr_EM_4_ID237_ColorName_E[3],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,3,utNone},
{&MI_1_ID3FB_SpotList[0],4,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[4],MenuStr_EM_4_ID237_ColorName_E[4],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,4,utNone},
{&MI_1_ID3FB_SpotList[0],5,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[5],MenuStr_EM_4_ID237_ColorName_E[5],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,5,utNone},
{&MI_1_ID3FB_SpotList[0],6,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[6],MenuStr_EM_4_ID237_ColorName_E[6],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,6,utNone},
{&MI_1_ID3FB_SpotList[0],7,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[7],MenuStr_EM_4_ID237_ColorName_E[7],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,7,utNone},
{&MI_1_ID3FB_SpotList[0],8,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[8],MenuStr_EM_4_ID237_ColorName_E[8],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,8,utNone},
{&MI_1_ID3FB_SpotList[0],9,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[9],MenuStr_EM_4_ID237_ColorName_E[9],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,9,utNone},
{&MI_1_ID3FB_SpotList[0],10,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[10],MenuStr_EM_4_ID237_ColorName_E[10],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,10,utNone},
{&MI_1_ID3FB_SpotList[0],11,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[11],MenuStr_EM_4_ID237_ColorName_E[11],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,11,utNone},
{&MI_1_ID3FB_SpotList[0],12,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[12],MenuStr_EM_4_ID237_ColorName_E[12],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,12,utNone},
{&MI_1_ID3FB_SpotList[0],13,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[13],MenuStr_EM_4_ID237_ColorName_E[13],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,13,utNone},
{&MI_1_ID3FB_SpotList[0],14,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[14],MenuStr_EM_4_ID237_ColorName_E[14],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,14,utNone},
{&MI_1_ID3FB_SpotList[0],15,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[15],MenuStr_EM_4_ID237_ColorName_E[15],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,15,utNone},
{&MI_1_ID3FB_SpotList[0],16,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[16],MenuStr_EM_4_ID237_ColorName_E[16],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,16,utNone},
{&MI_1_ID3FB_SpotList[0],17,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[17],MenuStr_EM_4_ID237_ColorName_E[17],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,17,utNone},
{&MI_1_ID3FB_SpotList[0],18,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[18],MenuStr_EM_4_ID237_ColorName_E[18],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,18,utNone},
{&MI_1_ID3FB_SpotList[0],19,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[19],MenuStr_EM_4_ID237_ColorName_E[19],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,19,utNone}
};

// SpotColor NameList for Spotcolor2
struct MenuItem MI_EM_3_ID236_Spot2Name[20]={
{&MI_1_ID3FB_SpotList[1],0,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[0],MenuStr_EM_4_ID237_ColorName_E[0],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,100,utNone},
{&MI_1_ID3FB_SpotList[1],1,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[1],MenuStr_EM_4_ID237_ColorName_E[1],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,101,utNone},
{&MI_1_ID3FB_SpotList[1],2,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[2],MenuStr_EM_4_ID237_ColorName_E[2],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,102,utNone},
{&MI_1_ID3FB_SpotList[1],3,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[3],MenuStr_EM_4_ID237_ColorName_E[3],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,103,utNone},
{&MI_1_ID3FB_SpotList[1],4,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[4],MenuStr_EM_4_ID237_ColorName_E[4],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,104,utNone},
{&MI_1_ID3FB_SpotList[1],5,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[5],MenuStr_EM_4_ID237_ColorName_E[5],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,105,utNone},
{&MI_1_ID3FB_SpotList[1],6,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[6],MenuStr_EM_4_ID237_ColorName_E[6],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,106,utNone},
{&MI_1_ID3FB_SpotList[1],7,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[7],MenuStr_EM_4_ID237_ColorName_E[7],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,107,utNone},
{&MI_1_ID3FB_SpotList[1],8,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[8],MenuStr_EM_4_ID237_ColorName_E[8],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,108,utNone},
{&MI_1_ID3FB_SpotList[1],9,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[9],MenuStr_EM_4_ID237_ColorName_E[9],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,109,utNone},
{&MI_1_ID3FB_SpotList[1],10,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[10],MenuStr_EM_4_ID237_ColorName_E[10],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,110,utNone},
{&MI_1_ID3FB_SpotList[1],11,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[11],MenuStr_EM_4_ID237_ColorName_E[11],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,111,utNone},
{&MI_1_ID3FB_SpotList[1],12,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[12],MenuStr_EM_4_ID237_ColorName_E[12],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,112,utNone},
{&MI_1_ID3FB_SpotList[1],13,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[13],MenuStr_EM_4_ID237_ColorName_E[13],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,113,utNone},
{&MI_1_ID3FB_SpotList[1],14,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[14],MenuStr_EM_4_ID237_ColorName_E[14],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,114,utNone},
{&MI_1_ID3FB_SpotList[1],15,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[15],MenuStr_EM_4_ID237_ColorName_E[15],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,115,utNone},
{&MI_1_ID3FB_SpotList[1],16,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[16],MenuStr_EM_4_ID237_ColorName_E[16],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,116,utNone},
{&MI_1_ID3FB_SpotList[1],17,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[17],MenuStr_EM_4_ID237_ColorName_E[17],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,117,utNone},
{&MI_1_ID3FB_SpotList[1],18,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[18],MenuStr_EM_4_ID237_ColorName_E[18],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,118,utNone},
{&MI_1_ID3FB_SpotList[1],19,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[19],MenuStr_EM_4_ID237_ColorName_E[19],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,119,utNone}
};

// SpotColor NameList for Spotcolor3
struct MenuItem MI_EM_3_ID236_Spot3Name[20]={
{&MI_1_ID3FB_SpotList[2],0,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[0],MenuStr_EM_4_ID237_ColorName_E[0],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,200,utNone},
{&MI_1_ID3FB_SpotList[2],1,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[1],MenuStr_EM_4_ID237_ColorName_E[1],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,201,utNone},
{&MI_1_ID3FB_SpotList[2],2,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[2],MenuStr_EM_4_ID237_ColorName_E[2],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,202,utNone},
{&MI_1_ID3FB_SpotList[2],3,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[3],MenuStr_EM_4_ID237_ColorName_E[3],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,203,utNone},
{&MI_1_ID3FB_SpotList[2],4,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[4],MenuStr_EM_4_ID237_ColorName_E[4],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,204,utNone},
{&MI_1_ID3FB_SpotList[2],5,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[5],MenuStr_EM_4_ID237_ColorName_E[5],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,205,utNone},
{&MI_1_ID3FB_SpotList[2],6,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[6],MenuStr_EM_4_ID237_ColorName_E[6],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,206,utNone},
{&MI_1_ID3FB_SpotList[2],7,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[7],MenuStr_EM_4_ID237_ColorName_E[7],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,207,utNone},
{&MI_1_ID3FB_SpotList[2],8,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[8],MenuStr_EM_4_ID237_ColorName_E[8],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,208,utNone},
{&MI_1_ID3FB_SpotList[2],9,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[9],MenuStr_EM_4_ID237_ColorName_E[9],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,209,utNone},
{&MI_1_ID3FB_SpotList[2],10,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[10],MenuStr_EM_4_ID237_ColorName_E[10],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,210,utNone},
{&MI_1_ID3FB_SpotList[2],11,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[11],MenuStr_EM_4_ID237_ColorName_E[11],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,211,utNone},
{&MI_1_ID3FB_SpotList[2],12,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[12],MenuStr_EM_4_ID237_ColorName_E[12],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,212,utNone},
{&MI_1_ID3FB_SpotList[2],13,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[13],MenuStr_EM_4_ID237_ColorName_E[13],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,213,utNone},
{&MI_1_ID3FB_SpotList[2],14,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[14],MenuStr_EM_4_ID237_ColorName_E[14],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,214,utNone},
{&MI_1_ID3FB_SpotList[2],15,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[15],MenuStr_EM_4_ID237_ColorName_E[15],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,215,utNone},
{&MI_1_ID3FB_SpotList[2],16,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[16],MenuStr_EM_4_ID237_ColorName_E[16],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,216,utNone},
{&MI_1_ID3FB_SpotList[2],17,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[17],MenuStr_EM_4_ID237_ColorName_E[17],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,217,utNone},
{&MI_1_ID3FB_SpotList[2],18,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[18],MenuStr_EM_4_ID237_ColorName_E[18],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,218,utNone},
{&MI_1_ID3FB_SpotList[2],19,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[19],MenuStr_EM_4_ID237_ColorName_E[19],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,219,utNone}
};

// SpotColor NameList for Spotcolor4
struct MenuItem MI_EM_3_ID236_Spot4Name[20]={
{&MI_1_ID3FB_SpotList[3],0,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[0],MenuStr_EM_4_ID237_ColorName_E[0],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,300,utNone},
{&MI_1_ID3FB_SpotList[3],1,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[1],MenuStr_EM_4_ID237_ColorName_E[1],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,301,utNone},
{&MI_1_ID3FB_SpotList[3],2,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[2],MenuStr_EM_4_ID237_ColorName_E[2],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,302,utNone},
{&MI_1_ID3FB_SpotList[3],3,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[3],MenuStr_EM_4_ID237_ColorName_E[3],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,303,utNone},
{&MI_1_ID3FB_SpotList[3],4,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[4],MenuStr_EM_4_ID237_ColorName_E[4],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,304,utNone},
{&MI_1_ID3FB_SpotList[3],5,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[5],MenuStr_EM_4_ID237_ColorName_E[5],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,305,utNone},
{&MI_1_ID3FB_SpotList[3],6,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[6],MenuStr_EM_4_ID237_ColorName_E[6],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,306,utNone},
{&MI_1_ID3FB_SpotList[3],7,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[7],MenuStr_EM_4_ID237_ColorName_E[7],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,307,utNone},
{&MI_1_ID3FB_SpotList[3],8,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[8],MenuStr_EM_4_ID237_ColorName_E[8],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,308,utNone},
{&MI_1_ID3FB_SpotList[3],9,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[9],MenuStr_EM_4_ID237_ColorName_E[9],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,309,utNone},
{&MI_1_ID3FB_SpotList[3],10,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[10],MenuStr_EM_4_ID237_ColorName_E[10],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,310,utNone},
{&MI_1_ID3FB_SpotList[3],11,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[11],MenuStr_EM_4_ID237_ColorName_E[11],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,311,utNone},
{&MI_1_ID3FB_SpotList[3],12,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[12],MenuStr_EM_4_ID237_ColorName_E[12],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,312,utNone},
{&MI_1_ID3FB_SpotList[3],13,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[13],MenuStr_EM_4_ID237_ColorName_E[13],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,313,utNone},
{&MI_1_ID3FB_SpotList[3],14,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[14],MenuStr_EM_4_ID237_ColorName_E[14],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,314,utNone},
{&MI_1_ID3FB_SpotList[3],15,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[15],MenuStr_EM_4_ID237_ColorName_E[15],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,315,utNone},
{&MI_1_ID3FB_SpotList[3],16,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[16],MenuStr_EM_4_ID237_ColorName_E[16],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,316,utNone},
{&MI_1_ID3FB_SpotList[3],17,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[17],MenuStr_EM_4_ID237_ColorName_E[17],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,317,utNone},
{&MI_1_ID3FB_SpotList[3],18,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[18],MenuStr_EM_4_ID237_ColorName_E[18],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,318,utNone},
{&MI_1_ID3FB_SpotList[3],19,0,0,(unsigned short *)MenuStr_EM_4_ID237_ColorName[19],MenuStr_EM_4_ID237_ColorName_E[19],0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)UI_EM_Get_SpotColorName,(UISetFloatFuncExType)UI_EM_Set_SpotColorName,0,319,utNone}
};

// Spot Color Define
struct MenuItem MI_1_ID3FB_SpotList[4]={
{&MI_1_ID3FB_System[4],0,0,0,(unsigned short *)MenuStr_EM_ID3FB_SpotName[0],"Spot Color 1",MI_EM_3_ID236_Spot1Name,20,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_1_ID3FB_System[4],0,0,0,(unsigned short *)MenuStr_EM_ID3FB_SpotName[1],"Spot Color 2",MI_EM_3_ID236_Spot2Name,20,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MI_1_ID3FB_System[4],0,0,0,(unsigned short *)MenuStr_EM_ID3FB_SpotName[2],"Spot Color 3",MI_EM_3_ID236_Spot3Name,20,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MI_1_ID3FB_System[4],0,0,0,(unsigned short *)MenuStr_EM_ID3FB_SpotName[3],"Spot Color 4",MI_EM_3_ID236_Spot4Name,20,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone}
};

struct MenuItem MI_1_ID3FB_Head_Decryption[]={
#if defined(LCD_MODULE_128_64_ST5668A)
 #ifndef EPSON_4H
{&MI_1_ID3FB_System[5],0,0,0,(unsigned short *)MenuStr_2_ID3E_Head1_Encry,"J12,J14 Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head1_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head1_Encry_OnOff,0,0,utNone},
{&MI_1_ID3FB_System[5],1,0,0,(unsigned short *)MenuStr_2_ID3E_Head2_Encry,"J13,J15 Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head2_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head2_Encry_OnOff,0,1,utNone}
#else
{&MI_1_ID3FB_System[5],0,0,0,(unsigned short *)MenuStr_2_ID3E_Head1_Encry,"J9,J12 Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head1_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head1_Encry_OnOff,0,0,utNone},
{&MI_1_ID3FB_System[5],1,0,0,(unsigned short *)MenuStr_2_ID3E_Head2_Encry,"J10,J13 Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head2_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head2_Encry_OnOff,0,1,utNone},
{&MI_1_ID3FB_System[5],2,0,0,(unsigned short *)MenuStr_2_ID3E_Head3_Encry,"J8,J15 Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head3_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head3_Encry_OnOff,0,2,utNone},
{&MI_1_ID3FB_System[5],3,0,0,(unsigned short *)MenuStr_2_ID3E_Head4_Encry,"J11,J14 Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head4_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head4_Encry_OnOff,0,3,utNone}
#endif
#else
#ifndef EPSON_4H
{&MI_1_ID3FB_System[5],0,0,0,(unsigned short *)MenuStr_2_ID3E_Head1_Encry,"J12,J14 is Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head1_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head1_Encry_OnOff,0,0,utNone},
{&MI_1_ID3FB_System[5],1,0,0,(unsigned short *)MenuStr_2_ID3E_Head2_Encry,"J13,J15 is Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head2_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head2_Encry_OnOff,0,1,utNone}
#else
{&MI_1_ID3FB_System[5],0,0,0,(unsigned short *)MenuStr_2_ID3E_Head1_Encry,"J9,J12 is Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head1_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head1_Encry_OnOff,0,0,utNone},
{&MI_1_ID3FB_System[5],1,0,0,(unsigned short *)MenuStr_2_ID3E_Head2_Encry,"J10,J13 is Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head2_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head2_Encry_OnOff,0,1,utNone},
{&MI_1_ID3FB_System[5],2,0,0,(unsigned short *)MenuStr_2_ID3E_Head3_Encry,"J8,J15 is Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head3_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head3_Encry_OnOff,0,2,utNone},
{&MI_1_ID3FB_System[5],3,0,0,(unsigned short *)MenuStr_2_ID3E_Head4_Encry,"J11,J14 is Encry",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)GetPRTParam_Head4_Encry_OnOff,(UISetFloatFuncExType)SetPRTParam_Head4_Encry_OnOff,0,3,utNone}
#endif
#endif
};

// 系统
struct MenuItem MI_1_ID3FB_System[]={
#if defined(LCD_MODULE_128_64_ST5668A)
{&MI_0_ID00_MenuChilds[14],0,0,0,(unsigned short *)MenuStr_2_ID3FC_AdvancedMode,"Advanced Mode",0,0,chs,pwdEditor,normalStatus,0,tNone,5,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_0_ID00_MenuChilds[14],1,0,0,(unsigned short *)MenuStr_2_ID3FC_Calibration_Reset,"CalibrationRst",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)UI_Reset_Calibration,1,utNone},
{&MI_0_ID00_MenuChilds[14],2,0,0,(unsigned short *)MenuStr_2_ID3FC_CleanConfig_Reset,"Clean Rst",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)UI_Reset_CleanConfig,2,utNone},
#if defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1) ||defined (SUPPORT_MOTOR_CONTROL_ONLY_STEP)
{&MI_0_ID00_MenuChilds[14],3,0,0,(unsigned short *)MenuStr_2_ID3FC_MotionConfig_Reset,"Motion Rst",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,(UIActionFuncExType)UI_Reset_MotionConfig,3,utNone},
#else
{&MI_0_ID00_MenuChilds[14],3,0,0,(unsigned short *)MenuStr_2_ID3FC_MotionConfig_Reset,"Motion Rst",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,3,utNone},
#endif
{&MI_0_ID00_MenuChilds[14],4,0,0,(unsigned short *)MenuStr_2_ID3FC_SpotColorName,"Spot Color Nam",MI_1_ID3FB_SpotList,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
 #ifndef EPSON_4H
{&MI_0_ID00_MenuChilds[14],5,0,0,(unsigned short *)MenuStr_2_ID3FC_Head_Decryption,"H Decryption",MI_1_ID3FB_Head_Decryption,2,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,5,utNone}
 #else
{&MI_0_ID00_MenuChilds[14],5,0,0,(unsigned short *)MenuStr_2_ID3FC_Head_Decryption,"H Decryption",MI_1_ID3FB_Head_Decryption,4,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,5,utNone}
 #endif
 #ifdef FUNC_WAVE_MAP
,{&MI_0_ID00_MenuChilds[14],6,0,0,(unsigned short *)MenuStr_Sys_WaveMap_CN,MenuStr_Sys_WaveMap_EN,MI_Sys_WaveMap,MAX_HEAD_DRIVE_NUM,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone}
#endif
#else
{&MI_0_ID00_MenuChilds[14],0,0,0,(unsigned short *)MenuStr_2_ID3FC_AdvancedMode,"Advanced Mode",0,0,chs,pwdEditor,normalStatus,0,tNone,7,0,0,0,1,{0},0,0,0,0,utNone},
{&MI_0_ID00_MenuChilds[14],1,0,0,(unsigned short *)MenuStr_2_ID3FC_Calibration_Reset,"Calibration Reset",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)UI_Reset_Calibration,1,utNone},
{&MI_0_ID00_MenuChilds[14],2,0,0,(unsigned short *)MenuStr_2_ID3FC_CleanConfig_Reset,"Clean Config Reset",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)UI_Reset_CleanConfig,2,utNone},
#if defined(EPSON_BOTTOM_BOARD_V3)  ||defined (EPSON_BOTTOM_BOARD_V2_1) ||defined (SUPPORT_MOTOR_CONTROL_ONLY_STEP)
{&MI_0_ID00_MenuChilds[14],3,0,0,(unsigned short *)MenuStr_2_ID3FC_MotionConfig_Reset,"Motion Config Reset",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,(UIActionFuncExType)UI_Reset_MotionConfig,3,utNone},
#else
{&MI_0_ID00_MenuChilds[14],3,0,0,(unsigned short *)MenuStr_2_ID3FC_MotionConfig_Reset,"Motion Config Reset",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,3,utNone},
#endif
{&MI_0_ID00_MenuChilds[14],4,0,0,(unsigned short *)MenuStr_2_ID3FC_SpotColorName,"Spot Color Name",MI_1_ID3FB_SpotList,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
 #ifndef EPSON_4H
 {&MI_0_ID00_MenuChilds[14],5,0,0,(unsigned short *)MenuStr_2_ID3FC_Head_Decryption,"Head Type",MI_1_ID3FB_Head_Decryption,2,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,5,utNone}
 #else
{&MI_0_ID00_MenuChilds[14],5,0,0,(unsigned short *)MenuStr_2_ID3FC_Head_Decryption,"Head Type",MI_1_ID3FB_Head_Decryption,4,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,5,utNone}
#endif
#ifdef FUNC_WAVE_MAP
,{&MI_0_ID00_MenuChilds[14],6,0,0,(unsigned short *)MenuStr_Sys_WaveMap_CN,MenuStr_Sys_WaveMap_EN,MI_Sys_WaveMap,MAX_HEAD_DRIVE_NUM,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone}
#endif
#endif
};

// MainScreenChilds
struct MenuItem MI_1_ID3FD_MainScreenChilds[8]={
{&MainScreenRoot,0,0,0,(unsigned short *)MenuStr_2_ID3FE_Status,"Status",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
{&MainScreenRoot,1,0,0,(unsigned short *)MenuStr_2_ID3FF_Percent,"Percent",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MainScreenRoot,2,0,0,(unsigned short *)ErrorCodeCHS,"Error Code",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MainScreenRoot,3,0,0,(unsigned short *)MenuStr_2_ID401_MediaWidth,"Width",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetMediaWidth,0,0,3,utCM},
{&MainScreenRoot,4,0,0,(unsigned short *)MenuStr_2_ID402_Origin,"Origin",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetBasePoint,0,0,4,utCM},
{&MainScreenRoot,5,0,0,(unsigned short *)MenuStr_2_ID403_PassNumber,"Pass Number",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Cali_EM_Get_Step_PassNum,0,0,5,utNone},
{&MainScreenRoot,6,0,0,(unsigned short *)MenuStr_2_ID404_Resolution,"Resolution",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone},
{&MainScreenRoot,7,0,0,(unsigned short *)MenuStr_2_ID405_Errorinfo,"Error Info",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,7,utNone}
};

#if defined(EPSON_BOTTOM_BOARD_V3)||defined(EPSON_BOTTOM_BOARD_V2_1)
//平台加热
struct MenuItem MI_EM_1_ID3F4_Heater[10]={
#if defined(LCD_MODULE_128_64_ST5668A)  // test for qicaihong
{&MI_0_ID00_MenuChilds[8],0,0,0,(unsigned short *)MenuStr_UM_1_ID3F4_Heater_Pre_onoff,"PreH on/off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterPre,(UISetFloatFuncExType)Menu_SetHeaterPre,0,0,utNone},
{&MI_0_ID00_MenuChilds[8],1,0,0,(unsigned short *)MenuStr_UM_1_ID3F4_Heater_Middle_onoff,"Plat H on/off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterMiddle,(UISetFloatFuncExType)Menu_SetHeaterMiddle,0,1,utNone},
{&MI_0_ID00_MenuChilds[8],2,0,0,(unsigned short *)MenuStr_UM_1_ID3F4_Heater_Post_onoff,"PostH on/off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterPost,(UISetFloatFuncExType)Menu_SetHeaterPost,0,2,utNone},
{&MI_0_ID00_MenuChilds[8],3,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_Temperature_Pre,"PreHeater Tem",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterTempPre,(UISetFloatFuncExType)Menu_SetHeaterTempPre,0,3,utNone},
{&MI_0_ID00_MenuChilds[8],4,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_Temperature_Middle,"Platform Tem",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterTempMiddle,(UISetFloatFuncExType)Menu_SetHeaterTempMiddle,0,4,utNone},
{&MI_0_ID00_MenuChilds[8],5,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_Temperature_Post,"PostHeater Tem",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterTempPost,(UISetFloatFuncExType)Menu_SetHeaterTempPost,0,5,utNone},
{&MI_0_ID00_MenuChilds[8],6,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_RealTemp_Pre,"Pre Senseor T",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetRealHeaterTempPre,(UISetFloatFuncExType)Menu_SetRealHeaterTempPre,0,6,utNone},
{&MI_0_ID00_MenuChilds[8],7,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_RealTemp_Middle,"Plat Sensor T",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetRealHeaterTempMiddle,(UISetFloatFuncExType)Menu_SetRealHeaterTempMiddle,0,7,utNone},
{&MI_0_ID00_MenuChilds[8],8,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_RealTemp_Post,"Post Sensor T",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetRealHeaterTempPost,(UISetFloatFuncExType)Menu_SetRealHeaterTempPost,0,8,utNone},
{&MI_0_ID00_MenuChilds[8],9,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_RealTemp_Env,"Environment T",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetRealHeaterTempEnv,(UISetFloatFuncExType)Menu_SetRealHeaterTempEnv,0,9,utNone}
#else
{&MI_0_ID00_MenuChilds[8],0,0,0,(unsigned short *)MenuStr_UM_1_ID3F4_Heater_Pre_onoff,"PreHeater on/off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterPre,(UISetFloatFuncExType)Menu_SetHeaterPre,0,0,utNone},
{&MI_0_ID00_MenuChilds[8],1,0,0,(unsigned short *)MenuStr_UM_1_ID3F4_Heater_Middle_onoff,"Platform Heater on/off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterMiddle,(UISetFloatFuncExType)Menu_SetHeaterMiddle,0,1,utNone},
{&MI_0_ID00_MenuChilds[8],2,0,0,(unsigned short *)MenuStr_UM_1_ID3F4_Heater_Post_onoff,"PostHeater on/off",0,0,chs,checkbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterPost,(UISetFloatFuncExType)Menu_SetHeaterPost,0,2,utNone},
{&MI_0_ID00_MenuChilds[8],3,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_Temperature_Pre,"PreHeater Temperature",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterTempPre,(UISetFloatFuncExType)Menu_SetHeaterTempPre,0,3,utNone},
{&MI_0_ID00_MenuChilds[8],4,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_Temperature_Middle,"Platform Temperature",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterTempMiddle,(UISetFloatFuncExType)Menu_SetHeaterTempMiddle,0,4,utNone},
{&MI_0_ID00_MenuChilds[8],5,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_Temperature_Post,"PostHeater Temperature",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetHeaterTempPost,(UISetFloatFuncExType)Menu_SetHeaterTempPost,0,5,utNone},
{&MI_0_ID00_MenuChilds[8],6,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_RealTemp_Pre,"Pre Senseor Temp",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetRealHeaterTempPre,(UISetFloatFuncExType)Menu_SetRealHeaterTempPre,0,6,utNone},
{&MI_0_ID00_MenuChilds[8],7,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_RealTemp_Middle,"Plate Sensor Temp",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetRealHeaterTempMiddle,(UISetFloatFuncExType)Menu_SetRealHeaterTempMiddle,0,7,utNone},
{&MI_0_ID00_MenuChilds[8],8,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_RealTemp_Post,"Post Sensor Temp",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetRealHeaterTempPost,(UISetFloatFuncExType)Menu_SetRealHeaterTempPost,0,8,utNone},
{&MI_0_ID00_MenuChilds[8],9,0,0,(unsigned short *)MenuStr_EM_1_ID3F4_RealTemp_Env,"Environment Temp",0,0,chs,editor,normalStatus,1,tFloat,5,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetRealHeaterTempEnv,(UISetFloatFuncExType)Menu_SetRealHeaterTempEnv,0,9,utNone}
#endif
};
#endif
#if defined(MICOLOR_AUTOFUCTION)
//平台吸风
struct MenuItem MI_EM_1_ID3F4_PlateFan[3]={
{&MI_0_ID00_MenuChilds[9],0,0,0,(unsigned short *)MenuStr_2_ID1E_SuckStrong,"Strong",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPlateFanMode,(UISetFloatFuncExType)Menu_SetPlateFanMode,0,0,utNone},
{&MI_0_ID00_MenuChilds[9],1,0,0,(unsigned short *)MenuStr_2_ID1F_SuckWeak,"Weak",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPlateFanMode,(UISetFloatFuncExType)Menu_SetPlateFanMode,0,1,utNone},
{&MI_0_ID00_MenuChilds[9],2,0,0,(unsigned short *)MenuStr_2_ID1F_SuckOff,"Off",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPlateFanMode,(UISetFloatFuncExType)Menu_SetPlateFanMode,0,2,utNone},
};
#elif defined(MANUFACTURER_DYSS)
//平台吸风
struct MenuItem MI_EM_1_ID3F4_PlateFan[3]={
{&MI_0_ID00_MenuChilds[9],0,0,0,(unsigned short *)MenuStr_2_ID1E_SuckStrong,"Auto",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPlateFanMode,(UISetFloatFuncExType)Menu_SetPlateFanMode,0,0,utNone},
{&MI_0_ID00_MenuChilds[9],1,0,0,(unsigned short *)MenuStr_2_ID1F_SuckWeak,"Open",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPlateFanMode,(UISetFloatFuncExType)Menu_SetPlateFanMode,0,1,utNone},
{&MI_0_ID00_MenuChilds[9],2,0,0,(unsigned short *)MenuStr_2_ID1F_SuckOff,"Off",0,0,chs,radiocheckbox,normalStatus,0,tNone,0,0,0,0,1,{0},(UIGetFloatFuncExType)Menu_GetPlateFanMode,(UISetFloatFuncExType)Menu_SetPlateFanMode,0,2,utNone},
};
#endif
// Main
struct MenuItem MI_0_ID00_MenuChilds[]={
{&MainMenuRoot,0,0,0,(unsigned short *)MenuStr_UM_2_ID26_CheckNozzle,"Check Nozzle",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)Cali_UM_CheckNozzle,0,utNone},
{&MainMenuRoot,1,0,0,(unsigned short *)MenuStr_UM_2_ID26_CheckNozzle,"Check Nozzle",MI_EM_2_ID4C_CheckNozzle,4,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#if defined(EPSON_CLEAN_UPDOWN)||defined(RICOH_CLEAN_PRESS)||defined (DYSS_UV_CLEAN)
#ifdef MANUFACTURER_DGEN
{&MainMenuRoot,2,0,0,(unsigned short *)MenuStr_ALLWIN_1_ID23C_HEADCLEANING,"Head Cleaning",MI_ALLWIN_1_ID23C_HEADCLEANING,10,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,7,utNone},
#else
{&MainMenuRoot,2,0,0,(unsigned short *)MenuStr_ALLWIN_1_ID23C_HEADCLEANING,"Head Cleaning",MI_ALLWIN_1_ID23C_HEADCLEANING,9,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,7,utNone},
#endif
#elif defined(EPSON_CLEAN_INTEGRATE) ||defined (EPSON_CLEAN_INTEGRATE_1) ||defined(EPSON_CLEAN_INTEGRATE_2) ||defined(EPSON_CLEAN_INTEGRATE_3)
{&MainMenuRoot,2,0,0,(unsigned short *)MenuStr_ALLWIN_1_ID23C_HEADCLEANING,"Head Cleaning",MI_MICOLOR_1_ID28B_HEADCLEANING,10,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,7,utNone},
#endif
#ifdef HEAD_EPSON_GEN5
{&MainMenuRoot,3,0,0,(unsigned short *)MenuStr_UM_1_ID25_ADJUSTMENT,"Adjustment",MI_UM_1_ID25_ADJUSTMENT,7,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone},
{&MainMenuRoot,4,0,0,(unsigned short *)MenuStr_UM_1_ID25_ADJUSTMENT,"Adjustment",MI_EM_1_ID4B_ADJUSTMENT,10,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone},
#else
{&MainMenuRoot,3,0,0,(unsigned short *)MenuStr_UM_1_ID25_ADJUSTMENT,"Adjustment",MI_UM_1_ID25_ADJUSTMENT,7,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,5,utNone},
{&MainMenuRoot,4,0,0,(unsigned short *)MenuStr_UM_1_ID25_ADJUSTMENT,"Adjustment",MI_EM_1_ID4B_ADJUSTMENT,9,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,6,utNone},
#endif
{&MainMenuRoot,5,0,0,(unsigned short *)MenuStr_1_ID01_MediaTYPE,"Media Type",MI_2_ID05_SHEETTYPE,7,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,0,utNone},
#ifndef MICOLOR_AUTOFUCTION
#if defined(LCD_MODULE_128_64_ST5668A)
#if defined RIPSTAR_FLAT_EX
{&MainMenuRoot,6,0,0,(unsigned short *)MenuStr_1_ID0D_OriginAndMediaWidth,"Origin & Width",MI_1_ID0D_OriginAndMediaWidth,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#elif defined(LECAI_EPSON_4H_Y_ROOLENCODER)
{&MainMenuRoot,6,0,0,(unsigned short *)MenuStr_1_ID0D_OriginAndMediaWidth,"Origin & Width",MI_1_ID0D_OriginAndMediaWidth,7,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#else
{&MainMenuRoot,6,0,0,(unsigned short *)MenuStr_1_ID0D_OriginAndMediaWidth,"Origin & Width",MI_1_ID0D_OriginAndMediaWidth,6,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#endif
#else
#if defined RIPSTAR_FLAT_EX
{&MainMenuRoot,6,0,0,(unsigned short *)MenuStr_1_ID0D_OriginAndMediaWidth,"Origin and Width",MI_1_ID0D_OriginAndMediaWidth,8,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#elif defined(LECAI_EPSON_4H_Y_ROOLENCODER)
{&MainMenuRoot,6,0,0,(unsigned short *)MenuStr_1_ID0D_OriginAndMediaWidth,"Origin and Width",MI_1_ID0D_OriginAndMediaWidth,7,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#else
{&MainMenuRoot,6,0,0,(unsigned short *)MenuStr_1_ID0D_OriginAndMediaWidth,"Origin and Width",MI_1_ID0D_OriginAndMediaWidth,6,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
#endif
#endif
{&MainMenuRoot,7,0,0,(unsigned short *)MenuStr_1_ID15_NewNodeTag,"New Node Tag",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,2,utNone},
#else
#if defined(LCD_MODULE_128_64_ST5668A)
{&MainMenuRoot,6,0,0,(unsigned short *)MenuStr_1_ID0D_OriginAndMediaWidth,"Origin & Width",MI_1_ID0D_OriginAndMediaWidth,10,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MainMenuRoot,7,0,0,(unsigned short *)MenuStr_MICOLOR_EM_1_ID16_SetAutofunction,"Auto Function",MI_MICOLOR_EM_1_ID16_SetAutofunction,10,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
#else
{&MainMenuRoot,6,0,0,(unsigned short *)MenuStr_1_ID0D_OriginAndMediaWidth,"Origin and Width",MI_1_ID0D_OriginAndMediaWidth,10,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,1,utNone},
{&MainMenuRoot,7,0,0,(unsigned short *)MenuStr_MICOLOR_EM_1_ID16_SetAutofunction,"Automatic Function",MI_MICOLOR_EM_1_ID16_SetAutofunction,10,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
#endif
#endif
#if defined(EPSON_BOTTOM_BOARD_V3)
{&MainMenuRoot,8,0,0,(unsigned short *)MenuStr_1_ID1D_Platformheating,"Platform Heat",MI_EM_1_ID3F4_Heater,10,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
{&MainMenuRoot,9,0,0,(unsigned short *)MenuStr_1_ID1D_PlatformFan,"Platform Fan",MI_EM_1_ID3F4_PlateFan,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
#elif defined(EPSON_BOTTOM_BOARD_V2_1) &&(defined(TATE_BEMAJET)||defined(BEMAJET_2H))
{&MainMenuRoot,8,0,0,(unsigned short *)MenuStr_1_ID1D_Platformheating,"Platform Heat",MI_EM_1_ID3F4_Heater,10,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
{&MainMenuRoot,9,0,0,(unsigned short *)MenuStr_1_ID1D_PlatformFan,"Platform Fan",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,3,utNone},
#elif defined(MANUFACTURER_DYSS)
{&MainMenuRoot,8,0,0,(unsigned short *)MenuStr_1_ID1D_Platformheating,"Platform Heat",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,3,utNone},
{&MainMenuRoot,9,0,0,(unsigned short *)MenuStr_1_ID1D_PlatformFan,"Platform Fan",MI_EM_1_ID3F4_PlateFan,3,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,3,utNone},
#else
{&MainMenuRoot,8,0,0,(unsigned short *)MenuStr_1_ID1D_Platformheating,"Platform Heat",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,3,utNone},
{&MainMenuRoot,9,0,0,(unsigned short *)MenuStr_1_ID1D_PlatformFan,"Platform Fan",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,0,{0},0,0,0,3,utNone},
#endif
{&MainMenuRoot,10,0,0,(unsigned short *)MenuStr_1_ID20_QUALITY,"Quality",MI_1_ID20_QUALITY,5,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,4,utNone},
{&MainMenuRoot,11,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_Nozzleparameter,"Head Parameter",MI_EM_1_ID3F3_Nozzleparameter,MAX_HEAD_DRIVE_NUM,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,8,utNone},
{&MainMenuRoot,12,0,0,(unsigned short *)MenuStr_EM_1_ID3F3_HeadSwitch,"Disable Head",MI_EM_1_ID3F3_HeadSwitch,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,2,utNone},
{&MainMenuRoot,13,0,0,(unsigned short *)MenuStr_1_ID3F8_Language,"Language",MI_1_ID3F8_Language,2,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,9,utNone},
#ifdef FUNC_WAVE_MAP
{&MainMenuRoot,14,0,0,(unsigned short *)MenuStr_1_ID3FB_System,"System",MI_1_ID3FB_System,7,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,10,utNone}
#else
{&MainMenuRoot,14,0,0,(unsigned short *)MenuStr_1_ID3FB_System,"System",MI_1_ID3FB_System,6,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,0,10,utNone}
#endif
#ifdef DYSS_UV_CLEAN
,
{&MainMenuRoot,15,0,0,(unsigned short *)MenuStr_UM_2_ID26_PowerOff,"Power Off Mode",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)DoPower_Off_Mode_DYSS,0,utNone}
#elif defined(EPSON_FLASH_IDLE)&&!defined(FULGENCY_FUN)
,
{&MainMenuRoot,15,0,0,(unsigned short *)MenuStr_UM_2_ID26_CAPPING,"capping",0,0,chs,normal,normalStatus,0,tNone,0,0,0,0,1,{0},0,0,(UIActionFuncExType)UIAction_FLAH_IDLE,1,utNone}
#endif
};
#include "global.h"
void Change_Unit_For_Language(void)
{
#if 0
  if(LCDMenuConfig.LanguageIndex == 0)
	{
       MI_1_ID0D_OriginAndMediaWidth[1].Unit = utCM;
	   MI_1_ID0D_OriginAndMediaWidth[2].Unit = utCM;
#ifdef MICOLOR_AUTOFUCTION
 		MI_1_ID0D_OriginAndMediaWidth[6].Unit = utCM;
#else
 		MI_1_ID0D_OriginAndMediaWidth[4].Unit = utCM;
#endif
		sprintf(MainInfo_MediaWidth, "%.2f", LCDMenuConfig.MediaWidth*2.54);
		sprintf(MainInfo_PrintOrigin, "%.2f", LCDMenuConfig.OriginX*2.54);
	}
	else  if(LCDMenuConfig.LanguageIndex == 3)
	{
       MI_1_ID0D_OriginAndMediaWidth[1].Unit = utInch;
	   MI_1_ID0D_OriginAndMediaWidth[2].Unit = utInch;
#ifdef MICOLOR_AUTOFUCTION
 		MI_1_ID0D_OriginAndMediaWidth[6].Unit = utInch;
#else
 		MI_1_ID0D_OriginAndMediaWidth[4].Unit = utInch;
#endif
		sprintf(MainInfo_MediaWidth, "%.2f", LCDMenuConfig.MediaWidth);
		sprintf(MainInfo_PrintOrigin, "%.2f", LCDMenuConfig.OriginX);
	}
#endif
	Set_PrintQuality(LCDMenuConfig.PrintQuality);
}
#ifdef RIPSTAR_FLAT_EX
void RIPSTAR_FLAT_INIT(void)
{
	MI_UM_3_ID3E_VSD1[2].Visible = 0;
	MI_UM_3_ID41_VSD2[2].Visible = 0;
	MI_UM_3_ID44_VSD3[2].Visible = 0;
	MI_UM_3_ID47_VSD4[2].Visible = 0;
	MI_EM_5_ID72_leftalignment[2].Visible = 0;
	MI_EM_5_ID85_BiDir[2].Visible = 0;
	MI_EM_5_ID88_rightalignment[2].Visible = 0;
	MI_EM_5_ID9C_leftalignment[2].Visible = 0;
	MI_EM_5_IDAF_BiDir[2].Visible = 0;
	MI_EM_5_IDB2_rightalignment[2].Visible = 0;
	MI_EM_5_IDC6_leftalignment[2].Visible = 0;
	MI_EM_5_IDD9_BiDir[2].Visible = 0;
	MI_EM_5_IDDC_rightalignment[2].Visible = 0;
	MI_EM_5_IDF0_leftalignment[2].Visible = 0;
	MI_EM_5_ID103_BiDir[2].Visible = 0;
	MI_EM_5_ID106_rightalignment[2].Visible = 0;
	MI_EM_5_ID11B_leftalignment[2].Visible = 0;
	MI_EM_5_ID12E_BiDir[2].Visible = 0;
	MI_EM_5_ID131_rightalignment[2].Visible = 0;
	MI_EM_5_ID145_leftalignment[2].Visible = 0;
	MI_EM_5_ID158_BiDir[2].Visible = 0;
	MI_EM_5_ID15B_rightalignment[2].Visible = 0;
	MI_EM_5_ID16F_leftalignment[2].Visible = 0;
	MI_EM_5_ID182_BiDir[2].Visible = 0;
	MI_EM_5_ID185_rightalignment[2].Visible = 0;
	MI_EM_5_ID199_leftalignment[2].Visible = 0;
	MI_EM_5_ID1AC_BiDir[2].Visible = 0;
	MI_EM_5_ID1AF_rightalignment[2].Visible = 0;
	MI_0_ID00_MenuChilds[5].Visible = 0;
	MI_0_ID00_MenuChilds[6].ItemCount = 8;
	MI_0_ID00_MenuChilds[4].ItemCount = 10;
	MI_0_ID00_MenuChilds[3].ItemCount = 8;
	MI_1_ID20_QUALITY[3].Visible = 0;
#ifdef MANUFACTURER_DYSS	
    MI_1_ID0D_OriginAndMediaWidth[0].Visible = 0;
#endif
    MI_1_ID0D_OriginAndMediaWidth[6].Visible = 1;
    MI_1_ID0D_OriginAndMediaWidth[7].Visible = 0;

}
#endif
#if defined(MANUFACTURER_DYSS)||defined(FULGENCY_FUN)
void RICOH_VSD34_INIT(void)
{
	for(INT8U i=2;i<4;i++)
	{
		MI_UM_2_ID3D_BiDir[i].Visible = 0;
		MI_EM_2_ID4C_CheckNozzle[i].Visible = 0;
		MI_EM_2_ID51_DotCheck[i].Visible = 0;
		MI_UM_3_ID3E_Head_Space[i].Visible = 0;
		MI_EM_3_ID3E_Head_Space[i].Visible = 0;
		MI_EM_3_ID70_360DPI[i].Visible = 0;
		MI_EM_3_ID119_720DPI[i].Visible = 0;
		MI_EM_3_ID1CD_360DPI[i].Visible = 0;
		MI_EM_3_ID1FA_720DPI[i].Visible = 0;
	}
}
#endif
#ifdef DYSS_UV_CLEAN
void RICOH_DYSS_CLEAN_DIS_INIT(void)
{

#if 0
	MI_ALLWIN_1_ID23C_HEADCLEANING[0].ItemCount = 7;

	MenuStr_ALLWIN_UM_3_CleanHead1[0] = MenuStr_ALLWIN_UM_3_CleanHead1_E[0] = factoryDataEx.m_nColorOrder[0];
	MenuStr_ALLWIN_UM_3_CleanHead2[0] = MenuStr_ALLWIN_UM_3_CleanHead2_E[0] = factoryDataEx.m_nColorOrder[1];
	MenuStr_ALLWIN_UM_3_CleanHead3[0] = MenuStr_ALLWIN_UM_3_CleanHead3_E[0] = factoryDataEx.m_nColorOrder[2];

	MenuStr_ALLWIN_UM_3_CleanHead1[1] = MenuStr_ALLWIN_UM_3_CleanHead1_E[1] = 0;
	MenuStr_ALLWIN_UM_3_CleanHead2[1] = MenuStr_ALLWIN_UM_3_CleanHead2_E[1] = 0;
	MenuStr_ALLWIN_UM_3_CleanHead3[1] = MenuStr_ALLWIN_UM_3_CleanHead3_E[1] = 0;

	MenuStr_ALLWIN_UM_3_CleanHead4[0] = factoryDataEx.m_nColorOrder[3];
	MenuStr_ALLWIN_UM_3_CleanHead5[0] = COLORCODE_W;
	MenuStr_ALLWIN_UM_3_CleanHead6[0] = COLORCODE_V;

	MenuStr_ALLWIN_UM_3_CleanHead4[1] = MenuStr_ALLWIN_UM_3_CleanHead5[1] = MenuStr_ALLWIN_UM_3_CleanHead6[1] = 0;
#endif
	MI_ALLWIN_1_ID23C_HEADCLEANING[5].Visible = 0;
	MI_ALLWIN_1_ID23C_HEADCLEANING[6].Text_ch = (unsigned short *)MenuStr_MICOLOR_EM_2_ID3F0_AutoFlash;
	MI_ALLWIN_1_ID23C_HEADCLEANING[6].Text_en = "Auto Spitting";
	MainMenuRoot.ItemCount = 16;

	sprintf(MenuStr_ALLWIN_UM_3_CleanHead1_E,"Head %c,%c", factoryDataEx.m_nColorOrder[0], factoryDataEx.m_nColorOrder[1]);
	sprintf(MenuStr_ALLWIN_UM_3_CleanHead2_E,"Head %c,%c", factoryDataEx.m_nColorOrder[2], factoryDataEx.m_nColorOrder[3]);
	sprintf(MenuStr_ALLWIN_UM_3_CleanHead3_E,"Head %c,%c", COLORCODE_W, COLORCODE_V);
	sprintf(MenuStr_ALLWIN_UM_3_CleanHead1,"喷头 %c,%c", factoryDataEx.m_nColorOrder[0], factoryDataEx.m_nColorOrder[1]);
	sprintf(MenuStr_ALLWIN_UM_3_CleanHead2,"喷头 %c,%c", factoryDataEx.m_nColorOrder[2], factoryDataEx.m_nColorOrder[3]);
	sprintf(MenuStr_ALLWIN_UM_3_CleanHead3,"喷头 %c,%c", COLORCODE_W, COLORCODE_V);
}
#endif
INT8U imgTop[8]={0x00, 0x00, 0x18, 0x3C, 0x7E, 0xFF, 0x00, 0x00};

INT8U imgBot[8]={0x00, 0x00, 0xFF, 0x7E, 0x3C, 0x18, 0x00, 0x00};

INT8U imgLeft[8]={0x04, 0x0C, 0x1C, 0x3C, 0x3C, 0x1C, 0x0C, 0x04};

INT8U imgRight[8]={0x20, 0x30, 0x38, 0x3C, 0x3C, 0x38, 0x30, 0x20};
INT8U imgZero[16]={0};
#endif

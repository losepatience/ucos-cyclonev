#ifndef	__MENU_GUI_h__
#define	__MENU_GUI_h__
//	write your header here
#include "LCD_Main.h"

typedef unsigned int UINT;
typedef unsigned char UCHAR;
typedef char CHAR;

#define LCD_MENUITEM_HEIGHT (16)
#define EDITOR_MAX_LEN  (8)
#define INDICATE_AREAS_WIDTH (8)
#define CHECKBOX_WIDTH (8)
#define UNIT_AREAS_WIDTH (16)
#if defined(LCD_MODULE_128_64_ST5668A)
	#define MENU_NAME_LEN (16)
#else
	#define MENU_NAME_LEN (21)
#endif

extern UCHAR Dis_Menu_Num_Page;
extern UCHAR UILanguage;
extern UCHAR DistanceUnit; //0:inch, 1:CM.
extern UCHAR TimeUnit;  //0:second.
extern enum LCD_DisModeEnum CurrentDisMode;//当前显示模式
extern enum StatusTypeEnum CurrentStatus; //当前状态
extern enum UnitType CurrentLengthUnit;  //当前长度单位
extern enum UnitType CurrentTimeUnit;      //当前时间单位
extern enum UnitType CurrentFreUnit;       //当前频率单位

extern UCHAR MainScreen_ColCount;

extern char Dis_Inf[];
extern char *Dis_Inf_Case;

//显示模式
enum LCD_DisModeEnum
{
	MainScreen,
	Menu,
	Edit,
	Reserved
};

//菜单项类型
enum MenuItemType
{
	normal,         //普通菜单
	checkbox,       //多选框
	radiocheckbox,  //单选框
	editor,         //编辑器
	pwdEditor		//密码输入框
};

//菜单项的值类型
enum ItemValueType
{
	tNone,
	tFloat,         //浮点运算
	tInt,       //整形
	tShort,		//短整型
	tByte  //字节
};

////菜单项状态
enum MenuItemStatus
{
	normalStatus,   //普通显示状态
	selected,       //选择状态
	checked,        //多选选中状态
	radiochecked,   //单选选中状态
	editting        //编辑状态
};

////菜单语言
enum Language
{
	chs,   //中文
	en     //英文
};

////菜单语言
enum UnitType
{
	utNone,   //无
	//Length part
	utInch,     //英寸
	utCM,     //厘米
	//Time part
	utSec,    //秒
	utH,		//小时
	//Freq part
	utKHz,    //千赫兹
	utT
};

struct MenuItem
{
	struct MenuItem *Parents; //父级菜单
	int Index;			//index of parents menuitem
	UCHAR LastFirstIndex;  // 最后一次显示的第一项的index
	UCHAR SelectedIndex;//当前选中项在ChildItems中的序号
	unsigned short *Text_ch;   //中文文本
	CHAR *Text_en;   //英文文本
	//UINT Height;                //高度
	//UINT ch_SizeMode;          //中文字体大小
	//UINT en_SizeMode;          //英文字体大小
	struct MenuItem *ChildItems;        //子菜单
	UCHAR ItemCount;            //子菜单数
	enum Language Lang;                 //显示语言
	enum MenuItemType type;   //菜单类型
	enum MenuItemStatus status;         //当前状态
	UCHAR DecimalPlaces;		//type = editor时,小数点后位数,不等于0为浮点数，等于零则为整数
	enum ItemValueType vlaueType;		//type = editor时,菜单项的值类型
	float maxVal;			//type = editor时,最大值
	float minVal;			//type = editor时,最小值
	UCHAR ActiveIndex;		//type = editor时,当前正在编辑的位置
	UCHAR selected;			//是否选中状态		
	UCHAR Visible;			//是否可见
	UCHAR Value[EDITOR_MAX_LEN];			//type = editor时,值
	UIGetFloatFuncExType GetFloatFunc;    //get函数
	UISetFloatFuncExType SetFloatFunc;    //Set函数
	UIActionFuncExType ActionFunc;      //Action函数
	float ActionParameters;       //Action参数
	enum UnitType Unit;       //Value的单位；
};

//rn struct MenuItem mMainMenu = 0; 
//struct MenuItem MainMenu = 0;

/*struct MenuItemList
{
struct MenuItemInfo *Items;
}*mMainMenu;*/

UCHAR GetMLNum(struct MenuItem *Menu_List);
UCHAR GetMLiNum_Page();
void RefreshScreen(UCHAR First_Index,struct MenuItem *Menu_List,enum LCD_DisModeEnum DisMode);
void DrawMainScreen(enum StatusTypeEnum status,struct MenuItem *Menu_List);
void Initial_Menu(struct MenuItem *Menu_List);

float CallUIGetFuncByType(struct MenuItem *mitem);
float GetValueFormUI(struct MenuItem *mitem);
void SetUIMenuItemValue(struct MenuItem *mitem,float val);
UCHAR CallUISetFuncByType(struct MenuItem *mitem,int index,float val);

void Set_PrintQuality(INT8U index);
void Draw_PrintQuality(void);
#endif

#ifndef _LCD_H_
#define _LCD_H_


//#if (defined (EPSON_LCD))
//#define LCD_MODULE_LCM128645
//#endif

#if defined( LCD_MODULE_ZX19264M1A)
	#if defined(LCD_MODULE_128_64_ST5668A)
		#define LCD_PIXEL_WIDTH 128
		#define LCD_PIXEL_HIGH 64
	#else
		#define LCD_PIXEL_WIDTH 192
		#define LCD_PIXEL_HIGH 64
	#endif
	#define LCD_MAX_CMD_SIZE 700
#elif defined( LCD_MODULE_LCM128645)
#define LCD_PIXEL_WIDTH 128
#define LCD_PIXEL_HIGH 64
#endif

#define LCD_SUBMODULE_COUNT  2
#define LCD_PAGENUMBER  (LCD_PIXEL_HIGH/8)
#define LCD_MODULE_WIDTH  (LCD_PIXEL_WIDTH/LCD_SUBMODULE_COUNT)

#ifdef LCD_MODULE_LCM128645
//for LCM128645, X is from top to bottom. Y is from left to right.
#define LCD_CMD_DISPLAY_OFF 0x3E
#define LCD_CMD_DISPLAY_ON 0x3F
#define LCD_CMD_SetYAddr  0x40
#define LCD_CMD_SetPageNo 0xb8
#define LCD_CMD_SetStartLine 0xc0

#endif

//#define LCD_Text(x,y,text) LCD_DrawText(x,y*8,(char*)text)
#define LCD_Text(x,y,text)

void LCD_Init();
void LCD_ClearScreen();
INT8U LCD_DrawPixel(unsigned short X, unsigned short Y, unsigned char isBlack);
INT8U LCD_FillRectangle(unsigned short X, unsigned short Y, unsigned short Width, unsigned short High, unsigned char isBlack);
INT8U LCD_DrawPicture(unsigned short X, unsigned short Y, unsigned short pictureWidth, 
					  unsigned short pictureHigh, unsigned char * pictureBuffer);
INT8U LCD_InverseRegion(unsigned short X, unsigned short Y, unsigned short regionWidth, unsigned short regionHigh);
INT8U LCD_GetRegion(unsigned short X, unsigned short Y, unsigned short regionWidth, unsigned short regionHigh, unsigned char * regionBuffer);
INT8U LCD_DrawText(unsigned short X, unsigned short Y, char * strText);
INT8U LCD_DrawChineseText(unsigned short X, unsigned short Y, unsigned short * strText);
INT8U LCD_DrawMixedText(unsigned short X, unsigned short Y, unsigned short * strText);
#ifdef ALLWIN_EPSON_SAME
extern INT8U pause_cmd_from_dsp;
#endif
#endif

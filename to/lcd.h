#ifndef _LCD_H_
#define _LCD_H_


#define LCD_PIXEL_WIDTH 192
#define LCD_PIXEL_HIGH 64
#define LCD_MAX_CMD_SIZE 700

#define LCD_SUBMODULE_COUNT  2
#define LCD_PAGENUMBER  (LCD_PIXEL_HIGH/8)
#define LCD_MODULE_WIDTH  (LCD_PIXEL_WIDTH/LCD_SUBMODULE_COUNT)

#define LCD_Text(x,y,text)

void LCD_Init(void);
void LCD_ClearScreen(void);
INT8U LCD_FillRectangle(unsigned short X, unsigned short Y,
		unsigned short Width, unsigned short High,
		unsigned char isBlack);
INT8U LCD_DrawPicture(unsigned short X, unsigned short Y,
		unsigned short pictureWidth, unsigned short pictureHigh,
		unsigned char * pictureBuffer);
INT8U LCD_InverseRegion(unsigned short X, unsigned short Y,
		unsigned short regionWidth, unsigned short regionHigh);
INT8U LCD_DrawText(unsigned short X, unsigned short Y, char * strText);
INT8U LCD_DrawMixedText(unsigned short X, unsigned short Y,
		unsigned short * strText);

#endif

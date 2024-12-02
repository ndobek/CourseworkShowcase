/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     20 MHz (Requires Active PLL)
// Author:        Simon Walker
// Details:       LCD Library
// Revision History :
// Created : Unknown
//  Dec 07 2020 - Modified Documentation
//  Dec    2020 - Modified names, modified to use timer for delays
//  Nov 2021, Added PIT Timer for delays, by Carlos
/////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////
// 8-Bit interface init on LCD
// LCD is wired to PTH for data, PK0:2 for control :
// 2     1     0     PTK 
// A     R/W*  E     LCD 
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// LCD Address Scheme (HEX):
// 00 01 ... 12 13
// 40 41 ... 52 53
// 14 15 ... 26 27
// 54 55 ... 66 67
////////////////////////////////////////////////////
#define LCD_WIDTH 20
#define LCD_ROW0 0
#define LCD_ROW0LAST 19
#define LCD_ROW1 64
#define LCD_ROW1LAST 83
#define LCD_ROW2 20
#define LCD_ROW2LAST 39
#define LCD_ROW3 84
#define LCD_ROW3LAST 103


#define null 0

#define SHIFT_CUR 0
#define SHIFT_SCR (1<<3)

/////////////////////////////////////////////////////////////////////////////
// Library Prototypes
/////////////////////////////////////////////////////////////////////////////

void LCD_INIT (void);
void LCD_INSTRUCTION (unsigned char); //LCD_Inst
void LCD_DATA (unsigned char data);
char LCD_BUSY (void); //LCD_Inst
char LCD_GETADDRESS(void);
void LCD_GETADDRESSXY(unsigned char *, unsigned char *);
void LCD_SETADDRESS (unsigned char addr);
void LCD_SETADDRESSXY (unsigned char x, unsigned char y);
void LCD_STRING (char const * straddr);
void LCD_SPRINTF_XY(unsigned char ix, unsigned char iy, char const *format, ...);
void LCD_STRINGXY (unsigned char ix, unsigned char iy, char const * straddr);
void LCD_STRINGXY_RIGHTJUSTIFIED (unsigned char ix, unsigned char iy, char const * straddr);
void LCD_CHECKWRAP(void);

void LCD_DISPLAYSETTINGS (unsigned char curon, unsigned char blinkon);
void LCD_CLEAR (void);
void LCD_CLEARLINE(unsigned char y);
void LCD_HOME (void);
void LCD_SHIFTL (char);
void LCD_SHIFTR (char);
void lcd_CGAddr (unsigned char addr);
void lcd_CGChar (unsigned char cgAddr, unsigned const char* cgData, int size);
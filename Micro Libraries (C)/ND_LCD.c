/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       7 Segment Display Management
// Revision History
//
/////////////////////////////////////////////////////////////////////////////
#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */
#include <string.h>
#include <stdio.h>

#include "ND_LCD.h"
#include "ND_PIT.h"

// other includes, as *required* for this implementation

// small delay required on read. LCD timing seems to vary on different devices
// if issues are discovered, increasing __x below should be tested first
#define LCD_Latch       \
    {                   \
        PORTK_PK0 = 1;  \
        LCD_MicroDelay; \
        PORTK_PK0 = 0;  \
    }
#define LCD_MicroDelay   \
    {                    \
        PIT_Sleep_us(1); \
    }
#define LCD_WriteMode  \
    {                  \
        DDRH = 0xFF;   \
        PORTK_PK1 = 0; \
    }
#define LCD_ReadMode   \
    {                  \
        DDRH = 0x00;   \
        PORTK_PK1 = 1; \
    }
#define LCD_InstructionMode \
    {                       \
        PORTK_PK2 = 0;      \
    }
#define LCD_DataMode   \
    {                  \
        PORTK_PK2 = 1; \
    }

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////

void LCD_INIT()
{
    // Data Direction
    DDRH = 0xFF;
    DDRK |= 0b00000111;

    PIT_Sleep(45);
    // Present Data on PTH
    PTH = 0b00111000; // Function Set
    /*        ||||||_____(don't care)
              |||||______(don't care)
              ||||_______font:  5x8 matrix (LOW)
              |||________lines: 2 (HIGH)
              ||_________data:  8-bit (HIGH)
              |__________function set of commands
    */

    // Write Operation - Instruction Register
    // PK1->R/W->0  PK2->RS->0
    LCD_WriteMode;
    LCD_InstructionMode;
    LCD_Latch;
    PIT_Sleep(5);
    LCD_Latch;
    PIT_Sleep_us(150);
    LCD_Latch;

    // Busy flag is active now**********************
    // lcd_Ins(0b00111000); //Function set
    //     //We can check for busy flag now**********************
    //     //5x8 dots, 2 lines
    LCD_INSTRUCTION(0b00111000);
    LCD_INSTRUCTION(0b00001110); // display controls
                                 /*         ||||_____Blink:   LOW for off
                                            |||______Cursor:  HIGH for on
                                            ||_______Display: HIGH for on
                                            |________Display Control commands
                              */
    LCD_INSTRUCTION(0b00000001); // clear display, home position
    LCD_INSTRUCTION(0b00000110); // mode controls
                                 /*             |||_____Shift:   LOW for no display shift
                                                ||______Inc/Dec: HIGH for increment (to the left)
                                                |_______Entry Mode commands
                                 */

    LCD_DISPLAYSETTINGS(0, 0);
}

// Send Instruction
void LCD_INSTRUCTION(unsigned char command)
{

    while (LCD_BUSY())
        ; // wait for the Busy Flag to be CLEARED

    LCD_InstructionMode;
    LCD_WriteMode;

    PTH = command;

    // Latch Instruction
    LCD_Latch;
}
// Send Data
void LCD_DATA(unsigned char data)
{

    while (LCD_BUSY())
        ; // wait for the Busy Flag to be CLEARED

    LCD_DataMode;
    LCD_WriteMode;

    PTH = data;

    // Latch Instruction
    LCD_Latch;

    // LCD_CHECKWRAP();
}

// Return positive if LCD is busy
char LCD_BUSY()
{
    char result;
    LCD_ReadMode;
    LCD_InstructionMode; // Instruction Register

    // Latch Instruction
    LCD_Latch;

    // Busy Flag is the MSB and AC B0 - B6 of the Status Register
    result = PTH & PTH_PTH7_MASK;
    // LCD_WriteMode;
    return result;
}

// Sets a cursor to specified x and y coords
void LCD_SETADDRESSXY(unsigned char ix, unsigned char iy)
{
    // Account for y Value
    unsigned char addr = 0;
    if (iy == 1)
        addr = LCD_ROW1;
    else if (iy == 2)
        addr = LCD_ROW2;
    else if (iy >= 3)
        addr = LCD_ROW3;

    // Account for X Value
    addr += ix;

    LCD_SETADDRESS(addr);
}
// Sets a cursor to specified position
void LCD_SETADDRESS(unsigned char addr)
{
    LCD_INSTRUCTION((0b01111111 & addr) | 0b10000000);
}
// Gets Current CursorPosition
char LCD_GETADDRESS()
{
    LCD_ReadMode;
    LCD_InstructionMode;

    LCD_Latch;

    return PTH & 0b01111111;
}
// Get Position and extract into x and y values
void LCD_GETADDRESSXY(unsigned char *ix, unsigned char *iy)
{
    char addr = LCD_GETADDRESS();
    if (addr >= LCD_ROW0 && addr <= LCD_ROW0LAST)
    {
        *iy = 0;
        *ix = addr;
    }
    else if (addr >= LCD_ROW1 && addr <= LCD_ROW1LAST)
    {
        *iy = 1;
        *ix = addr - LCD_ROW1;
    }
    else if (addr >= LCD_ROW2 && addr <= LCD_ROW2LAST)
    {
        *iy = 2;
        *ix = addr - LCD_ROW2;
    }
    else if (addr >= LCD_ROW3 && addr <= LCD_ROW3LAST)
    {
        *iy = 3;
        *ix = addr - LCD_ROW3;
    }
    else
    { // Flag out of bounds
        *iy = 0xff;
        *ix = 0xff;
    }
}

// Sends a string to the LCD, including correct wrapping
void LCD_STRING(char const *string)
{
    int i = 0;

    while (string[i] != '\0')
    {
        LCD_DATA(string[i++]);
    }
}
// Puts a string on the LCD at specified coords
void LCD_SPRINTF_XY(unsigned char ix, unsigned char iy, char const *format, ...)
{
    char message[21];
    va_list args;
    va_start(args, format);
    (void)vsprintf(message, format, args);
    va_end(args);

    LCD_SETADDRESSXY(ix, iy);
    LCD_STRING(message);
}

// Puts a string on the LCD at specified coords
void LCD_STRINGXY(unsigned char ix, unsigned char iy, char const *string)
{
    LCD_SETADDRESSXY(ix, iy);
    LCD_STRING(string);
}

void LCD_STRINGXY_RIGHTJUSTIFIED(unsigned char ix, unsigned char iy, char const *string)
{
    ix -= strlen(string) - 1;
    LCD_SETADDRESSXY(ix, iy);
    LCD_STRING(string);
}

// Checks if cursor position is on the first spot in a row and moves it to the expected row
void LCD_CHECKWRAP()
{
    unsigned char addr = LCD_GETADDRESS();
    switch (addr)
    {
    case LCD_ROW2:
        LCD_SETADDRESS(LCD_ROW1);
        break;
    case LCD_ROW3:
        LCD_SETADDRESS(LCD_ROW2);
        break;
    case LCD_ROW1:
        LCD_SETADDRESS(LCD_ROW3);
        break;
    }
}
// Change Cursor Settings
void LCD_DISPLAYSETTINGS(unsigned char curon, unsigned char blinkon)
{
    unsigned char inst = 0b00001100;
    if (curon)
        inst |= 0b00000010;
    if (blinkon)
        inst |= 0b00000001;
    LCD_INSTRUCTION(inst);
}

void LCD_CLEAR(void)
{
    LCD_INSTRUCTION(0b00000001);
}

void LCD_CLEARLINE(unsigned char y)
{
    LCD_STRINGXY(0, y, "                   ");
}
void LCD_HOME(void)
{
    LCD_INSTRUCTION(0b00000010);
}
void LCD_SHIFTL(char wholeScreen)
{
    unsigned char inst = 0b00010000;
    if (wholeScreen)
        inst |= 0b00001000;
    LCD_INSTRUCTION(inst);
}
void LCD_SHIFTR(char wholeScreen)
{
    unsigned char inst = 0b00010100;
    if (wholeScreen)
        inst |= 0b00001000;
    LCD_INSTRUCTION(inst);
}
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

#include "ND_SEG.h"
#include "ND_PIT.h"

// other includes, as *required* for this implementation

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

void SEG_INIT(void)
{
    // Set Data Direction Registers
    DDRA |= DDRA_DDRA0_MASK | DDRA_DDRA1_MASK;
    DDRB = 0xFF;

    // Clear Displays
    SEG_Clear();
}

void SEG_SendDataRaw(unsigned char control, unsigned char data)
{
    // Send Control Bit
    SEG_SetBitType(1);
    PORTB = control;
    SEG_Latch();

    // Send Data Bit
    SEG_SetBitType(0);
    PORTB = data;
    SEG_Latch();
}

void SEG_SetBitType(unsigned char isControl)
{
    // Indicate if bit is control bit
    if (isControl)
        PORTA |= PORTA_PA1_MASK;
    else
        PORTA &= ~(unsigned char)PORTA_PA1_MASK;
}

void SEG_Latch()
{
    // Set flag low and then high
    PORTA &= ~(unsigned char)PORTA_PA0_MASK;
    PIT_Sleep_us(2);
    PORTA |= PORTA_PA0_MASK;
}

void SEG_Normal(unsigned char address, unsigned char data, SEG_DPOption decimalPoint)
{
    // control bit set to hex decode
    unsigned char control = 0b01011000 | (address & 0b00000111);
    // Set Decimal Point
    if (decimalPoint)
        data &= ~SEG_NO_DP;
    else
        data |= SEG_NO_DP;
    // Send to display
    SEG_SendDataRaw(control, data);
}
void SEG_Custom(unsigned char address, unsigned char data)
{
    // Control bit set to no decode
    unsigned char control = 0b01111000 | (address & 0b00000111);
    // Send to display
    SEG_SendDataRaw(control, data);
}

void SEG_Clear()
{
    unsigned char i;
    for (i = 0; i <= 7; i++)
    {
        SEG_ClearDigit(i);
    }
};
void SEG_ClearDigit(unsigned char address)
{
    SEG_Custom(address, 0x80);
}

void SEG_ClearLine(SEG_LineOption line)
{
    char i;
    if (line == SEG_LineTop)
    {
        for (i = 0; i <= 3; i++)
        {
            SEG_ClearDigit(i);
        }
    }
    else
    {
        for (i = 4; i <= 7; i++)
        {
            SEG_ClearDigit(i);
        }
    }
}

void SEG_16H(unsigned int data, SEG_LineOption line)
{
    char i;
    
    SEG_ClearLine(line);
    for (i = 3; i >= 0; --i)
    {
        // Send last 4 digits of data to display then shift remaining data over
        SEG_Normal(i + (line * 4), data & 0x000f, 0);
        data = data >> 4;
    }
}
void SEG_16D(unsigned int value, SEG_LineOption line)
{
    char i;
    int base = 1;

    // Display error if value too large for display
    if (value > 9999)
    {
        SEG_SayErr(line);
        return;
    }

    SEG_ClearLine(line);

    // Extract digits from value
    for (i = 3; i >= 0; --i)
    {
        //line * 4 to move to next line if appropriate
        //Divide and modulo to extract digit
        SEG_Normal(i + (line * 4), (value / base) % 10, 0);
        base *= 10;//Move to next Digit
    }
}

void SEG_8H(unsigned char address, unsigned char data)
{
    // Send last 4 digits of data to display then shift remaining data over
    SEG_Normal((unsigned char)((address + 1) % 8), data & 0x0f, 0);
    data = data >> 4;
    SEG_Normal(address, data & 0x0f, 0);
}

void SEG_SayErr(SEG_LineOption line)
{
    SEG_ClearLine(line);
    SEG_Custom(0 + (line * 4), SEG_LETTER_E);
    SEG_Custom(1 + (line * 4), SEG_LETTER_R);
    SEG_Custom(2 + (line * 4), SEG_LETTER_R);
}

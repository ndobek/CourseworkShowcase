/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Analog to digital converter
// Revision History
//      Created: 03/18/2024
/////////////////////////////////////////////////////////////////////////////

#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */

#include "ND_RTC.h"
#include "ND_CLOCK.h"
#include "ND_I2C.h"

#include <stdio.h>

// other includes, as *required* for this implementation

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////
RTC_DateTime RTC_HaltTime;
 char RTC_Halted = 0;
const char *RTC_DayNames_Abbrv[] =
    {
        "NA", // 0
        "Su", // 1
        "Mo", // 2
        "Tu", // 3
        "We", // 4
        "Th", // 5
        "Fr", // 6
        "Sa", // 7
};

const char *RTC_MonthNames_Abbrv[] =
    {
        "NON", // 0
        "Jan", // 1
        "Feb", // 2
        "Mar", // 3
        "Apr", // 4
        "May", // 5
        "Jun", // 6
        "Jul", // 7
        "Aug", // 8
        "Sep", // 9
        "Oct", // 10
        "Nov", // 11
        "Dec", // 12
};
/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////

void RTC_INIT(char InitI2C)
{
    char read = 0;
    if (InitI2C)
    {
        I2C0_Init();
    }

    (void)I2C0_RegRead(&read, RTC_ADD, RTC_SECONDS);
    if (read & RTC_SECONDS_ST)
    {
        (void)I2C0_RegWrite(RTC_ADD, RTC_SECONDS, read & ~RTC_SECONDS_ST, IIC0_STOP);
    }

    (void)I2C0_RegRead(&read, RTC_ADD, RTC_AL_HOUR);
    if (read & RTC_AL_HOUR_HT)
    {
        RTC_Halted = 1;
        RTC_HaltTime = RTC_ReadCurrent();
        (void)I2C0_RegWrite(0x68,0x0C,read & 0b10111111, 1);
    }
}

RTC_DateTime RTC_ReadCurrent()
{
    RTC_DateTime result;
    char data;

    (void)I2C0_RegRead(&data, 0x68, 0x07);
    result.Year = (((data >> 4) & 0x0F) * 10) + (data & 0x0F) + 2000;

    (void)I2C0_RegRead(&data, 0x68, 0x06);
    result.Month = (((data >> 4) & 0x01) * 10) + (data & 0x0F);

    (void)I2C0_RegRead(&data, 0x68, 0x05);
    result.MonthDay = (((data >> 4) & 0x03) * 10) + (data & 0x0F);

    (void)I2C0_RegRead(&data, 0x68, 0x04);
    result.Day = (data & 0x07);

    (void)I2C0_RegRead(&data, 0x68, 0x03);
    result.Hours = (((data >> 4) & 0x0F) * 10) + (data & 0x0F);

    (void)I2C0_RegRead(&data, 0x68, 0x02);
    result.Minutes = (((data >> 4) & 0x07) * 10) + (data & 0x0F);

    (void)I2C0_RegRead(&data, 0x68, 0x01);
    result.Seconds = (((data >> 4) & 0x07) * 10) + (data & 0x0F);

    (void)I2C0_RegRead(&data, 0x68, 0x00);
    result.HuSeconds = (((data >> 4) & 0x0F) * 10) + (data & 0x0F);

    return result;
}

void RTC_Write(RTC_DateTime time)
{
    char data;

    time.Year = time.Year - 2000;
    data = ((time.Year / 10) << 4) | (time.Year % 10);
    (void)I2C0_RegWrite(0x68, 0x07, data, 1);

    data = ((time.Month / 10) << 4) | (time.Month % 10);
    (void)I2C0_RegWrite(0x68, 0x06, data, 1);

    data = ((time.MonthDay / 10) << 4) | (time.MonthDay % 10);
    (void)I2C0_RegWrite(0x68, 0x05, data, 1);

    data = (char)time.Day;
    (void)I2C0_RegWrite(0x68, 0x04, data, 1);

    data = ((time.Hours / 10) << 4) | (time.Hours % 10);
    (void)I2C0_RegWrite(0x68, 0x03, data & 0b00111111, 1);

    data = ((time.Minutes / 10) << 4) | (time.Minutes % 10);
    (void)I2C0_RegWrite(0x68, 0x02, data, 1);

    data = ((time.Seconds / 10) << 4) | (time.Seconds % 10);
    (void)I2C0_RegWrite(0x68, 0x01, data & 0b01111111, 1);

    data = ((time.HuSeconds / 10) << 4) | (time.HuSeconds % 10);
    (void)I2C0_RegWrite(0x68, 0x00, data, 1);
}
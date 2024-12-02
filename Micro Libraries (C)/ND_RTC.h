/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Analog to digital converter
// Revision History
//      Created: 04/20/2024
/////////////////////////////////////////////////////////////////////////////
typedef struct
{
    unsigned int Year;
    enum RTC_Month Month;
    unsigned int MonthDay;
    enum RTC_Day Day;
    unsigned char Hours;
    unsigned char Minutes;
    unsigned char Seconds;
    unsigned char HuSeconds;
} RTC_DateTime;

typedef enum __RTC_Day
{
    Sun = 1,
    Mon = 2,
    Tue = 3,
    Wed = 4,
    Th = 5,
    Fri = 6,
    Sat = 7,
}RTC_Day;

typedef enum __RTC_Month
{
    Jan = 1,
    Feb = 2,
    Mar = 3,
    Apr = 4,
    May = 5,
    Jun = 6,
    Jul = 7,
    Aug = 8,
    Sep = 9,
    Oct = 10,
    Nov = 11,
    Dec = 12,
}RTC_Month;

extern const char *RTC_DayNames_Abbrv[];
extern const char *RTC_MonthNames_Abbrv[];
extern char RTC_Halted;
extern RTC_DateTime RTC_HaltTime;
void RTC_INIT(char InitI2C);
RTC_DateTime RTC_ReadCurrent(void);
void RTC_Write(RTC_DateTime time);
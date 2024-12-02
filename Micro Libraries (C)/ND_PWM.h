/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Pulse Width Modulation
// Revision History
//      Created: 04/09/2024
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    PWM_CLOCK_A,
    PWM_CLOCK_B
} PWM_Clock;

typedef enum
{
    PWM_Prescale_1,
    PWM_Prescale_2,
    PWM_Prescale_4,
    PWM_Prescale_8,
    PWM_Prescale_16,
    PWM_Prescale_32,
    PWM_Prescale_64,
    PWM_Prescale_128,
} PWM_PrescaleMode;

typedef enum
{
    PWM_DoubleChannel_01 = 1,
    PWM_DoubleChannel_23 = 3,
    PWM_DoubleChannel_45 = 5,
    PWM_DoubleChannel_67 = 7
} PWM_DoubleChannel;

typedef enum
{
    PWM_Channel_BlueRGB,
    PWM_Channel_GreenRGB,
    PWM_Channel_2,
    PWM_Channel_LCD,
    PWM_Channel_RedRGB,
    PWM_Channel_5,
    PWM_Channel_Speaker,
    PWM_Channel_7
} PWM_SingleChannel;

typedef enum
{
    Note_3C = 262,
    Note_3CS = 139,
    Note_3D = 147,
    Note_3DS = 156,
    Note_3E = 165,
    Note_3F = 175,
    Note_3FS = 185,
    Note_3G = 196,
    Note_3GS = 208,
    Note_3A = 220,
    Note_3AS = 233,
    Note_3B = 247,
    Note_4C = 262,
    Note_4CS = 277,
    Note_4D = 294,
    Note_4DS = 311,
    Note_4E = 330,
    Note_4F = 349,
    Note_4FS = 370,
    Note_4G = 392,
    Note_4GS = 415,
    Note_4A = 440,
    Note_4AS = 466,
    Note_4B = 494,
    Note_5C = 523,
    Note_5CS = 554,
    Note_5D = 587,
    Note_5DS = 622,
    Note_5E = 659,
    Note_5F = 698,
    Note_5FS = 740,
    Note_5G = 784,
    Note_5GS = 830,
    Note_5A = 880,
    Note_5AS = 932,
    Note_5B = 988
} PWM_Note;

void PWM_INIT(void);

unsigned long PWM_ClockSpeed(PWM_Clock clock, char scaled);
unsigned int PWM_GetTicks_Hz(PWM_Clock clock, char scaled, int Hz);

void PWM_Prescale(PWM_Clock clock, PWM_PrescaleMode preScaler);
void PWM_OptimizeClocks_Hz(PWM_Clock clock, unsigned long Hz);
void PWM_SetTickFreq_Hz(PWM_Clock clock, unsigned long Hz);
void PWM_SetScaledClock(PWM_Clock clock, unsigned char halfDivisor);
void PWM_PickClock(unsigned char channel, char scaledClock);

void PWM_SetPolarity(unsigned char channel, char dutyHigh);
void PWM_ConfigureChannel_Single_Hz(unsigned char channel, unsigned int Hz, unsigned char duty);
void PWM_ConfigureChannel_Single(unsigned char channel, unsigned char periodTicks, unsigned char dutyTicks);
void PWM_ConfigureChannel_Double(PWM_DoubleChannel channel, unsigned int periodTicks, unsigned int dutyTicks);

void PWM_Disable(unsigned char channel);

void PWM_SetPeriod_Single(unsigned char channel, unsigned char periodTicks);
void PWM_SetPeriod_Double(PWM_DoubleChannel channel, unsigned int periodTicks);
void PWM_SetDutyCycle_Single(unsigned char channel, unsigned char percent);
void PWM_SetDutyCycle_Double(PWM_DoubleChannel channel, unsigned char percent);

void PWM_Enable_RGB(void);
void PWM_RGB(char R, char G, char B);

void PWM_Enable_Speaker(void);
void PWM_Speaker_PlayNote(PWM_Note note);
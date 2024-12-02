/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Pulse Width Modulation
// Revision History
//      Created: 04/09/2024
/////////////////////////////////////////////////////////////////////////////

#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */
#include "math.h"

#include "ND_PWM.h"
#include "ND_CLOCK.h"
// #include "ND_IO.h"
#include "ND_SEG.h"

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
void PWM_INIT()
{
    PWM_Prescale(PWM_CLOCK_A, PWM_Prescale_2);
    PWM_Prescale(PWM_CLOCK_B, PWM_Prescale_2);
    PWMPOL = 0xFF; // Default High Duty Cycle
}

unsigned long PWM_ClockSpeed(PWM_Clock clock, char scaled)
{
    unsigned long result = CLOCK_GetBusSpeed();
    if (clock == PWM_CLOCK_A)
    {
        result /= (long)pow(2, PWMPRCLK & 0b00000111);
    }
    else
    {
        result /= (long)pow(2, ((PWMPRCLK & 0b01110000) >> 4));
    }
    if (scaled)
    {
        if (clock == PWM_CLOCK_A)
        {
            result /= (PWMSCLA * 2);
        }
        else
        {
            result /= (PWMSCLB * 2);
        }
    }
    return result;
}

unsigned int PWM_GetTicks_Hz(PWM_Clock clock, char scaled, int Hz)
{
    return (unsigned int)(PWM_ClockSpeed(clock, scaled) / Hz);
}

void PWM_Prescale(PWM_Clock clock, PWM_PrescaleMode preScaler)
{
    if (clock == PWM_CLOCK_A)
    {
        PWMPRCLK &= 0b11111000;
    }
    else
    {
        PWMPRCLK &= 0b10001111;
    }
    PWMPRCLK |= preScaler << (clock * 4);
}
void PWM_OptimizeClocks_Hz(PWM_Clock clock, unsigned long Hz)
{
    PWM_SetTickFreq_Hz(clock, Hz * 250);
}
void PWM_SetTickFreq_Hz(PWM_Clock clock, unsigned long Hz)
{
    PWM_PrescaleMode preScale = PWM_Prescale_1;
    unsigned char scale = 1;
    unsigned long newSpeed = CLOCK_GetBusSpeed();
    while ((newSpeed / Hz) > 512 && preScale < PWM_Prescale_128)
    {
        newSpeed /= 2;
        preScale += 1;
    }
    if (newSpeed / Hz == 512)
        scale = 0;
    else
    {
        scale = (unsigned char)((newSpeed / Hz) / 2);
    }

    PWM_Prescale(clock, preScale);
    PWM_SetScaledClock(clock, scale);
}
void PWM_SetScaledClock(PWM_Clock clock, unsigned char halfDivisor)
{
    if (clock == PWM_CLOCK_A)
    {
        PWMSCLA = halfDivisor;
    }
    else
    {
        PWMSCLB = halfDivisor;
    }
}

void PWM_PickClock(unsigned char channel, char scaledClock)
{
    if (scaledClock)
    {
        PWMCLK |= (1 << channel);
    }
    else
    {
        PWMCLK &= ~(1 << channel);
    }
}

void PWM_SetPolarity(unsigned char channel, char dutyHigh)
{
    if (dutyHigh)
        PWMPOL |= (1 << channel);
    else
        PWMPOL &= ~(1 << channel);
}

void PWM_ConfigureChannel_Single_Hz(unsigned char channel, unsigned int Hz, unsigned char duty)
{
    PWM_Clock clock = (PWM_Clock)((channel / 2) % 2);
    char scaled = PWMCLK & (1 << channel);
    unsigned int ticks = PWM_GetTicks_Hz(clock, scaled, Hz);
    if (ticks > 255)
        ticks = 255;
    PWM_ConfigureChannel_Single(channel, (unsigned char)ticks, 0);
    PWM_SetDutyCycle_Single(channel, duty);
}

void PWM_ConfigureChannel_Single(unsigned char channel, unsigned char periodTicks, unsigned char dutyTicks)
{
    (&PWMPER0)[channel] = periodTicks;
    (&PWMDTY0)[channel] = dutyTicks;
    // Enable Channel
    PWME |= 1 << channel;
}

void PWM_ConfigureChannel_Double(PWM_DoubleChannel channel, unsigned int periodTicks, unsigned int dutyTicks)
{
    PWMCTL |= (0b00010000 << (channel / 2));
    (&PWMPER01)[(channel / 2)] = periodTicks;
    (&PWMDTY01)[(channel / 2)] = dutyTicks;
    PWME |= 1 << (channel);
}

void PWM_Disable(unsigned char channel)
{
    PWME &= ~(1 << channel);
}


void PWM_SetPeriod_Single(unsigned char channel, unsigned char periodTicks)
{
    unsigned char duty = (unsigned char)(int)(((&PWMDTY0)[channel] / (&PWMPER0)[channel]) * 100);
    (&PWMPER0)[channel] = periodTicks;
    PWM_SetDutyCycle_Single(channel, duty);
}
void PWM_SetPeriod_Double(PWM_DoubleChannel channel, unsigned int periodTicks)
{
    unsigned char duty = (unsigned char)(long)(((&PWMDTY01)[channel] / (&PWMPER01)[channel]) * 100);
    (&PWMPER01)[channel] = periodTicks;
    PWM_SetDutyCycle_Double(channel, duty);
}

void PWM_SetDutyCycle_Single(unsigned char channel, unsigned char percent)
{
    unsigned char dutyTicks = (unsigned char)(int)((double)(&PWMPER0)[channel] * (percent / 100.0));
    if (dutyTicks > (&PWMPER0)[channel])
        dutyTicks = (&PWMPER0)[channel];
    (&PWMDTY0)[channel] = dutyTicks;
}
void PWM_SetDutyCycle_Double(PWM_DoubleChannel channel, unsigned char percent)
{
    unsigned int dutyTicks = (unsigned int)(int)((double)(&PWMPER01)[(channel / 2)] * (percent / 100.0));
    if (dutyTicks > (&PWMPER01)[(channel / 2)])
        dutyTicks = (&PWMPER01)[(channel / 2)];
    (&PWMDTY01)[(channel / 2)] = dutyTicks;
}

void PWM_Enable_RGB()
{
    PWM_ConfigureChannel_Single(0, 255, 0);
    PWM_ConfigureChannel_Single(1, 255, 0);
    PWM_ConfigureChannel_Single(4, 255, 0);
    PWM_PickClock(0, 1);
    PWM_PickClock(1, 1);
    PWM_PickClock(4, 1);
}
void PWM_RGB(char R, char G, char B)
{
    PWMDTY4 = R;
    PWMDTY1 = G;
    PWMDTY0 = B;
}

void PWM_Enable_Speaker()
{
    PWM_INIT();
    PWM_SetScaledClock(PWM_CLOCK_B, 88); // Gets Concert A to around 128 ticks
    PWM_PickClock(6, 1);
    PWM_ConfigureChannel_Single(6, 0, 0);
}

void PWM_Speaker_PlayNote(PWM_Note note)
{
    PWM_ConfigureChannel_Single_Hz(6, note, 50);
}

/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Generic Input and Output
// Revision History
//      Created: 10/02/2023
/////////////////////////////////////////////////////////////////////////////
#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */

#include "ND_PIT.h"
#include "ND_CLOCK.h"

// other includes, as *required* for this implementation

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////

void (*PIT_Interrupt_CH0)(void) = (void *)0;
void (*PIT_Interrupt_CH1)(void) = (void *)0;
void (*PIT_Interrupt_CH2)(void) = (void *)0;

/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////

// Start up a channel
void PIT_InitChannel(PIT_Channel ch, PIT_MicroTimer mt, PIT_Interrupt ie)
{

    if (mt)
    {
        PITMUX |= ch;
    }
    else
    {
        PITMUX &= ~ch;
    }
    if (ie)
    {
        PITINTE |= ch;
    }
    else
    {
        PITINTE &= ~ch;
    }

    PITCE |= ch;
    PIT_Start();
}

void PIT_EnableInterrupt(PIT_Channel ch, void (*function)(void))
{
    PITINTE |= ch;
    if (ch == PIT_CH0)
        PIT_Interrupt_CH0 = function;
    if (ch == PIT_CH1)
        PIT_Interrupt_CH1 = function;
    if (ch == PIT_CH2)
        PIT_Interrupt_CH2 = function;
}
void PIT_DisableInterrupt(PIT_Channel ch)
{
    PITINTE &= ~ch;
    if (ch == PIT_CH0)
        PIT_Interrupt_CH0 = (void *)0;
    if (ch == PIT_CH1)
        PIT_Interrupt_CH1 = (void *)0;
    if (ch == PIT_CH2)
        PIT_Interrupt_CH2 = (void *)0;
}
void PIT_ClearFlag(PIT_Channel ch)
{
    PITTF = (char)ch;
}

// Set Channel Delay according to previously set mt ticks
void PIT_SetDelay(PIT_Channel ch, unsigned long ticks, PIT_MicroTimer mt)
{
    unsigned long chTicks;
    PIT_InitChannel(ch, mt, PIT_IDIS);
    chTicks = ticks / (unsigned long)PIT_GetMTTicks(mt);
    PIT_SetChannelTicks(ch, (unsigned int)chTicks);
}
// Set Channel Delay in us
void PIT_SetDelay_us(PIT_Channel ch, unsigned long us)
{

    // MT 1 dedicated to small numbers
    PIT_SetDelay(ch, PIT_GetTotalRequiredTicks(us), 1);
}
// Set Channel Delay in ms
void PIT_SetDelay_ms(PIT_Channel ch, int ms)
{
    // Multiply by 1000 to turn us to ms.
    // MT0 for 1ms and greater
    PIT_SetDelay(ch, PIT_GetTotalRequiredTicks((unsigned long)ms * (unsigned long)1000), 0);
}
unsigned long PIT_GetTotalRequiredTicks(unsigned long us)
{
    // Divide by 1000000 to convert bus speed (Hz) to ticks per microsecond
    return (unsigned long)((unsigned long)(CLOCK_GetBusSpeed() / (unsigned long)(1000000)) * us);
}

// Set Ticks on a channel
void PIT_SetChannelTicks(PIT_Channel ch, unsigned int chTicks)
{
    if (ch == PIT_CH0)
    {
        PITLD0 = chTicks - 1;
    }
    else if (ch == PIT_CH1)
    {
        PITLD1 = chTicks - 1;
    }
    else if (ch == PIT_CH2)
    {
        PITLD2 = chTicks - 1;
    }
    else if (ch == PIT_CH3)
    {
        PITLD3 = chTicks - 1;
    }
}
unsigned int PIT_GetChannelTicks(PIT_Channel ch)
{
    if (ch == PIT_CH0)
    {
        return PITLD0 + 1;
    }
    else if (ch == PIT_CH1)
    {
        return PITLD1 + 1;
    }
    else if (ch == PIT_CH2)
    {
        return PITLD2 + 1;
    }
    else if (ch == PIT_CH3)
    {
        return PITLD3 + 1;
    }
}

// Reset channel ticks
void PIT_ResetCount(PIT_Channel ch)
{
    PITFLT |= ch;
    // PITTF = ch;
}
// Check for flag and reset. (Non Blocking)
char PIT_TimerElapsed(PIT_Channel ch)
{
    if (PITTF & ch)
    {
        PITTF = (char)ch;
        return 1;
    }
    return 0;
}
// Included to match the header file
void PIT_Set1msDelay(PIT_Channel ch)
{
    PIT_SetDelay_ms(ch, 1);
}

void PIT_Start()
{
    // Default MT Values: One for large values one for small
    PIT_SetMTTicks(0, 250);
    PIT_SetMTTicks(1, 1);
    PITCFLMT |= PITCFLMT_PITE_MASK;
}
void PIT_SetMTTicks(PIT_MicroTimer mt, unsigned int ticks)
{
    if (mt)
    {
        PITMTLD1 = ticks - 1;
    }
    else
    {
        PITMTLD0 = ticks - 1;
    }
}
unsigned int PIT_GetMTTicks(PIT_MicroTimer mt)
{
    if (mt)
    {
        return PITMTLD1 + 1;
    }
    else
    {
        return PITMTLD0 + 1;
    }
}

// PIT Sleep

void PIT_Sleep(int ms)
{
    int i;
    PIT_SetDelay_ms(PIT_CH3, 1);
    PIT_ResetCount(PIT_CH3);
    for (i = 0; i < ms; ++i)
    {
        while (!PIT_TimerElapsed(PIT_CH3))
            ;
    }
}
void PIT_Sleep_us(int us)
{
    PIT_SetDelay_us(PIT_CH3, us);
    PIT_ResetCount(PIT_CH3);
    while (!PIT_TimerElapsed(PIT_CH3))
        ;
}

// Interrupt Handlers
// Channel 3 is reserved for PIT_SLEEP

interrupt VectorNumber_Vpit0 void PIT_CH0_InterruptHandler(void)
{
    PITTF = PITTF_PTF0_MASK; // clear flag;
    if (PIT_Interrupt_CH0 != 0)
        PIT_Interrupt_CH0();
}

interrupt VectorNumber_Vpit1 void PIT_CH1_InterruptHandler(void)
{
    PITTF = PITTF_PTF1_MASK; // clear flag;
    if (PIT_Interrupt_CH1 != 0)
        PIT_Interrupt_CH1();
}

interrupt VectorNumber_Vpit2 void PIT_CH2_InterruptHandler(void)
{
    PITTF = PITTF_PTF2_MASK; // clear flag;
    if (PIT_Interrupt_CH2 != 0)
        PIT_Interrupt_CH2();
}
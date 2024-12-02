/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Enhanced Capture Timer
// Revision History
//      Created: 03/18/2024
/////////////////////////////////////////////////////////////////////////////

#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */

#include "ND_ECT.h"
#include "ND_CLOCK.h"
// #include "ND_IO.h"

// other includes, as *required* for this implementation

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////
void (*ECT_Interrupts[8])(void);
unsigned int ECT_TickValue_Current[8];
unsigned int ECT_TickValue_Previous[8];
unsigned int ECT_Increments[8];
unsigned int currentMax = 0XFFFF;
/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////

void ECT_INIT(void)
{
    int i;
    // Initialize Interrupts to 0
    for (i = 0; i < 9; ++i)
    {
        ECT_Interrupts[i] = (void *)0;
    }

    // ECT Timer - precision prescale mode, enable
    TSCR1 |= TSCR1_PRNT_MASK | TSCR1_TEN_MASK;

    ECT_Prescale_us(1);
}

void ECT_Prescale_us(unsigned long us)
{
    PTPSR = (char)((CLOCK_GetBusSpeed() / (unsigned long)1000000) * (unsigned long)us) - 1;
}

void ECT_Prescale_divisor(unsigned char divisor)
{
    PTPSR = divisor - 1;
}

void ECT_SetResetTicks(unsigned int resetTicks)
{
    TSCR2 |= TSCR2_TCRE_MASK;
    currentMax = resetTicks;
    ECT_ConfigureOutputChannel(7, resetTicks, 0);
}

void ECT_ConfigureOutputChannel(unsigned char channel, unsigned int startTicks, unsigned int incrementTicks)
{
    // Set as output
    TIOS |= 1 << channel;
    // Set Trigger Ticks
    (&TC0)[channel] = startTicks;

    ECT_ConfigureOutputChannelAction(channel, ECT_Action_Toggle);

    ECT_Increments[channel] = incrementTicks;
    if (incrementTicks != 0)
        ECT_EnableInterrupt(channel, ECT_Interrupts[channel]);
}

void ECT_ConfigureInputChannel(unsigned char channel, ECT_EdgeType edgeType)
{
    // Set as input
    TIOS &= ~(1 << channel);

    ECT_ConfigureInputChannelEdgeType(channel, edgeType);

    ECT_EnableInterrupt(channel, ECT_Interrupts[channel]);
}

void ECT_ConfigureOutputChannelAction(unsigned char channel, ECT_Action action)
{
    /*****************************************
Compare result Action - TCTL1/TCTL2
OMx   OLx
0     0  Timer Disconnected from pin
0     1  Toggle OCx output line
1     0  Clear OCx output lize to zero
1     1  Set OCx output line to one
*****************************************/
    // Set Two bits that correspond to channel to match action

    if (channel >= 4)
    {
        TCTL1 &= ~(3 << ((channel - 4) * 2));
        TCTL1 |= (action << ((channel - 4) * 2));
    }
    else
    {
        TCTL2 &= ~(3 << (channel * 2));
        TCTL2 |= (action << (channel * 2));
    }
}

void ECT_ConfigureInputChannelEdgeType(unsigned char channel, ECT_EdgeType edgeType)
{
    /*****************************************
Compare result Action - TCTL3/TCTL4
EDGxB EDGxA
0     0  Capture Disabled
0     1  Rising Edge Only
1     0  Falling Edge Only
1     1  Any Edge
*****************************************/
    // Set Two bits that correspond to channel to match edgeType

    if (channel >= 4)
    {
        TCTL3 &= ~(3 << ((channel - 4) * 2));
        TCTL3 |= (edgeType << ((channel - 4) * 2));
    }
    else
    {
        TCTL4 &= ~(3 << (channel * 2));
        TCTL4 |= (edgeType << (channel * 2));
    }
}

unsigned int ECT_DeltaTicks(unsigned char channel)
{
    return ECT_TickValue_Current[channel] - ECT_TickValue_Previous[channel];
}

char ECT_ChannelHigh(unsigned char channel){
    return PTT & (1<<channel);
}

void ECT_LinkChannel(unsigned char channel, char driveValue)
{
    char mask = 1 << channel;
    OC7M |= mask;

    if (driveValue)
    {
        OC7D |= mask;
    }
    else
    {
        OC7D &= ~mask;
    }
}

void ECT_UnlinkChannel(unsigned char channel)
{
    OC7M &= ~(1 << channel);
}

void ECT_EnableInterrupt(unsigned char channel, void (*function)(void))
{
    ECT_Interrupts[channel] = function;

    // Enable Interrupt
    TIE |= 1 << channel;
}

void ECT_DisableInterrupt(unsigned char channel)
{
    TIE &= ~(1 << channel);
}

void ECT_DoInterrupt(unsigned char channel)
{
    // Update Recorded Values
    ECT_TickValue_Previous[channel] = ECT_TickValue_Current[channel];
    ECT_TickValue_Current[channel] = (&TC0)[channel];

    // and increment timer
    if (channel < 7)
    {
        (&TC0)[channel] = (ECT_TickValue_Current[channel] + ECT_Increments[channel]) % currentMax;
    }
    else
    {
        // Don't want to modulus channel 7
        TC7 += ECT_Increments[7];
    }

    // Do linked interrupt function
    if (ECT_Interrupts[channel] != 0)
    {
        ECT_Interrupts[channel]();
    }

    // Clear flag
    TFLG1 = 1 << channel;
}

void ECT_EnablePulseAccumulator(ECT_PulseAccumulator PA)
{

    // Pulse Acummulator B is connected to TC0
    // Pulse Acummulator A is connected to TC7

    if (PA == ECT_PulseAccumulatorA_Channel7)
    {
        PACTL = PACTL_PAEN_MASK; // Pulse accumulator A enable
    }
    else
    {
        PBCTL = PBCTL_PBEN_MASK; // Pulse accumulator B enable
    }
}

unsigned int ECT_ReadPulseAccumulator(ECT_PulseAccumulator PA)
{
    unsigned int result;
    if (PA == ECT_PulseAccumulatorA_Channel7)
    {
        result = PACN32;
        PACN32 = 0;
        return result;
    }
    else
    {
        result = PACN10;
        PACN10 = 0;
        return result;
    }
}

interrupt VectorNumber_Vtimch0 void ECT_CH0_ISR(void)
{
    ECT_DoInterrupt(0);
}
interrupt VectorNumber_Vtimch1 void ECT_CH1_ISR(void)
{
    ECT_DoInterrupt(1);
}
interrupt VectorNumber_Vtimch2 void ECT_CH2_ISR(void)
{
    ECT_DoInterrupt(2);
}
interrupt VectorNumber_Vtimch3 void ECT_CH3_ISR(void)
{
    ECT_DoInterrupt(3);
}
interrupt VectorNumber_Vtimch4 void ECT_CH4_ISR(void)
{
    ECT_DoInterrupt(4);
}
interrupt VectorNumber_Vtimch5 void ECT_CH5_ISR(void)
{
    ECT_DoInterrupt(5);
}
interrupt VectorNumber_Vtimch6 void ECT_CH6_ISR(void)
{
    ECT_DoInterrupt(6);
}
interrupt VectorNumber_Vtimch7 void ECT_CH7_ISR(void)
{
    ECT_DoInterrupt(7);
}
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

#include "ND_ADC.h"
#include "ND_CLOCK.h"
#include "ND_IO.h"

// other includes, as *required* for this implementation

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////

unsigned int ADC_Results[8];
char scale = 5;
char ADC_qtyActiveChannels = 0;
volatile char ADC_CompleteFlag;
void (*ADC_OnComplete)(void) = (void *)0;

/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////

void ADC_INIT(char firstChannel, char qtyOfChannels, char lastChannelBeforeWrap)
{
    // Power up - Fast flag clear - ATD Power Down in wait mode - EnableInterrupts
    ATD0CTL2 = ATD0CTL2_ADPU_MASK | ATD0CTL2_AFFC_MASK | ATD0CTL2_AWAI | ATD0CTL2_ASCIE_MASK;

    ADC_SetChannels(firstChannel, qtyOfChannels, lastChannelBeforeWrap);

    // finish current conversion then freeze
    ATD0CTL3 |= ATD0CTL3_FRZ1_MASK;

    // 10-bit resolution, 4 A/D coversion clock periods
    ATD0CTL4 = ATD0CTL4_SMP0;

    // Right Justified
    ATD0CTL5 |= ATD0CTL5_DJM_MASK;

    ADC_Prescale_MHz(2);
}

void ADC_SetScale(char newScale){
    scale = newScale;
}

void ADC_EnableInterrupts(void (*function)(void))
{
    ATD0CTL2 |= ATD0CTL2_ASCIE_MASK;
    ADC_OnComplete = function;
}
void ADC_DisableInterrupts()
{
    ATD0CTL2 &= ~(char)ATD0CTL2_ASCIE_MASK;
}

void ADC_ContinuousMode(char on)
{
    if (on)
    {
        ATD0CTL5 |= ATD0CTL5_SCAN_MASK;
    }
    else
    {
        ATD0CTL5 &= ~(char)ATD0CTL5_SCAN_MASK;
    }
}

void ADC_SetChannels(char firstChannel, char qtyOfChannels, char lastChannelBeforeWrap)
{
    // Set First Channel
    ATD0CTL5 = (ATD0CTL5 & 0b11111000) | (firstChannel & 0b00000111);

    // SetWrapChannel
    if (lastChannelBeforeWrap == 0)
        ++lastChannelBeforeWrap;
    ATD0CTL0 = (ATD0CTL0 & 0b11111000) | (lastChannelBeforeWrap & 0b00000111);

    // Number of conversion per sequence,
    ATD0CTL3 = (ATD0CTL3 & 0b00000111) | (qtyOfChannels << 3);

    ADC_qtyActiveChannels = qtyOfChannels;

    // Set Multiple Channel Flag
    if (qtyOfChannels > 1)
    {
        ATD0CTL5 |= ATD0CTL5_MULT_MASK;
    }
    else
    {
        ATD0CTL5 &= ~(char)ATD0CTL5_MULT_MASK;
    }
}

void ADC_Prescale_MHz(char MHz)
{
    ATD0CTL4 |= ((CLOCK_GetBusSpeed() / 1000000) / (2 * MHz)) - 1;
}

void ADC_ManualConvert()
{
    ADC_CompleteFlag = 0;
    // Trigger right-justified conversion
    ATD0CTL5 |= ATD0CTL5_DJM_MASK;
}

char ADC_ResultsComplete()
{
    if (ADC_CompleteFlag || (ATD0STAT0 & ATD0STAT0_SCF_MASK))
    {
        ADC_CompleteFlag = 0;
        return 1;
    }
    return 0;
}

void ADC_Read()
{
    int i;
    for (i = 0; i < ADC_qtyActiveChannels; ++i)
    {
        ADC_Results[i] = (&ATD0DR0)[i] * scale;
    }
}

interrupt VectorNumber_Vatd0 void ADC_ISR(void)
{
    ADC_Read();
    ADC_CompleteFlag = 1;
    if (ADC_OnComplete != 0)
        ADC_OnComplete();
}
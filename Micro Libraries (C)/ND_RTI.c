/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Delays and Intterupts
// Revision History
//      Created: 10/30/2023
/////////////////////////////////////////////////////////////////////////////
#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */

#include "ND_RTI.h"
#include "ND_IO.h"

// other includes, as *required* for this implementation

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////

volatile unsigned int RTI_BlockCounter = 0;

volatile unsigned int RTI_ActionCounter = 0;
void (*RTI_Action)(void) = (void*)0;
volatile unsigned int RTI_ActionInterval = 1;

/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////

void RTI_INIT(void)
{
  RTICTL = 0b10010111;        // Set RTI to 1ms
  CRGINT |= CRGINT_RTIE_MASK; // Enable RTI
}

void RTI_Delay(unsigned int ms)
{
  RTI_BlockCounter = ms;
  while (RTI_BlockCounter)
    ;
}

void RTI_INIT_Callback(void (*function)(void))
{
  RTI_INIT_Callback_ms(function, 1);
}

void RTI_INIT_Callback_ms(void (*function)(void), unsigned int interval)
{
  RTI_INIT();
  RTI_Action = function;
  RTI_ActionInterval = interval;
}

interrupt VectorNumber_Vrti void RTI_UPDATE_TIMER(void)
{
  CRGFLG = CRGFLG_RTIF_MASK; // Set Flag

  if (RTI_BlockCounter)
  {
    --RTI_BlockCounter;
  }

  if (RTI_ActionCounter)
  {
    --RTI_ActionCounter;
  }

  if (!RTI_ActionCounter && RTI_Action != 0)
  {
    RTI_ActionCounter = RTI_ActionInterval;
    RTI_Action();
  }
}
/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Manage Clock Speed
// Revision History
//      Created: 10/02/2023
/////////////////////////////////////////////////////////////////////////////
#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */

#include "ND_CLOCK.h"

// other includes, as *required* for this implementation

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////

unsigned long currentClockSpeed = DEF_BUS_CLOCK;
int currentDivider = 1;
unsigned int loopsPerMillisecond = 246;

/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////
void CLOCK_INIT(int busSpeedMHz){
  if(busSpeedMHz == 8) CLOCK_Set8MHZ();
  else CLOCK_SETPLL(busSpeedMHz);
}

void CLOCK_SETPLL(int busSpeedMHz)
{
  if (busSpeedMHz > 60)
  {
    SYNR = (busSpeedMHz / 2) - 1;
    REFDV = 7;
  }
  else
  {
    SYNR = busSpeedMHz - 1;
    REFDV = 15;
  }

  currentClockSpeed = busSpeedMHz * 1000000;

  PLLCTL |= PLLCTL_PLLON_MASK | PLLCTL_AUTO_MASK; // Init PLL
  while (!CRGFLG_LOCK)
    ; // Wait for PLL to spin up

  CLKSEL |= CLKSEL_PLLSEL_MASK; // Select PLL
}
void CLOCK_PLL_OFF(void)
{
  currentClockSpeed = DEF_BUS_CLOCK;
  CLKSEL &= (int)~CLKSEL_PLLSEL_MASK; // Select Oscillator
}

void CLOCK_DIVIDE_BY(int divider)
{
  if (divider >= 1 && divider <= 4)
  {
    ECLKCTL = divider - 1;
    currentDivider = divider;
  }
  else
  {
    ECLKCTL = 0;
    currentDivider = 1;
  }
}
void CLOCK_SET_DIVIDER(ClockOutDiv divider)
{
  CLOCK_DIVIDE_BY((int)divider + 1);
}

void CLOCK_Set8MHZ(void)
{
  CLOCK_PLL_OFF();
}
void CLOCK_Set20MHZ(void)
{
  CLOCK_SETPLL(20);
}
void CLOCK_Set24MHZ(void)
{
  CLOCK_SETPLL(24);
}
void CLOCK_Set40MHZ(void)
{
  CLOCK_SETPLL(40);
}
void CLOCK_Set80MHZ(void)
{
  CLOCK_SETPLL(80);
}
unsigned long CLOCK_GetBusSpeed(void){
  return currentClockSpeed/currentDivider;
}

unsigned char CLOCK_GetBusSpeed_MHz(void){
  return (unsigned char)((currentClockSpeed/currentDivider)/1000000);
}
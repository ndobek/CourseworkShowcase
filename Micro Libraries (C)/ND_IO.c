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

#include "ND_IO.h"
#include "ND_PIT.h"

// other includes, as *required* for this implementation

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////

SWITCH_STATE SWITCH_TOP = Idle;
SWITCH_STATE SWITCH_LEFT = Idle;
SWITCH_STATE SWITCH_BOTTOM = Idle;
SWITCH_STATE SWITCH_RIGHT = Idle;
SWITCH_STATE SWITCH_CTR = Idle;
SWITCH_STATE SWITCH_ALL = Idle;

void (*J0_Interrupt_Press)(void) = (void *)0;
void (*J0_Interrupt_Release)(void) = (void *)0;
void (*J1_Interrupt_Press)(void) = (void *)0;
void (*J1_Interrupt_Release)(void) = (void *)0;

/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////

void IO_INIT(void)
{
  IO = 0;                       // Starting state is off
  DDR1AD1 = (char)LED_MASK_ALL; // All LEDs will be outputs
  ATD1DIEN1 = (char)SWITCH_MASK_ALL;
}
// Debounced
void IO_UPDATE(void)
{
  unsigned char firstSample;
  unsigned char secondSample = (unsigned char)SWITCH_GET_RAW(SWITCH_MASK_ALL);

  do
  {
    firstSample = secondSample;
    PIT_Sleep_us(30);
    secondSample = (unsigned char)SWITCH_GET_RAW(SWITCH_MASK_ALL);
  } while (firstSample != secondSample);

  SWITCH_UPDATE(&SWITCH_TOP, firstSample & SWITCH_MASK_TOP);
  SWITCH_UPDATE(&SWITCH_LEFT, firstSample & SWITCH_MASK_LEFT);
  SWITCH_UPDATE(&SWITCH_BOTTOM, firstSample & SWITCH_MASK_BOTTOM);
  SWITCH_UPDATE(&SWITCH_RIGHT, firstSample & SWITCH_MASK_RIGHT);
  SWITCH_UPDATE(&SWITCH_CTR, firstSample & SWITCH_MASK_CTR);
  SWITCH_UPDATE(&SWITCH_ALL, firstSample & SWITCH_MASK_ALL);
}

// LED HANDLING
int LED_GET(LED_MASK LEDMask)
{
  int result = IO & LEDMask;
  return result;
}
void LED_SET(LED_MASK LEDMask, int onOff)
{
  if (onOff)
  {
    LED_ON(LEDMask);
  }
  else
  {
    LED_OFF(LEDMask);
  }
}
void LED_SET_ALL(LED_MASK LEDMask)
{
  LED_ON(LEDMask);
  LED_OFF(~LEDMask);
}
void LED_TOGGLE(LED_MASK LEDMask)
{
  IO ^= (LEDMask & LED_MASK_ALL);
}
void LED_ON(LED_MASK LEDMask)
{
  IO |= (LEDMask & LED_MASK_ALL);
}
void LED_OFF(LED_MASK LEDMask)
{
  IO &= ~(LEDMask & LED_MASK_ALL);
}

// SWITCH HANDLING
void SWITCH_UPDATE(SWITCH_STATE *pastState, int isPressed)
{
  if (isPressed)
  {
    if (*pastState == Idle || *pastState == Released)
      *pastState = Pressed;
    else if (*pastState == Pressed)
      *pastState = Held;
  }
  else
  {
    if (*pastState == Held || *pastState == Pressed)
      *pastState = Released;
    else if (*pastState == Released)
      *pastState = Idle;
  }
}
// Not Debounced
void SWITCH_UPDATE_ALL(void)
{
  SWITCH_UPDATE(&SWITCH_TOP, SWITCH_GET_RAW(SWITCH_MASK_TOP));
  SWITCH_UPDATE(&SWITCH_LEFT, SWITCH_GET_RAW(SWITCH_MASK_LEFT));
  SWITCH_UPDATE(&SWITCH_BOTTOM, SWITCH_GET_RAW(SWITCH_MASK_BOTTOM));
  SWITCH_UPDATE(&SWITCH_RIGHT, SWITCH_GET_RAW(SWITCH_MASK_RIGHT));
  SWITCH_UPDATE(&SWITCH_CTR, SWITCH_GET_RAW(SWITCH_MASK_CTR));
  SWITCH_UPDATE(&SWITCH_ALL, SWITCH_GET_RAW(SWITCH_MASK_ALL));
}
int SWITCH_GET_RAW(SWITCH_MASK SWITCHMask)
{
  int result = IO & SWITCHMask;
  return result;
}
int SWITCH_GET_RAW_ANY(void)
{
  return SWITCH_GET_RAW(SWITCH_MASK_ALL);
}
int SWITCH_QTY_ON(void)
{
  int count = 0;
  unsigned int mask = SWITCH_GET_RAW(SWITCH_MASK_ALL);
  while (mask != 0)
  {
    count += mask & 1;
    mask >>= 1;
  }
  return count;
}

void J_INIT(J_EDGE edges, void (*function)(void))
{
  // Set PJ0 and PJ1 as inputs
  DDRJ &= ~(char)(DDRJ_DDRJ1_MASK | DDRJ_DDRJ0_MASK);

  // Both Ports Initially set to rising edge. (Press)
  PPSJ |= PPSJ_PPSJ1_MASK | PPSJ_PPSJ0_MASK;

  if ((char)edges & (char)J0_PRESS)
  {
    J0_Interrupt_Press = function;
    PIEJ |= PIEJ_PIEJ0_MASK; // Enable J Interrupts for Port J0
  }
  if ((char)edges & (char)J0_RELEASE)
  {
    J0_Interrupt_Release = function;
    PIEJ |= PIEJ_PIEJ0_MASK; // Enable J Interrupts for Port J0
  }
  if ((char)edges & (char)J1_PRESS)
  {
    J1_Interrupt_Press = function;
    PIEJ |= PIEJ_PIEJ1_MASK; // Enable J Interrupts for Port J1
  }
  if ((char)edges & (char)J1_RELEASE)
  {
    J1_Interrupt_Release = function;
    PIEJ |= PIEJ_PIEJ1_MASK; // Enable J Interrupts for Port J1
  }
}

char J_IsHigh(char port)
{
  if (port)
  {
    return PIFJ & PIFJ_PIFJ1_MASK;
  }
  return PIFJ & PIFJ_PIFJ0_MASK;
}

interrupt VectorNumber_Vportj void J_InterruptHandler(void)
{
  // Port J0
  if (PIFJ & PIFJ_PIFJ0_MASK)
  {
    PIFJ = PIFJ_PIFJ0_MASK; // clear flag;

    // Press
    if (PPSJ & PPSJ_PPSJ0_MASK)
    {
      if (J0_Interrupt_Press != 0)
      {
        J0_Interrupt_Press();
      }
      // Switch to falling edge if initialized
      if (J0_Interrupt_Release != 0)
      {
        PPSJ &= ~(char)PPSJ_PPSJ0_MASK;
      }
    }

    // Release
    else if (!(PPSJ & PPSJ_PPSJ0_MASK))
    {
      if (J0_Interrupt_Release != 0)
      {
        J0_Interrupt_Release();
      }
      // Switch to rising edge if initialized
      if (J0_Interrupt_Press != 0)
      {
        PPSJ |= PPSJ_PPSJ0_MASK;
      }
    }
  }

  // Port J1
  if (PIFJ & PIFJ_PIFJ1_MASK)
  {
    PIFJ = PIFJ_PIFJ1_MASK; // clear flag;

    // Press
    if (PPSJ & PIFJ_PIFJ1_MASK)
    {
      if (J1_Interrupt_Press != 0)
      {
        J1_Interrupt_Press();
      }
      // Switch to falling edge if initialized
      if (J1_Interrupt_Release != 0)
      {
        PPSJ &= ~(char)PIFJ_PIFJ1_MASK;
      }
    }

    // Release
    else if (!(PPSJ & PIFJ_PIFJ1_MASK))
    {
      if (J1_Interrupt_Release != 0)
      {
        J1_Interrupt_Release();
      }
      // Switch to rising edge if initialized
      if (J1_Interrupt_Press != 0)
      {
        PPSJ |= PIFJ_PIFJ1_MASK;
      }
    }
  }
}
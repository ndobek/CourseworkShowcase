/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Generic Input and Output
// Revision History
//      Created: 10/02/2023
/////////////////////////////////////////////////////////////////////////////
#include <hidef.h>      /* common defines and macros */
#include <string.h>     //for memcpy, memset
#include "derivative.h" /* derivative-specific definitions */

#include "ND_SCI.h"
#include "ND_CLOCK.h"
#include "math.h"
#include <stdio.h>

// other includes, as *required* for this implementation

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////

unsigned int timeOutCounter = 0;
unsigned int timeOutTotalTicks = 0;

char SCI0_Timeout_DataIncoming = 0;
char SCI0_Timeout_DataComplete = 0;
char SCI0_Timeout_BufferOverflow = 0;

int SCI0_Timeout_DataSize = 0;
int SCI0_Timeout_BufferSize = 0;

char *SCI0_Timeout_Data;
/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////

unsigned long SCI0_INIT(unsigned long ulBaudRate)
{
  float baud;
  SCI0CR1 = 0;                                  // SCI Messaging Characteristics
  SCI0CR2 |= SCI0CR2_TE_MASK | SCI0CR2_RE_MASK; // Enable Send and Recieve

  // Get ideal register value to set baud rate to
  baud = CLOCK_GetBusSpeed() / (16.0 * ulBaudRate);
  // Round value
  if ((baud - (int)baud) >= 0.5)
  {
    baud = ceilf(baud);
  }
  else
  {
    baud = floorf(baud);
  }

  SCI0BD = (unsigned int)(int)baud; // Set Baud Rate

  return (unsigned int)(int)(CLOCK_GetBusSpeed() / (16.0 * baud));
}

void SCI0_Enable_Timeout(char *buffer, int bufferSize, unsigned int catchesBeforeTimeout)
{
  SCI0CR2 |= SCI0CR2_RIE_MASK;

  SCI0_Timeout_Data = buffer;
  SCI0_Timeout_BufferSize = bufferSize;
  timeOutTotalTicks = catchesBeforeTimeout;
}

// Wait for input and read it
unsigned char SCI0_Read_Blocking(void)
{
  while (!(SCI0SR1 & SCI0SR1_RDRF_MASK)) // Wait for Register to be filled
    ;
  return SCI0DRL;
}

// Non blocking single byte read
unsigned char SCI0_Read(unsigned char *pData)
{
  if (SCI0SR1 & SCI0SR1_RDRF_MASK) // If Register is filled
  {
    *pData = SCI0DRL;
    return 1;
  }
  return 0;
}

// Send single byte to SCI
void SCI0_Write(unsigned char data)
{
  if (SCI0SR1 & SCI0SR1_TDRE_MASK) // If Register is empty
  {
    SCI0DRL = data;
  }
}

// Send string to SCI
void SCI0_Write_String(char const *string)
{
  unsigned int i = 0;
  while (string[i] != '\0')
  {
    while (!(SCI0SR1 & SCI0SR1_TDRE_MASK)) // Wait for Register to be empty
      ;
    SCI0DRL = string[i++];
  }
}

// Get string from SCI (Non blocking)
char SCI0_Read_String(char *string, char lineEnding, char echo)
{
  unsigned char data;
  int i;
  if (SCI0_Read(&data))
  {
    if (echo)
      SCI0_Write(data);
    if (data == '\0' || data == lineEnding)
    {
      return 1;
    }
    i = 0;
    while (string[i] != '\0')
    {
      ++i;
    }
    string[i] = data;
  }
  return 0;
}

char SCI0_Read_Timeout()
{
  SCI0_Timeout_DataComplete = 0;
  if (SCI0_Timeout_DataIncoming)
  {
    if (timeOutCounter++ >= timeOutTotalTicks)
    {
      SCI0_Timeout_DataComplete = 1;

      timeOutCounter = 0;
      SCI0_Timeout_DataIncoming = 0;
    }
  }
  return SCI0_Timeout_DataComplete;
}

void SCI0_Timeout_ClearBuffer()
{
  (void)memset(SCI0_Timeout_Data, 0, SCI0_Timeout_DataSize);
  SCI0_Timeout_DataSize = 0;
  SCI0_Timeout_BufferOverflow = 0;
}

int SCI_INIT(SCI_Base sci, unsigned long ulBaudRate, int iRDRF_Interrupt);
void sci_txByte(SCI_Base sci, unsigned char data);
void sci_txStr(SCI_Base sci, char const *straddr);
unsigned char sci_rxByte(SCI_Base sci, unsigned char *pData);

interrupt VectorNumber_Vsci0 void SCI_ISR(void)
{
  unsigned char data;

  if (SCI0_Timeout_DataIncoming == 0)
  {
    SCI0_Timeout_ClearBuffer();
  }
  SCI0_Timeout_DataIncoming = 1;
  timeOutCounter = 0;

  if (SCI0SR1_RDRF) // CHECK the flag for receiving
  {
    data = SCI0DRL; // clear the flag by reading the register
    if (SCI0_Timeout_DataSize >= SCI0_Timeout_BufferSize)
    {
      SCI0_Timeout_BufferOverflow = 1;
    }
    else
      SCI0_Timeout_Data[SCI0_Timeout_DataSize++] = data;
  }
}

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

#include "ND_THERMO.h"
#include "ND_CLOCK.h"
#include "ND_I2C.h"

#include <stdio.h>

// other includes, as *required* for this implementation

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////
double THERMO_Pressure;
double THERMO_Temperature;
/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////

void THERMO_INIT(char InitI2C)
{
    char successFlag = 0;
    if (InitI2C)
    {
        I2C0_Init();
    }

    successFlag += I2C0_RegWrite(MPL3115A2_ADD, CTRL_REG1, 0x38, IIC0_STOP);
    successFlag += I2C0_RegWrite(MPL3115A2_ADD, PT_DATA_CFG, 0x07, IIC0_STOP);
    successFlag += I2C0_RegWrite(MPL3115A2_ADD, CTRL_REG1, 0x39, IIC0_STOP);
}

void THERMO_Read()
{
    int successFlag = 0;
    char raw[5];
    unsigned long data;

    while (!(raw[0] & DR_STATUS_PTDR_MASK))
    {
        successFlag = I2C0_RegRead(raw, MPL3115A2_ADD, DR_STATUS);
    }

    // Pressure
    successFlag = I2C0_RegRead(raw, MPL3115A2_ADD, 0x01);
    successFlag = I2C0_RegRead(raw + 1, MPL3115A2_ADD, 0x02);
    successFlag = I2C0_RegRead(raw + 2, MPL3115A2_ADD, 0x03);

    data = 0;
    data = raw[0];
    data <<= 8;
    data += raw[1];
    data <<= 2;
    data += (raw[2] >> 6);
    // THERMO_Pressure = (double)(long)(data) ;
        THERMO_Pressure = (double)(long)(data) + 7783;

    // Temperature
    successFlag = I2C0_RegRead(raw + 3, MPL3115A2_ADD, 0x04);
    successFlag = I2C0_RegRead(raw + 4, MPL3115A2_ADD, 0x05);

    THERMO_Temperature = ((raw[3] << 4) | ((data >> 4) & 0x0F)) / 16.0;
}
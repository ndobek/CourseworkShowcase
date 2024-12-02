/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Analog to digital converter
// Revision History
//      Created: 04/20/2024
/////////////////////////////////////////////////////////////////////////////
extern double THERMO_Pressure;
extern double THERMO_Temperature;
void THERMO_INIT(char InitI2C);
void THERMO_Read(void);

/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Analog to digital converter
// Revision History
//      Created: 03/18/2024
/////////////////////////////////////////////////////////////////////////////

extern unsigned int ADC_Results[8];
extern volatile char ADC_CompleteFlag;

void ADC_INIT(char firstChannel, char qtyOfChannels, char lastChannelBeforeWrap);
void ADC_SetScale(char newScale);
void ADC_EnableInterrupts(void (*function)(void));
void ADC_DisableInterrupts(void);

void ADC_ContinuousMode(char on);

void ADC_SetChannels(char firstChannel, char qtyOfChannels, char lastChannelBeforeWrap);

void ADC_Prescale_MHz(char MHz);

void ADC_ManualConvert(void);

char ADC_ResultsComplete(void);

void ADC_Read(void);
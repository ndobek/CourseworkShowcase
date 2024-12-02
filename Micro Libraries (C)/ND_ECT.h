/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Enhanced Capture Timer
// Revision History
//      Created: 03/23/2024
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
    ECT_Action_Disconnected,
    ECT_Action_Toggle,
    ECT_Action_Clear,
    ECT_Action_Set
} ECT_Action;

typedef enum
{
    ECT_EdgeType_Disabled,
    ECT_EdgeType_RisingEdge,
    ECT_EdgeType_FallingEdge,
    ECT_EdgeType_AnyEdge
} ECT_EdgeType;

typedef enum
{
    ECT_PulseAccumulatorA_Channel7,
    ECT_PulseAccumulatorB_Channel0
} ECT_PulseAccumulator;

extern unsigned int ECT_TickValue_Current[8];
extern unsigned int ECT_TickValue_Previous[8];
extern unsigned int ECT_Increments[8];

void ECT_INIT(void);

void ECT_Prescale_us(unsigned long us);
void ECT_Prescale_divisor(unsigned char divisor);

void ECT_SetResetTicks(unsigned int resetTicks);
void ECT_ConfigureOutputChannel(unsigned char channel, unsigned int startTicks, unsigned int incrementTicks);
void ECT_ConfigureInputChannel(unsigned char channel, ECT_EdgeType edgeType);

void ECT_ConfigureOutputChannelAction(unsigned char channel, ECT_Action action);
void ECT_ConfigureInputChannelEdgeType(unsigned char channel, ECT_EdgeType edgeType);

unsigned int ECT_DeltaTicks(unsigned char channel);
char ECT_ChannelHigh(unsigned char channel);

void ECT_LinkChannel(unsigned char channel, char driveValue);
void ECT_UnlinkChannel(unsigned char channel);

void ECT_EnableInterrupt(unsigned char channel, void (*function)(void));
void ECT_DisableInterrupt(unsigned char channel);

void ECT_EnablePulseAccumulator(ECT_PulseAccumulator PA);
unsigned int ECT_ReadPulseAccumulator(ECT_PulseAccumulator PA);
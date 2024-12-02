/*
  Periodic Interrupt Timer (PIT) Header File
  FILE: pit.h
  Created on: October 24th, 2022, by Carlos Estay
  Edited March 4th, 2023
*/

typedef enum PIT_ChannelTyepedef_
{
    PIT_CH0 = 0b00000001,
    PIT_CH1 = 0b00000010,
    PIT_CH2 = 0b00000100,
    PIT_CH3 = 0b00001000
}PIT_Channel;

typedef enum PIT_MicroTimerTyepedef_
{
    PIT_MT1 = 1,
    PIT_MT0 = 0
}PIT_MicroTimer;

typedef enum PIT_InterruptTyepedef_
{
    PIT_IEN = 1,
    PIT_IDIS = 0
}PIT_Interrupt;

/// @brief Configures a channel (ch0 - ch3)
/// @param ch The channel in question (PIT_CH0 - PIT_CH3)
/// @param mt The micro-timer to be connected to (MT1 or MT0(default))
/// @param ie Enables or disables interrupt for the channel
void PIT_InitChannel(PIT_Channel ch, PIT_MicroTimer mt, PIT_Interrupt ie);

void PIT_EnableInterrupt(PIT_Channel ch, void (*function)(void));
void PIT_DisableInterrupt(PIT_Channel ch);
void PIT_ClearFlag(PIT_Channel ch);

void PIT_SetDelay(PIT_Channel ch, unsigned long ticks, PIT_MicroTimer mt);
void PIT_SetDelay_ms(PIT_Channel ch, int ms);
void PIT_SetDelay_us(PIT_Channel ch, unsigned long us);
unsigned long PIT_GetTotalRequiredTicks(unsigned long ms);

void PIT_SetChannelTicks(PIT_Channel ch, unsigned int chTicks);
unsigned int PIT_GetChannelTicks(PIT_Channel ch);

void PIT_ResetCount(PIT_Channel ch);

/// @brief Configures the channel to a 1[ms] event, fix connection to micro-timer1 
/// @param ch The channel to be configured
void PIT_Set1msDelay(PIT_Channel ch);

char PIT_TimerElapsed(PIT_Channel ch);

/// @brief This enables the PIT. It is recommened to be called last. 
///        Nothing will work if this is not called
void PIT_Start(void);

void PIT_SetMTTicks(PIT_MicroTimer mt, unsigned int ticks);
unsigned int PIT_GetMTTicks(PIT_MicroTimer mt);

/// @brief A blocking delay function in milliseconds
/// @param ch The channel to use with the delay
/// @param ms The number of milliseconds to delay
void PIT_Sleep(int ms);

/// @brief Optional. Reasonable for anything above 20us
/// @param ch The channel to use with the delay
/// @param us The number of microseconds to delay
void PIT_Sleep_us(int us);
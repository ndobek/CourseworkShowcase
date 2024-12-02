// RTI Module Library
// File: rti.h (header file)
// Processor: MC9S12XDP512
// Crystal: 16 MHz
// by Carlos Estay
// September 2023
// Last edit, October 24th, 2023

/// @brief Enables RTI Moule
/// @param
void RTI_INIT(void);

/// @brief Blocking delay to be used  once the RTI MOdule is enabled
/// @param timeout
void RTI_Delay(unsigned int ms);

/// @brief Enables RTI Module with callback to be used in main
/// @param function
/*
We will implemente this function later once we cover
function pointers
*/
void RTI_INIT_Callback(void (*function)(void));

/// @brief Enables RTI Module with callback to be used in main and called
///        every "x" milliseconds
/// @param function
/// @param interval in [ms]
void RTI_INIT_Callback_ms(void (*function)(void), unsigned int);

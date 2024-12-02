/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     20 MHz (Requires Active PLL)
// Author:        Simon Walker
// Details:       LCD Library
// Revision History :
// Created : Unknown
//  Dec 07 2020 - Modified Documentation
//  Dec    2020 - Modified names, modified to use timer for delays
//  Nov 2021, Added PIT Timer for delays, by Carlos
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Library Prototypes
/////////////////////////////////////////////////////////////////////////////

unsigned char ExtractDigit_Dec_Val(unsigned int value, unsigned char digitPlace);
unsigned char ExtractDigit_Hex_Val(unsigned int value, unsigned char digitPlace);
unsigned char ExtractDigit_Bin_Val(unsigned int value, unsigned char digitPlace);
unsigned char ExtractDigit_Dec_Char(unsigned int value, unsigned char digitPlace);
unsigned char ExtractDigit_Hex_Char(unsigned int value, unsigned char digitPlace);
unsigned char ExtractDigit_Bin_Char(unsigned int value, unsigned char digitPlace);



unsigned int GreatestCommonDenominator(unsigned int n1, unsigned int n2);
unsigned int GreatestCommonDenominator_Three(unsigned int n1, unsigned int n2, unsigned int n3);
void Reduce(unsigned int* n1, unsigned int* n2);
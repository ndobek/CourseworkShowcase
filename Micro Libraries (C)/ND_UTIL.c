/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     ? MHz
// Author:        Nathaniel Dobek
// Details:       Misc Helper Functions
// Revision History
//
/////////////////////////////////////////////////////////////////////////////
#include <hidef.h>      /* common defines and macros */
#include "derivative.h" /* derivative-specific definitions */

#include <math.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// local prototypes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// library variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// constants
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// function implementations
/////////////////////////////////////////////////////////////////////////////
unsigned char ExtractDigit_Dec_Val(unsigned int value, unsigned char digitPlace)
{
    return (value / (int)(pow(10, digitPlace))) % 10;
}
unsigned char ExtractDigit_Hex_Val(unsigned int value, unsigned char digitPlace)
{
    return (value >> (unsigned char)(4 * digitPlace)) & 0x000F;
}
unsigned char ExtractDigit_Bin_Val(unsigned int value, unsigned char digitPlace)
{
    return (value >> digitPlace) & 0x0001;
}
unsigned char ExtractDigit_Dec_Char(unsigned int value, unsigned char digitPlace)
{
    return '0' + ExtractDigit_Dec_Val(value, digitPlace);
}
unsigned char ExtractDigit_Hex_Char(unsigned int value, unsigned char digitPlace)
{
    int extractedValue = ExtractDigit_Hex_Val(value, digitPlace);
    if (extractedValue < 10)
        return '0' + extractedValue;
    else
        return 'A' + (extractedValue - 10);
}
unsigned char ExtractDigit_Bin_Char(unsigned int value, unsigned char digitPlace)
{
    return '0' + ExtractDigit_Bin_Val(value, digitPlace);
}

// I Got this algorithm from https://www.programiz.com/c-programming/examples/hcf-gcd
unsigned int GreatestCommonDenominator(unsigned int n1, unsigned int n2)
{
    while (n1 != n2)
    {
        if (n1 > n2)
            n1 -= n2;
        else
            n2 -= n1;
    }
    return n1;
}
unsigned int GreatestCommonDenominator_Three(unsigned int n1, unsigned int n2, unsigned int n3)
{
    while ((n1 != n2) || (n1 != n3))
    {
        unsigned int min = n1;

        if (n2 < min)
            min = n2;
        if (n3 < min)
            min = n3;

        if (min == 1)
            return 1;

        if (min != n1)
            n1 -= min;
        if (min != n2)
            n2 -= min;
        if (min != n3)
            n3 -= min;
    }
    return n1;
}

// BEYOND THIS LINE IS UNTESTED
void Reduce(unsigned int *n1, unsigned int *n2)
{
    unsigned int gcd;
    do
    {
        gcd = GreatestCommonDenominator(*n1, *n2);
        *n1 /= gcd;
        *n2 /= gcd;
    } while (gcd != 1);
}

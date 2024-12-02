/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Author:        Simon Walker
// Details:       library for ICM7212A 7-segment display
// Revision History :
// Created : *unknown*
// May 9th 2014   - Port/Fixup to C library
// June 2018      - Cleanup
// Dec 07 2020    - simplified helpers to be #defines
//                - tested library for 20MHz operation
// Dec    2020    - changed name, simplified naming
//                - made arg type same, simplified
// Mar 13 2021    - added line specific Err function
//                - deprecate CodeB use and HELP function after term 1202
//                - added enum for line targeting (16H, 16D, Err)
// Oct 25 2022    - Small edits by Carlos
// Oct 25 2022    - Added union /bitfields and constant defines
// device connection
// PA0 - W*
// PA1 - MODE       
// PB0-PB7 - IDO-ID7

/////////////////////////////////////////////////////////////////////////////
#define DEGREE_SIGN SEG_A | SEG_B | SEG_F | SEG_G
#define SEG_LETTER_E SEG_A | SEG_D | SEG_F | SEG_G | SEG_E | SEG_NO_DP
#define SEG_LETTER_R SEG_G | SEG_E | SEG_NO_DP

#define SEG_A 0b01000000
#define SEG_B 0b00100000 
#define SEG_C 0b00010000 
#define SEG_D 0b00000001 
#define SEG_E 0b00001000 
#define SEG_F 0b00000010 
#define SEG_G 0b00000100 


#define SEG_DP 0b00000000
#define SEG_NO_DP 0b10000000
#define SEG_DP_MASK 0b00000000
#define SEG_NO_DP_MASK 0b10000000

/////////////////////////////////////////////////////////////////////////////
// Optional Custom Unions/bitfields typedef
/////////////////////////////////////////////////////////////////////////////
typedef union SevSegTypedef__
{
  unsigned char Byte;
  struct SevenSeg_Bits
  {
    unsigned int ADDR:3; 
    unsigned int BANK:1;
    unsigned int SDWN:1;
    unsigned int DEC:1;
    unsigned int HEX_B:1;
    unsigned int S_M:1;
  }Bits;
}SevSegTypedef;

typedef union Seg16Typedef__
{
  unsigned int Word;

  struct Seg16_Byte
  {//This is because our micro is Little Endian (LSB first)
    unsigned int Byte1:8;
    unsigned int Byte0:8;  
  }Byte;

  struct Seg16_Nibble
  {//This is because our micro is Little Endian (LSB first)
    unsigned int Nibble2:4;
    unsigned int Nibble3:4;
    unsigned int Nibble0:4;
    unsigned int Nibble1:4;
  }Nibble;
  
}Seg16Typedef;


/////////////////////////////////////////////////////////////////////////////
// Custom Type Definition
/////////////////////////////////////////////////////////////////////////////

// for functions that want to add/kill the decimal point
typedef enum SEG_DPOption_
{
  SEG_DP_OFF = 0, 
  SEG_DP_ON = 1
} SEG_DPOption;

// for functions that specify the upper or lower full display
typedef enum SEG_LineOption_
{
  SEG_LineTop = 0, 
  SEG_LineBottom = 1
} SEG_LineOption;


/////////////////////////////////////////////////////////////////////////////
// Library Prototypes
/////////////////////////////////////////////////////////////////////////////
// setup ports to speak with 7-segs
void SEG_INIT (void);

void SEG_SendDataRaw(unsigned char control, unsigned char data);
void SEG_SetBitType(unsigned char);
void SEG_Latch(void);

// show HEX decoding (changes all of display to HEX, w/wo dp)
/* Params: (address, data, dp) */
void SEG_Normal (unsigned char, unsigned char, SEG_DPOption);

// control segs manually
/* Params: (address, data) */
void SEG_Custom (unsigned char, unsigned char);

// clear the display
/* Params: (address) */
void SEG_ClearDigit(unsigned char);

// clear the entire display
void SEG_Clear(void);

// clear display  upper or lower line
/* Params: (line) */
void SEG_ClearLine(SEG_LineOption);

// show a 16-bit value on the upper or lower display
/* Params: (value, line) */
void SEG_16H (unsigned int, SEG_LineOption);

// show a decimal value on the first or second line of the 7-segs
/* Params: (value, line) */
void SEG_16D (unsigned int, SEG_LineOption);

// show the 8-bit value starting on the digit as addr (0-6)
/* Params: (addr, value) */
void SEG_8H (unsigned char, unsigned char);

// say Err on the appropriate line
/* Params: (line) */
void SEG_SayErr (SEG_LineOption);




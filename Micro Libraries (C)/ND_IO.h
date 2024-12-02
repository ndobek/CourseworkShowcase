/////////////////////////////////////////////////////////////////////////////
// Processor:     MC9S12XDP512
// Bus Speed:     8 MHz
// Author:        Nathaniel Dobek
// Details:       Generic Input and Output
// Revision History
//      Created: 10/02/2023
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Library Prototypes
/////////////////////////////////////////////////////////////////////////////

#define IO PT1AD1

typedef enum LED_MASKTypeDef
{
    LED_MASK_ALL = 0b11100000,
    LED_MASK_RED = 0b10000000,
    LED_MASK_YELLOW = 0b01000000,
    LED_MASK_GREEN = 0b00100000
} LED_MASK;

typedef enum SWITCH_MASKTypeDef
{
    SWITCH_MASK_ALL = 0b00011111,
    SWITCH_MASK_TOP = 0b00010000,
    SWITCH_MASK_LEFT = 0b00001000,
    SWITCH_MASK_BOTTOM = 0b00000100,
    SWITCH_MASK_RIGHT = 0b00000010,
    SWITCH_MASK_CTR = 0b00000001
} SWITCH_MASK;

typedef enum SWITCH_STATETypeDef
{
    Idle,
    Pressed,
    Held,
    Released
} SWITCH_STATE;

typedef enum J_EDGETypeDef
{
    J0_PRESS = 0b00000001,
    J0_RELEASE = 0b00000010,
    J1_PRESS = 0b00000100,
    J1_RELEASE = 0b00001000
} J_EDGE;

extern SWITCH_STATE SWITCH_TOP;
extern SWITCH_STATE SWITCH_LEFT;
extern SWITCH_STATE SWITCH_BOTTOM;
extern SWITCH_STATE SWITCH_RIGHT;
extern SWITCH_STATE SWITCH_CTR;
extern SWITCH_STATE SWITCH_ALL;

void IO_INIT(void);
void IO_UPDATE(void);

int LED_GET(LED_MASK LEDMask);
void LED_SET(LED_MASK LEDMask, int onOff);
void LED_SET_ALL(LED_MASK LEDMask);
void LED_TOGGLE(LED_MASK LEDMask);
void LED_ON(LED_MASK LEDMask);
void LED_OFF(LED_MASK LEDMask);

void SWITCH_UPDATE(SWITCH_STATE *pastState, int isPressed);
void SWITCH_UPDATE_ALL(void);
int SWITCH_GET_RAW(SWITCH_MASK SWITCHMask);
int SWITCH_GET_RAW_ANY(void);
int SWITCH_QTY_ON(void);

void J_INIT(J_EDGE edges, void (*function)(void));
char J_IsHigh(char);

/////////////////////////////////////////////////////////////////////////////
// Hidden Helpers (local to implementation only)
/////////////////////////////////////////////////////////////////////////////

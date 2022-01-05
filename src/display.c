#include "display.h"
#include "utils.h"

#include <stm32f0xx.h>

#define GPIO_MODE_INPUT     0
#define GPIO_MODE_OUTPUT    1
#define GPIO_MODE_ALTERNATE 2
#define GPIO_MODE_ANALOG    3

#define GPIO_SPEED_SLOW     0
#define GPIO_SPEED_MEDIUM   2
#define GPIO_SPEED_HIGH     3

#define DISPLAY_SEGA_Pos    0
#define DISPLAY_SEGA_Msk    (1 << DISPLAY_SEGA_Pos)

#define DISPLAY_SEGB_Pos    1
#define DISPLAY_SEGB_Msk    (1 << DISPLAY_SEGB_Pos)

#define DISPLAY_SEGC_Pos    2
#define DISPLAY_SEGC_Msk    (1 << DISPLAY_SEGC_Pos)

#define DISPLAY_SEGD_Pos    3
#define DISPLAY_SEGD_Msk    (1 << DISPLAY_SEGD_Pos)

#define DISPLAY_SEGE_Pos    4
#define DISPLAY_SEGE_Msk    (1 << DISPLAY_SEGE_Pos)

#define DISPLAY_SEGF_Pos    5
#define DISPLAY_SEGF_Msk    (1 << DISPLAY_SEGF_Pos)

#define DISPLAY_SEGG_Pos    6
#define DISPLAY_SEGG_Msk    (1 << DISPLAY_SEGG_Pos)

#define DISPLAY_SEGH_Pos    7
#define DISPLAY_SEGH_Msk    (1 << DISPLAY_SEGH_Pos)

#define DISPLAY_DIGIT_Msk   (DISPLAY_SEGA_Msk \
  | DISPLAY_SEGB_Msk \
  | DISPLAY_SEGC_Msk \
  | DISPLAY_SEGD_Msk \
  | DISPLAY_SEGE_Msk \
  | DISPLAY_SEGF_Msk \
  | DISPLAY_SEGG_Msk \
  | DISPLAY_SEGH_Msk)

#define DISPLAY_CAT1_Pos    8
#define DISPLAY_CAT1_Msk    (1 << DISPLAY_CAT1_Pos)

#define DISPLAY_CAT2_Pos    9
#define DISPLAY_CAT2_Msk    (1 << DISPLAY_CAT2_Pos)

#define DISPLAY_CAT3_Pos    10
#define DISPLAY_CAT3_Msk    (1 << DISPLAY_CAT3_Pos)

#define DISPLAY_CAT4_Pos    11
#define DISPLAY_CAT4_Msk    (1 << DISPLAY_CAT4_Pos)

#define DISPLAY_CAT5_Pos    12
#define DISPLAY_CAT5_Msk    (1 << DISPLAY_CAT5_Pos)

#define DISPLAY_CAT6_Pos    13
#define DISPLAY_CAT6_Msk    (1 << DISPLAY_CAT6_Pos)

#define DISPLAY_CATODES_Msk (DISPLAY_CAT1_Msk \
  | DISPLAY_CAT2_Msk \
  | DISPLAY_CAT3_Msk \
  | DISPLAY_CAT4_Msk \
  | DISPLAY_CAT5_Msk \
  | DISPLAY_CAT6_Msk)

#define LED_CHARGE_Pos      14
#define LED_CHARGE_Msk      (1 << LED_CHARGE_Pos)

#define LED_OVERCURRENT_Pos 15
#define LED_OVERCURRENT_Msk (1 << LED_OVERCURRENT_Pos)

#define DISPLAY_DECIMAL_POINT 0x80
#define DISPLAY_ERROR         0x40

unsigned char green_buffer[3];
unsigned char red_buffer[3];
bool charge_state;
bool overcurrent_state;
unsigned char current_digit;

const unsigned char symbol_table[10] =
{
  0x3F,                                                     // 0
  0x06,                                                     // 1
  0x5B,                                                     // 2
  0x4F,                                                     // 3
  0x46,                                                     // 4
  0x6D,                                                     // 5
  0x7D,                                                     // 6
  0x07,                                                     // 7
  0x7F,                                                     // 8
  0x6F                                                      // 9
};

//******************  SelectCathode  ******************

void SelectCathode(int n)
{
  unsigned long u32;

  if (n < 6 && n >= 0)
    {
      u32 = GPIOB->ODR;
      u32 &= ~DISPLAY_CATODES_Msk;
      u32 |= (1 << (DISPLAY_CAT1_Pos + n));
      GPIOB->ODR = u32;
    }
  else
    GPIOB->ODR &= ~DISPLAY_CATODES_Msk;
}

//********************  SetDigit  *********************

void SetDigit(unsigned char n)
{
  GPIOB->ODR &= ~DISPLAY_DIGIT_Msk;
  if (n < 3)
    GPIOB->ODR |= green_buffer[n];
  else if (n < 6)
    GPIOB->ODR |= red_buffer[n - 3];
}

//********************  PrintFloat  *******************

void PrintFloat(unsigned char * buffer, float value)
{
  uint32_t int_val;
  uint32_t num;

  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;

  if (value < 0)
    {
      buffer[0] = symbol_table[0];
      buffer[1] = symbol_table[0];
      buffer[2] = symbol_table[0] | DISPLAY_DECIMAL_POINT;
      return;
    }
  else if (value < 9.999f)
    {
      buffer[0] = DISPLAY_DECIMAL_POINT;
      int_val = value*100.0f;
    }
  else if (value < 99.99f)
    {
      buffer[1] = DISPLAY_DECIMAL_POINT;
      int_val = value*10.0f;
    }
  else if (value < 999.9f)
    {
      buffer[2] = DISPLAY_DECIMAL_POINT;
      int_val = value;
    }
  else
    {
      buffer[0] = DISPLAY_ERROR;
      buffer[1] = symbol_table[0];
      buffer[2] = symbol_table[0];
      return;
    }

  num = int_val/100;
  buffer[0] |= symbol_table[num];
  int_val %= 100;

  num = int_val/10;
  buffer[1] |= symbol_table[num];

  buffer[2] |= symbol_table[int_val%10];
}

//*******************  InitDisplay  *******************

void InitDisplay()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;                        // Enable GPIOB in RCC

  GPIOB->MODER |= (GPIO_MODE_OUTPUT << DISPLAY_SEGA_Pos*2)  // Set display pins as outputs
    | (GPIO_MODE_OUTPUT << DISPLAY_SEGB_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_SEGC_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_SEGD_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_SEGE_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_SEGF_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_SEGG_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_SEGH_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_CAT1_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_CAT2_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_CAT3_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_CAT4_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_CAT5_Pos*2)
    | (GPIO_MODE_OUTPUT << DISPLAY_CAT6_Pos*2)
    | (GPIO_MODE_OUTPUT << LED_CHARGE_Pos*2)
    | (GPIO_MODE_OUTPUT << LED_OVERCURRENT_Pos*2);

  GPIOB->OSPEEDR |= (GPIO_SPEED_HIGH << DISPLAY_SEGA_Pos*2) // Set display pins as outputs
    | (GPIO_SPEED_HIGH << DISPLAY_SEGB_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_SEGC_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_SEGD_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_SEGE_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_SEGF_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_SEGG_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_SEGH_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_CAT1_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_CAT2_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_CAT3_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_CAT4_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_CAT5_Pos*2)
    | (GPIO_SPEED_HIGH << DISPLAY_CAT6_Pos*2)
    | (GPIO_SPEED_HIGH << LED_CHARGE_Pos*2)
    | (GPIO_SPEED_HIGH << LED_OVERCURRENT_Pos*2);

  GPIOB->ODR &= ~(DISPLAY_SEGA_Msk                          // Clear all display
    | DISPLAY_SEGB_Msk
    | DISPLAY_SEGC_Msk
    | DISPLAY_SEGD_Msk
    | DISPLAY_SEGE_Msk
    | DISPLAY_SEGF_Msk
    | DISPLAY_SEGG_Msk
    | DISPLAY_SEGH_Msk
    | DISPLAY_CAT1_Msk
    | DISPLAY_CAT2_Msk
    | DISPLAY_CAT3_Msk
    | DISPLAY_CAT4_Msk
    | DISPLAY_CAT5_Msk
    | DISPLAY_CAT6_Msk
    | LED_CHARGE_Msk
    | LED_OVERCURRENT_Msk);

  // Set display state

  current_digit = 0;

  SetTestState(true);
}

//*******************  SetTestState  ******************

void SetTestState(bool state)
{
  if (state)
    {
      green_buffer[0] = 0xFF;
      green_buffer[1] = 0xFF;
      green_buffer[2] = 0xFF;

      red_buffer[0] = 0xFF;
      red_buffer[1] = 0xFF;
      red_buffer[2] = 0xFF;

      charge_state = true;
      overcurrent_state = true;
    }
  else
    {
      green_buffer[0] = symbol_table[0];
      green_buffer[1] = symbol_table[0];
      green_buffer[2] = symbol_table[0];

      red_buffer[0] = symbol_table[0];
      red_buffer[1] = symbol_table[0];
      red_buffer[2] = symbol_table[0];

      charge_state = false;
      overcurrent_state = false;
    }
}

//******************  SetGreenValue  ******************

void SetGreenValue(float value)
{
  PrintFloat(green_buffer, value);
}

//*******************  SetRedValue  *******************

void SetRedValue(float value)
{
  PrintFloat(red_buffer, value);
}

//******************  DisplayIterate  *****************

void DisplayIterate()
{
  SelectCathode(-1);
  SetDigit(current_digit);
  SelectCathode(current_digit);

  current_digit++;
  if (current_digit >= 6)
    current_digit = 0;
}

#include "keyboard.h"
#include "hal/systick.h"

#include <stm32f0xx.h>

#define GPIO_MODE_INPUT     0
#define GPIO_MODE_OUTPUT    1
#define GPIO_MODE_ALTERNATE 2
#define GPIO_MODE_ANALOG    3

#define GPIO_SPEED_SLOW     0
#define GPIO_SPEED_MEDIUM   2
#define GPIO_SPEED_HIGH     3

#define KEYBOARD_KEY_START_Pos  0
#define KEYBOARD_KEY_START_Msk  (1 << KEYBOARD_KEY_START_Pos)

#define KEYBOARD_KEY_UP_Pos     1
#define KEYBOARD_KEY_UP_Msk     (1 << KEYBOARD_KEY_UP_Pos)

#define KEYBOARD_KEY_DOWN_Pos   6
#define KEYBOARD_KEY_DOWN_Msk   (1 << KEYBOARD_KEY_DOWN_Pos)

#define KEYBOARD_KEY_MODE_Pos   7
#define KEYBOARD_KEY_MODE_Msk   (1 << KEYBOARD_KEY_MODE_Pos)

#define KEYBOARD_Msk (KEYBOARD_KEY_START_Msk \
  | KEYBOARD_KEY_UP_Msk \
  | KEYBOARD_KEY_DOWN_Msk \
  | KEYBOARD_KEY_MODE_Msk)

// FIXME : Make keyboard timeout independent of charger cycle
#define KEYBOARD_TIMEOUT 250

key_callback_proc_t keyboard_callback;
uint32_t key_timer;
key_t hold_key;

/***********************  InitKeyboard  ***********************/

void InitKeyboard(key_callback_proc_t callback)
{
  RCC->AHBENR |= RCC_AHBENR_GPIOFEN;                        // Enable GPIOF in RCC

  GPIOF->MODER |= (GPIO_MODE_INPUT << KEYBOARD_KEY_START_Pos*2)   // Set keyboard pins as inputs
    | (GPIO_MODE_INPUT << KEYBOARD_KEY_UP_Pos*2)
    | (GPIO_MODE_INPUT << KEYBOARD_KEY_DOWN_Pos*2)
    | (GPIO_MODE_INPUT << KEYBOARD_KEY_MODE_Pos*2);

  keyboard_callback = callback;
  hold_key = kNone;
  key_timer = 0;
}

/************************  GetKeyState  ***********************/

key_t GetKeyState()
{
  uint32_t keys = ~(GPIOF->IDR) & KEYBOARD_Msk;

  switch (keys)
    {

    case KEYBOARD_KEY_START_Msk:
      return kStart;

    case KEYBOARD_KEY_UP_Msk:
      return kUp;

    case KEYBOARD_KEY_DOWN_Msk:
      return kDown;

    case KEYBOARD_KEY_MODE_Msk:
      return kMode;

    default:
      return kNone;

    }
}

/**********************  ProcessKeyboard  *********************/

void ProcessKeyboard()
{
  key_t key = GetKeyState();
  if (key != kNone)
    {
      if (key != hold_key)
        {
          hold_key = key;
          if (keyboard_callback != 0)
            keyboard_callback(key);
        }
      else if (key_timer >= KEYBOARD_TIMEOUT)
        {
          key_timer = 0;
          if (keyboard_callback != 0)
            keyboard_callback(key);
        }
      else
        key_timer++;
    }
  else
    {
      key_timer = 0;
      hold_key = kNone;
    }
}

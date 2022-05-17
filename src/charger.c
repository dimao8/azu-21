#include "charger.h"
#include "display.h"
#include "keyboard.h"

#include "hal/systick.h"
#include "hal/adc.h"
#include "hal/pwm.h"

#include <stdint.h>

#define CURRENT_LIMIT             500
#define VOLTAGE_LIMIT             21000
#define CURRENT_TRESHOLD_LIMIT    100

#define DEFAULT_CHARGER_VOLTAGE   21000
#define DEFAULT_CHARGER_CURRENT   500

#define DEFAULT_CHARGER_CYCLE_MS  2

#define TEST_TIMEOUT_MS           2000
#define VALUES_UPDATE_TIMEOUT_MS  500
#define VALUES_BLINK_TIMEOUT_MS   1000

/**
 * State machine enumerator for charger states.
 */
typedef enum charger_state_tag
{
  stTest,                                         //!< Test is shown
  stIdle,                                         //!< Normal state of the charger. No charge
  stCharge,                                       //!< Charge in progress
  stVoltage,                                      //!< Voltage set state
  stCurrent                                       //!< Current set state
} charger_state_t;

charger_state_t charger_state;                    //!< State of the charger

uint32_t charger_current;                         //!< The maximum current on second stage (mA)
uint32_t charger_voltage;                         //!< The maximum voltage at the end of charging (mV)

uint32_t measured_current;                        //!< Current in charge process
uint32_t measured_voltage;                        //!< Voltage in charge process

uint32_t charger_timer;                           //!< Global timer for charger

// TODO : Delete this -->
int n;
// <--

/**************************  InitCharger  **************************/

void init_charger()
{
  charger_current = DEFAULT_CHARGER_CURRENT;
  charger_voltage = DEFAULT_CHARGER_VOLTAGE;

  measured_current = 0;
  measured_voltage = 0;

  charger_state = stTest;

  charger_timer = 0;

  init_systick();
  init_display();
  init_keyboard(on_key_press);
  init_adc();
  init_pwm();

  n = 1;
}

/*****************************  OnIdle  ****************************/

void on_idle()
{
  int adc_reference;

  display_iterate();
  msleep(DEFAULT_CHARGER_CYCLE_MS);
  charger_timer += DEFAULT_CHARGER_CYCLE_MS;
  process_keyboard();

  switch (charger_state)
    {

    case stTest:
      set_pwm_value(0);
      if (charger_timer > TEST_TIMEOUT_MS)
        {
          charger_state = stIdle;
          set_test_state(false);
          set_green_value(charger_voltage);
          set_red_value(charger_current);
          charger_timer = 0;
        }
      break;

    case stIdle:
      set_pwm_value(0);
      if (charger_timer > VALUES_UPDATE_TIMEOUT_MS)
        {
          set_green_value(charger_voltage);
          set_red_value(charger_current);
          charger_timer = 0;
        }
      break;

    case stCharge:
      // UpdateCharge();
      adc_reference = convet_reference();

      measured_current = convert_channel(ADC_CHANNEL_ISEN);
      measured_current = measured_current*adc_reference/4096;

      measured_voltage = convert_channel(ADC_CHANNEL_VSENP);
      measured_voltage -= convert_channel(ADC_CHANNEL_VSENN);
      measured_voltage = measured_voltage*adc_reference/4096;

      // TODO : Delete this -->
      set_pwm_value(n*10);
      // <--
      // set_green_value(measured_voltage);
      set_red_value(measured_current);
      break;

    case stVoltage:
      set_pwm_value(0);
      if (charger_timer > VALUES_BLINK_TIMEOUT_MS)
        {
          charger_timer = 0;
        }
      if (charger_timer > VALUES_BLINK_TIMEOUT_MS/2)
        clear_green_value();
      else
        set_green_value(charger_voltage);
      break;

    case stCurrent:
      set_pwm_value(0);
      if (charger_timer > VALUES_BLINK_TIMEOUT_MS)
        {
          charger_timer = 0;
        }
      if (charger_timer > VALUES_BLINK_TIMEOUT_MS/2)
        clear_red_value();
      else
        set_red_value(charger_current);
      break;

    default:
      break;

    }
}

/***************************  OnKeyPress  **************************/

void on_key_press(key_t key)
{
  switch (charger_state)
    {

    case stIdle:
      switch (key)
        {

        case k_mode:
          charger_state = stVoltage;
          break;

        case k_start:
          // TODO : StartCharge();
          charger_state = stCharge;
          set_pwm_value(0);
          break;

        default:
          break;

        }
      break;

    case stVoltage:
      switch (key)
        {

        case k_mode:
          charger_state = stCurrent;
          break;

        case k_up:
          charger_voltage += 100;
          if (charger_voltage > VOLTAGE_LIMIT)
            charger_voltage = VOLTAGE_LIMIT;
          break;

        case k_down:
          charger_voltage -= 100;
          if (charger_voltage > VOLTAGE_LIMIT)
            charger_voltage = 0;
          break;

        default:
          break;

        }
      break;

    case stCurrent:
      switch (key)
        {

        case k_mode:
          charger_state = stIdle;
          break;

        case k_up:
          charger_current += 10;
          if (charger_current > CURRENT_LIMIT)
            charger_current = CURRENT_LIMIT;
          break;

        case k_down:
          charger_current -= 10;
          if (charger_current > CURRENT_LIMIT)
            charger_current = 0;
          break;

        default:
          break;

        }
      break;

    case stCharge:
      switch (key)
        {

          case k_up:
            n++;
            if (n > 100)
              n = 100;
            set_green_value(n*10);
            break;

          case k_down:
            n--;
            if (n < 0)
              n = 0;
            set_green_value(n*10);
            break;

        }
      break;

    default:
      break;

    }
}

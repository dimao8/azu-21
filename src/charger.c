#include "charger.h"
#include "display.h"
#include "keyboard.h"

#include "hal/systick.h"
#include "hal/adc.h"
#include "hal/pwm.h"

#include <stdint.h>

#define CURRENT_LIMIT               500
#define VOLTAGE_LIMIT               21000
#define CURRENT_TRESHOLD_LIMIT      100

#define DEFAULT_CHARGER_VOLTAGE     21000
#define DEFAULT_CHARGER_CURRENT     500

#define DEFAULT_CHARGER_CYCLE_MS    2

#define TEST_TIMEOUT_MS             2000
#define VALUES_UPDATE_TIMEOUT_MS    500
#define VALUES_BLINK_TIMEOUT_MS     1000
#define PRECHARGE_TIMEOUT_MS        10000
#define FILTER_FREQUENCY_REDUCTION  100

#define IIR_A0                      1.0f
#define IIR_A1                      -0.9390625f
#define IIR_B0                      0.0304687f
#define IIR_B1                      0.0304687f

#define CURRENT_P_K                 0.1E-6f
#define CURRENT_I_K                 0.1E-6f

#define VOLTAGE_P_K                 0.1E-6f
#define VOLTAGE_I_K                 0.1E-6f

/**
 * State machine enumerator for charger states.
 */
typedef enum charger_state_tag
{
  st_test,                                        //!< Test is shown
  st_idle,                                        //!< Normal state of the charger. No charge
  st_charge,                                      //!< Charge in progress
  st_voltage,                                     //!< Voltage set state
  st_current                                      //!< Current set state
} charger_state_t;

charger_state_t charger_state;                    //!< State of the charger

/**
 * Charger process stage
 */
typedef enum charge_process_stage_tag
{
  cs_off,                                         //!< Charge is now stopped
  cs_testing,                                     //!< Test accumulator
  cs_precharge,                                   //!< Precharge stage
  cs_current,                                     //!< Charge with constant current
  cs_voltage                                      //!< Charge with constant voltage
} charge_process_stage_t;

charge_process_stage_t charge_process_stage;

uint32_t charger_current;                         //!< The maximum current on second stage (mA)
uint32_t charger_voltage;                         //!< The maximum voltage at the end of charging (mV)

uint32_t measured_current[2];                     //!< Current in charge process
uint32_t measured_voltage[2];                     //!< Voltage in charge process

uint32_t filtered_current;                        //!< Filtered current in charging process
uint32_t filtered_voltage;                        //!< Filtered voltage in charging process

uint32_t charger_timer;                           //!< Global timer for charger

/**
 * Update charger during charging process. Call every 2ms
 */
void update_charger();

/**************************  init_charger  **************************/

void init_charger()
{
  charger_current = DEFAULT_CHARGER_CURRENT;
  charger_voltage = DEFAULT_CHARGER_VOLTAGE;

  measured_current[0] = 0;
  measured_current[1] = 0;
  measured_voltage[0] = 0;
  measured_voltage[1] = 0;

  charger_state = st_test;
  charge_process_stage = cs_off;

  charger_timer = 0;

  init_systick();
  init_display();
  init_keyboard(on_key_press);
  init_adc();
  init_pwm();
}

/*****************************  on_idle  ****************************/

void on_idle()
{
  int adc_reference;

  display_iterate();
  msleep(DEFAULT_CHARGER_CYCLE_MS);
  charger_timer += DEFAULT_CHARGER_CYCLE_MS;
  process_keyboard();

  switch (charger_state)
    {

    case st_test:
      set_pwm_value(0);
      if (charger_timer > TEST_TIMEOUT_MS)
        {
          charger_state = st_idle;
          set_test_state(false);
          set_green_value(charger_voltage);
          set_red_value(charger_current);
          charger_timer = 0;
        }
      break;

    case st_idle:
      set_pwm_value(0);
      if (charger_timer > VALUES_UPDATE_TIMEOUT_MS)
        {
          set_green_value(charger_voltage);
          set_red_value(charger_current);
          charger_timer = 0;
        }
      break;

    case st_charge:
      adc_reference = convert_reference();

      measured_current[0] = measured_current[1];
      measured_current[1] = convert_channel(ADC_CHANNEL_ISEN);
      measured_current[1] = measured_current[1]*adc_reference/4096;
      filtered_current = (1.0f/IIR_A0)*(IIR_B1*measured_current[1] + IIR_B0*measured_current[0] - filtered_current*IIR_A1);

      measured_voltage[0] = measured_voltage[1];
      measured_voltage[1] = convert_channel(ADC_CHANNEL_VSENP);
      measured_voltage[1] -= convert_channel(ADC_CHANNEL_VSENN);
      measured_voltage[1] = measured_voltage[1]*adc_reference/4096;
      filtered_voltage = (1.0f/IIR_A0)*(IIR_B1*measured_voltage[1] + IIR_B0*measured_voltage[0] - filtered_voltage*IIR_A1);

      // update_charger();
      
      set_green_value(filtered_voltage);
      set_red_value(filtered_current);
      break;

    case st_voltage:
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

    case st_current:
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

/***************************  on_key_press  **************************/

void on_key_press(key_t key)
{
  switch (charger_state)
    {

    case st_idle:
      switch (key)
        {

        case k_mode:
          charger_state = st_voltage;
          break;

        case k_start:
          charger_state = st_charge;
          charge_process_stage = cs_testing;
          charger_timer = 0;
          set_pwm_value(0);
          break;

        default:
          break;

        }
      break;

    case st_voltage:
      switch (key)
        {

        case k_mode:
          charger_state = st_current;
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

    case st_current:
      switch (key)
        {

        case k_mode:
          charger_state = st_idle;
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

    case st_charge:
      //
      break;

    default:
      break;

    }
}

/**************************  update_charger  *************************/

void update_charger()
{
  switch (charge_process_stage)
    {

    case cs_off:
      charger_state = st_idle;
      break;

    case cs_testing:
      break;

    }
}

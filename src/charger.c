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

void InitCharger()
{
  charger_current = DEFAULT_CHARGER_CURRENT;
  charger_voltage = DEFAULT_CHARGER_VOLTAGE;

  measured_current = 0;
  measured_voltage = 0;

  charger_state = stTest;

  charger_timer = 0;

  InitSysTick();
  InitDisplay();
  InitKeyboard(OnKeyPress);
  InitADC();
  InitPWM();

  n = 1;
}

/*****************************  OnIdle  ****************************/

void OnIdle()
{
  int adc_reference;

  DisplayIterate();
  msleep(DEFAULT_CHARGER_CYCLE_MS);
  charger_timer += DEFAULT_CHARGER_CYCLE_MS;
  ProcessKeyboard();

  switch (charger_state)
    {

    case stTest:
      SetPWMValue(0);
      if (charger_timer > TEST_TIMEOUT_MS)
        {
          charger_state = stIdle;
          SetTestState(false);
          SetGreenValue(charger_voltage);
          SetRedValue(charger_current);
          charger_timer = 0;
        }
      break;

    case stIdle:
      SetPWMValue(0);
      if (charger_timer > VALUES_UPDATE_TIMEOUT_MS)
        {
          SetGreenValue(charger_voltage);
          SetRedValue(charger_current);
          charger_timer = 0;
        }
      break;

    case stCharge:
      // UpdateCharge();
      adc_reference = ConvertReference();

      measured_current = ConvertChannel(ADC_CHANNEL_ISEN);
      measured_current = measured_current*adc_reference/4096;

      measured_voltage = ConvertChannel(ADC_CHANNEL_VSENP);
      measured_voltage -= ConvertChannel(ADC_CHANNEL_VSENN);
      measured_voltage = measured_voltage*adc_reference/4096;

      // TODO : Delete this -->
      SetPWMValue(n*10);
      // <--
      // SetGreenValue(measured_voltage);
      SetRedValue(measured_current);
      break;

    case stVoltage:
      SetPWMValue(0);
      if (charger_timer > VALUES_BLINK_TIMEOUT_MS)
        {
          charger_timer = 0;
        }
      if (charger_timer > VALUES_BLINK_TIMEOUT_MS/2)
        ClearGreenValue();
      else
        SetGreenValue(charger_voltage);
      break;

    case stCurrent:
      SetPWMValue(0);
      if (charger_timer > VALUES_BLINK_TIMEOUT_MS)
        {
          charger_timer = 0;
        }
      if (charger_timer > VALUES_BLINK_TIMEOUT_MS/2)
        ClearRedValue();
      else
        SetRedValue(charger_current);
      break;

    default:
      break;

    }
}

/***************************  OnKeyPress  **************************/

void OnKeyPress(key_t key)
{
  switch (charger_state)
    {

    case stIdle:
      switch (key)
        {

        case kMode:
          charger_state = stVoltage;
          break;

        case kStart:
          // TODO : StartCharge();
          charger_state = stCharge;
          SetPWMValue(0);
          break;

        default:
          break;

        }
      break;

    case stVoltage:
      switch (key)
        {

        case kMode:
          charger_state = stCurrent;
          break;

        case kUp:
          charger_voltage += 100;
          if (charger_voltage > VOLTAGE_LIMIT)
            charger_voltage = VOLTAGE_LIMIT;
          break;

        case kDown:
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

        case kMode:
          charger_state = stIdle;
          break;

        case kUp:
          charger_current += 10;
          if (charger_current > CURRENT_LIMIT)
            charger_current = CURRENT_LIMIT;
          break;

        case kDown:
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

          case kUp:
            n++;
            if (n > 100)
              n = 100;
            SetGreenValue(n*10);
            break;

          case kDown:
            n--;
            if (n < 0)
              n = 0;
            SetGreenValue(n*10);
            break;

        }
      break;

    default:
      break;

    }
}

#include "charger.h"
#include "display.h"

#include "hal/systick.h"

#define CURRENT_LIMIT             0.5f
#define VOLTAGE_LIMIT             21.0f
#define CURRENT_TRESHOLD_LIMIT    0.1f

#define DEFAULT_CHARGER_VOLTAGE   21.0f
#define DEFAULT_CHARGER_CURRENT   0.5f

#define DEFAULT_CHARGER_CYCLE_MS  2

#define TEST_TIMEOUT_MS           2000
#define VALUES_UPDATE_TIMEOUT_MS  500

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

float charger_current;                            //!< The maximum current on second stage
float charger_voltage;                            //!< The maximum voltage at the end of charging

uint32_t charger_timer;                           //!< Global timer for charger

//*************************  InitCharger  *************************

void InitCharger()
{
  charger_current = DEFAULT_CHARGER_CURRENT;
  charger_voltage = DEFAULT_CHARGER_VOLTAGE;

  charger_state = stTest;

  charger_timer = 0;

  InitSysTick();
  InitDisplay();
}

//****************************  OnIdle  ***************************

void OnIdle()
{
  DisplayIterate();
  msleep(DEFAULT_CHARGER_CYCLE_MS);
  charger_timer += DEFAULT_CHARGER_CYCLE_MS;

  switch (charger_state)
    {

    case stTest:
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
      if (charger_timer > VALUES_UPDATE_TIMEOUT_MS)
        {
          SetGreenValue(charger_voltage);
          SetRedValue(charger_current);
          charger_timer = 0;
        }
      break;

    default:
      break;

    }
}

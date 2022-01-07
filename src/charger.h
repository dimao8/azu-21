/**
 * \file charger.h
 *
 * There is 5 states in the charger: <em>test state</em>, <em>idle state</em>,
 * <em>charge state</em>, <em>voltage set state</em>, <em>current set state</em>.
 *
 * In <em>test state</em> charger just light up all indicators for 2 seconds and go
 * to the <em>idle state</em>.
 *
 */

#ifndef CHARGER_H
#define CHARGER_H

#include "keyboard.h"

///
/// \brief InitCharger
///
void InitCharger();

///
/// \brief OnIdle
///
void OnIdle();

/**
 * \param [in] key -- Pressed key
 *
 * Keyboard callback
 */
void OnKeyPress(key_t key);

#endif // CHARGER_H

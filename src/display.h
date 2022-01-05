#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>

/**
 * Init display module.
 */
void InitDisplay();

/**
 * Set display state into test. All segments will be lighted.
 *
 * \param [in] state -- Set to \c true to turn on test state
 */
void SetTestState(bool state);

/**
 * \param [in] value -- Value to show on green display
 *
 * Set value to display on green display. \b value can not be greater
 * than 999.0 and lesser than 0.01
 */
void SetGreenValue(float value);

/**
 * \param [in] value -- Value to show on red display
 *
 * Set value to display on red display. \b value can not be greater
 * than 999.0 and lesser than 0.01
 */
void SetRedValue(float value);

/**
 * One pass for display. Call it repeatedly for dynamic display to work.
 * Whole display update complete with 6 iterations.
 */
void DisplayIterate();

#endif // DISPLAY_H

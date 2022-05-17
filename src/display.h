#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Init display module.
 */
void init_display();

/**
 * Set display state into test. All segments will be lighted.
 *
 * \param [in] state -- Set to \c true to turn on test state
 */
void set_test_state(bool state);

/**
 * \param [in] value -- Value to show on green display
 *
 * Set value to display on green display. \b value can not be greater
 * than 999.0 and lesser than 0.01
 */
void set_green_value(uint32_t value);

void clear_green_value();

/**
 * \param [in] value -- Value to show on red display
 *
 * Set value to display on red display. \b value can not be greater
 * than 999.0 and lesser than 0.01
 */
void set_red_value(uint32_t value);

void clear_red_value();

/**
 * One pass for display. Call it repeatedly for dynamic display to work.
 * Whole display update complete with 6 iterations.
 */
void display_iterate();

#endif // DISPLAY_H

/**
 * \file adc.h
 * \author Khruschev Dmitry aka DimaO <dimao8@yandex.ru>
 * \date 2022
 * \version 0.1.0.0
 * \brief ADC module
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ADC_H
#define ADC_H

#define ADC_MAX_CHANNEL     17
#define ADC_CHANNEL_ISEN    0
#define ADC_CHANNEL_VSENP   1
#define ADC_CHANNEL_VSENN   2
#define ADC_CHANNEL_TSEN1   3
#define ADC_CHANNEL_TSEN2   4

/**
 * Init ADC module.
 */
void init_adc();

/**
 * Convert single channel.
 *
 * \param [in] channel -- Channel to convert. It must be lesser than ADC_MAX_CHANNEL
 * \return Return result of ADC conversion. Negative value means error.
 */
int convert_channel(unsigned char channel);

/**
 * Convert reference voltage source.
 *
 * \return Return result of ADC conversion in millivolts. Negative value means error.
 */
int convet_reference();

#endif // ADC_H


#include "adc.h"

#include <stm32f0xx.h>
#include <stdint.h>

#define GPIO_MODE_INPUT     0
#define GPIO_MODE_OUTPUT    1
#define GPIO_MODE_ALTERNATE 2
#define GPIO_MODE_ANALOG    3

#define GPIO_SPEED_SLOW     0
#define GPIO_SPEED_MEDIUM   2
#define GPIO_SPEED_HIGH     3

#define ADC_ISEN_PIN_Pos    0
#define ADC_ISEN_PIN_Msk    (1 << ADC_ISEN_PIN_Pos)

#define ADC_VSENP_PIN_Pos   1
#define ADC_VSENP_PIN_Msk   (1 << ADC_VSENP_PIN_Pos)

#define ADC_VSENN_PIN_Pos   2
#define ADC_VSENN_PIN_Msk   (1 << ADC_VSENN_PIN_Pos)

#define ADC_TSEN1_PIN_Pos   3
#define ADC_TSEN1_PIN_Msk   (1 << ADC_TSEN1_PIN_Pos)

#define ADC_TSEN2_PIN_Pos   4
#define ADC_TSEN2_PIN_Msk   (1 << ADC_TSEN2_PIN_Pos)

#define VREF_ADDRESL        0x1FFFF7BA
#define VREF_ADDRESH        0x1FFFF7BB

int get_reference_calibration_value()
{
  int result = (int)(*(uint8_t*)(VREF_ADDRESL));

  result |= (int)(*(uint8_t*)(VREF_ADDRESH)) << 8;

  return result;
}

/***************  InitADC  ***************/

void init_adc()
{
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  ADC1->CR = ADC_CR_ADEN;
  ADC1->CFGR1 = ADC_CFGR1_DISCEN;
  ADC1->CFGR2 = (3 << ADC_CFGR2_CKMODE_Pos);

  ADC1->SMPR = (7 << ADC_SMPR_SMP_Pos);
  ADC1_COMMON->CCR = ADC_CCR_VREFEN;

  GPIOA->MODER |= (GPIO_MODE_ANALOG << ADC_ISEN_PIN_Pos)
    | (GPIO_MODE_ANALOG << ADC_VSENP_PIN_Pos)
    | (GPIO_MODE_ANALOG << ADC_VSENN_PIN_Msk)
    | (GPIO_MODE_ANALOG << ADC_TSEN1_PIN_Msk)
    | (GPIO_MODE_ANALOG << ADC_TSEN2_PIN_Msk);

}

/************  convert_channel  ***********/

int convert_channel(unsigned char channel)
{
  if (channel > ADC_MAX_CHANNEL)
    return -1;

  ADC1->CHSELR = (1 << channel);
  ADC1->CR |= ADC_CR_ADSTART;
  while (!(ADC1->ISR & ADC_ISR_EOC)) { asm("nop"); }
  ADC1->ISR = ADC_ISR_EOC;
  return ADC1->DR & ADC_DR_DATA_Msk;
}

/***********  convert_reference  **********/

int convert_reference()
{
  int primary_result;

  ADC1->CHSELR = (1 << 17);

  ADC1->CR |= ADC_CR_ADSTART;
  while (!(ADC1->ISR & ADC_ISR_EOC)) { asm("nop"); }
  ADC1->ISR = ADC_ISR_EOC;

  primary_result = ADC1->DR & ADC_DR_DATA_Msk;

  return 3300*get_reference_calibration_value()/primary_result;
}

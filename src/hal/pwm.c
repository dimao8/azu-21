#include "pwm.h"

#include <stm32f0xx.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif // HAVE_CONFIG_H

#define GPIO_MODE_INPUT     0
#define GPIO_MODE_OUTPUT    1
#define GPIO_MODE_ALTERNATE 2
#define GPIO_MODE_ANALOG    3

#define GPIO_SPEED_SLOW     0
#define GPIO_SPEED_MEDIUM   2
#define GPIO_SPEED_HIGH     3

#define PWM_PIN_Pos             5
#define PWM_PIN_Msk             (1 << PWM_PIN_Pos)

#define DEFAULT_PWM_RESOLUTION  600
#define DEFAULT_PWM_FREQUENCY   6666

#define MAX_PWM_VALUE           100

/*****************  InitPWM  *****************/

void InitPWM()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  GPIOA->MODER |= (GPIO_MODE_OUTPUT << 2*PWM_PIN_Pos);
  GPIOA->OSPEEDR |= (GPIO_SPEED_HIGH << 2*PWM_PIN_Pos);
  GPIOA->ODR &= ~PWM_PIN_Msk;

  TIM3->PSC = FREQUENCY/(DEFAULT_PWM_RESOLUTION*DEFAULT_PWM_FREQUENCY) - 1;
  TIM3->ARR = DEFAULT_PWM_RESOLUTION;
  TIM3->EGR = TIM_EGR_CC1G;
  TIM3->DIER = TIM_DIER_CC1IE | TIM_DIER_UIE;
}

/***************  SetPWMValue  ***************/

void SetPWMValue(unsigned short value)
{
  if (value > MAX_PWM_VALUE)
    TIM3->CCR1 = MAX_PWM_VALUE;
  else
    TIM3->CCR1 = value;

  if (value == 0)
    EnablePWM(false);
  else
    EnablePWM(true);
}

/****************  EnablePWM  ****************/

void EnablePWM(bool ena)
{
  if (ena)
    {
      NVIC_EnableIRQ(TIM3_IRQn);
      TIM3->CR1 |= TIM_CR1_CEN;
      GPIOA->ODR &= ~PWM_PIN_Msk;
    }
  else
    {
      NVIC_DisableIRQ(TIM3_IRQn);
      TIM3->CR1 &= ~TIM_CR1_CEN;
      GPIOA->ODR &= ~PWM_PIN_Msk;
    }
}

/*************  TIM3_IRQHandler  *************/

void TIM3_IRQHandler()
{
  if (TIM3->SR & TIM_SR_UIF)
    {
      GPIOA->ODR |= PWM_PIN_Msk;
    }
  else if (TIM3->SR & TIM_SR_CC1IF)
    {
      GPIOA->ODR &= ~PWM_PIN_Msk;
    }
  TIM3->SR = 0;
}

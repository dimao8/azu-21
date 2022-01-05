// **********************************************************
/// \file startup_ARMCM0.c
/// \brief startup file for STM32F0x MCUs.
///        Should be used with GCC 'GNU Tools ARM Embedded'
///
/// \date 18 June 2021
// **********************************************************

#include <stdint.h>

#include <stm32f0xx.h>

//----------------------------------------------------------------------------
//  Linker generated Symbols
//----------------------------------------------------------------------------

extern uint32_t _end_stack;                                     ///< Cap of the stack
extern uint32_t _end_text;                                      ///< End of the instructions
extern uint32_t _start_data;                                    ///< Start of the RO data
extern uint32_t _end_data;                                      ///< End of the RO data
extern uint32_t _start_bss;                                     ///< Start of BSS region
extern uint32_t _end_bss;                                       ///< End of BSS region

extern int main(void);

//----------------------------------------------------------------------------
//  Exception / Interrupt Handler Function Prototype
//----------------------------------------------------------------------------
typedef void( *pFunc )( void );


//----------------------------------------------------------------------------
//  External References
//----------------------------------------------------------------------------

#ifndef __NO_SYSTEM_INIT
extern void SystemInit (void);            // CMSIS System Initialization
#endif


//----------------------------------------------------------------------------
//  Internal References
//----------------------------------------------------------------------------
void Default_Handler(void);                          // Default empty handler
void Reset_Handler(void);                            // Reset Handler


//----------------------------------------------------------------------------
// User Initial Stack & Heap
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
//  Exception / Interrupt Handler
//----------------------------------------------------------------------------
// Cortex-M0 Processor Exceptions
void NMI_Handler                      (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler                      (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler                  (void) __attribute__ ((weak, alias("Default_Handler")));

// STM32F0x Specific Interrupts
void WWDG_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler                   (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI0_1_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI2_3_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void EXTI4_15_IRQHandler              (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel2_3_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel4_5_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC1_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_UP_TRG_COM_IRQHandler   (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler               (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM3_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM14_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM16_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void TIM17_IRQHandler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler                  (void) __attribute__ ((weak, alias("Default_Handler")));
void USART1_IRQHandler                (void) __attribute__ ((weak, alias("Default_Handler")));


//----------------------------------------------------------------------------
//  Exception / Interrupt Vector table
//----------------------------------------------------------------------------
const pFunc __Vectors[] __attribute__ ((section(".vectors"))) = {
  // Cortex-M0 Exceptions Handler
  (pFunc)&_end_stack,                       //      Initial Stack Pointer
  Reset_Handler,                            //      Reset Handler
  NMI_Handler,                              //      NMI Handler
  HardFault_Handler,                        //      Hard Fault Handler
  0,                                        //      Reserved
  0,                                        //      Reserved
  0,                                        //      Reserved
  0,                                        //      Reserved
  0,                                        //      Reserved
  0,                                        //      Reserved
  0,                                        //      Reserved
  SVC_Handler,                              //      SVCall Handler
  0,                                        //      Reserved
  0,                                        //      Reserved
  PendSV_Handler,                           //      PendSV Handler
  SysTick_Handler,                          //      SysTick Handler

  // External interrupts
  WWDG_IRQHandler,                          //  0:  Watchdog Timer
  0,                                        //  1:  Reserved
  RTC_IRQHandler,                           //  2:  RTC
	FLASH_IRQHandler,                         //  3:  Flash Controller
  RCC_IRQHandler,                           //  4:  Clock and Reset
  EXTI0_1_IRQHandler,                       //  5:  External Interrupt 0 and 1
  EXTI2_3_IRQHandler,                       //  6:  External Interrupt 2 and 3
	EXTI4_15_IRQHandler,                      //  7:  External Interrupt 4 through 15
	0,                                        //  8:  Reserved
  DMA1_Channel1_IRQHandler,                 //  9:  DMA Channel 1
	DMA1_Channel2_3_IRQHandler,               // 10:  DMA Channels 2 and 3
	DMA1_Channel4_5_IRQHandler,               // 11:  DMA Channels 4 and 5
  ADC1_IRQHandler,                          // 12:  ADC 1
	TIM1_BRK_UP_TRG_COM_IRQHandler,           // 13:  Timer 1 Break, Trig
  TIM1_CC_IRQHandler,                       // 14:  Timer 1 Compare and capture
  0,                                        // 15:  Reserved
  TIM3_IRQHandler,                          // 16:  Timer 3
  0,                                        // 17:  Reserved
  0,                                        // 18:  Reserved
  TIM14_IRQHandler,                         // 19:  Timer 14
  0,                                        // 20:  Reserved
  TIM16_IRQHandler,                         // 21:  Timer 16
  TIM17_IRQHandler,                         // 22:  Timer 17
  I2C1_IRQHandler,                          // 23:  I2C 1
	0,                                        // 24:  Reserved
  SPI1_IRQHandler,                          // 25:  SPI 1/
  0,                                        // 26:  Reserved
  USART1_IRQHandler,                        // 27:  USART 1
  0,                                        // 28:  Reserved
	0,                                        // 29:  Reserved
	0,                                        // 30:  Reserved
	0                                         // 31:  Reserved
};


//----------------------------------------------------------------------------
//  Reset Handler called on controller reset
//----------------------------------------------------------------------------
void Reset_Handler(void)
{
  uint8_t *src,*dst;

// Copy data from flash to ram
	src = (uint8_t*)(&_end_text);
	dst = (uint8_t*)(&_start_data);
	while (dst < (uint8_t*)(&_end_data))
		*dst++ = *src++;

// Clear bss
	dst = (uint8_t*)(&_start_bss);
	while (dst < (uint8_t*)(&_end_bss))
		*dst++ = 0;

#ifndef __NO_SYSTEM_INIT
	SystemInit();
#endif

	main();

}


//----------------------------------------------------------------------------
//  Default Handler for Exceptions / Interrupts
//----------------------------------------------------------------------------
void Default_Handler(void) {

	while(1);
}

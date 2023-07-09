#include "stm32f4xx.h"                  // Device header
#include "stdint.h"
#include "Wave.h"
#include "Input.h"
#include "UsartUnit.h"


void pwm_clock_init(void){
	

	// enable clock for power interface: we want to use pll
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR &= ~PWR_CR_VOS_Msk;
	PWR->CR |= PWR_CR_VOS_1; // scale mode 2 : more power efficiency since we are using the power constantly
	
	
	// set the latency of flash memory to 2 wait states to enusre relialbity
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
	
	// HSI CONFIGURATION
	RCC->CR |= RCC_CR_HSION;
	while( !(RCC->CR & RCC_CR_HSIRDY) ){}
	
	
	// PLL CONFIGURATION
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC; // sets the PLL source to HSI by clearing the PLLSRC bit in the PLLCFGR.
	
		// setting the division/multiplication factors
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;
	RCC->PLLCFGR |= 16 << RCC_PLLCFGR_PLLM_Pos;
	
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
	RCC->PLLCFGR |= 336 << RCC_PLLCFGR_PLLN_Pos;
	
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0; // div4
	
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLQ_Msk;
	RCC->PLLCFGR |= 4 << RCC_PLLCFGR_PLLQ_Pos;
	
	RCC->CR |= RCC_CR_PLLON; // enable the PLL by setting the PLLON bit in the CR 
	while( !(RCC->CR & RCC_CR_PLLRDY) ){} // wait for the PLL to stabilize by checking the PLLRDY bit in the CR register.

		
		
	// set the latency of flash memory again
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
	
		
		// sets bus prescalars	
		
	RCC->CFGR &= ~RCC_CFGR_HPRE; // AHB / 1
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	
	RCC->CFGR &= ~RCC_CFGR_PPRE1_Msk; // APB1 / 2
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	
	RCC->CFGR &= ~RCC_CFGR_PPRE2_Msk; // APB2 / 1
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

		
	RCC->CFGR &= ~RCC_CFGR_SW_Msk;// set PLL as clock src
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while( !(RCC->CFGR & RCC_CFGR_SWS_PLL) ){} // wait in a loop until the SWS bits in the CFGR register indicate that the PLL is the system clock source

	SystemCoreClockUpdate(); // update the system core clock variable `SystemCoreClock `
}


void pwm_init(void){
	// TIM5 CH3 PA2

	GPIOA->MODER &= ~GPIO_MODER_MODE2_Msk;
	GPIOA->MODER |= GPIO_MODER_MODE2_1; // sets the mode of PA2 to alternate function mode.
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL2_1; // set the exact functionalty of PA2: timer
	
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; // enable the clock
	TIM5->PSC = 1024-1; // prescaler: should statrt from 0. since we have only 8 bits.
	TIM5->ARR = RES-1; // set the step counts to 4096
	TIM5->CNT = 0; // counter
	TIM5->CCER |= TIM_CCER_CC3E; // enable the output
	TIM5->CCMR2 |= 6 << TIM_CCMR2_OC3M_Pos; // set the pwm setup to edge-aligned
	TIM5->CCR3 = 512; // set the duty cycle (percentage of the ARR value)
}


void tim2_init(void) {

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->CR1 |= TIM_CR1_CKD_1;
	TIM2->PSC = 1024-1;
	TIM2->ARR = RES - 1;
	TIM2->CNT = 0;
	
	
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn);
	
	
}



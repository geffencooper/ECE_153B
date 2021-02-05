#include "UART.h"

void UART1_Init(void) {
	// [TODO]
}

void UART2_Init(void) 
{
	// Set up the clock for USART2
	
	// enable USART2 clock in peripheral clock register (APB1, P.243)
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

	// set system clock as USART2 clock source (P.257) 
	RCC->CCIPR &= ~RCC_CCIPR_USART2SEL; // reset
	RCC->CCIPR |= RCC_CCIPR_USART2SEL_0; // system clock --> (01)
}

void UART1_GPIO_Init(void) {
	// [TODO]
}

void UART2_GPIO_Init(void) 
{
	// configure the GPIOs for TX(PD5) and RX(PD6) 
	
	// enable the GPIO Port D clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
	
	// set modes to alternative functions
	GPIOD->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6); // reset
	GPIOD->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1); // 10 = alternative function
	
	// configure PD5 to UART2_TX and PD6 to USART2_RX alternative functions (AF7)
	GPIOD->AFR[0] &= ~(GPIO_AFRL_AFSEL5| GPIO_AFRL_AFSEL6); // reset
	
	// AF7 --> 0111, set first three bits for both pins
	GPIOD->AFR[0]  |= (GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_1 | GPIO_AFRL_AFSEL5_2 | \
										 GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_2);
	
	// configure speed to very high
	GPIOD->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6); // reset
	GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6); // 11 = very high speed

	// configure modes to push-pull output
	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6); // reset state = push-pull
	
	// configure to use pull up resistors
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6); // reset
	GPIOD->PUPDR |= (GPIO_PUPDR_PUPD5_0 | GPIO_PUPDR_PUPD6_0); // 01 = pull up
}

void USART_Init(USART_TypeDef* USARTx) 
{
	// disable USART to modify settings
	USARTx->CR1 &= ~USART_CR1_UE; // clear to disable
	
	// set the word length to 8 bits
	USARTx->CR1 &= ~(USART_CR1_M0 | USART_CR1_M1); // 00 = 8 bit

	// set the stop bits to 1
	USARTx->CR2 &= ~USART_CR2_STOP; // 00 = 1 stop bit 
	
	// set oversampling mode to oversample by 16
	USARTx->CR1 &= ~USART_CR1_OVER8; // 0 = oversample by 16
	
	/* 
	   Set the baudrade to 9600
	   -for oversampling by 16, baud = f_clk/USARTDIV
		 -the value of USARTDIV is written to BRR
		 -USARTDIV = 80MHz/9600 --> ~8333
	*/
	USARTx->BRR &= ~0xFFFF; // clear the first 16 bits
	USARTx->BRR |= 8333; // set BRR to USARTDIV
	
	// enable TX and RX
	USARTx->CR1 |= (USART_CR1_RE | USART_CR1_TE);
	
	// enable USART
	USARTx->CR1 |= USART_CR1_UE;
}

uint8_t USART_Read (USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}

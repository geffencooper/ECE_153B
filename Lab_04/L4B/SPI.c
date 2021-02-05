#include "SPI.h"
#include "SysTimer.h"

// Note: When the data frame size is 8 bit, "SPIx->DR = byte_data;" works incorrectly. 
// It mistakenly send two bytes out because SPIx->DR has 16 bits. To solve the program,
// we should use "*((volatile uint8_t*)&SPIx->DR) = byte_data";

// LSM303C eCompass (a 3D accelerometer and 3D magnetometer module) SPI Interface: 
//   MAG_DRDY = PC2   MEMS_SCK  = PD1 (SPI2_SCK)   XL_CS  = PE0             
//   MAG_CS   = PC0   MEMS_MOSI = PD4 (SPI2_MOSI)  XL_INT = PE1       
//   MAG_INT  = PC1 
//
// L3GD20 Gyro (three-axis digital output) SPI Interface: 
//   MEMS_SCK  = PD1 (SPI2_SCK)    GYRO_CS   = PD7 (GPIO)
//   MEMS_MOSI = PD4 (SPI2_MOSI)   GYRO_INT1 = PD2
//   MEMS_MISO = PD3 (SPI2_MISO)   GYRO_INT2 = PB8

extern uint8_t Rx1_Counter;
extern uint8_t Rx2_Counter;

void SPI2_GPIO_Init(void) 
{
	// Enable GPIO Port D Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
	
	// set mode to alternative function
	GPIOD->MODER &= ~(GPIO_MODER_MODE1 | GPIO_MODER_MODE3 | GPIO_MODER_MODE4); // reset
	GPIOD->MODER |= (GPIO_MODER_MODE1_1 | GPIO_MODER_MODE3_1 | GPIO_MODER_MODE4_1); // set to 10 = alternative function

	// configure the alternative function for GPIO PD1, PD3, PD4 (AF5)
	GPIOD->AFR[0] &= ~(GPIO_AFRL_AFSEL1 | GPIO_AFRL_AFSEL3 | GPIO_AFRL_AFSEL4);  // reset selection
	GPIOD->AFR[0] |= (GPIO_AFRL_AFSEL1_2 | GPIO_AFRL_AFSEL1_0 | \
										GPIO_AFRL_AFSEL3_2 | GPIO_AFRL_AFSEL3_0 | \
										GPIO_AFRL_AFSEL4_2 | GPIO_AFRL_AFSEL4_0); // set bit 2,0 to select AF5 --> 0101 
	
	// configure modes to push-pull output
	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT1 | GPIO_OTYPER_OT3 | GPIO_OTYPER_OT4); // reset state = push-pull
	
	// set output speed to very high
	GPIOD->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED1 | GPIO_OSPEEDR_OSPEED3 | GPIO_OSPEEDR_OSPEED4); // reset
	GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED1 | GPIO_OSPEEDR_OSPEED3 | GPIO_OSPEEDR_OSPEED4); // set to 11 = very high speed
	
	// set PUPD to no pull up no pull down
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPDR1 | GPIO_PUPDR_PUPDR3 | GPIO_PUPDR_PUPDR4); // reset, no pull up, pull down = 00
}

void SPI_Init(void)
{
	// enable SPI2 clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;
	
	// reset SPI2 then clear
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_SPI2RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_SPI2RST;
	
	// disable SPI
	SPI2->CR1 &= ~SPI_CR1_SPE;
	
	// configure channel for full duplex communication
	SPI2->CR1 &= ~SPI_CR1_RXONLY;
	
	// configure channel for 2-line unidirectional mode, disable output in bidirectional mode
	SPI2->CR1 &= ~(SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE);

	// set MSB first for TX
	SPI2->CR1 &= ~SPI_CR1_LSBFIRST;
	
	// data length to 8 bits
	SPI2->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);

	// frame format to Motorola
	SPI2->CR2 &= ~SPI_CR2_FRF;
	
	// clock polarity to 0, first clock transition is first data capture edge
	SPI2->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);

	// baud rate prescaler to 16
	SPI2->CR1 |= (SPI_CR1_BR_0 | SPI_CR1_BR_1); // 011 --> PSC 16

	// disable CRC calculation
	SPI2->CR1 &= ~SPI_CR1_CRCEN;

	// set to master configuration, enable software peripheral management
	SPI2->CR1 |= (SPI_CR1_MSTR | SPI_CR1_SSM);
	
	// set NSS pulse management
	SPI2->CR2 |= SPI_CR2_NSSP;
	
	// set the internal peripheral select bit
	SPI2->CR1 |= SPI_CR1_SSI;
	
	// set FIFO reception threshold to 8 bit
	SPI2->CR2 |= SPI_CR2_FRXTH;
	
	// enable SPI
	SPI2->CR1 |= SPI_CR1_SPE;
}
 
void SPI_Write(SPI_TypeDef * SPIx, uint8_t *txBuffer, uint8_t * rxBuffer, int size) {
	volatile uint32_t tmpreg; 
	int i = 0;
	for (i = 0; i < size; i++) {
		while( (SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE );  // Wait for TXE (Transmit buffer empty)
		*((volatile uint8_t*)&SPIx->DR) = txBuffer[i];
		while((SPIx->SR & SPI_SR_RXNE ) != SPI_SR_RXNE); // Wait for RXNE (Receive buffer not empty)
		rxBuffer[i] = *((__IO uint8_t*)&SPIx->DR);
	}
	while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY ); // Wait for BSY flag cleared
}

void SPI_Read(SPI_TypeDef * SPIx, uint8_t *rxBuffer, int size) {
	int i = 0;
	for (i = 0; i < size; i++) {
		while( (SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE ); // Wait for TXE (Transmit buffer empty)
		*((volatile uint8_t*)&SPIx->DR) = rxBuffer[i];	
		// The clock is controlled by master. Thus the master has to send a byte
		// data to the slave to start the clock. 
		while((SPIx->SR & SPI_SR_RXNE ) != SPI_SR_RXNE); 
		rxBuffer[i] = *((__IO uint8_t*)&SPIx->DR);
	}
	while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY ); // Wait for BSY flag cleared
}

void SPI_Delay(uint32_t us) {
	uint32_t i, j;
	for (i = 0; i < us; i++) {
		for (j = 0; j < 18; j++) // This is an experimental value.
			(void)i;
	}
}

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

void SPI2_GPIO_Init(void) {
	// [TODO]
}

void SPI_Init(void){
	// [TODO] 
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

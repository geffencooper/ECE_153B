#include "L3GD20.h"
#include "SPI.h"

// Modifed from l3gd20.c V2.0.0, COPYRIGHT(c) 2015 STMicroelectronics

void GYRO_Init(void) {
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	// Initialize SPI
	SPI2_GPIO_Init();
	SPI_Init();
	
	// Initialize Gyroscope
	GYRO_IO_CS_Init();	
	L3GD20_Init();
}

// Gyroscope IO functions
void GYRO_IO_CS_Init(void)
{
	// Enable GPIO Port D Clock
	//RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
	
	// set mode to output
	GPIOD->MODER &= ~(GPIO_MODER_MODE7);
	GPIOD->MODER |= (GPIO_MODER_MODE7_0); // reset = output

	// configure modes to push-pull output
	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT7); // reset state = push-pull
	
	// set output speed to very high
	GPIOD->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED7); // reset
	GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEED7); // set to 11 = very high speed
	
	// set PUPD to no pull up no pull down
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPDR7); // reset, no pull up, pull down = 00
	
	// Deselect the Gyroscope
	L3GD20_CS_HIGH;
}

void GYRO_IO_Write(uint8_t *pBuffer, uint8_t WriteAddr, uint8_t NumByteToWrite){
	
	uint8_t rxBuffer[32];
	
	/* Configure the MS bit: 
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
	*/
	if(NumByteToWrite > 0x01){
		WriteAddr |= (uint8_t) MULTIPLEBYTE_CMD; // Set write mode & auto-increment
	}
	
	// Set chip select Low at the start of the transmission 
	L3GD20_CS_LOW;  // Gyro CS low
	SPI_Delay(10);  
	
	// Send the Address of the indexed register 
	SPI_Write(SPI2, &WriteAddr, rxBuffer, 1);
	
	// Send the data that will be written into the device (MSB First) 
	SPI_Write(SPI2, pBuffer, rxBuffer, NumByteToWrite);
  
	// Set chip select High at the end of the transmission  
	SPI_Delay(10); // Gyro CS high
	L3GD20_CS_HIGH;
}

void GYRO_IO_Read(uint8_t *pBuffer, uint8_t ReadAddr, uint8_t NumByteToRead){

	uint8_t rxBuffer[32];
	
	uint8_t AddrByte = ReadAddr | ((uint8_t) 0xC0);  // set read mode & autoincrement
	
	// Set chip select Low at the start of the transmission 
	L3GD20_CS_LOW; // Gyro CS low
	SPI_Delay(10);
	
	// Send the Address of the indexed register 
	SPI_Write(SPI2, &AddrByte, rxBuffer, 1);
	
	// Receive the data that will be read from the device (MSB First) 
	SPI_Read(SPI2, pBuffer, NumByteToRead);
  
	// Set chip select High at the end of the transmission  
	SPI_Delay(10); // Gyro CS high
	L3GD20_CS_HIGH;
}	


void L3GD20_Init(void) 
{  
	// try to read the gyro ID as a test
	uint8_t test = 0;
	GYRO_IO_Read(&test, L3GD20_WHO_AM_I_ADDR, 1);
	
	// write to control register 1 of the gyroscope
	// -enable x, y, z axis
	// -set to normal mode
	// -set DR and BW to 95Hz and 25
	
	uint8_t ctrl_reg1_val = 0;
	uint8_t ctrl_reg4_val = 0;
	
	ctrl_reg1_val |= (L3GD20_Z_ENABLE | L3GD20_Y_ENABLE | L3GD20_X_ENABLE | \
										L3GD20_MODE_ACTIVE | \
										L3GD20_BANDWIDTH_2 | \
										L3GD20_OUTPUT_DATARATE_1);

	// initialize the gyro parameters
	GYRO_IO_Write(&ctrl_reg1_val, L3GD20_CTRL_REG1_ADDR, 1);
	
	// read the register written to as a test
	//test = 0;
	//GYRO_IO_Read(&test, L3GD20_CTRL_REG1_ADDR, 1);
	
	// set 2000 dps in control register 4
	ctrl_reg4_val |= L3GD20_FULLSCALE_2000;
	GYRO_IO_Write(&ctrl_reg4_val, L3GD20_CTRL_REG4_ADDR, 1);
	
	//test = 0;
	//GYRO_IO_Read(&test, L3GD20_CTRL_REG4_ADDR, 1);
}

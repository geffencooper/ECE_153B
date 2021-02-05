/*
 * ECE 153B - Winter 2021
 *
 * Name(s): Geffen Cooper
 * Section: Wednesday 7:00 - 9:50 PM
 * Lab: 2C
 */
 
#include "RTC.h"

// Use the 32.768 kHz low-speed external clock as RTC clock source

// Helper macro to convert a value from 2 digit decimal format to BCD format
// __VALUE__ Byte to be converted
#define __RTC_CONVERT_BIN2BCD(__VALUE__) (uint8_t)((((__VALUE__) / 10) << 4) | ((__VALUE__) % 10))
#define __RTC_CONVERT_BCD2BIN(__VALUE__) (uint8_t)(((uint8_t)((__VALUE__) & (uint8_t)0xF0) >> (uint8_t)0x4) * 10 + ((__VALUE__) & (uint8_t)0x0F))

#define RTC_WEEKDAY_MONDAY              ((uint32_t)0x01) /*!< Monday    */
#define RTC_WEEKDAY_TUESDAY             ((uint32_t)0x02) /*!< Tuesday   */
#define RTC_WEEKDAY_WEDNESDAY           ((uint32_t)0x03) /*!< Wednesday */
#define RTC_WEEKDAY_THURSDAY            ((uint32_t)0x04) /*!< Thrusday  */
#define RTC_WEEKDAY_FRIDAY              ((uint32_t)0x05) /*!< Friday    */
#define RTC_WEEKDAY_SATURDAY            ((uint32_t)0x06) /*!< Saturday  */
#define RTC_WEEKDAY_SUNDAY              ((uint32_t)0x07) /*!< Sunday    */

#define RTC_MONTH_JANUARY               ((uint8_t)0x01)  /*!< January   */
#define RTC_MONTH_FEBRUARY              ((uint8_t)0x02)  /*!< February  */
#define RTC_MONTH_MARCH                 ((uint8_t)0x03)  /*!< March     */
#define RTC_MONTH_APRIL                 ((uint8_t)0x04)  /*!< April     */
#define RTC_MONTH_MAY                   ((uint8_t)0x05)  /*!< May       */
#define RTC_MONTH_JUNE                  ((uint8_t)0x06)  /*!< June      */
#define RTC_MONTH_JULY                  ((uint8_t)0x07)  /*!< July      */
#define RTC_MONTH_AUGUST                ((uint8_t)0x08)  /*!< August    */
#define RTC_MONTH_SEPTEMBER             ((uint8_t)0x09)  /*!< September */
#define RTC_MONTH_OCTOBER               ((uint8_t)0x10)  /*!< October   */
#define RTC_MONTH_NOVEMBER              ((uint8_t)0x11)  /*!< November  */
#define RTC_MONTH_DECEMBER              ((uint8_t)0x12)  /*!< December  */

void RTC_BAK_SetRegister(uint32_t BackupRegister, uint32_t Data) 
{
	register uint32_t tmp = 0;

	tmp = (uint32_t)(&(RTC->BKP0R));
	tmp += (BackupRegister * 4);

	/* Write the specified register */
	*(__IO uint32_t *)tmp = (uint32_t)Data;
}

void RTC_Init(void) 
{
	
	/* Enables the PWR Clock and Enables access to the backup domain #######*/
	/* To change the source clock of the RTC feature (LSE, LSI), you have to:
	 - Enable the power clock
	 - Enable write access to configure the RTC clock source (to be done once after reset).
	 - Reset the Back up Domain
	 - Configure the needed RTC clock source */
	RTC_Clock_Init();
	
	// Disable RTC registers write protection
	RTC_Disable_Write_Protection();
	
	// Enter in initialization mode
	RTC->ISR |= RTC_ISR_INIT;
	while( (RTC->ISR & RTC_ISR_INITF) == 0); // Wait until INITF is set after entering initialization mode
	
	// Configure the Date 
	/* Note: __LL_RTC_CONVERT_BIN2BCD helper macro can be used if user wants to */
	/*       provide directly the decimal value:                                */
	RTC_Set_Calendar_Date(RTC_WEEKDAY_THURSDAY, 27, RTC_MONTH_JANUARY, 21); /* [TODO] These values are stubs - fill in current date */
	
	// Configure the Time 
	RTC_Set_Time(RTC_TR_PM, 7, 40, 58); /* [TODO] These values are stubs - fill in current time */
  
	// Exit of initialization mode 
	RTC->ISR &= ~RTC_ISR_INIT;
	while((RTC->ISR & RTC_ISR_RSF) == 0); /* Wait for synchro */
                                          /* Note: Needed only if Shadow registers is enabled */
                                          /* LL_RTC_IsShadowRegBypassEnabled function can be used */
	
	// Enable RTC registers write protection 
	RTC_Enable_Write_Protection();

	// Writes a data in a RTC Backup data Register1 
	// to indicate date/time updated 
	RTC_BAK_SetRegister(1, 0x32F2);
}

// these give the starting bit position so we can use shifting
#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))
#define RTC_POSITION_TR_HT    (uint32_t)POSITION_VAL(RTC_TR_HT)
#define RTC_POSITION_TR_HU    (uint32_t)POSITION_VAL(RTC_TR_HU)
#define RTC_POSITION_TR_MT    (uint32_t)POSITION_VAL(RTC_TR_MNT)
#define RTC_POSITION_TR_MU    (uint32_t)POSITION_VAL(RTC_TR_MNU)
#define RTC_POSITION_TR_ST    (uint32_t)POSITION_VAL(RTC_TR_ST)
#define RTC_POSITION_TR_SU    (uint32_t)POSITION_VAL(RTC_TR_SU)
#define RTC_POSITION_DR_YT    (uint32_t)POSITION_VAL(RTC_DR_YT)
#define RTC_POSITION_DR_YU    (uint32_t)POSITION_VAL(RTC_DR_YU)
#define RTC_POSITION_DR_MT    (uint32_t)POSITION_VAL(RTC_DR_MT)
#define RTC_POSITION_DR_MU    (uint32_t)POSITION_VAL(RTC_DR_MU)
#define RTC_POSITION_DR_DT    (uint32_t)POSITION_VAL(RTC_DR_DT)
#define RTC_POSITION_DR_DU    (uint32_t)POSITION_VAL(RTC_DR_DU)
#define RTC_POSITION_DR_WDU   (uint32_t)POSITION_VAL(RTC_DR_WDU)

void RTC_Set_Calendar_Date(uint32_t WeekDay, uint32_t Day, uint32_t Month, uint32_t Year) 
{
	// seems that can only write to the register once so need to save reserved bits and overwrite the rest
	uint32_t reserved_bits_mask = (0xFF << 24) | (3 << 5);
	uint32_t reserved_bits = (RTC->DR & reserved_bits_mask);
	
	// store values to an intermediate variable
	uint32_t date = 0; 
	
	// Write the date values in the correct place within the RTC Date Register by shifting using position #defines
	date |= (__RTC_CONVERT_BIN2BCD(WeekDay) << RTC_POSITION_DR_WDU); // place the weekday unit
	date |= (__RTC_CONVERT_BIN2BCD(Day) << RTC_POSITION_DR_DU);      // place the day unit and tens
	date |= (__RTC_CONVERT_BIN2BCD(Month) << RTC_POSITION_DR_MU);    // place the month unit and tens
	date |= (__RTC_CONVERT_BIN2BCD(Year) << RTC_POSITION_DR_YU);     // place the year unit and tens
	
	// overwrite all values in one shot, make sure to retain reserved bits
	RTC->DR = (date | reserved_bits);
}

void RTC_Set_Time(uint32_t Format12_24, uint32_t Hour, uint32_t Minute, uint32_t Second) 
{
	// seems that can only write to the register once so need to save reserved bits and overwrite the rest
	uint32_t reserved_bits_mask = (0x1FF << 23) | (1 << 15) | (1 << 7);
	uint32_t reserved_bits = (RTC->TR & reserved_bits_mask);
	
	// store values to an intermediate variable
	uint32_t time = 0; 
	
	// Write the time values in the correct place within the RTC Time Register by shifting using position #defines
	time |= (Format12_24);   // place format to 12 or 24
	time |= (__RTC_CONVERT_BIN2BCD(Hour) << RTC_POSITION_TR_HU);   // place the hour unit
	time |= (__RTC_CONVERT_BIN2BCD(Minute) << RTC_POSITION_TR_MU); // place the minute unit
	time |= (__RTC_CONVERT_BIN2BCD(Second) << RTC_POSITION_TR_SU); // place the second unit
	
	// overwrite all values in one shot, make sure to retain reserved bits
	RTC->TR = (time | reserved_bits);
}

void RTC_Clock_Init(void) 
{
	
	// Enable write access to Backup domain
	if ((RCC->APB1ENR1 & RCC_APB1ENR1_PWREN) == 0) {
		RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN; // Power interface clock enable
		(void) RCC->APB1ENR1; // Delay after an RCC peripheral clock enabling
	}
	
	// Select LSE as RTC clock source 
	if ( (PWR->CR1 & PWR_CR1_DBP) == 0) {
		PWR->CR1  |= PWR_CR1_DBP;             // Enable write access to Backup domain
		while((PWR->CR1 & PWR_CR1_DBP) == 0); // Wait for Backup domain Write protection disable
	}
	
	// Reset LSEON and LSEBYP bits before configuring the LSE
	RCC->BDCR &= ~(RCC_BDCR_LSEON | RCC_BDCR_LSEBYP);

	// RTC Clock selection can be changed only if the Backup Domain is reset
	RCC->BDCR |=  RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;
	
	// Note from STM32L4 Reference Manual: 	
	// RTC/LCD Clock:  (1) LSE is in the Backup domain. (2) HSE and LSI are not.	
	while((RCC->BDCR & RCC_BDCR_LSERDY) == 0) 
	{ // Wait until LSE clock ready
		RCC->BDCR |= RCC_BDCR_LSEON;
	}
	
	// Select LSE as RTC clock source
	// BDCR = Backup Domain Control Register 
	RCC->BDCR	&= ~RCC_BDCR_RTCSEL;  // RTCSEL[1:0]: 00 = No Clock, 01 = LSE, 10 = LSI, 11 = HSE
	RCC->BDCR	|= RCC_BDCR_RTCSEL_0; // Select LSE as RTC clock	
	
	RCC->BDCR |= RCC_BDCR_RTCEN; // Enable RTC
	
	RCC->APB1ENR1 &= ~RCC_APB1ENR1_PWREN; // Power interface clock disable
}

void RTC_Disable_Write_Protection(void) 
{
	// write 0xCA and 0x53 sequentially to unlock
	RTC->WPR |= 0xCAU;
	RTC->WPR |= 0x53U;
}
	
void RTC_Enable_Write_Protection(void) 
{
	// write an incorrect byte
	RTC->WPR |= 0xFF;
}

uint32_t RTC_TIME_GetHour(void) 
{	        // read the tens value   read the units value     shift back to LSB
	return (((RTC->TR & RTC_TR_HT) + (RTC->TR & RTC_TR_HU)) >> RTC_POSITION_TR_HU);
}

uint32_t RTC_TIME_GetMinute(void) 
{
	        // read the tens value     read the units value      shift back to LSB
	return (((RTC->TR & RTC_TR_MNT) + (RTC->TR & RTC_TR_MNU)) >> RTC_POSITION_TR_MU);
}

uint32_t RTC_TIME_GetSecond(void) 
{
	         // read the tens value     read the units value      shift back to LSB
	return (((RTC->TR & RTC_TR_ST) + (RTC->TR & RTC_TR_SU)) >> RTC_POSITION_TR_SU);
}

uint32_t RTC_DATE_GetMonth(void) 
{
	        // read the tens value     read the units value      shift back to LSB
	return (((RTC->DR & RTC_DR_MT) + (RTC->DR & RTC_DR_MU)) >> RTC_POSITION_DR_MU);
}

uint32_t RTC_DATE_GetDay(void) 
{
	         // read the tens value     read the units value      shift back to LSB
	return (((RTC->DR & RTC_DR_DT) + (RTC->DR & RTC_DR_DU)) >> RTC_POSITION_DR_DU);
}

uint32_t RTC_DATE_GetYear(void) 
{
	        // read the tens value     read the units value      shift back to LSB
	return (((RTC->DR & RTC_DR_YT) + (RTC->DR & RTC_DR_YU)) >> RTC_POSITION_DR_YU);
}

uint32_t RTC_DATE_GetWeekDay(void) 
{
	       // read the weekday unit    shift back to LSB
	return ((RTC->DR & RTC_DR_WDU) >> RTC_POSITION_DR_WDU) ;
}

void Get_RTC_Calendar(char * strTime, char * strDate) 
{
	/* Note: need to convert in decimal value in using __LL_RTC_CONVERT_BCD2BIN helper macro */
	/* Display time Format : hh:mm:ss */
	sprintf((char*)strTime,"%.2d:%.2d:%.2d", 
		__RTC_CONVERT_BCD2BIN(RTC_TIME_GetHour()), 
		__RTC_CONVERT_BCD2BIN(RTC_TIME_GetMinute()), 
		__RTC_CONVERT_BCD2BIN(RTC_TIME_GetSecond()));
	/* Display date Format : mm-dd-yy */
	sprintf((char*)strDate,"%.2d-%.2d-%.2d", 
		__RTC_CONVERT_BCD2BIN(RTC_DATE_GetMonth()), 
		__RTC_CONVERT_BCD2BIN(RTC_DATE_GetDay()), 
		2000 + __RTC_CONVERT_BCD2BIN(RTC_DATE_GetYear()));
}

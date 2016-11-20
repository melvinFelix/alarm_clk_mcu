//**************************************************************//
//Microcontroller			:ATmega16
//System Clock				:2MHz 
//Project					:DS1307 RTC Interfacing with ATmega16 and LCD Display
//Software					:AVR Studio 4 
//LCD Data Interfacing		:8-Bit  
//TWI Frequency				:100KHz 
//Author					:Arun Kumar Garg 
//							:ABLab Solutions
//							:www.ablab.in
//							:info@ablab.in
//Date						:1st January 2012
//**************************************************************//


#include<avr/io.h>
#include<util/delay.h>
#include "ds1307.h"
#include "twi.h"
#include<CLib/LCD.h>

int main(void)
{
	InitLCD();

	unsigned char sec=0,min=0;
	/*Variable declaration*/
	
	
	twi_init();
	/*TWI initialisaiton*/	
	

	ds1307_second_write(0);

	ds1307_minute_write(51);

	ds1307_hour_write(15,0,1);
	
	ds1307_day_write(7);
	
	ds1307_date_write(13);
	
	ds1307_month_write(11);
	
	ds1307_year_write(16);


	/*Start of infinite loop*/
	while(1)
	{
		sec=ds1307_read_second();
		/*Reading second value from DS1307*/

		min=ds1307_read_minute();
		/*Reading minute value from DS1307*/

		min=convert_bcd_to_decimal(min);
		/*Converting BCD number to Decimal value*/
		CursorPos(1,1);
		
		SendInteger(min);
		SendString("  ");
		/*Minute value display in 2nd row of LCD*/

		CursorPos(2,1);

		sec=convert_bcd_to_decimal(sec);
		/*Converting BCD number to Decimal value*/

		SendInteger(sec);
		SendString("  ");
		/*Second value display in 2nd row of LCD*/

		
	}
}
/*End of Program*/

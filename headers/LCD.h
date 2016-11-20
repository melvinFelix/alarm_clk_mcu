//##########################################################################################################################################################
//	                          LIBRARY FOR 1602A 16*2 LCD DISPLAY   Author:- Samiran Patra
//##########################################################################################################################################################

#ifndef LCD
#define LCD

#include<avr/io.h>
#include<stdlib.h>
#include<util/delay.h>

#define DDR_LCD					DDRB			//Set Data direction for LCD
#define LCD_PORT 				PORTB			//Set Port for LCD
#define DDR_Control_PORT		DDRA			//Set Data direction for control of LCD
#define LCD_Control_PORT		PORTA			//Set Port for control of LCD
#define Enable 					3				//Set Pin for Enable Switch
#define RW 						2				//Set Pin for Read/Write Switch
#define RS 						1				//Set Pin for Resistor Switch

//###############################################################################################################################################################
//																End of Define Statements
//############################################################################################################################################################

char ColumnPosition[2] = {0, 64};				//Array to store the position of columns

void CheckBusy(void);							//Function to check if LCD is ready for receiving data
void FlashLCD(void);							//Function to flash the enable switch on and off
void SendCmd(unsigned char cmd);				//Function to send a Command to the LCD
void SendChar(unsigned char character);			//Function to send a Character to the LCD
void SendString(char *str);						//Function to send a String to the LCD
void SendInteger(int Int);						//Function to send Integer to the LCDvoid 
void CursorPos(uint8_t y, uint8_t x);			//Function to POsition Cursor at a particular location
void InitLCD(void);								//Function to initialize the LCD at the start of operation

//############################################################################################################################################################
//																End of Prototyping
//############################################################################################################################################################


void CheckBusy()
{
	DDR_LCD= 0x00;								//Set Data Direction for LCD to Input
	LCD_Control_PORT|= 1<<RW;					//Set LCD to Read Mode; R/w pin High
	LCD_Control_PORT&= ~1<<RS;					//Set LCD to Read Commands; RS pin Low

	while (LCD_PORT >= 0x80)					//Condition Checking; if LCD sends back a value < 80 then it is busy
	{
		FlashLCD();
	}

	DDR_LCD= 0xFF; 								//Set Data Direction for LCD to Output, for further operations
}

void FlashLCD()
{
	LCD_Control_PORT|= 1<<Enable;				//Enable Pin High
	_delay_ms(5);
	LCD_Control_PORT&= ~1<<Enable;				//Enable Pin Low
}

void SendCmd(unsigned char cmd)
{
	CheckBusy();
	LCD_PORT = cmd;
	LCD_Control_PORT&= ~ ((1<<RW)|(1<<RS));	//write mode; command mode
	FlashLCD();
	LCD_PORT = 0x00;
}

void SendChar(unsigned char character)
{
	CheckBusy();
	LCD_PORT = character;
	LCD_Control_PORT&= ~ (1<<RW);
	LCD_Control_PORT|= 1<<RS;
	FlashLCD();
	LCD_PORT = 0x00;
}

void SendString(char *str)
{
	while(*str > 0)
	{
		SendChar(*str++);
	}
}

void SendInteger(int Int)
{
	char ch[16];
	itoa(Int,ch,10);
	SendString(ch);
}


void CursorPos(uint8_t y, uint8_t x)
{
	SendCmd(0x80 + ColumnPosition[y-1] + (x-1));
}

void InitLCD()
{
	DDR_Control_PORT |= 1<<Enable | 1<<RW | 1<<RS;
	_delay_ms(50);

	SendCmd(0x01); 								//Clear Screen 0x01 = 00000001
	_delay_ms(2);
	SendCmd(0x38);								//Selecting 8- bit Mode
	_delay_us(50);
	SendCmd(0b00001110);						//Set attributes on 1st three bits: {Display-ON/OF, Cursor-ON/OFF, Cursor-Blink-ON/OFF}
	_delay_us(50);
}

#endif
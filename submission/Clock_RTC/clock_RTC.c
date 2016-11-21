//
//
//Program for alarm clock with temperature sensor
//
//created by Melvin Felix on 20/11/2016
//

#define numberOfButtons	3							//Defining no of buttons used in the circuit

#include<avr/io.h>
#include<stdlib.h>
#include<CLib/LCD.h>						
#include<avr/interrupt.h>
#include<CLib/ButtonPress.h>				
#include<CLib/DS1307.h>
#include<CLib/TWI.h>

#define swPORT		PINA
#define sw1			6
#define sw2			5
#define sw3			4

//
//
//  Function prototypes used in the programme
//

void WriteData(void);
void ReadData(void);
void Initialize(void);	
void EnterValues(void);			
void CheckMonth(void);
void CheckDay(void);
void CheckYear(void);
void ToggleCollen(void);
void GetTemp(void);
void Menu(void);
void GetValues(void);
void Reset(void);	
void SetAlarm(void);
void GetDate(void);							
void GetDay(void);							
void GetMonth(void);						
void GetTime(void);							
void GetYear(void);			
void ConvertTime(void);		
void CheckAlarm(void);
void Alarm(void);
void ConvertAlarmTime(void);		

//
//
//													Global Variables used in the programme
//
//

unsigned char sec=0,min=0,hour=0,day=0,date=0,month=0,year=0;
double counter_temp=0;
int countAlarm1=0;
int	AHour=0,AHour1=0,AHour2=0;
int	AMinute=0,AMinute1=0,AMinute2=0;
int AlarmStatus=0;
int i=0,j=0,k=0,l=0,m=0,n=0;


int main(void)
{
	DDRA &= ~(1<<sw1);
	DDRA &= ~(1<<sw2);
	DDRA &= ~(1<<sw3);
	DDRA |= 1<<PINA7;
	PORTA |= (1<<sw1);
	PORTA |= (1<<sw2);
	PORTA |= (1<<sw3);
	PORTA |= 1<<PINA7;
	DDRD=0b11111011;
	PORTD |= 1<<PIND2;
	
	ADCSRA |= 1<<ADPS2;			//Setting up resisitors for ADC conversion
	ADCSRA |= 1<<ADPS1;
	ADCSRA |= 1<<ADPS0;
	ADCSRA |= 1<<ADIE;
	ADCSRA |= 1<<ADEN;
	
	GICR |= 1<<INT0;
	
	sei();						//Global Interrupt routine

	InitLCD();
	twi_init();	
	Initialize();
	GetTemp();

	while(1)
	{
		counter_temp++;
		ReadData();
		EnterValues();
		if(counter_temp>360)
		{
			GetTemp();
			counter_temp=0;
		}
		if(AlarmStatus==1)
		{
			ConvertAlarmTime();
			CheckAlarm();
		}
	}
	
}

ISR(ADC_vect)					//Interrupt Service Routine for ADC Conversion
{
	char tempC[5];
	int low= ADCL;
	int tenBitValue= (ADCH<<8) | low;
	double value = tenBitValue*0.0049462*100;
	dtostrf(value, 5, 1, tempC);
	CursorPos(2,10);
	SendString(tempC);
	SendChar(0xDF);
	SendString("C");
}	

void GetTemp(void)
{
	ADCSRA |= 1<<ADSC;			//Start ADC Conversion
	_delay_ms(1);
}

ISR(INT0_vect)					//Interrupt Service Routine for External Interrupt Pin INT0
{	
	if(countAlarm1==0)
	{
		Menu();
	}
	if(countAlarm1>0)
	{
		AMinute++;
	}
}

void Menu(void)
{
	int count=0, check=1, page=1;
	SendCmd(0x01);
	SendCmd(0b00001111);
	CursorPos(1,1);
	SendString("1: Settings");
	CursorPos(2,1);
	SendString("2: Set Alarm   ");
	CursorPos(1,14);
	
	while(count==0)
	{
		if(ButtonPressed(2, swPORT, sw3, 500))
		{
			check--;
			if(check==1)
			CursorPos(1,14);
			if(check<1)
			{
				page=1;
			}
			if(page==1)
			{
				CursorPos(1,1);
				SendString("1: Settings ");
				CursorPos(2,1);
				SendString("2: Set Alarm");
				CursorPos(1,14);
				check=1;
			}
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
		{
			if((check==1)&(page==1))
			{
				GetValues();
				Reset();
				count++;
			}
			if(((check==2)&(page==1))|((check==1)&(page==2)))
			{
				SetAlarm();
				Reset();
				count++;
			}
			if((check==2)&(page==2))
			{
				Reset();
				return;
			}
		}
		else if(ButtonPressed(0, swPORT, sw1, 500))
		{	
			check++;
			if(check==2)
			CursorPos(2,14);
			if(check>2)
			{	check=2;
				page=2;
			}
			if(page==2)
			{
				CursorPos(1,1);
				SendString("2: Set Alarm");
				CursorPos(2,1);
				SendString("3: Exit      ");
				CursorPos(2,14);
			}
		}
	}
}

void Initialize(void)
{
	SendCmd(0b00001100);
	CursorPos(1,1);
	SendString("    HELLO!     ");
	_delay_ms(2000);
	SendCmd(0x01);
	CursorPos(1,1);
	SendString("     HOLA!     ");
	_delay_ms(2000);
	SendCmd(0x01);
	CursorPos(1,1);
	SendString("    SALUT!     ");
	_delay_ms(2000);
	SendCmd(0x01);
	CursorPos(1,1);
	SendString("A Project By :-");
	CursorPos(2,1);
	SendString("Melvin Felix");
	_delay_ms(2000);
	CursorPos(2,1);
	SendString("Lokesh Dasila");
	_delay_ms(2000);
	CursorPos(2,1);
	SendString("     and     ");
	_delay_ms(2000);
	SendCmd(0x01);
	CursorPos(1,1);
	SendString("   Groovy   ");
	CursorPos(2,1);
	SendString("    Guru    ");
	_delay_ms(2000);
	SendCmd(0x01);
}

void EnterValues(void)
{
	CursorPos(1,1);
	SendInteger(date);
	SendChar(0x2f);
	CheckMonth();
	SendChar(0x2f);
	CheckYear();
	CursorPos(1,14);
	CheckDay();
	CursorPos(2,1);
	
	if(hour<10)
	{
		SendChar(0x30);
		SendInteger(hour);
	}
	else
	{
		SendInteger(hour);
	}
	ToggleCollen();
	CursorPos(2,4);
	if(min<10)
	{
		SendChar(0x30);
		SendInteger(min);
	}
	else
	{
		SendInteger(min);
	}
	ToggleCollen();
	CursorPos(2,7);
	if(sec<10)
	{
		SendChar(0x30);
		SendInteger(sec);
	}
	else
	{
		SendInteger(sec);
	}
}

void ToggleCollen(void)
{
	if((sec%2)==0)
	{
		SendChar(0x3a);
		if(AlarmStatus==1)
		{
			CursorPos(1,13);
			SendChar(0x5e);
		}
	}
	else
	{
		counter_temp++;
		SendChar(0x20);
		if(AlarmStatus==1)
		{
			CursorPos(1,13);
			SendChar(0x20);
		}
	}
}
void CheckMonth(void)
{
	switch(month)
	{
		case 1:		SendString("JAN");
					break;
		case 2:		SendString("FEB");
					break;
		case 3:		SendString("MAR");
					break;
		case 4:		SendString("APR");
					break;
		case 5:		SendString("MAY");
					break;
		case 6:		SendString("JUN");
					break;
		case 7:		SendString("JUL");
					break;
		case 8:		SendString("AUG");
					break;
		case 9:		SendString("SEP");
					break;
		case 10:	SendString("OCT");
					break;
		case 11:	SendString("NOV");
					break;
		case 12:	SendString("DEC");
					break;
	}
}

void CheckYear(void)
{
	switch(year)
	{
		case 16:		SendString("2016 ");
					break; 
		case 17:		SendString("2017 ");
					break;
		case 18:		SendString("2018 ");
					break;
		case 19:		SendString("2019 ");
					break;
		case 20:		SendString("2020 ");
					break;
		case 21:		SendString("2021 ");
					break;
	}
}


void CheckDay(void)
{
	switch(day)
	{
		case 1:		SendString("SUN");
					break;
		case 2:		SendString("MON");
					break;
		case 3:		SendString("TUE");
					break;
		case 4:		SendString("WED");
					break;
		case 5:		SendString("THU");
					break;
		case 6:		SendString("FRI");
					break;
		case 7:		SendString("SAT");
					break;
	}
}

void GetValues(void)
{
	GetDate();
	SendCmd(0x01);
	GetMonth();
	SendCmd(0x01);
	GetYear();
	SendCmd(0x01);
	GetDay();
	SendCmd(0x01);
	GetTime();
	SendCmd(0x01);
	SendCmd(0b00001100);
	WriteData();
}

void GetDate(void)
{
	int count=0;
	CursorPos(1,1);
	SendCmd(0x01);
	SendCmd(0b00001111);
	SendString("Enter Date:- ");
	SendInteger(date);
	SendString("  ");
	CursorPos(1,14);
	while(count==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(date==1)	date=1;
			else date--;
			SendInteger(date);
			SendString("  ");
			CursorPos(1,14);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(date==31)	date=31;
			else date++;
			SendInteger(date);
			SendString("  ");
			CursorPos(1,14);
		}
	}
}

void GetDay(void)
{

	int count=0;
	CursorPos(1,1);
	SendCmd(0b00001111);
	SendString("Enter Day:-  ");
	CheckDay();
	CursorPos(1,14);
	while(count==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(day==1)	day=1;
			else day--;
			CheckDay();
			CursorPos(1,14);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(day==7)	day=7;
			else day++;
			CheckDay();
			CursorPos(1,14);
		}
	}		
}

void GetMonth(void)
{
	int count=0;
	CursorPos(1,1);
	SendCmd(0b00001111);
	SendString("Enter Month:-");
	CheckMonth();
	CursorPos(1,14);	
	while(count==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(month==1)	month=1;
			else month--;
			CheckMonth();
			CursorPos(1,14);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(month==12)	month=12;
			else month++;
			CheckMonth();
			CursorPos(1,14);
		}
	}	
}

void GetYear()
{
	int count=0;
	CursorPos(1,1);
	SendCmd(0b00001111);
	SendString("Enter Year:-");
	CheckYear();
	CursorPos(1,13);	
	while(count==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(year==16)	year=16;
			else year--;
			CheckYear();
			CursorPos(1,13);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(year==21)	year=21;
			else year++;
			CheckYear();
			CursorPos(1,13);
		}
	}	
}


void GetTime(void)
{
	CursorPos(1,1);
	SendCmd(0b00001111);
	SendString("Enter Time(24HR)");
	CursorPos(2,1);
	SendString("HH");
	SendChar(0x2f);
	SendString("MM :-");
	CursorPos(2,9);
	SendInteger(i);
	SendInteger(j);
	SendChar(0x2f);
	SendInteger(k);
	SendInteger(l);
	SendChar(0x2f);
	SendInteger(m);
	SendInteger(n);
	CursorPos(2,9);
	
	int count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(i==0)	i=0;
			else i--;
			SendInteger(i);
			CursorPos(2,9);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(i==2)	i=2;
			else i++;
			SendInteger(i);
			CursorPos(2,9);
		}
	}

	CursorPos(2,10);
	count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(j==0)	j=0;
			else j--;
			SendInteger(j);
			CursorPos(2,10);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if((i==2) & (j==3))	j=3;
			else if(j==9)	j=9;
			else j++;
			SendInteger(j);
			CursorPos(2,10);
		}
	}

	CursorPos(2,12);
	count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(k==0)	j=0;
			else k--;
			SendInteger(k);
			CursorPos(2,12);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(k==5)	k=5;
			else k++;
			SendInteger(k);
			CursorPos(2,12);
		}
	}

	CursorPos(2,13);
	count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(l==0)	l=0;
			else l--;
			SendInteger(l);
			CursorPos(2,13);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(l==9)	l=9;
			else l++;
			SendInteger(l);
			CursorPos(2,13);
		}
	}
	CursorPos(2,15);
	count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(m==0)	m=0;
			else m--;
			SendInteger(m);
			CursorPos(2,15);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(m==5)	m=5;
			else m++;
			SendInteger(m);
			CursorPos(2,15);
		}
	}

	CursorPos(2,16);
	count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(n==0)	n=0;
			else n--;
			SendInteger(n);
			CursorPos(2,16);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(n==9)	n=9;
			else n++;
			SendInteger(n);
			CursorPos(2,16);
		}
	}
	ConvertTime();
}

void Reset(void)
{
	_delay_ms(10);
	PORTA |= 1<<PINA7;
	countAlarm1=0;
	GetTemp();
}

void SetAlarm(void)
{
	AlarmStatus=1;
	SendCmd(0x01);
	CursorPos(1,1);
	SendCmd(0b00001111);
	SendString("Enter Alarm Time");
	CursorPos(2,1);
	SendString("HH");
	SendChar(0x2f);
	SendString("MM :-");
	CursorPos(2,12);
	SendInteger(AHour1);
	SendInteger(AHour2);
	SendChar(0x2f);
	SendInteger(AMinute1);
	SendInteger(AMinute2);
	CursorPos(2,12);
	
	int count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(AHour1==0)	AHour1=0;
			else AHour1--;
			SendInteger(AHour1);
			CursorPos(2,12);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(AHour1==2)	AHour1=2;
			else AHour1++;
			SendInteger(AHour1);
			CursorPos(2,12);
		}
	}

	CursorPos(2,13);
	count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(AHour2==0)	AHour2=0;
			else AHour2--;
			SendInteger(AHour2);
			CursorPos(2,13);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if((AHour1==2) & (AHour2==3))	AHour2=3;
			else if(AHour2==9)	AHour2=9;
			else AHour2++;
			SendInteger(AHour2);
			CursorPos(2,13);
		}
	}

	CursorPos(2,15);
	count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(AMinute1==0)	AHour2=0;
			else AMinute1--;
			SendInteger(AMinute1);
			CursorPos(2,15);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(AMinute1==5)	AMinute1=5;
			else AMinute1++;
			SendInteger(AMinute1);
			CursorPos(2,15);
		}
	}

	CursorPos(2,16);
	count1=0;
	while(count1==0)
	{
		if(ButtonPressed(0, swPORT, sw1, 500))
		{
			if(AMinute2==0)	AMinute2=0;
			else AMinute2--;
			SendInteger(AMinute2);
			CursorPos(2,16);
			
		}
		else if(ButtonPressed(1, swPORT, sw2, 500))
			{
				count1++;
			}	
		else if(ButtonPressed(2, swPORT, sw3, 500))
		{	
			if(AMinute2==9)	AMinute2=9;
			else AMinute2++;
			SendInteger(AMinute2);
			CursorPos(2,16);
		}
	}
	SendCmd(0b00001100);
	SendCmd(0x01);
}
	
void CheckAlarm(void)
{
	if((AHour==hour) & (AMinute==min))
	{
		Alarm();
	}
}

void Alarm(void)
{
	countAlarm1++;
	AlarmStatus=0;
	SendCmd(0x01);
	CursorPos(1,1);
	SendString("*****ALARM******");
	CursorPos(2,1);
	SendString("Press Menu 2 Stp");
	while(AMinute==min)
	{
		PORTA &= ~(1<<PINA7);
		_delay_ms(100);
		PORTA |= 1<<PINA7;
		_delay_ms(100);
		
		PORTA &= ~(1<<PINA7);
		_delay_ms(100);
		PORTA |= 1<<PINA7;
		_delay_ms(500);
		ReadData();
	}
	SendCmd(0x01);
	Reset();
}


void WriteData(void)
{
	ds1307_second_write(sec);

	ds1307_minute_write(min);

	ds1307_hour_write(hour,0,1);
	
	ds1307_day_write(day);
	
	ds1307_date_write(date);
	
	ds1307_month_write(month);
	
	ds1307_year_write(year);
}

void ReadData(void)
{
	sec=ds1307_read_second();
	min=ds1307_read_minute();
	hour=ds1307_read_hour();
	day=ds1307_read_day();
	date=ds1307_read_date();
	month=ds1307_read_month();
	year=ds1307_read_year();
	
	sec=convert_bcd_to_decimal(sec);
	min=convert_bcd_to_decimal(min);
	hour=convert_bcd_to_decimal(hour);
	day=convert_bcd_to_decimal(day);
	date=convert_bcd_to_decimal(date);
	month=convert_bcd_to_decimal(month);
	year=convert_bcd_to_decimal(year);
}

void ConvertTime(void)
{
	hour=(i*10)+j;
	min=(k*10)+l;
	sec=(m*10)+n;
}

void ConvertAlarmTime(void)
{
	AHour=(AHour1*10)+AHour2;
	AMinute=(AMinute1*10)+AMinute2;
}
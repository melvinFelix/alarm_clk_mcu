//
//
//
//  1602A 16*2 LCD display
//
//  created by Melvin Felix on 20/11/2016.
//  library for lcd displays (tested on Atmega16 and similar MCU's)
//  based on "LCD.h" by Samiran Patra

#ifndef lcd
#define lcd
//
//
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
//
//
#define ddr_lcd                      DDRB
#define lcd_port                     PORTB
#define ddr_control_port             DDRA
#define lcd_control_port             PORTA
#define Enable                       3
#define RW                           2
#define RS                           1
//
//
//

char column_position[2] = {0, 64};

//
//
//

void check_busy(void);
//
void flash_lcd(void);
//
void send_command(unsigned char command);
//
void send_character(unsigned char character);
//
void send_string(char *str);
//
void send_integer(int Int);
//
void cursor_position(uint8_t y,uint8_t x);
//
void initialize_lcd(void);

//
//
//
//
//

void check_busy() {
    ddr_lcd = 0x00;
    lcd_control_port |= 1<<RW;
    lcd_control_port &= ~1<<Rs;
    
    while (lcd_port > 0x80) {
        flash_lcd();
    }
    
    ddr_lcd = 0xFF;
}

//
//
//
//
//
//

void flash_lcd() {
    lcd_control_port |= 1<<Enable;
    _delay_ms(5);
    lcd_control_port &= ~1<<Enable;
}
//
//
//
//
//

void send_command(unsigned char command) {
    check_busy();
    lcd_port = command;
    lcd_control_port &= ~((1<<RW)|(1<<RS));
    flash_lcd();
    lcd_port = 0x00;
}

//
//
//
//
//

void send_character(unsigned char character) {
    check_busy();
    lcd_port = character;
    lcd_control_port &= ~(1<<RW);
    lcd_control_port |= 1<<Rs;
    flash_lcd();
    lcd_port = 0x00;
}

//
//
//
//
//

void send_string(char *str) {
    while (*str > 0) {
        send_character(*str++);
    }
}

//
//
//
//
//

void send_integer(int Int) {
    char ch[16];
    itoa(Int,ch,10);
    send_string(ch);
}

void cursor_position(uint8_t y,uint8_t x) {
    send_command(0x80 + column_position[y-1] + (x-1));
}

//
//
//
//
//

void initialize_lcd() {
    ddr_control_port |= 1<<Enable | 1 <<RW | 1<<RS;
    _delay_ms(50);
    
    send_command(0x01);
    _delay_ms(2);
    send_command(0x38);
    _delay_ms(50);
    send_command(0b00001110);
    _delay_ms(50);
}

#endif
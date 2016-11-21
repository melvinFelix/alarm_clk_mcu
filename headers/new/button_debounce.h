//
//  Button Debouncing
//
//
//  button_debounce.c
//  alarm_clk_mcu
//
//  Created by Melvin Felix on 20/11/16.
//  library for button debouncing (tested on Atmega16 and similar MCU's)
//  based on "ButtonPress.h" by Samiran Patra
//
//  Warning: Define number_of_buttons in the main routine

#ifndef button_debounce
#define button_debounce

#include <avr/io.h>

char button_pressed(int button_number, unsigned char pin_of_button, unsigned char port_bit,int confidence_level);
//
//  the function prototype for the following function ...
//

char button_previous_state[number_of_buttons];
int pressed_confidence_level[number_of_buttons];
int released_confidence_level[number_of_buttons];
//
//the above variables measure button previous state and confidence levels
//
char button_pressed(int button_number, unsigned char pin_of_button, unsigned char port_bit,int confidence_level) {
    
    if(bit_is_clear(pin_of_button, port_bit)) {
        pressed_confidence_level[button_number]++;
        released_confidence_level[button_number] = 0;
        
        if (pressed_confidence_level[button_number] > confidence_level) {
            if (button_previous_state[button_number] == 0) {
                button_previous_state[button_number] = 1;
                return 1;
            }
            pressed_confidence_level[button_number] = 0;
        }
    }
    else {
        released_confidence_level[button_number]++;
        pressed_confidence_level[button_number] = 0;
        if (released_confidence_level[button_number] > confidence_level) {
            button_previous_state[button_number] = 0;
            released_confidence_level[button_number] = 0;
        }
    }
    return 0;
}

#endif


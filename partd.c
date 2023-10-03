/*
 * partd.c
 *
 * Created: 2023/10/1 11:48:11
 *  Author: haoche
 */ 
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#define BUTTON_PIN PIND2
#define BUTTON_PIN_BIT (1 << BUTTON_PIN)

volatile unsigned long button_press_time_count = 0;
volatile unsigned long button_release_time_count = 0;
volatile unsigned char last_button_state = 0;

void init_timer(void) {
	TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
	TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt
	OCR1A   = 1999; // Set CTC compare value to 1ms at 16MHz AVR clock, with a prescaler of 8
	TCCR1B |= (1 << CS11); // Start timer at F_CPU/8
}


ISR(TIMER1_COMPA_vect) {
	unsigned char current_button_state = (PIND & (1 << BUTTON_PIN)) ? 1 : 0;
	
	// Button is currently pressed
	if (current_button_state == 1) {
		button_press_time_count++;
		button_release_time_count = 0;
	}
	// Button is currently not pressed
	else {
		button_release_time_count++;
		
		// Check if to send a space
		if (button_release_time_count == 400) {
			UART_putstring(" ");
			// We don't reset the count here to prevent multiple spaces from being sent
		}
	}
	
	// Check for state change from pressed to released
	if (last_button_state == 1 && current_button_state == 0) {
		if (button_press_time_count >= 30 && button_press_time_count < 200) {
			UART_putstring(".");
			} else if (button_press_time_count >= 200 && button_press_time_count < 400) {
			UART_putstring("-");
		}
		button_press_time_count = 0;  // Reset the counter after handling the press duration
	}
	last_button_state = current_button_state;  // Update the last button state every time
}



int main(void) {
	UART_init(BAUD_PRESCALER);
	DDRD &= ~(1 << BUTTON_PIN); // Set PIND2 as input
	init_timer();

	sei(); // Enable global interrupts

	while (1) {
		// Do nothing in main loop, all functionality is in interrupts
	}
}
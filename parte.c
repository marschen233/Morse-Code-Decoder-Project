/*
 * parte.c
 *
 * Created: 2023/10/2 20:51:43
 *  Author: haoche
 */ 
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "uart.h"

#define BUTTON_PIN PIND2
#define RED_LED PB1
#define WHITE_LED PB2

// Timing Definitions
#define DOT_MIN 30
#define DOT_MAX 200
#define DASH_MIN 200
#define DASH_MAX 400
#define SPACE_MIN 400  // For simplicity, only one threshold for space detection

char sequence[6] = {0};  // Store the sequence of dots, dashes, and spaces

// Morse code definitions A-Z
const char* morse_alphabet[26] = {
	".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
	".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
	"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
};

// Morse code definitions 0-9
const char* morse_numbers[10] = {
	"-----", ".----", "..---", "...--", "....-", ".....", "-....",
	"--...", "---..", "----."
};

void decode_morse(char *sequence);

int main(void) {
	UART_init(BAUD_PRESCALER);
	
	// Set PIND2 as input and PB1, PB2 as outputs
	DDRD &= ~(1 << BUTTON_PIN);
	DDRB |= (1 << RED_LED) | (1 << WHITE_LED);
	
	// Timer initialization for every 1ms
	TCCR1B |= (1 << WGM12) | (1 << CS12);  // CTC mode, prescaler 256
	OCR1A = 62;  // Top value for 1ms
	TIMSK1 |= (1 << OCIE1A);  // Enable timer compare interrupt
	sei();  // Enable global interrupts

	while (1) {
		// All action is happening in the ISR and the functions it calls
	}
}

void decode_morse(char *sequence) {
	char output[2] = {0, 0};  // Output buffer for single character conversion

	//loop to print A-Z
	for (int i = 0; i < 26; i++) {
		if (strcmp(sequence, morse_alphabet[i]) == 0) {
			output[0] = 'A' + i;
			UART_putstring(output);
			return;
		}
	}

	//loop to print 0-9
	for (int i = 0; i < 10; i++) {
		if (strcmp(sequence, morse_numbers[i]) == 0) {
			output[0] = '0' + i;
			UART_putstring(output);
			return;
		}
	}
}

ISR(TIMER1_COMPA_vect) {
	static unsigned char last_button_state = 0;
	static volatile int button_press_time_count = 0;
	static volatile int button_release_time_count = 0;

	//check button state
	unsigned char current_button_state = (PIND & (1 << BUTTON_PIN)) ? 1 : 0;
	
	if (current_button_state == 1) {
		button_press_time_count++; //keep counting press state
		button_release_time_count = 0; // reset release state
		} else {
		button_release_time_count++; //keep counting release state
		
		if (button_release_time_count == SPACE_MIN) {
			decode_morse(sequence);
			memset(sequence, 0, sizeof(sequence));  // Reset sequence after decoding
		}
	}
	
	//determine dot, dash
	if (last_button_state == 1 && current_button_state == 0) {
		if (button_press_time_count >= DOT_MIN && button_press_time_count < DOT_MAX) {
			PORTB |= (1 << WHITE_LED);
			_delay_ms(50);
			PORTB &= ~(1 << WHITE_LED);
			strncat(sequence, ".", sizeof(sequence)-1);
			} else if (button_press_time_count >= DASH_MIN && button_press_time_count < DASH_MAX) {
			PORTB |= (1 << RED_LED);
			_delay_ms(50);
			PORTB &= ~(1 << RED_LED);
			strncat(sequence, "-", sizeof(sequence)-1);
		}
		button_press_time_count = 0;
	}
	
	last_button_state = current_button_state; //loop over, reset button state
}

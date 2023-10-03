/*
 * parte.c
 *
 * Created: 2023/10/2 20:51:43
 *  Author: haoche
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#define RED_LED PB1
#define WHITE_LED PB2
#define BUTTON_PIN PIND2

// Timing definitions
#define DOT_MIN 30
#define DOT_MAX 200
#define DASH_MIN 200
#define DASH_MAX 400
#define SPACE_MIN 400

//A-Z
char* morse_alphabet[] = {
	".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",
	".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",
	"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."
};

//0-9
char* morse_numbers[] = {
	"-----", ".----", "..---", "...--", "....-", ".....",
	"-....", "--...", "---..", "----."
};

volatile int button_press_time_count = 0;
volatile int button_release_time_count = 0;
unsigned char last_button_state = 0;

void init();
void decode_morse(char *sequence);
char sequence[6] = {0};
int idx = 0;
int main(void) {
	
	init();
	
	while(1) {
		if (sequence[idx] == ' ') { // Sequence complete
			sequence[idx] = 0;  // Null terminate
			decode_morse(sequence);
			idx = 0;  // Reset for next sequence
		}
	}

	return 0;
}

void init() {
	// Initialize UART
	UART_init(BAUD_PRESCALER);

	// Initialize LEDs
	DDRB |= (1 << RED_LED) | (1 << WHITE_LED);
	DDRD &= ~(1 << BUTTON_PIN);  // Set as input

	// Timer setup for timing checks
	TCCR1B |= (1 << WGM12) | (1 << CS10);  // CTC mode, No prescaler
	TIMSK1 |= (1 << OCIE1A);  // Enable timer compare interrupt
	OCR1A = 16000;  // Set for 1ms @ 16MHz
	sei();  // Enable global interrupts
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
	unsigned char current_button_state = (PIND & (1 << BUTTON_PIN)) ? 1 : 0; //check button state
	
	if (current_button_state == 1) {
		button_press_time_count++; //keep counting press state
		button_release_time_count = 0; // reset release state
		} else {
		button_release_time_count++; //keep counting release state
		
		//determine space
		if (button_release_time_count == SPACE_MIN) {
			sequence[idx] = ' ';
			if (idx < 5) idx++;
		}
	}
	
	//determine dot, dash
	if (last_button_state == 1 && current_button_state == 0) {
		if (button_press_time_count >= DOT_MIN && button_press_time_count < DOT_MAX) {
			PORTB |= (1 << WHITE_LED);
			_delay_ms(50);
			PORTB &= ~(1 << WHITE_LED);
			sequence[idx] = '.';
			if (idx < 5) idx++;
			} else if (button_press_time_count >= DASH_MIN && button_press_time_count < DASH_MAX) {
			PORTB |= (1 << RED_LED);
			_delay_ms(50);
			PORTB &= ~(1 << RED_LED);
			sequence[idx] = '-';
			if (idx < 5) idx++;
		}
		button_press_time_count = 0;
	}
	
	last_button_state = current_button_state; //loop over, reset button state
}
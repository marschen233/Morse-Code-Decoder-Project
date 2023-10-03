/*
 * partb.c
 *
 * Created: 2023/9/22 17:47:46
 *  Author: haoche
 */ 
#include <avr/io.h>


void Initialize() {
	// Setting PB5 (built-in LED) as output
	DDRB |= (1<<DDB5);

	// Turn off the built-in LED initially
	PORTB &= ~(1<<PORTB5);

	// Set PB0 as input
	DDRD &= ~(1 << PINB0);
	
	// Enable internal pull-up for PB0
	PORTD |= (1 << PINB0);
}

int main(void) {
	Initialize();
	uint8_t prevState = 1;  // Assuming button starts unpressed
	uint8_t ledState = 0;   // This variable keeps track of which LED state (on/off)

	while (1) {
		uint8_t currentState = (PINB & (1 << PINB0)) >> PINB0;

		if (prevState == 1 && currentState == 0) {
			ledState = !ledState;  // Toggle LED state

			if (ledState) {
				PORTB |= (1 << PORTB5);   // Turn on the built-in LED
				} else {
				PORTB &= ~(1 << PORTB5);  // Turn off the built-in LED
			}
			
			
		}

		prevState = currentState;
		
	}

	return 0;
}
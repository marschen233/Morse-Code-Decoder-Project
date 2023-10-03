/*
 * partc.c
 *
 * Created: 2023/9/29 10:46:40
 *  Author: haoche
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

void Initialize() {
    // Setting PB5 (built-in LED) as output
    DDRB |= (1 << DDB5);

    // Turn off the built-in LED initially
    PORTB &= ~(1 << PORTB5);

    // Set PB0 (ICP1) as input and enable pull-up
    DDRB &= ~(1 << DDB0);
    PORTB |= (1 << PORTB0);

    // Set edge control for Input Capture on falling edge first
    TCCR1B &= ~(1 << ICES1);

    // Enable the Input Capture interrupt
    TIMSK1 |= (1 << ICIE1);

    // Set the clock source to no prescaler (Direct system clock)
    TCCR1B |= (1 << CS10);
}

ISR(TIMER1_CAPT_vect) {
    // Clear the input capture flag by writing a one to ICF1
    TIFR1 |= (1 << ICF1);

    // Toggle the edge detection type
    TCCR1B ^= (1 << ICES1);

    // Check the state of the button
    if (PINB & (1 << PINB0)) {  // If button is released (HIGH)
        PORTB |= (1 << PORTB5);  // Turn off LED
    } else {  // If button is pressed (LOW)
        PORTB &= ~(1 << PORTB5);   // Turn on LED
    }
}

int main(void) {
    Initialize();
    sei();  // Enable global interrupts

    while (1) {
        // Main loop remains empty as we're using interrupts to handle the button input capture
    }

    return 0;
}
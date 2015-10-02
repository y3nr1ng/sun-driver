// this code sets up a timer0 for 4ms @ 16Mhz clock cycle
// an interrupt is triggered each time the interval occurs.

#include <avr/io.h>
#include <avr/interrupt.h>

#define enable_interrupt sei()

void init_timers(void)
{
	/*
	 * Timer for backround illumination.
	 */

	// Set Timer0 to CTC mode.
	TCCR0A |= (1 << WGM01);
	// Set the value to count to.
	OCR0A = 0x7C;
	// Set the ISR COMPA vect.
	TIMSK0 |= (1 << OCIE0A);
}

void start_timers(void)
{
	// Set prescalar to 256 and start the timer.
	TCCR0B |= (1 << CS02);
}

void init_ports(void)
{
	// Set PB5 as LED output.
	DDRB |= (1 << 5);
	PORTB &= ~(1 << 5);
}

int main(void)
{
	init_ports();
	init_timers();

	enable_interrupt;

	start_timers();

	while (1)
	{
	}
}

// Timer0 overflow interrupt.
ISR (TIMER0_COMPA_vect)
{
	// Toggle PB5.
	PORTB ^= (1 << 5);
}

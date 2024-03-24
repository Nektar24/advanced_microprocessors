/*
 * GccApplication1.c
 *
 * Author : nektar24
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define T2 100;
#define T3 200;

void LED_on();
void LED_off();
void start_timer_t2_and_t3();

bool button_pressed = false; // is true when t3 is running
int LED = 0; // is true when t2 is running (and light is on)

int main() {
	PORTD.DIR |= PIN0_bm; //PIN0 is output
	PORTD.DIR |= PIN2_bm; //PIN2 is output
	PORTD.OUT |= PIN0_bm; //LED0 is off
	PORTD.OUT |= PIN2_bm; //LED2 is off
	// Listen for interrupt on bit 5
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	// Enable Interrupt
	sei();
	while (1)
	{
		while (!button_pressed)
		{
			LED_off();
		}
	}
	// Disable Interrupt
	cli();
}

// Button Press INTERRUPT
ISR(PORTF_PORT_vect){
	button_pressed = true;
	LED_on();
	start_timer_t2_and_t3();
	// Reset Interrupt Flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;
}

// TIMER T2 INTERRUPT
ISR(TCA0_CMP0_vect){
	LED_off();
	// Reset Interrupt Flag
	int intflags2 = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags2;
}

// TIMER T3 INTERRUPT
ISR(TCA0_CMP1_vect){
	button_pressed = true;
	// stop timer to save resources
	TCA0.SINGLE.CTRLA = 0 ;
	// Reset Interrupt Flag
	int intflags2 = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags2;
}

void LED_on(){
	PORTD.OUTCLR = PIN0_bm;
	PORTD.OUTCLR = PIN2_bp;
	PORTD.OUT |= PIN2_bm; //LED2 is off
	LED = 1;
}
void LED_off(){
	PORTD.OUTCLR = PIN0_bp;
	PORTD.OUTCLR = PIN2_bm;
	PORTD.OUT |= PIN0_bm; //LED0 is off
	LED = 0;
}
void start_timer_t2_and_t3(){
	//-----initialize timer
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CTRLB = 0;
	// TRIGGER INTERRUPT AT T2
	TCA0.SINGLE.CMP0 = T2;
	// TRIGGER INTERRUPT AT T3
	TCA0.SINGLE.CMP1 = T3;
	TCA0.SINGLE.CTRLA = 0x7<<1;
	TCA0.SINGLE.CTRLA |= 1;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
}
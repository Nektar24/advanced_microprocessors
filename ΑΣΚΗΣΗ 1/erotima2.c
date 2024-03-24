/*
 * GccApplication1.c
 *
 * Author : nektar24
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define T1 800;
#define T2 100;
#define T3 200;

void LED_on();
void LED_off();
void start_timer_t1();
void start_timer_t2();
void start_timer_t3();
void init_timer_t2_and_t3();

bool button_pressed = false; // is true when t3 is running
int LED = 0; // is true when t2 is running (and light is on)

int main() {
	PORTD.DIR |= PIN0_bm; //PIN0 is output
	PORTD.DIR |= PIN1_bm; //PIN1 is output
	PORTD.DIR |= PIN2_bm; //PIN2 is output
	PORTD.OUT |= PIN0_bm; //LED0 is off
	PORTD.OUT |= PIN1_bm; //LED1 is off
	PORTD.OUT |= PIN2_bm; //LED2 is off
	start_timer_t1();
	// Listen for interrupt on bit 5
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	// Enable Interrupt
	sei();
	while (1)
	{
		LED_off();
	}
	// Disable Interrupt
	cli();
}

// Button Press INTERRUPT
ISR(PORTF_PORT_vect){
	button_pressed = true;
	start_timer_t2();
	start_timer_t3();
	LED_on();
	// Reset Interrupt Flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;
}

// TIMER T1 INTERRUPT
ISR(TCB0_CCMP_vect){
	start_timer_t2();
	LED_on();
	start_timer_t1();
	PORTD.OUTCLR = PIN1_bp; // TRAM LED IS ON
	// Reset Interrupt Flag
	int intflags = TCB0.INTFLAGS;
	TCB0.INTFLAGS = intflags;
}

// TIMER T2 INTERRUPT
ISR(TCA0_CMP0_vect){
	LED_on();
	// Reset Interrupt Flag
	int intflags2 = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags2;
}

// TIMER T3 INTERRUPT
ISR(TCA0_CMP1_vect){
	button_pressed = true;
	TCA0.SINGLE.CTRLA = 0 ;
	// Reset Interrupt Flag
	int intflags3 = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags3;
}

void LED_on(){
	PORTD.OUTCLR = PIN0_bm;
	PORTD.OUTCLR = PIN2_bp;
	PORTD.OUT |= PIN2_bm; //LED2 is off
	LED = 1;
}
void LED_off(){
	PORTD.OUTCLR = PIN0_bp;
	PORTD.OUTCLR = PIN1_bp;
	PORTD.OUTCLR = PIN2_bm;
	PORTD.OUT |= PIN0_bm; //LED0 is off
	PORTD.OUT |= PIN1_bm; //LED1 is off
	LED = 0;
}
void start_timer_t1(){
	//-----initialize timer t1
	TCB0.CNT = 0;
	TCB0.CTRLB = 0;
	// TRIGGER INTERRUPT AT T1
	TCB0.CCMP = T1;
	TCB0.CTRLA = 0x7<<1;
	TCB0.CTRLA |= 1;
	TCB0.INTCTRL = TCB_CCMPINIT_bm;
}
void start_timer_t2(){
	// TRIGGER INTERRUPT AT T2
	TCA0.SINGLE.CMP0 = T2;
}
void start_timer_t3(){
	// TRIGGER INTERRUPT AT T3
	TCA0.SINGLE.CMP1 = T3;
}
void init_timer_t2_and_t3(){
	//-----initialize timer
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CTRLB = 0;
	TCA0.SINGLE.CTRLA = 0x7<<1;
	TCA0.SINGLE.CTRLA |= 1;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
}
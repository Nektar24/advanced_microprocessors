/*
 * GccApplication1.c
 *
 * Author : nektar24
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define Tb 20;
#define Db 40;
#define Tl 10;
#define Dl 50;

void start_timer_t1_and_t2();
void LED0_on();
void LED0_off();
void LED1_on();
void LED1_off();

int LED0 = 0;
int LED1 = 0;

int main() {
    PORTD.DIR |= PIN0_bm; //PIN0 is output
    PORTD.DIR |= PIN1_bm; //PIN1 is output
	PORTD.OUT |= PIN0_bm; //LED0 is off
    PORTD.OUT |= PIN1_bm; //LED1 is off

    // Listen for interrupt on bit 5
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;

    // Enable Interrupt
	sei();
    while (1)
    {
        ;
    }
	// Disable Interrupt
	cli();
}

// Button Press INTERRUPT
ISR(PORTF_PORT_vect){
    start_timer_t1_and_t2();
	// Reset Interrupt Flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;
}
// Timer Βάσης
ISR(TCB0_INT_vect) {
    switch (LED1){
        case 0:
            LED1 = 1;
            LED1_on();
        break;
        case 1:
            LED1 = 0;
            LED1_off();
        break;
    }
}
// Timer λεπίδων
ISR(TCB1_INT_vect) {
    switch (LED0){
        case 0:
            LED0 = 1;
            LED0_on();
        break;
        case 1:
            LED0 = 0;
            LED0_off();
        break;
    }
}
void start_timer_t1_and_t2(){
    // Set TCB0 to compare/capture mode with waveform generator
    TCB0.CCMP = Db;
    TCB0.CTRLA = TCB_ENABLE_bm;
    TCB0.CTRLB = TCB_CCMPEN_bm | TCB_WGMODE_SS_gc;

    // Set TCB1 to compare/capture mode with waveform generator
    TCB1.CCMP = Dl;
    TCB1.CTRLA = TCB_ENABLE_bm;
    TCB1.CTRLB = TCB_CCMPEN_bm | TCB_WGMODE_SS_gc;

    // Set TCB0 and TCB1 period
    TCB0.PER = Tb;
    TCB1.PER = Tl;
}
// TURN ON LED 0 - κίνηση λεπίδων
void LED0_on(){
    PORTD.OUTCLR = PIN0_bm; //LED0 is on
}
// TURN ON LED 0 - κίνηση λεπίδων
void LED0_off(){
    PORTD.OUT |= PIN0_bm; //LED0 is off
}
// TURN ON LED 1 - κίνηση βάσης
void LED1_on(){
    PORTD.OUTCLR = PIN1_bm; //LED1 is on
}
// TURN ON LED 1 - κίνηση βάσης
void LED1_off(){
    PORTD.OUT |= PIN1_bm; //LED1 is off
}
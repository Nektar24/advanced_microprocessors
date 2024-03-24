/*
 * GccApplication1.c
 *
 * Author : nektar24
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define T1 30;

void enable();
void start_timer_t1();
bool is5bit();
bool is6bit();
void LED0_on();
void LED0_off();

int state = 0;
bool armed = false;

int main() {
    // Listen for interrupt on bit 5 and 6
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
    PORTF.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;

    // Enable Interrupt
	sei();
    while (true)
    {
        ;
    }
	// Disable Interrupt
	cli();
}

// Button Press INTERRUPT
ISR(PORTF_PORT_vect){
    if (!armed){ enable(); }
    // Reset Interrupt Flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;
}

// TIMER T1 INTERRUPT
ISR(TCA0_CMP0_vect){
    armed = true;
	// Reset Interrupt Flag
	int intflags2 = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags2;
}

void enable(){
    switch (state){
        case 1:
            if (is5bit()){
                state ++ ;
            } else {
                state = 1;
            }
        break;
        case 2:
            if (is6bit()){
                state ++ ;
            } else {
                state = 1;
            }
        break;
        case 3:
            if (is5bit()){
                state ++ ;
            } else {
                state = 1;
            }
        break;
        case 4:
            if (is6bit()){
                // COMBINATION IS CORRECT
                state ++ ;
                start_timer_t1();
            } else {
                state = 1;
            }
        break;
    }
}
void start_timer_t1(){
	//-----initialize timer
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CTRLB = 0;
	// TRIGGER INTERRUPT AT T1
	TCA0.SINGLE.CMP0 = T1;
	TCA0.SINGLE.CTRLA = 0x7<<1;
	TCA0.SINGLE.CTRLA |= 1;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
}
// Ελέγχει αν το 5ο bit ειναι on στο INTFLAGS 
// αλλα οχι και το 5ο και το 6ο
bool is5bit(){
    if (PORTF.INTFLAGS & (PIN5_bm & PIN6_bm)){
        return false;
    }
    if (PORTF.INTFLAGS & PIN5_bm){
        return true;
    }
}
// Ελέγχει αν το 6ο bit ειναι on στο INTFLAGS 
// αλλα οχι και το 5ο και το 6ο
bool is6bit(){
    if (PORTF.INTFLAGS & (PIN5_bm & PIN6_bm)){
        return false;
    }
    if (PORTF.INTFLAGS & PIN6_bm){
        return true;
    }
}
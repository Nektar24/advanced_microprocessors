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
#define Tl 10;

int button_state = 1;

int main() {
    PORTD.DIR |= PIN0_bm; //PIN0 is output
    PORTD.DIR |= PIN1_bm; //PIN1 is output
	PORTD.DIR |= PIN2_bm; //PIN2 is output
	PORTD.OUT |= PIN0_bm; //LED0 is off
    PORTD.OUT |= PIN1_bm; //LED1 is off
	PORTD.OUT |= PIN2_bm; //LED2 is off
    // Listen for interrupt on bit 5
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
    // Enable Interrupt
	sei();
    while (button_state <= 3)
    {
        ;
    }
    PORTD.OUT |= PIN0_bm; //LED0 is off
    PORTD.OUT |= PIN1_bm; //LED1 is off
	PORTD.OUT |= PIN2_bm; //LED2 is off
	// Disable Interrupt
	cli();
}

// Button Press INTERRUPT
ISR(PORTF_PORT_vect){
	switch (button_state){
        case 1:
            // Πάτημα κουμπιού για πρώτη φορά
        break;
        case 2:
            // Πάτημα κουμπιού δευτερη φορά
            // η περίοδος της κυκλικής κίνησης διπλασιάζεται
        break;
        case 3:
            // Πάτημα κουμπιού τρίτη φορά
            // τέλος προγράμματος
        break;
    }
    button_state++;
	// Reset Interrupt Flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;
}
// TURN ON LED 0 - κίνηση λεπίδων
void LED0(){
    PORTD.OUTCLR = PIN0_bm; //LED0 is on
}
// TURN ON LED 1 - κίνηση βάσης
void LED1(){
	PORTD.OUTCLR = PIN1_bm; //LED1 is on
}
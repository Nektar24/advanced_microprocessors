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
void LED2_on();
void LED2_off();

bool FINGER = false;
int button_state = 0;
int LED0 = 0;
int LED1 = 0;

int main() {
    PORTD.DIR |= PIN0_bm; //PIN0 is output
    PORTD.DIR |= PIN1_bm; //PIN1 is output
    PORTD.DIR |= PIN2_bm; //PIN2 is output
	PORTD.OUT |= PIN0_bm; //LED0 is off
    PORTD.OUT |= PIN1_bm; //LED1 is off
    PORTD.OUT |= PIN2_bm; //LED2 is off

    //iniitalize the ADC for value LT = 10
    ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
    ADC0.CTRLA |= ADC_FREERUN_bm;
    ADC0.CTRLA |= ADC_ENABLE_bm;
    ADC0.MYXPOS |= ADC_MYXPOS_AIN7_gc;
    ADC0.DBGCTRL |= ADC_DBGRUN_bg;
    ADC0.WINLT |= 10;
    ADC0.INTCTRL |= ADC_WCWP_bm;
    ADC0.CTRLE |= ADC_WINCM0_bm;

    // ENABLING ADC
    ADC0.COMMAND |= ADC_STCONV_bm;

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

// Δάχτυλο στον Ανεμυστήρα
ISR(ADC0_WCOMP_vect){
    FINGER = true;
    LED0_off();
    LED1_off();
    LED2_on();
    // Reset Interrupt Flag
    int intflags = ADC0.INTFLAGS;
    ADC0.INTFLAGS = intflags;
}

// Button Press INTERRUPT
ISR(PORTF_PORT_vect){
    switch (button_state){
        case 0:
            // Πάτημα κουμπιού για πρώτη φορά
            start_timer_t1_and_t2();
            button_state = 1;
            break;
        case 1:
            if (FINGER){
                // Πάτημα κουμπιού για επανέναρξη μετα απο emergency shutdown
                start_timer_t1_and_t2();
                FINGER = false;
                LED2_off();
            }
            break;
    }
	// Reset Interrupt Flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;
}
// Timer Βάσης
ISR(TCB0_INT_vect) {
    if (FINGER) {return;}
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
    // Reset Interrupt Flag
	int intflags = TCB0.INTFLAGS;
	TCB0.INTFLAGS = intflags;
}
// Timer λεπίδων
ISR(TCB1_INT_vect) {
    if (FINGER) {return;}
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
    // Reset Interrupt Flag
	int intflags = TCB1.INTFLAGS;
	TCB1.INTFLAGS = intflags;
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
// TURN ON LED 2 - emergency LED
void LED2_on(){
    PORTD.OUTCLR = PIN2_bm; //LED2 is on
}
// TURN ON LED 2 - emergency LED
void LED2_off(){
    PORTD.OUT |= PIN2_bm; //LED2 is off
}
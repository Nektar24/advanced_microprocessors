/*
 * GccApplication1.c
 *
 * Author : nektar24
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define T1 200; // 20 seconds to get out of building
#define T2 100; // 10 seconds to type correct code
#define Ta 20;
#define Da 10;

void enable();
void disable();
void start_timer_t1();
void start_timer_t2();
void start_alarm_PWD();
bool is5bit();
bool is6bit();
void LED0_on();
void LED0_off();

int state = 0;
bool armed = false;
int tries = 0;
bool detected_criminal = false;

int main() {
    PORTD.DIR |= PIN0_bm; //PIN0 is output
    PORTD.OUT |= PIN0_bm; //LED0 is off

    //iniitalize the ADC for value LT = 10
    ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
    ADC0.CTRLA |= ADC_FREERUN_bm;
    ADC0.CTRLA |= ADC_ENABLE_bm;
    ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc;
    ADC0.DBGCTRL |= ADC_DBGRUN_bp;
    ADC0.WINLT |= 10;
    ADC0.INTCTRL |= ADC_WCMP_bm;
    ADC0.CTRLE |= ADC_WINCM0_bm;

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
    if (!armed){
        enable();
    }
    if (armed && detected_criminal){
        disable();
    }
    // Reset Interrupt Flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;
}

// TIMER T1 INTERRUPT
ISR(TCA0_CMP0_vect){
    armed = true;
    // ENABLING ADC
    ADC0.COMMAND |= ADC_STCONV_bm;
	// Reset Interrupt Flag
	int intflags2 = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags2;
}
// TIMER T2 INTERRUPT
ISR(TCA0_CMP1_vect){
    start_alarm_PWD();
	// Reset Interrupt Flag
	int intflags2 = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags2;
}

// Κάποιος μπήκε στο σπίτι
ISR(ADC0_WCOMP_vect){
    LED0_on();
    start_timer_t2();
    detected_criminal = true;
    state = 0;
    // Reset Interrupt Flag
    int intflags = ADC0.INTFLAGS;
    ADC0.INTFLAGS = intflags;
}

ISR(TCB0_INT_vect){
    if (TCB0.INTFLAGS & TCB_CAPT_bm){
        LED0_on();
    }
    else {
        LED0_off();
    }
	// Reset Interrupt Flag
	int intflags = TCB0.INTFLAGS;
	TCB0.INTFLAGS = intflags;
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
void disable(){
    switch (state){
        case 1:
            if (is5bit()){
                state ++ ;
            } else {
                state = 1;
                tries++;
            }
        break;
        case 2:
            if (is6bit()){
                state ++ ;
            } else {
                state = 1;
                tries++;
            }
        break;
        case 3:
            if (is5bit()){
                state ++ ;
            } else {
                state = 1;
                tries++;
            }
        break;
        case 4:
            if (is6bit()){
                // COMBINATION IS CORRECT
                LED0_off();
                armed = false;
                tries = 0;
                state = 1;
                // TURN OFF ALARM IF IT IS ON
                TCB0.CTRLA &= ~TCB_ENABLE_bm;
            } else {
                state = 1;
                tries++;
            }
        break;
    }
    if (tries > 3){
        start_alarm_PWD();
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
void start_timer_t2(){
	//-----initialize timer
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CTRLB = 0;
	// TRIGGER INTERRUPT AT T2
	TCA0.SINGLE.CMP1 = T2;
	TCA0.SINGLE.CTRLA = 0x7<<1;
	TCA0.SINGLE.CTRLA |= 1;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
}
void start_alarm_PWD(){
    // Set TCB0 to compare/capture mode with waveform generator
	TCB0.CCMPL = Ta;
    TCB0.CTRLA |= TCB_ENABLE_bm;
    TCB0.CTRLB |= TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;
	
    // Set TCB0 and TCB1 period
    TCB0.CCMPH = Da;
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
// TURN ON LED 0
void LED0_on(){
    PORTD.OUTCLR = PIN0_bm; //LED0 is on
}
// TURN OFF LED 0
void LED0_off(){
    PORTD.OUT |= PIN0_bm; //LED0 is off
}
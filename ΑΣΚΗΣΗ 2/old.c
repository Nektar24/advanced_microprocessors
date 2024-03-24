/*
 * GccApplication1.c
 *
 * Author : nektar24
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#define T1 900;
#define T2 500;
#define T3 300;

void go_Straight();
void go_Left();
void go_Right();
void start_timer_t2_and_t3();

int Corner_degrees = 0; // when it reaches or surpasses 360 degrees it knows it has done a circle
bool T2_PAUSE = false;
bool NOTINATURN = true;

int main() {
	PORTD.DIR |= PIN0_bm; //PIN0 is output
    PORTD.DIR |= PIN1_bm; //PIN1 is output
	PORTD.DIR |= PIN2_bm; //PIN2 is output
	PORTD.OUT |= PIN0_bm; //LED0 is off
    PORTD.OUT |= PIN1_bm; //LED1 is off
	PORTD.OUT |= PIN2_bm; //LED2 is off
    //iniitalize the ADC for Free-Running mode
    ADC0.CTRLA |= ADC_RESSEL_10BIT_gc;
    ADC0.CTRLA |= ADC_FREERUN_bm;
    ADC0.CTRLA |= ADC_ENABLE_bm;
    ADC0.MYXPOS |= ADC_MYXPOS_AIN7_gc;
    ADC0.DBGCTRL |= ADC_DBGRUN_bg;
    ADC0.WINLT |= 125;
    ADC0.INTCTRL |= ADC_WCWP_bm;
    ADC0.CTRLE |= ADC_WINCM0_bm;
    // Enable Interrupt
	sei();
    ADC0.COMMAND |= ADC_STCONV_bm;
    start_timer_t2_and_t3();
    while (Corner_degrees < 360)
	{
        while (!T2_PAUSE && NOTINATURN)
        {
            while (!ADC0.RES) // RESULT FROM ADC
            {
                go_Straight();
            }
        }
	}
    PORTD.OUT |= PIN0_bm; //LED0 is off
    PORTD.OUT |= PIN1_bm; //LED1 is off
	PORTD.OUT |= PIN2_bm; //LED2 is off
	// Disable Interrupt
	cli();
}

// ADC Single Conversion interrupt
ISR(ADC0_WCOMP_vect){

    // Reset Interrupt Flag
    ADC0.COMMAND |= ADC_STCONV_bm;
    int intflags = ADC0.INTFLAGS;
    ADC0.INTFLAGS = intflags;
}

// TIMER T3 INTERRUPT
ISR(TCA0_CMP0_vect){
    T2_PAUSE = true;
	go_Left();
    Corner_degrees += 90;
    start_turning();
	// Reset Interrupt Flag
	int intflags2 = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags2;
}

// TIMER T2 INTERRUPT
ISR(TCA0_CMP1_vect){
	T2_PAUSE = false;
	// Reset Interrupt Flag
	int intflags3 = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags3;
}

// TURNING TIMER INTERRUPT - TURNING FINISHED
ISR(TCB0_CCMP_vect){
	go_Straight();
    NOTINATURN = true;
	// Reset Interrupt Flag
	int intflags = TCB0.INTFLAGS;
	TCB0.INTFLAGS = intflags;
}

// TURN ON LED 1 AND TURN OFF LEDS 0 AND 2
void go_Straight(){
    PORTD.OUTCLR = PIN0_bp;
	PORTD.OUTCLR = PIN1_bm; //LED1 is on
	PORTD.OUTCLR = PIN2_bp;
    PORTD.OUT |= PIN0_bm; //LED0 is off
	PORTD.OUT |= PIN2_bm; //LED2 is off
}
// TURN ON LED 0 AND TURN OFF LEDS 1 AND 2
void go_Right(){
    PORTD.OUTCLR = PIN0_bm; //LED0 is on
	PORTD.OUTCLR = PIN1_bp;
	PORTD.OUTCLR = PIN2_bp;
    PORTD.OUT |= PIN1_bm; //LED1 is off
    PORTD.OUT |= PIN2_bm; //LED2 is off
}
// TURN ON LED 2 AND TURN OFF LEDS 0 AND 1
void go_Left(){
    PORTD.OUTCLR = PIN0_bp;
	PORTD.OUTCLR = PIN1_bp;
	PORTD.OUTCLR = PIN2_bm; //LED2 is on
    PORTD.OUT |= PIN0_bm; //LED0 is off
    PORTD.OUT |= PIN1_bm; //LED1 is off
}
void start_timer_t2_and_t3(){
	//-----initialize timer
	TCA0.SINGLE.CNT = 0;
	TCA0.SINGLE.CTRLB = 0;
	// TRIGGER INTERRUPT AT T3
	TCA0.SINGLE.CMP0 = T3;
	// TRIGGER INTERRUPT AT T2+T3 (DEFINE T2 IS T2+T3)
	TCA0.SINGLE.CMP1 = T2;
	TCA0.SINGLE.CTRLA = 0x7<<1;
	TCA0.SINGLE.CTRLA |= 1;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_CMP0_bm;
}
void start_turning(){
	//-----initialize turning time to T1
	TCB0.CNT = 0;
	TCB0.CTRLB = 0;
	// TRIGGER INTERRUPT AT T1
	TCB0.CCMP = T1;
	TCB0.CTRLA = 0x7<<1;
	TCB0.CTRLA |= 1;
	TCB0.INTCTRL = TCB_CCMPINIT_bm;
    NOTINATURN = false;
}
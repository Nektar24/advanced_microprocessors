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
#define Db 10;
#define Dl 5;
#define Tl2 20;
#define Dl2 10;

void start_timer_t1_and_t2();
void start_timer_t1_and_t2_with_faster_t2();
void turn_off_TIMERS();
void LED0_on();
void LED0_off();
void LED1_on();
void LED1_off();
void LED2_on();
void LED2_off();

bool FINGER = false;
int button_state = 1;
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
    ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc;
    ADC0.DBGCTRL |= ADC_DBGRUN_bp;
    ADC0.WINLT |= 10;
    ADC0.INTCTRL |= ADC_WCMP_bm;
    ADC0.CTRLE |= ADC_WINCM0_bm;

    // ENABLING ADC
    ADC0.COMMAND |= ADC_STCONV_bm;

    // Listen for interrupt on bit 5
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;

    // Enable Interrupt
	sei();
    while (button_state < 3)
    {
        ;
    }
	// Disable Interrupt
	cli();
}

// Δάχτυλο στον Ανεμυστήρα
ISR(ADC0_WCOMP_vect){
    FINGER = true;
	turn_off_TIMERS();
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
        case 1:
            // Πάτημα κουμπιού για πρώτη φορά
            start_timer_t1_and_t2();
            button_state = 2;
            break;
        case 2:
            if (!FINGER){
                // Πάτημα κουμπιού δευτερη φορά
                // η περίοδος της κυκλικής κίνησης διπλασιάζεται
                start_timer_t1_and_t2_with_faster_t2();
                button_state = 3;
            }
            break;
        case 3:
            // LEDs to 0
            LED0_off();
            LED1_off();
            // Disable ADC
            ADC0.CTRLA &= ~ADC_ENABLE_bm;
            // Disable TCB0 and TCB1
            TCB0.CTRLA &= ~TCB_ENABLE_bm;
            TCB1.CTRLA &= ~TCB_ENABLE_bm;
            break;
    }
    if (FINGER){
        // Πάτημα κουμπιού για επανέναρξη μετα απο emergency shutdown
        start_timer_t1_and_t2();
        FINGER = false;
        LED2_off();
    }
	// Reset Interrupt Flag
	int intflags = PORTF.INTFLAGS;
	PORTF.INTFLAGS = intflags;
}
ISR(TCB1_INT_vect){
	if (TCB0.INTFLAGS == 1 ){
		// Timer Βάσης
		// TOGGLE LED 1
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
	// TOGGLE LED 0
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
	TCB0.CCMPL = Tb;
    TCB0.CTRLA |= TCB_ENABLE_bm;
    TCB0.CTRLB |= TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;

    // Set TCB1 to compare/capture mode with waveform generator
    TCB1.CCMPL = Tl;
    TCB1.CTRLA |= TCB_ENABLE_bm;
    TCB1.CTRLB |= TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;
	
    // Set TCB0 and TCB1 period
    TCB1.CCMPH = Dl;
    TCB0.CCMPH = Db;
}
void start_timer_t1_and_t2_with_faster_t2(){
    // Set TCB0 to compare/capture mode with waveform generator
    TCB0.CCMPL = Tb;
    TCB0.CTRLA |= TCB_ENABLE_bm;
    TCB0.CTRLB |= TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;

    // Set TCB1 to compare/capture mode with waveform generator
    TCB1.CCMPL = Tl2;
    TCB1.CTRLA |= TCB_ENABLE_bm;
    TCB1.CTRLB |= TCB_CCMPEN_bm | TCB_CNTMODE_PWM8_gc;
    
    // Set TCB0 and TCB1 period with faster TCB1
    TCB1.CCMPH = Dl2;
    TCB0.CCMPH = Db;
}
void turn_off_TIMERS(){
	TCB0.CTRLA &= ~TCB_ENABLE_bm;
	TCB1.CTRLA &= ~TCB_ENABLE_bm;
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
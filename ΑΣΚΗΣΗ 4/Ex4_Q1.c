#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int correct = 0;

int main(void)
{
	// Configure PORTF
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc; // Enable pull-up resistor and set interrupt on both edges
	PORTF.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc ; // Enable pull-up resistor and set interrupt on both edges
	sei(); //enable interrupts
	while(1){
		;
	}
}

ISR(TCA0_OVF_vect)
{
	//clear the interrupt flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags;
	//set permanently on
	PORTD.OUTSET= PIN0_bm; //Output Value Set (instead of a read-modify-write σελ 152)
}

ISR(PORTF_PORT_vect){
	cli(); // disenable interrupts
	int y = PORTF.INTFLAGS;
	PORTF.INTFLAGS = y;
	//Accepted SW5 or SW5 -> SW6 -> SW5
	if((y & PIN5_bm) && !(y & ~(PIN5_bm))){
		if(correct == 0 || correct == 2){correct++;}
		else{correct = 0;}
	}
	//Accepted SW5 -> SW6 or SW5 -> SW6 -> SW5 -> SW6
	else if((y & PIN6_bm) && !(y & ~(PIN6_bm))){
		if(correct == 1 || correct == 3){correct++;}
		else{correct = 0;}
	}
	if (correct == 4) // SW5 -> SW6 -> SW5 -> SW6
	{
		// init timer + set LED0: ON
		correct = 0; // start over
		TCA(10);	
	}
	// Clear the interrupt flag
	PORTF.INTFLAGS = y;
	sei(); //enable interrupts
}

// Initialize the TCA timer
void TCA(int period)
{
	TCA0.SINGLE.CNT = 0; //clear counter
	TCA0.SINGLE.CTRLB = 0; //Normal Mode (TCA_SINGLE_WGMODE_NORMAL_gc σελ 199)
	TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm | TCA_SINGLE_CLKSEL_DIV1024_gc; // timer + prescaler
	TCA0.SINGLE.PER = period; // timer period
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm; // Normal Mode (Timer Overflow/Underflow Interrupt Enable σελ 207)
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm; // Normal Mode (Overflow/Underflow Interrupt Flag σελ 208)
}
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int correct = 0;
int firstTime = 1; //by default, it's the first time

void TCA(int period);
void ADC(int threshold);

int main(void)
{
	//PIN0 is output
	PORTD.DIR |= PIN0_bm; //PIN0_bm
	PORTF.PIN5CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc; // Enable pull-up resistor and set interrupt on both edges
	PORTF.PIN6CTRL |= PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc ; // Enable pull-up resistor and set interrupt on both edges
	sei(); // Enable global interrupts
	while(1)
	{
		if (failures == 3 && firstTime == 0)
		{
			// siren
			// set permanently on
			PORTD.OUTSET= PIN0_bm; //Output Value Set (instead of a read-modify-write σελ 152)
		}
		
	}
}

ISR(PORTF_PORT_vect){
	cli(); // disenable interrupts
	int y = PORTF.INTFLAGS;
	PORTF.INTFLAGS = y;
	//Accepted SW5 or SW5 -> SW6 -> SW5
	if((y & PIN5_bm) && !(y & ~(PIN5_bm))){
		if(correct == 0 || correct == 2){correct++;}
		else
		{
			failures++; //missed tries
			correct = 0; //start-over
		}
	}
	//Accepted SW5 -> SW6 or SW5 -> SW6 -> SW5 -> SW6
	else if((y & PIN6_bm) && !(y & ~(PIN6_bm))){
		if(correct == 1 || correct == 3){correct++;}
		else
		{
			failures++; //missed tries
			correct = 0; //start-over
		}
	}
	// normal scenario
	if(correct == 4 && firstTime == 1)
	{
		// init timer + set LED0: ON
		correct = 0; // start over
		failures = 0; // start over
		TCA(10);
	}
	else if(correct == 4 && firstTime == 0)
	{
		// Disable alarm
		TCA0.SINGLE.CTRLA &= ~(TCA_SINGLE_ENABLE_bm); // Disable timer
		ADC0.CTRLA &= ~(ADC_ENABLE_bm); // Disable ADC
		PORTD.OUTSET = PIN0_bm; // Turn off LED0
		correct = 0;
		failures = 0;
		firstTime = 1;
	}
	
	// Clear the interrupt flag
	PORTF.INTFLAGS = y;
	sei(); //enable interrupts
}

ISR(TCA0_OVF_vect)
{
	cli(); // disenable interrupts
	//clear the interrupt flag
	int intflags = TCA0.SINGLE.INTFLAGS;
	TCA0.SINGLE.INTFLAGS = intflags;
	if (firstTime == 1)
	{
		firstTime = 0;
		TCA0.SINGLE.CTRLA &= ~(TCA_SINGLE_ENABLE_bm); // TCA off
		ADC(10); // Enable ADC with threshold of 10
	}
	// end of time
	// there could've been less than 3 tries, but still
	else
	{
		// siren
		// set permanently on
		PORTD.OUTSET= PIN0_bm; //Output Value Set (instead of a read-modify-write σελ 152)
	}
	sei(); //enable interrupts
	
}

ISR(ADC0_WCOMP_vect)
{
	// Clear the interrupt flag
	ADC0.INTFLAGS = ADC_WCMP_bm;
	TCA(1500); // Enable TCA with threshold of 1500
	PORTD.OUTCLR = PIN0_bm; // Turn on LED0
	failures = 0;
	correct = 0;
	ADC0.CTRLA &= ~(ADC_ENABLE_bm); // ADC: off
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

// Initialize the Analog-to-Digital Converter
void ADC(int threshold)
{
	// Initialize the ADC for Free-Running mode
	ADC0.CTRLA |= ADC_RESSEL_10BIT_gc; // 10-bit resolution
	ADC0.CTRLA |= ADC_FREERUN_bm; // Enable Free-Running mode
	ADC0.CTRLA |= ADC_ENABLE_bm; // Enable ADC
	ADC0.MUXPOS |= ADC_MUXPOS_AIN7_gc; // The bit //Enable DebugMode
	ADC0.DBGCTRL |= ADC_DBGRUN_bm; //Window Comparator Mode
	ADC0.WINLT |= threshold; // Set threshold
	ADC0.INTCTRL |= ADC_WCMP_bm; // Enable Interrupts for WCM
	ADC0.CTRLE |= ADC_WINCM0_bm; // Interrupt when RESULT < WINLT
	ADC0.COMMAND |= ADC_STCONV_bm; // Start Conversion
}
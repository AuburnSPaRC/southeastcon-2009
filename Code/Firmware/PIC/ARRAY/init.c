#include "hardware.h"
#include "init.h"
#include "serial.h"
#include "main.h"


int Init (void) 
{	
	Init_Oscillator();
	Init_Interrupts();
	Init_USART();
	Init_ADC();
	Init_Timers();
	
	TRISB = 0x00;
	LATB = 0x01; // Turn on a little status LED;
	return 1;
}

void Init_Oscillator(void)
{
	OSCCON = 0b01110000; //configure PIC to primary oscillator block at 8MHz
	OSCTUNEbits.PLLEN = 1;	// Enable the PLL for 32MHz
}

void Init_Interrupts(void)
{	
	RCONbits.IPEN = 1; //enable high priority and low priority interrupts
	INTCONbits.GIEL = 1; //low priority interrupts enabler
	INTCONbits.GIEH = 1; //high priority interrupt enabler
}	

void Init_USART(void)
{
	// SPBRG = (Fosc/Baud)/4 - 1
	// Fosc = 32000000
	// Baud = 115200
	// SPBRG = (32000000/115200)/4 - 1
	// SPBRG = 277.778/4 - 1 = 69.4444 - 1 = 68.4444
	
	// Actual Baud = Fosc/(4*(SPBRG + 1)) = 32000000/(4*(68 + 1)) = 115942 baud
	// Error = (115942-115200)/115200 * 100% = 0.6%
	unsigned int baud = 68;
	
	TXSTA = 0;		// Reset registers
	RCSTA = 0;
	RCSTAbits.CREN = 1;		// Continuous Reception
	TXSTAbits.BRGH = 1;		// High Baud Rate
	BAUDCONbits.BRG16 = 1;	// 16-bit Baud Rate counter
	
	TXSTAbits.TXEN = 0;
	RCSTAbits.SPEN = 0;	

	SPBRG = baud;			// Write the baud rate
	SPBRGH = baud >> 8;
	
	TXSTAbits.TXEN = 1;		// Enable TX
	RCSTAbits.SPEN = 1;		// Enable RX
	
	TRISCbits.TRISC6 = 0;	// Set Data Directions for PortC pins
	TRISCbits.TRISC7 = 1;

	PIR1bits.RCIF = 0;
	PIE1bits.RCIE = 1;		// Enable Receive Interrupt
	IPR1bits.RCIP = 1;		// High Priority
	PIR1bits.TXIF = 0;
	PIE1bits.TXIE = 1;		// Enable Transmit Interrupt
	IPR1bits.TXIP = 1;		// High Priority
}

void Init_Timers(void)
{
	OpenTimer0(	TIMER_INT_OFF	&
				T0_16BIT		&
				T0_SOURCE_INT	&
				T0_PS_1_8);
	
	WriteTimer0(65536-ADC_DELAY);
				
	INTCONbits.TMR0IF = 0;
	INTCON2bits.TMR0IP = 0;	// Low Priority
	INTCONbits.TMR0IE = 0;	//Enable Interrupt		
}	

void Init_ADC(void)
{
	TRISAbits.TRISA0 = 1;		//set pins 2,3,4, AND 5 as inputs
	TRISAbits.TRISA1 = 1;
	TRISAbits.TRISA2 = 1;
	TRISAbits.TRISA3 = 1;		
	
	OpenADC( 	ADC_FOSC_32      	&
         		ADC_RIGHT_JUST   	&
         		ADC_12_TAD,
         		ADC_CH0				&
                ADC_REF_VDD_VSS  	&
         		ADC_INT_OFF,
         		ADC_4ANA  );
}	
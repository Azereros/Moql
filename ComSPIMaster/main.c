#include <msp430.h> 
#include <Arret.h>
#include <Avancer.h>
#include <CapteurIR.h>
#include <ConvNumAn.h>
#include <Afficheur.h>


/**
 * main.c
 */
 volatile char reception;
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	if(CALBC1_1MHZ==0xFF || CALDCO_1MHZ==0xFF)
	{
	    __bis_SR_register(LMP4_bits);
	}
	else
	{
	    BCSCTL1 = CALBC1_1MHZ;
	    DCOCTL = CALDCO_1MHZ;
	}

	P1OUT |= BIT5;
	P1DIR |= BIT5;
	
	P1SEL |= (BIT1 | BIT2 | BIT4);
	P1SEL2 |= (BIT1 | BIT2 | BIT4);

	UCA0CTL1 = UCSWRST;

	UCA0CTL0 |= (UCCKPH | UCMSB | UCMST | UCSYNC);
	UCA0CTL1 |= UCSSEL_2;

	UCA0BR0 = 0x02;
	UCA0BR1 = 0x00;
	UCA0CTL = 0x00;

	UCA0CLTL1 &= ~UCSWRST;

	P1OUT &= ~BIT5;
	while(!(IFG2 & UCA0TXIFG)); //attente buffer
	UCAOTXBUF = CapteurIR(); // envoi de value
	//while(!(IFG2 & UCA0RXIFG)); //receptionUSCI_A0 ?
	//reception = UCA0RXBUF;
	P1OUT |= BIT5; //CS unselect

}

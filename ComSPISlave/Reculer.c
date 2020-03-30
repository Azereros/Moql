#include <msp430.h>


/*
 * Reculer.c
 *
 *  Created on: 14 janv. 2020
 *      Author: 17620384
 */


void Reculer(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    BCSCTL1= CALBC1_1MHZ;      //frequence d’horloge 1MHz
    DCOCTL= CALDCO_1MHZ;       //    "

    P2DIR |= BIT2;             // P2.2 en sortie
    P2DIR |= BIT4;
    P2DIR |= BIT1|BIT5;

    P2SEL |= BIT2;             // selection fonction TA1.1
    P2SEL |= BIT4;

    P2SEL2 &= ~BIT2;          // selection fonction TA1.1 (suite)
    P2SEL2 &= ~BIT4;          // selection fonction TA1.1 (suite)

    TA1CTL = TASSEL_2 | MC_1;  // source SMCLK pour TimerA (no 2), mode comptage Up
    TA1CCTL1 |= OUTMOD_7;       // activation mode de sortie n°7
    TA1CCTL2 |= OUTMOD_7;       // activation mode de sortie n°7

    TA1CCR0 = 2000;            // determine la periode du signal
    TA1CCR1 = 1000;         // determine le rapport cyclique du signal while(1);
    TA1CCR2 = 1000;

    P2OUT   |= (BIT2|BIT4);
    P2OUT &= ~(BIT5);
    P2OUT |= (BIT1);

    while(1);

}




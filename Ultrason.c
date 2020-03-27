#include <msp430.h>

int miliseconds;
double distance = 0;
long sensor;

double Ultrason(double distance)
{
    P1IE &= ~BIT0;                                          // disable interupt
    P1DIR |= BIT4;                                          // trigger pin as output
    P1OUT |= BIT4;                                          // generate pulse
    __delay_cycles(10);                                     // for 10us
    P1OUT &= ~BIT4;                                         // stop pulse
    P1DIR &= ~BIT5;                                         // make pin P1.2 input (ECHO)
    P1IFG = 0x00;                                           // clear flag just in case anything happened before
    P1IE |= BIT5;                                           // enable interupt on ECHO pin
    P1IES &= ~BIT5;                                         // rising edge on ECHO pin
    __delay_cycles(30000);                                  // delay for 30ms (after this time echo times out if there is no object detected)
    distance = sensor/58;                                   // converting ECHO lenght into cm
    if(distance < 20 && distance != 0){
        P1OUT |= 0x01;                                      //turning LED on if distance is less than 20cm and if distance isn't 0.
    }
    else
    {
        P1OUT &= ~BIT0;
    }
    return distance;
}

void main(void)
{
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;                                     // submainclock 1mhz
  WDTCTL = WDTPW + WDTHOLD;                                 // Stop WDT

  CCTL0 = CCIE;                                             // CCR0 interrupt enabled
  CCR0 = 1000;                                              // 1ms at 1mhz
  TACTL = TASSEL_2 + MC_1;                                  // SMCLK, upmode

  P1IFG  = 0x00;                                            //clear all interrupt flags
  P1DIR |= BIT0;                                            // P1.0 as output for LED
  P1OUT &= ~BIT0;                                           // turn LED off

  _BIS_SR(GIE);                                             // global interrupt enable

  while(1){
    Ultrason(distance);
  }
}



#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    if(P1IFG&BIT5)                                          //is there interrupt pending?
        {
          if(!(P1IES&BIT5))                                 // is this the rising edge?
          {
            TACTL|=TACLR;                                   // clears timer A
            miliseconds = 0;
            P1IES |= BIT5;                                  //falling edge
          }
          else
          {
            sensor = (long)miliseconds*1000 + (long)TAR;    //calculating ECHO lenght

          }
    P1IFG &= ~BIT5;                                         //clear flag
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  miliseconds++;
}
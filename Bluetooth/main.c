#include <msp430.h> 
#include<capteur_infrarouge.h>
//------------------------------------------------------------------------------
// uart_simple_2.c
//
// test de l'UART : echo des caract�res re�us
//                  (r�ception et envoi par scrutation)
//
// Launchpad 1.5 / 2553 debug model
//------------------------------------------------------------------------------
#include <msp430g2553.h>
#include <string.h>

#define CMDLEN  12               // longueur maximum de la commande utilisateur
#define LF      0x0A
#define CR      0x0D
int LireEtat(int i){
    int var = 0;
    var= var + i;
    if ((var%2)!=0){
        return(0);
    }
    else{
        return(1);
    }
}

void Moteur(int a, int b, int c){
    //int i;
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
        TA1CCR1 = a;         // determine le rapport cyclique du signal while(1);
        TA1CCR2 = b;


        P2OUT   |= (BIT2|BIT4);
        if (c == 1){
            P2OUT &= ~(BIT1);
            P2OUT |= (BIT5);
        }
        else{
            P2OUT &= ~(BIT5);
            P2OUT |= (BIT1);

        }
}

void Init_IO( void )
{
   P1DIR |= BIT0 | BIT6;  // port 1.0  en sortie
   P1OUT &= ~(BIT0 | BIT6);  // force etat bas P1.0 - LED1
   P1REN |= BIT6;
}

void InitUART(void)
{
    P1SEL |= (BIT1 + BIT2);                 // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= (BIT1 + BIT2);                // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;                   // SMCLK
    UCA0BR0 = 104;                          // 1MHz, 9600
    UCA0BR1 = 0;                            // 1MHz, 9600
    UCA0CTL0 &= ~UCPEN & ~UCPAR & ~UCMSB;
    UCA0CTL0 &= ~UC7BIT & ~UCSPB & ~UCMODE1;
    UCA0CTL0 &= ~UCMODE0 & ~UCSYNC;
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
}

void RXdata(unsigned char *c)
{
    while (!(IFG2&UCA0RXIFG));              // buffer Rx USCI_A0 plein ?
    *c = UCA0RXBUF;
}

void TXdata( unsigned char c )
{
    while (!(IFG2&UCA0TXIFG));              // buffer Tx USCI_A0 vide ?
    UCA0TXBUF = c;
}

void Send_STR_UART(const char *msg)
{
    int i = 0;
    for(i=0 ; msg[i] != 0x00 ; i++)
    {
        TXdata(msg[i]);
    }
}

void command( char *cmd )
{
  if(strcmp(cmd, "h") == 0)          // aide
  {
    P1OUT |= BIT6;
    Send_STR_UART("\r\n'8' : avancer :\n");
    Send_STR_UART("\r\n'2' : reculer\n");
    Send_STR_UART("\r\n'4' : tourner à gauche\n");
    Send_STR_UART("\r\n'6' : tourner à droite\n");
    Send_STR_UART("\r\n'5' : s'arreter\n");
    Send_STR_UART("\r\n'h' : affichage de cette aide\n");
  }
  else if (strcmp(cmd, "8") == 0)     // version
  {
    Send_STR_UART("\r avance\n");
    Moteur(1200, 1200, 1);
  }
  else if(strcmp(cmd, "5") == 0)     // allumage led rouge
  {
    Send_STR_UART("\r Arret\n");
    Moteur(0, 0, 1);
  }
  else if(strcmp(cmd, "2") == 0)     // extinction led rouge
  {
      Send_STR_UART("\r recule \n");
      Moteur(1200, 1200, 0);
  }
  else if (strcmp(cmd, "4") == 0)     // version
   {
     Send_STR_UART("\r tourne à gauche\n");
     Moteur(0, 1200, 1);
   }
  else if (strcmp(cmd, "6") == 0)     // version
    {
      Send_STR_UART("\r tourne à droite\n");
      Moteur(1200, 0, 1);
    }
  else
  {
    Send_STR_UART("\rMauvaise commande ");
    Send_STR_UART(cmd);
    Send_STR_UART("\rEntrez 'h' pour l'aide\n");
  }
}

int main(void)
{
  unsigned char c;
  char  cmd[CMDLEN];      // tableau de caractere lie a la commande user
  int   nb_car;           // compteur nombre carateres saisis
  int   g;

    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    // clock calibration verification
    if(CALBC1_1MHZ==0xFF || CALDCO_1MHZ==0xFF)
      __low_power_mode_4();
    // factory calibration parameters
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    Init_IO();
    InitUART();

    nb_car = 0;
    Send_STR_UART("MSP430 Ready !");
    while(1)
    {
        g = capteur_infrarouge();
        if (g == 0){
            if( nb_car<(CMDLEN-1) )
                  {
                    RXdata(&c);
                    if( (cmd[nb_car]=c) != CR )
                    {
                    TXdata(c);
                    nb_car++;
                    LireEtat(1);
                    }
                    else
                    {
                      cmd[nb_car]=0x00;
                      command(cmd);
                      nb_car=0;
                      LireEtat(0);
                    }
                  }
        }
        else{
            Moteur(0 ,0 ,1);
            Send_STR_UART("\r obstacle\n");
        }

    }
}



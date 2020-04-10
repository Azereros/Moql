#include <msp430.h>
#include <ADC.h>
#include <Afficheur.h>
#include <capteur_infrarouge.h>

/*
 * capteur_infrarouge.c
 *
 *  Created on: 24 janv. 2020
 *      Author: 17620384
 */

int capteur_infrarouge(void)
{

    ADC_init();
   //P1SEL=0x00;
   //P1SEL2 = 0x00;
   P2DIR &= ~BIT3;

     int resultat;


       ADC_Demarrer_conversion(0);
              resultat = convert_Hex_Dec(ADC_Lire_resultat());
                 if(resultat>650){
                     return(1);
                 }
                 else{
                     return(0);
                 }

}




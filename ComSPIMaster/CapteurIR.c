/*
 * CapteurIR.c
 *
 *  Created on: 21 mars 2020
 *      Author: MORVA
 */
#include <Afficheur.h>
#include <CapteurIR.h>
#include <msp430.h>
#include <ConvNumAn.h>
#include <Avancer.h>
#include <Arret.h>
#include <CapteurIR.h>

unsigned char CapteurIR(void)
{
    ADC_init();

    P1DIR &= ~BIT0;

    int resultat;

    while(1)
    {
        ADC_Demarrer_conversion(0);
        resultat = convert_Hex_Dec(ADC_Lire_resultat());
        while(resultat<600)
        {
            ADC_init();
            ADC_Demarrer_conversion(0);
            resultat = convert_Hex_Dec(ADC_Lire_resultat());
            return('0');
        }
        return('1');
    }


}



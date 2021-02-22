/*
 * File:   config.c
 * Author: Mauro
 *
 * Created on 24 de junio de 2020, 18:27
 */

#include "xc.h"

void iniTimer(void)
{
    //Configuramos y habilitamos timer
    TMR1 = 0;
    PR1 = 1000;
    T1CONbits.TON = 1;

    // habilito interrupción
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
}

void iniINT0(void)
{
    // habilito interrupción
    IFS0bits.INT0IF = 0;
    IEC0bits.INT0IE = 1;
}

void iniPortb (void)
{
    /*Todos los pines de todos los puertos como digitales*/
    AD1PCFGL= 0XFFFF;
    AD1PCFGH= 0XFFFF;
    AD2PCFGL= 0XFFFF;
    /*---------------------------------------------------*/
    TRISB = 0x0000; // PORTB Como salida para utilizar sus 8 bits más bajos
    PORTB = 0; //Apago los LED´s
}

void config (void)
{
    iniPortb();
    iniTimer();
    iniINT0();
}

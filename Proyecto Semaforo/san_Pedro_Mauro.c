/*
 * File:   san_Pedro_Mauro.c
 * Author: Mauro
 * 
 * Programa 2, Semaforo
 * 
 * Utilizando el Timer 1 (se recomienda el uso de interrupciones), desarrolla un programa que
 * implemente un semáforo con la siguiente lógica temporal: Verde 15 seg; Verde-Amarillo 3 seg; Amarillo 3 seg;
 * Rojo 24 seg; Amarillo-Rojo 3 seg; Amarillo 3 seg; Verde 15 seg; Etc..
 * Para la configuración del timer no es necesario alcanzar ?el segundo? como base de tiempo.
 * Lo que se desea es que el tiempo entre interrupción e interrupción del timer se considere
 * como base de tiempo ?1 segundo?. A partir de esa base de tiempo es posible simular el resto de los tiempos.
 * Existe un botón pulsador (entrada digital, se recomienda el uso de interrupción por INT0),
 * para cruce de no videntes, que a los 10 segundos de ser operado hace que el semáforo
 * recomience su ciclo a partir del inicio de la luz roja (para que permita el cruce).
 * 
 * Created on 24 de junio de 2020, 11:36
 */

#include "xc.h"
#include "config.h"
#include<stdbool.h>

#define INICIAL 0// va a estar casi siempre activa, solo va dejar de ejecutarse al pulsar INT0 y momentaneamente
                          //RB0 | RB1 | RB2 
#define VERDE 4           // 0  |  0  |  1      
#define VERDE_AMARILLO 6  // 0  |  1  |  1 
#define ROJO 1            // 1  |  0  |  0 
#define ROJO_AMARILLO 3   // 1  |  1  |  0 
#define NO_VIDENTES 8     //se activara al presionar INT0, ayudara a cambiar momentaneamente el ciclo
//cada costante limita el tiempo de cada semaforo
#define TIEMPO_VERDE 15  //Tiempo de semaforo en VERDE
#define TIEMPO_AMARILLO 3//Tiempo de semaforo en AMARILLO
#define TIEMPO_ROJO 24   //Tiempo de semaforo en ROJO
#define TIEMPO_NO_VIDENTES 10//Tiempo de semaforo en TIEMPO_NO_VIDENTES

unsigned int contador=0;//contador que ayuda a llevar un control de tiempo en estado VERDE, VERDE_AMARILLO, ROJO y ROJO_AMARILLO
unsigned int contadorNoVidentes=0;//contador que ayuda a llevar un control en el TIEMPO_NO_VIDENTES
unsigned int ciclo = INICIAL;//ciclo = INICIAL, recorrido normal del semaforo; ciclo = NO_VIDENTES, iniciara el sistema para NO_VIDENTES
unsigned int estado = ROJO;// estado = ROJO, semaforo en ROJO; estado = VERDE, semaforo en VERDE; estado = ROJO_AMARILLO, semaforo en ROJO_AMARILLO; estado = VERDE_AMARILLO, semaforo en VERDE_AMARILLO;
bool activaNoVidentes = false;// activaNoVidentes = false, mantiene fijo el contador contadorNoVidentes; activaNoVidentes = true, comienza a contar el contador contadorNoVidentes;

void __attribute__((interrupt, auto_psv)) _T1Interrupt( void )
{
    //Va a interrumpir cada 1 segundo
    IFS0bits.T1IF = 0;      // desactivo flag
    contador++; // aumentara en 1 cada 1 segundo
    if(activaNoVidentes){//SI activaNoVidentes = false, mantiene fijo el contador contadorNoVidentes; SI activaNoVidentes = true, comienza a contar el contador contadorNoVidentes;
        contadorNoVidentes++;//SOLO aumentara en 1 cuando activaNoVidentes sea TRUE (VERDADERO)
    }
}

void __attribute__((interrupt, auto_psv)) _INT0Interrupt( void )
{	
    IFS0bits.INT0IF = 0;    // desactivo flag
    ciclo = NO_VIDENTES;    // Cambiamos el ciclo
    activaNoVidentes = true;// activaNoVidentes = true, comenzara a contar contadorNoVidentes;
    contadorNoVidentes=0; //Se resetea para iniciar desde cero
}


int main(void) {

    config(); //Configuramos timer, interrupciones y puertos 
    
    while(1){        
        if (ciclo == NO_VIDENTES || contadorNoVidentes == TIEMPO_NO_VIDENTES){
            //Solo ingresara a este if cuando ciclo sea igual a NO_VIDENTES o cuando contadorNoVidentes sea igual a TIEMPO_NO_VIDENTES
            if (contadorNoVidentes == TIEMPO_NO_VIDENTES){
                //Si contadorNoVidentes es igual a TIEMPO_NO_VIDENTES
                estado = ROJO; //VOLVEMOS estado a ROJO
                contador=0; //RESETEAMOS contador a cero
                contadorNoVidentes=0;//RESETEAMOS contador a cero
                activaNoVidentes = false; // activaNoVidentes = false, asique deja de contar contadorNoVidentes;
            }else if(ciclo == NO_VIDENTES){
                //Despues de activar el contador, se pasara el ciclo de nuevo a INICIAL para continuar normalmente con el semaforo
                ciclo = INICIAL;
            }
        }else if (ciclo == INICIAL){
            //ciclo normal del semaforo
            switch (estado){
                case ROJO:
                    
                    if(contador < TIEMPO_ROJO){
                        PORTBbits.RB2 = 0; //Apago LED VERDE
                        PORTBbits.RB1 = 0;//Apago LED AMARILLO
                        PORTBbits.RB0 = 1;//Prendo LED ROJO
                    }else if(contador == TIEMPO_ROJO){
                        //Cuando contador llege a TIEMPO_ROJO
                        estado = ROJO_AMARILLO;//Se cambia el estado a ROJO_AMARILLO
                        contador=0;// Se resetea el contador
                    }
                    break;
                    
                case ROJO_AMARILLO:
                    
                    if(contador < TIEMPO_AMARILLO){
                        PORTBbits.RB2 = 0; //Apago LED VERDE
                        PORTBbits.RB1 = 1;//Prendo LED AMARILLO
                        PORTBbits.RB0 = 1;//Prendo LED ROJO
                    }else if(contador == TIEMPO_AMARILLO){
                        //Cuando contador llege a TIEMPO_AMARILLO
                        estado = VERDE;//Se cambia el estado a VERDE
                        contador=0;// Se resetea el contador
                    }
                    break;
                    
                case VERDE:
                        
                    if(contador < TIEMPO_VERDE){
                        PORTBbits.RB2 = 1; //Prendo LED VERDE
                        PORTBbits.RB1 = 0;//Apago LED AMARILLO
                        PORTBbits.RB0 = 0;//Apago LED ROJO
                    }else if(contador == TIEMPO_VERDE){
                        //Cuando contador llege a TIEMPO_VERDE
                        estado = VERDE_AMARILLO;//Se cambia el estado a VERDE_AMARILLO
                        contador=0;// Se resetea el contador
                    }
                    
                    break;
                case VERDE_AMARILLO:
                      
                    if(contador < TIEMPO_AMARILLO){
                        PORTBbits.RB2 = 1; //prendo LED VERDE
                        PORTBbits.RB1 = 1;//prendo LED AMARILLO
                        PORTBbits.RB0 = 0;//apago LED ROJO
                    }else if(contador == TIEMPO_AMARILLO){
                        //Cuando contador llege a TIEMPO_AMARILLO
                        estado = ROJO;//Se cambia el estado a ROJO
                        contador=0;// Se resetea el contador
                    }
                    break;
            }
        }
    }
}


/*
 * planificador.c
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */

#include "planificador.h"

void PLANTask (void *pvParameters)
{
    unsigned short state=0;     //el estado del robot:
                                //0: estado inicial
                                //1: enemigo encontrado
                                //2: borde encontrado
                                //3: post-borde (sin enemigo)
                                //4: post-borde (con enemigo)
                                //5: enemigo detrás
                                //6: enemigo enfrentado

    unsigned short uSwitch=0x0; //registra cada uSwitch para llevar los cambios con cada interrupción

    unsigned short SLineas=0x0; //lleva el estado de los sensores de linea

    unsigned short ADC_state=0; //los estados del ADC:
                                //0: no se detecta nada (fuera de rango)
                                //1: se detecta algo lejos
                                //2: se detecta algo a media distancia
                                //3: se detecta algo a distancia mínima (en cuanto nos acerquemos más volverá a estados anteriores pero seguirá estando cerca)
    while(1)
    {
        static short seq=0;
        static int aux=0x00;
        switch(state)
        {
        case 0:
            if(aux&0x60)                        //sensor de línea activado
            {
                state=2;
                seq=0;
                Msg_PID(-1,0,0,10);//va hacia atrás 10cm
                SLineas=GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1);    //llevar el registro de los sensores de líneas
                if((aux&0x08)||(uSwitch&0x04))//si microswitch trasero activo o se acaba de activar
                {
                    state=5;                    //nos empujan desde atrás
                    Msg_PID(-1,90.0,0,10);      //hacia atrás giro 90º
                }
            }
            else if(aux&0x0E)                   //algún uSwitch activado
            {
                //SLineas=                       -----llevar el registro de los sensores de líneas
                if(aux&0x08)                    //uSwitch trasero activo
                {
                    state=5;                    //nos empujan desde atrás
                    seq=0;
                }
                else                            //uSwitch delantero/s activo
                {
                    state=6;                    //estamos enfrentados contra el enemigo
                    seq=0;
                    Msg_PID(1,0,0,10);         //empuja adelante
                }
            }
            else if(aux&0x10)                   //aviso del ADC
            {
                xQueueReceive(ADC_Plan, &ADC_state,0);
                if(ADC_state>0)
                {
                    state=1;                    //cambiamos al estado enemigo encontrado
                    seq=0;
                    Msg_PID(1,0,10,10);
                }
            }
           else if(aux&01)                     //el PID ha acabado un movimiento
            {
                switch(seq)
                {
                case 0:
                    Msg_PID(1,45.0,0,0);    //medio giro a izquierda
                    seq=1;
                    break;
                case 1:
                    Msg_PID(1,-90.0,0,0);   //giro a derecha
                    seq=2;
                    break;
                case 2:
                    Msg_PID(1,395.0,0,0);   //damo la vuelta completa
                    seq=3;
                    break;
                case 3:
                    Msg_PID(1,0,0,50);      //avanzamos medio metro
                    seq=0;
                    break;
                }
            }
            else
            {
                Msg_PID(1,45.0,0,0);
                seq=1;
            }
            break;
        case 1:
            if (aux&0x60)                   //sensor de línea activado
            {
                state=2;
                Msg_PID(-1,0,0,10);         //retrocede 10cm
                SLineas=GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1);
                if((aux&0x08)||(uSwitch&0x04))//si uSwitch trasero activo o se acaba de activar
                {
                    state=5;
                    Msg_PID(-1,90.0,0,10);      //hacia atrás 90º
                }
            }
            else if (aux&0x06)              //uSwitch delanteros detectados
            {
                state=6;
                Msg_PID(1,0,0,10);          //empuja 10cm al enemigo
            }
            break;
        case 2:
            if(SLineas&GPIO_PIN_1!=0)                    //se activa sensor de línea izquierdo
            {
                Msg_PID(1,-45.0,0,10);         //gira 45 a la derecha y avanza 10 cm

                if(SLineas&GPIO_PIN_0!=0)                //se activa sensor de línea derecho
                {
                Msg_PID(1,45.0,0,10);         //gira 45º a la izquiera y avanza 10cm
                }
            }
            else if(aux&0x0E)               //algún uSwitch activado
            {
                if(aux&0x08)                //uSwitch trasero activo
                 {
                  state=5;                  //nos empujan desde atrás
                  Msg_PID(-1,90.0,0,10);      //hacia atrás 90º
                 }
                 else                       //uSwitch delantero/s activo
                 {
                  state=6;                  //estamos enfrentados contra el enemigo
                  Msg_PID(1,0,0,10);        //empuja adelante
                 }
             }
             else if(aux&0x10)                   //aviso del ADC
             {
                 xQueueReceive(ADC_Plan, &ADC_state,0);
                 if(ADC_state>0)                //si localiza al enemigo
                  {
                    state=1;
                    Msg_PID(1,0,10,0);
                  }
             }
             else if(aux&0x10)                   //aviso del ADC
              {
                  xQueueReceive(ADC_Plan, &ADC_state,0);
                  if(ADC_state>0)
                   {
                     state=4;                    //detecta enemigo después de salir del borde
                     Msg_PID(1,0,0,3);
                    }
                  else
                  {
                     state=3;                    //no detecta enemigo después de salir del borde
                     Msg_PID(-1,395.0,10,0);       //da la vuelta completa en busca del enemigo
                  }
                    }
            break;
        case 3:
            if(aux&0x10)                   //aviso del ADC
             {
              xQueueReceive(ADC_Plan, &ADC_state,0);
              if(ADC_state>0)
              {
               state=1;                    //cambiamos al estado enemigo encontrado
               Msg_PID(1,0,10,10);
              }
              else
              {
               state=0;
              }
              }
            break;
        case 4:
            if(aux&0x60)                        //sensor de línea activado
            {
                state=2;
                Msg_PID(-1,0,0,10);//va hacia atrás 10cm
                SLineas=GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1);    //llevar el registro de los sensores de líneas
                if((aux&0x08)||(uSwitch&0x04))//si microswitch trasero activo o se acaba de activar
                {
                    state=5;                    //nos empujan desde atrás
                    Msg_PID(-1,90.0,0,10);      //hacia atrás giro 90º
                }
            }
            else if(aux&0x0E)                   //algún uSwitch activado
            {
                if(aux&0x08)                    //uSwitch trasero activo
                {
                    state=5;                    //nos empujan desde atrás
                    Msg_PID(-1,90.0,0,10);      //hacia atrás giro 90º
                }
                else                            //uSwitch delantero/s activo
                {
                    state=6;                    //estamos enfrentados contra el enemigo
                    Msg_PID(1,0,0,10);         //empuja adelante
                }
            }
            break;
        case 5:
            if(aux&0x60)                        //sensor de línea activado
            {
                state=2;
                Msg_PID(-1,0,0,10);              //va hacia atrás 10cm
                SLineas=GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1);    //llevar el registro de los sensores de líneas
            }
            else if(aux&0x10)                   //aviso del ADC
            {
                xQueueReceive(ADC_Plan, &ADC_state,0);
                if(ADC_state>0)
                {
                    state=1;                    //cambiamos al estado enemigo encontrado
                    Msg_PID(1,0,10,10);
                 }
             }

            break;
        case 6:
            if(aux&0x60)                        //sensor de línea activado
            {
                state=2;
                Msg_PID(-1,0,0,3);              //va hacia atrás 3cm
                SLineas=GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1);    //llevar el registro de los sensores de líneas
             }
            else if(aux&0x02)                   //uswitch izquierdo
            {
                Msg_PID(1,5.0,10,0);           //intenta ponerse de frente y seguir empujando
            }
            else if (aux&0x04)                  //uswitch derecho
            {
                Msg_PID(1,-5.0,10,0);          //intenta ponerse de frente y seguir empujando
            }
            else if (aux&0x06)                  //uswitch delantertos
            {
                Msg_PID(1,0,10,10);             //enemigo enfrentado sigue empujando
            }

            break;
        }
       aux=xEventGroupWaitBits(Plan,0x00FF,pdTRUE,pdFALSE,configTICK_RATE_HZ*10 );

    }
  }


void PrepPLAN()
{
    Plan = xEventGroupCreate();
    Dist_Plan=xQueueCreate(1, sizeof(uint32_t ));
}

void Msg_PID( short dir, double giro, unsigned short speed, double dist)
{
    mensaje.dir=dir;
    mensaje.dist=dist;
    mensaje.giro=giro;
    mensaje.speed=speed;
    xQueueSend(Plan_PID, &mensaje,0);
    xEventGroupSetBits(Encods,0x0002);
}

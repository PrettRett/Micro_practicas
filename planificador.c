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
        static int aux=0x00;
        switch(state)
        {
        case 0:
            if(aux&0x60)                        //sensor de línea activado
            {
                state=2;
                Msg_PID(-1,0,0,10);//va hacia atrás 10cm
                //SLineas=                                          -----llevar el registro de los sensores de líneas
                if((aux&0x08)||(uSwitch&0x04))//si microswitch trasero activo o se acaba de activar
                {
                    state=5;                    //nos empujan desde atrás
                }
            }
            else if(aux&0x0E)                   //algún uSwitch activado
            {
                //SLineas=                                          -----llevar el registro de los sensores de líneas
                if(aux&0x08)                    //uSwitch trasero activo
                {
                    state=5;                    //nos empujan desde atrás
                }
                else                            //uSwitch delantero/s activo
                {
                    state=6;                    //estamos enfrentados contra el enemigo
                    Msg_PID(1,0,0,10);         //empuja adelante
                }
            }
            else if(aux&0x10)                   //aviso del ADC
            {

            }
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        }
        //Código de prueba de movimiento
        /*switch(state)
        {
        case 0:
            Msg_PID(1,0,0,10);
            state=1;
            break;
        case 1:
            Msg_PID(1,90.0,0,0);
            state=2;
            break;
        case 2:
            Msg_PID(1,0,0,20);
            state=3;
            break;
        case 3:
            Msg_PID(1,90.0,0,0);
            state=4;
            break;
        case 4:
            Msg_PID(1,0,0,10);
            state=5;
            break;
        case 5:
            Msg_PID(1,90.0,0,0);
            state=6;
            break;
        case 6:
            Msg_PID(1,0,0,20);
            state=7;
            break;
        case 7:
            Msg_PID(1,180.0,0,0);
            state=8;
            break;
        default:
            Msg_PID(1,0,0,0);
            break;
        }
        xEventGroupWaitBits(Plan,0x0001,pdTRUE,pdFALSE,portMAX_DELAY );*/

        //Código de prueba de microswitches
        /*static int aux=0;
        switch(aux&0x0E)
          {
          case 0x002:
              Msg_PID(1,0,100);     //delantero izquierdo
              state=1;
              break;
          case 0x004:
              Msg_PID(-1,0,100);    //delantero derecho
              state=2;
              break;
          case 0x008:
              Msg_PID(1,-90.0,0);   //trasero
              state=3;
              break;
          case 0x003:
              Msg_PID(1,90,0);
              state=4;
              break;
          case 0x006:
              Msg_PID(1,180,70);
              state=5;
              break;
          default:
              Msg_PID(1,0,0);
              break;
          }
          aux=xEventGroupWaitBits(Plan,0x000E,pdTRUE,pdFALSE,portMAX_DELAY);*/

        //Código de prueba del ADC por Trigger Timer
        /*xEventGroupWaitBits(Plan,0x010,pdTRUE,pdFALSE,configTICK_RATE_HZ*10 );
        if(ADCMean <= 0xC66 )//si esta mas lejos de 4cm
        {
            if(ADCMean <= 0x707)//si esta mas lejos de 8 cm
            {
                if(ADCMean <= 0x431)//si esta mas lejos de 14 cm
                {
                    if(ADCMean > 0x20c)//si esta entre 14 y 29 cm
                    {
                        Msg_PID(1,-90,0);
                    }
                    else//si esta mas lejos de 29 cm
                    {
                        Msg_PID(1,0,0);
                    }
                }
                else//si esta entre 8 y 14 cm
                {
                    Msg_PID(1,0,0);
                }
            }
            else//si esta entre 4 y 8 cm
            {
                Msg_PID(1,90,0);
            }
        }
        else// si esta más cerca de 4 cm (aunque demasiado cerca empezará a creer que está en otro rango)
        {
            Msg_PID(1,0,0);
        }*/

        //Código de prueba de sensores de línea
        /*static int aux=0;
                switch((aux&0xE0)>>4)
                  {
                  case 0x002:
                      Msg_PID(1,0,100);     //delantero izquierdo
                      state=1;
                      break;
                  case 0x004:
                      Msg_PID(-1,0,100);    //delantero derecho
                      state=2;
                      break;
                  case 0x008:
                      Msg_PID(1,-90.0,0);   //trasero
                      state=3;
                      break;
                  case 0x003:
                      Msg_PID(1,90,0);
                      state=4;
                      break;
                  case 0x006:
                      Msg_PID(1,180,70);
                      state=5;
                      break;
                  default:
                      Msg_PID(1,0,0);
                      break;
                  }
                  aux=xEventGroupWaitBits(Plan,0x0E0,pdTRUE,pdFALSE,portMAX_DELAY);*/
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
    xQueueOverwrite(Plan_PID, &mensaje);
    xEventGroupSetBits(Encods,0x0002);
}

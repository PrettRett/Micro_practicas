/*
 * planificador.c
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */

#include "planificador.h"

void PLANTask (void *pvParameters)
{
    short state=0;
    while(1)
    {
        //Código de prueba de movimiento
        switch(state)
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
        xEventGroupWaitBits(Plan,0x0001,pdTRUE,pdFALSE,portMAX_DELAY );

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
    Plan = xEventGroupCreate();
    Dist_Plan=xQueueCreate(1, sizeof(uint32_t ));
}

void Msg_PID( short dir, double giro, unsigned short speed, double dist)
{
    mensaje.dir=dir;
    mensaje.dist=dist;
    mensaje.giro=giro;
    mensaje.speed=speed;
    xQueueSend(Plan_PID, &mensaje, ( TickType_t )0);
    xEventGroupSetBits(Encods,0x0002);
}

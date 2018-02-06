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
      /*  switch(state)
        {
        case 0:
            Msg_PID(1,-90.0,0);
            state=1;
            break;
        case 1:
            Msg_PID(1,90,0);
            state=2;
            break;
        case 2:
            Msg_PID(1,-90.0,0);
            state=3;
            break;
        case 3:
            Msg_PID(1,-90,90);
            state=4;
            break;
        case 4:
            Msg_PID(1,180,70);
            state=5;
            break;
        default:
            Msg_PID(1,0,0);
            break;
        }
        xEventGroupWaitBits(Plan,0x0001,pdTRUE,pdFALSE,configTICK_RATE_HZ*10 );*/

        //Código de prueba de microswitches
        static int aux=0;
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
          aux=xEventGroupWaitBits(Plan,0x000E,pdTRUE,pdFALSE,portMAX_DELAY);

    }
}


void PrepPLAN()
{
    Plan = xEventGroupCreate();
    Plan = xEventGroupCreate();
    Dist_Plan=xQueueCreate(1, sizeof(uint32_t ));
}

void Msg_PID( short dir, double giro, unsigned short speed)
{
    mensaje.dir=dir;
    mensaje.giro=giro;
    mensaje.speed=speed;
    xQueueSend(Plan_PID, &mensaje, ( TickType_t )0);
    xEventGroupSetBits(Encods,0x0002);
}

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
        switch(state)
        {
        case 0:
            Msg_PID(1,90,100);
            state=1;
            break;
        case 1:
            Msg_PID(1,180,100);
            state=1;
            break;
        case 2:
            Msg_PID(1,-90,255);
            state=1;
            break;
        case 3:
            Msg_PID(1,-180,10);
            state=1;
            break;
        default:
            break;
        }
        xEventGroupWaitBits(Plan,0x0001,pdTRUE,pdFALSE,portMAX_DELAY );
    }
}


void PrepPLAN()
{
    Plan = xEventGroupCreate();
}

void Msg_PID( short dir, float giro, unsigned short speed)
{
    mensaje.dir=dir;
    mensaje.giro=giro;
    mensaje.speed=speed;
    xQueueSend(Plan_PID, &mensaje, ( TickType_t )0);
    xEventGroupSetBits(Encods,0x0002);
}

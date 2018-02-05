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
            Msg_PID(1,-90,100);
            state=4;
            break;
        case 4:
            Msg_PID(1,90,100);
            state=5;
            break;
        default:
            Msg_PID(1,0,0);
            break;
        }
        xEventGroupWaitBits(Plan,0x0001,pdTRUE,pdFALSE,configTICK_RATE_HZ*10 );
    }
}


void PrepPLAN()
{
    Plan = xEventGroupCreate();
}

void Msg_PID( short dir, double giro, unsigned short speed)
{
    mensaje.dir=dir;
    mensaje.giro=giro;
    mensaje.speed=speed;
    xQueueSend(Plan_PID, &mensaje, ( TickType_t )0);
    xEventGroupSetBits(Encods,0x0002);
}

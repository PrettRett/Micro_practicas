/*
 * planificador.h
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h" // Include para poder configurar el pin como salida PWM#include <stdbool.h>
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "inc/hw_nvic.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "stdlib.h"
#include "pid.h"
#include "distancia.h"

void PLANTask (void *pvParameters) ;

void PrepPLAN();

void Msg_PID( short dir, double giro, unsigned short speed, double dist);

struct MenPID
{
    short dir;                  //dirección de la velocidad 1 o -1
    double dist;
    double giro;                 //de -180º a 180º
    unsigned short speed;       //de 0 a 255
} mensaje;
//Variables

EventGroupHandle_t Plan;    //EventGroups que se usa para avisar al planificador de cualquier evento
                            //bit 1: El PID avisa que ha acabado el giro/distancia
                            //bit 2/3/4: Se ha activado la interupcción de algún microswitch
                            //bit 5: Aviso del ADC
                            //bit 6/7/8: Sensores de línea

QueueHandle_t Plan_PID;     //Queue usado para recibir la información del ADC
QueueHandle_t Dist_Plan;     //Queue usado para recibir la información del ADC

#endif /* PLANIFICADOR_H_ */

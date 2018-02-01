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

void PLANTask (void *pvParameters) ;

void PrepPLAN();

EventGroupHandle_t Plan;    //EventGroups que se usa para avisar al planificador de cualquier evento


#endif /* PLANIFICADOR_H_ */

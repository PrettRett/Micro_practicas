/*
 * distancia.h
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */

#ifndef DISTANCIA_H_
#define DISTANCIA_H_

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_adc.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"
#include "driverlib/pin_map.h" // Include para poder configurar el pin como salida PWM#include <stdbool.h>
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "drivers/buttons.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/systick.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "queue.h"
#include "stdlib.h"
#include "planificador.h"


//void DISTTask (void *pvParameters);     //tarea que controla la distancia

void SensoresContacto();

void SensoresLinea();

void SensoresProximidad();

void SensoresContacto_interrupt ();

void SensorProximidad_interrupt ();

void PrepararSensores();

//EventGroupHandle_t ADC;

uint32_t ADCMean;

#endif /* DISTANCIA_H_ */

/*
 * PID.h
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */

#ifndef PID_H_
#define PID_H_

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
#include "FreeRTOS/Source/include/event_groups.h"

#define PERIOD_PWM SysCtlClockGet()/64*0.02
#define STOPCOUNT (PERIOD_PWM)/20*1.528
#define COUNT_1MS PERIOD_PWM/20
#define COUNT_2MS PERIOD_PWM/10
#define NUM_STEPS 50
#define CYCLE_INCREMENTS (abs(COUNT_1MS-COUNT_2MS))/NUM_STEPS

void PIDTask (void *pvParameters);

void Prep_Motores();

void Prep_Encoders();

void Enc_interrupt();

//Variables

EventGroupHandle_t Encods;


#endif /* PID_H_ */

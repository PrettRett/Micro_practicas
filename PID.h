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
#include "inc/hw_nvic.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
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

#define PERIOD_PWM SysCtlClockGet()/64*0.02
#define STOPCOUNT (PERIOD_PWM)/20*1.528
#define COUNT_1MS PERIOD_PWM/20
#define COUNT_2MS PERIOD_PWM/10
#define NUM_STEPS 256
#define CYCLE_INCREMENTS (abs(STOPCOUNT-COUNT_2MS))/NUM_STEPS
#define GET_PWM1 PWMPulseWidthGet(PWM1_BASE, PWM_OUT_6)
#define GET_PWM2 PWMPulseWidthGet(PWM1_BASE, PWM_OUT_7)

#define N_VUELTAS 18.0
#define R_RUEDA 3.0       //centimetros
#define D_RUEDA 8.0       //centimetros
#define GRADOS_INT 360.0/18.0
#define DIST_REC (R_RUEDA*3.14/N_VUELTAS)
#define GRADOS_REC (R_RUEDA/D_RUEDA)*GRADOS_INT


void PIDTask (void *pvParameters);

void Prep_Motores();

void Prep_Encoders();

void Enc_interrupt();

void Prep_PID();

struct MenPID mensaje_PID;
//Variables

EventGroupHandle_t Encods;  //EventGroup de los encoders, también usado para avisar al PID de llegada de mensaje
                            //bit 0:sin usar
                            //bit 1:Mensaje del planificador
                            //bit 2/3:Interrupción de cada encoder
QueueHandle_t Plan_PID;     //Queue por el que se le pasan mensajes al PID de siguiente distancia y giro

#endif /* PID_H_ */

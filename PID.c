/*
 * PID.c
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */
#include "PID.h"

void PIDTask(void *pvParameters)
{

}

void Prep_Motores ()
{
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_PWM1);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN |
                     PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, PERIOD_PWM);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,STOPCOUNT);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,STOPCOUNT);
}

void Prep_Encoders()
{
    // Configuracion de puertos (servos)
    // Habilita puerto GPIOF (servos)
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2);
    GPIOIntTypeSet(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2,GPIO_BOTH_EDGES);

    GPIOIntClear(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2);
    GPIOIntEnable(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2);

    ROM_IntEnable(INT_GPIOA);
    ROM_IntMasterEnable();
}

void Enc_interrupt ()
{

}

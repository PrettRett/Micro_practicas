/*
 * PID.c
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */
#include "PID.h"



void PIDTask(void *pvParameters)
{
    static float dist(0), measure(0),
                speed(0), giro(0);
    while(1)
    {
        EventBits_t flags = xEventGroupWaitBits(Encods,0x000E,
                                                pdTRUE,pdFALSE,
                                                portMAX_DELAY );
        switch(flags)
        {
        case 0x0002:
            xQueueRecieve(Plan_PID, *mensajePID, 0);
            dist=mensajePID.dist;
            giro=mensajePID.giro;
            speed=mensajePID.speed;
            break;
        case 0x0004:

            break;
        case 0x0008:
            break;
        case 0x000C:
            break;
        default:
            break;
        }
    }
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

    //creación del grupo de eventos
    Encods = xEventGroupCreate();

}

void Prep_PID()
{
    Prep_Encoders();
    Prep_Motores();
    Plan_PID=xQueueCreate(1, sizeof(struct MenPID *));
    if(Plan_PID == NULL)
        while(1);  //queueError
}

void Enc_interrupt ()
{
    //enconders en PA2 y PA3
    int pin=GPIOIntStatus(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2);

    BaseType_t xHigherPriorityTaskWoken, xResult;

    xHigherPriorityTaskWoken = pdFALSE;

    //activa en el eventgroups los pines que se han activado
    xResult = xEventGroupSetBitsFromISR(Encods,pin,&xHigherPriorityTaskWoken );

    /* Was the message posted successfully? */
    if( xResult != pdFAIL )
    {
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

    GPIOIntClear(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2);

}

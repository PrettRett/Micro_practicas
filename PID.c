/*
 * PID.c
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */
#include "FreeRTOS.h"
#include "PID.h"
#include "event_groups.h"
#include "portmacro.h"
#include "queue.h"

// R1 o rueda 1, es considerada la rueda derecha
// R2 o rueda 2, es considerada la rueda izquierda

void PIDTask(void *pvParameters)
{
    float measure=0;
    double giro=0;
    double dist=0;
    int prev_measure=0;
    short dir=0;
    unsigned short speed=0;
    while(1)
    {
        EventBits_t flags = xEventGroupWaitBits(Encods,0x000E,
                                                pdTRUE,pdFALSE,
                                                portMAX_DELAY );
        switch(flags)
        {
        case 0x000E:
        case 0x0006:
        case 0x000A:
        case 0x0002:
            xQueueReceive(Plan_PID, &mensaje_PID, 0);
            dir=mensaje_PID.dir;
            dist=mensaje_PID.dist;
            giro=mensaje_PID.giro;
            speed=mensaje_PID.speed;
            if((flags&0x000C)==0)
                break;
        case 0x000C:    //se han activado ambos encoders a la vez
        case 0x0004:    //se ha activado el encoder izquierdo
            if (flags&0x0004)
            {

                if(giro!=0)
                {

                    if((giro<GRADOS_REC/2)&&(giro>(-1*GRADOS_REC)/2))   //Comprobación de que se ha llegado lo más cerca posible del giro que se pretende hacer.
                    {
                        if(dist==0)
                            xEventGroupSetBits(Plan,0x001);//avisamos al planificador que se ha recorrido la distancia/el giro que se quería
                        giro=0;
                    }
                    if(GET_PWM2>STOPCOUNT)
                    {
                        giro+=GRADOS_REC;
                        position.ang+=GRADOS_REC;
                    }
                    else
                    {
                        giro-=GRADOS_REC;
                        position.ang-=GRADOS_REC;
                    }
                }
                if(dist!=0)
                {
                    if((dist<=DIST_REC/2)&&(dist>=(-1*DIST_REC)/2))   //Comprobación de que se ha llegado lo más cerca posible del giro que se pretende hacer.
                    {
                        if(giro==0)
                            xEventGroupSetBits(Plan,0x001);//avisamos al planificador que se ha recorrido la distancia/el giro que se quería
                        dist=0;
                    }
                    if(GET_PWM2>STOPCOUNT)
                    {
                        dist-=DIST_REC;
                    }
                    else
                    {
                        dist+=DIST_REC;
                    }
                }
            }
            if ((flags&0x0008)!=0x0008)
                break;
        case 0x0008:    //se ha activado el encoder derecho
            if(giro!=0)
            {
                if((giro<GRADOS_REC/2)&&(giro>(-1*GRADOS_REC)/2))   //Comprobación de que se ha llegado lo más cerca posible del giro que se pretende hacer.
                {
                    if(dist==0)
                        xEventGroupSetBits(Plan,0x001);//avisamos al planificador que se ha recorrido la distancia/el giro que se quería
                    giro=0;
                }
                if(GET_PWM1>STOPCOUNT)
                {
                    giro+=GRADOS_REC;
                    position.ang+=GRADOS_REC;
                }
                else
                {
                    giro-=GRADOS_REC;
                    position.ang-=GRADOS_REC;
                }
            }
            if(dist!=0)
            {
                if((dist<=DIST_REC/2)&&(dist>=(-1*DIST_REC)/2))   //Comprobación de que se ha llegado lo más cerca posible del giro que se pretende hacer.
                {
                    if(giro==0)
                        xEventGroupSetBits(Plan,0x001);//avisamos al planificador que se ha recorrido la distancia/el giro que se quería
                    dist=0;
                }
                if(GET_PWM1<STOPCOUNT)
                {
                    dist-=DIST_REC;
                }
                else
                {
                    dist+=DIST_REC;
                }
            }
            break;
        default:
            break;
        }
        int frec1, frec2;

        if(dist>0)
        {
            frec1=STOPCOUNT-dir*100*CYCLE_INCREMENTS;
            frec2=STOPCOUNT+dir*100*CYCLE_INCREMENTS;
            //bucle PID
            if(giro>0)
                frec2-=dir*((giro/128)*50+60)*CYCLE_INCREMENTS;
            else if(giro<0)
                frec1+=dir*((giro/128)*50+60)*CYCLE_INCREMENTS;
        }
        else if(speed>0) //hay avance
        {
            frec1=STOPCOUNT-dir*speed*CYCLE_INCREMENTS;
            frec2=STOPCOUNT+dir*speed*CYCLE_INCREMENTS;
            //bucle PID
            if(giro>0)
                frec2-=dir*((giro/128)*50+60)*CYCLE_INCREMENTS;
            else if(giro<0)
                frec1+=dir*((giro/128)*50+60)*CYCLE_INCREMENTS;

        }
        else if(giro>0) //giro sin avance
        {
            frec1=STOPCOUNT-128*CYCLE_INCREMENTS;
            frec2=STOPCOUNT-128*CYCLE_INCREMENTS;
        }
        else if(giro<0) //giro sin avance
        {
            frec1=STOPCOUNT+128*CYCLE_INCREMENTS;
            frec2=STOPCOUNT+128*CYCLE_INCREMENTS;
        }
        else
        {
            frec1=STOPCOUNT;
            frec2=STOPCOUNT;
        }
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, frec1 );
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, frec2 );

    }
}

void Prep_Motores ()
{
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN |
                     PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, PERIOD_PWM);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,STOPCOUNT);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,STOPCOUNT);
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

}

void Prep_Encoders()
{
    // Configuracion de puertos (servos)
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2);
    GPIOIntTypeSet(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2,GPIO_BOTH_EDGES);

    GPIOIntClear(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2);
    GPIOIntEnable(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2);

    IntEnable(INT_GPIOA);
    IntMasterEnable();

    /*//TIMER usado para el calculo velocidad           //no usado
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER0);
    // Configura el Timer0 para cuenta periodica de 32 bits (no lo separa en TIMER0A y TIMER0B)
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // Periodo de cuenta de 0.05s. SysCtlClockGet() te proporciona la frecuencia del reloj del sistema, por lo que una cuenta
    // del Timer a SysCtlClockGet() tardara 1 segundo, a 0.5*SysCtlClockGet(), 0.5seg, etc...
    // Carga la cuenta en el Timer0A
    TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF);
    TimerEnable(TIMER0_BASE, TIMER_A);*/

    //creación del grupo de eventos
    Encods = xEventGroupCreate();

}

void Prep_PID()
{
    Prep_Encoders();
    Prep_Motores();
    Plan_PID=xQueueCreate(1, sizeof(struct MenPID ));
    if(Plan_PID == NULL)
        while(1);  //queueError
}

void Enc_interrupt ()
{
    //enconders en PA2 y PA3
    EventBits_t pin=GPIOIntStatus(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2);

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

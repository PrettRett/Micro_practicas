/*
 * distancia.c
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */

#include "distancia.h"

void DISTTask (void *pvParameters)
{
    unsigned short prev_value=0;
    unsigned short nuevo_value=0;
    int valor_h=0x230;
    while(1)
    {
        xEventGroupWaitBits(ADC,1,pdTRUE,pdFALSE,portMAX_DELAY);
        if(ADCMean <= 0xC66 )//si esta mas lejos de 4cm
                {
                    if(ADCMean <= 0x707)//si esta mas lejos de 8 cm
                    {
                        if(ADCMean <= 0x431)//si esta mas lejos de 14 cm
                        {
                            if(ADCMean > valor_h)//si esta entre 14 y 29 cm
                            {
                                nuevo_value=1;
                            }
                            else//si esta mas lejos de 29 cm
                            {
                                nuevo_value=0;
                            }
                        }
                        else//si esta entre 8 y 14 cm
                        {
                            nuevo_value=2;
                        }
                    }
                    else//si esta entre 4 y 8 cm
                    {
                        nuevo_value=3;
                    }
                }
                else// si esta más cerca de 4 cm (aunque demasiado cerca empezará a creer que está en otro rango)
                {
                    nuevo_value=1;
                }
        if(prev_value!=nuevo_value)
        {
            xQueueSend(ADC_Plan,&nuevo_value,0);
            xEventGroupSetBits(Plan,0x010);
        }
        prev_value=nuevo_value;
        if(nuevo_value==0)
        {
            valor_h=0x235;
        }
        else if(nuevo_value>0)
        {
            valor_h=0x211;
        }
    }
}

void SensoresContacto()
{
    // Configuracion de puertoE para micro switch//Boton PE0/1/2---------------------------------
    ROM_GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2,GPIO_BOTH_EDGES);
    GPIOIntClear(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
    GPIOIntEnable(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
    //-------------------------------------------

    ROM_IntEnable(INT_GPIOE);

    //creación del grupo de eventos
    //Contacto = xEventGroupCreate();
}

void SensoresLinea()
{
    // Configuracion de puertoB para sensores linea
        ROM_GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_2, GPIO_DIR_MODE_IN);
      GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_2);
      GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_2,GPIO_BOTH_EDGES);

      GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_2);
      GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_2);

      ROM_IntEnable(INT_GPIOD);

      //creación del grupo de eventos
      //Linea = xEventGroupCreate();
}

void SensoresProximidad()
{

    //TIMER usado para el disparo de ADC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER1);
    // Configura el Timer0 para cuenta periodica de 32 bits (no lo separa en TIMER0A y TIMER0B)
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    // Periodo de cuenta de 0.05s. SysCtlClockGet() te proporciona la frecuencia del reloj del sistema, por lo que una cuenta
    // del Timer a SysCtlClockGet() tardara 1 segundo, a 0.5*SysCtlClockGet(), 0.5seg, etc...
    // Carga la cuenta en el Timer0A
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet()*0.1);

    //ADC
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
    ADCSequenceDisable(ADC0_BASE, 1); // Deshabilita el secuenciador 1 del ADC0 para su configuracion
    TimerControlTrigger(TIMER1_BASE, TIMER_A,1);
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_TIMER, 0);
    // Configuramos los 4 conversores del secuenciador 1 para muestreo del sensor de temperatura
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH7);
    // El conversor 4 es el ultimo, y  se genera un aviso de interrupcion
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH7 | ADC_CTL_IE | ADC_CTL_END);


    ADCSequenceEnable(ADC0_BASE, 1); // habilita el secuenciador 1 del ADC0 para su configuracion
    ADCIntClear(ADC0_BASE, 1);
    ADCIntEnable(ADC0_BASE, 1);
    IntEnable(INT_ADC0SS1);
    TimerEnable(TIMER1_BASE, TIMER_A);

    ADCMean=0x0;

    ADC=xEventGroupCreate();

}

void PrepararSensores()
{
    SensoresContacto();
    SensoresLinea();
    SensoresProximidad();
    ADC_Plan=xQueueCreate(1, sizeof(unsigned short));
}

void CalculoDistancia()
{

}

void SensoresContacto_interrupt ()
{

    int pin=GPIOIntStatus(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);

    BaseType_t xHigherPriorityTaskWoken, xResult;

    xHigherPriorityTaskWoken = pdFALSE;

    //activa en el eventgroups los pines que se han activado
    xResult = xEventGroupSetBitsFromISR(Plan,(pin<<1),&xHigherPriorityTaskWoken );

    /* Was the message posted successfully? */
    if( xResult != pdFAIL )
    {
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

    GPIOIntClear(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);

}

void SensoresLinea_interrupt ()
{

    int pin=GPIOIntStatus(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);

    BaseType_t xHigherPriorityTaskWoken, xResult;

    xHigherPriorityTaskWoken = pdFALSE;

    //activa en el eventgroups los pines que se han activado
    xResult = xEventGroupSetBitsFromISR(Plan,(pin<<4),&xHigherPriorityTaskWoken );

    /* Was the message posted successfully? */
    if( xResult != pdFAIL )
    {
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

    GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_1|GPIO_PIN_2);

}

void SensorProximidad_interrupt ()
{
    uint32_t ADCValue[4];
    ADCSequenceDataGet(ADC0_BASE,1,ADCValue);
    ADCMean=(ADCValue[3]+ADCValue[2]+ADCValue[1]+ADCValue[0])/4;

    BaseType_t xHigherPriorityTaskWoken, xResult;

    xHigherPriorityTaskWoken = pdFALSE;

    //activa en el eventgroups los pines que se han activado
    xResult = xEventGroupSetBitsFromISR(ADC,1,&xHigherPriorityTaskWoken );

    /* Was the message posted successfully? */
    if( xResult != pdFAIL )
    {
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

    ADCIntClear(ADC0_BASE, 1);
}

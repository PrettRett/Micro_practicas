/*
 * distancia.c
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */

#include "distancia.h"

void DISTTask (void *pvParameters)
{
    
}

void SensoresContacto()
{
    // Configuracion de puertoE para micro switch
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
    GPIOIntTypeSet(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2,GPIO_BOTH_EDGES);

    GPIOIntClear(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
    GPIOIntEnable(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);

    ROM_IntEnable(INT_GPIOE);
    ROM_IntMasterEnable();

    //creación del grupo de eventos
    Contacto = xEventGroupCreate();
    }

void SensoresLinea()
{
    // Configuracion de puertoB para sensores linea
      GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
      GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2,GPIO_BOTH_EDGES);

      GPIOIntClear(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
      GPIOIntEnable(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);

      ROM_IntEnable(INT_GPIOB);
      ROM_IntMasterEnable();

      //creación del grupo de eventos
      Linea = xEventGroupCreate();
      
}
//preparar funcion para enviar la informacion de la linea

void SensoresProximidad()
{

}

void SensoresContacto_interrupt ()
{

    int pin=GPIOIntStatus(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);

    BaseType_t xHigherPriorityTaskWoken, xResult;

    xHigherPriorityTaskWoken = pdFALSE;

    //activa en el eventgroups los pines que se han activado
    xResult = xEventGroupSetBitsFromISR(Contacto,pin,&xHigherPriorityTaskWoken );

    /* Was the message posted successfully? */
    if( xResult != pdFAIL )
    {
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }

    GPIOIntClear(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);

}
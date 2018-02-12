/*
 * planificador.c
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */

#include "planificador.h"

void PLANTask (void *pvParameters)
{
    unsigned short state=0;     //el estado del robot:
                                //0: estado inicial
                                //1: enemigo encontrado
                                //2: borde encontrado
                                //3: post-borde (sin enemigo)
                                //4: post-borde (con enemigo)
                                //5: enemigo detr�s
                                //6: enemigo enfrentado

    unsigned short uSwitch=0x0; //registra cada uSwitch para llevar los cambios con cada interrupci�n

    unsigned short SLineas=0x0; //lleva el estado de los sensores de linea

    unsigned short ADC_state=0; //los estados del ADC:
                                //0: no se detecta nada (fuera de rango)
                                //1: se detecta algo lejos
                                //2: se detecta algo a media distancia
                                //3: se detecta algo a distancia m�nima (en cuanto nos acerquemos m�s volver� a estados anteriores pero seguir� estando cerca)
    vTaskDelay(configTICK_RATE_HZ*5);
    while(1)
    {
        static short seq=0;
        static int aux=0x00;
        switch(state)
        {
        case 0:
            if(aux&0x60)                        //sensor de l�nea activado
            {
                SLineas=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);    //llevar el registro de los sensores de l�neas
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                if(SLineas)
                {
                    state=2;                         //nos encontramos el borde
                    seq=0;
                    Msg_PID(-1,0,0,10);              //va hacia atr�s 10cm
                    if(uSwitch&0x04==0)//si encontramos la linea y nos empujan desde atras
                    {
                        state=5;
                        Msg_PID(-1,90.0,0,0);      //hacia atr�s 90�
                    }
                }
            }
            else if(aux&0x0E)                   //alg�n uSwitch activado
            {
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                if(uSwitch&0x04==0)                    //uSwitch trasero activo
                {
                    state=5;                    //nos empujan desde atr�s
                    seq=0;
                    Msg_PID(1,-90.0,0,0);
                }
                else                            //uSwitch delantero/s activo
                {
                    state=6;
                    if((uSwitch&0x03)==0)
                    {
                        Msg_PID(1,0,100,0);//empujamos
                        seq=0;
                    }
                    else if((uSwitch&0x01)==0)
                    {
                        Msg_PID(1,180.0,60,0);
                        seq=1;
                    }
                    else if((uSwitch&0x02)==0)
                    {
                        Msg_PID(1,-180.0,60,0);
                        seq=0;
                    }
                    else//no deberia entrar
                    {
                        state=0;
                    }
                }
            }
            else if(aux&0x10)                   //aviso del ADC
            {
                xQueueReceive(ADC_Plan, &ADC_state,0);
                if(ADC_state>0)
                {
                    state=1;                    //cambiamos al estado enemigo encontrado
                    seq=0;

                    Msg_PID(1,0,0,10);
                }
            }
           else if(aux&01)                     //el PID ha acabado un movimiento
            {
                switch(seq)
                {
                case 0:
                    Msg_PID(1,45.0,0,0);    //medio giro a izquierda
                    seq=1;
                    break;
                case 1:
                    Msg_PID(1,-90.0,0,0);   //giro a derecha
                    seq=2;
                    break;
                case 2:
                    Msg_PID(1,405.0,0,0);   //damo la vuelta completa
                    seq=3;
                    break;
                case 3:
                    Msg_PID(1,0,0,15);      //avanzamos medio metro
                    seq=0;
                    break;
                }
            }
            else
            {
                Msg_PID(1,45.0,0,0);
                seq=1;
            }
            break;
        case 1://enemigo encontrado
            if (aux&0x60)                   //sensor de l�nea activado
            {
                SLineas=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);    //llevar el registro de los sensores de l�neas
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                if(SLineas)
                {
                    state=2;                         //nos encontramos el borde
                    Msg_PID(-1,0,0,10);              //va hacia atr�s 10cm
                    if(uSwitch&0x04==0)//si encontramos la linea y nos empujan desde atras
                    {
                        state=5;
                        Msg_PID(-1,90.0,0,0);      //hacia atr�s 90�
                    }
                }

            }
            else if (aux&0x0E)              //uSwitch delanteros detectados
            {
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                if((uSwitch&0x03)==0)
                {
                    state=6;
                    Msg_PID(1,0,100,0);//empujamos
                }
                else if((uSwitch&0x01)==0)
                {
                    state=6;
                    Msg_PID(1,180.0,60,0);
                    seq=1;
                }
                else if((uSwitch&0x02)==0)
                {
                    state=6;
                    Msg_PID(1,-180.0,60,0);
                    seq=0;
                }
                else
                {
                    if(ADC_state>0)
                    {
                        state=1;
                        Msg_PID(1,0,0,10);
                    }
                }
            }
            else if(aux&0x10)                   //aviso del ADC
            {
                xQueueReceive(ADC_Plan, &ADC_state,0);
                if(ADC_state==0)
                {
                    state=0;                    //cambiamos al estado enemigo encontrado
                    seq=1;
                    Msg_PID(1,45.0,0,0);
                }
            }
            else if (aux&0x01)              //PID acabado
            {
                Msg_PID(1,0,100,0);
            }
            break;
        case 2://borde encontrado
            if(aux&0x60)                        //sensor de l�nea activado
            {
                SLineas=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);
                if((SLineas&GPIO_PIN_1)&&(SLineas&GPIO_PIN_2))
                {
                    Msg_PID(-1,0,0,10);
                }
                else if(SLineas&GPIO_PIN_2)                    //se activa sensor de l�nea izquierdo
                {
                    Msg_PID(-1,0.0,0,10);         //gira 45 a la derecha y avanza 10 cm
                }
                else if(SLineas&GPIO_PIN_1)                //se activa sensor de l�nea derecho
                {
                    Msg_PID(-1,0,0,10);         //gira 45� a la izquiera y avanza 10cm
                }
                else
                {
                    uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                    if((uSwitch&GPIO_PIN_0==0)||(uSwitch&GPIO_PIN_1==0))
                    {
                        state=4;
                        Msg_PID(1,0,0,5);
                        seq=1;
                    }
                    else if(uSwitch&GPIO_PIN_2==0)
                    {
                        state=5;
                        Msg_PID(-1,180.0,100,0);
                    }
                    else
                    {
                        if(ADC_state==0)
                        {
                            state=3;
                            if(aux&0x020)
                                Msg_PID(1,180.0,0,0);
                            if(aux&0x040)
                                Msg_PID(1,-180.0,0,0);
                            seq=1;
                        }
                        else
                        {
                            state=4;                  //estamos enfrentados contra el enemigo
                            Msg_PID(1,0,0,5);        //empuja adelante
                            seq=1;
                        }
                    }
                }
            }
            else if(aux&0x0E)               //alg�n uSwitch activado
            {
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                if(uSwitch&0x04==0)               //uSwitch trasero activo
                {
                    Msg_PID(-1,-180.0,0,10);      //hacia atr�s 90�
                    SLineas=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);
                    if(SLineas&GPIO_PIN_1)                    //se activa sensor de l�nea izquierdo
                    {
                        Msg_PID(-1,-180.0,0,10);         //gira 45 a la derecha y avanza 10 cm
                    }
                    else if(SLineas&GPIO_PIN_0)                //se activa sensor de l�nea derecho
                    {
                        Msg_PID(-1,180.0,0,10);         //gira 45� a la izquiera y avanza 10cm
                    }
                    else if((SLineas&GPIO_PIN_0)&&(SLineas&GPIO_PIN_1))
                    {
                        Msg_PID(-1,180.0,0,10);
                    }

                 }
                 else if((uSwitch&0x01==0)||(uSwitch&0x02==0))                    //uSwitch delantero/s activo
                 {
                     state=4;                  //estamos enfrentados contra el enemigo
                     Msg_PID(1,0,0,5);        //empuja adelante
                     seq=1;
                 }
             }
             else if(aux&0x10)                   //aviso del ADC
             {
                 xQueueReceive(ADC_Plan, &ADC_state,0);
                 if(ADC_state>0)                //si localiza al enemigo
                 {
                    state=4;                  //estamos enfrentados contra el enemigo
                    Msg_PID(1,0,0,5);        //empuja adelante
                    seq=1;
                 }
             }
             else if(aux&0x01)
             {
                 Msg_PID(-1,0,0,10);
             }
            break;
        case 3://post-borde sin enemigo
            if(aux&0x60)                        //sensor de l�nea activado
            {
                SLineas=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);    //llevar el registro de los sensores de l�neas
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);if(SLineas)
                {
                    state=2;                         //nos encontramos el borde
                    seq=0;
                    Msg_PID(-1,0,0,10);              //va hacia atr�s 10cm
                    if(uSwitch&0x04==0)//si encontramos la linea y nos empujan desde atras
                    {
                        state=5;
                        seq=0;
                        Msg_PID(-1,90.0,0,0);      //hacia atr�s 90�
                    }
                }
            }
            else if(aux&0x0E)                   //alg�n uSwitch activado
            {
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                if((uSwitch&0x04==0))                    //uSwitch trasero activo
                {
                    state=5;                    //nos empujan desde atr�s
                    seq=0;
                }
                else                            //uSwitch delantero/s activo
                {
                    state=6;                    //estamos enfrentados contra el enemigo
                    if((uSwitch&0x03)==0)
                    {
                        Msg_PID(1,0,100,0);//empujamos
                        seq=0;
                    }
                    else if((uSwitch&0x01)==0)
                    {
                        Msg_PID(1,180.0,60,0);
                        seq=1;
                    }
                    else if((uSwitch&0x02)==0)
                    {
                        Msg_PID(1,-180.0,60,0);
                        seq=0;
                    }
                    else//no deberia de entrar aqui
                    {
                        state=3;
                    }
                }
            }
            else if(aux&0x10)                   //aviso del ADC
            {
                xQueueReceive(ADC_Plan, &ADC_state,0);
                if(ADC_state>0)
                {
                    state=1;                    //cambiamos al estado enemigo encontrado
                    seq=0;
                    Msg_PID(1,0,0,10);
                }
            }
           else if(aux&01)                     //el PID ha acabado un movimiento
            {
                switch(seq)
                {
                case 0:
                    Msg_PID(1,180.0,0,0);    //giro completo
                    seq=1;
                    break;
                case 1:
                    Msg_PID(1,0.0,0,30);   //avanzamos al centro
                    seq=2;
                    break;
                case 2:
                    Msg_PID(1,540.0,0,0);   //damos la vuelta y media
                    seq=3;
                    break;
                case 3:
                    Msg_PID(1,0,0,16);      //avanzamos medio tatami
                    seq=4;
                    break;
                case 4:
                    Msg_PID(1,90.0,0,0);      //no giramos al lateral
                    seq=5;
                    break;
                case 5:
                    Msg_PID(1,0,0,20);      //avanzamos medio metro
                    seq=6;
                    break;
                case 6:
                    Msg_PID(1,90.0,0,0);      //no giramos al lateral
                    seq=7;
                    break;
                case 7:
                    Msg_PID(1,0,0,20);      //no giramos al lateral
                    seq=8;
                    break;
                case 8:
                    Msg_PID(1,90.0,0,0);      //no giramos al lateral
                    seq=9;
                    break;
                case 9:
                    Msg_PID(1,0,0,20);      //no giramos al lateral
                    seq=0;
                    state=0;                 //debido a posible error de PID ya no sabemos estadisticamente donde puede estar, pasamos al estado inicial
                    break;

                }
            }
            else
            {
                Msg_PID(1,45.0,0,0);
                seq=1;
            }
            break;
        case 4://post-borde enemigo
            if(aux&0x60)                        //sensor de l�nea activado
            {
                SLineas=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);
                if(SLineas==0)
                {
                    if(seq==0)
                    {
                        Msg_PID(1,0.0,0,2);    //empujamos
                        seq=1;
                    }
                }
                if(SLineas)
                {
                    state=2;

                }
            }
            else if(aux&0x0E)                   //alg�n uSwitch activado
            {
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                SLineas=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);
                if(uSwitch&0x04==0)                    //uSwitch trasero activo
                {
                    state=5;                    //nos empujan desde atr�s
                    Msg_PID(-1,180.0,0,10);      //hacia atr�s giro 90�
                }
            }
            else if(aux&10)                     //el ADC nos avisa
            {
                xQueueReceive(ADC_Plan, &ADC_state,0);
                if(ADC_state==0)
                {
                    state=0;                    //cambiamos al estado enemigo encontrado
                    seq=0;
                    Msg_PID(1,180.0,0,0);
                }
            }
            else if(aux&01)                     //el PID ha acabado un movimiento
            {
                switch(seq)
                {
                case 0:
                    Msg_PID(1,0.0,0,3);    //empujamos
                    seq=1;
                    break;
                case 1:
                    Msg_PID(-1,0.0,0,20);   //retrocedemos
                    seq=0;
                    state=0;
                    break;
                }
            }
            break;
        case 5://Nos dan por detras
            if(aux&0x60)                        //sensor de l�nea activado
            {
                SLineas=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);    //llevar el registro de los sensores de l�neas
                if(SLineas)
                {
                    state=2;                         //nos encontramos el borde
                    Msg_PID(-1,0,0,10);              //va hacia atr�s 10cm
                }
            }
            else if(aux&10)                     //el ADC nos avisa
            {
                xQueueReceive(ADC_Plan, &ADC_state,0);
            }
            else if(aux&0x0E)                   //alg�n uSwitch activado
            {
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                if(uSwitch&0x04)
                {//hemos triunfao
                    state=0;
                    seq=2;
                    Msg_PID(1,395.0,0,0);
                }
            }
            else if(aux&01)                     //el PID ha acabado un movimiento
            {
                Msg_PID(1,180.0,0,10);
            }

            break;
        case 6://enemigo enfrentado
            if(aux&0x60)                        //sensor de l�nea activado
            {
                SLineas=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1|GPIO_PIN_2);    //llevar el registro de los sensores de l�neas
                if(SLineas)
                {
                    state=4;                         //nos encontramos el borde
                    Msg_PID(1,0,0,2);              //va hacia atr�s 10cm
                }
             }
            else if(aux&0x0E)                   //uswitch
            {
                uSwitch=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
                if((uSwitch&0x03)==0)
                {
                    Msg_PID(1,0,100,0);//empujamos
                }
                else if((uSwitch&0x01)==0)
                {
                    Msg_PID(1,90.0,60,0);
                    seq=1;
                }
                else if((uSwitch&0x02)==0)
                {
                    Msg_PID(1,-90.0,60,0);
                    seq=0;
                }
                else
                {
                    if(ADC_state>0)
                    {
                        state=1;
                        Msg_PID(1,0,0,10);
                    }
                    else
                    {
                        state=0;
                        seq=1;
                        Msg_PID(1,45.0,0,0);
                    }
                }
            }
            else if(aux&10)                     //el ADC nos avisa
            {
                xQueueReceive(ADC_Plan, &ADC_state,0);
            }
            else if(aux&0x0E)                   //PID
            {
                switch(seq)
                {
                case 0:
                    Msg_PID(1,-20.0,100,0);    //empujamos
                    seq=1;
                    break;
                case 1:
                    Msg_PID(1,20.0,100,0);   //retrocedemos
                    seq=0;
                    break;
                }
            }
            else
            {
                Msg_PID(1,0,100,0);
            }

            break;
        }
       aux=xEventGroupWaitBits(Plan,0x00FF,pdTRUE,pdFALSE,configTICK_RATE_HZ*10 );

    }
  }


void PrepPLAN()
{
    Plan = xEventGroupCreate();
    Dist_Plan=xQueueCreate(1, sizeof(uint32_t ));
}

void Msg_PID( short dir, double giro, unsigned short speed, double dist)
{
    mensaje.dir=dir;
    mensaje.dist=dist;
    mensaje.giro=giro;
    mensaje.speed=speed;
    xQueueSend(Plan_PID, &mensaje,0);
    xEventGroupSetBits(Encods,0x0002);
}

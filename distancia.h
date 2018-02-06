/*
 * distancia.h
 *
 *  Created on: 21 dic. 2017
 *      Author: Alumno
 */

#ifndef DISTANCIA_H_
#define DISTANCIA_H_


void DISTTask (void *pvParameters);     //tarea que controla la distancia

void SensoresContacto();

void SensoresLinea();

void SensoresProximidad();

void SensoresContacto_interrupt ();

EventGroupHandle_t Contacto_Linea;

#endif /* DISTANCIA_H_ */

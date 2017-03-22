/* 
 * File:   Tennis.h
 * Author: Fano
 *
 * Created on 3 de marzo de 2017, 0:52
 */

#ifndef TENNIS_H
#define	TENNIS_H
#include "HALCore/HALCore.h"

void __attribute__((section(".funcdin"))) reset();

void __attribute__((section(".funcdin"))) game();

void __attribute__((section(".funcdin"))) menu();

void __attribute__((section(".funcdin"))) Tennis();

#endif	/* TENNIS_H */


/*
 * lab1math.h
 *
 *  Created on: Dec. 29, 2022
 *      Author: SLsec
 */

#ifndef INC_LAB1MATH_H_
#define INC_LAB1MATH_H_
#include "main.h"

extern void asmMax(float *arrat, uint32_t size, float *max, uint32_t *maxIndex);
extern float* asmMul(float *a, float *b, uint_t size);
void cMax(float *array, uint32_t size, float *max, uint32_t *maxIndex);
float* arrayMul(float *a, float *b, uint_t size);
#endif /* INC_LAB1MATH_H_ */


/*
 * cmax.c
 *
 *  Created on: Dec. 29, 2022
 *      Author: SLsec
 */
#include "main.h"

float* arrayMul(float *a, float *b, uint_t size){
	float y[size];
	for(uint32_t i =0; i<size; i++){
		y[i] = a[i]*b[i];
	}
	return *y;
}

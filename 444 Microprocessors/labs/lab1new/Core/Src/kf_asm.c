/*
 * kf_asm.c
 *
 *  Created on: Feb. 4, 2023
 *      Author: SLsec
 */
#include "utility.h"

extern int kalmanfilter_asm2(kstate* ks, float meas);

int kf_asm(float* InputArray, float* OutputArray, kstate* ks, uint32_t length){
	for(uint32_t i=0; i<length; i++){
		float measurement = InputArray[i];
		int error = kalmanfilter_asm2(&ks, measurement); //error code returned by the assembly function
		OutputArray[i] = ks->x;
		if(error<0){ //overflow:-1, underflow:-2, div by 0: -3
			return -1;
		}
	}
	return 0;
}


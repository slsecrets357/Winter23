/*
 * Kalmanfilter_DSP.c
 *
 *  Created on: Jan. 22, 2023
 *      Author: SLsec
 */
#include "main.h"
#include "arm_math.h"
#include "utility.h"
#define ARM_MATH_CM4

int kalmanfilter_DSP(float *InputArray, float *OutputArray, kstate* ks, uint32_t Length){
	//Inside the Kalmanfilter function, use the CMSIS-DSP functions to perform the necessary
	//calculations for the Kalman filter algorithm.
	float32_t p[1];
	float32_t q[1];
	float32_t r[1];
	float32_t x[1];
	float32_t k[1];
	float32_t measurement[1];
	for(int i = 0; i < Length; i++){
		measurement[0] = InputArray[i];
		p[0] = ks->p;
		q[0] = ks->q;
		r[0] = ks->r;
		x[0] = ks->x;
		k[0] = ks->k;
		arm_add_f32(p,q,p,1);
		arm_add_f32(p,r,k,1);
		k[0]= p[0]/k[0];
		ks->k = k[0];
		arm_sub_f32(measurement,x,q,1);
		arm_mult_f32(k,q,q,1);
		arm_add_f32(x,q,x,1);
		ks->x = x[0];
		float32_t a[1] = {1};
		arm_sub_f32(a,k,k,1);
		arm_mult_f32(k,p,p,1);
		ks->p = p[0];
		OutputArray[i] = ks->x;
	}
	return 0;

}


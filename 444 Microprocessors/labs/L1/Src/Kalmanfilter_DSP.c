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
	  for (int i = 0; i < Length; i++) {
		// Read the next measurement from the input array
	    float measurement = InputArray[i];

	    // Update the prediction error covariance (p)
	    float32_t p_temp;
	    arm_add_f32(&(ks->p), &(ks->q), &p_temp, 1); // Add ks->q to ks->p and store the result in p_temp
	    ks->p = p_temp;

	    // Calculate the Kalman gain (k)
	    float32_t k_temp;
	    k_temp= ks->p/(ks->r+ks->p);
	    //arm_divide_f32(&(ks->p), &p_temp, &(ks->r), &k_temp); // Divide ks->p by ks->r and store the result in k_temp
	    ks->k = k_temp;

	    // Update the estimate of the state (x)
	    float32_t x_temp;
	    float32_t measurement_minus_x;
	    arm_sub_f32(&measurement, &(ks->x), &measurement_minus_x, 1); // Subtract ks->x from measurement and store the result in measurement_minus_x
	    float32_t k_times_measurement_minus_x;
	    arm_mult_f32(&(ks->k), &measurement_minus_x, &k_times_measurement_minus_x, 1); // Multiply ks->k and measurement_minus_x and store the result in k_times_measurement_minus_x
	    arm_add_f32(&(ks->x), &k_times_measurement_minus_x, &x_temp, 1);// Add ks->x and k_times_measurement_minus_x and store the result in x_temp
	    ks->x = x_temp;

	    // Update the prediction error covariance (p)
	    float32_t one_minus_k;
	    float32_t one[1] = {1};
	    arm_sub_f32(one, &(ks->k), &one_minus_k, 1); // Subtract ks->k from 1
	    float32_t p_temp2;
	    arm_mult_f32(&one_minus_k, &(ks->p), &p_temp2, 1); // Multiply one_minus_k and ks->p and store the result in p_temp2
	    ks->p = p_temp2;

	    // Write the updated estimate of the state (x) to the output array
	    OutputArray[i] = ks->x;

	    // Check if the output value is NaN (Not-a-Number) or INF (Infinite) and return 1 if either of these is true
	    if (isnan(OutputArray[i]) || isinf(OutputArray[i])) {
	      return 1;
	    }
	  }
	  return 0;
	}

	//int kalmanfilter_DSP(float *InputArray, float *OutputArray, kstate* ks, uint32_t Length){
	//Inside the Kalmanfilter function, use the CMSIS-DSP functions to perform the necessary
	//calculations for the Kalman filter algorithm.
//	float32_t p[1];
//	float32_t q[1];
//	float32_t r[1];
//	float32_t x[1];
//	float32_t k[1];
//	float32_t measurement[1];
//	for(int i = 0; i < Length; i++){
//		measurement[0] = InputArray[i];
//		p[0] = ks->p;
//		q[0] = ks->q;
//		r[0] = ks->r;
//		x[0] = ks->x;
//		k[0] = ks->k;
//		arm_add_f32(p,q,p,1);
//		arm_add_f32(p,r,k,1);
//		k[0]= p[0]/k[0];
//		ks->k = k[0];
//		arm_sub_f32(measurement,x,q,1);
//		arm_mult_f32(k,q,q,1);
//		arm_add_f32(x,q,x,1);
//		ks->x = x[0];
//		float32_t a[1] = {1};
//		arm_sub_f32(a,k,k,1);
//		arm_mult_f32(k,p,p,1);
//		ks->p = p[0];
//		OutputArray[i] = ks->x;
//	}
//	return 0;
//
//}


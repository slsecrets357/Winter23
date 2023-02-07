/*
 * utility.h
 *
 *  Created on: Jan. 19, 2023
 *      Author: SLsec
 */

#ifndef INC_UTILITY_H_
#define INC_UTILITY_H_
#include "main.h"

typedef struct {
    float q; //process noise covariance
    float r; //measurement noise covariance
    float x; //estimated value
    float p; //estimation error covariance
    float k; // adaptive Kalman filter gain.
} kstate;

float* statistics(float* InputArray, float* OutputArray, uint32_t N);
float stddev(float *vec, float *output, int N);
int kf_asm(float* InputArray, float* OutputArray, kstate* ks, uint32_t length);
int kalmanfilter(float* InputArray, float* OutputArray, kstate* ks, uint32_t length);
extern int kalmanfilter_asm2(kstate* ks, float meas);
extern int kalmanfilter_asm(float* InputArray, float* OutputArray, kstate* ks, uint32_t Length);
int kalmanfilter_DSP(float* InputArray, float* OutputArray, kstate* ks, uint32_t Length);

#endif /* INC_UTILITY_H_ */



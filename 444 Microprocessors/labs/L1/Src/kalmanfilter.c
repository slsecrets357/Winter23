/*
 * kalmanfilter.c
 *
 *  Created on: Jan. 19, 2023
 *      Author: SLsec
 */
#include "main.h"
#include "utility.h"


int kalmanfilter(float* InputArray, float* OutputArray, kstate* ks, uint32_t Length){
    for(int i = 0; i < Length; i++){
        float measurement = InputArray[i];
        ks->p = ks->p + ks->q; // Update the prediction error covariance (p)
        ks->k = ks->p / (ks->p + ks->r); // Calculate the Kalman gain (k)
        float a = ks->x;
        float b = ks->k * (measurement - ks->x);
        ks->x = a+b; // Update the estimate of the state (x)
        ks->p = (1 - ks->k) * ks->p; // Update the prediction error covariance (p)
        OutputArray[i] = ks->x; // Write the updated estimate of the state (x) to the output array

        // Check if the output value is NaN (Not-a-Number) or INF (Infinite) and return -1 if either of these is true
        if (isnan(OutputArray[i]) || isinf(OutputArray[i])){
            return -1;
        }
        if(a > 0 && b > 0 && OutputArray[i] < 0)
        	return -1;
        if(a < 0 && b < 0 && OutputArray[i] > 0)
        	return -1;
    	}
    return 0;
}



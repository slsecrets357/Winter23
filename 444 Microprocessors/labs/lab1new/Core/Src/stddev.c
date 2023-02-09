/*
 * stddev.c
 *
 *  Created on: Feb. 4, 2023
 *      Author: SLsec
 */
#include <math.h>
#include "utility.h"

float stddev(float *vec, float *output, int N) {
  float sum = 0.0, mean, variance = 0.0, dev=0.0;
  float diff[N];
  // Compute the mean of the input vector
  for (int i = 0; i < N; i++) {
    sum += (output[i]-vec[i]);
    diff[i] = output[i]-vec[i];
  }
  mean = sum / N;

  // Compute the variance of the input vector
  for (int i = 0; i < N; i++) {
    variance += pow(diff[i] - mean, 2);
  }
  variance = variance / N;

  // Compute the standard deviation of the input vector
  dev = sqrt(variance);
  float ret[] = {dev, mean};
  return dev;
}

//float std(float* input, float* output, uint32_t length){
//	float sigma = 0.0;
//	float avg = 0.0;
//	for (int i = 0; i < length; i++){
//		avg += input[i];
//	}
//	avg /= length;
//	for (int i = 0; i < length; i++){
//		sigma += pow(input[i]-avg,2);
//	}
//	sigma = sqrt(sigma/length);
//	return sigma;
//}

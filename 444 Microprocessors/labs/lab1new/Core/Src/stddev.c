/*
 * stddev.c
 *
 *  Created on: Feb. 4, 2023
 *      Author: SLsec
 */
#include <math.h>
#include "utility.h"

float stddev(float *vec, float *output, int N) {
  float sum = 0.0, mean, variance = 0.0, dev;

  // Compute the mean of the input vector
  for (int i = 0; i < N; i++) {
    sum += (output[i]-vec[i]);
  }
  mean = sum / N;

  // Compute the variance of the input vector
  for (int i = 0; i < N; i++) {
    variance += pow(vec[i] - mean, 2);
  }
  variance = variance / N;

  // Compute the standard deviation of the input vector
  dev = sqrt(variance);

  return dev;
}

/*
 * statistics.c
 *
 *  Created on: Feb. 4, 2023
 *      Author: SLsec
 */
#include "utility.h"

float* statistics(float* InputArray, float* OutputArray, uint32_t N){
	  float input_sum = 0.0, output_sum = 0.0, input_mean, output_mean, covariance = 0.0, input_stddev = 0.0, output_stddev = 0.0, correlation;

	  // Compute the mean of the input and output arrays
	  for (int i = 0; i < N; i++) {
	    input_sum += input[i];
	    output_sum += output[i];
	  }
	  input_mean = input_sum / N;
	  output_mean = output_sum / N;
	  float mean = output_mean - intput_mean;

	  // Compute the standard deviation of the input and output arrays
	  for (int i = 0; i < N; i++) {
	    input_stddev += pow(input[i] - input_mean, 2);
	    output_stddev += pow(output[i] - output_mean, 2);
	  }
	  input_stddev = sqrt(input_stddev / N);
	  output_stddev = sqrt(output_stddev / N);

	  // Compute the covariance of the input and output arrays
	  for (int i = 0; i < N; i++) {
	    covariance += (input[i] - input_mean) * (output[i] - output_mean);
	  }
	  covariance = covariance / N;

	  // Compute the correlation between the input and output arrays
	  correlation = covariance / (input_stddev * output_stddev);
	  // Compute the convolution between the input and output arrays

	  float convolution = 0.0;
	  for (int i = 0; i < N; i++) {
	    for (int j = 0; j < N; j++) {
	      if (i + j < N) {
	        convolution += input[i] * output[j];
	      }
	    }
	  }

	  float convolution = 0.0;
	  float y[3] = {mean,correlation,convolution };
	  return y;
}

/*
 * statistics.c
 */
#include "main.h"
#include "utility.h"
void convolution( float *InputArray, float *OutputArray, uint32_t length, float *result)
{
	for (int i = 0; i < length; i++) {
		result[i] = 0;
		for (int j = 0; j < length ; j++) {
			if (i >= j && ((i-j) < length)) {
				result[i] += InputArray[i-j] * OutputArray[j];
			}
		}
	}
}
void correlation( float *InputArray, float *OutputArray, uint32_t length, float *result)
{
	for (int i = 0; i < length; i++) {
		result[i] = 0;
		for (int j = 0; j < length ; j++) {
			if (i >= j && ((i-j) < length)) {
				result[i] += InputArray[i-j] * OutputArray[length-1-j];
			}
		}
	}
}
//int correlation(float* input, float* kernel, uint32_t length, float* corr) {
//	uint32_t i, j;
//	float sum;
//	for (i = 0; i < length; i++) {
//		sum = 0.0f;
//		for (j = 0; j < length; j++) {
//			if (i + j < length) {
//				sum += input[i + j] * kernel[length - 1 - j];
//			}
//		}
//		corr[i] = sum;
//	}
//	return 0;
//}

int statistics(float* InputArray, float* OutputArray, uint32_t length, float* corr, float* conv){

	correlation(InputArray, OutputArray, length, corr);
	convolution(InputArray, OutputArray, length, conv);
	return 0;
}


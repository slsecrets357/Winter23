<<<<<<< HEAD
/*
 * cmax.c
 *
 *  Created on: Dec. 29, 2022
 *      Author: SLsec
 */
#include "main.h"

void cMax(float *array, uint32_t size, float *max, uint32_t *maxIndex);
=======
/*
 * cmax.c
 *
 *  Created on: Dec. 29, 2022
 *      Author: SLsec
 */
#include "main.h"

void cMax(float *array, uint32_t size, float *max, uint32_t *maxIndex){
	(*max) = array[0];
	(*maxIndex) = 0;
	for(uint32_t i =1; i<size; i++){
		if(array[i]>(*max)){
			(*max) = array[i];
			(*maxIndex) = i;
		}
	}
}
>>>>>>> 57cca87149b6a2c58469570993efa5876b986ef6

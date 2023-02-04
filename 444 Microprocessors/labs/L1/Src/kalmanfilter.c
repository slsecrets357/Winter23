/*
 * kalmanfilter.c
 *
 *  Created on: Jan. 19, 2023
 *      Author: SLsec
 */
#include "main.h"
#include "utility.h"

//typedef struct {
//	float q,r,x,p,k;
//} kstate;

int kalmanfilter(float* InputArray, float* OutputArray, kstate* ks, uint32_t Length){
    for(int i = 0; i < Length; i++){
        float measurement = InputArray[i];
        ks->p = ks->p + ks->q;
        ks->k = ks->p / (ks->p + ks->r);
        ks->x = ks->x + ks->k * (measurement - ks->x);
        ks->p = (1 - ks->k) * ks->p;
        OutputArray[i] = ks->x;
        if (isnan(OutputArray[i]) || isinf(OutputArray[i])){
            return 1;
        }
    }
    return 0;
}
//extern void kalmanfilter_asm(float* InputArray, float measurement);
//int Kalmanfilter(kalman* states, float measurement){
//	float* s[5] = {states->q, states->r, states->x, states->p, states->k};
//
//	kalmanfilter_asm(s, measurement);
//	return 0;
//}


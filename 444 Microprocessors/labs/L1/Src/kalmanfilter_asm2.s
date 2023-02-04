/*
 * kalmanfilter_asm.s
 *
 *  Created on: Jan. 21, 2023
 *      Author: SLsec
 */

.syntax unified
.global kalmanfilter_asm
.section .text.rodata
/**
* int kalmanfilter_asm(float* InputArray, float* OutputArray, kalman_state* kstate, int length);
*
* R0 = pointer to kstate [q r x p k]
* S0 = measurement
*/
kalmanfilter_asm:
	//push {s1}
	vldr.f32 s2, [r0] //q
	vldr.f32 s1, [r0, #12] //p
	vadd.f32 s1, s1, s0 //p+=q
	vstr.f32 s1, [r0, #12] //update p
	vldr.f32 s2, [r0, #4] //r
	vadd.f32 s2, s2, s1 //s2=r+p
	vdiv.f32 s1, s1, s2 //p/(r+p)
	vstr.f32 s1, [r0, #16]//k=p/(r+p)
	vmov.f32 s0, r1 //meas
	vmov.f32 s2, s0 //s2 = meas
	vldr.f32 s1, [r0, #8] //x
	vsub.f32 s2, s2, s1 //s2 = meas - x
	vldr.f32 s1, [r0, #16] //k
	vmul.f32 s2, s2, s1 //s2 = k*(meas-x)
	vldr.f32 s1, [r0, #8] //x
	vadd.f32 s1, s1, s2
	vstr.f32 s1, [r0, #8]
	vldr.f32 s1, [r0, #16] //k
	vmov.f32 s2, #1
	vsub.f32 s1, s0, s1
	vldr.f32 s2, [r0, #12]
	vmul.f32 s2, s2, s1
	vstr.f32 s2, [r0, #12]
	//pop {s1}
	bx lr

//kalmanfilter_asm:
//    vldr.f32 s2, [r0] //q
//    vldr.f32 s1, [r0, #12] //p
//    vadd.f32 s1, s1, s0, vc //p+=q, vc indicates the result is calculated with overflow checking
//    vmrs APSR_nzcv, FPSCR //move the value of FPSCR to the APSR register
//    bvs overflow //branch to overflow if the overflow bit in APSR is set
//    vstr.f32 s1, [r0, #12] //update p
//    vldr.f32 s2, [r0, #4] //r
//    vadd.f32 s2, s2, s1, vc //s2=r+p, vc indicates the result is calculated with overflow checking
//    vmrs APSR_nzcv, FPSCR //move the value of FPSCR to the APSR register
//    bvs overflow //branch to overflow if the overflow bit in APSR is set
//    vdiv.f32 s1, s1, s2, vc //p/(r+p), vc indicates the result is calculated with overflow checking
//    vmrs APSR_nzcv, FPSCR //move the value of FPSCR to the APSR register
//    bvs overflow //branch to overflow if the overflow bit in APSR is set
//    vstr.f32 s1, [r0, #16]//k=p/(r+p)
//    vmov.f32 s0, r1 //meas
//    vmov.f32 s2, s0 //s2 = meas
//    vldr.f32 s1, [r0, #8] //x
//    vsub.f32 s2, s2, s1, vc //s2 = meas - x, vc indicates the result is calculated with overflow checking
//    vmrs APSR_nzcv, FPSCR //move the value of FPSCR to the APSR register
//    bvs overflow //branch to overflow if the overflow bit in APSR is set
//    vldr.f32 s1, [r0, #16] //k
//    vmul.f32 s2, s2, s1, vc //s2 = k*(meas-x), vc indicates the result is calculated with overflow checking
//    vmrs APSR_nzcv, FPSCR //move the value of FPSCR to the APSR register
//    bvs overflow //branch to overflow if the overflow bit in APSR is set
//    vldr.f32 s1, [r0, #8] //x
//    vadd.f32 s1, s1, s2, vc //s1=x+k*(meas-x), v
//    vmrs APSR_nzcv, FPSCR //move the value of FPSCR to the APSR register
//    bvs overflow //branch to overflow if the overflow bit in APSR is set
//    vstr.f32 s1, [r0, #8]
//	  vldr.f32 s1, [r0, #16] //k
//	  vmov.f32 s2, #1
//	  vsub.f32 s1, s0, s1
//	  vldr.f32 s2, [r0, #12]
//	  vmul.f32 s2, s2, s1, vc
//	  vmrs APSR_nzcv, FPSCR //move the value of FPSCR to the APSR register
//    bvs overflow //branch to overflow if the overflow bit in APSR is set
//	  vstr.f32 s2, [r0, #12]
//	  //pop {s1}
//	  bx lr
//overflow:
//	mov r0, #-1
//	bx lr


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
* R0 = InputArray
* R1 = OutputArray
* R2 = pointer to kstate [q r x p k]
* R3 = length
*/

kalmanfilter_asm:
	push {r4,r5}
	sub R3, #1
loop:
	vldr.f32 s2, [r2] //q
    vldr.f32 s1, [r2, #12] //p
    vadd.f32 s1, s1, s2 //p+=q,
    vstr.f32 s1, [r2, #12] //update p

    vldr.f32 s2, [r2, #4] //r
    vadd.f32 s2, s2, s1 //s2=r+p,
    mrs r4,APSR   // save MCU flags
    vcmp.f32 s2, #0.0 // compare s2 to 0.0 to avoid division by 0
    vmrs APSR_nzcv, FPSCR
    beq division_by_0 // branch to division_by_0 if s2 is equal to 0.0
    msr APSR_nzcvq, r4 // restore MCU flags
    vdiv.f32 s1, s1, s2 //p/(r+p),
    vstr.f32 s1, [r2, #16]//k=p/(r+p)

	vldr.f32 s2, [r0] //load meas from input array
	add r0, r0, #4
    vldr.f32 s1, [r2, #8] //x
    vsub.f32 s2, s2, s1 //s2 = meas - x,
    vldr.f32 s1, [r2, #16] //k
    vmul.f32 s2, s2, s1 //s2 = k*(meas-x),
    vldr.f32 s1, [r2, #8] //x
    vadd.f32 s1, s1, s2 //s1=x+k*(meas-x),
    vstr.f32 s1, [r2, #8]
    vstr.f32 s1, [r1] //store x in output array
    add r1, r1, #4

	vldr.f32 s1, [r2, #16] //k
	vmov.f32 s2, #1
	vsub.f32 s1, s2, s1
	vldr.f32 s2, [r2, #12]
	vmul.f32 s2, s2, s1
	mrs r4,APSR   // save MCU flags
	vmrs APSR_nzcv, FPSCR //move the value of FPSCR to the APSR register
    bvs overflow //branch to overflow if the overflow bit in APSR is set
	//vmrs r5, FPSCR
	//tst r5, #8
	//beq underflow
	//tst r5, #2
	//beq division_by_0
	vstr.f32 s2, [r2, #12]
	msr APSR_nzcvq, r4  // restore MCU flags
	cmp r3, #0
	sub r3, #1
	bgt loop
	pop {r4,r5}
	mov r0, #0
	bx lr
overflow:
	pop {r4,r5}
	mov r0, #-1
	bx lr
division_by_0:
	pop {r4,r5}
	mov r0, #-1
	bx lr
underflow:
	pop {r4,r5}
	mov r0, #-1
	bx lr


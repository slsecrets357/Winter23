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
	sub R3, #1
loop:
	vldr.f32 s2, [r0] //q
    vldr.f32 s1, [r0, #12] //p
    vadd.f32 s1, s1, s2 //p+=q,
    vstr.f32 s1, [r0, #12] //update p

    vldr.f32 s2, [r0, #4] //r
    vadd.f32 s2, s2, s1 //s2=r+p,
    vcmp.f32 s2, #0.0 // compare s2 to 0.0 to avoid division by 0
    vmrs APSR_nzcv, FPSCR
    beq division_by_0 // branch to division_by_0 if s2 is equal to 0.0
    vdiv.f32 s1, s1, s2 //p/(r+p),
    vstr.f32 s1, [r0, #16]//k=p/(r+p)

    vmov.f32 s2, s0 //s2 = meas
    vldr.f32 s1, [r0, #8] //x
    vsub.f32 s2, s2, s1 //s2 = meas - x,
    vldr.f32 s1, [r0, #16] //k
    vmul.f32 s2, s2, s1 //s2 = k*(meas-x),
    vldr.f32 s1, [r0, #8] //x
    vadd.f32 s1, s1, s2 //s1=x+k*(meas-x),
    vstr.f32 s1, [r0, #8]

	vldr.f32 s1, [r0, #16] //k
	vmov.f32 s2, #1
	vsub.f32 s1, s2, s1
	vldr.f32 s2, [r0, #12]
	vmul.f32 s2, s2, s1
	mrs r3,APSR   // save MCU flags
	// only once at the end, underflow, divide by 0 cases
	vmrs APSR_nzcv, FPSCR //move the value of FPSCR to the APSR register
    bvs overflow //branch to overflow if the overflow bit in APSR is set

    // Check for underflow
	//vmsr FPSCR, APSR_nzcv //move the value of APSR to the FPSCR register
	//tst r2, #(1<<24) //test the underflow bit
	//bne underflow //branch to underflow if the underflow bit is set

	vstr.f32 s2, [r0, #12]
	msr APSR_nzcvq, r3  // restore MCU flags
	cmp r3, #0
	sub r3, #1
	bgt loop
	//pop {s1}
	bx lr
overflow:
	mov r0, #-1
	bx lr



.syntax unified
.global asmMax
.section .text.rodata
/**
* void asmMax(float *array, uint32_t size, float *max, uint32_t *maxIndex);
*
* R0 = pointer to array
* R1 = size
* R2 = pointer to max
* R3 = pointer to maxIndex
*/
asmMax:
	push {r4, r5}
	vldr.f32 s0, [r0] //max
	mov r4, #0 //maxIndex
loop:
	subs r1, #1
	blt done
	add r5, r0, r1, lsl #2
	vldr.f32 s1, [r5]
	vcmp.f32 s0, s1
	vmrs APSR_nzvc, FPSCR
	bgt continue
	vmov.f32 s0, s1
	mov r4, r1
continue:
	b loop
done:
	vstr.32 s0, [r2]
	str r4, [r3]
	pop {r4, r5}
	bx lr


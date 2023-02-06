
.syntax unified
.global asmMax
.section .text.rodata
/**
* float* asmMul(float *a, float *b, uint_t size);
*
* R0 = pointer to array1
* R1 = pointer to array2
* R2 = pointer to y
* R3 = size
*/
asmMul:
	push {r4, r5}
	//vldr.f32 s0, [r0]
	//vldr.f32 s1, [r1]
loop:
	subs r3, #1
	blt done
	add r4, r0, r3, lsl #2
	add r5, r1, r3, lsl #2
	vldr.f32 s0, [r4]
	vldr.f32 s1, [r5]
	vmul.f32 s0, s1
	vstr.f32 s0, [r2, r3, lsl #2]
continue:
	b loop
done:
	pop {r4, r5}
	bx lr


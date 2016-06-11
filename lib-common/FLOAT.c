#include "FLOAT.h"
#define uint32_t unsigned int
#define uint64_t unsigned long long

FLOAT F_mul_F(FLOAT a, FLOAT b) {
	uint32_t sgn = (a ^ b) & 0x80000000;
	a &= 0x7FFFFFFF;
	b &= 0x7FFFFFFF;
	a >>= 8;
	b >>= 8;
	uint32_t tmp = a * b;
	FLOAT res = tmp & 0x7FFFFFFF;
	return res | sgn;
}

FLOAT F_div_F(FLOAT a, FLOAT b) {
	uint32_t sgn = (a ^ b) & 0x80000000;
	a &= 0x7FFFFFFF;
	b &= 0x7FFFFFFF;
	uint32_t tmp = a / b;
	FLOAT res = (tmp << 16) & 0x7FFFFFFF;
	return res | sgn;
}

FLOAT f2F(float aa) {
	uint32_t a = *((uint32_t*)(&a));
	uint32_t val;
	int v_exp;
	FLOAT res;

	// +-zero
	if((a & 0x7FFFFFFF) == 0) return a;
	// +-inf
	if((a & 0x7FFFFFFF) == 0x7F800000) return (FLOAT)((uint32_t)a | 0xFFFFFF);

	v_exp = (a & 0x7F800000) >> 23;
	val = (a & 0x7FFFFF) | 0x800000;
	// NaN
	if(v_exp == 0xFF) nemu_assert(0);

	if(v_exp - 127 < -16) {
		return 0;
	} else {
		v_exp = v_exp - 127;
		if(v_exp < -16+23) {
			res = val >> (-v_exp-16+23);
		} else {
			res = val << (v_exp+16-23);
		}
	}

	res = (res & 0x7FFFFFFF) | (a & 0x80000000);
	return res;
}

FLOAT Fabs(FLOAT a) {
	a &= 0x7FFFFFFF;
	return a;
}

FLOAT sqrt(FLOAT x) {
	FLOAT dt, t = int2F(2);

	do {
		dt = F_div_int((F_div_F(x, t) - t), 2);
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

FLOAT pow(FLOAT x, FLOAT y) {
	/* we only compute x^0.333 */
	FLOAT t2, dt, t = int2F(2);

	do {
		t2 = F_mul_F(t, t);
		dt = (F_div_F(x, t2) - t) / 3;
		t += dt;
	} while(Fabs(dt) > f2F(1e-4));

	return t;
}

#undef uint32_t
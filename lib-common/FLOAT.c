#include "FLOAT.h"
#define uint32_t unsigned int
#define uint64_t unsigned long long

FLOAT F_mul_F(FLOAT aa, FLOAT bb) {
	uint32_t a, b, sgn;
	unsigned long long res;
	FLOAT ret;
	a = Fabs(aa);
	b = Fabs(bb);
	sgn = (a != aa) ^ (b != bb);
	res = (uint64_t)a * (uint64_t)b;
	res >>= 16;
	res &= 0x7FFFFFFF;
	ret = res;
	return sgn ? -ret : ret;
}

FLOAT F_div_F(FLOAT aa, FLOAT bb) {
	uint32_t a, b, sgn;
	FLOAT res;
	int i;
	a = Fabs(aa);
	b = Fabs(bb);
	sgn = (a != aa) ^ (b != bb);
	nemu_assert(b != 0);
	res = a / b;
	a = a % b;
	for(i=0; i<16; i++) {
		res <<= 1;
		a <<= 1;
		if(a >= b) {
			res |= 1;
			a -= b;
		}
	}
	return sgn ? -res : res;
}

FLOAT f2F(float aa) {
	uint32_t a = *((uint32_t*)(&aa));
	uint32_t val;
	int v_exp;
	FLOAT res;

	// +-zero
	if((a & 0x7FFFFFFF) == 0) return 0;
	// +-inf
	if((a & 0x7FFFFFFF) == 0x7F800000) return (FLOAT)((uint32_t)a | 0xFFFFFF);

	v_exp = (a & 0x7F800000) >> 23;
	val = (a & 0x7FFFFF) | 0x800000;
	// NaN
	if(v_exp == 0xFF) nemu_assert(0);

	v_exp = v_exp - 127;

	if(v_exp+16-23 > 0) {
		res = val << (v_exp+16-23);
	} else {
		res = val >> (-v_exp-16+23);
	}

	if(res == 0) return 0;
	res &= 0x7FFFFFFF;
	if(a & 0x80000000) res = -res;
	return res;
}

FLOAT Fabs(FLOAT a) {
	return (a < 0) ? (-a) : a;
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
#undef uint64_t
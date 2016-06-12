#include "trap.h"
#include "FLOAT.h"

FLOAT f(FLOAT x) { 
	/* f(x) = 1/(1+25x^2) */
	return F_div_F(int2F(1), int2F(1) + F_mul_int(F_mul_F(x, x), 25));
}

FLOAT computeT(int n, FLOAT a, FLOAT b, FLOAT (*fun)(FLOAT)) {
	int k;
	FLOAT s,h;
	h = F_div_int((b - a), n);
	s = F_div_int(fun(a) + fun(b), 2 );
	for(k = 1; k < n; k ++) {
		s += fun(a + F_mul_int(h, k));
	}
	s = F_mul_F(s, h);
	return s;
}

int main() { 
	FLOAT eps = f2F(1e-4);

	nemu_assert(f2F(0.1245)-f2F(0.124499) < eps);
	FLOAT aa = f2F(12.4499);
	FLOAT bb = int2F(100);
	nemu_assert(bb == 0x640000);
	FLOAT tmp = F_div_F(aa, bb);
	nemu_assert(f2F(0.124498)-tmp < eps);

	FLOAT a = computeT(10, f2F(-1.0), f2F(1.0), f);
	FLOAT ans = f2F(0.551222);

	nemu_assert(Fabs(a - ans) < eps);
	HIT_GOOD_TRAP;
	return 0;
}

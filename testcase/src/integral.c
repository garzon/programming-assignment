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
	for(k = 1; k <= n; k ++) {
		s += fun(a + F_mul_int(h, k));
	}
	s = F_mul_F(s, h);
	return s;
}

int main() { 
	FLOAT eps = f2F(1e-3);

	nemu_assert(f2F(12.45)-f2F(12.449999) < eps);

	FLOAT aa = f2F(124.99);
	FLOAT bb = int2F(10);
	nemu_assert(bb == 0xa0000);

	FLOAT tmp = F_mul_F(aa, bb);
	tmp = F_div_int(aa, 5);
	tmp = F_div_int(aa, 2);
	tmp = F_mul_int(aa, 10);

	nemu_assert(f2F(1244.99)-tmp < eps);

	nemu_assert(F2int(int2F(-1)) == -1);
	nemu_assert(F2int(int2F(-3263)) == -3263);

	nemu_assert(Fabs(F_mul_F(bb, int2F(-1)))-bb < eps);

	FLOAT x = f2F(1.0);
	nemu_assert(x == 0x10000);
	FLOAT A = F_mul_F(x, x);
	nemu_assert(A == 0x10000);
	A = F_mul_int(A, 25);
	nemu_assert(Fabs(A - f2F(25.0)) < eps);
	A = A + int2F(1);
	nemu_assert(Fabs(A - f2F(26.0)) < eps);
	A = F_div_F(int2F(1), A);
	nemu_assert(Fabs(A - f2F(1/26.0)) < eps);


	nemu_assert(Fabs(f(f2F(-0.8))-f2F(0.0588235)) < eps);
	nemu_assert(Fabs(f(f2F(-0.6))-f2F(0.1)) < eps);
	nemu_assert(Fabs(f(f2F(-0.4))-f2F(0.2)) < eps);
	nemu_assert(Fabs(f(f2F(-0.2))-f2F(0.5)) < eps);
	nemu_assert(Fabs(f(f2F(0))-f2F(1)) < eps);

	nemu_assert(Fabs(f(f2F(0.8))-f2F(0.0588235)) < eps);
	nemu_assert(Fabs(f(f2F(0.6))-f2F(0.1)) < eps);
	nemu_assert(Fabs(f(f2F(0.4))-f2F(0.2)) < eps);
	nemu_assert(Fabs(f(f2F(0.2))-f2F(0.5)) < eps);
	nemu_assert(Fabs(f(f2F(1))-f2F(0.0384615)) < eps);
	FLOAT X= f(f2F(1.0));
	FLOAT ttmp=f2F(1/26.0);
	nemu_assert(Fabs(X-ttmp) < eps);

	/*FLOAT a = computeT(10, f2F(-1.0), f2F(1.0), f);
	FLOAT ans = f2F(0.551222);

	FLOAT diff = a - ans;
	nemu_assert(Fabs(diff) < eps);*/
	HIT_GOOD_TRAP;
	return 0;
}

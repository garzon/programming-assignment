#include "cpu/exec/template-start.h"

#define instr cmps

make_helper(concat(cmps_, SUFFIX)) {
	DATA_TYPE a = swaddr_read(REG(R_EDI), DATA_BYTE);
	DATA_TYPE b = swaddr_read(REG(R_ESI), DATA_BYTE);
	DATA_TYPE_S real_val = (DATA_TYPE_S)b;
	DATA_TYPE res;
	res = a + (-real_val);

	cpu.eflags_cf = a < b;
	cpu.eflags_of = (MSB(a) ^ MSB(real_val)) && (MSB(a) ^ MSB(res));
	SET_PSZ_FLAGS(res);

	if(cpu.eflags_df) {
		REG(R_EDI) -= DATA_BYTE;
		REG(R_ESI) -= DATA_BYTE;
	} else {
		REG(R_EDI) += DATA_BYTE;
		REG(R_ESI) += DATA_BYTE;
	}

	print_asm("cmps ESI, EDI");
	return 1;
}

#include "cpu/exec/template-end.h"

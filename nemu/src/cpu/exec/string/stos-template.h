#include "cpu/exec/template-start.h"

#define instr stos

make_helper(concat(stos_, SUFFIX)) {
	swaddr_write(REG(R_EDI), REG(R_EAX), DATA_BYTE);
	if(cpu.eflags_df) {
		REG(R_EDI) -= DATA_BYTE;
	} else {
		REG(R_EDI) += DATA_BYTE;
	}

	print_asm("stos EAX to (EDI)");
	return 1;
}

#include "cpu/exec/template-end.h"

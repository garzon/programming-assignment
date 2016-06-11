#include "cpu/exec/template-start.h"

#define instr movs

make_helper(concat(movs_, SUFFIX)) {
	swaddr_write(REG(R_EDI), swaddr_read(REG(R_ESI), DATA_BYTE), DATA_BYTE);
	if(cpu.eflags_df) {
		REG(R_ESI) -= DATA_BYTE;
		REG(R_EDI) -= DATA_BYTE;
	} else {
		REG(R_ESI) += DATA_BYTE;
		REG(R_EDI) += DATA_BYTE;
	}

	print_asm("movs (ESI) to (EDI)");
	return 1;
}

#include "cpu/exec/template-end.h"

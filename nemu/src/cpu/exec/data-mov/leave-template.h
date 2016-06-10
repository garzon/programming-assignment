#include "cpu/exec/template-start.h"

#define instr leave

#if DATA_BYTE == 2
make_helper(leave_w) {
	cpu.esp = cpu.ebp;
	REG(R_BP) = swaddr_read(cpu.esp, 2);
	cpu.esp += 2;
	print_asm("LEAVE");

	return 1;
}
#endif

#if DATA_BYTE == 4
make_helper(leave_l) {
	cpu.esp = cpu.ebp;
	cpu.ebp = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	print_asm("LEAVE");

	return 1;
}
#endif

#include "cpu/exec/template-end.h"

#include "cpu/exec/helper.h"

make_helper(call_rel32) {
	swaddr_t rel32 = eip + swaddr_read(eip + 1, 4);
	cpu.esp -= 4;
	swaddr_write(cpu.esp, 4, eip);
	cpu.eip = rel32 + 4;
	print_asm("CALL 0x%x", cpu.eip);

	return 1;
}
#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	DATA_TYPE res;
#if DATA_BYTE == 2
	res = swaddr_read(cpu.esp, 2);
	cpu.esp += 2;
#else
	res = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
#endif
	OPERAND_W(op_src, res);
	print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"

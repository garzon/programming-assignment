#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	if(op_src->type == OP_TYPE_IMM) {
		cpu.eip += (DATA_TYPE_S)op_src->val;
	} else
		cpu.eip = op_src->val;

	print_asm("jmp 0x%x (jumped)", cpu.eip);

	cpu.eip -= 1 + DATA_BYTE;
}

make_instr_helper(i)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"

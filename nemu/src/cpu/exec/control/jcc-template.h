#include "cpu/exec/template-start.h"

#define instr jcc

static void do_execute() {
	uint8_t opcode = ops_decoded.opcode;
	bool cond = false;
	if(DATA_BYTE != 1)
		opcode -= 0x10;
	switch(opcode) {
		case 0x74:
			// JE
			cond = cpu.eflags_zf;
		default:
			panic("not implemented");
	}
	if(cond) {
		cpu.eip += (DATA_TYPE_S)op_src->val;
	}
	print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"

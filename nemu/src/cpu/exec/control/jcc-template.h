#include "cpu/exec/template-start.h"

#define instr jcc

static void do_execute() {
	uint8_t opcode = ops_decoded.opcode;
	bool cond = false;
	const char *opName;
	if(DATA_BYTE != 1)
		opcode -= 0x10;
	switch(opcode) {
#define SUBOP(name, condition) opName = str(name); cond = (condition); break
		case 0x74: SUBOP(JE, cpu.eflags_zf);
		case 0x7E: SUBOP(JLE, cpu.eflags_zf || cpu.eflags_sf != cpu.eflags_of);
		default:
			panic("not implemented");
#undef SUBOP
	}

	if(cond) {
		cpu.eip += (DATA_TYPE_S)op_src->val;
		print_asm("%s 0x%x (jumped)", opName, op_src->val);
	} else {
		print_asm("%s 0x%x (not jumped)", opName, op_src->val);
	}
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"

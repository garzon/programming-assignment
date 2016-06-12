#include "cpu/exec/template-start.h"

#define instr ret

static void do_execute() {
	cpu.eip = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	if(op_src->type == OP_TYPE_IMM) {
		cpu.esp += op_src->val;
		print_asm("RET %d to %s", op_src->val, find_obj_name(cpu.eip));
	} else {
		print_asm("RET to %s", find_obj_name(cpu.eip));
	}
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"

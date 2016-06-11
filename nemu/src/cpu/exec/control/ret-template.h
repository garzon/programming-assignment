#include "cpu/exec/template-start.h"

#define instr ret

static void do_execute() {
	cpu.eip = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	/*if(op_src->type == OP_TYPE_IMM) {
		cpu.esp += op_src->val;
		print_asm_template2();
	} else {
		print_asm_template1();
	}*/
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"

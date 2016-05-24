#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
	DATA_TYPE a = op_dest->val;
	DATA_TYPE b = op_src->val;
	cpu.eflags_cf = a < b;
	cpu.eflags_of = (a-(DATA_TYPE)(b)) > a;
	a -= b;
	cpu.eflags_sf = MSB(a);
	cpu.eflags_zf = a == 0;
	set_pf(a);
	OPERAND_W(op_dest, a);
	print_asm_template2();
}

make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"

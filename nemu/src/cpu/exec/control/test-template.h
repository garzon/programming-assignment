#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
	DATA_TYPE a = op_dest->val & op_src->val;
	cpu.eflags_cf = false;
	cpu.eflags_of = false;
	cpu.eflags_sf = MSB(a);
	cpu.eflags_zf = a == 0;
	set_pf(a);
	print_asm_template2();
}

make_instr_helper(i2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"

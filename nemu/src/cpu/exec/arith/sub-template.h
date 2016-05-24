#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
	DATA_TYPE a = op_dest->val;
	DATA_TYPE b = op_src->val;
	cpu.eflags_cf = a < b;
	cpu.eflags_of = ((DATA_TYPE_S)(a-b) < (DATA_TYPE_S)a) || ((DATA_TYPE_S)(a-b) < (DATA_TYPE_S)(-b));
	a -= b;
	cpu.eflags_sf = MSB(a);
	cpu.eflags_zf = a == 0;
	OPERAND_W(op_dest, a);
	print_asm_template2();
}

make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"

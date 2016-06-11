#include "cpu/exec/template-start.h"

#define instr adc

static void do_execute() {
	DATA_TYPE a = op_dest->val;
	DATA_TYPE b = op_src->val + cpu.eflags_cf;
	DATA_TYPE res = a+b;
	cpu.eflags_cf = res < a || res < b;
	cpu.eflags_of = (MSB(a) ^ MSB(b) ^ 1) && (MSB(a) ^ MSB(res));
	SET_PSZ_FLAGS(res);
	OPERAND_W(op_dest, res);
	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"

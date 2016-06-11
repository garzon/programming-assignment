#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute() {
	DATA_TYPE a = op_dest->val;
	DATA_TYPE b = op_src->val;
	DATA_TYPE res;
	if(ops_decoded.opcode == 0x83) {
		res = a + (-(int8_t)b);
		bool sgn = b & 0x80;
		cpu.eflags_of = (MSB(a) ^ sgn) && (MSB(a) ^ MSB(res));
	} else {
		res = a - b;
		cpu.eflags_of = (MSB(a) ^ MSB(b)) && (MSB(a) ^ MSB(res));
	}
	cpu.eflags_cf = a < b;
	cpu.eflags_sf = MSB(res);
	cpu.eflags_zf = res == 0;
	set_pf(res);
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

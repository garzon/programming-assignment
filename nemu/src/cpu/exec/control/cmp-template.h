#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	DATA_TYPE a = op_dest->val;
	DATA_TYPE b = op_src->val;
	DATA_TYPE_S real_val;
	DATA_TYPE res;
	if(op_src->size == 0) op_src->size = DATA_BYTE;
	switch(op_src->size) {
		case 1: real_val = (int8_t)(b); break;
		case 2: real_val = (int16_t)(b); break;
		case 4: real_val = (int32_t)(b); break;
		default: panic("cmp_src_size_err(%d): %x", (int)op_src->size, cpu.eip);
	}
	res = a + (-real_val);
	cpu.eflags_cf = a < b;
	cpu.eflags_of = (MSB(a) ^ MSB(real_val)) && (MSB(a) ^ MSB(res));
	cpu.eflags_sf = MSB(res);
	cpu.eflags_zf = res == 0;
	set_pf(res);
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

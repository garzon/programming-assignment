#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	DATA_TYPE a = op_dest->val;
	DATA_TYPE_S b = op_src->val;
	DATA_TYPE res = a + (DATA_TYPE_S)(-b);
	cpu.eflags_cf = a < (DATA_TYPE)(b);
	cpu.eflags_of = (MSB(a) ^ MSB(((DATA_TYPE)(b)))) && (MSB(a) ^ MSB(res));
	cpu.eflags_sf = MSB(res);
	cpu.eflags_zf = res == 0;
	set_pf(res);
	printf("\n");
	printf(str(DATA_TYPE));
	printf("%d %d", b, MSB(b));
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

#include "cpu/exec/template-start.h"

#define instr movzx

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	if(op_src->size == 4) {
		panic("movsx_size_err");
	}
	print_asm_template2();
}

#if DATA_BYTE != 1
make_instr_helper(rmb2r)
#if DATA_BYTE != 2
make_instr_helper(rmw2r)
#endif
#endif

#include "cpu/exec/template-end.h"

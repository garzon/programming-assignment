#include "cpu/exec/template-start.h"

#define instr movsx

static void do_execute() {
	DATA_TYPE_S res = op_src->val;
	if(op_dest->size == 4)
		write_operand_l(op_dest, (int32_t)res);
	if(op_dest->size == 2)
		write_operand_w(op_dest, (int16_t)res);
	if(op_dest->size == 1) {
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

#include "cpu/exec/template-start.h"

#define instr movsx

static void do_execute() {
	DATA_TYPE_S res = op_src->val;
	if(op_dest->size == 4)
		write_operand_l(op_dest, (int32_t)res);
	if(op_dest->size == 2)
		write_operand_w(op_dest, (int16_t)res);
	if(op_dest->size == 1) {
		printf("dest_size: %d", (int)op_dest->size);
		panic("movsx_size_err");
	}
	print_asm_template2();
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"

#include "cpu/exec/template-start.h"

#define instr movsx

static void do_execute() {
	if(op_dest->size == 4)
		write_operand_l(op_dest, (int32_t)op_src->val);
	if(op_dest->size == 2)
		write_operand_w(op_dest, (int16_t)op_src->val);
	print_asm_template2();
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"

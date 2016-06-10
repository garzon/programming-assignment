#include "cpu/exec/template-start.h"

#define instr movzx

static void do_execute() {
	DATA_TYPE res = op_src->val;
	printf("src, %08x", op_src->val);
	printf("src, %08x", op_dest->val);
	if(op_dest->size == 4)
		write_operand_l(op_dest, (int32_t)res);
	if(op_dest->size == 2)
		write_operand_w(op_dest, (int16_t)res);
	print_asm_template2();
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"

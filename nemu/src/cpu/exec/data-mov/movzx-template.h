#include "cpu/exec/template-start.h"

#define instr movzx

static void do_execute() {
	DATA_TYPE res = op_src->val;
	if(op_dest->size == 4)
		write_operand_l(op_dest, (int32_t)res);
	if(op_dest->size == 2)
		write_operand_w(op_dest, (int16_t)res);
	if(op_dest->size == 1) {
		printf("dest_val: %08x\n", op_dest->val);
		printf("dest_size: %d", (int)op_dest->size);
		printf("srcval: %08x\n", op_src->val);
		printf("srcsize: %d", (int)op_src->size);
		panic("movsx_size_err");
	}
	print_asm_template2();
}

make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"

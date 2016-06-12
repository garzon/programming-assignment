#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	cpu.esp -= DATA_BYTE;
	
	switch(ops_decoded.opcode & 0xff) {
		case 0xE8: 
			swaddr_write(cpu.esp, DATA_BYTE, (DATA_TYPE)cpu.eip+1+DATA_BYTE);
			cpu.eip += (DATA_TYPE_S)op_src->val; 
			print_asm("CALL %s", find_obj_name(cpu.eip+DATA_BYTE+1));
			break;
		case 0xFF: 
			swaddr_write(cpu.esp, DATA_BYTE, (DATA_TYPE)cpu.eip+2);
			cpu.eip = (DATA_TYPE)op_src->val; 
			print_asm("CALL %s", find_obj_name(cpu.eip));
			cpu.eip -= 2;
			break;
		default: panic("unknown CALL opcode");
	}

#if DATA_BYTE == 2
	cpu.eip &= 0xffff;
#endif

}

make_instr_helper(i)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"

#include "cpu/exec/template-start.h"

#define instr jcc

static void do_execute() {
	uint8_t opcode = ops_decoded.opcode;
	bool cond = false;
	const char *opName;
	bool CF = cpu.eflags_cf,
		 ZF = cpu.eflags_zf,
		 SF = cpu.eflags_sf,
		 OF = cpu.eflags_of,
		 PF = cpu.eflags_pf;

	uint32_t dest_addr = cpu.eip + (DATA_TYPE_S)op_src->val;
	uint32_t show_addr = dest_addr + 1 + DATA_BYTE;

	if(DATA_BYTE != 1)
		opcode -= 0x10;
	switch(opcode) {
#define SUBOP(name, condition) opName = str(name); cond = (condition); break
		case 0x74: SUBOP(JE, ZF);
		case 0x7E: SUBOP(JLE, ZF || SF != OF);
		case 0x77: SUBOP(JA, !CF && !ZF);
		case 0x73: SUBOP(JAE, !CF);
		case 0x72: SUBOP(JB, CF);
		case 0x76: SUBOP(JBE, CF || ZF);
		case 0x7F: SUBOP(JG, !ZF && SF == OF);
		case 0X7D: SUBOP(JGE, SF == OF);
		case 0x7C: SUBOP(JL, SF != OF);
		case 0x75: SUBOP(JNE, !ZF);
		case 0x71: SUBOP(JNO, !OF);
		case 0x7B: SUBOP(JNP, !PF);
		case 0x79: SUBOP(JNS, !SF);
		case 0x70: SUBOP(JO, OF);
		case 0x7A: SUBOP(JP, PF);
		case 0x78: SUBOP(JS, SF);
		default:
			panic("not implemented");
#undef SUBOP
	}


	if(cond) {
		cpu.eip = dest_addr;
		print_asm("%s 0x%x (jumped)", opName, show_addr);
	} else {
		print_asm("%s 0x%x (not jumped)", opName, show_addr);
	}
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"

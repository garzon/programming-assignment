#include "cpu/exec/template-start.h"

#define instr setcc

static void do_execute() {
	uint8_t opcode = ops_decoded.opcode;
	bool cond = false;
	const char *opName;
	bool CF = cpu.eflags_cf,
		 ZF = cpu.eflags_zf,
		 SF = cpu.eflags_sf,
		 OF = cpu.eflags_of,
		 PF = cpu.eflags_pf;

	switch(opcode) {
#define SUBOP(name, condition) opName = str(name); cond = (condition); break
		case 0x94: SUBOP(SETE, ZF);
		case 0x9E: SUBOP(SETLE, ZF || SF != OF);
		case 0x97: SUBOP(SETA, !CF && !ZF);
		case 0x93: SUBOP(SETAE, !CF);
		case 0x92: SUBOP(SETB, CF);
		case 0x96: SUBOP(SETBE, CF || ZF);
		case 0x9F: SUBOP(SETG, !ZF && SF == OF);
		case 0X9D: SUBOP(SETGE, SF == OF);
		case 0x9C: SUBOP(SETL, SF != OF);
		case 0x95: SUBOP(SETNE, !ZF);
		case 0x91: SUBOP(SETNO, !OF);
		case 0x9B: SUBOP(SETNP, !PF);
		case 0x99: SUBOP(SETNS, !SF);
		case 0x90: SUBOP(SETO, OF);
		case 0x9A: SUBOP(SETP, PF);
		case 0x98: SUBOP(SETS, SF);
		default:
			panic("not implemented");
#undef SUBOP
	}


	if(cond) {
		OPERAND_W(op_src, 1);
		print_asm("%s(1)", opName);
	} else {
		OPERAND_W(op_src, 0);
		print_asm("%s(0)", opName);
	}
	print_asm_template2();
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"

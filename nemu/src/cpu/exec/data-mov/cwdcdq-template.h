#include "cpu/exec/template-start.h"

#define instr cwd_cdq

make_helper(concat(cwd_cdq_, SUFFIX)) {
#if DATA_BYTE == 2
	if((int16_t)REG(R_AX) < 0) REG(R_DX) = 0xffff;
	else REG(R_DX) = 0;
	print_asm("cwd");
#else
	if((int32_t)cpu.eax < 0) cpu.edx = 0xffffffff;
	else cpu.edx = 0;
	print_asm("cdq");
#endif
	return 1;
}

#include "cpu/exec/template-end.h"

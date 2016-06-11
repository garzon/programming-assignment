#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "ret-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "ret-template.h"
#undef DATA_BYTE

make_helper_v(ret_i)

#define DATA_BYTE 4
#include "cpu/exec/template-start.h"
#define instr ret
make_helper(ret) {
	cpu.eip = swaddr_read(cpu.esp, 4)-1;
	cpu.esp += 4;
	print_asm_template1();

	return 1;
}
#include "cpu/exec/template-end.h"
#undef DATA_BYTE
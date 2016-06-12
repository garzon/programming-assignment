#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "monitor/elf.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_si(char *args) {
	const char *arg = strtok(NULL, " ");
	int numSteps = 1;
	if(arg != NULL)
		numSteps = atoi(arg);
	cpu_exec(numSteps);
	return 0;
}

static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");
	if(!arg) {
		printf("Invalid argument for command 'info'.\n");
		return 0;
	}
	bool flag = false;
	if(!strcasecmp(arg, "r")) {
		printf("EAX: 0x%08X\t", cpu.eax);
		printf("EBX: 0x%08X\t", cpu.ebx);
		printf("ECX: 0x%08X\n", cpu.ecx);
		printf("EDX: 0x%08X\t", cpu.edx);
		printf("ESP: 0x%08X\t", cpu.esp);
		printf("EBP: 0x%08X\n", cpu.ebp);
		printf("ESI: 0x%08X\t", cpu.esi);
		printf("EDI: 0x%08X\t", cpu.edi);
		printf("EIP: 0x%08X\n", cpu.eip);
		flag = true;
	}
	if(!strcasecmp(arg, "w")) {
		print_wp();
		flag = true;
	}
	if(!flag) {
		printf("Invalid argument for command 'info'.\n");
	}
	return 0;
}

static int cmd_p(char *args) {
	if(!args) {
		printf("Invalid argument for command 'p'.\n");
		return 0;
	}

	bool succ;
	uint32_t res = expr(args, &succ);

	if(!succ) {
		printf("Invalid expression.\n");
	} else {
		printf("eval result: 0x%X %d\n", res, res);
	}
	return 0;
}

static int cmd_x(char *args) {
	const char *arg = strtok(NULL, " ");
	bool succ;
	uint32_t res;
	int num = 0, i;

	if(arg != NULL)
		num = atoi(arg);
	if(num == 0) {
		printf("Invalid argument '%s'\n", arg);
		return 0;
	}

	arg = strtok(NULL, " ");
	if(!arg) {
		printf("Invalid 2nd argument for command 'p'.\n");
		return 0;
	}
	res = expr(arg, &succ);
	if(!succ) {
		printf("Invalid expression '%s'\n", arg);
	} else {
		for(i=0; i<num; i++) {
			if(i % 4 == 0) {
				printf("0x%08X: ", res);
			}
			printf("0x%08X ", swaddr_read(res, 4));
			if(i % 4 == 3) printf("\n");
			res += 4;
		}
	}

	printf("\n");
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_w(char *args) {
	if(!args) {
		printf("Invalid argument for command 'w'.\n");
		return 0;
	}

	bool succ;
	uint32_t res = expr(args, &succ);
	WP *wp;

	if(!succ) {
		printf("Invalid expression.\n");
	} else {
		wp = new_wp(args);
		if(wp) {
			wp->last_value = res;
			printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
		} else {
			printf("Error: too many watchpoints!\n");
		}
	}

	printf("\n");
	return 0;
}

static int cmd_d(char *args) {
	if(!args) {
		printf("Invalid argument for command 'd'.\n");
		return 0;
	}
	uint32_t res = atoi(args);

	if(!res) {
		printf("Invalid watchpoint id.\n");
	} else {		
		printf("Delete watchpoint %d\n", res);
		free_wp(res);
	}
	return 0;
}

struct StackInfo {
	uint32_t prev_ebp;
	uint32_t ret_addr;
	uint32_t args[4];
};

static struct StackInfo stackinfo_buf;

static struct StackInfo *load_stack_info(uint32_t ebp) {
	if(!ebp) return NULL;
	int times = sizeof(struct StackInfo)/4, i;
	for(i=0; i<times; i++) {
		*(((uint32_t *)&stackinfo_buf)+i) = swaddr_read(ebp+i*4, 4);
	}
	return &stackinfo_buf;
};

static int cmd_bt(char *args) {
	struct StackInfo *info;
	info = load_stack_info(cpu.ebp);
	int counter = 0;
	while(info) {
		counter++;
		printf("#%d %s(0x%x, 0x%x, 0x%x, 0x%x)", counter, find_obj_name(info->ret_addr), info->args[0], info->args[1], info->args[2], info->args[3]);
		info = load_stack_info(info->prev_ebp);
	}
	return 0;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "help [cmd]: Display informations about all supported commands", cmd_help },
	{ "c", "c: Continue the execution of the program", cmd_c },
	{ "si", "si [n]: Single step", cmd_si },
	{ "info", "info [type]: \n\ttype='r': print all registers, type='w': print all watchpoints", cmd_info },
	{ "p", "p [expr]: \n\tprint the value of the input expression", cmd_p },
	{ "x", "x [N] [expr]: \n\tprint the DWORDs from the memory address specified in [expr] N times", cmd_x },
	{ "d", "d [N]: \n\tdelete the #N watchpoint", cmd_d },
	{ "w", "w [expr]: \n\twatch the expr and interrupt when it changes", cmd_w },
	{ "bt", "bt: print the backtrace", cmd_bt },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}

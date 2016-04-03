#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
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
	bool flag = false;
	if(!strcmp(arg, "r") || strcmp(arg, "R")) {
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
	if(!flag) {
		printf("Invalid argument for command 'info'.\n");
	}
	return 0;
}

static int cmd_p(char *args) {
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
	int num = 0;

	if(arg != NULL)
		num = atoi(arg);
	if(num == 0) {
		printf("Invalid argument '%s'\n", arg);
	}

	arg = strtok(NULL, " ");
	res = expr(arg, &succ);
	if(!succ) {
		printf("Invalid expression '%s'\n", arg);
	} else {
		for(; num>0; num--) {
			printf("%08X: %08X \n", res, swaddr_read(res, 4));
			res += 4;
		}
	}

	return 0;
}

static int cmd_q(char *args) {
	return -1;
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
	{ "info", "info [type]: \n\ttype='r': print all registers", cmd_info },
	{ "p", "p [expr]: \n\tprint the value of the input expression", cmd_p },
	{ "x", "x [N] [expr]: \n\tprint the DWORDs from the memory address specified in [expr] N times", cmd_x },
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

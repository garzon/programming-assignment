#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	char expr[100];
	uint32_t last_value;
} WP;

WP* new_wp(const char *expr);
void free_wp(int no);
void print_wp();

bool check_wp();

#endif

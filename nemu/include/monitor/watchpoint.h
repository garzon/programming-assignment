#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	char expr[100];

	/* TODO: Add more members if necessary */


} WP;

WP* new_wp(const char *expr);
void free_wp(int no);

#endif

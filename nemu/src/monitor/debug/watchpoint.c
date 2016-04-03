#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

WP* new_wp(const char *expr) {
	WP *res = free_, *p;
	if(free_ == NULL) return NULL;
	res->next = NULL;
	if(head == NULL) head = res;
	else {
		p = head;
		while(p->next) p = p->next;
		p->next = res;
	}
	return res;
}

void free_wp(int no) {
	WP *p = head;
	while(p && p->NO != no) p = p->next;
	if(p == NULL) return;
}



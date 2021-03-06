#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;

void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i+1;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}

WP* new_wp(const char *expr) {
	WP *res = free_, *p;
	if(free_ == NULL) return NULL;
	free_ = free_->next;
	res->next = NULL;
	if(head == NULL) head = res;
	else {
		p = head;
		while(p->next) p = p->next;
		p->next = res;
	}
	assert(strlen(expr) < sizeof(wp_list[0].expr));
	strcpy(res->expr, expr);
	return res;
}

void free_wp(int no) {
	WP *p = head, *q = head;
	while(p && p->NO != no) p = p->next;
	if(p == NULL) return;
	if(p != head) {
		while(q->next != p) q = q->next;
		q->next = p->next;
	} else {
		head = p->next;
	}
	p->next = free_;
	free_ = p;
}


void print_wp() {
	WP *p = head;
	while(p) {
		printf("#%d: %s, value: %08X\n", p->NO, p->expr, p->last_value);
		p = p->next;
	}
}

bool check_wp() {
	WP *p = head;
	bool flag = false, dummy;
	uint32_t new_val;
	while(p) {
		new_val = expr(p->expr, &dummy);
		if(new_val != p->last_value) {
			flag = true;
			printf("Watchpoint #%d: %s\n", p->NO, p->expr);
			printf("Old value: 0x%08X %d\n", p->last_value, p->last_value);
			p->last_value = new_val;
			printf("New value: 0x%08X %d\n\n", p->last_value, p->last_value);
		}
		p = p->next;
	}
	return flag;	
}
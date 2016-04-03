#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

enum {
	VALUE,
	OPERATOR,
	OTHERS,
	UNARY_OPERATOR
};

enum {
	NOTYPE = 256, EQ = '=',

	REGISTER = 'r',
	PLUS = '+',
	MINUS = '-',
	ASTERISK = '*',
	SLASH = '/',
	LEFT_PAR = '(',
	RIGHT_PAR = ')',
	INTEGER = 'i',
	HEX_INTEGER = 'X',

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int category;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", OTHERS, NOTYPE},
	{"\\+", OPERATOR, PLUS},
	{"==", OPERATOR, EQ},
	{"\\$[a-zA-Z]{2,3}", VALUE, REGISTER},
	{"-", OPERATOR, MINUS},
	{"\\*", OPERATOR, ASTERISK},
	{"\\/", OPERATOR, SLASH},
	{"\\(", OTHERS, LEFT_PAR},
	{"\\)", OTHERS, RIGHT_PAR},
	{"0x[0-9a-fA-F]+", VALUE, HEX_INTEGER},
	{"[0-9]+", VALUE, INTEGER}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret != 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int category;
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(const char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				const char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				if(rules[i].token_type == NOTYPE) break;

				assert(nr_token < 32);

				tokens[nr_token].category = rules[i].category;
				tokens[nr_token].type = rules[i].token_type;

				switch(rules[i].category) {
					case VALUE:
						assert(substr_len < 32);
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						tokens[nr_token].str[substr_len] = '\0';
						break;
					default: //panic("please implement me");
						break;
				}

				if(rules[i].token_type == '*' ||
					rules[i].token_type == '-')
				{
					if(nr_token == 0 || 
						tokens[nr_token-1].category == OPERATOR || 
						tokens[nr_token-1].category == UNARY_OPERATOR) 
					{
						
						tokens[nr_token].category = UNARY_OPERATOR;
					}
				}

				nr_token++;

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool invalid = false;

bool invalid_expr() {
	invalid = true;
	return false;
}

bool check_parentheses(int p, int q) {
	int i, counter;
	if(tokens[p].type != '(') return false;
	if(tokens[q].type != ')') return false;
	counter = 0;
	for(i=p; i<=q; i++) {
		if(tokens[i].type == '(') counter++;
		if(tokens[i].type == ')') counter--;
		if(counter < 0) return invalid_expr();
	}
	return counter == 0;
}

uint32_t register_eval(const char *reg_name) {
	const char *names_32[] = {
		"$EAX", "$ECX", "$EDX", "$EBX",
		"$ESP", "$EBP", "$ESI", "$EDI"
	};
	const char *names_16[] = {
		"$AX", "$CX", "$DX", "$BX",
		"$SP", "$BP", "$SI", "$DI"
	};
	const char *names_8l[] = {
		"$AH", "$CH", "$DH", "$BH"
	};
	const char *names_8h[] = {
		"$AH", "$CH", "$DH", "$BH"
	};
	int i;
	if(!strcasecmp(reg_name, "$EIP")) return cpu.eip;
	for(i=0; i<8; i++) {
		if(!strcasecmp(reg_name, names_32[i]))
			return cpu.gpr[i]._32;
	}
	for(i=0; i<8; i++) {
		if(!strcasecmp(reg_name, names_16[i]))
			return cpu.gpr[i]._16;
	}
	for(i=0; i<4; i++) {
		if(!strcasecmp(reg_name, names_8h[i]))
			return cpu.gpr[i]._8[1];
		if(!strcasecmp(reg_name, names_8l[i]))
			return cpu.gpr[i]._8[0];
	}
	return invalid_expr();
}

bool check_op_priority(int op1, int op2) {
	const int rank1[] = {'*', '/', 0};
	const int rank2[] = {'+', '-', 0};
	const int *opList[] = {
		&rank1[0],
		&rank2[0],
		(int *)NULL
	};
	bool flag1 = false, flag2 = false;
	int i, j;
	if(op1 == -1) return true;
	for(i=0; opList[i]; i++) {
		for(j=0; opList[i][j]; j++) {
			if(tokens[op1].type == opList[i][j])
				flag1 = true;
			if(tokens[op2].type == opList[i][j])
				flag2 = true;
		}
		if(flag1 && !flag2) return true;
		if(flag2) return false;
	}
	return invalid_expr();
}

bool unary_operator_only(int p, int q) {
	int i, counter = 0; bool flag = false;
	for(i=p; i<=q; i++) {
		if(tokens[i].type == '(') {
			counter++;
			flag = true;
		}
		if(tokens[i].type == ')') counter--;
		if(counter) continue;
		if(flag && tokens[i].type != ')')
			return false;
		if(tokens[i].category == OPERATOR) return false;
	}
	return true;
}

uint32_t eval(int p, int q) {
	int op, counter = 0, old_op = -1;
	uint32_t val1, val2;
	if(p > q) {
		return invalid_expr();
	}
	else if(p == q) {
		if(tokens[p].category != VALUE) {
			return invalid_expr();
		}
		switch(tokens[p].type) {
			case INTEGER:
				return atoi(tokens[p].str);
			case HEX_INTEGER:
				return strtol(tokens[p].str, NULL, 16);
			case REGISTER:
				return register_eval(tokens[p].str);
			default:
				return invalid_expr();
		}
	}
	else if(check_parentheses(p, q) == true) {
		return eval(p + 1, q - 1); 
	}
	else if(unary_operator_only(p, q)) {
		assert(tokens[p].category == UNARY_OPERATOR);
		switch(tokens[p].type) {
			case '*':
				return swaddr_read(eval(p+1, q), 4);
			case '-':
				return -eval(p+1, q);
			default:
				return invalid_expr();	
		}
	}
	else {

		for(op = q; op >= p; op--) {
			if(tokens[op].type == ')') counter++;
			if(tokens[op].type == '(') counter--;
			if(counter == 0) {
				if(tokens[op].category == OPERATOR) {
					if(check_op_priority(old_op, op)) {
						old_op = op;
					}
				}
			}
		}

		op = old_op;
		if(op == -1) return invalid_expr();


		val1 = eval(p, op - 1);
		if(invalid) return invalid_expr();
		val2 = eval(op + 1, q);
		if(invalid) return invalid_expr();

		switch(tokens[op].type) {
			case '+': return val1 + val2;
			case '-': return val1 - val2;
			case '*': return val1 * val2;
			case '/': return val1 / val2;
			default: return invalid_expr();
		}
	}
}

uint32_t expr(const char *e, bool *success) {
	uint32_t res;

	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	invalid = false;

	res = eval(0, nr_token-1);

	if(invalid) {
		*success = false;
		return 0;
	}

	*success = true;
	return res;
}
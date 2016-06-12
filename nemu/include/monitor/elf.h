#ifndef __MONITOR_ELF_H__
#define __MONITOR_ELF_H__

#include "common.h"
#include <stdlib.h>
#include <elf.h>

uint32_t symtab_value(const char *key_name);
const char *find_obj_name(uint32_t addr);

#endif
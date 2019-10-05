#ifndef __BINLOAD__
#define __BINLOAD__

#include <stdlib.h>
#include <stdint.h>
#include "llist.h"

enum bin_type {
	BIN_TYPE_AUTO,
	BIN_TYPE_ELF,
	BIN_TYPE_PE
};

enum bin_arch {
	ARCH_X86
};

enum sec_type {
	SEC_TYPE_NONE,
	SEC_TYPE_DATA,
	SEC_TYPE_CODE
};

enum sym_type {
	SYM_TYPE_SEC,
	SYM_TYPE_FUNC,
	SYM_TYPE_LOCAL,
	SYM_TYPE_GLOBAL,
	SYM_TYPE_DYNAMIC
};

struct binary {
	char *name;
	uint64_t entry;
	uint8_t type;
	char *type_str;
	uint16_t arch;
	char *arch_str;
	uint8_t bits;
	struct llist *sections;
	struct llist *symbols;
};

struct section {
	uint8_t type;
	char *name;
	uint64_t vma;
	uint8_t *bytes;
	size_t size;
};

struct symbol {
	uint8_t type;
	char *name;
	uint64_t addr;
};

/*
 * Load a binary
 *
 * @params:
 * 	name: binary file name
 * @returns:
 * 	pointer to struct binary for the loaded binary
 */
struct binary *binary_load(char *name);

/*
 * Unload a binary
 *
 * @params:
 * 	bin: struct binary to unload
 * @returns:
 * 	-
 */
void binary_unload(struct binary *bin);

/*
 * Get a section of the binary by name
 *
 * @params:
 * 	bin: struct binary
 * 	name: section name
 * @returns:
 * 	pointer to struct section on success, else NULL
 */
struct section *binary_get_section_by_name(struct binary *bin, char *name);

#endif /* __BINLOAD__ */

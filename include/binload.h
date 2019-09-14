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
	ARCH_X86,
	ARCH_X86_64
};

enum sec_type {
	SEC_TYPE_NONE,
	SEC_TYPE_DATA,
	SEC_TYPE_CODE
};

enum sym_type {
	SYM_TYPE_UKN,
	SYM_TYPE_FUNC
};

struct binary {
	char *name;
	uint8_t type;
	uint16_t arch;
	uint8_t *entry;
	struct llist *sections;
	struct llist *symbols;
};

struct section {
	uint8_t type;
	char *name;
	uint8_t *vma;
	uint8_t *bytes;
	size_t size;
};

struct symbol {
	uint8_t type;
	char *name;
	uint8_t *addr;
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
 * Get the text section of a binary
 *
 * @params:
 * 	bin: struct binary from which to get the text section
 * @returns:
 * 	pointer to struct section on success, else NULL
 */
struct section *binary_get_text_section(struct binary *bin);

#endif /* __BINLOAD__ */

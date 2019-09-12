#ifndef __BINLOAD__
#define __BINLOAD__

#include <stdint.h>
#include "includes/llist.h"

enum bin_type {
	x86,
	x86_64
}

enum sec_type {
	DATA,
	CODE
}

struct binary {
	char *fname;
	uint16_t bin_type;
	uint8_t *entry;
	struct llist *sections;
	struct llist *symbols;
}

struct section {
	char *name;
	uint8_t sec_type;
	uint8_t *bytes;
	size_t size;
}

struct symbol {
	uint8_t *addr;
	char *name;
}

/*
 * Load a binary
 *
 * @params:
 * 	fname: binary file name
 * @returns:
 * 	pointer to struct binary for the loaded binary
 */
struct binary *binary_load(char *fname);

/*
 * Unload a binary
 *
 * @params:
 * 	bin: binary to unload
 * @returns:
 * 	-
 */
void binary_unload(struct binary *bin);

#endif /* __BINLOAD__ */

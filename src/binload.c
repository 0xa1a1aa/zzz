/*
 * This is a workaround. See:
 * https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
 */
#define PACKAGE "binload"
#define PACKAGE_VERSION "1.0.0"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bfd.h"
#include "binload.h"
#include "llist.h"

static bfd *open_bfd(char *fname);
static void load_sections(bfd *bfd_h, struct binary *bin);
static void load_symbols(bfd *bfd_h, struct binary *bin);
static void load_dyn_symbols(bfd *bfd_h, struct binary *bin);
static struct binary *binary_init(void);
static struct section *section_init(void);
static struct symbol *symbol_init(void);
static void binary_destroy(struct binary *bin);
static void section_destroy(struct section *sec);
static void symbol_destroy(struct symbol *sym);

struct binary *
binary_load(char *fname)
{
	struct binary *bin;
	bfd *bfd_h;
	const bfd_arch_info_type *bfd_info;

	bfd_h = open_bfd(fname);

	bin = binary_init();
	bin->name = (char *) malloc(strlen(fname) + 1);
	if (bin->name) {
		strncpy(bin->name, fname, strlen(fname));
	}

	bin->entry = bfd_get_start_address(bfd_h);
	bin->type_str = (char *) malloc(strlen(bfd_h->xvec->name) + 1);
	if (bin->type_str) {
		strncpy(bin->type_str, bfd_h->xvec->name, strlen(bfd_h->xvec->name));
	}
	switch(bfd_h->xvec->flavour) {
		case bfd_target_elf_flavour:
			bin->type = BIN_TYPE_ELF;
			break;
		case bfd_target_coff_flavour:
			bin->type = BIN_TYPE_PE;
			break;
		default:
			fprintf(stderr, "unsupported binary type (%s)\n", bfd_h->xvec->name);
			goto fail;
	}
	bfd_info = bfd_get_arch_info(bfd_h);
	bin->arch_str = (char *) malloc(strlen(bfd_info->printable_name));
	if (bin->arch_str) {
		strncpy(bin->arch_str, bfd_info->printable_name, strlen(bfd_info->printable_name));
	}
	switch(bfd_info->mach) {
		case bfd_mach_i386_i386:
			bin->arch = ARCH_X86;
			bin->bits = 32;
		case bfd_mach_x86_64:
			bin->arch = ARCH_X86;
			bin->bits = 64;
		default:
			fprintf(stderr, "unsupported architecture (%s)\n", bfd_info->printable_name);
			goto fail;
	}
	load_sections(bfd_h, bin);
	load_symbols(bfd_h, bin);
	load_dyn_symbols(bfd_h, bin);

	goto cleanup;
fail:
	binary_destroy(bin);
	bin = NULL;
cleanup:
	if (bfd_h) bfd_close(bfd_h);

	return bin;
}

void
binary_unload(struct binary *bin)
{
	struct llist *llist, *node;

	/* free sections */
	llist = bin->sections;
	while(llist) {
		node = llist_get_by_idx(llist, 0);
		section_destroy((struct section *) node->data);
		llist = llist_rem_by_idx(llist, 0);
	}
	/* free symbols */
	llist = bin->symbols;
	while(llist) {
		node = llist_get_by_idx(llist, 0);
		symbol_destroy((struct symbol *) node->data);
		llist = llist_rem_by_idx(llist, 0);
	}
	binary_destroy(bin);
}

struct section *
binary_get_section_by_name(struct binary *bin, char *name)
{
	struct llist *llist_sec;
	struct section *sec;

	llist_sec = bin->sections;
	while(llist_sec) {
		sec = (struct section *) llist_sec->data;
		if (strstr(sec->name, name) != NULL) {
			return sec;
		}
		llist_sec = llist_sec->next;
	}
	return NULL;
}

static bfd *
open_bfd(char *fname)
{
	static int init;
	bfd *bfd_h;

	init = 0;
	if (!init) {
		init = 1;
		bfd_init();
	}
	bfd_h = bfd_openr(fname, NULL);
	if (!bfd_h) {
		return NULL;
	}
	if (!bfd_check_format(bfd_h, bfd_object)) {
		fprintf(stderr, "file '%s' does not look like an executable (%s)\n",
				fname, bfd_errmsg(bfd_get_error()));
		return NULL;
	}
	bfd_set_error(bfd_error_no_error);
	if (bfd_get_flavour(bfd_h) == bfd_target_unknown_flavour) {
		fprintf(stderr, "unrecognized format for binary '%s' (%s)",
				fname, bfd_errmsg(bfd_get_error()));
		return NULL;
	}
	return bfd_h;
}

static void
load_sections(bfd *bfd_h, struct binary *bin)
{
}

static void
load_symbols(bfd *bfd_h, struct binary *bin)
{
}

static void
load_dyn_symbols(bfd *bfd_h, struct binary *bin)
{
}

static struct binary *
binary_init(void)
{
	(struct binary *) calloc(1, sizeof(struct binary));
}

static struct section *
section_init(void)
{
	return (struct section *) calloc(1, sizeof(struct section));
}

static struct symbol *
symbol_init(void)
{
	return (struct symbol *) calloc(1, sizeof(struct symbol));
}

static void
binary_destroy(struct binary *bin)
{
	if (bin->name) free(bin->name);
	if (bin->type_str) free(bin->type_str);
	if (bin->arch_str) free(bin->arch_str);
	free(bin);
}

static void
section_destroy(struct section *sec)
{
	if (sec->name) free(sec->name);
	free(sec);
}

static void
symbol_destroy(struct symbol *sym)
{
	if (sym->name) free(sym->name);
	free(sym);
}

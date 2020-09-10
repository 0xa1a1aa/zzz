/* * This is a workaround, for not using autotools.
 * See: https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
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
static void llist_sec_dtor(void *data);
static void llist_sym_dtor(void *data);
static void sym_set_type(asymbol *bfd_sym, struct symbol *sym);

struct binary *
binary_load(char *fname)
{
	struct binary *bin;
	bfd *bfd_h;
	const bfd_arch_info_type *bfd_info;

	bfd_h = open_bfd(fname);
	if (!bfd_h) {
		return NULL;
	}

	bin = binary_init();
	bin->name = (char *) malloc(strlen(fname) + 1);
	if (bin->name) {
		strncpy(bin->name, fname, strlen(fname));
		bin->name[strlen(fname)] = '\0';
	}

	bin->entry = bfd_get_start_address(bfd_h);
	bin->type_str = (char *) malloc(strlen(bfd_h->xvec->name) + 1);
	if (bin->type_str) {
		strncpy(bin->type_str, bfd_h->xvec->name, strlen(bfd_h->xvec->name));
		bin->type_str[strlen(bfd_h->xvec->name)] = '\0';
	}
	switch (bfd_h->xvec->flavour) {
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
	bin->arch_str = (char *) malloc(strlen(bfd_info->printable_name) + 1);
	if (bin->arch_str) {
		strncpy(bin->arch_str, bfd_info->printable_name, strlen(bfd_info->printable_name));
		bin->arch_str[strlen(bfd_info->printable_name)] = '\0';
	}
	switch (bfd_info->mach) {
		case bfd_mach_i386_i386:
			bin->arch = ARCH_X86;
			bin->bits = 32;
			break;
		case bfd_mach_x86_64:
			bin->arch = ARCH_X86;
			bin->bits = 64;
			break;
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
	if (bin) binary_destroy(bin);
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
		fprintf(stderr, "Failed to open file '%s' (%s)\n",
				fname, bfd_errmsg(bfd_get_error()));
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
	asection *bfd_sec;
	struct section *sec;

	for (bfd_sec = bfd_h->sections; bfd_sec != NULL; bfd_sec = bfd_sec->next) {
		sec = section_init();
		if (bfd_sec->flags & SEC_NO_FLAGS) {
			sec->type = SEC_TYPE_NONE;
		} else if (bfd_sec->flags & SEC_DATA) {
			sec->type = SEC_TYPE_DATA;
		} else if (bfd_sec->flags & SEC_CODE) {
			sec->type = SEC_TYPE_CODE;
		}
		sec->name = (char *) malloc(sizeof(bfd_section_name(bfd_sec)) + 1);
		if (sec->name) {
			strncpy(sec->name, bfd_section_name(bfd_sec), sizeof(bfd_section_name(bfd_sec)));
			sec->name[sizeof(bfd_section_name(bfd_sec))] = '\0';
		}
		sec->vma = bfd_section_vma(bfd_sec);
		sec->size = bfd_section_size(bfd_sec);
		sec->bytes = (uint8_t *) malloc(sec->size);
		if (sec->bytes) {
			if (!bfd_get_section_contents(bfd_h, bfd_sec, sec->bytes, 0, sec->size)) {
				section_destroy(sec);
				continue;
			}
		}

		if (!bin->sections) {
			bin->sections = llist_init((void *) sec);
		} else {
			llist_append(bin->sections, (void *) sec);
		}
	}

}

static void
load_symbols(bfd *bfd_h, struct binary *bin)
{
	asymbol **sym_tab;
	long nbytes, nsyms, i;
	struct symbol *sym;

	nbytes = bfd_get_symtab_upper_bound (bfd_h);
	if (nbytes < 0) {
		fprintf(stderr, "failed to read symtab (%s)\n", bfd_errmsg(bfd_get_error()));
	} else if (nbytes) {
		sym_tab = (asymbol **) malloc(nbytes);
		if (sym_tab) {
			nsyms = bfd_canonicalize_symtab(bfd_h, sym_tab);
			if (nsyms < 0) {
				fprintf(stderr, "failed to read symtab (%s)\n", bfd_errmsg(bfd_get_error()));
			}
			for (i = 0; i < nsyms; ++i) {
				sym = symbol_init();
				sym_set_type(sym_tab[i], sym);
				sym->name = (char *) malloc(strlen(sym_tab[i]->name) + 1);
				if (sym->name) {
					strncpy(sym->name, sym_tab[i]->name, strlen(sym_tab[i]->name));
					sym->name[strlen(sym_tab[i]->name)] = '\0';
				}
				sym->addr = bfd_asymbol_value(sym_tab[i]);

				if (!bin->symbols) {
					bin->symbols = llist_init((void *) sym);
				} else {
					llist_append(bin->symbols, (void *) sym);
				}
			}
			free(sym_tab);
		}
	}
}

static void
load_dyn_symbols(bfd *bfd_h, struct binary *bin)
{
	asymbol **sym_tab;
	long nbytes, nsyms, i;
	struct symbol *sym;

	nbytes = bfd_get_dynamic_symtab_upper_bound(bfd_h);
	if (nbytes < 0) {
		fprintf(stderr, "failed to read dynamic symtab (%s)\n", bfd_errmsg(bfd_get_error()));
	} else if (nbytes) {
		sym_tab = (asymbol **) malloc(nbytes);
		if (sym_tab) {
			nsyms = bfd_canonicalize_dynamic_symtab(bfd_h, sym_tab);
			if (nsyms < 0) {
				fprintf(stderr, "failed to read dynamic symtab (%s)\n", bfd_errmsg(bfd_get_error()));
			}
			for (i = 0; i < nsyms; ++i) {
				sym = symbol_init();
				sym_set_type(sym_tab[i], sym);
				sym->name = (char *) malloc(strlen(sym_tab[i]->name) + 1);
				if (sym->name) {
					strncpy(sym->name, sym_tab[i]->name, strlen(sym_tab[i]->name));
					sym->name[strlen(sym_tab[i]->name)] = '\0';
				}
				sym->addr = bfd_asymbol_value(sym_tab[i]);

				if (!bin->symbols) {
					bin->symbols = llist_init((void *) sym);
				} else {
					llist_append(bin->symbols, (void *) sym);
				}
			}
			free(sym_tab);
		}
	}

}

static void
sym_set_type(asymbol *bfd_sym, struct symbol *sym)
{
	if (bfd_sym->flags & BSF_FUNCTION) {
		sym->type = SYM_TYPE_FUNC;
	} else if (bfd_sym->flags & BSF_SECTION_SYM) {
		sym->type = SYM_TYPE_SEC;
	} else if (bfd_sym->flags & BSF_LOCAL) {
		sym->type = SYM_TYPE_LOCAL;
	} else if (bfd_sym->flags & BSF_GLOBAL) {
		sym->type = SYM_TYPE_GLOBAL;
	} else if (bfd_sym->flags & BSF_GLOBAL) {
		sym->type = SYM_TYPE_DYNAMIC;
	}
}

static struct binary *
binary_init(void)
{
	return (struct binary *) calloc(1, sizeof(struct binary));
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
	llist_destroy(bin->sections, llist_sec_dtor);
	llist_destroy(bin->symbols, llist_sym_dtor);
	free(bin);
}

static void
section_destroy(struct section *sec)
{
	if (sec->name) free(sec->name);
	if (sec->bytes) free(sec->bytes);
	free(sec);
}

static void
symbol_destroy(struct symbol *sym)
{
	if (sym->name) free(sym->name);
	free(sym);
}

static void
llist_sec_dtor(void *data)
{
	section_destroy((struct section *) data);
}

static void
llist_sym_dtor(void *data)
{
	symbol_destroy((struct symbol *) data);
}

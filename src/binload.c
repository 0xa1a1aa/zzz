/*
 * This is a workaround. See:
 * https://stackoverflow.com/questions/11748035/binutils-bfd-h-wants-config-h-now
 */
#define PACKAGE "binload"
#define PACKAGE_VERSION "1.0.0"

#include <string.h>
#include "bfd.h"
#include "binload.h"
#include "llist.h"

static struct binary *binary_init(void);
static struct section *section_init(void);
static struct symbol *symbol_init(void);

struct binary *
binary_load(char *fname)
{
	static int init;
	bfd *bfd_h;
	struct binary *bin;

	init = 0;
	if (!init) {
		init = 1;
		bfd_init();
	}
	bfd_h = bfd_openr(fname, NULL);
	if (!bfd_h) {
		return NULL;
	}
	bin = binary_init();


	bfd_close(bfd_h);
	return bin;
}

void
binary_unload(struct binary *bin)
{
	// free sections
	// free symbols
	free(bin);
}

struct section *
binary_get_text_section(struct binary *bin)
{
	struct llist *llist_sec;
	struct section *sec;

	llist_sec = bin->sections;
	while(llist_sec) {
		sec = (struct section *)llist_sec->data;
		if (strstr(sec->name, "text") != NULL) {
			return sec;
		}
		llist_sec = llist_sec->next;
	}
	return NULL;
}

static struct binary *
binary_init(void)
{
	return (struct binary *)malloc(sizeof(struct binary));
}

static struct section *
section_init(void)
{
	return (struct section *)malloc(sizeof(struct section));
}

static struct symbol *
symbol_init(void)
{
	return (struct symbol *)malloc(sizeof(struct symbol));
}

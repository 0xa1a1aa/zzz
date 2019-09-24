#include <stdio.h>
#include "binload.h"

int
main(int argc, char *argv[])
{
	struct binary *bin;
	struct llist *llist_sec;
	struct llist *llist_sym;
	struct section *sec;
	struct symbol *sym;

	if (argc < 2) {
		printf("Usage: %s <binary>\n", argv[0]);
		return 1;
	}

	bin = binary_load(argv[1]);

	printf("-----[ Binary ]-----\n");
	printf("name: %s\n", bin->name);
	printf("entry: %p\n", bin->entry);
	printf("type_str: %s\n", bin->type_str);

	printf("\n-----[ Sections ]-----\n");
	for (llist_sec = bin->sections; llist_sec != NULL; llist_sec = llist_sec->next) {
		sec = (struct section *) llist_sec->data;
		printf("%s\n", sec->name);
	}

	printf("\n-----[ Function symbols ]-----\n");
	for (llist_sym = bin->symbols; llist_sym != NULL; llist_sym = llist_sym->next) {
		sym = (struct symbol *) llist_sym->data;
		if (sym->type == SYM_TYPE_FUNC) {
			printf("%s\n", sym->name);
		}
	}

	binary_unload(bin);

	return 0;
}

#include <stdio.h>
#include "binload.h"

int
main(void)
{
	struct binary *bin;
	struct llist *llist_sec;
	struct section *sec;

	bin = binary_load("/bin/ls");
	printf("name: %s\n", bin->name);
	printf("entry: %p\n", bin->entry);
	printf("type_str: %s\n", bin->type_str);

	for (llist_sec = bin->sections; llist_sec != NULL; llist_sec = llist_sec->next) {
		sec = (struct section *) llist_sec->data;
		printf("-----[ Section ]-----\n");
		printf("Name: %s\n", sec->name);
	}

	binary_unload(bin);

	return 0;
}


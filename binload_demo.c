#include <stdio.h>
#include "binload.h"

int
main(void)
{
	struct binary *bin;

	bin = binary_load("/bin/ls");
	printf("name: %s\n", bin->name);
	printf("entry: %p\n", bin->entry);
	printf("type_str: %s\n", bin->type_str);
	binary_unload(bin);

	return 0;
}


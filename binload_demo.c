#include <stdio.h>
#include "binload.h"

int
main(void)
{
	struct binary *bin;

	bin = binary_load("/bin/ls");
	printf("name: %s\n", bin->name);
	binary_unload(bin);

	return 0;
}


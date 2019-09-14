#include <stdio.h>
#include "binload.h"

int
main(void)
{
	struct binary *bin;

	bin = binary_load("/bin/ls");
	//printf("%p\n", bin);

	return 0;
}


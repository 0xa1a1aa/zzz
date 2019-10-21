#include <stdio.h>
#include <stdint.h>
#include <capstone/capstone.h>
#include "binload.h"

void print_ins(cs_insn *ins, size_t nins);

int
main(int argc, char *argv[])
{
	csh cs_h;
	cs_insn *ins;
	size_t nins;
	int err;
	struct binary *bin;
	struct section *text_sec;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <binary>\n", argv[0]);
		return 1;
	}

	err = 0;
	cs_h = 0;

	bin = binary_load(argv[1]);
	if (bin->type != BIN_TYPE_ELF ||
		bin->arch != ARCH_X86) {
		fprintf(stderr, "Unsupported binary type\n");
		err = 2;
		goto exit;
	}

	text_sec = binary_get_section_by_name(bin, "text");
	if (!text_sec) {
		fprintf(stderr, "Failed to get text section\n");
		err = 3;
		goto exit;
	}

	if (cs_open(CS_ARCH_X86,
				bin->bits == 32 ? CS_MODE_32 : CS_MODE_64,
				&cs_h) != CS_ERR_OK) {
		fprintf(stderr, "Capstone init failed\n");
		err = 4;
		goto exit;
	}

	nins = cs_disasm(cs_h,
					 text_sec->bytes,
					 text_sec->size,
					 text_sec->vma,
					 0, // num of ins to disasm, 0 for all
					 &ins);
	if (!nins) {
		fprintf(stderr, "Disassembly failed\n");
		err = 5;
		goto exit;
	} else {
		print_ins(ins, nins);
		cs_free(ins, nins);
	}

exit:
	if (cs_h) cs_close(&cs_h);
	binary_unload(bin);

	return err;
}

void
print_ins(cs_insn *ins, size_t nins)
{
#define X86_INS_MAX_BYTES 15

	size_t i, j;

	for (i = 0; i < nins; ++i) {
		printf("0x%"PRIx64":\t", ins[i].address);
		for (j = 0; j < X86_INS_MAX_BYTES + 1; ++j) {
			if (j < ins[i].size) {
				printf("%02x ", ins[i].bytes[j]);
			} else {
				printf("   ");
			}
		}
		printf("\t%s", ins[i].mnemonic);
		printf("\t%s\n", ins[i].op_str);
	}
}

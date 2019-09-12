#ifndef __ZZZ__
#define __ZZZ__

#include <stdint.h>
#include "x86_64.h"

#define ins_is_cf(ins) \
	// return if ins is a control flow instruction

struct ins {
	int arch;
	union opcode;
	operands;
};

enum arch {
	x86_64
}

/*
 * Disassemble bytestream to ins
 *
 * @params:
 * 	addr: start address of disassembly
 * 	instr: address to struct ins where the disassembled instruction gets stored
 * 	arch: machine architecture
 * @returns:
 * 	address of byte following disassembled instruction
 */
uint8_t *zzz_bytestream_to_ins(uint8_t *addr, struct ins *instr, int arch);

/*
 * Decodes instruction to assembly string
 *
 * @params:
 * 	ins: instruction to decode
 * 	buf: buffer used to store the decoded instruction
 * @returns:
 * 	
 */
uint8_t zzz_decode_ins(struct *ins, char *buf);

/*
 * Checks if instruction is of type control-flow
 *
 * @params:
 * 	ins: instruction to check
 * @returns:
 */
uint8_t zzz_ins_is_cf(struct *ins);


#endif /* __ZZZ__ */

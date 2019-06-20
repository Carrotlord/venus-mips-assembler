#ifndef ENCODE_H
#define ENCODE_H

#include <stdio.h>

/** Instructions are 32-bit integers */
typedef unsigned int instr_t;

instr_t writeOpcode(instr_t command, unsigned char opcode);
instr_t writeRegSource(instr_t command, unsigned char regID);
instr_t writeRegTemp(instr_t command, unsigned char regID);
instr_t writeRegDest(instr_t command, unsigned char regID);
instr_t writeShiftAmount(instr_t command, unsigned char shiftAmount);
instr_t writeFunct(instr_t command, unsigned char funct);
instr_t writeImmediate(instr_t command, unsigned short imm);
instr_t writeAddress(instr_t command, unsigned int addr);

#endif
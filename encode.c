#include "encode.h"

/** Opcode is the leftmost 6-bit field. */
instr_t writeOpcode(instr_t command, unsigned char opcode) {
    opcode &= 0x3F;
    command &= 0x03FFFFFF;
    command += opcode << 26;
    return command;
}

/** RS is the leftmost 5-bit register field. */
instr_t writeRegSource(instr_t command, unsigned char regID) {
    regID &= 0x1F;
    command &= 0xFC1FFFFF;
    command += regID << 21;
    return command;
}

/** RT is the second leftmost 5-bit register field. */
instr_t writeRegTemp(instr_t command, unsigned char regID) {
    regID &= 0x1F;
    command &= 0xFFE0FFFF;
    command += regID << 16;
    return command;
}

/** RD is the rightmost 5-bit register field. */
instr_t writeRegDest(instr_t command, unsigned char regID) {
    regID &= 0x1F;
    command &= 0xFFFF07FF;
    command += regID << 11;
    return command;
}

instr_t writeShiftAmount(instr_t command, unsigned char shiftAmount) {
    shiftAmount &= 0x1F;
    command &= 0xFFFFF83F;
    command += shiftAmount << 6;
    return command;
}

instr_t writeFunct(instr_t command, unsigned char funct) {
    funct &= 0x3F;
    command &= 0xFFFFFFC0;
    command += funct;
    return command;
}

/** Writes a 16-bit immediate. */
instr_t writeImmediate(instr_t command, unsigned short imm) {
    command &= 0xFFFF0000;
    command += imm;
    return command;
}

/** Writes a 26-bit address. */
instr_t writeAddress(instr_t command, unsigned int addr) {
    command &= 0xFC000000;
    addr &= 0x03FFFFFF;
    command += addr;
    return command;
}

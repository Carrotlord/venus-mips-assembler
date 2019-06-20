#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "encode.h"
#include "strtools.h"
#include "hashmap.h"
#include "fileio.h"

const unsigned int START_ADDR;

char isLabel(char *string);

struct Assembled {
    instr_t encoded;
    char success;
    instr_t *others;
    unsigned char numOthers;
};

void errorMessage(char *message);
char compileRegister(char *token);
instr_t tripleRegisterOperation(char delimiters[]);
instr_t shiftInstruction(char delimiters[]);
instr_t twoRegsWithImmediate(char delimiters[], char isBranch,
                             Hashmap labels, unsigned int address);
instr_t jumpAddress(char *delimiters, Hashmap labels);
char *getLabel(char *labeledLine);
Hashmap collectLabelAddresses(char **lines, unsigned int numLines);
void printLabelDump(char *fileName);
struct Assembled loadAddress(char delimiters[], Hashmap labels);
instr_t loadUpperImmediate(char *delimiters);
instr_t jumpRegister(char *delimiters);
instr_t memoryOperation(char *delimiters);
struct Assembled assembleLine(char *line, Hashmap labels, unsigned int address);
char **insertNops(char **strings, unsigned int numOrigLines,
                  unsigned int *resultLines);
void assembleFile(char *sourceFile, char *compiledFile);

#endif
#ifndef FILEIO_H
#define FILEIO_H

#include <stdlib.h>
#include "encode.h"

int getFileSize(FILE *filePtr);
char *getFileData(FILE *filePtr);
char writeFileData(FILE *filePtr, char data[],
                   unsigned int amount);
void writeFileMIPS(FILE *filePtr, instr_t data[], unsigned int amount);
instr_t *readFileMIPS(FILE *filePtr);
void printFileAsHex(char fileName[]);
void printFileMIPS(char fileName[]);

#endif
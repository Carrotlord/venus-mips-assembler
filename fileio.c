#include "fileio.h"

/** Returns a file's size in bytes
 */
int getFileSize(FILE *filePtr) {
    fseek(filePtr, 0, SEEK_END);
    int size = ftell(filePtr);
    fseek(filePtr, 0, SEEK_SET);
    return size;
}

/** Writes a files contents into memory (as bytes).
 *  Uses malloc.
 */
char *getFileData(FILE *filePtr) {
    int size = getFileSize(filePtr);
    char *data = malloc(size + 1);
    fread(data, sizeof(char), size, filePtr);
    /* Store a 0x00 at the end of the stream: */
    data[size] = 0;
    return data;
}

char writeFileData(FILE *filePtr, char data[],
                   unsigned int amount) {
    fwrite(data, sizeof(char), amount / sizeof(char), filePtr);
    return '\0';
}

void writeFileMIPS(FILE *filePtr, instr_t data[], unsigned int amount) {
    fwrite(data, sizeof(instr_t), amount, filePtr);
}

instr_t *readFileMIPS(FILE *filePtr) {
    int size = getFileSize(filePtr);
    int amount = size / sizeof(instr_t);
    instr_t *data = malloc(size);
    fread(data, sizeof(instr_t), amount, filePtr);
    return data;
}

void printFileAsHex(char fileName[]) {
    printf("%s:\n", fileName);
    FILE *filePtr = fopen(fileName, "rb");
    if (filePtr != NULL) {
        int size = getFileSize(filePtr);
        char *data = getFileData(filePtr);
        printHex(data, size);
        free(data);
        fclose(filePtr);
    } else {
        printf("No such file: %s\n", fileName);
    }
}

void printFileMIPS(char fileName[]) {
    printf("%s:\n", fileName);
    FILE *filePtr = fopen(fileName, "rb");
    if (filePtr != NULL) {
        int size = getFileSize(filePtr);
        int amount = size / sizeof(instr_t);
        instr_t *data = readFileMIPS(filePtr);
        int i;
        for (i = 0; i < amount; ++i) {
            printf("%x\n", data[i]);
        }
        free(data);
    } else {
        printf("No such file: %s\n", fileName);
    }
}

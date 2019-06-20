#include "assemble.h"

const unsigned int START_ADDR = 0x100000;

char isLabel(char *string) {
    return endsWith(string, ':');
}

void errorMessage(char *message) {
    printf("Error: %s\n", message);
    exit(EXIT_FAILURE);
}

char compileRegister(char *token) {
    if (token == NULL) {
        errorMessage("Not enough registers.");
    } else if (token[0] != '$') {
        printf("Error: Illegal register %s\n", token);
        exit(EXIT_FAILURE);
    } else {
        /* Chop off the dollar sign. */
        char *withoutDollar = token;
        ++withoutDollar;
        if (isLowercaseLetter(withoutDollar[0])) {
            /* Registers specified with letters,
             * such as $t0.
             */
            switch (withoutDollar[0]) {
                case 'a':
                    if (withoutDollar[1] == 't') {
                        return 1;
                    } else {
                        return 4 + atoi(withoutDollar + 1);
                    }
                case 'f':
                    return 30;
                case 'g':
                    return 28;
                case 'k':
                    return 26 + atoi(withoutDollar + 1);
                case 'r':
                    return 31;
                case 's':
                    if (withoutDollar[1] == 'p') {
                        return 29;
                    } else {
                        return 16 + atoi(withoutDollar + 1);
                    }
                case 't': {
                    int specifier = atoi(withoutDollar + 1);
                    if (specifier >= 8) {
                        /* Return 24 + specifier - 8 */
                        return 16 + specifier;
                    } else {
                        return 8 + specifier;
                    }
                } case 'v':
                    return 2 + atoi(withoutDollar + 1);
                case 'z':
                    return 0;
                default:
                    return 0;
            }
        } else {
            /* Registers specified with numbers,
             * such as $31.
             */
            if (isAllDigits(withoutDollar)) {
                return atoi(withoutDollar);
            }
        }
    }
}

/** Generates a binary skeleton
 *  For all instructions of the format:
 *  R_DEST = R_SRC op R_TEMP
 *  where op is an operator.
 */
instr_t tripleRegisterOperation(char delimiters[]) {
    instr_t command = 0;
    int i;
    for (i = 0; i < 3; ++i) {
        char *nextToken = strtok(NULL, delimiters);
        /* compileRegister will automatically
         * detect when nextToken equals NULL.
         */
        char regCode = compileRegister(nextToken);
        switch (i) {
            case 0:
                command = writeRegDest(command, regCode);
                break;
            case 1:
                command = writeRegSource(command, regCode);
                break;
            case 2:
                command = writeRegTemp(command, regCode);
                break;
            default:
                break;
        }
    }
    return command;
}

/** Generates a binary skeleton
 *  For all instructions of the format:
 *  R_DEST = R_TEMP shifted 5_BIT_IMMEDIATE
 */
instr_t shiftInstruction(char delimiters[]) {
    instr_t command = 0;
    int i;
    for (i = 0; i < 2; ++i) {
        char *nextToken = strtok(NULL, delimiters);
        char regCode = compileRegister(nextToken);
        switch (i) {
            case 0:
                command = writeRegDest(command, regCode);
                break;
            case 1:
                command = writeRegTemp(command, regCode);
                break;
            default:
                break;
        }
    }
    /* No R_SRC is needed */
    command = writeRegSource(command, 0x0);
    char *shiftAmt = strtok(NULL, delimiters);
    if (shiftAmt == NULL) {
        printf("No shift amount detected.\n");
        exit(EXIT_FAILURE);
    } else {
        int DETECT_BASE = 0;
        unsigned char immediate = (unsigned char)
                                  strtol(shiftAmt, NULL, DETECT_BASE);
        command = writeShiftAmount(command, immediate);
    }
    return command;
}

/** Generates a binary skeleton
 *  For all instructions of the format:
 *  R_TEMP = R_SRC op IMMEDIATE_VALUE
 *  where op is an operator.
 */
instr_t twoRegsWithImmediate(char delimiters[], char isBranch,
                             Hashmap labels, unsigned int address) {
    instr_t command = 0;
    int i;
    /* 2 registers: */
    for (i = 0; i < 2; ++i) {
        char *nextToken = strtok(NULL, delimiters);
        char regCode = compileRegister(nextToken);
        /* Branches have R_SRC first: */
        switch (i) {
            case 0:
                if (isBranch) {
                    command = writeRegSource(command, regCode);
                } else {
                    command = writeRegTemp(command, regCode);
                }
                break;
            case 1:
                if (isBranch) {
                    command = writeRegTemp(command, regCode);
                } else {
                    command = writeRegSource(command, regCode);
                }
                break;
            default:
                break;
        }
    }
    /* 1 immediate: */
    char *immToken = strtok(NULL, delimiters);
    if (immToken == NULL) {
        printf("No immediate value detected.\n");
        exit(EXIT_FAILURE);
    } else if (hashmapHasKey(labels, immToken)) {
        /* Label_Address - (Current_Address + 1) */
        int relativeBranchOffset = hashmapGetValue(labels, immToken) - address - 1;
        unsigned short immediate = (unsigned short) relativeBranchOffset;
        command = writeImmediate(command, immediate);
    } else {
        int DETECT_BASE = 0;
        unsigned short immediate = (unsigned short)
                                    strtol(immToken, NULL, DETECT_BASE);
        command = writeImmediate(command, immediate);
    }
    return command;
}

/** Generates a binary skeleton for all instructions
 *  of the form:
 *  JUMP ADDRESS
 *  Where address is a constant or a label.
 */
instr_t jumpAddress(char *delimiters, Hashmap labels) {
    instr_t command = 0;
    char *target = strtok(NULL, delimiters);
    if (target == NULL) {
        printf("No address or label specified for jump.\n");
        exit(EXIT_FAILURE);
    } else if (hashmapHasKey(labels, target)) {
        unsigned int address = hashmapGetValue(labels, target);
        command = writeAddress(command, address);
    } else {
        int DETECT_BASE = 0;
        unsigned int address = (unsigned int)
                               strtol(target, NULL, DETECT_BASE);
        command = writeAddress(command, address);
    }
    return command;
}

/** Removes the colon at the end of a label. */
char *getLabel(char *labeledLine) {
    char *label = malloc(sizeof(char) * strlen(labeledLine));
    int i;
    for (i = 0; labeledLine[i] != ':' && labeledLine[i] != '\0'; ++i) {
        label[i] = labeledLine[i];
    }
    label[i] = '\0';
    return label;
}

/** Collects addresses of the labels used
 *  in the ASM source file.
 *  All addresses are divided by 4, since MIPS
 *  uses 4-byte width instructions.
 */
Hashmap collectLabelAddresses(char **lines, unsigned int numLines) {
    int i;
    /* Starting address, divided by 4. */
    unsigned int address = START_ADDR;
    Hashmap directory = hashmapCreate();
    /* Do not use strtok, as it will destroy the strings. */
    for (i = 0; i < numLines; ++i) {
        char *current = lines[i];
        if (isLabel(current)) {
            Binding keyVal = {.key = getLabel(current), .value = address};
            directory = hashmapAdd(directory, keyVal);
        } else if (startsWithTrimmed(current, "la ") ||
                   startsWithTrimmed(current, "la\t")) {
            address += 2;
        } else if (!isWhitespace(current)) {
            ++address;
        }
        /* Do nothing if a line is whitespace. */
    }
    return directory;
}

/** Prints labels and associated addresses
 *  in a hashmap format.
 *  Do not remove this function. It is a feature
 *  of the assembler (use option -l)
 */
void printLabelDump(char *fileName) {
    printf("%s:\n", fileName);
    FILE *fPtr = fopen(fileName, "rb");
    char *commands = getFileData(fPtr);
    char *converted = unixifyNewlines(commands);
    char *uncommented = eraseComments(converted, '#');
    unsigned int numLines = countLines(uncommented);
    char **strings = splitLines(uncommented);
    Hashmap labels = collectLabelAddresses(strings, numLines);
    hashmapPrint(labels, 0);
    hashmapDestroy(&labels, 1);
    fclose(fPtr);
    free(strings);
    free(uncommented);
    free(converted);
    free(commands);
}

/** Generates encoding for psuedoinstruction la (load address).
 */
struct Assembled loadAddress(char delimiters[], Hashmap labels) {
    struct Assembled result;
    result.encoded = 0;
    result.success = 1;
    result.others = malloc(sizeof(instr_t));
    result.numOthers = 1;
    /* Grab register and label address */
    char *reg = strtok(NULL, delimiters);
    char regCode = compileRegister(reg);
    char *label = strtok(NULL, delimiters);
    if (label == NULL) {
        errorMessage("No label given for load address.");
    } else if (hashmapHasKey(labels, label)) {
        /* Addresses are byte aligned, so multiply by 4. */
        unsigned int address = hashmapGetValue(labels, label) << 2;
        unsigned short lowerHalf = address & 0xFFFF;
        unsigned short upperHalf = (address & 0xFFFF0000) >> 16;
        /* Generate lui $register, upperHalf */
        instr_t loadUpperImm = 0;
        loadUpperImm = writeOpcode(loadUpperImm, 0xF);
        loadUpperImm = writeRegSource(loadUpperImm, 0x0);
        loadUpperImm = writeRegTemp(loadUpperImm, regCode);
        loadUpperImm = writeImmediate(loadUpperImm, upperHalf);
        result.encoded = loadUpperImm;
        /* Generate ori $register, $register, lowerHalf */
        instr_t orLowerImm = 0;
        orLowerImm = writeOpcode(orLowerImm, 0xD);
        orLowerImm = writeRegSource(orLowerImm, regCode);
        orLowerImm = writeRegTemp(orLowerImm, regCode);
        orLowerImm = writeImmediate(orLowerImm, lowerHalf);
        *(result.others) = orLowerImm;
    } else {
        printf("Label %s is not defined.", label);
        exit(EXIT_FAILURE);
    }
    return result;
}

/** Generates encoding for lui */
instr_t loadUpperImmediate(char *delimiters) {
    instr_t command = 0;
    char *reg = strtok(NULL, delimiters);
    char regCode = compileRegister(reg);
    char *imm = strtok(NULL, delimiters);
    if (imm == NULL) {
        errorMessage("No immediate operand for lui.");
    } else {
        int DETECT_BASE = 0;
        unsigned short immValue = (unsigned short)
                                  strtol(imm, NULL, DETECT_BASE);
        command = writeImmediate(command, immValue);
        command = writeRegTemp(command, regCode);
        command = writeRegSource(command, 0x0);
    }
    return command;
}

/** Generates encoding for jr */
instr_t jumpRegister(char *delimiters) {
    instr_t command = 0;
    char *reg = strtok(NULL, delimiters);
    char regCode = compileRegister(reg);
    command = writeRegSource(command, regCode);
    command = writeRegDest(command, 0x0);
    command = writeRegTemp(command, 0x0);
    command = writeShiftAmount(command, 0x0);
    return command;
}

/** Generates encoding for any memory operation:
 *  MEMORY_OP $RT, offset($RS)
 *  Where the effective memory address is
 *  offset + $RS (byte aligned, so offsets will
 *                be in multiples of 4 when loading
 *                words)
 *  The transfer register is $RT,
 *  which is written to during a load operation,
 *  and read during a store operation.
 */
instr_t memoryOperation(char *delimiters) {
    char *regTransfer = strtok(NULL, delimiters);
    char *immOffset = strtok(NULL, delimiters);
    char *regBase = strtok(NULL, delimiters);
    printf("RT: %s, RS: %s, OFF: %s\n", regTransfer, regBase, immOffset);
    return 0;
}

/** Converts a line of MIPS assembler
 *  code into a 32-bit raw instruction.
 */
struct Assembled assembleLine(char *line, Hashmap labels, unsigned int address) {
    char delimiters[] = " ,\t";
    char memDelimiters[] = " ,\t()";
    struct Assembled result;
    result.encoded = 0;
    result.success = 0;
    result.others = NULL;
    result.numOthers = 0;
    if (isLabel(line)) {
        return result;
    }
    char *token = strtok(line, delimiters);
    if (token != NULL) {
        if (strcmp(token, "add") == 0) {
            instr_t command = tripleRegisterOperation(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeShiftAmount(command, 0x0);
            command = writeFunct(command, 0x20);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "addi") == 0) {
            instr_t command = twoRegsWithImmediate(delimiters, 0, labels, address);
            command = writeOpcode(command, 0x8);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "addiu") == 0) {
            instr_t command = twoRegsWithImmediate(delimiters, 0, labels, address);
            command = writeOpcode(command, 0x9);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "addu") == 0) {
            instr_t command = tripleRegisterOperation(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeShiftAmount(command, 0x0);
            command = writeFunct(command, 0x21);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "and") == 0) {
            instr_t command = tripleRegisterOperation(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeShiftAmount(command, 0x0);
            command = writeFunct(command, 0x24);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "andi") == 0) {
            instr_t command = twoRegsWithImmediate(delimiters, 0, labels, address);
            command = writeOpcode(command, 0xC);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "beq") == 0) {
            instr_t command = twoRegsWithImmediate(delimiters, 1, labels, address);
            command = writeOpcode(command, 0x4);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "bne") == 0) {
            instr_t command = twoRegsWithImmediate(delimiters, 1, labels, address);
            command = writeOpcode(command, 0x5);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "j") == 0) {
            instr_t command = jumpAddress(delimiters, labels);
            command = writeOpcode(command, 0x2);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "jal") == 0) {
            instr_t command = jumpAddress(delimiters, labels);
            command = writeOpcode(command, 0x3);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "jr") == 0) {
            instr_t command = jumpRegister(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeFunct(command, 0x8);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "la") == 0) {
            result = loadAddress(delimiters, labels);
        } else if (strcmp(token, "lbu") == 0) {
            /* TODO : Finish lbu (and other load/store instructions) */
            memoryOperation(memDelimiters);
        } else if (strcmp(token, "lui") == 0) {
            instr_t command = loadUpperImmediate(delimiters);
            command = writeOpcode(command, 0xF);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "nor") == 0) {
            instr_t command = tripleRegisterOperation(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeShiftAmount(command, 0x0);
            command = writeFunct(command, 0x27);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "nop") == 0) {
            result.encoded = 0x0;
            result.success = 1;
        } else if (strcmp(token, "or") == 0) {
            instr_t command = tripleRegisterOperation(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeShiftAmount(command, 0x0);
            command = writeFunct(command, 0x25);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "ori") == 0) {
            instr_t command = twoRegsWithImmediate(delimiters, 0, labels, address);
            command = writeOpcode(command, 0xD);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "slt") == 0) {
            instr_t command = tripleRegisterOperation(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeShiftAmount(command, 0x0);
            command = writeFunct(command, 0x2A);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "slti") == 0) {
            instr_t command = twoRegsWithImmediate(delimiters, 0, labels, address);
            command = writeOpcode(command, 0xA);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "sltiu") == 0) {
            instr_t command = twoRegsWithImmediate(delimiters, 0, labels, address);
            command = writeOpcode(command, 0xB);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "sltu") == 0) {
            instr_t command = tripleRegisterOperation(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeShiftAmount(command, 0x0);
            command = writeFunct(command, 0x2B);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "sll") == 0) {
            instr_t command = shiftInstruction(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeFunct(command, 0x0);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "srl") == 0) {
            instr_t command = shiftInstruction(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeFunct(command, 0x2);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "sub") == 0) {
            instr_t command = tripleRegisterOperation(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeShiftAmount(command, 0x0);
            command = writeFunct(command, 0x22);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "subu") == 0) {
            instr_t command = tripleRegisterOperation(delimiters);
            command = writeOpcode(command, 0x0);
            command = writeShiftAmount(command, 0x0);
            command = writeFunct(command, 0x23);
            result.encoded = command;
            result.success = 1;
        } else if (strcmp(token, "syscall") == 0) {
            result.encoded = 0xC;
            result.success = 1;
        } else {
            printf("Sorry, command %s is not supported.\n", token);
        }
    }
    return result;
}

/** Inserts nops after jal
 */
char **insertNops(char **strings, unsigned int numOrigLines,
                  unsigned int *resultLines) {
    char **output = NULL;
    unsigned int numResultLines = 0;
    int stringPtrSize = sizeof(char **);
    int i, j;
    for (i = j = 0; i < numOrigLines; ++i) {
        if (startsWith(strings[i], "jal ")) {
            numResultLines += 2;
            output = realloc(output, numResultLines * stringPtrSize);
            output[j] = strCopy(strings[i]);
            output[j + 1] = strCopy("nop");
            j += 2;
        } else {
            ++numResultLines;
            output = realloc(output, numResultLines * stringPtrSize);
            output[j] = strCopy(strings[i]);
            ++j;
        }
    }
    *resultLines = numResultLines;
    return output;
}

void assembleFile(char *sourceFile, char *compiledFile) {
    FILE *asmSource = fopen(sourceFile, "rb");
    char *commands = getFileData(asmSource);
    char *converted = unixifyNewlines(commands);
    char *uncommented = eraseComments(converted, '#');
    unsigned int numLines = countLines(uncommented);
    char **apart = splitLines(uncommented);
    int resultLines;
    char **strings = insertNops(apart, numLines, &resultLines);
    Hashmap labels = collectLabelAddresses(strings, resultLines);
    int j, k;
    instr_t *destination = NULL;
    unsigned int totalSize = 0;
    for (j = k = 0; j < resultLines; ++j) {
        struct Assembled result = assembleLine(strings[j], labels, k + START_ADDR);
        if (result.success) {
            if (result.numOthers > 0) {
                /* This instruction is a psuedoinstruction
                 * that compiles to more than 1 MIPS 32-bit encoding.
                 */
                unsigned int numInstructions = 1 + result.numOthers;
                totalSize += numInstructions * sizeof(instr_t);
                destination = realloc(destination, totalSize);
                destination[k] = result.encoded;
                int m;
                for (m = 1; m < numInstructions; ++m) {
                    destination[k + m] = result.others[m - 1];
                }
                k += numInstructions;
                free(result.others);
            } else {
                /* The instruction compiles to
                 * 1 MIPS 32-bit encoding
                 */
                totalSize += sizeof(instr_t);
                destination = realloc(destination, totalSize);
                destination[k] = result.encoded;
                ++k;
            }
        }
    }
    fclose(asmSource);
    FILE *asmDest = fopen(compiledFile, "wb");
    fwrite(destination, sizeof(instr_t), k, asmDest);
    fclose(asmDest);
    printf("Assembled %s into %s.\n", sourceFile, compiledFile);
    hashmapDestroy(&labels, 1);
    free(strings);
    free(apart);
    free(uncommented);
    free(destination);
    free(converted);
    free(commands);
}

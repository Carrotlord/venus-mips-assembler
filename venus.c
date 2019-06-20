/** Venus MIPS Assembler
 *  By Jiangcheng Oliver Chu
 *  8/15/13
 */
 
/* TODO : Finish lbu (and other load/store instructions)
 *        See assemble.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encode.h"
#include "safety.h"
#include "hex.h"
#include "fileio.h"
#include "strtools.h"
#include "assemble.h"
#include "hashmap.h"

const int TEST_MAX = 0;
const int QUEUE_FAIL = 0;

void unitTests(int argc, char *argv[]) {
    printf("The following files will be assembled:\n");
    int i;
    /* We start at i = 1 to avoid the program's name. */
    for (i = 1; i < argc; ++i) {
        printf("%s\n", argv[i]);
    }

    instr_t command = 0x65010203;
    printf("%x\n", command);
    /* printHex(&command, 4); */
    command = writeOpcode(command, 63);
    printf("%x\n", command);
    /* printHex(&command, 4); */
    
    instr_t program[] = {command, command, command};
    FILE *filePtr = fopen("program.mips", "wb");
    writeFileMIPS(filePtr, program, 3);
    fclose(filePtr);
    
    filePtr = fopen("program.mips", "rb");    
    char *data = getFileData(filePtr);
    printHex(data, 12);
    free(data);
    
    char *strings[] = {"Hello.\nWorld", "Hello.\r\nWorld",
                       "Hello.\rWorld", "\r\n\r\n\r\n",
                       "add $t0,$t1,$t2\r\nadd $t3,$t4,300",
                       "add $t0,$t1,$t2\r\nadd $t3,$t4,300\r\n"};
    
    /* Note: i is declared above. */
    for (i = 0; i < sizeof(strings)/sizeof(strings[0]); ++i) {
        printf("Length: %d\n", countCharsNewlines(strings[i]));
    }

    for (i = 0; i < sizeof(strings)/sizeof(strings[0]); ++i) {
        char *converted = unixifyNewlines(strings[i]);
        printStringAsHex(converted);
        /* Tokenize the string and print the individual
         * lines.
         */
        int numLines = countLines(converted);
        char **strings = splitLines(converted);
        /* Converted was modified: */
        printStringAsHex(converted);
        int j;
        for (j = 0; j < numLines; ++j) {
            printf("Line: %p %s\n", *(strings + j), strings[j]);
        }
        free(converted);
    }

    printf("Pointer size: %d bytes.\n", sizeof(char *));

    char *registers[] = {"$ra", "$t0", "$t8", "$sp", "$s4"};
    
    for (i = 0; i < sizeof(registers)/sizeof(registers[0]); ++i) {
        printf("%s = %d\n", registers[i], compileRegister(registers[i]));
    }
    
    {
        /* Correct encodings are
         * 0x012a4020 (add $t0,$t1,$t2)
         * 0x03bf8020 (add $s0,$sp,$ra)
         */
        char *commands = "add $t0,$t1,$t2\r\n    add $s0,$sp,$ra";
        /* Used to be "add $t0,$t1,$t2\r\n    add $s0,$sp,$ra\r\n\tj label" */
        char *converted = unixifyNewlines(commands);
        int numLines = countLines(converted);
        char **strings = splitLines(converted);
        int j;
        Hashmap labels = hashmapCreate();
        for (j = 0; j < numLines; ++j) {
            printf("%s --> %x\n", strings[j], assembleLine(strings[j], labels, j).encoded);
        }
    }
}

void unitTests1(int argc, char *argv[]) {
    {
        /* Test whether comments are removed or not. */
        char *someString = "Alpha.#Beta\r\nGamma#Delta.";
        char *converted = unixifyNewlines(someString);
        printf("%s\n", converted);
        char *uncommented = eraseComments(converted, '#');
        printf("%s\n", uncommented);
        free(uncommented);
        free(converted);
    }
    printf("\n");
    {
        /* Test label detection. */
        char *labels = "a label:#Comment\r\nNot a label#:Comment:\r\nLABEL:\t";
        char *converted = unixifyNewlines(labels);
        printf("%s\n", converted);
        char *uncommented = eraseComments(converted, '#');
        /* Warning: You must count lines before splitting,
         * because splitLines affects the string it is passed.
         */
        unsigned int lines = countLines(uncommented);
        char **strings = splitLines(uncommented);
        int i;
        for (i = 0; i < lines; ++i) {
            if (isLabel(strings[i])) {
                printf("Label -> %s\n", strings[i]);
            } else {
                printf("Nonlabel -> %s\n", strings[i]);
            }
        }
        free(strings);
        free(uncommented);
        free(converted);
    }
    printf("\n");
    {
        Hashmap h = hashmapCreate();
        printf("At: %p, Length: %d\n", h.buckets, h.length);
    }
    printf("\n");
    {
        BindingList b = {.head = NULL, .tail = NULL};
        b.tail = &b;
        printf("%p = %p\n", &b, b.tail);
    }
    printf("\n");
}

void unitTests2(int argc, char *argv[]) {
    {
        Binding b0 = {.key = "Hello", .value = 50};
        Binding b1 = {.key = "Alpha", .value = 60};
        Binding b2 = {.key = "yeah", .value = -2};
        Binding b3 = {.key = "Beta", .value = -1};
        BindingList bAll = {.head = b0, .tail = NULL};
        bListAppend(&bAll, b1);
        bListAppend(&bAll, b2);

        printf("bAll:\n");
        bListPrint(&bAll);
        
        BindingList bFirst = {.head = b0, .tail = NULL};
        bListAppend(&bFirst, b3);
        BindingList bSecond = {.head = b1, .tail = NULL};
        bListAppend(&bSecond, b2);
        
        printf("bFirst, bSecond:\n");
        bListConcat(&bFirst, &bSecond);
        bListPrint(&bFirst);
        bListPrint(&bSecond);
        
        Hashmap h = hashmapCreate();

        h = hashmapAdd(h, b0);
        h = hashmapAdd(h, b1);
        h = hashmapAdd(h, b2);
        h = hashmapAdd(h, b3);

        printf("Hashmap (1):\n");
        hashmapPrint(h, 1);

        h = hashmapAdd(h, b0);
        h = hashmapAdd(h, b1);
        h = hashmapAdd(h, b2);
        h = hashmapAdd(h, b3);

        printf("Hashmap (2):\n");
        hashmapPrint(h, 0);
        
        /* Note: Be careful when testing bListDestroy
         * and hashmapDestroy at the same time.
         * They (might?) share heap pointers.
         */
        bListDestroy(&bAll);
        hashmapDestroy(&h, 1);
    }
}

void unitTests3(int argc, char *argv[]) {
    {
        char *toBeCopied = "Hello World!";
        char *copied = strCopy(toBeCopied);
        printf("%s : %s\n", toBeCopied, copied);
        free(copied);
    }
}

void printHelp() {
    printf("venus [asm source file] [FLAGS]\n");
    printf("\t-t [number]\tRun numbered tests starting at 0\n");
    printf("\t-h [file]\tRead file as hex pairs\n");
    printf("\t-i [file]\tRead file as 32-bit MIPS commands\n");
    printf("\t-o [file]\tOutput assembled binary into file\n");
    printf("\t-l [file]\tLabel dump of MIPS source file\n");
}

void runTest(int tNum, int argc, char *argv[]) {
    switch (tNum) {
        case 0:
            unitTests(argc, argv);
            break;
        case 1:
            unitTests1(argc, argv);
            break;
        case 2:
            unitTests2(argc, argv);
            break;
        case 3:
            unitTests3(argc, argv);
            break;
        default:
            printf("No test number %d\n", tNum);
            exit(EXIT_FAILURE);
            break;
    }
}

void checkArgs(int argc, char *argv[]) {
    if (argc == 1) {
        printHelp();
    } else {
        int i;
        int queued = QUEUE_FAIL;
        /* Start at 1 to avoid program name. */
        for (i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "-t") == 0) {
                if (i == argc - 1) {
                    /* Run all tests: */
                    int t;
                    for (t = 0; t < TEST_MAX + 1; ++t) {
                        runTest(t, argc, argv);
                    }
                } else {
                    /* FIXME WARNING: If another option
                     * or filename is placed after the
                     * -t flag, atoi will fail to parse. */
                    int tNum = atoi(argv[i + 1]);
                    runTest(tNum, argc, argv);
                    ++i;
                }
            } else if (strcmp(argv[i], "-h") == 0) {
                if (i == argc - 1) {
                    printf("No file given for -h\n");
                    exit(EXIT_FAILURE);
                } else {
                    printFileAsHex(argv[i + 1]);
                    ++i;
                }
            } else if (strcmp(argv[i], "-i") == 0) {
                if (i == argc - 1) {
                    printf("No file given for -i\n");
                    exit(EXIT_FAILURE);
                } else {
                    printFileMIPS(argv[i + 1]);
                    ++i;
                }
            } else if (strcmp(argv[i], "-l") == 0) {
                if (i == argc - 1) {
                    printf("No file given for -l\n");
                    exit(EXIT_FAILURE);
                } else {
                    printLabelDump(argv[i + 1]);
                    ++i;
                }
            } else if (strcmp(argv[i], "-o") == 0) {
                if (queued == QUEUE_FAIL) {
                    printf("No input file specified.\n");
                    exit(EXIT_FAILURE);
                } else if (i == argc - 1) {
                    printf("No output file specified.\n");
                    exit(EXIT_FAILURE);
                } else {
                    /* Source: argv[queued]
                     * Compiled: argv[i + 1]
                     */
                    assembleFile(argv[queued], argv[i + 1]);
                    queued = QUEUE_FAIL;
                    ++i;
                }
            } else {
                FILE *testing = fopen(argv[i], "rb");
                if (testing == NULL) {
                    printf("Sorry, option %s is not supported yet.\n",
                           argv[i]);
                } else {
                    /* This is the source file that
                     * will be assembled (the filename at index i)
                     */
                    queued = i;
                    fclose(testing);
                }
            }
        }
        if (queued != QUEUE_FAIL) {
            printf("Please use the -o flag when assembling file %s.\n",
                   argv[queued]);
        }
    }
}

int main(int argc, char *argv[]) {
    checkSize();
    checkArgs(argc, argv);
    return 0;
}
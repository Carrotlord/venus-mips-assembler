#ifndef STRTOOLS_H
#define STRTOOLS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

unsigned int countCharsNewlines(char *string);
void printStringAsHex(char *string);
char *unixifyNewlines(char *string);
unsigned int countLines(char *string);
char startsWith(char *string, char *begin);
char *findFront(char *string);
char startsWithTrimmed(char *string, char *begin);
char endsWith(char *string, char end);
int skipUntil(char *string, int startIndex, char until);
char *eraseComments(char *string, char comChar);
char **splitLines(char *string);
char isLowercaseLetter(char c);
char isDigit(char c);
char isAllDigits(char *string);
char isWhitespace(char *string);
char *strCopy(char *original);

#endif
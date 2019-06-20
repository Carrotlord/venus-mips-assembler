#ifndef HEX_H
#define HEX_H

#include <stdio.h>
#include <stdlib.h>

const unsigned int HEX_LINE;
const unsigned char MOST_SIGNIF_NYBBLE;
const unsigned char LEAST_SIGNIF_NYBBLE;

void printHex(char data[], unsigned int amount);
char isHexDigit(char c);
unsigned int countLegalHexPairs(char hexPairs[], unsigned int digits);
char hexDigitToNybble(char c);

struct Datums;
struct Datums hexToData(char hexPairs[], unsigned int digits);

#endif
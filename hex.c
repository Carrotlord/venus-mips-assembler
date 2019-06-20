#include "hex.h"

/** The number of hexadecimal pairs to be printed
 *  on a single line.
 */
const unsigned int HEX_LINE = 16;
const unsigned char MOST_SIGNIF_NYBBLE = 1;
const unsigned char LEAST_SIGNIF_NYBBLE = 0;

/** FIXME: On bytes such as 0xFD, will print 0xFFFFFFFD
 *  NOTE:  Use printf("%x\n", value) to print hex value
 *         of MIPS command with most significant byte
 *         first.
 */
void printHex(char data[], unsigned int amount) {
    char *digits = "0123456789abcdef";
    int counter, i;
    for (i = counter = 0; i < amount; ++i, ++counter) {
        if (counter >= HEX_LINE) {
            counter = 0;
            printf("\n");
        }
        /* In gcc, where chars are signed by default,
         * you must cast to unsigned char or else
         * right shift will carry bits.
         * Prints padded hex pairs: */
        unsigned char entire = (unsigned char) data[i];
        unsigned char upper = entire >> 4;
        unsigned char lower = entire & 0x0F;
        printf("%c%c ", digits[upper], digits[lower]);
    }
    printf("\n");
}

char isHexDigit(char c) {
    return (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'F') ||
           (c >= 'a' && c <= 'f');
}

unsigned int countLegalHexPairs(char hexPairs[], unsigned int digits) {
    unsigned int count = 0;
    int i;
    for (i = 0; i < digits; ++i) {
        char current = hexPairs[i];
        if (current == '\0') {
            break;
        }
        if (isHexDigit(current)) {
            ++count;
        }
    }
    int amount = count / 2;
    if ((count & 1) == 1) {
        /** Odd number of digits: */
        return amount + 1;
    } else {
        return amount;
    }
}

char hexDigitToNybble(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return (c - 'A') + 10;
    } else if (c >= 'a' && c <= 'f') {
        return (c - 'a') + 10;
    }
    return '\0';
}

struct Datums {
    unsigned int numBytes;
    char *bytes;
};

struct Datums hexToData(char hexPairs[], unsigned int digits) {
    unsigned int numPairs = countLegalHexPairs(hexPairs, digits);
    char *data = malloc(numPairs);
    char isUpperNybble = MOST_SIGNIF_NYBBLE;
    int i, j;
    for (i = j = 0; i < numPairs; ++j) {
        char digit = hexPairs[j];
        if (digit == '\0') {
            break;
        }
        if (isHexDigit(digit)) {
            char nybble = hexDigitToNybble(digit);
            if (isUpperNybble) {
                data[i] = nybble << 4;
            } else {
                data[i] += nybble;
                ++i;
            }
            isUpperNybble = !isUpperNybble;
        }
    }
    struct Datums result;
    result.numBytes = numPairs;
    result.bytes = data;
    return result;
}

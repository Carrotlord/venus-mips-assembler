#include "strtools.h"

/** Counts characters, with newlines always
 *  being 1 char long.
 */
unsigned int countCharsNewlines(char *string) {
    int i, count;
    for (i = count = 0; string[i] != '\0'; ++i) {
        ++count;
        if (string[i] == '\r' && string[i + 1] == '\n') {
            ++i;
        }
    }
    return count;
}

void printStringAsHex(char *string) {
    int i;
    for (i = 0; string[i] != '\0'; ++i) {
        printf("%02x ", string[i]);
    }
    printf("\n");
}

/** Converts all newlines to Unix \n
 *  Uses malloc.
 */
char *unixifyNewlines(char *string) {
    unsigned int needed = countCharsNewlines(string) + 1;
    char *unixified = malloc(needed * sizeof(char));
    int i, j;
    i = j = 0;
    while (string[i] != '\0') {
        if (string[i] == '\r' && string[i + 1] == '\n') {
            unixified[j] = '\n';
            i += 2;
        } else if (string[i] == '\r' || string[i] == '\n') {
            unixified[j] = '\n';
            ++i;
        } else {
            unixified[j] = string[i];
            ++i;
        }
        ++j;
    }
    unixified[j] = '\0';
    return unixified;
}

unsigned int countLines(char *string) {
    unsigned int count, i;
    for (count = i = 0; string[i] != '\0'; ++i) {
        if (string[i] == '\n') {
            ++count;
        }
    }
    /* Each piece of text has at least
     * 1 line, even if it does not end
     * in a newline.
     */
    return count + 1;
}

/** Returns true if string begins with
 *  the substring begin.
 *  Otherwise, returns false.
 */
char startsWith(char *string, char *begin) {
    int beginLen = strlen(begin);
    int i;
    for (i = 0; i < beginLen; ++i) {
        if (string[i] != begin[i]) {
            return 0;
        }
    }
    return 1;
}

/** Ignores leading whitespace
 *  until the front of the string
 *  is found (a non-whitespace character)
 */
char *findFront(char *string) {
    int max = strlen(string);
    int i;
    for (i = 0; i < max; ++i) {
        if (string[i] != '\t' && string[i] != ' ') {
            return string + i;
        }
    }
    return string + i;
}

/** Returns true if string begins with
 *  the substring begin.
 *  Otherwise, returns false.
 *  Ignores leading whitespace for both strings.
 */
char startsWithTrimmed(char *string, char *begin) {
    if (begin[0] == '\0') {
        return 1;
    } else if (string[0] == '\0') {
        return 0;
    } else {
        return startsWith(findFront(string), findFront(begin));
    }
}

/** Returns 1 if string ends with character,
 *  else 0.
 *  Ignores trailing whitespace.
 */
char endsWith(char *string, char end) {
    int len = strlen(string);
    int i;
    for (i = len - 1; i >= 0; --i) {
        if (string[i] != ' ' && string[i] != '\t') {
            return string[i] == end;
        }
    }
    /* The string is purely whitespace: */
    return 0;
}

/** Returns a relative string pointer
 *  skipping from the start of the string until
 *  the character until.
 *  The until character is not skipped.
 */
int skipUntil(char *string, int startIndex, char until) {
    int i = startIndex;
    while (string[i] != '\0' && string[i] != until) {
        ++i;
    }
    return i - startIndex;
}

/** Erases comments that start with the character comChar.
 *  Returns a string pointer. Uses malloc.
 */
char *eraseComments(char *string, char comChar) {
    if (comChar == '\n') {
        printf("Comment character cannot be \\n.\n");
        exit(EXIT_FAILURE);
    }
    unsigned int allocSize = 0;
    char *resultString = NULL;
    int i, j;
    i = j = 0;
    while (string[i]) {
        if (string[i] == comChar) {
            i += skipUntil(string, i, '\n');
        }
        /* Copy the character: */
        resultString = realloc(resultString, ++allocSize);
        resultString[j] = string[i];
        ++i;
        ++j;
    }
    resultString = realloc(resultString, ++allocSize);
    resultString[j] = '\0';
    return resultString;
}

/** Splits a string across the \n character.
 */
char **splitLines(char *string) {
    /* Create an array of string pointers. */
    unsigned int numLines = countLines(string);
    char **strings = malloc(sizeof(char *) * numLines);
    strings[0] = string;
    int i, j;
    char *stringStart = string;
    for (i = 1, j = 0; i < numLines; ++i, ++j) {
        while (stringStart[j] != '\n' && stringStart[j] != '\0') {
            ++j;
        }
        stringStart[j] = '\0';
        strings[i] = stringStart + j + 1;
    }
    return strings;
}

char isLowercaseLetter(char c) {
    return c >= 'a' && c <= 'z';
}

char isDigit(char c) {
    return c >= '0' && c <= '9';
}

char isAllDigits(char *string) {
    int i = 0;
    while (string[i]) {
        if (!isDigit(string[i])) {
            return 0;
        }
        ++i;
    }
    return 1;
}

/** Returns true if a string is
 * completely whitespace.
 */
char isWhitespace(char *string) {
    int i;
    for (i = 0; string[i] != '\0'; ++i) {
        char c = string[i];
        if (c != ' ' && c != '\t' &&
            c != '\r' && c != '\n') {
            return 0;
        }
    }
    return 1;
}

/** Duplicates a string into heap space. */
char *strCopy(char *original) {
    /* Allow 1 extra byte for the ending 0x00. */
    unsigned int len = strlen(original);
    char *result = malloc((1 + len) * sizeof(char));
    int i;
    for (i = 0; i < len; ++i) {
        result[i] = original[i];
    }
    result[i] = '\0';
    return result;
}

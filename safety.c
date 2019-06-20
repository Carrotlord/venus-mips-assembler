#include "safety.h"

status_t checkSize() {
    if (sizeof(unsigned int) != 4) {
        printf("Unsigned int is not 4 bytes!\n");
        return FAIL;
    }
    if (sizeof(int) != 4) {
        printf("Signed int is not 4 bytes!\n");
        return FAIL;
    }
    return SUCCESS;
}

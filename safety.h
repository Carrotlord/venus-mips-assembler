#ifndef SAFETY_H
#define SAFETY_H

#include <stdio.h>
#include "encode.h"

typedef enum {SUCCESS, FAIL} status_t;

status_t checkSize();

#endif
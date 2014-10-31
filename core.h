
#ifndef _CORE_H
#define _CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#define ASSERT(cond, format, ...) if (!cond) { printf(format, __VA_ARGS__); assert(false); }
#define ASSERTN(cond) if (!cond) { assert(false); }
#define LOG(format, ...) printf(format, __VA_ARGS__); printf("\n");
#define LOGN(format) printf(format ); printf("\n");

#define countof(a) (sizeof(a) / sizeof(a[0]))

#define KB *1024
#define MB *1024*1024
#define GB *1024*1024*1024

#include "test.h"
#include "vm.h"

#endif

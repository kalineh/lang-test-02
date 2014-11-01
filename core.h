
#ifndef _CORE_H
#define _CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <stdint.h>

typedef uint8_t mem8;
typedef int32_t mem32;

inline mem32 cast32_wrap(int x) { return *((mem32*)&x); } 
inline mem32 cast32_wrap(float x) { return *((mem32*)&x); } 
inline mem32 cast32_wrap(char x) { return *((mem32*)&x); } 
inline mem32 cast32_wrap(void* x) { return *((mem32*)&x); } 

#define ASSERT(cond, format, ...) if (!cond) { printf(format, __VA_ARGS__); assert(false); }
#define ASSERTN(cond) if (!cond) { assert(false); }
#define LOG(format, ...) printf(format, __VA_ARGS__); printf("\n");
#define LOGN(format) printf(format ); printf("\n");

#define countof(a) (sizeof(a) / sizeof(a[0]))
#define cast32(a) cast32_wrap(a)
#define write32(dst, val) { \
	mem32 inner = val; \
	memcpy(dst, &inner, sizeof(mem32)); \
}
#define ofs32(p, o) (p + (o * 4))
#define inc32(p) (p += 4);
#define dec32(p) (p += 4);

#define KB *1024
#define MB *1024*1024
#define GB *1024*1024*1024

#include "test.h"
#include "vm.h"

#endif

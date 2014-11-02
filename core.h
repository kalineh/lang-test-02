
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

inline void write32(mem8* p, mem32 val) { memcpy(p, &val, sizeof(mem32)); }
inline void read32(mem8* p, mem32* val) { memcpy(val, p, sizeof(mem32)); }

inline mem8* ofs32(mem8* p, int ofs) { return p + ofs * 4; }
inline mem8* inc32(mem8* p) { return ofs32(p, +1); }
inline mem8* dec32(mem8* p) { return ofs32(p, -1); }

#define ISZ 4

#define KB *1024
#define MB *1024*1024
#define GB *1024*1024*1024

#include "test.h"
#include "vm.h"

#endif

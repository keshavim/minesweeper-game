#ifndef STDAFX_H
#define STDAFX_H

// includes a bunch of headers and has some other support typedefs and macros

#include <cglm/struct.h>
#include <glad/glad.h>
#include <stb/stb_image.h>

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

#define DEBUG_LOG(...) fprintf(stderr, __VA_ARGS__)
#define random(min, max) rand() / ((RAND_MAX + 1u) / max) + min

#endif // STDAFX_H

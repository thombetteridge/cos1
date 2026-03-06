#pragma once

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float  f32;
typedef double f64;

#ifdef __cplusplus
#define C_LINKAGE_BEGIN extern "C" {
#define C_LINKAGE_END }
#else
#define C_LINKAGE_BEGIN
#define C_LINKAGE_END
#endif

#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#include <stdint.h>

typedef int32_t q16_16;

#define SHIFT_VAL 16
#define Q_SIGN ((int32_t)1 << SHIFT_VAL)   // force 32-bit shift

static inline q16_16 itoq(int32_t x)
{
    return (x << SHIFT_VAL);
}

static inline int32_t qtoi(q16_16 x)
{
    return (x >> SHIFT_VAL);
}

static inline q16_16 ftoq(float x)
{
    return (q16_16)(x * Q_SIGN);
}

static inline float qtof(q16_16 x)
{
    return (float)x / Q_SIGN;
}

static inline q16_16 q_add(q16_16 x, q16_16 y) { return x + y; }
static inline q16_16 q_sub(q16_16 x, q16_16 y) { return x - y; }

static inline q16_16 q_mul(q16_16 x, q16_16 y)
{
    return (q16_16)(((int64_t)x * y) >> SHIFT_VAL);
}

static inline q16_16 q_div(q16_16 x, q16_16 y)
{
    return (q16_16)(((int64_t)x << SHIFT_VAL) / y);
}

#endif
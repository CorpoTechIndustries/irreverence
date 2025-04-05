#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define MATH_MAX(a, b) a > b ? a : b
#define MATH_MIN(a, b) a < b ? a : b

typedef float vec_t;
typedef int ivec_t;

float Math_Rad(float degrees);
float Math_Deg(float radians);

#ifdef __cplusplus
}
#endif

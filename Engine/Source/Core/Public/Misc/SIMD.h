//
// Created by Egor Orachyov on 06.02.2019.
//

#ifndef BERSERK_SIMD_H
#define BERSERK_SIMD_H

#include "nmmintrin.h"
#include "immintrin.h"

/** float32 4 component vector */
#define SIMD4_FLOAT32                      __m128

/** int32 4 component vector */
#define SIMD4_INT32                        __m128i

/** @return float32 zero 4 component vector */
#define SIMD4_FLOAT32_ZERO                 _mm_setzero_ps ()

/** @return float32 4 component vector with data from ptr pointer */
#define SIMD4_FLOAT32_LOAD(ptr)            _mm_load_ps (ptr)

/** @return float32 4 component vector initialized by values */
#define SIMD4_FLOAT32_SET(x,y,z,w)         _mm_set_ps (w,z,y,x)

/** convert to int32 4 component vector */
#define SIMD4_FLOAT32_TO_INT32(a)          _mm_cvtps_epi32 (a)

/** copy source values (4 x float32) in target memory section */
#define SIMD4_FLOAT32_COPY(target,source)  _mm_store_ps (target,source)

/** copy source values (4 x int32) in target memory section */
#define SIMD4_INT32_COPY(target,source)    _mm_store_si128 (target,source)

/** @return a + b per value */
#define SIMD4_FLOAT32_ADD(a, b)            _mm_add_ps (a,b)

/** @return a - b per value */
#define SIMD4_FLOAT32_SUB(a, b)            _mm_sub_ps (a,b)

/** @return a * b per value */
#define SIMD4_FLOAT32_MUL(a, b)            _mm_mul_ps (a,b)

/** @return a / b per value */
#define SIMD4_FLOAT32_DIV(a, b)            _mm_div_ps (a,b)

/** @return a == b per value */
#define SIMD4_FLOAT32_EQ(a,b)              _mm_cmpeq_ps (a,b)

/** @return a > b per value */
#define SIMD4_FLOAT32_GR(a,b)              _mm_cmpgt_ps (a,b)

/** @return a >= b per value */
#define SIMD4_FLOAT32_GR_OR_EQ(a,b)        _mm_cmpge_ps (a,b)

/** @return a < b per value */
#define SIMD4_FLOAT32_LS(a,b)              _mm_cmplt_ps (a,b)

/** @return a <= b per value */
#define SIMD4_FLOAT32_LS_OR_EQ(a,b)        _mm_cmple_ps (a,b)

/** @return Per bit | operation */
#define SIMD4_FLOAT32_OR(a,b)              _mm_or_ps (a,b)

/** @return Per bit & operation */
#define SIMD4_FLOAT32_AND(a,b)             _mm_and_ps (a,b)


#endif //BERSERK_SIMD_H

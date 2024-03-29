////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  AlignedVector.h                                               //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef ALIGNED_VECTOR_H
#define ALIGNED_VECTOR_H

#include <vector>
#include <ostream>

#include "../concepts/concepts.h"

#if defined(_MSC_VER)
    #include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    #include <x86intrin.h>
#endif

#if defined(_MSC_VER)
    #define ALIGNED_(x) __declspec(align(x))
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    #define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif

#define ALIGNED_TYPE_(t, x) typedef ALIGNED_(x) t

#include "allocator/AlignedMemory.h"

using Memory::Alignment;

#ifdef __AVX__
    #ifdef DEBUG
        #pragma message "Selecting AVX 32 byte alignment"
    #endif

    static constexpr std::size_t __alignment = to_integral(Alignment::AVX);

#elif __SSE__
    #ifdef DEBUG
        #pragma message "Selecting SSE 16 byte alignment"
    #endif

    static constexpr std::size_t __alignment = to_integral(Alignment::SSE);

#else
    #ifdef DEBUG
        #pragma message "Selecting no forced alignment"
    #endif

    static constexpr std::size_t __alignment = to_integral(Alignment::Normal);

#endif // end alignment switch

// Create aligned types
ALIGNED_TYPE_(double,           __alignment) align_double;
ALIGNED_TYPE_(float,            __alignment) align_float;
ALIGNED_TYPE_(int,              __alignment) align_int;
ALIGNED_TYPE_(unsigned int,     __alignment) align_uint;
ALIGNED_TYPE_(long,             __alignment) align_long;
ALIGNED_TYPE_(unsigned long,    __alignment) align_ulong;

ALIGNED_TYPE_(double,           __alignment) aligned_double;
ALIGNED_TYPE_(float,            __alignment) aligned_float;
ALIGNED_TYPE_(int,              __alignment) aligned_int;
ALIGNED_TYPE_(unsigned int,     __alignment) aligned_uint;
ALIGNED_TYPE_(long,             __alignment) aligned_long;
ALIGNED_TYPE_(unsigned long,    __alignment) aligned_ulong;

#endif

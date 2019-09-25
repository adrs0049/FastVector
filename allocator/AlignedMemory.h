////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  AlignedMemory.h                                               //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2016-04-20                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef ALIGNED_MEMORY_H
#define ALIGNED_MEMORY_H

#include <stdlib.h>
#include <malloc.h>
#include <memory>
#include <type_traits>

//
// From 6.32.1 from the gcc manual
//
// This tells the compiler that a function is malloc-like, i.e., that the pointer
// P returned by the function cannot alias any other pointer valid when the
// function returns, and moreover no pointers to valid objects occur in any
// storage addressed by P.
//
// Using this attribute can improve optimization. Compiler predicts that a
// function with the attribute returns non-null in most cases. Functions like
// malloc and calloc have this property because they return a pointer to
// uninitialized or zeroed-out storage. However, functions like realloc do not
// have this property, as they can return a pointer to storage containing pointers.
//
#define MALLOC __attribute__((malloc))

//
// From 6.32.1 from the gcc manual
//
//  The alloc_size attribute may be applied to a function that returns a pointer
//  and takes at least one argument of an integer or enumerated type. It
//  indicates that the returned pointer points to memory whose size is given by
//  the function argument at position-1, or by the product of the arguments at
//  position-1 and position-2. Meaningful sizes are positive values less than
//  PTRDIFF_MAX. GCC uses this information to improve the results of
//  __builtin_object_size.
//
// The function parameter(s) denoting the allocated size are specified by one
// or two integer arguments supplied to the attribute. The allocated size is
// either the value of the single function argument specified or the product of
// the two function arguments specified. Argument numbering starts at one for
// ordinary functions, and at two for C++ non-static member functions.
//
#if __has_attribute(alloc_size)
    #define ALLOC_SIZE(args...) __attribute__((alloc_size(args)))
#else
    #define ALLOC_SIZE(args...) /* ignored */
#endif

#if defined(_MSC_VER)
    #define RESTRICT restrict__
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
    #define RESTRICT __restrict__
#endif

//
// From 6.32.1 from the gcc manual
//
// The alloc_align attribute may be applied to a function that returns a pointer
// and takes at least one argument of an integer or enumerated type. It indicates
// that the returned pointer is aligned on a boundary given by the function
// argument at position. Meaningful alignments are powers of 2 greater than one.
// GCC uses this information to improve pointer alignment analysis.
//
// The function parameter denoting the allocated alignment is specified by one
// constant integer argument whose number is the argument of the attribute.
// Argument numbering starts at one.
//
#if __has_attribute(alloc_align) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9)))
    #define ALLOC_ALIGN(arg) __attribute__((alloc_align(arg)))
#else
    #define ALLOC_ALIGN(arg) /* ignored */
#endif

//
// From 6.32.1 from the gcc manual
//
// The assume_aligned attribute may be applied to a function that returns a
// pointer. It indicates that the returned pointer is aligned on a boundary
// given by alignment. If the attribute has two arguments, the second argument
// is misalignment offset. Meaningful values of alignment are powers of 2
// greater than one. Meaningful values of offset are greater than zero and less
// than alignment.
//
#if (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7))) || defined(__clang__)
    #define ASSUME_ALIGNED(ptr, alignment) __builtin_assume_aligned(ptr, alignment)
// TODO check for icc
//#elif __has_attribute(__assume_aligned)
//    #define ASSUME_ALIGNED(ptr, alignment) __assume_aligned(ptr, alignment)
#else
    #define ASSUME_ALIGNED(ptr, alignment) /* ignored */
#endif

using std::size_t;

namespace Memory {

enum class Alignment : short
{
    Normal  = sizeof(void*),
    SSE     = 16,
    AVX     = 32,
};

struct aligned_memory_header
{
    size_t offset;
    size_t allocated_size;
};

static bool is_aligned(const void * RESTRICT ptr, size_t alignment);
static size_t align_on(size_t value, size_t alignment) noexcept;

void * aligned_alloc(size_t alignment, size_t size) ALLOC_ALIGN(1) ALLOC_SIZE(2) MALLOC;
void * aligned_realloc(void * ptr, size_t alignment, size_t size) ALLOC_ALIGN(2) ALLOC_SIZE(3);
void aligned_free(void * ptr) noexcept;

template<typename T>
constexpr bool is_power_of_two(T x)
{
    return x && ((x & (x-1)) == 0);
}

} // end namespace Memory

#endif

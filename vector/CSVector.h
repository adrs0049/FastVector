////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  File Name:  CSVector.h                                                    //
//                                                                            //
//     Author:  Andreas Buttenschoen <andreas@buttenschoen.ca>                //
//    Created:  2018-03-30 12:22:20                                           //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef CS_VECTOR_CONTAINER_H
#define CS_VECTOR_CONTAINER_H

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <limits>

#include "AlignedMemory.h"
#include "AlignedVector.h"
#include "macros.h"
#include "concepts.h"

#include "CSVectorTraits.h"
#include "CSOperations.h"
#include "CSExpression.h"

#define PREFETCH_LENGTH 2

using namespace Memory;
using namespace Expression;

using std::max;

// TODO: write a bool version of this!
template <class T>
class CSVector : public VectorExpression<CSVector<T>>
{
    static_assert(Arithmetic<T>(), "CSVector must have an arithmetic type as base!");

public:
    // ValueType is aligned
    typedef T ValueType __attribute__ ((__aligned__(__alignment)));
    using value_type = T;
    typedef T UnderlyingType;

    using size_type = std::size_t;

    static constexpr size_type VectorSize = __alignment / sizeof(T);
    typedef T ScalarPacket __attribute__((vector_size (sizeof(T) * VectorSize)));

    // Creates an empty vector: Does not initialize values!
    CSVector(const size_t size)
        : mAllocationSize(size),
          mDataSize(size),
          mpStart(nullptr),
          mpEnd(nullptr)
    {
        mpStart = (ValueType *)Memory::aligned_alloc(__alignment, mAllocationSize*sizeof(ValueType));
        mpEnd   = mpStart + mDataSize;
    }

    CSVector(const size_t size, const T value)
        : mAllocationSize(size),
          mDataSize(size),
          mpStart(nullptr),
          mpEnd(nullptr)
    {
        mpStart = (ValueType *)Memory::aligned_alloc(__alignment, mAllocationSize*sizeof(ValueType));
        mpEnd   = mpStart + mDataSize;

        for (size_t i = 0; i < mDataSize; i++)
            mpStart[i] = value;
    }

    template <typename U, typename = Enable_if<Convertible<T, U>()> >
    CSVector(std::initializer_list<U> ilist)
        : CSVector(ilist.size())
    {
        std::copy(ilist.begin(), ilist.end(), begin());
    }

    CSVector(const CSVector& other)
        : mAllocationSize(other.mAllocationSize),
        mDataSize(other.mDataSize),
        mpStart(nullptr),
        mpEnd(nullptr)
    {
        mpStart = (ValueType *)Memory::aligned_alloc(__alignment, mAllocationSize*sizeof(ValueType));

        // fill in stuff
        if (mpStart)
            memcpy(mpStart, other.mpStart, mAllocationSize * sizeof(ValueType));

        mpEnd = mpStart + mDataSize;
    }

    CSVector(CSVector&& other)
        : mAllocationSize(other.mAllocationSize),
        mDataSize(other.mDataSize),
        mpStart(other.mpStart),
        mpEnd(other.mpEnd)
    {
        other.mpStart           = nullptr;
        other.mpEnd             = nullptr;
        other.mAllocationSize   = 0;
        other.mDataSize         = 0;
    }

    // Assignment operator for the use with template expressions
    template <class Other, typename = Disable_if<Same<Other, CSVector<T> >()> >
    CSVector(const Other& that)
        : mAllocationSize(Expression::size(that)),
        mDataSize(Expression::size(that)),
        mpStart(nullptr),
        mpEnd(nullptr)
    {
        mpStart = (ValueType *)Memory::aligned_alloc(__alignment, mAllocationSize*sizeof(ValueType));
        mpEnd   = mpStart + mDataSize;
        VectorAssignment<CSVector<T>, Other>()(static_cast<CSVector<T>&>(*this), that);
    }

    CSVector& operator=(const CSVector& other)
    {
        mpStart = (ValueType *)Memory::aligned_alloc(__alignment, other.mAllocationSize*sizeof(ValueType));

        // fill in stuff
        if (mpStart)
            memcpy(mpStart, other.mpStart, mAllocationSize * sizeof(ValueType));

        mpEnd = mpStart + mDataSize;

        return *this;
    }

    CSVector& operator=(CSVector&& other)
    {
        if (mpStart)
            std::cout << "Move operator with free!" << std::endl;

        // delete previous amount of stuff
        aligned_free(mpStart);

        // now copy everything across
        mAllocationSize = other.mAllocationSize;
        mDataSize       = other.mDataSize;
        mpStart         = other.mpStart;
        mpEnd           = other.mpEnd;

        other.mpStart   = nullptr;
        other.mpEnd     = nullptr;

        return *this;
    }

    // Assignment operator for the use with template expressions
    template <class Other>
    Disable_if<Same<Other, CSVector<T> >(), typename VectorAssignment<CSVector<T>, Other>::type>
    operator=(const Other& that)
    {
        return VectorAssignment<CSVector<T>, Other>()(static_cast<CSVector<T>&>(*this), that);
    }

    ~CSVector()
    {
        aligned_free( mpStart );
    }

    void swap( CSVector<ValueType> & other );

    const UnderlyingType & operator() (size_t index) const
    {
        return mpStart[index];
    }

    UnderlyingType & operator()(size_t index)
    {
        return mpStart[index];
    }

    const UnderlyingType & operator[] (size_t index) const
    {
        return mpStart[index];
    }

    UnderlyingType & operator[](size_t index)
    {
        return mpStart[index];
    }

    const UnderlyingType & at(size_t index) const
    {
        if (index >= mDataSize)
            throw std::out_of_range("Index " + std::to_string(index) + " larger than " + std::to_string(mDataSize));
        return operator[](index);
    }

    UnderlyingType & at(size_t index)
    {
        if (index >= mDataSize)
            throw std::out_of_range("Index " + std::to_string(index) + " larger than " + std::to_string(mDataSize));

        return operator[](index);
    }

    UnderlyingType * begin() { return mpStart; }
    const UnderlyingType * begin() const { return mpStart; }
    const UnderlyingType * cbegin() const { return mpStart; }

    UnderlyingType * end() { return mpEnd; }
    const UnderlyingType * end() const { return mpEnd; }
    const UnderlyingType * cend() const { return mpEnd; }

    UnderlyingType& front() { return operator[](0); }
    const UnderlyingType& front() const { return operator[](0); }

    UnderlyingType& back() { return operator[](size() - 1); }
    const UnderlyingType& back() const { return operator[](size() - 1); }

    ValueType * data() { return mpStart; }
    const ValueType * data() const { return mpStart; }

    size_t size() const { return mDataSize; }
    size_t memory_size() const { return mAllocationSize; }
    size_t alignment() const { return __alignment; }
    size_t capacity() const { return mAllocationSize; }

    void clear();
    void resize( size_t newSize, size_t allocationChunk = 0 );
    void resizeAndFill( size_t newSize, T value = T(0), size_t allocationChunk = 0 );

    void setZero();

private:

    void reallocate(size_t new_size, size_t allocationChunk=0);

    bool requires_reallocation(size_t elements);

    size_t mAllocationSize;
    size_t mDataSize;

    ValueType * mpStart; // pointer to the (1st element of the) data array;
    ValueType * mpEnd;   // pointer to the element after the last of the data array;
};

template <class T>
inline typename CSVector<T>::size_type
size(const CSVector<T>& vector)
{
    return vector.size();
}

//template <>
//class CSVector<bool>
//{
//public:
//    static constexpr int N = 1;
//
//
//    uint8_t * data() { return mData; }
//    std::size_t count() const;
//
//    // Operations
//    explicit operator bool() const { return all(); }
//    bool all() const;
//    bool any() const { return !none(); };
//    bool none() const;
//
//    CSVector operator~() const
//    {
//        CSVector ret;
//        for (std::size_t i = 0; i != mDataSize; ++i)
//            ret.mData[i] = ~mData[i];
//        return ret;
//    }
//
//    CSVector operator&=(const CSVector& other)
//    {
//        for (std::size_t i = 0; i != mDataSize; ++i)
//            mData[i] &= other.mData[i];
//        return *this;
//    }
//
//    CSVector operator|=(const CSVector& other)
//    {
//        for (std::size_t i = 0; i != mDataSize; ++i)
//            mData[i] |= other.mData[i];
//        return *this;
//    }
//
//    CSVector operator^=(const CSVector& other)
//    {
//        for (std::size_t i = 0; i != mDataSize; ++i)
//            mData[i] ^= other.mData[i];
//        return *this;
//    }
//
//private:
//    //enum : std::size_t
//    //{
//    //    FullSegmentMask = 0xFF,
//    //    LastSegmentMask = (1 << N % 8) - 1
//    //};
//
//    uint8_t * mData;
//
//    size_t mAllocationSize;
//    size_t mDataSize;
//};

template <class T>
inline bool CSVector<T>::requires_reallocation(size_t elements)
{
    return elements > capacity();
}

template <class T>
inline void
CSVector<T>::setZero()
{
    memset(mpStart, 0, mDataSize * sizeof(T));
}


template <class T>
inline
void
CSVector<T>::swap( CSVector<T> & other )
{
    using std::swap;
    swap(mpStart,           other.mpStart);
    swap(mpEnd,             other.mpEnd);
    swap(mDataSize,         other.mDataSize);
    swap(mAllocationSize,   other.mAllocationSize);
}


template <class T>
inline
void
CSVector<T>::resize(size_t newSize, size_t allocationChunk)
{
    // TODO: shrink!
    if (newSize >= mAllocationSize)
        reallocate(newSize, allocationChunk);

    assert(mAllocationSize >= newSize);
    mDataSize = newSize;
    mpEnd = mpStart + mDataSize;
}


template <class T>
inline
void
CSVector<T>::resizeAndFill( size_t newSize, T value, size_t allocationChunk )
{
    // TODO: shrink!
    if (newSize >= mAllocationSize)
        reallocate(newSize, allocationChunk);

    // If the vector was enlarged set new values to that value!
    if (newSize > mDataSize)
    {
        for (size_t i = mDataSize; i < newSize; i++)
            mpStart[i] = value;
    }

    mDataSize = newSize;
    mpEnd = mpStart + mDataSize;
}


template <class T>
inline
void
CSVector<T>::reallocate(size_t new_size, size_t allocationChunk)
{
    size_t factor = 0;
    size_t size_increase = 0;

    // allocationChunk may be zero
    allocationChunk = max(1ul, allocationChunk);

    if (new_size >= mAllocationSize)
    {
        size_t difference = new_size - mAllocationSize;
        factor = max(1ul, (difference + allocationChunk - 1) / allocationChunk);
        size_increase = factor * allocationChunk;
    }

    size_t new_allocation_size = mAllocationSize + size_increase;
    assert(new_allocation_size >= new_size);

    mpStart = (T *)aligned_realloc(mpStart, __alignment, new_allocation_size*sizeof(T));
    mpEnd   = mpStart + mDataSize;

    // make sure new memory is at least zeroed
    memset(mpStart + mAllocationSize, 0, size_increase * sizeof(T));

    // update allocation size!
    mAllocationSize = new_allocation_size;
};


template <class T>
inline void swap(CSVector<T>& lhs, CSVector<T>& rhs)
{
    lhs.swap(rhs);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarAssignmentOpExpression<E1, E2, plus_assign<typename E1::value_type, E2> >
operator+= (VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarAssignmentOpExpression<E1, E2, plus_assign<typename E1::value_type, E2> >;
    return rtype(static_cast<E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarAssignmentOpExpression<E1, E2, minus_assign<typename E1::value_type, E2> >
operator-= (VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarAssignmentOpExpression<E1, E2, minus_assign<typename E1::value_type, E2> >;
    return rtype(static_cast<E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarAssignmentOpExpression<E1, E2, product_assign<typename E1::value_type, E2> >
operator*= (VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarAssignmentOpExpression<E1, E2, product_assign<typename E1::value_type, E2> >;
    return rtype(static_cast<E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarAssignmentOpExpression<E1, E2, divide_assign<typename E1::value_type, E2> >
operator/= (VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarAssignmentOpExpression<E1, E2, divide_assign<typename E1::value_type, E2> >;
    return rtype(static_cast<E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarBinaryExpression<E1, E2, plus_test<typename E1::value_type, E2> >
operator+ (const VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarBinaryExpression<E1, E2, plus_test<typename E1::value_type, E2> >;
    return rtype(static_cast<const E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E1>()> >
inline VectorScalarBinaryExpression<E2, E1, plus_test<typename E2::value_type, E1> >
operator+ (const E1& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorScalarBinaryExpression<E2, E1, plus_test<typename E2::value_type, E1> >;
    return rtype(static_cast<const E2&>(e2), e1);
}


template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()> >
inline VectorScalarBinaryExpression<E1, E2, minus_test<typename E1::value_type, E2> >
operator- (const VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarBinaryExpression<E1, E2, minus_test<typename E1::value_type, E2> >;
    return rtype(static_cast<const E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E1>()> >
inline VectorScalarBinaryExpression<E2, E1, inverse_minus<typename E2::value_type, E1> >
operator- (const E1& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorScalarBinaryExpression<E2, E1, inverse_minus<typename E2::value_type, E1> >;
    return rtype(static_cast<const E2&>(e2), e1);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E1>()> >
inline VectorScalarBinaryExpression<E2, E1, product<typename E2::value_type, E1> >
operator* (const E1& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorScalarBinaryExpression<E2, E1, product<typename E2::value_type, E1> >;
    return rtype(static_cast<const E2&>(e2), e1);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()>>
inline VectorScalarBinaryExpression<E1, E2, product<typename E1::value_type, E2> >
operator* (const VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarBinaryExpression<E1, E2, product<typename E1::value_type, E2> >;
    return rtype(static_cast<const E1&>(e1), e2);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E1>()> >
inline VectorScalarBinaryExpression<E2, E1, inverse_divide<typename E2::value_type, E1> >
operator/ (const E1& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorScalarBinaryExpression<E2, E1, inverse_divide<typename E2::value_type, E1> >;
    return rtype(static_cast<const E2&>(e2), e1);
}

template <typename E1, typename E2,
          typename = Enable_if<Scalar<E2>()>>
inline VectorScalarBinaryExpression<E1, E2, divide<typename E1::value_type, E2> >
operator/ (const VectorExpression<E1>& e1, const E2& e2)
{
    using rtype = VectorScalarBinaryExpression<E1, E2, divide<typename E1::value_type, E2> >;
    return rtype(static_cast<const E1&>(e1), e2);
}

template <typename E1, typename E2>
inline VectorVectorBinaryExpression<E1, E2, product<typename E1::value_type, typename E2::value_type> >
operator* (const VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorBinaryExpression<E1, E2, product<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorBinaryExpression<E1, E2, divide<typename E1::value_type, typename E2::value_type> >
operator/ (const VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorBinaryExpression<E1, E2, divide<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorBinaryExpression<E1, E2, plus_test<typename E1::value_type, typename E2::value_type> >
operator+ (const VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorBinaryExpression<E1, E2, plus_test<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorBinaryExpression<E1, E2, minus_test<typename E1::value_type, typename E2::value_type> >
operator- (const VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorBinaryExpression<E1, E2, minus_test<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

// This is only enabled when T is of floating point so we don't interfer with the above!
template <class T, typename = Enable_if<All(Integral<T>(), Floating_Point<T>())> >
auto operator+(CSVector<T>& lhs, const size_t index)
{
    return lhs.begin() + index;
}

// This is only enabled when T is of floating point so we don't interfer with the above!
template <class T, typename = Enable_if<All(Integral<T>(), Floating_Point<T>())> >
const auto operator+(const CSVector<T>& lhs, const size_t index)
{
    return lhs.begin() + index;
}

template <typename E1>
inline VectorUnaryExpression<E1, Abs<typename E1::value_type> >
abs(const VectorExpression<E1>& e1)
{
    using rtype = VectorUnaryExpression<E1, Abs<typename E1::value_type> >;
    return rtype(static_cast<const E1&>(e1));
}

template <typename E1, typename E2>
inline VectorVectorAssignmentOpExpression<E1, E2, plus_assign<typename E1::value_type, typename E2::value_type> >
operator+= (VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorAssignmentOpExpression<E1, E2, plus_assign<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorAssignmentOpExpression<E1, E2, minus_assign<typename E1::value_type, typename E2::value_type> >
operator-= (VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorAssignmentOpExpression<E1, E2, minus_assign<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorAssignmentOpExpression<E1, E2, product_assign<typename E1::value_type, typename E2::value_type> >
operator*= (VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorAssignmentOpExpression<E1, E2, product_assign<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<E1&>(e1), static_cast<const E2&>(e2));
}

template <typename E1, typename E2>
inline VectorVectorAssignmentOpExpression<E1, E2, divide_assign<typename E1::value_type, typename E2::value_type> >
operator/= (VectorExpression<E1>& e1, const VectorExpression<E2>& e2)
{
    using rtype = VectorVectorAssignmentOpExpression<E1, E2, divide_assign<typename E1::value_type, typename E2::value_type> >;
    return rtype(static_cast<E1&>(e1), static_cast<const E2&>(e2));
}

template <class T>
T triple(const CSVector<T> &a, const CSVector<T>& b, const CSVector<T>& c)
{
    size_t N = a.size();
    if (N != b.size() || N != c.size())
        throw std::runtime_error("Incompatible vector lengths " + std::to_string(N)
                                 + " " + std::to_string(b.size()) +
                                 " " + std::to_string(c.size()) + "!");

    T triple = {0};
    for (size_t i = 0; i < N; i++)
        triple += a(i) * b(i) * c(i);
    return triple;
}

//
// The functions below use gcc's __builtin_prefetch: From 6.58 gcc manual:
//
// This function is used to minimize cache-miss latency by moving data into a
// cache before it is accessed. You can insert calls to __builtin_prefetch into
// code for which you know addresses of data in memory that is likely to be
// accessed soon. If the target supports them, data prefetch instructions are
// generated. If the prefetch is done early enough before the access then the
// data will be in the cache by the time it is accessed.

// The value of addr is the address of the memory to prefetch. There are two
// optional arguments, rw and locality. The value of rw is a compile-time
// constant one or zero; one means that the prefetch is preparing for a write to
// the memory address and zero, the default, means that the prefetch is
// preparing for a read. The value locality must be a compile-time constant
// integer between zero and three. A value of zero means that the data has no
// temporal locality, so it need not be left in the cache after the access. A
// value of three means that the data has a high degree of temporal locality and
// should be left in all levels of cache possible. Values of one and two mean,
// respectively, a low or moderate degree of temporal locality. The default is
// three.

//
// The other important piece for the code below is gcc's vector instructions.
// See section 6.51 in the manual.
//
template <class T>
T dot(const CSVector<T>& lhs, const CSVector<T>& rhs)
{
    size_t N = lhs.size();
    if (N != rhs.size())
        throw std::runtime_error("Incompatible vector lengths " + std::to_string(N)
                                 + " " + std::to_string(rhs.size()) + "(dot) !");

    // Optimize the computation of the dot product. This is ~66% faster than the
    // naive implementation
    constexpr size_t VECTOR_SIZE = __alignment / sizeof(T);

    size_t REMAINDER_LOOP_START = 0;
    constexpr size_t CHUNK_SIZE = PREFETCH_LENGTH * VECTOR_SIZE;

    // init
    T dot = {0};

    if (likely(N >= CHUNK_SIZE))
    {
        using ValueType = typename CSVector<T>::ValueType;
        typedef ValueType vec __attribute__((vector_size (sizeof(ValueType) * VECTOR_SIZE)));

        vec temp1 = {0}, temp2 = {0};

        const ValueType * __restrict__ a = lhs.data();
        const ValueType * __restrict__ b = rhs.data();

        vec * Av = (vec *) a;
        vec * Bv = (vec *) b;

        size_t NO_LOOPS = N / CHUNK_SIZE;
        for (size_t i = 0; i < NO_LOOPS; ++i)
        {
            __builtin_prefetch(Av + 2, 0, 0);
            temp1 += *Av * *Bv;
            Av++;
            Bv++;

            temp2 += *Av * *Bv;
            Av++;
            Bv++;
        }

        union {
            vec tempv;
            T tempd[VECTOR_SIZE];
        };

        tempv = temp1;

        for (size_t i = 0; i < VECTOR_SIZE; i++)
            dot += tempd[i];

        tempv = temp2;
        for (size_t i = 0; i < VECTOR_SIZE; i++)
            dot += tempd[i];

        // set remainder to consider last few elements
        REMAINDER_LOOP_START = NO_LOOPS * CHUNK_SIZE;
    }

    // deal with left overs
    for (size_t i = REMAINDER_LOOP_START; i < N; i++)
        dot = std::fma(lhs(i), rhs(i), dot);

    return dot;
}

template <class T>
T norm(const CSVector<T>& lhs)
{
    // TODO check if this violates the later use of __restrict__ But it
    // shouldn't as i only ever read from these values
    T norm2 = dot(lhs, lhs);
    return sqrt(norm2);
}

template <class T>
T max(const CSVector<T>& lhs)
{
    size_t N = lhs.size();
    if (N == 0)
        throw std::runtime_error("Zero length vector!");

    // result
    double res;

    // get vector size from alignment
    constexpr size_t VECTOR_SIZE = __alignment / sizeof(T);

    size_t REMAINDER_LOOP_START = 1;
    constexpr size_t CHUNK_SIZE = PREFETCH_LENGTH * VECTOR_SIZE;

    if (likely(N >= CHUNK_SIZE))
    {
        using ValueType = typename CSVector<T>::ValueType;
        typedef ValueType vec __attribute__((vector_size (sizeof(ValueType) * VECTOR_SIZE)));

        vec temp1, temp2;

        // need to initialize both vectors
        for (size_t i = 0; i < VECTOR_SIZE; ++i)
        {
            temp1[i] = std::numeric_limits<T>::lowest();
            temp2[i] = std::numeric_limits<T>::lowest();
        }

        const ValueType * __restrict__ a = lhs.data();
        vec * Av = (vec *) a;

        size_t NO_LOOPS = N / CHUNK_SIZE;
        for (size_t i = 0; i < NO_LOOPS; ++i)
        {
            __builtin_prefetch(Av + 2, 0, 0);
            temp1 = (*Av > temp1) ? *Av : temp1;
            Av++;

            temp2 = (*Av > temp2) ? *Av : temp2;
            Av++;
        }

        // combine the two chunks
        temp1 = (temp1 > temp2) ? temp1 : temp2;

        union {
            vec tempv;
            T tempd[VECTOR_SIZE];
        };

        tempv = temp1;

        // extract max
        res = tempd[0];
        for (size_t i = 1; i < VECTOR_SIZE; i++)
            res = ((tempd[i] > res) ? tempd[i] : res);

        // set remainder to consider last few elements
        REMAINDER_LOOP_START = NO_LOOPS * CHUNK_SIZE;
    }
    else
    {
        res = lhs(0);
    }

    // deal with left overs
    for (size_t i = REMAINDER_LOOP_START; i < N; i++)
        res = ((lhs(i) > res) ? lhs(i) : res);

    return res;
}

template <class T>
T min(const CSVector<T>& lhs)
{
    size_t N = lhs.size();
    if (N == 0)
        throw std::runtime_error("Zero length vector!");

    // result
    double res;

    // get vector size from alignment
    constexpr size_t VECTOR_SIZE = __alignment / sizeof(T);

    size_t REMAINDER_LOOP_START = 1;
    constexpr size_t CHUNK_SIZE = PREFETCH_LENGTH * VECTOR_SIZE;

    if (likely(N >= CHUNK_SIZE))
    {
        using ValueType = typename CSVector<T>::ValueType;
        typedef ValueType vec __attribute__((vector_size (sizeof(ValueType) * VECTOR_SIZE)));

        vec temp1, temp2;

        // need to initialize both vectors
        for (size_t i = 0; i < VECTOR_SIZE; ++i)
        {
            temp1[i] = std::numeric_limits<T>::max();
            temp2[i] = std::numeric_limits<T>::max();
        }

        const ValueType * __restrict__ a = lhs.data();
        vec * Av = (vec *) a;

        size_t NO_LOOPS = N / CHUNK_SIZE;
        for (size_t i = 0; i < NO_LOOPS; ++i)
        {
            __builtin_prefetch(Av + 2, 0, 0);
            temp1 = (*Av < temp1) ? *Av : temp1;
            Av++;

            temp2 = (*Av < temp2) ? *Av : temp2;
            Av++;
        }

        // combine the two chunks
        temp1 = (temp1 < temp2) ? temp1 : temp2;

        union {
            vec tempv;
            T tempd[VECTOR_SIZE];
        };

        tempv = temp1;

        // extract max
        res = tempd[0];
        for (size_t i = 1; i < VECTOR_SIZE; i++)
            res = ((tempd[i] < res) ? tempd[i] : res);

        // set remainder to consider last few elements
        REMAINDER_LOOP_START = NO_LOOPS * CHUNK_SIZE;
    }
    else
    {
        res = lhs(0);
    }

    // deal with left overs
    for (size_t i = REMAINDER_LOOP_START; i < N; i++)
        res = ((lhs(i) < res) ? lhs(i) : res);

    return res;
}

template <class T>
T supNorm(const CSVector<T>& lhs)
{
    size_t N = lhs.size();
    if (N == 0)
        throw std::runtime_error("Zero length vector!");

    // result
    double res;

    // get vector size from alignment
    constexpr size_t VECTOR_SIZE = __alignment / sizeof(T);

    size_t REMAINDER_LOOP_START = 1;
    constexpr size_t CHUNK_SIZE = PREFETCH_LENGTH * VECTOR_SIZE;

    if (likely(N >= CHUNK_SIZE))
    {
        using ValueType = typename CSVector<T>::ValueType;
        typedef ValueType vec __attribute__((vector_size (sizeof(ValueType) * VECTOR_SIZE)));

        vec temp1 = {0}, temp2 = {0};

        const ValueType * __restrict__ a = lhs.data();
        vec * Av = (vec *) a;

        size_t NO_LOOPS = N / CHUNK_SIZE;
        vec tabs;
        for (size_t i = 0; i < NO_LOOPS; ++i)
        {
            __builtin_prefetch(Av + 2, 0, 0);
            tabs = (*Av > 0) ? *Av : (- (*Av));
            temp1 = (tabs > temp1) ? tabs : temp1;
            Av++;

            tabs = (*Av > 0) ? *Av : (- (*Av));
            temp2 = (tabs > temp2) ? tabs : temp2;
            Av++;
        }

        // combine the two chunks
        temp1 = (temp1 > temp2) ? temp1 : temp2;

        union {
            vec tempv;
            T tempd[VECTOR_SIZE];
        };

        tempv = temp1;

        // extract max
        res = tempd[0];
        for (size_t i = 1; i < VECTOR_SIZE; i++)
            res = ((std::abs(tempd[i]) > res) ? std::abs(tempd[i]) : res);

        // set remainder to consider last few elements
        REMAINDER_LOOP_START = NO_LOOPS * CHUNK_SIZE;
    }
    else
    {
        res = std::abs(lhs(0));
    }

    // deal with left overs
    for (size_t i = REMAINDER_LOOP_START; i < N; i++)
        res = ((std::abs(lhs(i)) > res) ? std::abs(lhs(i)) : res);

    return res;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const CSVector<T>& vector)
{
    size_t N = vector.size();
    os << "[";
    for (size_t i = 0; i < N; i++)
    {
        os << vector[i];
        if (i != (N-1)) os << " ";
    }
    os << "]";

    return os;
}

#endif // CS_VECTOR
